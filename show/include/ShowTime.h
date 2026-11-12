#ifndef SHOWTIME_H
#define SHOWTIME_H

#pragma once

#include <stdint.h>

/// Keeping every prop on the same time of day, without them talking to each
/// other.
///
/// Core 1 looks after the time: it reads the board's DS3231 RTC when we
/// start and every ten minutes after, and when there's WiFi it asks an
/// internet time server (NTP) at start-up and every few hours, then sets the
/// RTC from that so the RTC is right even if the internet goes away. Every
/// prop doing that independently keeps them within a few milliseconds of
/// each other. Core 0 runs the lights and reads the time through a
/// ShowClock.
///
/// The radio's driver claims a PIO state machine of its own, so core 1 sets
/// the radio up before anything else, and TimeService::start() doesn't return
/// until it has: call it before the canvas's setup(), which claims PIO state
/// machines for the LEDs (the same ordering as OfficeSign's BtReadySignal).
namespace show {

enum class TimeSource : uint8_t {
    /// @brief We don't know the time yet.
    NONE,
    /// @brief From the DS3231.
    RTC,
    /// @brief From an internet time server.
    NTP,
};

const char* sourceName(TimeSource s);

/// @brief What core 1 knows about the time, and how it's been going.
struct TimeStatus {
    /// @brief The time since 1970 (UTC) is time_us_64() plus this.
    int64_t offsetUs = 0;
    TimeSource source = TimeSource::NONE;
    /// @brief time_us_64() when the offset was last set.
    uint64_t syncedAtUs = 0;
    /// @brief Whether there's a DS3231 that has the time (it hasn't lost
    /// power since it was last set).
    bool rtcOk = false;
    /// @brief Whether the radio started, and whether we're on the WiFi now.
    bool radioOk = false;
    bool wifiUp = false;
    /// @brief How many NTP syncs worked and failed.
    uint32_t ntpGood = 0;
    uint32_t ntpBad = 0;
    /// @brief time_us_64() at the last good NTP sync, or 0.
    uint64_t ntpAtUs = 0;
    /// @brief The round trip time of the last good NTP sync.
    int64_t ntpRttUs = 0;
    /// @brief How far NTP moved the clock last time: roughly how far the
    /// RTC had drifted since the sync before.
    int64_t ntpStepUs = 0;
    /// @brief How far the latest RTC read moved the clock (how far the
    /// Pico's own crystal had drifted since the read before).
    int64_t rtcStepUs = 0;
};

/// @brief The time keeper on core 1. There's only one, so it's all static.
class TimeService {
   public:
    /// @brief Starts core 1. Returns once the radio is set up (or has
    /// failed), so it's safe to claim PIO for the LEDs.
    static void start();

    /// @brief A copy of the current status. Safe to call from core 0 at any
    /// time.
    static TimeStatus status();

    /// @brief Asks core 1 for an NTP sync as soon as it can.
    static void requestNtp();
};

/// @brief Core 0's view of the show time, in microseconds since 1970 (UTC).
///
/// When core 1 corrects the time we don't jump to it (an effect would
/// stutter), we ease towards it by up to a millisecond a second, which no
/// one can see. Corrections bigger than half a second (the first time we
/// hear the time, or a badly wrong RTC) are taken at once.
class ShowClock {
   protected:
    int64_t appliedUs = 0;
    uint64_t lastUs = 0;
    bool have = false;

    /// @brief A deliberate offset, for trying out scenes in the daytime. Not
    /// shared with other props, so it puts this one out of step.
    int64_t testOffsetUs = 0;

   public:
    /// @brief Whether we know the time yet.
    bool valid();

    /// @brief The show time, in microseconds since 1970 (UTC).
    int64_t nowUs();

    void setTestOffsetUs(int64_t us) { testOffsetUs = us; }
    int64_t getTestOffsetUs() { return testOffsetUs; }
};

}  // namespace show

#endif
