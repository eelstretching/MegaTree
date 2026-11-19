#ifndef SHOWDIRECTOR_H
#define SHOWDIRECTOR_H

#pragma once

#include <stdint.h>

#include "ShowSchedule.h"
#include "ShowTime.h"

/// Core 0's side of the show: knows the show time (through a ShowClock),
/// works out the scene and fade level each frame, and takes commands over
/// serial for testing. The show animators (ShowTreeAnimator,
/// ShowArchAnimator) ask it what to draw.
///
/// Commands, one per line, over USB serial (or the UART):
///
///   S     Print the status.
///   A     Run all the time, ignoring the show hours (for daytime testing).
///   G n   Go to scene n (1-6) now and run all the time. This puts this prop
///         out of step with the others until N.
///   N     Back to normal: show hours, and in step with the other props.
///   T     Ask an internet time server for the time now.
namespace show {

class ShowDirector {
   protected:
    ShowClock clock;
    ShowHours hours;
    bool alwaysOn = false;
    Position pos = {};
    int scene = -1;
    int64_t sceneStartUs = 0;
    bool wasOn = false;
    uint64_t lastStatusUs = 0;

    char line[32];
    int lineLen = 0;

    void command(const char* cmd);

   public:
    ShowDirector(const ShowHours& hours = ShowHours()) : hours(hours) {}

    /// @brief The show time, in microseconds since 1970 (UTC), for the
    /// animators' clockUs().
    uint64_t clockUs() {
        int64_t t = clock.nowUs();
        return t > 0 ? (uint64_t)t : 0;
    }

    /// @brief Works out where the show is for a frame. Call once a frame.
    /// @param frameUs the frame's time on clockUs()
    /// @return true if a scene starts this frame (including when the show
    /// comes on, or a prop starts up partway through a scene), meaning the
    /// animator should start that scene's effect
    bool update(uint64_t frameUs);

    const Position& position() { return pos; }

    /// @brief Whether the lights should be on (showing something) at all.
    bool on() { return pos.on; }

    /// @brief A brightness scaled by the current fade level.
    uint8_t faded(uint8_t brightness) {
        return (uint8_t)((uint32_t)brightness * pos.level / 255);
    }

    /// @brief Reads any serial commands waiting, without blocking. Call once
    /// a frame.
    void poll();

    void printStatus();

    /// @brief Prints the commands.
    void printHelp();
};

}  // namespace show

#endif
