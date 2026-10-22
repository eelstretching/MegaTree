#ifndef CYLON_H
#define CYLON_H

#pragma once

#include "Archimation.h"

/// @brief A Cylon eye (a.k.a. Larson scanner): a red eye sweeps from one foot
/// of each arch to the other and back, leaving a glowing trail that fades
/// behind it. By your command. It goes by the clock, so it sweeps and fades at
/// the same speed at any frame rate.
class Cylon : public Archimation {
   protected:
    RGB color;

    /// @brief How many pixels wide the eye is.
    uint eyeWidth;

    /// @brief How many pixels the eye moves each second.
    uint speed;

    /// @brief How much of each pixel's brightness survives a 30th of a
    /// second, out of 255. Higher values leave longer trails.
    uint8_t fade;


    /// @brief If true, every other arch sweeps the opposite way.
    bool alternate;


   public:
    /// @param arches the arches to draw on
    /// @param color the color of the eye
    /// @param eyeWidth how many pixels wide the eye is
    /// @param speed how many pixels the eye moves each second
    /// @param fade how much brightness the trail keeps each 30th of a second,
    /// out of 255
    /// @param alternate if true, every other arch sweeps the opposite way
    Cylon(ArchSet* arches, const RGB& color = RGB::Red, uint eyeWidth = 3,
          uint speed = 30, uint8_t fade = 192, bool alternate = false);

    void init() override;
    bool step() override;
};

#endif
