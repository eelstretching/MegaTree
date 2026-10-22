#include "ArchRise.h"

ArchRise::ArchRise(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
                   const uint8_t* colors, bool together, uint holdMS,
                   uint speed)
    : Archimation(arches, colorMap, 0),
      colors(colors, colors + nColors),
      together(together),
      holdUs((uint64_t)holdMS * 1000),
      speed(speed) {}

RGB ArchRise::colorFor(uint archIndex) {
    return (*colorMap)[colors[(colorOffset + archIndex) % colors.size()]];
}

void ArchRise::init() {
    lit = 0;
    holding = false;
    phaseStart = 0;
    colorOffset = 0;
    arches->clear();
}

uint ArchRise::riseSteps() {
    //
    // Each leg runs from a foot to the apex. With an odd number of pixels the
    // apex pixel belongs to both legs.
    uint legLength = (arches->getPixelsPerArch() + 1) / 2;
    return together ? legLength : legLength * arches->getNumArches();
}

void ArchRise::light(uint i) {
    if (together) {
        for (uint a = 0; a < arches->getNumArches(); a++) {
            arches->getArch(a)->setMirrored(i, colorFor(a));
        }
        return;
    }
    uint legLength = (arches->getPixelsPerArch() + 1) / 2;
    uint a = i / legLength;
    arches->getArch(a)->setMirrored(i % legLength, colorFor(a));
}

bool ArchRise::step() {
    if (arches->getNumArches() == 0 || colors.empty()) {
        return true;
    }

    //
    // Catch up with the clock, going round as many phases as it takes.
    uint total = riseSteps();
    while (true) {
        if (holding) {
            if (elapsedUs() - phaseStart < holdUs) {
                break;
            }
            holding = false;
            phaseStart += holdUs;
            arches->clear();
            lit = 0;
            colorOffset = (colorOffset + 1) % colors.size();
            continue;
        }
        uint64_t target = countSince(phaseStart, speed);
        while (lit < total && lit < target) {
            light(lit++);
        }
        if (lit < total) {
            break;
        }
        holding = true;
        phaseStart += usFor(total, speed);
    }
    return true;
}
