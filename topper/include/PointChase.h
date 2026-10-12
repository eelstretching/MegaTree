#ifndef POINTCHASE_H
#define POINTCHASE_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief The star's five points light up one at a time, clockwise round the
/// star, each fading as the next one lights. The color moves on each time it
/// gets back to the top.
class PointChase : public TopperAnimation {
   protected:
    /// @brief Indices into the color map, used in turn for each time round.
    std::vector<uint8_t> colors;

    /// @brief How many frames each point stays lit.
    uint framesPerPoint;

    /// @brief How much of each pixel's brightness survives a frame, out of
    /// 255.
    uint8_t keep;

    uint frame = 0;
    uint point = 0;
    uint color = 0;

   public:
    /// @param topper the topper to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each time
    /// round
    /// @param framesPerPoint how many frames each point stays lit
    /// @param keep how much brightness the points keep each frame once
    /// they've gone out, out of 255
    PointChase(Topper* topper, ColorMap* colorMap, uint8_t nColors,
               const uint8_t* colors, uint framesPerPoint = 6,
               uint8_t keep = 200);

    void init() override;
    bool step() override;
};

#endif
