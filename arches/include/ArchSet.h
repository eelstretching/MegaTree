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

    /// @brief Where these arches are in the whole show, when the show's
    /// arches are split between controllers. See setShowPlace().
    uint firstShowArch = 0;
    uint numShowArches = 0;

    /// @brief Where the i'th arch from the left of n arches on a strip
    /// starts, given which end the strip is fed from.
    uint segmentStart(uint i, uint n, ArchFeed feed) {
        //
        // Fed from the right, the leftmost arch is the last one on the wire.
        return (feed == ArchFeed::RIGHT_FOOT ? n - 1 - i : i) * pixelsPerArch;
    }

    /// @brief Adds a strip to the renderer, saying if it has pixels left
    /// over after n arches.
    void addStrip(Strip* strip, uint n);

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

    /// @brief Adds two strips of arches mounted back to back, so that each
    /// arch has two strands showing the same thing (see Arch). Both strips
    /// are carved into arches the same way as add(strip, feed), and the
    /// arches made from the back strip become the second strand of the
    /// matching arch from the front strip. Each strip says which end its data
    /// wire comes in at, seen from the street, so the two can be fed from
    /// different feet.
    ///
    /// The strips should have the same number of pixels. If one is longer,
    /// the arches only take as many pixels as fit on the shorter one.
    /// @param front the strip for the arches' first strands
    /// @param frontFeed which end the front strip's data wire comes in at
    /// @param back the strip for the arches' second strands
    /// @param backFeed which end the back strip's data wire comes in at
    /// @return how many arches were made from the two strips
    uint add(Strip* front, ArchFeed frontFeed, Strip* back, ArchFeed backFeed);

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

    /// @brief Says where these arches are in the whole show, when the show's
    /// arches are split between controllers: these are arches first to
    /// first + getNumArches() - 1 of showArches, counting from 0 on the left.
    /// Archimations that travel from arch to arch (Leap) use it to run along
    /// the whole row, drawing just the part that's on this controller, and
    /// ArchGlow uses it to keep its pattern going across controllers.
    /// Without it, these arches are the whole show.
    void setShowPlace(uint first, uint showArches) {
        firstShowArch = first;
        numShowArches = showArches;
    }

    /// @brief Which arch of the whole show our first arch is.
    uint getFirstShowArch() { return firstShowArch; }

    /// @brief How many arches the whole show has, on every controller.
    uint getNumShowArches() {
        return numShowArches > 0 ? numShowArches : getNumArches();
    }

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
