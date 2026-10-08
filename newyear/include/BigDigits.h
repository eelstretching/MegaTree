#ifndef BIGDIGITS_H
#define BIGDIGITS_H

#pragma once

#include "Canvas.h"

/// @brief Draws digits from a little 5x7 font at any size, smoothly. Each
/// canvas pixel is shaded by how much of it the scaled-up glyph covers, so
/// the digits don't jump a whole pixel at a time as they grow.
///
/// The canvas is assumed to have y increasing upward, like the BDF fonts.
class BigDigits {
   public:
    /// @brief The width of a string of digits, in font units, where a digit
    /// is 5 units wide and 7 tall, with one unit between digits.
    static int unitWidth(const char *text);

    /// @brief Draws a string of digits centered on (cx, cy).
    /// @param canvas where to draw
    /// @param text the digits. Anything other than 0-9 draws as a space.
    /// @param cx the center of the text, across
    /// @param cy the center of the text, up
    /// @param height how tall the digits should be, in pixels. Can be bigger
    /// than the canvas.
    /// @param color the color of the digits
    /// @param alpha how opaque to draw, 0 to 255. What's already on the
    /// canvas shows through the rest.
    static void draw(Canvas *canvas, const char *text, float cx, float cy,
                     float height, const RGB &color, uint8_t alpha = 255);
};

#endif
