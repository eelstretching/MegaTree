#ifndef ARCHRISE_H
#define ARCHRISE_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief Fills arches with color from both feet up to the apex. Arches fill
/// one after another (or all together), hold for a moment once they're all
/// lit, then go dark and start again with the colors moved along by one. It
/// goes by the clock, so it runs at the same speed at any frame rate.
class ArchRise : public Archimation {
   protected:
    /// @brief Indices into the color map, used in turn for each arch.
    std::vector<uint8_t> colors;

    /// @brief If true, all arches rise at once rather than one by one.
    bool together;

    /// @brief How long to hold with all arches lit, in microseconds.
    uint64_t holdUs;

    /// @brief How many pixels up the legs we light each second.
    uint speed;

    /// @brief How many pixels we've lit this time round: one per arch per
    /// level when rising together, one per level of one arch otherwise.
    uint lit = 0;

    /// @brief True while we're holding with all arches lit.
    bool holding = false;

    /// @brief When the current rise or hold started, from elapsedUs().
    uint64_t phaseStart = 0;

    /// @brief How many lighting steps a full rise takes.
    uint riseSteps();

    /// @brief Takes lighting step i of the rise.
    void light(uint i);

    /// @brief Moves the colors along by one each time round.
    uint colorOffset = 0;

    RGB colorFor(uint archIndex);

   public:
    /// @param arches the arches to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each arch
    /// @param together if true, all arches rise at once
    /// @param holdMS how many milliseconds to hold with all arches lit
    /// @param speed how many pixels up the legs to light each second
    ArchRise(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
             const uint8_t* colors, bool together = false,
             uint holdMS = 1000, uint speed = 30);

    void init() override;
    bool step() override;
};

#endif
