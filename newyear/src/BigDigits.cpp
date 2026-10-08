#include "BigDigits.h"

#include <math.h>
#include <string.h>

//
// A classic 5x7 font for the digits. Each byte is a row, top first, with the
// leftmost pixel in bit 4.
static const uint8_t glyphs[10][7] = {
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},  // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},  // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},  // 2
    {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},  // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},  // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},  // 5
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},  // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},  // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},  // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},  // 9
};

#define GLYPH_W 5
#define GLYPH_H 7
#define ADVANCE 6

//
// How many samples per pixel, across and up, when working out coverage.
#define SAMPLES 3

int BigDigits::unitWidth(const char *text) {
    int n = strlen(text);
    return n == 0 ? 0 : n * ADVANCE - 1;
}

/// @brief Whether the point (u, v) in font units, measured from the top left
/// of the text, is inside a lit pixel of one of the glyphs.
static bool lit(const char *text, int n, float u, float v) {
    if (u < 0 || v < 0 || v >= GLYPH_H) {
        return false;
    }
    int ci = (int)(u / ADVANCE);
    if (ci >= n) {
        return false;
    }
    int col = (int)u - ci * ADVANCE;
    if (col >= GLYPH_W) {
        return false;
    }
    char c = text[ci];
    if (c < '0' || c > '9') {
        return false;
    }
    return glyphs[c - '0'][(int)v] & (0x10 >> col);
}

void BigDigits::draw(Canvas *canvas, const char *text, float cx, float cy,
                     float height, const RGB &color, uint8_t alpha) {
    int n = strlen(text);
    if (n == 0 || height <= 0 || alpha == 0) {
        return;
    }
    float unit = height / GLYPH_H;
    float width = unitWidth(text) * unit;
    float left = cx - width / 2;
    float top = cy + height / 2;

    int x0 = MAX(0, (int)floorf(left));
    int x1 = MIN((int)canvas->getWidth() - 1, (int)ceilf(left + width));
    int y0 = MAX(0, (int)floorf(top - height));
    int y1 = MIN((int)canvas->getHeight() - 1, (int)ceilf(top));

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            int hits = 0;
            for (int i = 0; i < SAMPLES; i++) {
                for (int j = 0; j < SAMPLES; j++) {
                    float sx = x + (i + 0.5f) / SAMPLES;
                    float sy = y + (j + 0.5f) / SAMPLES;
                    if (lit(text, n, (sx - left) / unit, (top - sy) / unit)) {
                        hits++;
                    }
                }
            }
            if (hits == 0) {
                continue;
            }
            uint8_t amount = (hits * alpha) / (SAMPLES * SAMPLES);
            RGB under = canvas->get(x, y);
            canvas->set(x, y, under.lerp8(color, amount));
        }
    }
}
