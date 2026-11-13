//
// A mini tree's part of the Christmas show. Both mini trees run this same
// firmware.
//
// A mini tree is 13 strands of 100 pixels, each folded over the hub at the
// top, so every strand makes two of the 26 columns: 50 pixels up, then 50
// back down (docs/mini-trees.md). That's exactly how a 50 pixel wide canvas
// lays out a 100 pixel strip (one row forwards, the next backwards), so the
// canvas is 50 x 26 with x running up each column from the bottom, the same
// way round as the MegaTree's.
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
#include "RotatingColumns.h"
#include "ShowDirector.h"
#include "ShowTime.h"
#include "ShowTreeAnimator.h"
#include "Strip.h"
#include "TwinkleFox.h"
#include "pico/stdlib.h"

using namespace show;

#define NUM_STRANDS 13
#define STRAND_LEN 100
// Each strand is two columns of 50.
#define COLUMN_LEN 50
// The mini trees run on LED processor boards: the strands are GP0-GP12
// (outputs 1-8 on the first shroud, 1-5 on the second). On a Pico 2 W they'd
// be GP2-GP14, staying off UART0 on GP0/1.
#ifndef START_PIN
#ifdef LED_PROCESSOR_BOARD
#define START_PIN 0
#else
#define START_PIN 2
#endif
#endif
#define BRIGHTNESS 32
#define FPS 40

// Last year's Wally's Lights bullet pixels. If red shows as green, try
// ColorOrder::OGRB.
#ifndef MINI_TREE_COLOR_ORDER
#define MINI_TREE_COLOR_ORDER ColorOrder::ORGB
#endif

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("ShowMiniTree: %d strands on GP%d-GP%d, %d pixels each\n",
           NUM_STRANDS, START_PIN, START_PIN + NUM_STRANDS - 1, STRAND_LEN);

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

    director.printHelp();
    while (true) {
        tree.step();
        if (tree.getFrameCount() % 2400 == 0) {
            tree.printStats();
        }
    }
}
