#include "FillWipe.h"

FillWipe::FillWipe(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
                   const uint8_t* colors, WipeFrom wipeFrom,
                   uint pixelsPerColor, uint holdFrames)
    : Archimation(arches, colorMap),
      colors(colors, colors + nColors),
      wipeFrom(wipeFrom),
      pixelsPerColor(pixelsPerColor > 0 ? pixelsPerColor : 1),
      holdFrames(holdFrames) {}

uint FillWipe::wipeSteps() {
    uint ppa = arches->getPixelsPerArch();
    return wipeFrom == WIPE_MIDDLE ? (ppa + 1) / 2 : ppa;
}

void FillWipe::init() {
    phase = FILLING;
    count = 0;
    arches->clear();
}

bool FillWipe::step() {
    uint n = arches->getNumArches();
    if (n == 0 || colors.empty()) {
        return true;
    }

    switch (phase) {
        case FILLING: {
            RGB c = (*colorMap)[colors[(count / pixelsPerColor) %
                                       colors.size()]];
            for (uint i = 0; i < n; i++) {
                arches->getArch(i)->setMirrored(count, c);
            }
            if (++count >= (arches->getPixelsPerArch() + 1) / 2) {
                phase = HOLDING_FULL;
                count = 0;
            }
            break;
        }
        case HOLDING_FULL:
            if (++count >= holdFrames) {
                phase = WIPING;
                count = 0;
            }
            break;
        case WIPING:
            for (uint i = 0; i < n; i++) {
                Arch* a = arches->getArch(i);
                switch (wipeFrom) {
                    case WIPE_LEFT:
                        a->shift(RIGHT, RGB::Black);
                        break;
                    case WIPE_RIGHT:
                        a->shift(LEFT, RGB::Black);
                        break;
                    case WIPE_MIDDLE:
                        a->shiftFromApex(RGB::Black);
                        break;
                }
            }
            if (++count >= wipeSteps()) {
                phase = HOLDING_CLEAR;
                count = 0;
            }
            break;
        case HOLDING_CLEAR:
            if (++count >= holdFrames) {
                phase = FILLING;
                count = 0;
            }
            break;
    }
    return true;
}
