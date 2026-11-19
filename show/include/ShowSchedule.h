#ifndef SHOWSCHEDULE_H
#define SHOWSCHEDULE_H

#pragma once

#include <stdint.h>

/// The Christmas show's schedule, shared by every prop.
///
/// Every prop has this same table built in and works out where the show is
/// from the time of day alone, so props change scene together without
/// talking to each other. All a prop needs is a clock that agrees with the
/// others (see ShowClock), and a reboot mid-show just jumps back in at the
/// right place.
///
/// The show is a loop of scenes that starts again every kLoopSeconds,
/// counting from local midnight, so with a 20 minute loop Hearth starts on
/// the hour, at :20 and at :40. Each prop says which of its own effects plays
/// in each scene. Every scene fades in from black and back out to black
/// over kFadeUs, which hides small differences between the props' clocks.
///
/// Nothing in here touches the hardware, so it can be tested on a computer
/// (show/test).
namespace show {

/// @brief The scenes, in the order they play.
enum SceneId : uint8_t {
    HEARTH,
    CLASSIC,
    ICICLES,
    LEAP,
    CANDY_CANE,
    GREETINGS,
    NUM_SCENES,
};

struct Scene {
    SceneId id;
    const char* name;
    uint32_t seconds;
};

/// @brief The scenes and how long each runs, in playing order.
constexpr Scene kScenes[NUM_SCENES] = {
    {HEARTH, "Hearth", 6 * 60},
    {CLASSIC, "Classic", 4 * 60},
    {ICICLES, "Icicles", 3 * 60},
    {LEAP, "Leap", 2 * 60},
    {CANDY_CANE, "Candy cane", 3 * 60},
    {GREETINGS, "Greetings", 2 * 60},
};

constexpr uint32_t loopSeconds() {
    uint32_t s = 0;
    for (const Scene& sc : kScenes) {
        s += sc.seconds;
    }
    return s;
}

/// @brief How long one time round the scenes takes.
constexpr uint32_t kLoopSeconds = loopSeconds();

static_assert(86400 % kLoopSeconds == 0,
              "The loop has to fit a whole number of times into a day, so "
              "that every day's loops start at the same times");

/// @brief How long each fade (in from black at the start of a scene, out to
/// black at its end) takes.
constexpr int64_t kFadeUs = 750000;

constexpr int64_t kUsPerSecond = 1000000;
constexpr int64_t kUsPerDay = 86400 * kUsPerSecond;

/// @brief When the show runs, in local time.
struct ShowHours {
    /// @brief Local time is UTC plus this. US Eastern standard time; Eastern
    /// stays on standard time from 2 Nov 2026 to 14 Mar 2027, so the whole
    /// season needs no daylight saving.
    int32_t utcOffsetMinutes = -5 * 60;
    /// @brief Lights on, in minutes after local midnight (4:30 pm).
    int32_t startMinute = 16 * 60 + 30;
    /// @brief Lights off, in minutes after local midnight (10:30 pm). If it's
    /// earlier than startMinute, the show runs past midnight.
    int32_t endMinute = 22 * 60 + 30;
};

/// @brief Where the show is at some moment.
struct Position {
    /// @brief Whether the show is running (inside show hours).
    bool on;
    /// @brief The scene playing (or that would be playing, if the show's
    /// off).
    SceneId scene;
    /// @brief When this scene started, in microseconds since 1970 (UTC). The
    /// same on every prop, so effects that go by the clock line up.
    int64_t sceneStartUs;
    /// @brief How far into the scene we are.
    int64_t usIntoScene;
    /// @brief How bright to be, out of 255, for the fades: 0 at the very
    /// start and end of each scene and outside show hours.
    uint8_t level;
};

/// @brief Floor division and modulo for negative times.
constexpr int64_t floorMod(int64_t a, int64_t m) {
    int64_t r = a % m;
    return r < 0 ? r + m : r;
}

/// @brief Works out where the show is.
/// @param unixUs the time, in microseconds since 1970 (UTC)
/// @param hours when the show runs
/// @param alwaysOn if true, ignore the show hours and run all day (for
/// testing in the daytime)
Position locate(int64_t unixUs, const ShowHours& hours = ShowHours(),
                bool alwaysOn = false);

/// @brief Breaks a time into a local date and time of day, for printing.
void localTime(int64_t unixUs, int32_t utcOffsetMinutes, int& year,
               int& month, int& day, int& hour, int& minute, int& second);

/// @brief Seconds since 1970 for a UTC date and time.
int64_t toUnixSeconds(int year, int month, int day, int hour, int minute,
                      int second);

}  // namespace show

#endif
