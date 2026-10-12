#include "RingChase.h"

RingChase::RingChase(Topper* topper, ColorMap* colorMap, uint8_t nColors,
                     const uint8_t* colors, uint16_t speed, uint8_t keep,
                     bool alternate)
    : TopperAnimation(topper, colorMap),
      colors(colors, colors + nColors),
      speed(speed),
      keep(keep),
      alternate(alternate) {}

void RingChase::init() {
    phase = 0;
    topper->clear();
}

bool RingChase::step() {
    topper->fade(keep);
    for (uint ring = 0; ring < topper->getNumRings(); ring++) {
        int n = topper->getRingSize(ring);
        RGB c = colors.empty()
                    ? RGB::White
                    : colorMap->getColor(colors[ring % colors.size()]);
        //
        // The same fraction of a turn is a different number of pixels on
        // each ring, so work out where the head is on this one.
        int head = ((uint32_t)phase * n) >> 16;
        if (alternate && ring % 2 == 1) {
            head = -head;
        }
        topper->set(ring, head, c);
    }
    phase += speed;
    return true;
}
