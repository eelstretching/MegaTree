#include "NewYearShow.h"

#include <math.h>
#include <stdio.h>

#include "BigDigits.h"
#include "HolidayPalettes.h"

//
// Colors for the last ten digits, one per digit.
static const RGB digitColors[] = {
    RGB(255, 200, 40),   // gold
    RGB(255, 20, 20),    // red
    RGB(28, 191, 38),    // green
    RGB(255, 252, 245),  // warm white
    RGB(40, 80, 255),    // blue
};

static const RGB gold(255, 190, 30);

NewYearShow::NewYearShow(Canvas *canvas, CountdownClock *clock)
    : Animation(canvas, nullptr),
      clock(clock),
      calm(canvas, 4, 5, 30),
      frantic(canvas, 7, 8, 10),
      fireworks(canvas) {
    frantic.addPalette(RedGreenWhitePalette);
    frantic.addPalette(RetroC9Palette);
    frantic.addPalette(FairyLightPalette);
}

const char *NewYearShow::phaseName(Phase p) {
    switch (p) {
        case WAITING:
            return "waiting for the time";
        case PRESHOW:
            return "pre-show";
        case COUNTDOWN:
            return "countdown";
        case FINAL_MINUTE:
            return "final minute";
        case FINAL_TEN:
            return "final ten";
        case CELEBRATE:
            return "celebrate";
        case POSTSHOW:
            return "post-show";
    }
    return "?";
}

NewYearShow::Phase NewYearShow::phaseFor(double s) {
    if (!clock->hasTime()) {
        return WAITING;
    }
    if (s > getCountdownSeconds()) {
        return PRESHOW;
    }
    if (s > 60) {
        return COUNTDOWN;
    }
    if (s > 10) {
        return FINAL_MINUTE;
    }
    if (s > 0) {
        return FINAL_TEN;
    }
    if (s > -celebrateSeconds) {
        return CELEBRATE;
    }
    return POSTSHOW;
}

void NewYearShow::init() {
    phase = phaseFor(clock->secondsToMidnight());
    calm.init();
    frantic.init();
    fireworks.init();
}

void NewYearShow::mask(double s) {
    int width = canvas->getWidth();
    int total = getCountdownSeconds();
    //
    // Pixels are numbered from the bottom row up, so the top goes dark first.
    // Pixel k has until k+1 seconds are left; the one numbered floor(s) is
    // going out right now.
    int going = (int)floor(s);
    float frac = s - going;
    for (int k = MAX(going, 0); k < total; k++) {
        int x = k % width;
        int y = k / width;
        if (k == going) {
            RGB glow(RGB::FairyLight);
            glow.nscale8_video((uint8_t)(frac * 255));
            canvas->set(x, y, glow);
        } else {
            canvas->set(x, y, RGB::Black);
        }
    }
}

void NewYearShow::drawFinalMinute(double s) {
    //
    // The whole tree lights up again with fast, dense twinkles that flare on
    // each tick and fade through the second, like a heartbeat.
    frantic.step();
    int seconds = (int)ceil(s);
    float frac = s - floor(s);
    uint8_t level = (uint8_t)(90 + 165 * frac);
    for (int y = 0; y < canvas->getHeight(); y++) {
        for (int x = 0; x < canvas->getWidth(); x++) {
            RGB c = canvas->get(x, y);
            c.nscale8_video(level);
            canvas->set(x, y, c);
        }
    }
    //
    // And the seconds left, big, over the top.
    char text[4];
    snprintf(text, sizeof(text), "%d", seconds);
    float h = canvas->getHeight();
    BigDigits::draw(canvas, text, canvas->getWidth() / 2.0f, h / 2.0f,
                    0.6f * h, RGB(255, 252, 245), 220);
}

void NewYearShow::drawFinalTen(double s) {
    canvas->clear();
    int digit = (int)ceil(s);
    //
    // How far through this digit's second we are, 0 to 1.
    float t = digit - s;
    float h = canvas->getHeight();
    float height = h * (0.3f + 1.0f * t);
    //
    // Fade in quickly and fade out over the last quarter of the second.
    float a = MIN(1.0f, t / 0.1f);
    if (t > 0.75f) {
        a *= (1.0f - t) / 0.25f;
    }
    char text[4];
    snprintf(text, sizeof(text), "%d", digit);
    BigDigits::draw(canvas, text, canvas->getWidth() / 2.0f, h / 2.0f, height,
                    digitColors[digit % 5], (uint8_t)(a * 255));
}

void NewYearShow::drawCelebration(double s) {
    float since = -s;
    //
    // The fireworks fade the canvas a little each step rather than clearing
    // it, which leaves trails behind the sparks, and lets the year and the
    // midnight flash fade out gently too. They open with a barrage.
    fireworks.setFinale(since < finaleSeconds);
    fireworks.step();

    if (since < yearSeconds) {
        char text[8];
        snprintf(text, sizeof(text), "%d", clock->getNewYear());
        float w = canvas->getWidth();
        float h = canvas->getHeight();
        //
        // As big as fits, leaving a little room around the edges.
        float height =
            MIN(0.8f * h, 0.9f * w * 7 / BigDigits::unitWidth(text));
        //
        // Shimmer between gold and white.
        uint8_t shimmer = (uint8_t)(127 + 127 * sinf(since * 6));
        BigDigits::draw(canvas, text, w / 2, h / 2, height,
                        gold.lerp8(RGB::White, shimmer / 2));
    }

    //
    // A flash at the stroke of midnight.
    if (since < 0.5f) {
        uint8_t flash = (uint8_t)(255 * (1 - since / 0.5f));
        for (int y = 0; y < canvas->getHeight(); y++) {
            for (int x = 0; x < canvas->getWidth(); x++) {
                RGB under = canvas->get(x, y);
                canvas->set(x, y, under.lerp8(RGB::White, flash));
            }
        }
    }
}

bool NewYearShow::step() {
    aw.start();
    double s = clock->secondsToMidnight();
    Phase p = phaseFor(s);
    if (p != phase) {
        printf("New Year's show: %s\n", phaseName(p));
        canvas->clear();
        if (p == CELEBRATE) {
            fireworks.init();
        }
        phase = p;
    }

    switch (phase) {
        case WAITING:
        case PRESHOW:
        case POSTSHOW:
            calm.step();
            break;
        case COUNTDOWN:
            calm.step();
            mask(s);
            break;
        case FINAL_MINUTE:
            drawFinalMinute(s);
            break;
        case FINAL_TEN:
            drawFinalTen(s);
            break;
        case CELEBRATE:
            drawCelebration(s);
            break;
    }
    aw.finish();
    //
    // The show never ends; it just goes back to twinkling.
    return true;
}
