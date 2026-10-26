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

// A group of three arches in a row, with the controller in the middle and one
// output per arch, so the data leads are about the same length. Outputs 1-3
// on the bank board ribbon are GP2-GP4.
//
// The arches are listed in show order, left to right as seen from the street.
// Each says which foot its data lead comes in at: with the controller in the
// middle, the left arch is fed at its right foot. Run with ARCH_WIRING_CHECK
// set to 1 to check the order and directions in the yard (see
// ArchWiringCheck.h).
#define PIXELS_PER_ARCH 80
#define BRIGHTNESS 32
#define FPS 40
#define ARCH_WIRING_CHECK 0

struct ArchWiring {
    uint pin;
    ArchFeed feed;
};

static const ArchWiring archWiring[] = {
    {2, ArchFeed::RIGHT_FOOT},  // Left arch: right foot is nearest the box.
    {3, ArchFeed::LEFT_FOOT},   // Middle arch.
    {4, ArchFeed::LEFT_FOOT},   // Right arch.
};

#define NUM_ARCHES (sizeof(archWiring) / sizeof(archWiring[0]))

int main() {
    stdio_init_all();

    ArchSet arches(PIXELS_PER_ARCH, BRIGHTNESS);
    Strip* strips[NUM_ARCHES];
    for (uint i = 0; i < NUM_ARCHES; i++) {
        strips[i] = new Strip(archWiring[i].pin, PIXELS_PER_ARCH);
        strips[i]->setColorOrder(ColorOrder::ORGB);
        arches.add(strips[i], archWiring[i].feed);
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
