//
// The MegaTree's part of the Christmas show, with the star topper.
//
// Every prop runs the same schedule (show/include/ShowSchedule.h): a 20
// minute loop of scenes that starts on the hour, :20 and :40, from 4:30 to
// 10:30 pm Eastern, fading through black between scenes. Each prop works
// out the scene from the time of day, so they change together without
// talking to each other. Core 1 keeps the time: the DS3231, corrected from an
// internet time server (NTP) over WiFi every few hours. See show/README.md.
//
// WiFi details go in show/wifi_secrets.cmake (not in git), see the README.
//
// Over USB serial: S status, A run all the time (daytime testing), G n go to
// scene n, N back to normal, T NTP now.
#include <stdio.h>

#include <vector>

#include "ArrayColorMap.h"
#include "Canvas.h"
#include "ClassicLights.h"
#include "FontTwoP.h"
#include "Icicles.h"
#include "Marquees.h"
#include "Pinwheel.h"
#include "PointChase.h"
#include "RingChase.h"
#include "ColorWipe.h"
#include "ScrollTexts.h"
#include "ShowDirector.h"
#include "ShowTime.h"
#include "ShowTreeAnimator.h"
#include "SimpleFont.h"
#include "Spiral.h"
#include "Starlight.h"
#include "Strip.h"
#include "Topper.h"
#include "TopperStarlight.h"
#include "pico/stdlib.h"

using namespace show;

// The LED processor board's outputs are GPIO0-31, the whole tree.
#define NUM_STRIPS 32
#define START_PIN 0
#define STRIP_LEN 200
#define CANVAS_WIDTH 100
#define BRIGHTNESS 32
#define FPS 40

// The topper is on its own output: GPIO35, on J2 (pin 2) on the v1
// processor board, through a spare output board. Set SHOW_TOPPER to 0 to run
// the tree alone.
#ifndef SHOW_TOPPER
#define SHOW_TOPPER 1
#endif
#define TOPPER_PIN 35
#define TOPPER_BRIGHTNESS 32

int main() {
    stdio_init_all();
    //
    // Give the laptop a moment to open the USB port.
    sleep_ms(2000);
    printf("ShowTree: %d strips on GP%d-GP%d, %d pixels each%s\n", NUM_STRIPS,
           START_PIN, START_PIN + NUM_STRIPS - 1, STRIP_LEN,
           SHOW_TOPPER ? ", topper on GP35" : "");

    //
    // The radio first: it has to claim its PIO state machine before the
    // LEDs claim theirs.
    TimeService::start();

    Canvas canvas(CANVAS_WIDTH);
    for (int i = 0; i < NUM_STRIPS; i++) {
        Strip* s = new Strip(START_PIN + i, STRIP_LEN);
        s->setColorOrder(ColorOrder::ORGB);
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
    uint8_t rColors[] = {0};
    uint8_t gColors[] = {1};
    uint8_t rwColors[] = {0, 3};
    uint8_t rgwColors[] = {0, 1, 3};
    uint8_t goldColors[] = {4};

    ShowDirector director;
    ShowTreeAnimator tree(&canvas, director, BRIGHTNESS, FPS);

    //
    // Hearth: the calm gold glow with warm white glints.
    Starlight starlight(&canvas);
    tree.set(HEARTH, &starlight);

    //
    // Classic: old-fashioned multicolor bulbs on slo-glo.
    ClassicLights classic(&canvas, ClassicLights::Multicolor,
                          ClassicMode::SLO_GLO);
    tree.set(CLASSIC, &classic);

    //
    // Icicles: white icicles dripping down the tree.
    ArrayColorMap icicleMap(8);
    Icicles icicles(&canvas, &icicleMap, 10, 6, RGB(128, 128, 128));
    tree.set(ICICLES, &icicles);

    //
    // Leap: red and green spirals winding up and down the tree while the
    // ball leaps along the arches.
    Spiral spiralRed(&canvas, &xmasColors, 20, 0, 1, rColors, 10, 25);
    spiralRed.setDirection(Direction::UP);
    Spiral spiralGreen(&canvas, &xmasColors, 0, canvas.getHeight() - 1, 1,
                       gColors, 10, 25);
    spiralGreen.setDirection(Direction::DOWN);
    MultiAnimation spirals(&canvas, &xmasColors);
    spirals.add(&spiralRed);
    spirals.add(&spiralGreen);
    tree.set(LEAP, &spirals);

    //
    // Candy cane: a slow red and white marquee.
    Marquees candyCane(&canvas, &xmasColors, 2, rwColors, 25, RIGHT,
                       canvas.getHeight());
    candyCane.setFPS(15);
    tree.set(CANDY_CANE, &candyCane);

    //
    // Greetings.
    SimpleFont font(FontTwoPData);
    ScrollTexts greetings(&canvas);
    ScrollText st1(&canvas, &font, "MERRY CHRISTMAS", 90, -30, RGB::Red);
    ScrollText st2(&canvas, &font, "HAPPY HOLIDAYS", 75, -45, RGB::Green);
    ScrollText st3(&canvas, &font, "JOYEUX NOEL", 55, 0, RGB::White);
    ScrollText st4(&canvas, &font, "CHAG SAMEACH", 40, -10, RGB(0, 80, 255));
    ScrollText st5(&canvas, &font, "FELIZ NAVIDAD", 25, -30, RGB::Red);
    ScrollText st6(&canvas, &font, "SEASON'S GREETINGS", 10, -20,
                   RGB(213, 181, 52));
    greetings.add(&st1);
    greetings.add(&st2);
    greetings.add(&st3);
    greetings.add(&st4);
    greetings.add(&st5);
    greetings.add(&st6);
    tree.set(GREETINGS, &greetings);

#if SHOW_TOPPER
    //
    // HolidayCoro's 24" 3 row star: rings of 20, 30 and 40 nodes, innermost
    // first, all on one output.
    Strip topperStrip(TOPPER_PIN, 90);
    topperStrip.setColorOrder(ColorOrder::ORGB);
    Topper topper(&topperStrip, {20, 30, 40}, 0, TOPPER_BRIGHTNESS);
    topper.setup();
    topper.clear();
    topper.show();
    ShowTopper star(&topper, TOPPER_BRIGHTNESS);

    TopperStarlight topperStarlight(&topper);
    star.set(HEARTH, &topperStarlight);
    // A slow chase round the rings: once round every 8 seconds.
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
