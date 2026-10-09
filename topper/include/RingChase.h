#ifndef RINGCHASE_H
#define RINGCHASE_H

#pragma once

#include <vector>

#include "TopperAnimation.h"

/// @brief A comet with a fading tail runs round each ring, the rings taking
/// turns to go clockwise and anticlockwise. They all go round at the same
/// rate, so the comets line up as they pass each other at the top and the
/// bottom of the star.
class RingChase : public TopperAnimation {
   protected:
    /// @brief Indices into the color map, used in turn for each ring.
    std::vector<uint8_t> colors;

    /// @brief How far round the comets go each frame, in 1/65536ths of a
    /// turn.
    uint16_t speed;

    /// @brief How much of each pixel's brightness survives a frame, out of
    /// 255. Higher values leave longer tails.
    uint8_t keep;

    /// @brief If true, every other ring goes anticlockwise.
    bool alternate;

    /// @brief How far round the comets are, in 1/65536ths of a turn.
    uint16_t phase = 0;

   public:
    /// @param topper the topper to draw on
    /// @param colorMap the colors to draw with
    /// @param nColors how many color indices are in colors
    /// @param colors indices into the color map, used in turn for each ring
    /// @param speed how far round the comets go each frame, in 1/65536ths of
    /// a turn (1092 is once round every two seconds at 30 fps)
    /// @param keep how much brightness the tails keep each frame, out of 255
    /// @param alternate if true, every other ring goes anticlockwise
    RingChase(Topper* topper, ColorMap* colorMap, uint8_t nColors,
              const uint8_t* colors, uint16_t speed = 1092,
              uint8_t keep = 210, bool alternate = true);

    void init() override;
    bool step() override;
};

#endif
