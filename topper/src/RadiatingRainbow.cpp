#include "RadiatingRainbow.h"

RadiatingRainbow::RadiatingRainbow(Topper* topper, uint spread, uint8_t speed,
                                   bool inward)
    : TopperAnimation(topper, nullptr),
      spread(spread),
      speed(speed),
      inward(inward) {}

void RadiatingRainbow::init() { hue = 0; }

bool RadiatingRainbow::step() {
    for (uint p = 0; p < topper->getNumPixels(); p++) {
        uint8_t r = topper->getPixel(p).radius;
        //
        // The hue a pixel shows now is the one the centre showed a little
        // while ago, so the colors travel outward.
        uint8_t h = hue - (uint8_t)((r * spread) >> 8);
        topper->set(p, RGB(HSV(h, 255, 255)));
    }
    hue += inward ? -speed : speed;
    return true;
}
