#include "Shockwave.h"

#include <stdlib.h>

#include "colorutils.h"

Shockwave::Shockwave(Topper* topper, ColorMap* colorMap, uint8_t nColors,
                     const uint8_t* colors, uint8_t width, uint8_t speed,
                     uint8_t keep, uint gap)
    : TopperAnimation(topper, colorMap),
      colors(colors, colors + nColors),
      width(width > 0 ? width : 1),
      speed(speed > 0 ? speed : 1),
      keep(keep),
      gap(gap) {}

void Shockwave::init() {
    innermost = 255;
    for (uint p = 0; p < topper->getNumPixels(); p++) {
        innermost = MIN(innermost, topper->getPixel(p).radius);
    }
    front = innermost - width;
    wait = 0;
    color = 0;
    topper->clear();
}

bool Shockwave::step() {
    topper->fade(keep);
    if (wait > 0) {
        wait--;
        return true;
    }

    RGB c = colors.empty() ? RGB::White
                           : colorMap->getColor(colors[color % colors.size()]);
    for (uint p = 0; p < topper->getNumPixels(); p++) {
        int d = abs((int)topper->getPixel(p).radius - front);
        if (d < width) {
            //
            // Brightest in the middle of the wave, dimmer towards its edges.
            // Keep whichever is brighter, the wave or the glow it left.
            RGB w = c.scale8(255 - d * 255 / width);
            RGB old = topper->get((int)p);
            topper->set((int)p, RGB(MAX(old.r, w.r), MAX(old.g, w.g),
                                    MAX(old.b, w.b)));
        }
    }

    front += speed;
    if (front > 255 + width) {
        front = innermost - width;
        wait = gap;
        color++;
    }
    return true;
}
