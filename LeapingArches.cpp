#include <stdlib.h>

#include "ArchAnimator.h"
#include "ArchRise.h"
#include "ArchSet.h"
#include "ArrayColorMap.h"
#include "Cylon.h"
#include "FillWipe.h"
#include "Leap.h"
#include "Strip.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

// One output driving a few arches wired end to end. Pin 10 is the first
// output after the ten the mega-tree uses.
#define ARCH_PIN 10
#define PIXELS_PER_ARCH 80
#define ARCHES_PER_STRIP 3
#define BRIGHTNESS 32
#define FPS 30

int main() {
    stdio_init_all();

    Strip strip(ARCH_PIN, PIXELS_PER_ARCH * ARCHES_PER_STRIP);
    strip.setColorOrder(ColorOrder::ORGB);

    ArchSet arches(PIXELS_PER_ARCH, BRIGHTNESS);
    arches.add(&strip);
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
    animator.setShuffle(true);

    Leap leap(&arches, &xmasColors, 2, rgColors, 12, 2);
    leap.setName("Leap");
    animator.addTimed(&leap, 20000);

    Leap fastLeap(&arches, &xmasColors, 5, rgbwgColors, 20, 4);
    fastLeap.setName("FastLeap");
    animator.addTimed(&fastLeap, 15000);

    ArchRise rise(&arches, &xmasColors, 5, rgbwgColors);
    rise.setName("Rise");
    animator.addTimed(&rise, 20000);

    ArchRise riseTogether(&arches, &xmasColors, 2, rgColors, true, 45);
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

    Cylon cylons(&arches, RGB::Red, 4, 2, 160, true);
    cylons.setName("Cylons");
    animator.addTimed(&cylons, 20000);

    animator.init();

    while (true) {
        animator.step();
        if (animator.getFrameCount() % 300 == 0) {
            animator.printStats();
        }
    }
}
