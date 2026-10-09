#ifndef STARLIGHT_H
#define STARLIGHT_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief The star glows a warm gold that slowly breathes, while bright
/// glints twinkle across it and fade away.
class Starlight : public TopperAnimation {
   protected:
    /// @brief The color the star glows.
    RGB glow;

    /// @brief The color of the glints.
    RGB glint;

    /// @brief The chance, out of 256, that a new glint starts each frame.
    uint8_t rate;

    /// @brief How much of a glint survives each frame, out of 255.
    uint8_t keep;

    /// @brief How bright each pixel's glint is now.
    std::vector<uint8_t> glints;

    /// @brief How far through a breath we are, out of 256.
    uint8_t breath = 0;

   public:
    /// @param topper the topper to draw on
    /// @param glow the color the star glows
    /// @param glint the color of the glints
    /// @param rate the chance, out of 256, that a new glint starts each frame
    /// @param keep how much of a glint survives each frame, out of 255
    Starlight(Topper* topper, const RGB& glow = RGB(213, 150, 20),
              const RGB& glint = RGB::White, uint8_t rate = 128,
              uint8_t keep = 220);

    void init() override;
    bool step() override;
};

#endif
