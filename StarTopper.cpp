#include <stdlib.h>

#include "ArrayColorMap.h"
#include "ColorWipe.h"
#include "Pinwheel.h"
#include "PointChase.h"
#include "RadiatingRainbow.h"
#include "RingChase.h"
#include "Shockwave.h"
#include "Starlight.h"
#include "Strip.h"
#include "Topper.h"
#include "TopperAnimator.h"
#include "WiringCheck.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

// The star topper on an output of its own. Pin 11 is the first output after
// the ten the mega-tree uses and the one the leaping arches use.
#define TOPPER_PIN 11
#define BRIGHTNESS 32
#define FPS 30

// Set to 1 to run only the WiringCheck, to find each ring's start and
// direction for the setRingStart()/setRingReversed() calls below.
#define WIRING_CHECK 0

int main() {
    stdio_init_all();

    //
    // HolidayCoro's 24" 3 row star: rings of 20, 30 and 40 nodes, innermost
    // first, all on one output.
    Strip strip(TOPPER_PIN, 90);
    strip.setColorOrder(ColorOrder::ORGB);

    Topper topper(&strip, {20, 30, 40}, 0, BRIGHTNESS);
    //
    // Where each ring's wire starts, counting clockwise from the top tip as
    // seen from the front, and whether it runs anticlockwise. Until it's on
    // the tree we're guessing that every ring starts at the top tip and runs
    // clockwise, as HolidayCoro's instructions say.
    // topper.setRingStart(0, 0);
    // topper.setRingReversed(0, false);
    topper.setup();
    topper.clear();
    topper.show();

    ArrayColorMap xmasColors({
        RGB::Red,
        RGB::Green,
        RGB::Blue,
        RGB::White,
        RGB(213, 181, 52),  // Gold
        RGB::Purple,
    });
    uint8_t rgColors[] = {0, 1};
    uint8_t rwColors[] = {0, 3};
    uint8_t rgwColors[] = {0, 1, 3};
    uint8_t rgbwgColors[] = {0, 1, 2, 3, 4};

    TopperAnimator animator(&topper, FPS);

#if WIRING_CHECK
    WiringCheck check(&topper);
    check.setName("WiringCheck");
    animator.add(&check);
#else
    animator.setShuffle(true);

    RadiatingRainbow rainbow(&topper);
    rainbow.setName("Rainbow");
    animator.addTimed(&rainbow, 20000);

    RadiatingRainbow rainbowIn(&topper, 96, 3, true);
    rainbowIn.setName("RainbowIn");
    animator.addTimed(&rainbowIn, 15000);

    Pinwheel rainbowSpiral(&topper, nullptr, 0, nullptr, 1, 128, 3);
    rainbowSpiral.setName("RainbowSpiral");
    animator.addTimed(&rainbowSpiral, 20000);

    Pinwheel peppermint(&topper, &xmasColors, 2, rwColors, 10, 96, -2);
    peppermint.setName("Peppermint");
    animator.addTimed(&peppermint, 20000);

    Pinwheel pinwheel(&topper, &xmasColors, 5, rgbwgColors, 5, 0, 2);
    pinwheel.setName("Pinwheel");
    animator.addTimed(&pinwheel, 15000);

    Shockwave shockwave(&topper, &xmasColors, 5, rgbwgColors);
    shockwave.setName("Shockwave");
    animator.addTimed(&shockwave, 20000);

    RingChase ringChase(&topper, &xmasColors, 3, rgwColors);
    ringChase.setName("RingChase");
    animator.addTimed(&ringChase, 20000);

    Starlight starlight(&topper);
    starlight.setName("Starlight");
    animator.addTimed(&starlight, 30000);

    PointChase pointChase(&topper, &xmasColors, 5, rgbwgColors);
    pointChase.setName("PointChase");
    animator.addTimed(&pointChase, 15000);

    ColorWipe wipe(&topper, &xmasColors, 2, rgColors);
    wipe.setName("ColorWipe");
    animator.addTimed(&wipe, 20000);
#endif

    animator.init();

    while (true) {
        animator.step();
        if (animator.getFrameCount() % 300 == 0) {
            animator.printStats();
        }
    }
}
