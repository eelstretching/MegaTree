// Checks the show schedule on a computer. Build and run from the repo root:
//
//   c++ -std=c++17 -Wall -Wextra -Ishow/include show/src/ShowSchedule.cpp
//       show/test/test_schedule.cpp -o /tmp/test_schedule
//   /tmp/test_schedule
//
#include <stdio.h>
#include <stdlib.h>

#include "ShowSchedule.h"

using namespace show;

static int failures = 0;

#define CHECK(cond)                                                  \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("%s:%d: FAILED: %s\n", __FILE__, __LINE__, #cond); \
            failures++;                                              \
        }                                                            \
    } while (0)

/// @brief Microseconds since 1970 for a local (US Eastern standard) time.
static int64_t eastern(int y, int mo, int d, int h, int mi, int s,
                       int64_t us = 0) {
    return (toUnixSeconds(y, mo, d, h, mi, s) + 5 * 3600) * kUsPerSecond + us;
}

int main() {
    CHECK(kLoopSeconds == 20 * 60);

    // Round trip through the date helpers.
    {
        int y, mo, d, h, mi, s;
        localTime(eastern(2026, 12, 24, 19, 41, 7), -300, y, mo, d, h, mi, s);
        CHECK(y == 2026 && mo == 12 && d == 24 && h == 19 && mi == 41 &&
              s == 7);
        CHECK(toUnixSeconds(1970, 1, 1, 0, 0, 0) == 0);
        CHECK(toUnixSeconds(2026, 11, 12, 0, 0, 0) == 1794441600);
    }

    // Dark before 4:30 pm, and at 10:30 pm.
    CHECK(!locate(eastern(2026, 12, 1, 16, 29, 59)).on);
    CHECK(locate(eastern(2026, 12, 1, 16, 29, 59)).level == 0);
    CHECK(!locate(eastern(2026, 12, 1, 22, 30, 0)).on);
    CHECK(!locate(eastern(2026, 12, 1, 3, 0, 0)).on);

    // 4:30 pm is ten minutes into a loop, at the start of Icicles, fading in
    // from black.
    {
        Position p = locate(eastern(2026, 12, 1, 16, 30, 0));
        CHECK(p.on);
        CHECK(p.scene == ICICLES);
        CHECK(p.usIntoScene == 0);
        CHECK(p.level == 0);
        p = locate(eastern(2026, 12, 1, 16, 30, 0, kFadeUs / 2));
        CHECK(p.level > 100 && p.level < 160);
        p = locate(eastern(2026, 12, 1, 16, 30, 1));
        CHECK(p.level == 255);
    }

    // The scenes in order through a loop, starting on the hour.
    {
        struct {
            int minute;
            int second;
            SceneId scene;
        } expect[] = {
            {0, 1, HEARTH},  {5, 59, HEARTH},     {6, 0, CLASSIC},
            {9, 59, CLASSIC}, {10, 0, ICICLES},   {13, 0, LEAP},
            {14, 59, LEAP},   {15, 0, CANDY_CANE}, {18, 0, GREETINGS},
            {19, 59, GREETINGS}, {20, 0, HEARTH}, {40, 30, HEARTH},
            {53, 30, LEAP},
        };
        for (auto& e : expect) {
            Position p = locate(eastern(2026, 12, 1, 19, e.minute, e.second));
            CHECK(p.on);
            CHECK(p.scene == e.scene);
        }
    }

    // Every prop gets the same scene start, wherever it is in the scene.
    {
        int64_t start = eastern(2026, 12, 1, 19, 13, 0);
        CHECK(locate(start).sceneStartUs == start);
        CHECK(locate(start + 12345678).sceneStartUs == start);
        CHECK(locate(start + 119 * kUsPerSecond).sceneStartUs == start);
        CHECK(locate(start + 12345678).usIntoScene == 12345678);
    }

    // Fade out at the end of a scene, all the way to black at the cut.
    {
        int64_t cut = eastern(2026, 12, 1, 19, 6, 0);
        CHECK(locate(cut - 1).level == 0);
        CHECK(locate(cut - kFadeUs / 2).level > 100);
        CHECK(locate(cut - kFadeUs - 1).level == 255);
        CHECK(locate(cut).level == 0);
    }

    // Classic fades out into lights off at 10:30.
    {
        Position p = locate(eastern(2026, 12, 1, 22, 29, 59, 900000));
        CHECK(p.on);
        CHECK(p.scene == CLASSIC);
        CHECK(p.level < 50);
    }

    // Show hours that aren't on a scene boundary still fade.
    {
        ShowHours h;
        h.startMinute = 17 * 60 + 3;  // 5:03 pm, halfway through Hearth
        Position p = locate(eastern(2026, 12, 1, 17, 3, 0, 100000), h);
        CHECK(p.on);
        CHECK(p.scene == HEARTH);
        CHECK(p.level < 50);
        CHECK(locate(eastern(2026, 12, 1, 17, 3, 1), h).level == 255);
    }

    // Shows that run past midnight.
    {
        ShowHours h;
        h.startMinute = 22 * 60;
        h.endMinute = 1 * 60;
        CHECK(locate(eastern(2026, 12, 31, 23, 30, 0), h).on);
        CHECK(locate(eastern(2027, 1, 1, 0, 30, 0), h).on);
        CHECK(!locate(eastern(2027, 1, 1, 1, 0, 0), h).on);
    }

    // Always on runs in the daytime.
    {
        Position p = locate(eastern(2026, 12, 1, 11, 0, 30), ShowHours(), true);
        CHECK(p.on);
        CHECK(p.scene == HEARTH);
        CHECK(p.level == 255);
    }

    if (failures == 0) {
        printf("All schedule tests passed\n");
        return 0;
    }
    printf("%d failures\n", failures);
    return 1;
}
