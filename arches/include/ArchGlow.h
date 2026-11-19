#ifndef ARCHGLOW_H
#define ARCHGLOW_H

#pragma once

#include <vector>

#include "Archimation.h"

/// @brief A calm, steady glow: each arch lights all over in one color and
/// slowly breathes brighter and dimmer, the breath rolling from arch to arch
/// along the row like a slow wave.
///
/// Give it one color for a plain warm white glow, or several to have the
/// arches take them in turn (red, green, red...). It goes by the clock, and
/// counts arches across the whole show (ArchSet::setShowPlace()), so the
/// colors and the wave carry on across controllers.
class ArchGlow : public Archimation {
   protected:
    std::vector<RGB> colors;
    /// @brief How long one breath takes, in milliseconds.
    uint32_t periodMs;
    /// @brief How dim a breath gets, out of 255.
    uint8_t low;
    /// @brief How far behind its left-hand neighbour each arch breathes, as
    /// a fraction of a breath out of 256.
    uint8_t lag;

   public:
    /// @param arches the arches to draw on
    /// @param colors the colors the arches take in turn, left to right
    /// @param periodMs how long one breath takes, in milliseconds
    /// @param low how dim a breath gets, out of 255
    /// @param lag how far behind its left-hand neighbour each arch breathes,
    /// out of 256 of a breath
    ArchGlow(ArchSet* arches, const std::vector<RGB>& colors,
             uint32_t periodMs = 8000, uint8_t low = 110, uint8_t lag = 40);

    bool step() override;
};

#endif
