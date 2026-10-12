#ifndef COLORWIPE_H
#define COLORWIPE_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief A new color sweeps across the star in a straight line, covering
/// the old one. Each sweep comes from a different direction: the first comes
/// down from the top, and each after that turns by the given amount.
class ColorWipe : public TopperAnimation {
   protected:
    /// @brief Indices into the color map, used in turn for each sweep.
    std::vector<uint8_t> colors;

    /// @brief How many frames a sweep takes to cross the star.
    uint frames;

    /// @brief How many frames to hold once a sweep is done.
    uint holdFrames;

    /// @brief How much the direction turns between sweeps, out of 256 of a
    /// turn.
    uint8_t turn;

    /// @brief Which way the current sweep is going, out of 256 of a turn,
    /// with 0 going down from the top.
    uint8_t direction = 0;

    uint frame = 0;
    uint color = 0;

   public:
    /// @param topper the topper to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each sweep
    /// @param frames how many frames a sweep takes to cross the star
    /// @param holdFrames how many frames to hold once a sweep is done
    /// @param turn how much the direction turns between sweeps, out of 256
    ColorWipe(Topper* topper, ColorMap* colorMap, uint8_t nColors,
              const uint8_t* colors, uint frames = 30, uint holdFrames = 20,
              uint8_t turn = 77);

    void init() override;
    bool step() override;
};

#endif
