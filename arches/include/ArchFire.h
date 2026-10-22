#ifndef ARCHFIRE_H
#define ARCHFIRE_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief A cozy hearth fire on the arches: flames flicker up both legs of
/// every arch from the feet, and now and then the fire crackles and throws an
/// ember that drifts up the leg, cooling as it goes.
///
/// Each leg is its own Fire2012-style column of heat (the same idea as
/// PicoLEDs' Fire), simulated at a fixed rate whatever the frame rate. Embers
/// move by the clock. It's meant to be slow and warm, so the hottest flames
/// stop at yellow rather than going white.
class ArchFire : public Archimation {
   protected:
    struct Ember {
        /// @brief Which leg it's on, as an index into the heat columns.
        uint leg;
        /// @brief How far up the leg it is, in 1/256ths of a pixel.
        uint32_t pos;
        /// @brief How fast it rises, in pixels per second.
        uint8_t speed;
        /// @brief How bright it is, out of 255.
        uint8_t heat;
    };

    /// @brief How fast the flames cool as they rise. Higher values make
    /// shorter flames.
    uint8_t cooling;

    /// @brief The chance out of 255, each simulation step, that a leg gets a
    /// new burst of heat at its foot. Higher values make a busier fire.
    uint8_t sparking;

    /// @brief How many simulation steps a second.
    uint simRate;

    /// @brief About how many embers a second fly off the whole fire.
    uint emberRate;

    /// @brief The hottest heat we show, so the fire stays orange and yellow.
    uint8_t maxHeat;

    /// @brief The slowest and fastest an ember drifts up, in pixels per
    /// second. Each ember picks a speed in between.
    uint8_t emberMinSpeed;
    uint8_t emberMaxSpeed;

    /// @brief Heat per pixel, legLength pixels for each leg, feet first.
    std::vector<uint8_t> heat;

    uint legLength = 0;
    uint numLegs = 0;

    /// @brief How many simulation steps we've taken since init().
    uint64_t simSteps = 0;

    /// @brief How many chances to throw an ember we've had since init().
    uint64_t emberChances = 0;

    std::vector<Ember> embers;

    /// @brief Takes one simulation step for every leg.
    void simulate();

    /// @brief Moves the embers on by usec microseconds.
    void moveEmbers(uint32_t usec);

    /// @brief Sets pixel i of a leg, counting from its foot.
    void setLeg(uint leg, uint i, const RGB& c);

   public:
    /// @param arches the arches to draw on
    /// @param cooling how fast the flames cool as they rise; higher is
    /// shorter
    /// @param sparking chance out of 255 per step of new heat at each foot
    /// @param emberRate about how many embers a second the whole fire throws
    /// @param simRate how many simulation steps a second
    /// @param maxHeat the hottest heat shown, 0-255 (191 tops out at yellow)
    /// @param emberMinSpeed the slowest an ember drifts up, in pixels a second
    /// @param emberMaxSpeed the fastest an ember drifts up, in pixels a second
    ArchFire(ArchSet* arches, uint8_t cooling = 95, uint8_t sparking = 110,
             uint emberRate = 3, uint simRate = 30, uint8_t maxHeat = 200,
             uint8_t emberMinSpeed = 6, uint8_t emberMaxSpeed = 14);

    void init() override;
    bool step() override;
};

#endif
