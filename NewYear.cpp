//
// The New Year's Eve show for the MegaTree: a countdown to midnight where one
// pixel goes dark per second, zooming digits for the last ten seconds, and
// fireworks at midnight. See newyear/include/NewYearShow.h for the details.
//
// The time comes from the board's DS3231 RTC, which keeps UTC (that's what
// the universal-strip-controller's wifi_rtc_sync example writes to it). We
// add UTC_OFFSET_MINUTES to get local time.
//
// Commands over serial (UART or USB), one per line:
//
//   T 2026-12-31 22:00:00   Set the local date and time (and the RTC, if
//                           there is one).
//   R 6400 60               Rehearse: pretend midnight is 6400 seconds away
//                           and run 60 times faster than real time. Both
//                           numbers are optional; the defaults are the full
//                           countdown at 60x, which takes under 2 minutes.
//   R 15 1                  Rehearse just the last 15 seconds in real time.
//   N                       Stop rehearsing and go back to the real time.
//   S                       Print the status.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Animator.h"
#include "Canvas.h"
#include "CountdownClock.h"
#include "NewYearShow.h"
#include "Strip.h"
#include "ds3231.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "status_lights.hpp"

#define NUM_STRIPS 32
// The universal-strip-controller's LED outputs are GPIO0-31.
#define START_PIN 0
#define STRIP_LEN 200
#define CANVAS_WIDTH 100
#define BRIGHTNESS 32
#define FPS 30

//
// Local time is UTC plus this. US Eastern standard time (-5 hours) is a
// placeholder: set it to wherever the tree is. It's the offset in effect on
// December 31st, so no daylight saving.
#define UTC_OFFSET_MINUTES (-5 * 60)

#define RTC_I2C i2c1
#define RTC_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define RTC_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN
#define RTC_BAUDRATE (100 * 1000)

//
// How often to re-read the RTC, since it keeps better time than the Pico.
#define RTC_RESYNC_MS (10 * 60 * 1000)

static CountdownClock countdownClock;
static Ds3231 *rtc = nullptr;

/// @brief Reads the RTC and sets the countdown clock from it, in local time.
static bool syncFromRtc() {
    if (rtc == nullptr) {
        return false;
    }
    auto dt = rtc->getDatetime();
    if (!dt) {
        printf("Couldn't read the RTC\n");
        return false;
    }
    int64_t local = CountdownClock::toEpoch(dt->year, dt->month, dt->day,
                                            dt->hour, dt->min, dt->sec) +
                    UTC_OFFSET_MINUTES * 60;
    int y, mo, d, h, mi, s;
    CountdownClock::fromEpoch(local, y, mo, d, h, mi, s);
    countdownClock.setTime(y, mo, d, h, mi, s);
    return true;
}

/// @brief Writes a local time to the RTC, as UTC.
static void writeRtc(int y, int mo, int d, int h, int mi, int s) {
    if (rtc == nullptr) {
        return;
    }
    int64_t utc = CountdownClock::toEpoch(y, mo, d, h, mi, s) -
                  UTC_OFFSET_MINUTES * 60;
    datetime_t dt;
    int year, month, day, hour, min, sec;
    CountdownClock::fromEpoch(utc, year, month, day, hour, min, sec);
    dt.year = year;
    dt.month = month;
    dt.day = day;
    dt.hour = hour;
    dt.min = min;
    dt.sec = sec;
    dt.dotw = 0;
    if (!rtc->setDatetime(dt)) {
        printf("Couldn't write the RTC\n");
    }
}

static void printStatus(NewYearShow &show) {
    if (!countdownClock.hasTime()) {
        printf("No time set. Use T YYYY-MM-DD HH:MM:SS, or R to rehearse.\n");
        return;
    }
    int y, mo, d, h, mi, s;
    countdownClock.getTime(y, mo, d, h, mi, s);
    printf("%s %04d-%02d-%02d %02d:%02d:%02d, %.1f s to midnight (%d), %s\n",
           countdownClock.isRehearsing() ? "Rehearsing" : "Local time", y, mo,
           d, h, mi, s, countdownClock.secondsToMidnight(),
           countdownClock.getNewYear(),
           NewYearShow::phaseName(show.getPhase()));
}

static void handleCommand(char *line, NewYearShow &show) {
    switch (line[0]) {
        case 'T':
        case 't': {
            int y, mo, d, h, mi, s;
            if (sscanf(line + 1, "%d-%d-%d %d:%d:%d", &y, &mo, &d, &h, &mi,
                       &s) == 6) {
                countdownClock.setTime(y, mo, d, h, mi, s);
                writeRtc(y, mo, d, h, mi, s);
            } else {
                printf("Use T YYYY-MM-DD HH:MM:SS\n");
            }
            break;
        }
        case 'R':
        case 'r': {
            float secondsToGo = show.getCountdownSeconds() + 10;
            float speed = 60;
            sscanf(line + 1, "%f %f", &secondsToGo, &speed);
            countdownClock.rehearse(secondsToGo, speed, 2027);
            break;
        }
        case 'N':
        case 'n':
            if (!syncFromRtc()) {
                printf("No RTC to go back to; use T to set the time.\n");
            }
            break;
        case 'S':
        case 's':
            break;
        default:
            printf("Commands: T YYYY-MM-DD HH:MM:SS, R [seconds [speed]], N, "
                   "S\n");
            return;
    }
    printStatus(show);
}

/// @brief Collects characters from serial into a line, without blocking.
/// Returns true when a whole line has arrived.
static bool readLine(char *buf, int size, int &len) {
    while (true) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) {
            return false;
        }
        if (c == '\r' || c == '\n') {
            if (len == 0) {
                continue;
            }
            buf[len] = 0;
            len = 0;
            return true;
        }
        if (len < size - 1) {
            buf[len++] = (char)c;
        }
    }
}

int main() {
    stdio_init_all();

    StatusLights lights(PICO_DEFAULT_WS2812_PIN, USC_STATUS_LIGHT_COUNT);
    lights.begin();

    //
    // The RTC.
    i2c_init(RTC_I2C, RTC_BAUDRATE);
    gpio_set_function(RTC_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(RTC_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(RTC_SDA_PIN);
    gpio_pull_up(RTC_SCL_PIN);
    static Ds3231 ds3231(RTC_I2C);
    if (ds3231.init()) {
        rtc = &ds3231;
        if (ds3231.lostPower()) {
            printf("The RTC lost power, so its time can't be trusted. Set it "
                   "with T.\n");
        } else {
            syncFromRtc();
        }
    } else {
        printf("No RTC on the bus. Set the time with T, or rehearse with R.\n");
    }

    //
    // Status light 0: green if we know the time, red if we don't.
    lights.set(0, countdownClock.hasTime() ? RGB::Green : RGB::Red);
    lights.update();

    //
    // A canvas made out of strips.
    Strip *strips[NUM_STRIPS];
    Canvas canvas(CANVAS_WIDTH);
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i] = new Strip(START_PIN + i, STRIP_LEN);
        strips[i]->setColorOrder(ColorOrder::ORGB);
        canvas.add(strips[i]);
    }
    canvas.setBrightness(BRIGHTNESS);
    canvas.setDithering(true);
    canvas.setup();
    canvas.clear();
    canvas.show();

    NewYearShow show(&canvas, &countdownClock);
    Animator animator(&canvas, FPS);
    animator.add(&show);
    animator.init();
    printStatus(show);

    char line[64];
    int len = 0;
    uint32_t lastSync = to_ms_since_boot(get_absolute_time());
    while (true) {
        animator.step();

        if (readLine(line, sizeof(line), len)) {
            handleCommand(line, show);
            lights.set(0, countdownClock.hasTime() ? RGB::Green : RGB::Red);
            lights.update();
        }

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - lastSync > RTC_RESYNC_MS) {
            lastSync = now;
            if (!countdownClock.isRehearsing()) {
                syncFromRtc();
            }
        }
    }
}
