#ifndef TOPPERANIMATOR_H
#define TOPPERANIMATOR_H

#pragma once

#include <vector>

#include "Topper.h"
#include "TopperAnimation.h"
#include "StopWatch.h"

/// @brief Runs a list of animations on a star topper, one after another,
/// showing a frame at the current animation's frame rate. Like PicoLEDs'
/// Animator, with RandomAnimator's shuffling available via setShuffle().
class TopperAnimator {
   protected:
    Topper* topper;
    std::vector<TopperAnimation*> animations;

    /// @brief The timed wrappers that addTimed made, which we delete.
    std::vector<TopperAnimation*> owned;

    int pos = 0;

    int fps = 30;

    /// @brief If true, reshuffle the list each time we get back to the start.
    bool shuffle = false;

    float usPerFrame;

    uint missedFrames = 0;

    uint frameCount = 0;

    StopWatch frameWatch;
    StopWatch showWatch;
    StopWatch stepWatch;

    uint64_t startTime = time_us_64();

   public:
    TopperAnimator(Topper* topper, int fps = 30) : topper(topper) {
        setFPS(fps);
    }

    virtual ~TopperAnimator();

    /// @brief Adds an animation that runs until it says it's done.
    void add(TopperAnimation* a);

    /// @brief Adds a never-ending animation that runs for durationMS
    /// milliseconds.
    void addTimed(TopperAnimation* a, int durationMS);

    void setFPS(int fps);
    int getFPS() { return fps; }

    void setShuffle(bool shuffle) { this->shuffle = shuffle; }

    /// @brief Starts the first animation.
    virtual void init();

    /// @brief Steps the current animation, shows the frame and sleeps until
    /// it's time for the next one.
    virtual bool step();

    /// @brief Called when we move to a new animation. The default shuffles
    /// the list at the top of each pass if shuffling is on.
    virtual void animationChanged();

    uint getMissedFrames() { return missedFrames; }
    uint getFrameCount() { return frameCount; }

    void printStats();
};

#endif
