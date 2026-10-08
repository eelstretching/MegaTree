#ifndef COUNTDOWNCLOCK_H
#define COUNTDOWNCLOCK_H

#pragma once

#include <stdint.h>

/// @brief Keeps the wall-clock time and works out how long it is until
/// midnight on New Year's.
///
/// The time comes from somewhere else (the RTC, or a command typed over
/// serial). We remember it along with the Pico's microsecond timer at that
/// moment, and count forward from there.
///
/// For rehearsals, the clock can pretend midnight is a given number of
/// seconds away and run faster than real time, so the whole show can be
/// checked in a few minutes.
class CountdownClock {
   protected:
    /// @brief Seconds since 1970 at the moment we were set, or -1 if we've
    /// never been set.
    int64_t baseEpoch = -1;

    /// @brief time_us_64() when we were set.
    uint64_t baseUs = 0;

    /// @brief How many clock seconds pass per real second. 1 except when
    /// rehearsing.
    float speed = 1;

    /// @brief Seconds since 1970 at the midnight we're counting down to.
    int64_t midnightEpoch = 0;

    /// @brief The year that starts at that midnight.
    int newYear = 0;

    bool rehearsing = false;

   public:
    /// @brief Seconds since 1970 for a calendar date and time.
    static int64_t toEpoch(int year, int month, int day, int hour, int min,
                           int sec);

    /// @brief Breaks seconds since 1970 back down into a calendar date and
    /// time.
    static void fromEpoch(int64_t t, int &year, int &month, int &day,
                          int &hour, int &min, int &sec);

    /// @brief Sets the current date and time. We count down to the next
    /// midnight on January 1st, unless it's already January 1st, in which case
    /// that midnight has just happened and we're celebrating.
    void setTime(int year, int month, int day, int hour, int min, int sec);

    /// @brief Pretends midnight is secondsToGo away, with time running speed
    /// times faster than real time.
    void rehearse(float secondsToGo, float speed, int year);

    /// @brief Whether we know what time it is.
    bool hasTime() { return baseEpoch >= 0; };

    bool isRehearsing() { return rehearsing; };

    /// @brief Seconds since 1970 now, with a fraction.
    double now();

    /// @brief How many seconds are left until midnight, with a fraction.
    /// Negative after midnight.
    double secondsToMidnight();

    /// @brief The year that starts at midnight.
    int getNewYear() { return newYear; };

    /// @brief Breaks the current time back down into a calendar date and
    /// time.
    void getTime(int &year, int &month, int &day, int &hour, int &min,
                 int &sec);
};

#endif
