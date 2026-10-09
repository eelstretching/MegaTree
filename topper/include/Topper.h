#ifndef TOPPER_H
#define TOPPER_H

#pragma once

#include <vector>

#include "Renderer.h"
#include "Strip.h"
#include "color.h"

/// @brief Where one of the topper's pixels is on the star, worked out once
/// when the topper is made so that animations can draw by position.
///
/// Everything is as seen from the front of the star (from the street), with
/// the star's centre at (0, 0) and the tip of the outer ring's top point at
/// (0, 1).
struct StarPixel {
    /// @brief Across from the centre: -1 is the far left, 1 the far right.
    float x;

    /// @brief Up from the centre: 1 is the top tip, about -0.81 the bottom
    /// tips.
    float y;

    /// @brief Which ring the pixel is in, 0 being the innermost.
    uint8_t ring;

    /// @brief Which of the five points the pixel is nearest: 0 is the top
    /// point, and they count clockwise from there.
    uint8_t point;

    /// @brief The direction from the centre, as a fraction of a full turn
    /// out of 256: 0 is straight up, 64 is to the right, and so on round
    /// clockwise.
    uint8_t angle;

    /// @brief How far from the centre, out of 255. The outer ring's tips are
    /// at 255.
    uint8_t radius;
};

/// @brief A star tree topper: rings of pixels in the shape of a five-pointed
/// star, one inside another, on a strip of their own.
///
/// The default layout is HolidayCoro's 24" 3 row star (123-24): 90 nodes in
/// three rings of 20, 30 and 40, wired innermost ring first. Each ring is a
/// star outline with ten corners (five tips and five valleys) and the same
/// number of nodes along each of its ten edges, with a node at every corner.
///
/// Pixels are numbered two ways:
///
/// - By ring: set(ring, i, color) where i counts clockwise round the ring (as
///   seen from the front), starting at the tip of the top point.
/// - Across the whole star: set(p, color) where p counts through ring 0, then
///   ring 1, and so on, each in the same order as above.
///
/// Neither depends on where the wire actually enters each ring.
/// setRingStart() and setRingReversed() say where it does, and the topper
/// takes care of the rest, so animations always see the star the right way
/// up. The WiringCheck animation shows whether they're right.
class Topper {
   protected:
    Strip* strip;

    /// @brief Where the topper's first pixel is on the strip.
    uint start;

    /// @brief How many pixels are in each ring, innermost first.
    std::vector<uint> ringSizes;

    /// @brief Where each ring's pixels start on the strip, relative to start.
    std::vector<uint> ringOffsets;

    /// @brief The position round each ring (counting clockwise from the top
    /// tip) of the first node on the wire.
    std::vector<uint> ringStarts;

    /// @brief Whether each ring is wired anticlockwise as seen from the front.
    std::vector<bool> ringReversed;

    /// @brief Where every pixel is, in ring order.
    std::vector<StarPixel> pixels;

    uint numPixels = 0;

    Renderer renderer;

    /// @brief Maps a position in a ring to a position on the strip.
    uint toStrip(uint ring, uint i);

    void layOut();

   public:
    /// @brief The outer ring's valleys are this far from the centre, as a
    /// fraction of the distance to its tips. Measured from the product photo
    /// of the 123-24; a regular star would be 0.38.
    static constexpr float VALLEY_RATIO = 0.43f;

    /// @brief How far in each ring is from the next one out, as a fraction
    /// of the outer ring's size.
    static constexpr float RING_STEP = 0.17f;

    /// @brief Creates a topper on part of a strip.
    /// @param strip the strip the topper is on
    /// @param ringSizes how many pixels are in each ring, innermost first, in
    /// the order they're wired. Multiples of 10 put a node on every corner.
    /// @param start the position on the strip of the topper's first pixel
    /// @param brightness the global brightness to render at
    Topper(Strip* strip, std::vector<uint> ringSizes = {20, 30, 40},
           uint start = 0, uint8_t brightness = 32);

    /// @brief Set up for rendering. Call once, before the first show().
    void setup() { renderer.setup(); }

    /// @brief Sends the current pixels out to the star.
    void show() { renderer.render(); }

    Renderer* getRenderer() { return &renderer; }

    Strip* getStrip() { return strip; }

    void setBrightness(uint8_t brightness) {
        renderer.setBrightness(brightness);
    }

    uint8_t getBrightness() { return renderer.getBrightness(); }

    /// @brief Says where the wire enters a ring.
    /// @param ring the ring, 0 being the innermost
    /// @param pos the position round the ring of the first node on the wire,
    /// counting clockwise (from the front) from the tip of the top point
    void setRingStart(uint ring, uint pos);

    /// @brief Says which way round a ring is wired. HolidayCoro's
    /// instructions wire them clockwise as seen from the front, which is the
    /// default.
    /// @param ring the ring, 0 being the innermost
    /// @param reversed true if the wire runs anticlockwise from the front
    void setRingReversed(uint ring, bool reversed);

    uint getNumRings() { return ringSizes.size(); }

    uint getRingSize(uint ring) {
        return ring < ringSizes.size() ? ringSizes[ring] : 0;
    }

    /// @brief Gets the position across the whole star of a ring's first
    /// pixel.
    uint getRingOffset(uint ring) {
        return ring < ringOffsets.size() ? ringOffsets[ring] : numPixels;
    }

    uint getNumPixels() { return numPixels; }

    /// @brief Gets where pixel p (counted across the whole star) is.
    const StarPixel& getPixel(uint p) { return pixels[p < numPixels ? p : 0]; }

    /// @brief Sets pixel i of a ring. Positions round the ring wrap, so
    /// animations can run round and round without checking.
    void set(uint ring, int i, const RGB& color);

    /// @brief Gets pixel i of a ring, wrapping round like set().
    const RGB& get(uint ring, int i);

    /// @brief Sets a pixel by its position across the whole star. Positions
    /// outside the star are ignored.
    void set(int p, const RGB& color);

    /// @brief Gets a pixel by its position across the whole star. Positions
    /// outside the star read as black.
    const RGB& get(int p);

    /// @brief Fills a whole ring with a color.
    void fillRing(uint ring, const RGB& color);

    /// @brief Fills the whole star with a color.
    void fill(const RGB& color);

    /// @brief Turns the whole star off.
    void clear() { fill(RGB::Black); }

    /// @brief Fades every pixel, keeping keep/256 of its brightness.
    void fade(uint8_t keep);
};

#endif
