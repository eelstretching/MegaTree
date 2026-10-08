#ifndef ARCH_H
#define ARCH_H

#pragma once

#include "Direction.h"
#include "Strip.h"
#include "color.h"

/// @brief One leaping arch: a run of consecutive pixels on a Strip. Several
/// arches wired end to end share one strip (and so one controller output),
/// with each Arch looking after its own segment.
///
/// Pixel 0 is the foot where data enters the arch and pixel getNumPixels() - 1
/// is the other foot. Positions outside the arch are ignored on set and read
/// as black on get, so animations can draw past the ends without checking.
class Arch {
   protected:
    /// @brief The strip holding this arch's pixels.
    Strip* strip;

    /// @brief Where this arch's first pixel is on the strip.
    uint start;

    /// @brief How many pixels are in this arch.
    uint numPixels;

    /// @brief If true, pixel 0 is at the far end of the segment, for an arch
    /// that was hung the other way round.
    bool reversed = false;

    /// @brief Maps a position in the arch to a position on the strip.
    uint toStrip(uint p) {
        return start + (reversed ? numPixels - 1 - p : p);
    }

   public:
    /// @brief Creates an arch on part of a strip.
    /// @param strip the strip the arch is on
    /// @param start the position on the strip of the arch's first pixel
    /// @param numPixels how many pixels the arch has
    Arch(Strip* strip, uint start, uint numPixels)
        : strip(strip), start(start), numPixels(numPixels) {}

    Strip* getStrip() { return strip; }

    uint getStart() { return start; }

    uint getNumPixels() { return numPixels; }

    /// @brief Gets the position of the top of the arch.
    uint getApex() { return numPixels / 2; }

    void setReversed(bool reversed) { this->reversed = reversed; }

    bool isReversed() { return reversed; }

    /// @brief Sets the pixel at position p in the arch to the given color.
    void set(int p, const RGB& color);

    /// @brief Sets the pixel at position p, counted from the first foot, and
    /// the matching pixel counted from the other foot. Drawing with this makes
    /// both legs of the arch do the same thing.
    void setMirrored(int p, const RGB& color);

    /// @brief Gets the color of the pixel at position p in the arch.
    const RGB& get(int p);

    /// @brief Fills the whole arch with a color.
    void fill(const RGB& color);

    /// @brief Fills n pixels of the arch with a color, starting at position p.
    void fill(const RGB& color, int p, int n);

    /// @brief Turns all of the arch's pixels off.
    void clear() { fill(RGB::Black); }

    /// @brief Rotates the arch's pixels by one. RIGHT and UP move pixels away
    /// from pixel 0; LEFT and DOWN move them towards it.
    void rotate(Direction direction);
};

#endif
