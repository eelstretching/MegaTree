//
// The mini tree's part of the Christmas show. There's one mini tree in 2026.
//
// It's 7 strands of 200 pixels (last year's 100-pixel strands joined in
// pairs), each laid as a zig-zag of four 50-pixel columns: pixel 1 at the
// bottom, up the first column, over the hub, down the second, up the third,
// down the fourth, so both ends of every strand land at the bottom by the
// controller (docs/mini-trees.md). That's exactly how a 50 pixel wide canvas
// lays out a 200 pixel strip (rows alternately forwards and backwards), so
// the canvas is 50 x 28 with x running up each column from the bottom, the
// same way round as the MegaTree's. Each strand's first column sits next to
// the previous strand's last one, all the way round the tree in the same
// direction. MiniTreeCheck lights the columns one at a time to check that.
//
// Every prop runs the same schedule (show/include/ShowSchedule.h); see
// ShowTree.cpp and show/README.md. The processor board's DS3231 keeps the
// time, corrected by NTP over WiFi.
//
// Over USB serial: S status, A run all the time (daytime testing), G n go to
// scene n, N back to normal, T NTP now.
#include <stdio.h>

#include "ArrayColorMap.h"
#include "Canvas.h"
#include "ClassicLights.h"
#include "Icicles.h"
#include "Marquees.h"
#include "Pinwheel.h"
#include "PointChase.h"
#include "RingChase.h"
#include "ColorWipe.h"
#include "RotatingColumns.h"
#include "ShowDirector.h"
#include "ShowTime.h"
#include "ShowTreeAnimator.h"
#include "Strip.h"
#include "Topper.h"
#include "TopperStarlight.h"
#include "TwinkleFox.h"
#include "pico/stdlib.h"

using namespace show;

#define NUM_STRANDS 7
#define STRAND_LEN 200
// Each strand is four columns of 50.
#define COLUMN_LEN 50
// The mini tree runs on an LED processor board: the strands are GP0-GP6,
// outputs 1-7 on the first shroud (one bank board). On a Pico 2 W they'd be
// GP2-GP8, staying off UART0 on GP0/1.
#ifndef START_PIN
#ifdef LED_PROCESSOR_BOARD
#define START_PIN 0
#else
#define START_PIN 2
#endif
#endif
#define BRIGHTNESS 32
#define FPS 40

// The mini tree's topper: HolidayCoro's 19" 2 row star, 50 bullet pixels in
// rings of 20 and 30 (inner first), on output 8, right after the strands.
// Set SHOW_TOPPER to 0 to run the tree alone.
#ifndef SHOW_TOPPER
#define SHOW_TOPPER 1
#endif
#define TOPPER_PIN (START_PIN + NUM_STRANDS)
#define TOPPER_BRIGHTNESS 32

// Last year's Wally's Lights bullet pixels. If red shows as green, try
// ColorOrder::OGRB.
#ifndef MINI_TREE_COLOR_ORDER
#define MINI_TREE_COLOR_ORDER ColorOrder::ORGB
#endif

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("ShowMiniTree: %d strands on GP%d-GP%d, %d pixels each%s\n",
           NUM_STRANDS, START_PIN, START_PIN + NUM_STRANDS - 1, STRAND_LEN,
           SHOW_TOPPER ? ", 19\" star on the next pin" : "");

    //
    // The radio first: it has to claim its PIO state machine before the
    // LEDs claim theirs.
    TimeService::start();

    Canvas canvas(COLUMN_LEN);
    for (int i = 0; i < NUM_STRANDS; i++) {
        Strip* s = new Strip(START_PIN + i, STRAND_LEN);
        s->setColorOrder(MINI_TREE_COLOR_ORDER);
        canvas.add(s);
    }
    canvas.setBrightness(BRIGHTNESS);
    canvas.setup();
    canvas.clear();
    canvas.show();

    ArrayColorMap xmasColors({
        RGB::Red,
        RGB::Green,
        RGB::Blue,
        RGB::White,
        RGB(213, 181, 52),  // Gold
    });
    uint8_t rgColors[] = {0, 1};
    uint8_t rwColors[] = {0, 3};
    uint8_t rgwColors[] = {0, 1, 3};
    uint8_t goldColors[] = {4};
    ArrayColorMap redGreen({RGB::Red, RGB::Green});

    ShowDirector director;
    ShowTreeAnimator tree(&canvas, director, BRIGHTNESS, FPS);

    //
    // Hearth: gentle multicolor twinkles.
    TwinkleFox twinkle(&canvas, 3, 5);
    tree.set(HEARTH, &twinkle);

    //
    // Classic: old-fashioned multicolor bulbs, now and then one twinkling.
    ClassicLights classic(&canvas, ClassicLights::Multicolor,
                          ClassicMode::TWINKLE);
    tree.set(CLASSIC, &classic);

    //
    // Icicles, like the MegaTree's.
    ArrayColorMap icicleMap(8);
    Icicles icicles(&canvas, &icicleMap, 5, 6, RGB(128, 128, 128));
    tree.set(ICICLES, &icicles);

    //
    // Leap: red and green columns turning round the tree.
    RotatingColumns columns(&canvas, &xmasColors, 2, rgColors, 2);
    tree.set(LEAP, &columns);

    //
    // Candy cane: a slow red and white marquee.
    Marquees candyCane(&canvas, &xmasColors, 2, rwColors, 12, RIGHT,
                       canvas.getHeight());
    candyCane.setFPS(15);
    tree.set(CANDY_CANE, &candyCane);

    //
    // Greetings: red and green twinkles, while the MegaTree scrolls its
    // messages.
    TwinkleFox redGreenTwinkle(&canvas, 4, 6);
    redGreenTwinkle.addPalette(&redGreen);
    tree.set(GREETINGS, &redGreenTwinkle);

#if SHOW_TOPPER
    //
    // The star, set up like the MegaTree's topper (ShowTree.cpp). Assumed
    // wired like the big star: every ring starts at the bottom valley and
    // runs anticlockwise as seen from the front. Check it with
    // TopperOrderMini.
    Strip topperStrip(TOPPER_PIN, 50);
    topperStrip.setColorOrder(MINI_TREE_COLOR_ORDER);
    Topper topper(&topperStrip, {20, 30}, 0, TOPPER_BRIGHTNESS);
    for (uint ring = 0; ring < topper.getNumRings(); ring++) {
        topper.setRingStart(ring, topper.getRingSize(ring) / 2);
        topper.setRingReversed(ring, true);
    }
    topper.setup();
    topper.clear();
    topper.show();
    ShowTopper star(&topper, TOPPER_BRIGHTNESS);

    TopperStarlight topperStarlight(&topper);
    star.set(HEARTH, &topperStarlight);
    RingChase ringChase(&topper, &xmasColors, 3, rgwColors, 273);
    star.set(CLASSIC, &ringChase);
    uint8_t whiteColors[] = {3};
    ColorWipe whiteWipe(&topper, &xmasColors, 1, whiteColors, 45, 45);
    star.set(ICICLES, &whiteWipe);
    uint8_t pinwheelColors[] = {0, 1, 3, 4};
    Pinwheel pinwheel(&topper, &xmasColors, 4, pinwheelColors, 5, 0, 2);
    star.set(LEAP, &pinwheel);
    Pinwheel peppermint(&topper, &xmasColors, 2, rwColors, 10, 96, -2);
    star.set(CANDY_CANE, &peppermint);
    PointChase goldChase(&topper, &xmasColors, 1, goldColors, 10);
    star.set(GREETINGS, &goldChase);
    tree.setTopper(&star);
#endif

    director.printHelp();
    while (true) {
        tree.step();
        if (tree.getFrameCount() % 2400 == 0) {
            tree.printStats();
        }
    }
}
