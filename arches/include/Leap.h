#ifndef LEAP_H
#define LEAP_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief The classic leaping arch: a ball of light with a fading tail runs
/// up and over one arch, then leaps to the next, looping back to the first
/// arch after the last. The ball takes on a different color in each arch.
class Leap : public Archimation {
   protected:
    /// @brief Indices into the color map, used in turn for each arch.
    std::vector<uint8_t> colors;

    /// @brief How many pixels of fading tail follow the ball.
    uint tail;

    /// @brief How many pixels the ball moves each frame.
    uint speed;

    /// @brief Where the ball is, across all of the arches.
    uint head = 0;

   public:
    /// @param arches the arches to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each arch
    /// @param tail how many pixels of fading tail follow the ball
    /// @param speed how many pixels the ball moves each frame
    Leap(ArchSet* arches, ColorMap* colorMap, uint8_t nColors,
         const uint8_t* colors, uint tail = 10, uint speed = 2);

    void init() override;
    bool step() override;
};

#endif
