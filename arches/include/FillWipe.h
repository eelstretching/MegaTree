#ifndef FILLWIPE_H
#define FILLWIPE_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief Where the black comes from when FillWipe clears the arches.
enum WipeFrom {
    /// @brief From pixel 0's foot, pushing the colors over to the other foot.
    WIPE_LEFT,
    /// @brief From the far foot, pushing the colors over to pixel 0's foot.
    WIPE_RIGHT,
    /// @brief From the apex, pushing the colors down both legs.
    WIPE_MIDDLE,
};

/// @brief Fills all of the arches from both feet up to the apex, using a
/// passed-in set of colors in turn, then clears them by shifting black in from
/// the left, the right or the middle, and starts again. It goes by the clock,
/// so it runs at the same speed at any frame rate.
class FillWipe : public Archimation {
   protected:
    /// @brief Indices into the color map, used in turn as the arches fill.
    std::vector<uint8_t> colors;

    WipeFrom wipeFrom;

    /// @brief How many pixels in a row get the same color while filling.
    uint pixelsPerColor;

    /// @brief How long to hold once full, and once clear, in microseconds.
    uint64_t holdUs;

    /// @brief How many pixels a second we fill, and wipe.
    uint speed;

    enum Phase { FILLING, HOLDING_FULL, WIPING, HOLDING_CLEAR };

    Phase phase = FILLING;

    /// @brief How far we've got through filling or wiping, in pixels.
    uint count = 0;

    /// @brief When the current phase started, from elapsedUs().
    uint64_t phaseStart = 0;

    /// @brief How many pixels it takes to fill the arches.
    uint fillSteps();

    /// @brief How many steps the wipe takes to clear the arches.
    uint wipeSteps();

   public:
    /// @param arches the arches to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn as we fill
    /// @param wipeFrom where the black comes in from when clearing
    /// @param pixelsPerColor how many pixels in a row get the same color
    /// @param holdMS how many milliseconds to hold when full and when clear
    /// @param speed how many pixels a second to fill, and to wipe
    FillWipe(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
             const uint8_t* colors, WipeFrom wipeFrom = WIPE_MIDDLE,
             uint pixelsPerColor = 4, uint holdMS = 1000, uint speed = 30);

    void setWipeFrom(WipeFrom wipeFrom) { this->wipeFrom = wipeFrom; }

    void init() override;
    bool step() override;
};

#endif
