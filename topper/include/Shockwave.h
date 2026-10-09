#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief A ring of light bursts out from the middle of the star and races
/// out past the tips, leaving a glow that fades behind it. Each burst is the
/// next color in the list.
class Shockwave : public TopperAnimation {
   protected:
    /// @brief Indices into the color map, used in turn for each burst.
    std::vector<uint8_t> colors;

    /// @brief How thick the wave is, as a distance out of 255.
    uint8_t width;

    /// @brief How far the wave moves each frame, as a distance out of 255.
    uint8_t speed;

    /// @brief How much of each pixel's brightness survives a frame, out of
    /// 255.
    uint8_t keep;

    /// @brief How many frames to wait between bursts.
    uint gap;

    /// @brief How far out the middle of the wave is.
    int front = 0;

    /// @brief Frames left to wait before the next burst.
    uint wait = 0;

    uint color = 0;

    /// @brief The distance from the centre of the nearest pixel, where each
    /// wave starts.
    int innermost = 0;

   public:
    /// @param topper the topper to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each burst
    /// @param width how thick the wave is, out of 255 (the star's radius)
    /// @param speed how far the wave moves each frame, out of 255
    /// @param keep how much brightness the glow keeps each frame, out of 255
    /// @param gap how many frames to wait between bursts
    Shockwave(Topper* topper, ColorMap* colorMap, uint8_t nColors,
              const uint8_t* colors, uint8_t width = 40, uint8_t speed = 8,
              uint8_t keep = 200, uint gap = 10);

    void init() override;
    bool step() override;
};

#endif
