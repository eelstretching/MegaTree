#ifndef ARCHANIMATOR_H
#define ARCHANIMATOR_H

#pragma once

#include <vector>

#include "ArchSet.h"
#include "Archimation.h"
#include "StopWatch.h"

/// @brief Runs a list of archimations on a set of arches, one after another.
/// Like PicoLEDs' Animator, with RandomAnimator's shuffling available via
/// setShuffle().
///
/// Frames are drawn at one fixed rate (40 a second unless you say otherwise),
/// and each frame starts on a whole multiple of the frame time on the
/// ArchAnimator's clock, just as PicoLEDs' Animator does. A subclass can
/// override clockUs() with a clock shared between Picos (PicoComm's show
/// time), and then the arches draw their frames at the same instants as every
/// other prop. Each archimation is stepped as often as its getFPS() asks; see
/// Archimation::getFPS().
class ArchAnimator {
   public:
    static constexpr int kDefaultFPS = 40;
    /// @brief Most steps an archimation gets in one frame, however far behind
    /// it is.
    static constexpr int kMaxStepsPerFrame = 4;

   protected:
    ArchSet* arches;
    std::vector<Archimation*> archimations;

    /// @brief The timed wrappers that addTimed made, which we delete.
    std::vector<Archimation*> owned;

    int pos = 0;

    int fps = kDefaultFPS;

    /// @brief If true, reshuffle the list each time we get back to the start.
    bool shuffle = false;

    /// @brief The number of microseconds per frame, at our frame rate.
    uint32_t usPerFrame;

    /// @brief The clock time this frame was due: a whole multiple of
    /// usPerFrame.
    uint64_t frameTime = 0;

    /// @brief Microseconds of steps the current archimation is owed, for
    /// archimations that step at their own rate.
    uint32_t stepOwedUs = 0;

    uint missedFrames = 0;

    uint frameCount = 0;

    StopWatch frameWatch;
    StopWatch showWatch;
    StopWatch stepWatch;

    uint64_t startTime = time_us_64();

   public:
    /// @param arches the arches to animate
    /// @param fps how many frames a second to draw, whatever rates the
    /// archimations step at
    ArchAnimator(ArchSet* arches, int fps = kDefaultFPS) : arches(arches) {
        setFPS(fps);
    }

    virtual ~ArchAnimator();

    /// @brief Adds an archimation that runs until it says it's done.
    void add(Archimation* a);

    /// @brief Adds a never-ending archimation that runs for durationMS
    /// milliseconds.
    void addTimed(Archimation* a, int durationMS);

    /// @brief Sets the frame rate: how many frames a second are drawn and
    /// shown. Archimations keep stepping at their own rates.
    void setFPS(int fps);
    int getFPS() { return fps; }

    /// @brief The clock frames are paced by, in microseconds. Override it to
    /// pace frames by a clock shared with other Picos. It has to move at the
    /// same rate as time_us_64() (give or take a fraction of a percent), but
    /// it can be offset from it, and can jump.
    virtual uint64_t clockUs() { return time_us_64(); }

    /// @brief The clock time the current frame was due.
    uint64_t getFrameTime() { return frameTime; }

    void setShuffle(bool shuffle) { this->shuffle = shuffle; }

    /// @brief Starts the first archimation.
    virtual void init();

    /// @brief Steps the current archimation as many times as it's due, shows
    /// the frame and waits until it's time for the next one.
    virtual bool step();

    /// @brief Called when we move to a new archimation. The default shuffles
    /// the list at the top of each pass if shuffling is on.
    virtual void archimationChanged();

    uint getMissedFrames() { return missedFrames; }
    uint getFrameCount() { return frameCount; }

    void printStats();

   protected:
    /// @brief Starts an archimation: gives it the clock time it starts at,
    /// inits it, and arranges for its first step to come on the next frame.
    void startArchimation(Archimation* a, uint64_t at);

    /// @brief How many times to step the archimation this frame.
    int stepsDue(Archimation* a);

    /// @brief Waits for the next frame's start, re-sending the current frame
    /// meanwhile if we're dithering. Counts a missed frame if we're already
    /// late.
    virtual void waitForNextFrame();
};

#endif
