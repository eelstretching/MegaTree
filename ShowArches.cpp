//
// A group of three leaping arches' part of the Christmas show.
//
// The show has six arches in a row, left to right from the street, run by
// two controllers: group 1 is arches 1-3 and group 2 arches 4-6. The build
// makes ShowArches1 and ShowArches2 (SHOW_ARCH_GROUP). Each knows where its
// arches are in the row, so the leaping ball runs from arch 1 to arch 6
// across both controllers, the two working it out from the same clock.
//
// Every prop runs the same schedule (show/include/ShowSchedule.h); see
// ShowTree.cpp and show/README.md.
//
// Wiring is LeapingArches': the controller in the middle of its group, two
// back-to-back strands per arch on neighbouring outputs, front strand first:
// left arch outputs 1+2, middle 3+4, right 5+6. Run LeapingArches with
// ARCH_WIRING_CHECK set to check a group in the yard first.
//
// Over USB serial: S status, A run all the time (daytime testing), G n go to
// scene n, N back to normal, T NTP now.
#include <stdio.h>

#include "ArchFire.h"
#include "ArchGlow.h"
#include "ArchRise.h"
#include "ArchSet.h"
#include "ArrayColorMap.h"
#include "FillWipe.h"
#include "Leap.h"
#include "ShowArchAnimator.h"
#include "ShowDirector.h"
#include "ShowTime.h"
#include "Strip.h"
#include "pico/stdlib.h"

using namespace show;

#ifndef SHOW_ARCH_GROUP
#define SHOW_ARCH_GROUP 1
#endif
#define ARCHES_PER_GROUP 3
#define SHOW_ARCHES 6

#define PIXELS_PER_ARCH 80
#define BRIGHTNESS 32
#define FPS 40

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

static const ArchWiring archWiring[ARCHES_PER_GROUP] = {
    // Left arch: right foot is nearest the box.
    {{OUTPUT(1), ArchFeed::RIGHT_FOOT}, {OUTPUT(2), ArchFeed::RIGHT_FOOT}},
    // Middle arch.
    {{OUTPUT(3), ArchFeed::LEFT_FOOT}, {OUTPUT(4), ArchFeed::LEFT_FOOT}},
    // Right arch.
    {{OUTPUT(5), ArchFeed::LEFT_FOOT}, {OUTPUT(6), ArchFeed::LEFT_FOOT}},
};

static Strip* makeStrip(uint pin) {
    Strip* strip = new Strip(pin, PIXELS_PER_ARCH);
    strip->setColorOrder(ColorOrder::ORGB);
    return strip;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    uint first = (SHOW_ARCH_GROUP - 1) * ARCHES_PER_GROUP;
    printf("ShowArches: group %d, arches %d-%d of %d\n", SHOW_ARCH_GROUP,
           first + 1, first + ARCHES_PER_GROUP, SHOW_ARCHES);

    //
    // The radio first: it has to claim its PIO state machine before the
    // LEDs claim theirs.
    TimeService::start();

    ArchSet arches(PIXELS_PER_ARCH, BRIGHTNESS);
    for (const ArchWiring& w : archWiring) {
        arches.add(makeStrip(w.front.pin), w.front.feed, makeStrip(w.back.pin),
                   w.back.feed);
    }
    arches.setShowPlace(first, SHOW_ARCHES);
    arches.setup();
    arches.clear();
    arches.show();

    ArrayColorMap xmasColors({
        RGB::Red,
        RGB::Green,
        RGB::Blue,
        RGB::White,
        RGB(213, 181, 52),  // Gold
        RGB(120, 190, 255),  // Ice blue
    });
    uint8_t rgColors[] = {0, 1};
    uint8_t rwColors[] = {0, 3};
    uint8_t iceColors[] = {3, 5};

    ShowDirector director;
    ShowArchAnimator animator(&arches, director, BRIGHTNESS, FPS);

    //
    // Hearth: a cozy fire in every arch.
    ArchFire fire(&arches);
    animator.set(HEARTH, &fire);

    //
    // Classic: a warm white glow, breathing slowly along the row.
    ArchGlow warmGlow(&arches, {RGB(255, 170, 70)});
    animator.set(CLASSIC, &warmGlow);

    //
    // Icicles: white and ice blue fill up from the feet, then clear from the
    // apex.
    FillWipe iceWipe(&arches, &xmasColors, 2, iceColors, WIPE_MIDDLE, 4, 2000,
                     20);
    animator.set(ICICLES, &iceWipe);

    //
    // Leap: the showpiece, a red and green ball leaping from arch 1 to arch
    // 6, across both controllers.
    Leap leap(&arches, &xmasColors, 2, rgColors, 12, 60);
    animator.set(LEAP, &leap);

    //
    // Candy cane: red and white rising from the feet, one arch after
    // another.
    ArchRise rise(&arches, &xmasColors, 2, rwColors, false, 2000, 25);
    animator.set(CANDY_CANE, &rise);

    //
    // Greetings: red and green arches, glowing.
    ArchGlow redGreen(&arches, {RGB::Red, RGB::Green});
    animator.set(GREETINGS, &redGreen);

    director.printHelp();
    while (true) {
        animator.step();
        if (animator.getFrameCount() % 2400 == 0) {
            animator.printStats();
        }
    }
}
