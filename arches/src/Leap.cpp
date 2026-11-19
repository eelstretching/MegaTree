#include "Leap.h"

Leap::Leap(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
           const uint8_t* colors, uint tail, uint speed)
    : Archimation(arches, colorMap, 0),
      colors(colors, colors + nColors),
      tail(tail),
      speed(speed) {}

void Leap::init() {
    arches->clear();
}

bool Leap::step() {
    //
    // The ball runs along every arch in the show. If the show's arches are
    // split between controllers (ArchSet::setShowPlace()), every controller
    // works out the same ball from the same clock and draws just the part of
    // it that's on its own arches.
    uint ppa = arches->getPixelsPerArch();
    uint total = arches->getNumShowArches() * ppa;
    uint first = arches->getFirstShowArch() * ppa;
    uint ours = arches->getNumPixels();
    if (total == 0 || colors.empty()) {
        return true;
    }
    uint head = countSince(0, speed) % total;
    arches->clear();
    for (uint k = 0; k <= tail && k < total; k++) {
        //
        // Wrap the tail back onto the last arch so the loop is seamless.
        uint p = (head + total - k) % total;
        if (p < first || p >= first + ours) {
            continue;
        }
        RGB c = (*colorMap)[colors[(p / ppa) % colors.size()]];
        arches->set(p - first, c.scale8(255 - (k * 255) / (tail + 1)));
    }
    return true;
}
