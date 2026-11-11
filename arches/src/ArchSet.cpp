#include "ArchSet.h"

#include <stdio.h>

#include <algorithm>

ArchSet::ArchSet(uint pixelsPerArch, uint8_t brightness)
    : pixelsPerArch(pixelsPerArch), renderer(brightness) {}

ArchSet::~ArchSet() {
    for (auto a : arches) {
        delete a;
    }
}

void ArchSet::addStrip(Strip* strip, uint n) {
    strips.push_back(strip);
    renderer.add(strip);
    if (n * pixelsPerArch != strip->getNumPixels()) {
        printf("Strip on pin %d has %d pixels left over after %d arches\n",
               strip->getPin(), strip->getNumPixels() - n * pixelsPerArch, n);
    }
}

uint ArchSet::add(Strip* strip, ArchFeed feed) {
    uint n = strip->getNumPixels() / pixelsPerArch;
    addStrip(strip, n);
    for (uint i = 0; i < n; i++) {
        arches.push_back(
            new Arch(strip, segmentStart(i, n, feed), pixelsPerArch, feed));
    }
    return n;
}

uint ArchSet::add(Strip* front, ArchFeed frontFeed, Strip* back,
                  ArchFeed backFeed) {
    uint n = std::min(front->getNumPixels(), back->getNumPixels()) /
             pixelsPerArch;
    addStrip(front, n);
    addStrip(back, n);
    for (uint i = 0; i < n; i++) {
        Arch* arch = new Arch(front, segmentStart(i, n, frontFeed),
                              pixelsPerArch, frontFeed);
        arch->addStrand(back, segmentStart(i, n, backFeed), backFeed);
        arches.push_back(arch);
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
