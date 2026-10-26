#ifndef ARCH_H
#define ARCH_H

#pragma once

#include "Direction.h"
#include "Strip.h"
#include "color.h"

/// @brief Which foot of an arch (or end of a run of arches on one strip) the
/// data wire comes in at, as seen from the street. The show always runs from
/// the left foot to the right foot, so LEFT_FOOT is fed from the show's start
/// and RIGHT_FOOT from its end.
enum class ArchFeed {
    LEFT_FOOT,
    RIGHT_FOOT,
};

/// @brief One leaping arch: a run of consecutive pixels on a Strip. Several
/// arches wired end to end share one strip (and so one controller output),
/// with each Arch looking after its own segment.
///
/// Positions in an arch are in show order, not wiring order: pixel 0 is the
/// left foot, seen from the street, and pixel getNumPixels() - 1 is the right
/// foot, whichever foot the data wire comes in at (see ArchFeed). An arch fed
/// at its right foot is handy when the controller sits to the arch's right;
/// animations draw the same either way.
///
/// Positions outside the arch are ignored on set and read as black on get,
/// so animations can draw past the ends without checking.
class Arch {
   protected:
    /// @brief The strip holding this arch's pixels.
    Strip* strip;

    /// @brief Where this arch's first pixel is on the strip.
    uint start;

    /// @brief How many pixels are in this arch.
    uint numPixels;

    /// @brief Which foot the data wire comes in at.
    ArchFeed feed = ArchFeed::LEFT_FOOT;

    /// @brief Maps a position in the arch to a position on the strip.
    uint toStrip(uint p) {
        return start + (feed == ArchFeed::RIGHT_FOOT ? numPixels - 1 - p : p);
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

    /// @brief Says which foot the data wire comes in at, seen from the
    /// street.
    void setFeed(ArchFeed feed) { this->feed = feed; }

    ArchFeed getFeed() { return feed; }

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

    /// @brief Shifts the arch's pixels by one, like rotate(), but the pixel
    /// shifted off the end is dropped and a new color comes in at the other
    /// end. RIGHT brings color in at pixel 0; LEFT brings it in at the far
    /// foot.
    void shift(Direction direction, const RGB& color);

    /// @brief Shifts both legs down by one, away from the apex and towards
    /// the feet, bringing the given color in at the apex.
    void shiftFromApex(const RGB& color);
};

#endif
