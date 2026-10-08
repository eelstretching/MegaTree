#include "CountdownClock.h"

#include "pico/stdlib.h"

//
// Days between 1970-01-01 and the given date in the proleptic Gregorian
// calendar, from Howard Hinnant's "days_from_civil".
static int64_t daysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int64_t)doe - 719468;
}

//
// The inverse of daysFromCivil, Hinnant's "civil_from_days".
static void civilFromDays(int64_t z, int &y, int &m, int &d) {
    z += 719468;
    const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = (unsigned)(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp < 10 ? mp + 3 : mp - 9;
    y = (int)(yoe + era * 400) + (m <= 2);
}

int64_t CountdownClock::toEpoch(int year, int month, int day, int hour,
                                int min, int sec) {
    return daysFromCivil(year, month, day) * 86400 + hour * 3600 + min * 60 +
           sec;
}

void CountdownClock::setTime(int year, int month, int day, int hour, int min,
                             int sec) {
    baseEpoch = toEpoch(year, month, day, hour, min, sec);
    baseUs = time_us_64();
    speed = 1;
    rehearsing = false;
    newYear = (month == 1 && day == 1) ? year : year + 1;
    midnightEpoch = toEpoch(newYear, 1, 1, 0, 0, 0);
}

void CountdownClock::rehearse(float secondsToGo, float speed, int year) {
    //
    // The date doesn't matter for a rehearsal, so we'll use one that's
    // obviously not real.
    midnightEpoch = toEpoch(2000, 1, 1, 0, 0, 0);
    baseEpoch = midnightEpoch - (int64_t)secondsToGo;
    baseUs = time_us_64();
    this->speed = speed;
    newYear = year;
    rehearsing = true;
}

double CountdownClock::now() {
    if (baseEpoch < 0) {
        return 0;
    }
    return baseEpoch + (time_us_64() - baseUs) * speed / 1e6;
}

double CountdownClock::secondsToMidnight() {
    if (baseEpoch < 0) {
        return 1e9;
    }
    return midnightEpoch - now();
}

void CountdownClock::fromEpoch(int64_t t, int &year, int &month, int &day,
                               int &hour, int &min, int &sec) {
    int64_t days = t / 86400;
    int64_t secs = t % 86400;
    if (secs < 0) {
        secs += 86400;
        days--;
    }
    civilFromDays(days, year, month, day);
    hour = secs / 3600;
    min = (secs / 60) % 60;
    sec = secs % 60;
}

void CountdownClock::getTime(int &year, int &month, int &day, int &hour,
                             int &min, int &sec) {
    fromEpoch((int64_t)now(), year, month, day, hour, min, sec);
}
