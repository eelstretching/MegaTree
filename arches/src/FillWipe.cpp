#include "FillWipe.h"

FillWipe::FillWipe(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
                   const uint8_t* colors, WipeFrom wipeFrom,
                   uint pixelsPerColor, uint holdMS, uint speed)
    : Archimation(arches, colorMap, 0),
      colors(colors, colors + nColors),
      wipeFrom(wipeFrom),
      pixelsPerColor(pixelsPerColor > 0 ? pixelsPerColor : 1),
      holdUs((uint64_t)holdMS * 1000),
      speed(speed) {}

uint FillWipe::wipeSteps() {
    uint ppa = arches->getPixelsPerArch();
    return wipeFrom == WIPE_MIDDLE ? (ppa + 1) / 2 : ppa;
}

uint FillWipe::fillSteps() { return (arches->getPixelsPerArch() + 1) / 2; }

void FillWipe::init() {
    phase = FILLING;
    count = 0;
    phaseStart = 0;
    arches->clear();
}

bool FillWipe::step() {
    uint n = arches->getNumArches();
    if (n == 0 || colors.empty()) {
        return true;
    }

    //
    // Catch up with the clock, going round as many phases as it takes. Each
    // phase starts exactly when the last one should have finished, so we're in
    // the same place whatever the frame rate.
    while (true) {
        switch (phase) {
            case FILLING: {
                uint64_t target = countSince(phaseStart, speed);
                while (count < fillSteps() && count < target) {
                    RGB c = (*colorMap)[colors[(count / pixelsPerColor) %
                                               colors.size()]];
                    for (uint i = 0; i < n; i++) {
                        arches->getArch(i)->setMirrored(count, c);
                    }
                    count++;
                }
                if (count < fillSteps()) {
                    return true;
                }
                phase = HOLDING_FULL;
                phaseStart += usFor(count, speed);
                break;
            }
            case HOLDING_FULL:
                if (elapsedUs() - phaseStart < holdUs) {
                    return true;
                }
                phase = WIPING;
                count = 0;
                phaseStart += holdUs;
                break;
            case WIPING: {
                uint64_t target = countSince(phaseStart, speed);
                while (count < wipeSteps() && count < target) {
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
                    count++;
                }
                if (count < wipeSteps()) {
                    return true;
                }
                phase = HOLDING_CLEAR;
                phaseStart += usFor(count, speed);
                break;
            }
            case HOLDING_CLEAR:
                if (elapsedUs() - phaseStart < holdUs) {
                    return true;
                }
                phase = FILLING;
                count = 0;
                phaseStart += holdUs;
                break;
        }
    }
}
