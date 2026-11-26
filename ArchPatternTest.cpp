//
// Arch pattern test: steps through the arch patterns by hand, with no
// wall-clock schedule, to try out a group of three arches as it's wired.
//
// Wiring is LeapingArches' and ShowArches': the controller in the middle of
// its group, two back-to-back strands per arch on neighbouring outputs, front
// strand first: left arch outputs 1+2, middle 3+4, right 5+6.
//
// Over USB serial (? prints this):
//   n / p      next / previous pattern
//   0-9, a-c   jump to a pattern by number (see the list)
//   space      auto-advance on/off (each pattern runs AUTO_SECONDS)
//   + / -      brighter / dimmer
//   r          restart the current pattern
//   l          list the patterns
//   s          status
//
// It starts on pattern 0, the wiring check, held (auto-advance off).
#include <stdio.h>
#include <string.h>

#include <vector>

#include "ArchAnimator.h"
#include "ArchFire.h"
#include "ArchGlow.h"
#include "ArchRise.h"
#include "ArchSet.h"
#include "ArchWiringCheck.h"
#include "ArrayColorMap.h"
#include "Cylon.h"
#include "FillWipe.h"
#include "Leap.h"
#include "Strip.h"
#include "pico/stdlib.h"

#define PIXELS_PER_ARCH 80
#define BRIGHTNESS 32
#define FPS 40
#define AUTO_SECONDS 20

#ifdef LED_PROCESSOR_BOARD
#define OUTPUT_1_PIN 0
#else
#define OUTPUT_1_PIN 2
#endif

/// @brief The pin for an output on the bank board ribbon, counting from 1.
#define OUTPUT(n) (OUTPUT_1_PIN + (n) - 1)

struct StrandWiring {
    uint pin;
    ArchFeed feed;
};

struct ArchWiring {
    StrandWiring front;
    StrandWiring back;
};

static const ArchWiring archWiring[] = {
    // Left arch: right foot is nearest the box.
    {{OUTPUT(1), ArchFeed::RIGHT_FOOT}, {OUTPUT(2), ArchFeed::RIGHT_FOOT}},
    // Middle arch.
    {{OUTPUT(3), ArchFeed::LEFT_FOOT}, {OUTPUT(4), ArchFeed::LEFT_FOOT}},
    // Right arch.
    {{OUTPUT(5), ArchFeed::LEFT_FOOT}, {OUTPUT(6), ArchFeed::LEFT_FOOT}},
};

static const uint8_t brightnessSteps[] = {4, 8, 16, 32, 48, 64, 96, 128, 192, 255};
#define NUM_BRIGHTNESS (sizeof(brightnessSteps) / sizeof(brightnessSteps[0]))

static const char kPatternKeys[] = "0123456789abc";

static Strip* makeStrip(uint pin) {
    Strip* strip = new Strip(pin, PIXELS_PER_ARCH);
    strip->setColorOrder(ColorOrder::ORGB);
    return strip;
}

/// @brief An ArchAnimator that can be sent to any pattern on demand. The
/// patterns are added untimed; we do the auto-advance timing here.
class TestAnimator : public ArchAnimator {
   public:
    using ArchAnimator::ArchAnimator;

    int size() { return archimations.size(); }
    int current() { return pos; }
    Archimation* at(int i) { return archimations[i]; }

    void goTo(int i) {
        int n = size();
        if (n == 0) {
            return;
        }
        archimations[pos]->finish();
        pos = ((i % n) + n) % n;
        archimationChanged();
        uint64_t now = clockUs();
        startArchimation(archimations[pos], now - now % usPerFrame + usPerFrame);
    }

    void archimationChanged() override {
        printf("\n>>> PATTERN %d/%d: %s\n", pos, size() - 1,
               archimations[pos]->getName());
        changedAt = time_us_64();
    }

    uint64_t changedAt = 0;
};

static void printList(TestAnimator& animator) {
    printf("Patterns:\n");
    for (int i = 0; i < animator.size(); i++) {
        printf("  %c  %s%s\n", kPatternKeys[i], animator.at(i)->getName(),
               i == animator.current() ? "   <-- now" : "");
    }
}

static void printHelp(TestAnimator& animator) {
    printf(
        "\n=== ArchPatternTest: 3 arches, 2 strands each, outputs 1-6 ===\n"
        "  n / p      next / previous pattern\n"
        "  0-9, a-c   jump to a pattern\n"
        "  space      auto-advance on/off (%d s each)\n"
        "  + / -      brighter / dimmer\n"
        "  r          restart this pattern\n"
        "  l          list patterns\n"
        "  s          status\n"
        "  ?          this help\n",
        AUTO_SECONDS);
    printList(animator);
}

static void printStatus(TestAnimator& animator, ArchSet& arches, bool autoAdvance) {
    printf(">>> PATTERN %d/%d: %s | brightness %d | auto %s\n",
           animator.current(), animator.size() - 1,
           animator.at(animator.current())->getName(), arches.getBrightness(),
           autoAdvance ? "ON" : "off");
    animator.printStats();
}

int main() {
    stdio_init_all();

    ArchSet arches(PIXELS_PER_ARCH, BRIGHTNESS);
    for (const ArchWiring& w : archWiring) {
        arches.add(makeStrip(w.front.pin), w.front.feed, makeStrip(w.back.pin),
                   w.back.feed);
    }
    arches.setup();
    arches.clear();
    arches.show();

    ArrayColorMap xmasColors({
        RGB::Red,
        RGB::Green,
        RGB::Blue,
        RGB::White,
        RGB(213, 181, 52),   // Gold
        RGB(120, 190, 255),  // Ice blue
    });
    uint8_t rgColors[] = {0, 1};
    uint8_t rwColors[] = {0, 3};
    uint8_t iceColors[] = {3, 5};
    uint8_t rgbwgColors[] = {0, 1, 2, 3, 4};

    TestAnimator animator(&arches, FPS);

    ArchWiringCheck check(&arches);
    check.setName("WiringCheck");
    animator.add(&check);

    // The six show scenes, as ShowArches sets them up.
    ArchFire fire(&arches);
    fire.setName("Fire (Hearth)");
    animator.add(&fire);

    ArchGlow warmGlow(&arches, {RGB(255, 170, 70)});
    warmGlow.setName("WarmGlow (Classic)");
    animator.add(&warmGlow);

    FillWipe iceWipe(&arches, &xmasColors, 2, iceColors, WIPE_MIDDLE, 4, 2000,
                     20);
    iceWipe.setName("Icicles");
    animator.add(&iceWipe);

    Leap leap(&arches, &xmasColors, 2, rgColors, 12, 60);
    leap.setName("Leap");
    animator.add(&leap);

    ArchRise candyCane(&arches, &xmasColors, 2, rwColors, false, 2000, 25);
    candyCane.setName("CandyCane");
    animator.add(&candyCane);

    ArchGlow redGreen(&arches, {RGB::Red, RGB::Green});
    redGreen.setName("RedGreen (Greetings)");
    animator.add(&redGreen);

    // A few more from LeapingArches.
    Leap fastLeap(&arches, &xmasColors, 5, rgbwgColors, 20, 120);
    fastLeap.setName("FastLeap");
    animator.add(&fastLeap);

    ArchRise riseTogether(&arches, &xmasColors, 2, rgColors, true, 1500);
    riseTogether.setName("RiseTogether");
    animator.add(&riseTogether);

    FillWipe wipeLeft(&arches, &xmasColors, 5, rgbwgColors, WIPE_LEFT);
    wipeLeft.setName("WipeLeft");
    animator.add(&wipeLeft);

    FillWipe wipeRight(&arches, &xmasColors, 2, rgColors, WIPE_RIGHT, 8);
    wipeRight.setName("WipeRight");
    animator.add(&wipeRight);

    Cylon cylon(&arches);
    cylon.setName("Cylon");
    animator.add(&cylon);

    Cylon cylons(&arches, RGB::Red, 4, 60, 160, true);
    cylons.setName("Cylons");
    animator.add(&cylons);

    bool autoAdvance = false;
    int brightIdx = 3;  // 32
    arches.setBrightness(brightnessSteps[brightIdx]);

    animator.init();
    printHelp(animator);
    printStatus(animator, arches, autoAdvance);

    while (true) {
        animator.step();

        if (autoAdvance &&
            time_us_64() - animator.changedAt > AUTO_SECONDS * 1000000ull) {
            animator.goTo(animator.current() + 1);
        }

        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) {
            continue;
        }
        switch (c) {
            case 'n':
            case 'N':
                animator.goTo(animator.current() + 1);
                break;
            case 'p':
            case 'P':
                animator.goTo(animator.current() - 1);
                break;
            case 'r':
            case 'R':
                animator.goTo(animator.current());
                break;
            case ' ':
                autoAdvance = !autoAdvance;
                animator.changedAt = time_us_64();
                printf(">>> AUTO-ADVANCE %s\n", autoAdvance ? "ON" : "OFF");
                break;
            case '+':
            case '=':
                if (brightIdx < (int)NUM_BRIGHTNESS - 1) brightIdx++;
                arches.setBrightness(brightnessSteps[brightIdx]);
                printf(">>> BRIGHTNESS %d\n", brightnessSteps[brightIdx]);
                break;
            case '-':
            case '_':
                if (brightIdx > 0) brightIdx--;
                arches.setBrightness(brightnessSteps[brightIdx]);
                printf(">>> BRIGHTNESS %d\n", brightnessSteps[brightIdx]);
                break;
            case 'l':
            case 'L':
                printList(animator);
                break;
            case 's':
            case 'S':
                printStatus(animator, arches, autoAdvance);
                break;
            case '?':
            case 'h':
                printHelp(animator);
                break;
            default: {
                const char* k = strchr(kPatternKeys, c);
                if (c != 0 && k != nullptr && k - kPatternKeys < animator.size()) {
                    animator.goTo(k - kPatternKeys);
                }
                break;
            }
        }
    }
}
