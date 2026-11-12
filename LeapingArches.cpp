#include <stdlib.h>

#include "ArchAnimator.h"
#include "ArchFire.h"
#include "ArchRise.h"
#include "ArchSet.h"
#include "ArchWiringCheck.h"
#include "ArrayColorMap.h"
#include "Cylon.h"
#include "FillWipe.h"
#include "Leap.h"
#include "Strip.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

// A group of three arches in a row, with the controller in the middle. Each
// arch has two strands of pixels mounted back to back, both showing the same
// thing, and each strand has its own output, so a group takes six outputs on
// one output board. Each arch's two strands are on neighbouring outputs, the
// front strand first: the left arch is outputs 1 and 2, the middle arch 3 and
// 4, and the right arch 5 and 6.
// Outputs 1-6 on the bank board ribbon are GP2-GP7 (GP0-GP5 on the LED
// processor board, whose LED outputs start at GPIO 0).
//
// The arches are listed in show order, left to right as seen from the street.
// Each strand says which foot its data lead comes in at: with the controller
// in the middle, the left arch is fed at its right foot. Run with
// ARCH_WIRING_CHECK set to 1 to check the order and directions in the yard
// (see ArchWiringCheck.h); a back strand wired the wrong way round shows up
// as a dot running against the front strand's.
#define PIXELS_PER_ARCH 80
#define BRIGHTNESS 32
#define FPS 40
#define ARCH_WIRING_CHECK 0

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

#define NUM_ARCHES (sizeof(archWiring) / sizeof(archWiring[0]))

static Strip* makeStrip(uint pin) {
    Strip* strip = new Strip(pin, PIXELS_PER_ARCH);
    strip->setColorOrder(ColorOrder::ORGB);
    return strip;
}

int main() {
    stdio_init_all();

    ArchSet arches(PIXELS_PER_ARCH, BRIGHTNESS);
    for (uint i = 0; i < NUM_ARCHES; i++) {
        const ArchWiring& w = archWiring[i];
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
        RGB(213, 181, 52),  // Gold
        RGB::Purple,
    });
    uint8_t rgColors[] = {0, 1};
    uint8_t rgbwgColors[] = {0, 1, 2, 3, 4};

    ArchAnimator animator(&arches, FPS);

#if ARCH_WIRING_CHECK
    ArchWiringCheck check(&arches);
    check.setName("WiringCheck");
    animator.add(&check);
#else
    animator.setShuffle(true);

    Leap leap(&arches, &xmasColors, 2, rgColors, 12, 60);
    leap.setName("Leap");
    animator.addTimed(&leap, 20000);

    Leap fastLeap(&arches, &xmasColors, 5, rgbwgColors, 20, 120);
    fastLeap.setName("FastLeap");
    animator.addTimed(&fastLeap, 15000);

    ArchRise rise(&arches, &xmasColors, 5, rgbwgColors);
    rise.setName("Rise");
    animator.addTimed(&rise, 20000);

    ArchRise riseTogether(&arches, &xmasColors, 2, rgColors, true, 1500);
    riseTogether.setName("RiseTogether");
    animator.addTimed(&riseTogether, 15000);

    FillWipe wipeLeft(&arches, &xmasColors, 5, rgbwgColors, WIPE_LEFT);
    wipeLeft.setName("WipeLeft");
    animator.addTimed(&wipeLeft, 20000);

    FillWipe wipeRight(&arches, &xmasColors, 2, rgColors, WIPE_RIGHT, 8);
    wipeRight.setName("WipeRight");
    animator.addTimed(&wipeRight, 20000);

    FillWipe wipeMiddle(&arches, &xmasColors, 5, rgbwgColors, WIPE_MIDDLE, 1);
    wipeMiddle.setName("WipeMiddle");
    animator.addTimed(&wipeMiddle, 20000);

    Cylon cylon(&arches);
    cylon.setName("Cylon");
    animator.addTimed(&cylon, 20000);

    Cylon cylons(&arches, RGB::Red, 4, 60, 160, true);
    cylons.setName("Cylons");
    animator.addTimed(&cylons, 20000);

    ArchFire fire(&arches);
    fire.setName("Fire");
    animator.addTimed(&fire, 60000);
#endif

    animator.init();

    while (true) {
        animator.step();
        if (animator.getFrameCount() % 300 == 0) {
            animator.printStats();
        }
    }
}
