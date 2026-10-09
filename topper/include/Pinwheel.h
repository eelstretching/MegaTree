#ifndef PINWHEEL_H
#define PINWHEEL_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief Blades of color that spin round the middle of the star like a
/// pinwheel. With a twist, the blades curl into a spiral, and in red and
/// white it's a peppermint candy. With no colors, the blades are a rainbow
/// that runs all the way round.
class Pinwheel : public TopperAnimation {
   protected:
    /// @brief Indices into the color map, used in turn for the blades.
    std::vector<uint8_t> colors;

    /// @brief How many blades go round the star.
    uint8_t blades;

    /// @brief How far the blades curl from the centre to the tips, out of
    /// 256 of a full turn. Negative curls them the other way.
    int twist;

    /// @brief How far the pinwheel turns each frame, out of 256 of a turn.
    /// Negative turns it anticlockwise.
    int8_t speed;

    uint8_t rotation = 0;

   public:
    /// @param topper the topper to draw on
    /// @param colorMap the colors to draw with, or nullptr for a rainbow
    /// @param nColors how many color indices are in colors (0 for a rainbow)
    /// @param colors indices into the color map, used in turn for the blades
    /// @param blades how many blades go round the star
    /// @param twist how far the blades curl from the centre to the tips, out
    /// of 256 of a full turn
    /// @param speed how far the pinwheel turns each frame, out of 256
    Pinwheel(Topper* topper, ColorMap* colorMap, uint8_t nColors,
             const uint8_t* colors, uint8_t blades = 5, int twist = 0,
             int8_t speed = 2);

    void init() override;
    bool step() override;
};

#endif
