#include "PointChase.h"

PointChase::PointChase(Topper* topper, ColorMap* colorMap, uint8_t nColors,
                       const uint8_t* colors, uint framesPerPoint,
                       uint8_t keep)
    : TopperAnimation(topper, colorMap),
      colors(colors, colors + nColors),
      framesPerPoint(framesPerPoint > 0 ? framesPerPoint : 1),
      keep(keep) {}

void PointChase::init() {
    frame = 0;
    point = 0;
    color = 0;
    topper->clear();
}

bool PointChase::step() {
    topper->fade(keep);
    RGB c = colors.empty() ? RGB::White
                           : colorMap->getColor(colors[color % colors.size()]);
    for (uint p = 0; p < topper->getNumPixels(); p++) {
        if (topper->getPixel(p).point == point) {
            topper->set((int)p, c);
        }
    }
    if (++frame >= framesPerPoint) {
        frame = 0;
        if (++point >= 5) {
            point = 0;
            color++;
        }
    }
    return true;
}
