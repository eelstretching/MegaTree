#ifndef ARCHSET_H
#define ARCHSET_H

#pragma once

#include <vector>

#include "Arch.h"
#include "Renderer.h"
#include "Strip.h"
#include "color.h"

/// @brief All of the leaping arches in a display, which is what Archimations
/// draw on. It plays the part that Canvas plays for 2D animations.
///
/// Every arch has the same number of pixels. Each strip added is carved into
/// as many arches as fit on it, and arches are numbered in the order they were
/// added, so arch 0 is the first arch on the first strip.
///
/// Arches go left to right in the show in the order they're added, whatever
/// pins their strips are on, so add strips in show order, left to right as
/// seen from the street, saying which end each is fed from (see add()).
///
/// The set also numbers every pixel in every arch from 0 to getNumPixels() - 1
/// in that same order, which treats the whole display as one long run that
/// animations can send things along.
class ArchSet {
   protected:
    /// @brief How many pixels each arch has.
    uint pixelsPerArch;

    std::vector<Strip*> strips;

    std::vector<Arch*> arches;

    Renderer renderer;

   public:
    /// @brief Creates an empty set of arches.
    /// @param pixelsPerArch how many pixels each arch has
    /// @param brightness the global brightness to render at
    ArchSet(uint pixelsPerArch, uint8_t brightness = 32);

    ~ArchSet();

    /// @brief Adds a strip of arches wired end to end. The strip is carved
    /// into getNumPixels() / pixelsPerArch arches. Any pixels left over at the
    /// end of the strip aren't part of an arch.
    ///
    /// A strip fed at ArchFeed::RIGHT_FOOT has its data wire come in at the
    /// right foot of its rightmost arch and run back to the left. The arches
    /// on it are still added left to right (the one at the far end of the
    /// wire first) and each is fed at its right foot, so animations run the
    /// same way as on a strip fed from the left. With one arch per strip,
    /// this just says which foot the arch is fed at.
    /// @param strip the strip to add
    /// @param feed which end the data wire comes in at, seen from the street
    /// @return how many arches were made from the strip
    uint add(Strip* strip, ArchFeed feed);

    /// @brief Set up for rendering, after all strips have been added.
    void setup() { renderer.setup(); }

    /// @brief Sends the current pixels out to the arches.
    void show() { renderer.render(); }

    Renderer* getRenderer() { return &renderer; }

    void setBrightness(uint8_t brightness) {
        renderer.setBrightness(brightness);
    }

    uint8_t getBrightness() { return renderer.getBrightness(); }

    uint getPixelsPerArch() { return pixelsPerArch; }

    uint getNumArches() { return arches.size(); }

    /// @brief Gets the total number of pixels in all of the arches.
    uint getNumPixels() { return arches.size() * pixelsPerArch; }

    Arch* getArch(uint i) { return i < arches.size() ? arches[i] : nullptr; }

    Arch* operator[](uint i) { return getArch(i); }

    /// @brief Sets a pixel by its position across all of the arches.
    void set(int p, const RGB& color);

    /// @brief Gets a pixel by its position across all of the arches.
    const RGB& get(int p);

    /// @brief Fills every arch with a color.
    void fill(const RGB& color);

    /// @brief Turns every arch off.
    void clear() { fill(RGB::Black); }
};

#endif
