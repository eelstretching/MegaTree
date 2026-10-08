#ifndef ARCHRISE_H
#define ARCHRISE_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief Fills arches with color from both feet up to the apex. Arches fill
/// one after another (or all together), hold for a moment once they're all
/// lit, then go dark and start again with the colors moved along by one.
class ArchRise : public Archimation {
   protected:
    /// @brief Indices into the color map, used in turn for each arch.
    std::vector<uint8_t> colors;

    /// @brief If true, all arches rise at once rather than one by one.
    bool together;

    /// @brief How many frames to hold with all arches lit.
    uint holdFrames;

    /// @brief The arch that's rising, when rising one by one.
    uint arch = 0;

    /// @brief How far up the legs we've lit.
    uint level = 0;

    /// @brief Frames left to hold for, or 0 if we're rising.
    uint hold = 0;

    /// @brief Moves the colors along by one each time round.
    uint colorOffset = 0;

    RGB colorFor(uint archIndex);

   public:
    /// @param arches the arches to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each arch
    /// @param together if true, all arches rise at once
    /// @param holdFrames how many frames to hold with all arches lit
    ArchRise(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
             const uint8_t* colors, bool together = false,
             uint holdFrames = 30);

    void init() override;
    bool step() override;
};

#endif
