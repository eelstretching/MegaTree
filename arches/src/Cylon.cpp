#include "Cylon.h"

#include <math.h>

Cylon::Cylon(ArchSet* arches, const RGB& color, uint eyeWidth, uint speed,
             uint8_t fade, bool alternate)
    : Archimation(arches, nullptr, 0),
      color(color),
      eyeWidth(eyeWidth > 0 ? eyeWidth : 1),
      speed(speed > 0 ? speed : 1),
      fade(fade),
      alternate(alternate) {}

void Cylon::init() { arches->clear(); }

bool Cylon::step() {
    int ppa = arches->getPixelsPerArch();
    int last = ppa - (int)eyeWidth;
    if (last < 0) {
        last = 0;
    }

    //
    // Where the eye is: it's gone countSince(0, speed) pixels, bouncing off
    // the feet.
    int pos = 0;
    if (last > 0) {
        int d = countSince(0, speed) % (2 * last);
        pos = d <= last ? d : 2 * last - d;
    }

    //
    // fade is what survives a 30th of a second, so work out what survives
    // this frame.
    uint8_t keep = 255;
    if (getFrameUs() > 0) {
        keep = (uint8_t)(255.0f * powf(fade / 255.0f, getFrameUs() * 30 / 1e6f) +
                         0.5f);
    }

    for (uint i = 0; i < arches->getNumArches(); i++) {
        Arch* a = arches->getArch(i);
        //
        // Fade the trail, then draw the eye where it is now.
        if (keep < 255) {
            for (int p = 0; p < ppa; p++) {
                a->set(p, a->get(p).scale8(keep));
            }
        }
        int start = alternate && (i % 2 == 1) ? last - pos : pos;
        a->fill(color, start, eyeWidth);
    }
    return true;
}
