#include "ArchFire.h"

#include "colorutils.h"
#include "math8.h"

ArchFire::ArchFire(ArchSet* arches, uint8_t cooling, uint8_t sparking,
                   uint emberRate, uint simRate, uint8_t maxHeat)
    : Archimation(arches, nullptr, 0),
      cooling(cooling),
      sparking(sparking),
      simRate(simRate > 0 ? simRate : 30),
      emberRate(emberRate),
      maxHeat(maxHeat) {}

void ArchFire::init() {
    //
    // With an odd number of pixels the apex belongs to both legs.
    legLength = (arches->getPixelsPerArch() + 1) / 2;
    numLegs = 2 * arches->getNumArches();
    heat.assign(legLength * numLegs, 0);
    embers.clear();
    simSteps = 0;
    emberChances = 0;
    arches->clear();
}

void ArchFire::setLeg(uint leg, uint i, const RGB& c) {
    Arch* a = arches->getArch(leg / 2);
    a->set(leg % 2 == 0 ? i : a->getNumPixels() - 1 - i, c);
}

void ArchFire::simulate() {
    uint8_t coolMax = (cooling * 10) / legLength + 2;
    for (uint leg = 0; leg < numLegs; leg++) {
        uint8_t* h = &heat[leg * legLength];
        //
        // Every cell cools a little.
        for (uint i = 0; i < legLength; i++) {
            h[i] = qsub8(h[i], random8(coolMax));
        }
        //
        // Heat drifts up the leg and spreads out a bit.
        for (uint k = legLength - 1; k >= 2; k--) {
            h[k] = (h[k - 1] + h[k - 2] + h[k - 2]) / 3;
        }
        //
        // New heat flares up near the foot.
        if (random8() < sparking) {
            uint y = random8(MIN(legLength, 5));
            h[y] = qadd8(h[y], random8(160, 255));
        }
    }
}

void ArchFire::moveEmbers(uint32_t usec) {
    for (auto it = embers.begin(); it != embers.end();) {
        //
        // Rise, and cool by about a third of their heat a second.
        it->pos += (uint64_t)it->speed * 256 * usec / 1000000;
        uint8_t cool = (uint64_t)it->heat * usec / 3000000 + 1;
        it->heat = qsub8(it->heat, cool);
        if (it->heat < 16 || it->pos / 256 >= legLength) {
            it = embers.erase(it);
        } else {
            ++it;
        }
    }
}

bool ArchFire::step() {
    if (numLegs == 0 || legLength < 3) {
        return true;
    }

    //
    // Catch the simulation up with the clock.
    uint64_t due = countSince(0, simRate);
    uint n = 0;
    while (simSteps < due && n++ < 8) {
        simulate();
        simSteps++;
    }
    simSteps = due;

    //
    // A crackle: now and then an ember pops out of the flames on a random leg.
    // We get simRate chances a second.
    if (emberRate > 0) {
        uint64_t chances = countSince(0, simRate);
        for (; emberChances < chances; emberChances++) {
            if (random16() % simRate < emberRate && embers.size() < 16) {
                Ember e;
                e.leg = random16() % numLegs;
                e.pos = (legLength / 4 + random8(legLength / 4 + 1)) * 256;
                e.speed = random8(6, 14);
                e.heat = random8(180, 255);
                embers.push_back(e);
                //
                // The pop: a quick flare of heat where it came from.
                uint8_t& h = heat[e.leg * legLength + e.pos / 256];
                h = qadd8(h, 80);
            }
        }
    }
    moveEmbers(getFrameUs());

    for (uint leg = 0; leg < numLegs; leg++) {
        uint8_t* h = &heat[leg * legLength];
        for (uint i = 0; i < legLength; i++) {
            setLeg(leg, i, HeatColor(scale8(h[i], maxHeat)));
        }
    }
    for (auto& e : embers) {
        //
        // A spark of orange-yellow, a little brighter than the flames around
        // it.
        RGB c = RGB(255, 140, 20).scale8(e.heat);
        uint i = e.pos / 256;
        Arch* a = arches->getArch(e.leg / 2);
        int p = e.leg % 2 == 0 ? i : a->getNumPixels() - 1 - i;
        RGB under = a->get(p);
        setLeg(e.leg, i,
               RGB(qadd8(under.r, c.r), qadd8(under.g, c.g),
                   qadd8(under.b, c.b)));
    }
    return true;
}
