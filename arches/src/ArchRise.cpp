#include "ArchRise.h"

ArchRise::ArchRise(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
                   const uint8_t* colors, bool together, uint holdFrames)
    : Archimation(arches, colorMap),
      colors(colors, colors + nColors),
      together(together),
      holdFrames(holdFrames) {}

RGB ArchRise::colorFor(uint archIndex) {
    return (*colorMap)[colors[(colorOffset + archIndex) % colors.size()]];
}

void ArchRise::init() {
    arch = 0;
    level = 0;
    hold = 0;
    colorOffset = 0;
    arches->clear();
}

bool ArchRise::step() {
    uint n = arches->getNumArches();
    if (n == 0 || colors.empty()) {
        return true;
    }

    if (hold > 0) {
        if (--hold == 0) {
            arches->clear();
            arch = 0;
            level = 0;
            colorOffset = (colorOffset + 1) % colors.size();
        }
        return true;
    }

    //
    // Each leg runs from a foot to the apex. With an odd number of pixels the
    // apex pixel belongs to both legs.
    uint legLength = (arches->getPixelsPerArch() + 1) / 2;
    if (together) {
        for (uint i = 0; i < n; i++) {
            arches->getArch(i)->setMirrored(level, colorFor(i));
        }
        if (++level >= legLength) {
            hold = holdFrames + 1;
        }
    } else {
        arches->getArch(arch)->setMirrored(level, colorFor(arch));
        if (++level >= legLength) {
            level = 0;
            if (++arch >= n) {
                hold = holdFrames + 1;
            }
        }
    }
    return true;
}
