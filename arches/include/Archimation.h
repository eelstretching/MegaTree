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

    /// @brief How many times a second this archimation wants step() called,
    /// or 0 for one that goes by the clock instead (see elapsedUs()).
    uint8_t fps = 30;

    /// @brief The ArchAnimator's clock, in microseconds, when this
    /// archimation started, and at its latest step. See elapsedUs() and
    /// getFrameUs().
    uint64_t startUs = 0;
    uint64_t nowUs = 0;
    /// @brief Microseconds since the previous step.
    uint32_t frameUs = 0;

    /// @brief How many times something that happens perSecond times a
    /// second has happened between since (an elapsedUs() time) and now.
    /// Working positions out from the time like this, rather than adding up a
    /// little each frame, keeps an archimation in exactly the same place at
    /// any frame rate.
    uint64_t countSince(uint64_t since, uint32_t perSecond) {
        uint64_t e = elapsedUs();
        return e > since ? (e - since) * perSecond / 1000000 : 0;
    }

    /// @brief How long something that happens perSecond times a second takes
    /// to happen n times, in microseconds, rounded up so that countSince()
    /// has reached n by then.
    static uint64_t usFor(uint64_t n, uint32_t perSecond) {
        return perSecond > 0 ? (n * 1000000 + perSecond - 1) / perSecond : 0;
    }

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

    /// @brief Gets how many times a second this archimation wants to be
    /// stepped. Default is 30.
    ///
    /// The ArchAnimator draws frames at one fixed rate for the whole show and
    /// steps each archimation as often as this asks, so an archimation that
    /// moves a fixed amount per step keeps its speed. Archimations that return
    /// 0 are stepped exactly once per frame and work out how far to move from
    /// getFrameUs() and elapsedUs(), so they look the same at any frame rate.
    /// All of the archimations here do that.
    /// @return steps per second, or 0 for a clock-driven archimation.
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

    /// @brief Called by the ArchAnimator just before init(), with its clock.
    /// Archimations that wrap others pass it on.
    virtual void startClock(uint64_t now) {
        startUs = now;
        nowUs = now;
        frameUs = 0;
    }

    /// @brief Called by the ArchAnimator before each step() with the time of
    /// the frame being drawn. A second step in the same frame sees a frameUs
    /// of 0, and the clock never goes backwards here even if the
    /// ArchAnimator's does.
    virtual void setFrameTime(uint64_t now) {
        if (now <= nowUs) {
            frameUs = 0;
            return;
        }
        frameUs = (uint32_t)(now - nowUs);
        nowUs = now;
    }

    /// @brief Microseconds since this archimation started (its last init()).
    uint64_t elapsedUs() const { return nowUs - startUs; }

    /// @brief Microseconds since the previous step: how far to move this
    /// step.
    uint32_t getFrameUs() const { return frameUs; }

    ArchSet* getArches() { return arches; }
};

/// @brief Runs several archimations at once, for example on different arches.
/// Later archimations draw over earlier ones. It steps as often as its
/// fastest stepped archimation wants, or once a frame if they all go by the
/// clock; clock-driven ones are fine stepped more often, since they move by
/// getFrameUs().
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

    void startClock(uint64_t now) override {
        Archimation::startClock(now);
        for (auto a : archimations) {
            a->startClock(now);
        }
    }

    void setFrameTime(uint64_t now) override {
        Archimation::setFrameTime(now);
        for (auto a : archimations) {
            a->setFrameTime(now);
        }
    }

    bool step() override {
        for (auto a : archimations) {
            a->step();
        }
        return true;
    }
};

/// @brief Wraps a never-ending archimation and makes it time-limited.
class TimedArchimation : public Archimation {
   protected:
    Archimation* archimation;

    /// @brief How long to run for, in microseconds of the ArchAnimator's
    /// clock.
    uint64_t duration;

   public:
    /// @param archimation the never-ending archimation.
    /// @param durationMS how many milliseconds to let it run for.
    TimedArchimation(Archimation* archimation, uint durationMS)
        : Archimation(archimation->getArches(), nullptr),
          archimation(archimation),
          duration((uint64_t)durationMS * 1000) {}

    void init() override { archimation->init(); }

    bool step() override {
        archimation->step();
        return elapsedUs() < duration;
    }

    void startClock(uint64_t now) override {
        Archimation::startClock(now);
        archimation->startClock(now);
    }

    void setFrameTime(uint64_t now) override {
        Archimation::setFrameTime(now);
        archimation->setFrameTime(now);
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
