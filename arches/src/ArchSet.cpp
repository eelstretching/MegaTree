#include "ArchSet.h"

#include <stdio.h>

ArchSet::ArchSet(uint pixelsPerArch, uint8_t brightness)
    : pixelsPerArch(pixelsPerArch), renderer(brightness) {}

ArchSet::~ArchSet() {
    for (auto a : arches) {
        delete a;
    }
}

uint ArchSet::add(Strip* strip) {
    strips.push_back(strip);
    renderer.add(strip);
    uint n = strip->getNumPixels() / pixelsPerArch;
    for (uint i = 0; i < n; i++) {
        arches.push_back(new Arch(strip, i * pixelsPerArch, pixelsPerArch));
    }
    if (n * pixelsPerArch != strip->getNumPixels()) {
        printf("Strip on pin %d has %d pixels left over after %d arches\n",
               strip->getPin(), strip->getNumPixels() - n * pixelsPerArch, n);
    }
    return n;
}

void ArchSet::set(int p, const RGB& color) {
    if (p < 0 || (uint)p >= getNumPixels()) {
        return;
    }
    arches[p / pixelsPerArch]->set(p % pixelsPerArch, color);
}

const RGB& ArchSet::get(int p) {
    if (p < 0 || (uint)p >= getNumPixels()) {
        return stripBlack;
    }
    return arches[p / pixelsPerArch]->get(p % pixelsPerArch);
}

void ArchSet::fill(const RGB& color) {
    for (auto a : arches) {
        a->fill(color);
    }
}
