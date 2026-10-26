#ifndef ARCHWIRINGCHECK_H
#define ARCHWIRINGCHECK_H

#pragma once

#include "Archimation.h"

/// @brief Shows whether the ArchSet knows the order of the arches and which
/// end each one is fed from. Each arch glows dimly in a color for its place
/// in the show (arch 0 red, then green, then blue, round again), the first
/// few pixels at its show-start foot are white, and a bright dot runs from
/// that foot over the apex to the other foot, again and again.
///
/// Seen from the street, the colors should go red, green, blue from the
/// left, every white foot should be on the left, and every dot should run
/// left to right. If an arch's white foot is on the right, flip the reversed
/// flag for its strip in ArchSet::add(). If the colors are out of order, add
/// the strips in a different order.
class ArchWiringCheck : public Archimation {
   protected:
    /// @brief How many pixels the dots move each second.
    uint speed;

   public:
    /// @param arches the arches to draw on
    /// @param speed how many pixels the dots move each second
    ArchWiringCheck(ArchSet* arches, uint speed = 20);

    void init() override;
    bool step() override;
};

#endif
