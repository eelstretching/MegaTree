#include "Pinwheel.h"

Pinwheel::Pinwheel(Topper* topper, ColorMap* colorMap, uint8_t nColors,
                   const uint8_t* colors, uint8_t blades, int twist,
                   int8_t speed)
    : TopperAnimation(topper, colorMap),
      colors(colors, colors + nColors),
      blades(blades > 0 ? blades : 1),
      twist(twist),
      speed(speed) {}

void Pinwheel::init() { rotation = 0; }

bool Pinwheel::step() {
    for (uint p = 0; p < topper->getNumPixels(); p++) {
        const StarPixel& px = topper->getPixel(p);
        //
        // Where the pixel is round the pinwheel, out of 256 of a turn, with
        // the outer pixels pushed round further when there's a twist.
        uint8_t a = px.angle - rotation + (uint8_t)(px.radius * twist / 256);
        RGB c;
        if (colors.empty() || colorMap == nullptr) {
            c = RGB(HSV(a * blades, 255, 255));
        } else {
            uint blade = (a * blades) >> 8;
            c = colorMap->getColor(colors[blade % colors.size()]);
        }
        topper->set(p, c);
    }
    rotation += speed;
    return true;
}
