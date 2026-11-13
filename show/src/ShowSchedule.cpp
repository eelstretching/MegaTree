#include "ShowSchedule.h"

namespace show {

/// @brief A fade level for a point that's us microseconds from the nearest
/// cut to black.
static uint8_t fadeLevel(int64_t us) {
    if (us <= 0) {
        return 0;
    }
    if (us >= kFadeUs) {
        return 255;
    }
    return (uint8_t)(us * 255 / kFadeUs);
}

static uint8_t minLevel(uint8_t a, uint8_t b) { return a < b ? a : b; }

Position locate(int64_t unixUs, const ShowHours& hours, bool alwaysOn) {
    int64_t localUs = unixUs + (int64_t)hours.utcOffsetMinutes * 60 * kUsPerSecond;

    //
    // Which scene: the loops start at local midnight and go round all day.
    int64_t loopUs = floorMod(localUs, (int64_t)kLoopSeconds * kUsPerSecond);
    Position pos;
    int64_t sceneOffsetUs = 0;
    pos.scene = kScenes[NUM_SCENES - 1].id;
    for (int i = 0; i < NUM_SCENES; i++) {
        int64_t len = (int64_t)kScenes[i].seconds * kUsPerSecond;
        if (loopUs < sceneOffsetUs + len) {
            pos.scene = kScenes[i].id;
            break;
        }
        sceneOffsetUs += len;
    }
    int64_t sceneLenUs = (int64_t)kScenes[pos.scene].seconds * kUsPerSecond;
    pos.usIntoScene = loopUs - sceneOffsetUs;
    pos.sceneStartUs = unixUs - pos.usIntoScene;
    pos.level = minLevel(fadeLevel(pos.usIntoScene),
                         fadeLevel(sceneLenUs - pos.usIntoScene));

    if (alwaysOn) {
        pos.on = true;
        return pos;
    }

    //
    // Inside show hours? Also fade at lights on and lights off, in case
    // they're not at the start of a scene.
    int64_t dayUs = floorMod(localUs, kUsPerDay);
    int64_t startUs = (int64_t)hours.startMinute * 60 * kUsPerSecond;
    int64_t endUs = (int64_t)hours.endMinute * 60 * kUsPerSecond;
    int64_t showLenUs = floorMod(endUs - startUs, kUsPerDay);
    int64_t intoShowUs = floorMod(dayUs - startUs, kUsPerDay);
    pos.on = intoShowUs < showLenUs;
    if (!pos.on) {
        pos.level = 0;
        return pos;
    }
    pos.level = minLevel(pos.level, fadeLevel(intoShowUs));
    pos.level = minLevel(pos.level, fadeLevel(showLenUs - intoShowUs));
    return pos;
}

//
// Days since 1970 for a date and back, from Howard Hinnant's date
// algorithms (public domain).
static int64_t daysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    int64_t era = (y >= 0 ? y : y - 399) / 400;
    int64_t yoe = y - era * 400;
    int64_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

static void civilFromDays(int64_t z, int& y, int& m, int& d) {
    z += 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    int64_t doe = z - era * 146097;
    int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int64_t mp = (5 * doy + 2) / 153;
    d = (int)(doy - (153 * mp + 2) / 5 + 1);
    m = (int)(mp < 10 ? mp + 3 : mp - 9);
    y = (int)(yoe + era * 400 + (m <= 2));
}

int64_t toUnixSeconds(int year, int month, int day, int hour, int minute,
                      int second) {
    return daysFromCivil(year, month, day) * 86400 + hour * 3600 +
           minute * 60 + second;
}

void localTime(int64_t unixUs, int32_t utcOffsetMinutes, int& year,
               int& month, int& day, int& hour, int& minute, int& second) {
    int64_t s = unixUs / kUsPerSecond;
    if (unixUs < 0 && unixUs % kUsPerSecond != 0) {
        s--;
    }
    s += (int64_t)utcOffsetMinutes * 60;
    int64_t days = s >= 0 ? s / 86400 : (s - 86399) / 86400;
    int64_t sod = s - days * 86400;
    civilFromDays(days, year, month, day);
    hour = (int)(sod / 3600);
    minute = (int)(sod / 60 % 60);
    second = (int)(sod % 60);
}

}  // namespace show
