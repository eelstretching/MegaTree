#ifndef RADIATINGRAINBOW_H
#define RADIATINGRAINBOW_H

#pragma once

#include "TopperAnimation.h"

/// @brief Rings of rainbow color that radiate out from the middle of the star
/// to its tips (or fall in from the tips to the middle). The color depends
/// only on how far a pixel is from the centre, so the bands follow circles
/// rather than the star's outline, and the points light up last.
class RadiatingRainbow : public TopperAnimation {
   protected:
    /// @brief How much the hue changes from the centre to the tips, out of
    /// 256. 256 shows one whole rainbow across the star at once.
    uint spread;

    /// @brief How much the hue moves each frame.
    uint8_t speed;

    /// @brief If true, the rainbow falls in towards the centre.
    bool inward;

    uint8_t hue = 0;

   public:
    /// @param topper the topper to draw on
    /// @param spread how much of the rainbow (out of 256) fits between the
    /// centre and the tips
    /// @param speed how much the hue moves each frame
    /// @param inward if true, the colors move in towards the centre
    RadiatingRainbow(Topper* topper, uint spread = 192, uint8_t speed = 4,
                     bool inward = false);

    void init() override;
    bool step() override;
};

#endif
