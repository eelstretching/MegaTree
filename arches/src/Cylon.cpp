#include "Cylon.h"

Cylon::Cylon(ArchSet* arches, const RGB& color, uint eyeWidth, uint speed,
             uint8_t fade, bool alternate)
    : Archimation(arches, nullptr),
      color(color),
      eyeWidth(eyeWidth > 0 ? eyeWidth : 1),
      speed(speed > 0 ? speed : 1),
      fade(fade),
      alternate(alternate) {}

void Cylon::init() {
    pos = 0;
    dir = 1;
    arches->clear();
}

bool Cylon::step() {
    int ppa = arches->getPixelsPerArch();
    int last = ppa - (int)eyeWidth;
    if (last < 0) {
        last = 0;
    }

    for (uint i = 0; i < arches->getNumArches(); i++) {
        Arch* a = arches->getArch(i);
        //
        // Fade the trail, then draw the eye where it is now.
        for (int p = 0; p < ppa; p++) {
            a->set(p, a->get(p).scale8(fade));
        }
        int start = alternate && (i % 2 == 1) ? last - pos : pos;
        a->fill(color, start, eyeWidth);
    }

    //
    // Bounce off the feet.
    pos += dir * (int)speed;
    if (pos >= last) {
        pos = last;
        dir = -1;
    } else if (pos <= 0) {
        pos = 0;
        dir = 1;
    }
    return true;
}
