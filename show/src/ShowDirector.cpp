#include "ShowDirector.h"

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

namespace show {

bool ShowDirector::update(uint64_t frameUs) {
    pos = locate((int64_t)frameUs, hours, alwaysOn);
    if (!clock.valid()) {
        //
        // Until we know the time, stay dark rather than guess.
        pos.on = false;
        pos.level = 0;
    }
    bool starts = pos.on && (!wasOn || pos.scene != scene ||
                             pos.sceneStartUs != sceneStartUs);
    if (starts) {
        scene = pos.scene;
        sceneStartUs = pos.sceneStartUs;
    }
    if (starts || pos.on != wasOn) {
        printStatus();
        lastStatusUs = time_us_64();
    } else if (time_us_64() - lastStatusUs > 60 * 1000000ull) {
        printStatus();
        lastStatusUs = time_us_64();
    }
    wasOn = pos.on;
    return starts;
}

static void printDuration(int64_t us) {
    int64_t s = us / kUsPerSecond;
    printf("%d:%02d", (int)(s / 60), (int)(s % 60));
}

void ShowDirector::printStatus() {
    TimeStatus ts = TimeService::status();
    if (ts.source == TimeSource::NONE) {
        printf("Show: dark, waiting for the time (rtc %s, wifi %s)\n",
               ts.rtcOk ? "ok" : "none", ts.wifiUp ? "up" : "down");
        return;
    }
    int y, mo, d, h, mi, s;
    localTime(clock.nowUs(), hours.utcOffsetMinutes, y, mo, d, h, mi, s);
    printf("Show: %04d-%02d-%02d %02d:%02d:%02d local | ", y, mo, d, h, mi, s);
    if (pos.on) {
        printf("%s ", kScenes[pos.scene].name);
        printDuration(pos.usIntoScene);
        printf("/");
        printDuration((int64_t)kScenes[pos.scene].seconds * kUsPerSecond);
        printf(" level %d", pos.level);
    } else {
        printf("dark (on %02d:%02d to %02d:%02d)", (int)(hours.startMinute / 60),
               (int)(hours.startMinute % 60), (int)(hours.endMinute / 60),
               (int)(hours.endMinute % 60));
    }
    if (alwaysOn) {
        printf(" [always on]");
    }
    if (clock.getTestOffsetUs() != 0) {
        printf(" [OUT OF STEP: G]");
    }
    uint64_t now = time_us_64();
    printf(" | time from %s %llu s ago", sourceName(ts.source),
           (unsigned long long)((now - ts.syncedAtUs) / 1000000));
    if (ts.ntpGood > 0) {
        printf(", last NTP %llu min ago moved it %+.1f ms (rtt %.1f ms)",
               (unsigned long long)((now - ts.ntpAtUs) / 60000000),
               ts.ntpStepUs / 1000.0, ts.ntpRttUs / 1000.0);
    }
    printf(", last RTC read moved it %+.1f ms", ts.rtcStepUs / 1000.0);
    printf(" | rtc %s, wifi %s, NTP %lu ok %lu failed\n",
           ts.rtcOk ? "ok" : "NONE",
           ts.radioOk ? (ts.wifiUp ? "up" : "down") : "no radio",
           (unsigned long)ts.ntpGood, (unsigned long)ts.ntpBad);
}

void ShowDirector::printHelp() {
    printf(
        "Show commands: S status | A run all the time | G n go to scene n "
        "now (1-%d) | N normal | T sync with NTP now\n",
        NUM_SCENES);
    printf("Scenes:");
    for (int i = 0; i < NUM_SCENES; i++) {
        printf(" %d %s (%lu s)%s", i + 1, kScenes[i].name,
               (unsigned long)kScenes[i].seconds, i + 1 < NUM_SCENES ? "," : "\n");
    }
}

void ShowDirector::command(const char* cmd) {
    while (*cmd == ' ') {
        cmd++;
    }
    switch (*cmd) {
        case 'S':
        case 's':
            printStatus();
            break;
        case 'A':
        case 'a':
            alwaysOn = true;
            printf("Running all the time, ignoring the show hours\n");
            break;
        case 'G':
        case 'g': {
            int n = atoi(cmd + 1);
            if (n < 1 || n > NUM_SCENES) {
                printf("G needs a scene number from 1 to %d\n", NUM_SCENES);
                break;
            }
            //
            // Shift our clock so that scene n started a moment ago.
            clock.setTestOffsetUs(0);
            int64_t now = clock.nowUs();
            Position p = locate(now, hours, true);
            int64_t loopStart = p.sceneStartUs;
            for (int i = 0; i < p.scene; i++) {
                loopStart -= (int64_t)kScenes[i].seconds * kUsPerSecond;
            }
            int64_t want = loopStart;
            for (int i = 0; i < n - 1; i++) {
                want += (int64_t)kScenes[i].seconds * kUsPerSecond;
            }
            clock.setTestOffsetUs(want - now + kFadeUs);
            alwaysOn = true;
            printf("Going to %s (out of step with the other props until N)\n",
                   kScenes[n - 1].name);
            break;
        }
        case 'N':
        case 'n':
            clock.setTestOffsetUs(0);
            alwaysOn = false;
            printf("Back to normal\n");
            break;
        case 'T':
        case 't':
            TimeService::requestNtp();
            printf("Asking for NTP\n");
            break;
        case 0:
            break;
        default:
            printHelp();
    }
}

void ShowDirector::poll() {
    while (true) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT || c < 0) {
            return;
        }
        if (c == '\r' || c == '\n') {
            line[lineLen] = 0;
            command(line);
            lineLen = 0;
        } else if (lineLen < (int)sizeof(line) - 1 && c >= ' ' && c < 127) {
            line[lineLen++] = (char)c;
        }
    }
}

}  // namespace show
