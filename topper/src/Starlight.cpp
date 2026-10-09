#include "Starlight.h"

#include "colorutils.h"
#include "math8.h"

Starlight::Starlight(Topper* topper, const RGB& glow, const RGB& glint,
                     uint8_t rate, uint8_t keep)
    : TopperAnimation(topper, nullptr),
      glow(glow),
      glint(glint),
      rate(rate),
      keep(keep) {}

void Starlight::init() {
    glints.assign(topper->getNumPixels(), 0);
    breath = 0;
}

bool Starlight::step() {
    uint n = topper->getNumPixels();
    if (glints.size() != n) {
        glints.assign(n, 0);
    }
    if (random8() < rate) {
        glints[random16(n)] = 255;
    }

    //
    // Breathe between half and full brightness, about every four seconds at
    // 30 fps.
    RGB base = glow.scale8(128 + (sin8(breath) >> 1));
    breath += 2;

    for (uint p = 0; p < n; p++) {
        topper->set((int)p, blend(base, glint, glints[p]));
        glints[p] = scale8(glints[p], keep);
    }
    return true;
}
