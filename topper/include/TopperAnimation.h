#ifndef TOPPERANIMATION_H
#define TOPPERANIMATION_H

#pragma once

#include <string.h>

#include <vector>

#include "Topper.h"
#include "ColorMap.h"
#include "StopWatch.h"

/// @brief An animation for the star topper: its counterpart of PicoLEDs'
/// Animation. Subclasses draw on the topper in step(), and a TopperAnimator
/// shows the result.
class TopperAnimation {
    friend class TopperAnimator;

   protected:
    /// @brief The topper that we're animating.
    Topper* topper;

    /// @brief The color map that we're using for this animation.
    ColorMap* colorMap;

    /// @brief A stop watch to count our animation time.
    StopWatch aw;

    char name[20] = {0};

    uint8_t fps = 30;

   public:
    TopperAnimation() : topper(nullptr), colorMap(nullptr) {}

    /// @brief Construct an animation that will draw on the given topper.
    /// @param topper the topper we'll draw on.
    /// @param colorMap the colors we'll draw with.
    TopperAnimation(Topper* topper, ColorMap* colorMap)
        : topper(topper), colorMap(colorMap) {}

    TopperAnimation(Topper* topper, ColorMap* colorMap, uint8_t fps)
        : topper(topper), colorMap(colorMap), fps(fps) {}

    virtual ~TopperAnimation() {}

    /// @brief Gets the frames-per-second this animation requires. Default
    /// is 30.
    virtual int getFPS() { return fps; }

    virtual void setFPS(uint8_t fps) { this->fps = fps; }

    virtual void setName(const char* n) { strncpy(name, n, sizeof(name) - 1); }

    virtual const char* getName() { return name; }

    /// @brief Initializes the animation, possibly after it has run to
    /// completion previously.
    virtual void init() {}

    /// @brief Takes one step in the animation. The default implementation
    /// doesn't do anything.
    /// @returns true if the animation will continue after this step, false
    /// if it is complete.
    virtual bool step() { return true; }

    /// @brief Finishes the animation. Can be used to clear up, etc.
    virtual void finish() {}

    Topper* getTopper() { return topper; }
};

/// @brief Runs several animations at once, for example on different rings.
/// Later animations draw over earlier ones.
class MultiTopperAnimation : public TopperAnimation {
    std::vector<TopperAnimation*> animations;

   public:
    MultiTopperAnimation(Topper* topper, ColorMap* colorMap)
        : TopperAnimation(topper, colorMap, 0) {}

    void add(TopperAnimation* a) {
        animations.push_back(a);
        fps = MAX(fps, a->getFPS());
    }

    void init() override {
        for (auto a : animations) {
            a->init();
        }
    }

    bool step() override {
        for (auto a : animations) {
            a->step();
        }
        return true;
    }

    int getFPS() override { return fps > 0 ? fps : 30; }
};

/// @brief Wraps a never-ending animation and makes it time-limited.
class TimedTopperAnimation : public TopperAnimation {
   protected:
    TopperAnimation* animation;

    /// @brief How long to run for, in microseconds.
    uint64_t duration;

    /// @brief When this round started, in microseconds.
    uint64_t start = 0;

   public:
    /// @param animation the never-ending animation.
    /// @param durationMS how many milliseconds to let it run for.
    TimedTopperAnimation(TopperAnimation* animation, uint durationMS)
        : TopperAnimation(animation->getTopper(), nullptr),
          animation(animation),
          duration((uint64_t)durationMS * 1000) {}

    void init() override {
        start = time_us_64();
        animation->init();
    }

    bool step() override {
        animation->step();
        return time_us_64() - start < duration;
    }

    void finish() override { animation->finish(); }

    int getFPS() override { return animation->getFPS(); }

    const char* getName() override {
        snprintf(name, sizeof(name), "T %s %.1f", animation->getName(),
                 duration / 1000000.0);
        return name;
    }
};

#endif
