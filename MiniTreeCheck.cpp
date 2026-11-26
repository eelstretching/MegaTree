//
// Mini tree wiring check: lights the 2026 mini tree's 28 columns one way or
// another so you can see each strand's zig-zag is laid the way ShowMiniTree
// expects. No clock, no radio, no schedule.
//
// Expected layout (docs/mini-trees.md): 7 strands of 200 on outputs 1-7, each
// a zig-zag of four 50-pixel columns, pixel 1 at the bottom: column 1 up,
// column 2 down, column 3 up, column 4 down. Each strand's first column sits
// next to the previous strand's last, all round the tree the same way. Tree
// column c (0-27) is strand c / 4, run c % 4.
//
// Over USB serial (? prints this):
//   w          walk: a comet climbs column 0, then 1, ... 27, then again.
//              On a good tree it always goes UP, and steps sideways one
//              column at a time round the tree.
//   c          one column at a time, red at the bottom fading to blue at the
//              top (n / p to step)
//   s          one strand at a time, its runs red, green, blue, white
//              (n / p to step)
//   a          all strands, each its own color, bottom pixel of every column
//              white
//   n / p      next / previous column or strand
//   + / -      brighter / dimmer
//   i          status
//   ?          this help
//
// It starts in walk mode at brightness 16.
#include <stdio.h>

#include "Canvas.h"
#include "Strip.h"
#include "pico/stdlib.h"

#define NUM_STRANDS 7
#define STRAND_LEN 200
#define COLUMN_LEN 50
#define RUNS_PER_STRAND (STRAND_LEN / COLUMN_LEN)
#define NUM_COLUMNS (NUM_STRANDS * RUNS_PER_STRAND)
#ifndef START_PIN
#ifdef LED_PROCESSOR_BOARD
#define START_PIN 0
#else
#define START_PIN 2
#endif
#endif
#ifndef MINI_TREE_COLOR_ORDER
#define MINI_TREE_COLOR_ORDER ColorOrder::ORGB
#endif
#define FPS 40
#define COMET_LEN 6

enum class CheckMode { WALK, COLUMN, STRAND, ALL };

static const uint8_t brightnessSteps[] = {4, 8, 16, 32, 48, 64, 96, 128};
#define NUM_BRIGHTNESS (sizeof(brightnessSteps) / sizeof(brightnessSteps[0]))

static const RGB strandColors[NUM_STRANDS] = {
    RGB::Red,  RGB::Green, RGB::Blue, RGB(255, 140, 0),  // orange
    RGB::Purple, RGB::Cyan, RGB(255, 200, 0),             // yellow
};
static const RGB runColors[RUNS_PER_STRAND] = {RGB::Red, RGB::Green,
                                               RGB::Blue, RGB::White};
static const char* runNames[RUNS_PER_STRAND] = {"up", "down", "up", "down"};

static const char* modeName(CheckMode m) {
    switch (m) {
        case CheckMode::WALK:
            return "walk";
        case CheckMode::COLUMN:
            return "column";
        case CheckMode::STRAND:
            return "strand";
        case CheckMode::ALL:
            return "all";
    }
    return "?";
}

static void printHelp() {
    printf(
        "\n=== MiniTreeCheck: %d strands x %d px on GP%d-GP%d, %d columns of "
        "%d ===\n"
        "  w      walk: comet climbs each column in turn (always UP if wired "
        "right)\n"
        "  c      one column, red bottom -> blue top\n"
        "  s      one strand, runs red/green/blue/white\n"
        "  a      all strands, own colors, bottom pixels white\n"
        "  n / p  next / previous column or strand\n"
        "  + / -  brighter / dimmer\n"
        "  i      status\n"
        "  ?      this help\n"
        "Expect: pixel 1 at the bottom, runs up/down/up/down, each strand\n"
        "starting next to where the last one ended.\n",
        NUM_STRANDS, STRAND_LEN, START_PIN, START_PIN + NUM_STRANDS - 1,
        NUM_COLUMNS, COLUMN_LEN);
}

static void printColumn(int c) {
    int strand = c / RUNS_PER_STRAND;
    int run = c % RUNS_PER_STRAND;
    printf(">>> COLUMN %2d/%d = output %d (GP%d) run %d, goes %s, pixels %d-%d\n",
           c, NUM_COLUMNS - 1, strand + 1, START_PIN + strand, run + 1,
           runNames[run], run * COLUMN_LEN + 1, (run + 1) * COLUMN_LEN);
}

static void printStrand(int s) {
    printf(">>> STRAND %d/%d = output %d (GP%d), columns %d-%d: red up, green "
           "down, blue up, white down\n",
           s + 1, NUM_STRANDS, s + 1, START_PIN + s, s * RUNS_PER_STRAND,
           s * RUNS_PER_STRAND + RUNS_PER_STRAND - 1);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    Canvas canvas(COLUMN_LEN);
    for (int i = 0; i < NUM_STRANDS; i++) {
        Strip* s = new Strip(START_PIN + i, STRAND_LEN);
        s->setColorOrder(MINI_TREE_COLOR_ORDER);
        canvas.add(s);
    }
    int bIndex = 2;
    canvas.setBrightness(brightnessSteps[bIndex]);
    canvas.setup();
    canvas.clear();
    canvas.show();

    CheckMode mode = CheckMode::WALK;
    int column = 0;
    int strand = 0;
    // Walk position: column * COLUMN_LEN + height of the comet's head.
    int walk = 0;
    int lastWalkColumn = -1;
    bool dirty = true;

    printHelp();

    absolute_time_t next = make_timeout_time_ms(1000 / FPS);
    while (true) {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT) {
            switch (ch) {
                case 'w':
                    mode = CheckMode::WALK;
                    walk = 0;
                    lastWalkColumn = -1;
                    printf(">>> MODE walk\n");
                    break;
                case 'c':
                    mode = CheckMode::COLUMN;
                    printColumn(column);
                    break;
                case 's':
                    mode = CheckMode::STRAND;
                    printStrand(strand);
                    break;
                case 'a':
                    mode = CheckMode::ALL;
                    printf(">>> MODE all: outputs 1-7 red, green, blue, "
                           "orange, purple, cyan, yellow\n");
                    break;
                case 'n':
                case 'p': {
                    int d = ch == 'n' ? 1 : -1;
                    if (mode == CheckMode::STRAND) {
                        strand = (strand + d + NUM_STRANDS) % NUM_STRANDS;
                        printStrand(strand);
                    } else {
                        if (mode != CheckMode::COLUMN) {
                            mode = CheckMode::COLUMN;
                        }
                        column = (column + d + NUM_COLUMNS) % NUM_COLUMNS;
                        printColumn(column);
                    }
                    break;
                }
                case '+':
                case '=':
                    if (bIndex < (int)NUM_BRIGHTNESS - 1) bIndex++;
                    canvas.setBrightness(brightnessSteps[bIndex]);
                    printf(">>> brightness %d\n", brightnessSteps[bIndex]);
                    break;
                case '-':
                    if (bIndex > 0) bIndex--;
                    canvas.setBrightness(brightnessSteps[bIndex]);
                    printf(">>> brightness %d\n", brightnessSteps[bIndex]);
                    break;
                case 'i':
                    printf(">>> mode %s | column %d | strand %d | brightness %d\n",
                           modeName(mode), column, strand + 1,
                           brightnessSteps[bIndex]);
                    break;
                case '?':
                case 'h':
                    printHelp();
                    break;
                default:
                    break;
            }
            dirty = true;
        }

        if (mode == CheckMode::WALK) {
            int c = walk / COLUMN_LEN;
            int head = walk % COLUMN_LEN;
            if (c != lastWalkColumn) {
                printColumn(c);
                lastWalkColumn = c;
            }
            canvas.clear();
            for (int k = 0; k < COMET_LEN; k++) {
                int x = head - k;
                if (x < 0) break;
                uint8_t v = 255 - k * (255 / COMET_LEN);
                RGB col = strandColors[c / RUNS_PER_STRAND];
                canvas.set(x, c,
                           RGB(col.r * v / 255, col.g * v / 255, col.b * v / 255));
            }
            walk = (walk + 1) % (NUM_COLUMNS * COLUMN_LEN);
            canvas.show();
        } else if (dirty) {
            canvas.clear();
            if (mode == CheckMode::COLUMN) {
                for (int x = 0; x < COLUMN_LEN; x++) {
                    int up = x * 255 / (COLUMN_LEN - 1);
                    canvas.set(x, column, RGB(255 - up, 0, up));
                }
            } else if (mode == CheckMode::STRAND) {
                for (int r = 0; r < RUNS_PER_STRAND; r++) {
                    int y = strand * RUNS_PER_STRAND + r;
                    for (int x = 0; x < COLUMN_LEN; x++) {
                        canvas.set(x, y, runColors[r]);
                    }
                }
            } else {
                for (int y = 0; y < NUM_COLUMNS; y++) {
                    for (int x = 0; x < COLUMN_LEN; x++) {
                        canvas.set(x, y,
                                   x == 0 ? RGB::White
                                          : strandColors[y / RUNS_PER_STRAND]);
                    }
                }
            }
            canvas.show();
            dirty = false;
        }

        sleep_until(next);
        next = delayed_by_ms(next, 1000 / FPS);
    }
}
