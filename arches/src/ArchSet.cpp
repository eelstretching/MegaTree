#include "ArchSet.h"

#include <stdio.h>

ArchSet::ArchSet(uint pixelsPerArch, uint8_t brightness)
    : pixelsPerArch(pixelsPerArch), renderer(brightness) {}

ArchSet::~ArchSet() {
    for (auto a : arches) {
        delete a;
    }
}

uint ArchSet::add(Strip* strip, ArchFeed feed) {
    strips.push_back(strip);
    renderer.add(strip);
    uint n = strip->getNumPixels() / pixelsPerArch;
    for (uint i = 0; i < n; i++) {
        //
        // Fed from the right, the leftmost arch is the last one on the wire.
        uint segment = feed == ArchFeed::RIGHT_FOOT ? n - 1 - i : i;
        Arch* arch = new Arch(strip, segment * pixelsPerArch, pixelsPerArch);
        arch->setFeed(feed);
        arches.push_back(arch);
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
