#ifndef NEWYEARSHOW_H
#define NEWYEARSHOW_H

#pragma once

#include "Animation.h"
#include "CountdownClock.h"
#include "Fireworks2D.h"
#include "TwinkleFox.h"

/// @brief The New Year's Eve show, driven by how long it is until midnight.
///
/// - Before the countdown: the whole tree twinkles.
/// - The countdown: one pixel per second goes dark, working down from the
///   top, so the lit part of the tree is the time left. The pixel that's
///   going out glows warm white as it fades. Twinkles play in the lit part.
/// - The last minute: the whole tree lights up again with fast, dense
///   twinkles that pulse with each second, and the seconds left show big
///   over the top.
/// - The last ten seconds: big digits zoom in, one per second.
/// - Midnight: a flash, fireworks, and the new year in big digits for the
///   first minute.
/// - Afterwards: back to twinkling.
class NewYearShow : public Animation {
   public:
    enum Phase {
        WAITING,
        PRESHOW,
        COUNTDOWN,
        FINAL_MINUTE,
        FINAL_TEN,
        CELEBRATE,
        POSTSHOW,
    };

   protected:
    CountdownClock *clock;

    TwinkleFox calm;
    TwinkleFox frantic;
    Fireworks2D fireworks;

    Phase phase = WAITING;

    /// @brief How long the fireworks go on after midnight, in seconds.
    float celebrateSeconds = 15 * 60;

    /// @brief How long the new year shows over the fireworks, in seconds.
    float yearSeconds = 60;

    /// @brief How long the opening barrage of fireworks lasts, in seconds.
    float finaleSeconds = 20;

    /// @brief Turns off the pixels whose seconds have run out, and makes the
    /// one going out this second glow.
    void mask(double secondsLeft);

    void drawFinalMinute(double secondsLeft);

    void drawFinalTen(double secondsLeft);

    void drawCelebration(double secondsLeft);

   public:
    NewYearShow(Canvas *canvas, CountdownClock *clock);

    /// @brief How many seconds the countdown takes: one per pixel.
    int getCountdownSeconds() {
        return canvas->getWidth() * canvas->getHeight();
    };

    /// @brief Which part of the show we're in, given the time to midnight.
    Phase phaseFor(double secondsLeft);

    Phase getPhase() { return phase; };

    static const char *phaseName(Phase p);

    void setCelebrateSeconds(float s) { celebrateSeconds = s; };

    void init() override;

    bool step() override;
};

#endif
