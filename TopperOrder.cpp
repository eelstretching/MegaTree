//
// Lights the star topper's pixels one at a time, in the order they're wired,
// so you can check every bullet pixel went into the right hole.
//
// What you see (fill mode, the default):
//   - Pixels light up in wire order and stay lit, colored by ring: inner
//     ring (20) red, middle ring (30) green, outer ring (40) blue. A wrong
//     color in a ring means a pixel from another ring is in it.
//   - The first pixel of each ring is white.
//   - The five tips of each ring are gold, spaced a fifth of the ring apart
//     from that ring's tip offset (tipOffsets below). 't' moves the current
//     ring's gold marks along one pixel, to find the offset.
//   - The newest pixel blinks white and off, so you can always find where it's got
//     to, even paused.
// Dot mode lights only the newest pixel (blinking), for when the fill gets
// too busy to follow.
//
// Keys (no Enter needed) in tio:
//   space     pause / run
//   n or .    step forward one pixel (and pause)
//   b or ,    step back one pixel (and pause)
//   + / -     faster / slower
//   1 2 3     jump to the start of that ring (and pause)
//   g <num>   then Enter: jump to pixel <num> (1-90) and pause
//   a         all on: the whole star, ring colors, white starts, gold tips
//   m         fill mode / dot mode
//   t         move the current ring's gold tip marks along one pixel
//   r         restart from the first pixel
//   [ / ]     dimmer / brighter
//   ?         show the keys
//
// USB stdio is on, so `picotool load -f -x` works while it runs.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "Renderer.h"
#include "Strip.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

#ifndef TOPPER_PIN
#ifdef LED_PROCESSOR_BOARD
// On J2, through the spare output board.
#define TOPPER_PIN 35
#else
#define TOPPER_PIN 11
#endif
#endif
#define FPS 30

// HolidayCoro's 24" 3 row star, innermost ring first on the wire.
static const int ringSizes[] = {20, 30, 40};
static const int numRings = sizeof(ringSizes) / sizeof(ringSizes[0]);
static const char* ringNames[] = {"INNER", "MIDDLE", "OUTER"};
static const char* ringColorNames[] = {"red", "green", "blue"};
static const RGB ringColors[] = {RGB::Red, RGB::Green, RGB::Blue};
static int numPixels = 0;

// Steps per second, slowest first.
static const float speeds[] = {0.5f, 1, 2, 4, 8, 16, 30};
static const int numSpeeds = sizeof(speeds) / sizeof(speeds[0]);
static int speed = 2;

static const uint8_t brightnesses[] = {8, 16, 32, 64, 128, 255};
static const int numBrightnesses = sizeof(brightnesses);
static int brightness = 2;

// Which pixel is newest (0-based, in wire order).
static int head = 0;
static bool paused = false;
static bool dotMode = false;
static bool allOn = false;

static Strip* strip;
static Renderer* renderer;

static int ringOf(int p, int* posInRing) {
    for (int r = 0; r < numRings; r++) {
        if (p < ringSizes[r]) {
            *posInRing = p;
            return r;
        }
        p -= ringSizes[r];
    }
    *posInRing = 0;
    return numRings - 1;
}

static int ringStart(int ring) {
    int p = 0;
    for (int r = 0; r < ring; r++) p += ringSizes[r];
    return p;
}

// Where each ring's first tip is on the wire (0-based within the ring); the
// other four follow every ring size / 5 pixels. Steve found the inner ring's
// tips at pixels 3, 7, 11, 15, 19 and the middle ring's at 4, 10, 16, 22, 28
// (1-based). Change one live with 't'.
static int tipOffsets[] = {2, 3, 0};

static bool isTip(int ring, int pos) {
    int spacing = ringSizes[ring] / 5;
    return pos % spacing == tipOffsets[ring] % spacing;
}

static RGB colorOf(int p) {
    int pos;
    int ring = ringOf(p, &pos);
    if (isTip(ring, pos)) return RGB::Gold;
    if (pos == 0) return RGB::White;
    return ringColors[ring];
}

static void printHelp() {
    printf("\nkeys:\n");
    printf("  space   pause / run\n");
    printf("  n .     step forward      b ,   step back\n");
    printf("  + -     faster / slower   [ ]   dimmer / brighter\n");
    printf("  1 2 3   jump to the start of a ring\n");
    printf("  g<num>  + Enter: jump to pixel <num> (1-%d)\n", numPixels);
    printf("  a       all on            m     fill / dot mode\n");
    printf("  r       restart           ?     this list\n");
    printf("  t       shift this ring's gold tips along one pixel\n");
    printf("colors: inner red, middle green, outer blue,\n");
    printf("        ring start white, the 5 tips gold\n");
}

static void printPixel() {
    int pos;
    int ring = ringOf(head, &pos);
    if (pos == 0) {
        printf("\n===== %s RING (%d px, %s) starts =====", ringNames[ring],
               ringSizes[ring], ringColorNames[ring]);
    }
    printf("\n>>> PIXEL %2d of %d   %s ring #%d%s%s", head + 1, numPixels,
           ringNames[ring], pos + 1,
           isTip(ring, pos) ? "  (tip, gold)"
                            : (pos == 0 ? "  (start, white)" : ""),
           paused ? "   [paused]" : "");
}

static void printSettings() {
    printf("\n--- %s, %s, %.1f px/s, brightness %d", paused ? "paused" : "running",
           dotMode ? "dot mode" : "fill mode", speeds[speed],
           brightnesses[brightness]);
}

static void draw(bool blinkOn) {
    strip->fill(RGB::Black);
    if (allOn) {
        for (int p = 0; p < numPixels; p++) strip->putPixel(colorOf(p), p);
        return;
    }
    if (!dotMode) {
        for (int p = 0; p < head; p++) strip->putPixel(colorOf(p), p);
    }
    strip->putPixel(blinkOn ? RGB::White : RGB::Black, head);
}

static void moveTo(int p) {
    allOn = false;
    head = (p % numPixels + numPixels) % numPixels;
    printPixel();
}

// Collecting the digits after a 'g'.
static bool gotoMode = false;
static char gotoBuf[8];
static int gotoLen = 0;

static void handleKey(int c) {
    if (gotoMode) {
        if (c == '\r' || c == '\n') {
            gotoMode = false;
            gotoBuf[gotoLen] = 0;
            int v = atoi(gotoBuf);
            if (v < 1 || v > numPixels) {
                printf("\n!!! pixel must be 1-%d", numPixels);
            } else {
                paused = true;
                moveTo(v - 1);
            }
        } else if ((c == '\b' || c == 127) && gotoLen > 0) {
            gotoLen--;
            printf("\b \b");
        } else if (isdigit(c) && gotoLen < (int)sizeof(gotoBuf) - 1) {
            gotoBuf[gotoLen++] = (char)c;
            putchar(c);
        } else if (c == 27) {
            gotoMode = false;
            printf(" (cancelled)");
        }
        return;
    }
    switch (c) {
        case ' ':
            paused = !paused;
            printSettings();
            break;
        case 'n':
        case '.':
            paused = true;
            moveTo(head + 1);
            break;
        case 'b':
        case ',':
            paused = true;
            moveTo(head - 1);
            break;
        case '+':
        case '=':
            if (speed < numSpeeds - 1) speed++;
            printSettings();
            break;
        case '-':
        case '_':
            if (speed > 0) speed--;
            printSettings();
            break;
        case '[':
            if (brightness > 0) brightness--;
            renderer->setBrightness(brightnesses[brightness]);
            printSettings();
            break;
        case ']':
            if (brightness < numBrightnesses - 1) brightness++;
            renderer->setBrightness(brightnesses[brightness]);
            printSettings();
            break;
        case 'a':
            allOn = !allOn;
            printf(allOn ? "\n>>> ALL ON" : "\n>>> back to pixel %d",
                   head + 1);
            break;
        case 'm':
            dotMode = !dotMode;
            printSettings();
            break;
        case 't': {
            int pos;
            int ring = ringOf(head, &pos);
            int spacing = ringSizes[ring] / 5;
            tipOffsets[ring] = (tipOffsets[ring] + 1) % spacing;
            printf("\n>>> %s ring gold tips now at", ringNames[ring]);
            for (int i = 0; i < 5; i++) {
                printf(" %d", tipOffsets[ring] + i * spacing + 1);
            }
            break;
        }
        case 'r':
            paused = false;
            moveTo(0);
            break;
        case 'g':
            gotoMode = true;
            gotoLen = 0;
            printf("\ngo to pixel: ");
            break;
        case '?':
        case 'h':
            printHelp();
            printSettings();
            break;
        default:
            if (c >= '1' && c < '1' + numRings) {
                paused = true;
                moveTo(ringStart(c - '1'));
            }
            break;
    }
}

int main() {
    stdio_init_all();
    for (int i = 0; i < 30 && !stdio_usb_connected(); i++) {
        sleep_ms(100);
    }
    for (int r = 0; r < numRings; r++) numPixels += ringSizes[r];

    strip = new Strip(TOPPER_PIN, numPixels);
    strip->setColorOrder(ColorOrder::ORGB);
    renderer = new Renderer(brightnesses[brightness]);
    renderer->add(strip);
    renderer->setup();

    printf("\nTopperOrder: %d star pixels on GP%d, in wire order\n", numPixels,
           TOPPER_PIN);
    printHelp();
    printSettings();
    printPixel();

    const uint32_t frameUs = 1000000 / FPS;
    uint32_t frame = 0;
    absolute_time_t nextStep =
        make_timeout_time_ms((uint32_t)(1000 / speeds[speed]));
    absolute_time_t nextFrame = get_absolute_time();
    while (true) {
        int c;
        while ((c = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            handleKey(c);
            nextStep = make_timeout_time_ms((uint32_t)(1000 / speeds[speed]));
        }

        if (!paused && !allOn && !gotoMode &&
            absolute_time_diff_us(get_absolute_time(), nextStep) <= 0) {
            if (head == numPixels - 1) {
                // Show the finished star for a few seconds, then go again.
                allOn = true;
                printf("\n>>> ALL %d DONE, starting over in 5 s", numPixels);
                draw(false);
                renderer->render();
                sleep_ms(5000);
                moveTo(0);
                nextFrame = get_absolute_time();
            } else {
                moveTo(head + 1);
            }
            nextStep = make_timeout_time_ms((uint32_t)(1000 / speeds[speed]));
        }

        // Blink the newest pixel at about 3 Hz.
        draw((frame / 5) % 2 == 0);
        renderer->render();
        frame++;
        nextFrame = delayed_by_us(nextFrame, frameUs);
        sleep_until(nextFrame);
    }
}
