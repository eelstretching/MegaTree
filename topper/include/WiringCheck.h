#ifndef WIRINGCHECK_H
#define WIRINGCHECK_H

#pragma once

#include "TopperAnimation.h"

/// @brief Shows whether the topper knows where each ring's wire starts and
/// which way it goes. Each ring glows dimly in its own color (the inner ring
/// red, then green, then blue), the pixel the topper thinks is at the top
/// tip is white, and a bright dot runs round each ring the way the topper
/// thinks is clockwise.
///
/// If the white pixels aren't all at the top tip, or a dot runs
/// anticlockwise, fix it with Topper::setRingStart() and
/// Topper::setRingReversed(). The easy way: run this with neither set. On
/// each ring the white pixel is then the first node on the wire; count
/// clockwise from the top tip to it (the tip is 0), and that's the ring's
/// start. If the dot runs anticlockwise, the ring is reversed as well.
class WiringCheck : public TopperAnimation {
   protected:
    /// @brief How many frames the dots take to move on by one pixel.
    uint framesPerPixel;

    uint frame = 0;

   public:
    /// @param topper the topper to draw on
    /// @param framesPerPixel how many frames the dots take to move on by one
    /// pixel
    WiringCheck(Topper* topper, uint framesPerPixel = 6);

    void init() override;
    bool step() override;
};

#endif
