#include "Leap.h"

Leap::Leap(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
           const uint8_t* colors, uint tail, uint speed)
    : Archimation(arches, colorMap),
      colors(colors, colors + nColors),
      tail(tail),
      speed(speed) {}

void Leap::init() {
    head = 0;
    arches->clear();
}

bool Leap::step() {
    uint total = arches->getNumPixels();
    uint ppa = arches->getPixelsPerArch();
    if (total == 0 || colors.empty()) {
        return true;
    }
    arches->clear();
    for (uint k = 0; k <= tail && k < total; k++) {
        //
        // Wrap the tail back onto the last arch so the loop is seamless.
        uint p = (head + total - k) % total;
        RGB c = (*colorMap)[colors[(p / ppa) % colors.size()]];
        arches->set(p, c.scale8(255 - (k * 255) / (tail + 1)));
    }
    head = (head + speed) % total;
    return true;
}
