#ifndef ARCHIMATION_H
#define ARCHIMATION_H

#pragma once

#include <string.h>

#include <vector>

#include "ArchSet.h"
#include "ColorMap.h"
#include "StopWatch.h"

/// @brief An animation for leaping arches: the 1D counterpart of PicoLEDs'
/// Animation. Subclasses draw on the arches in step(), and an ArchAnimator
/// shows the result.
class Archimation {
    friend class ArchAnimator;

   protected:
    /// @brief The arches that we're animating.
    ArchSet* arches;

    /// @brief The color map that we're using for this archimation.
    ColorMap* colorMap;

    /// @brief A stop watch to count our animation time.
    StopWatch aw;

    char name[20] = {0};

    uint8_t fps = 30;

   public:
    Archimation() : arches(nullptr), colorMap(nullptr) {}

    /// @brief Construct an archimation that will draw on the given arches.
    /// @param arches the arches we'll draw on.
    /// @param colorMap the colors we'll draw with.
    Archimation(ArchSet* arches, ColorMap* colorMap)
        : arches(arches), colorMap(colorMap) {}

    Archimation(ArchSet* arches, ColorMap* colorMap, uint8_t fps)
        : arches(arches), colorMap(colorMap), fps(fps) {}

    virtual ~Archimation() {}

    /// @brief Gets the frames-per-second this archimation requires. Default
    /// is 30.
    virtual int getFPS() { return fps; }

    virtual void setFPS(uint8_t fps) { this->fps = fps; }

    virtual void setName(const char* n) { strncpy(name, n, sizeof(name) - 1); }

    virtual const char* getName() { return name; }

    /// @brief Initializes the archimation, possibly after it has run to
    /// completion previously.
    virtual void init() {}

    /// @brief Takes one step in the archimation. The default implementation
    /// doesn't do anything.
    /// @returns true if the archimation will continue after this step, false
    /// if it is complete.
    virtual bool step() { return true; }

    /// @brief Finishes the archimation. Can be used to clear up, etc.
    virtual void finish() {}

    ArchSet* getArches() { return arches; }
};

/// @brief Runs several archimations at once, for example on different arches.
/// Later archimations draw over earlier ones.
class MultiArchimation : public Archimation {
    std::vector<Archimation*> archimations;

   public:
    MultiArchimation(ArchSet* arches, ColorMap* colorMap)
        : Archimation(arches, colorMap, 0) {}

    void add(Archimation* a) {
        archimations.push_back(a);
        fps = MAX(fps, a->getFPS());
    }

    void init() override {
        for (auto a : archimations) {
            a->init();
        }
    }

    bool step() override {
        for (auto a : archimations) {
            a->step();
        }
        return true;
    }

    int getFPS() override { return fps > 0 ? fps : 30; }
};

/// @brief Wraps a never-ending archimation and makes it time-limited.
class TimedArchimation : public Archimation {
   protected:
    Archimation* archimation;

    /// @brief How long to run for, in microseconds.
    uint64_t duration;

    /// @brief When this round started, in microseconds.
    uint64_t start = 0;

   public:
    /// @param archimation the never-ending archimation.
    /// @param durationMS how many milliseconds to let it run for.
    TimedArchimation(Archimation* archimation, uint durationMS)
        : Archimation(archimation->getArches(), nullptr),
          archimation(archimation),
          duration((uint64_t)durationMS * 1000) {}

    void init() override {
        start = time_us_64();
        archimation->init();
    }

    bool step() override {
        archimation->step();
        return time_us_64() - start < duration;
    }

    void finish() override { archimation->finish(); }

    int getFPS() override { return archimation->getFPS(); }

    const char* getName() override {
        snprintf(name, sizeof(name), "T %s %.1f", archimation->getName(),
                 duration / 1000000.0);
        return name;
    }
};

#endif
