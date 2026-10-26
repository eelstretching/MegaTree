#include "ArchWiringCheck.h"

ArchWiringCheck::ArchWiringCheck(ArchSet* arches, uint speed)
    : Archimation(arches, nullptr, 0), speed(speed > 0 ? speed : 1) {}

void ArchWiringCheck::init() { arches->clear(); }

bool ArchWiringCheck::step() {
    static const RGB archColors[] = {RGB(48, 0, 0), RGB(0, 48, 0),
                                     RGB(0, 0, 48)};
    static const RGB dotColors[] = {RGB::Red, RGB::Green, RGB::Blue};
    uint ppa = arches->getPixelsPerArch();
    if (ppa == 0) {
        return true;
    }
    int dot = countSince(0, speed) % ppa;
    for (uint i = 0; i < arches->getNumArches(); i++) {
        Arch* arch = arches->getArch(i);
        arch->fill(archColors[i % 3]);
        arch->fill(RGB::White, 0, 3);
        arch->set(dot, dotColors[i % 3]);
    }
    return true;
}
