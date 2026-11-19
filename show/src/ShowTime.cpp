#include "ShowTime.h"

#include <stdio.h>
#include <string.h>

#include "ShowSchedule.h"
#include "ds3231.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/critical_section.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#if SHOW_WIFI
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "pico/cyw43_arch.h"
#endif

//
// Where the DS3231 is. The LED processor board's is on its default I2C
// (I2C0, GPIO32/33); Steve's Pico 2 W perfboard has an MH mini DS3231 module
// on I2C1, GPIO26/27.
#ifndef SHOW_RTC_I2C
#if defined(LED_PROCESSOR_BOARD) || defined(UNIVERSAL_STRIP_CONTROLLER)
#define SHOW_RTC_I2C PICO_DEFAULT_I2C
#define SHOW_RTC_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define SHOW_RTC_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN
#else
#define SHOW_RTC_I2C 1
#define SHOW_RTC_SDA_PIN 26
#define SHOW_RTC_SCL_PIN 27
#endif
#endif

namespace show {

//
// How often to do things, in microseconds.
static constexpr uint64_t kRtcReadEveryUs = 10ull * 60 * 1000000;
static constexpr uint64_t kNtpEveryUs = 4ull * 3600 * 1000000;
// With no RTC to carry the time between syncs, sync more often.
static constexpr uint64_t kNtpEveryNoRtcUs = 30ull * 60 * 1000000;
static constexpr uint64_t kNtpRetryUs = 5ull * 60 * 1000000;
static constexpr uint64_t kWifiRetryUs = 60ull * 1000000;

static constexpr uint32_t kReady = 0x5157AB1E;

const char* sourceName(TimeSource s) {
    switch (s) {
        case TimeSource::RTC:
            return "RTC";
        case TimeSource::NTP:
            return "NTP";
        default:
            return "none";
    }
}

static critical_section_t lock;
static TimeStatus shared;
static volatile bool ntpWanted = false;

TimeStatus TimeService::status() {
    critical_section_enter_blocking(&lock);
    TimeStatus s = shared;
    critical_section_exit(&lock);
    return s;
}

void TimeService::requestNtp() { ntpWanted = true; }

/// @brief Changes the shared status under the lock.
template <typename F>
static void update(F f) {
    critical_section_enter_blocking(&lock);
    f(shared);
    critical_section_exit(&lock);
}

/// @brief Sets the time: the time since 1970 is time_us_64() plus offsetUs.
/// Returns how far that moved it.
static int64_t publish(int64_t offsetUs, TimeSource source) {
    int64_t step = 0;
    update([&](TimeStatus& s) {
        step = s.source == TimeSource::NONE ? 0 : offsetUs - s.offsetUs;
        s.offsetUs = offsetUs;
        s.source = source;
        s.syncedAtUs = time_us_64();
    });
    return step;
}

static int64_t currentOffset() {
    int64_t o;
    update([&](TimeStatus& s) { o = s.offsetUs; });
    return o;
}

//
// The RTC.

static Ds3231* rtc = nullptr;

static bool rtcStart() {
    i2c_init(I2C_INSTANCE(SHOW_RTC_I2C), 100 * 1000);
    gpio_set_function(SHOW_RTC_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SHOW_RTC_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SHOW_RTC_SDA_PIN);
    gpio_pull_up(SHOW_RTC_SCL_PIN);
    static Ds3231 chip(I2C_INSTANCE(SHOW_RTC_I2C));
    if (!chip.init()) {
        printf("Time: no DS3231 on I2C%d (GPIO%d/%d)\n", SHOW_RTC_I2C,
               SHOW_RTC_SDA_PIN, SHOW_RTC_SCL_PIN);
        return false;
    }
    rtc = &chip;
    if (rtc->lostPower()) {
        printf("Time: the DS3231 lost power, so its time can't be trusted "
               "until NTP sets it\n");
        return false;
    }
    return true;
}

static int64_t unixSeconds(const datetime_t& dt) {
    return toUnixSeconds(dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
}

/// @brief Reads the RTC right as its seconds tick over, so we know the time
/// to about a millisecond rather than to the second. Takes up to a second.
/// @param offsetUs set to the time since 1970 minus time_us_64()
static bool rtcRead(int64_t& offsetUs) {
    if (rtc == nullptr) {
        return false;
    }
    auto first = rtc->getDatetime();
    if (!first) {
        return false;
    }
    //
    // The DS3231 copies its counters into the registers we read at the start
    // of each read, so the tick came between the start of the last read that
    // showed the old second and the start of the first that shows the new
    // one. Reads back to back are under a millisecond apart.
    uint64_t prevStart = time_us_64();
    uint64_t deadline = prevStart + 1200000;
    while (time_us_64() < deadline) {
        uint64_t start = time_us_64();
        auto dt = rtc->getDatetime();
        if (!dt) {
            return false;
        }
        if (dt->sec != first->sec) {
            uint64_t tick = (prevStart + start) / 2;
            offsetUs = unixSeconds(*dt) * kUsPerSecond - (int64_t)tick;
            return true;
        }
        prevStart = start;
    }
    printf("Time: the DS3231's seconds aren't ticking\n");
    return false;
}

/// @brief Sets the RTC from our time. Waits for the start of the next second
/// and writes it then: writing the seconds restarts the DS3231's count
/// within the second, so it ticks in step with us from then on.
static bool rtcWrite(int64_t offsetUs) {
    if (rtc == nullptr) {
        return false;
    }
    int64_t now = (int64_t)time_us_64() + offsetUs;
    int64_t next = (now / kUsPerSecond + 1) * kUsPerSecond;
    while ((int64_t)time_us_64() + offsetUs < next) {
        tight_loop_contents();
    }
    int y, mo, d, h, mi, s;
    localTime(next, 0, y, mo, d, h, mi, s);
    datetime_t dt = {};
    dt.year = y;
    dt.month = mo;
    dt.day = d;
    dt.hour = h;
    dt.min = mi;
    dt.sec = s;
    dt.dotw = 0;
    return rtc->setDatetime(dt);
}

//
// NTP and the WiFi.

#if SHOW_WIFI

static constexpr char kNtpServer[] = "pool.ntp.org";
static constexpr uint16_t kNtpPort = 123;
static constexpr uint32_t kNtpDelta = 2208988800u;
static constexpr int kNtpTries = 4;
static constexpr int64_t kNtpMaxRttUs = 300000;

struct NtpReply {
    volatile bool got;
    uint64_t at;
    uint8_t msg[48];
};

static void ntpRecv(void* arg, udp_pcb*, pbuf* p, const ip_addr_t*,
                    u16_t port) {
    uint64_t at = time_us_64();
    NtpReply* r = (NtpReply*)arg;
    if (!r->got && port == kNtpPort && p->tot_len >= 48) {
        pbuf_copy_partial(p, r->msg, 48, 0);
        r->at = at;
        r->got = true;
    }
    pbuf_free(p);
}

struct DnsResult {
    volatile bool done;
    volatile bool ok;
    ip_addr_t addr;
};

static void dnsFound(const char*, const ip_addr_t* ip, void* arg) {
    DnsResult* r = (DnsResult*)arg;
    if (ip != nullptr) {
        r->addr = *ip;
        r->ok = true;
    }
    r->done = true;
}

/// @brief Polls the radio until done is true or timeoutUs passes.
static bool pollUntil(volatile bool& done, uint64_t timeoutUs) {
    uint64_t deadline = time_us_64() + timeoutUs;
    while (!done && time_us_64() < deadline) {
        cyw43_arch_poll();
        tight_loop_contents();
    }
    return done;
}

static int64_t ntpTimeUs(const uint8_t* b) {
    uint32_t sec = (uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 |
                   (uint32_t)b[2] << 8 | b[3];
    uint32_t frac = (uint32_t)b[4] << 24 | (uint32_t)b[5] << 16 |
                    (uint32_t)b[6] << 8 | b[7];
    return ((int64_t)sec - kNtpDelta) * kUsPerSecond +
           (int64_t)(((uint64_t)frac * 1000000) >> 32);
}

/// @brief Asks the time server a few times and keeps the answer with the
/// shortest round trip, which is the one we can trust most.
static bool ntpSync(int64_t& offsetUs, int64_t& rttUs) {
    DnsResult dns = {};
    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(kNtpServer, &dns.addr, dnsFound, &dns);
    cyw43_arch_lwip_end();
    if (err == ERR_OK) {
        dns.ok = dns.done = true;
    } else if (err != ERR_INPROGRESS || !pollUntil(dns.done, 5000000) ||
               !dns.ok) {
        printf("Time: couldn't look up %s\n", kNtpServer);
        return false;
    }

    udp_pcb* pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb == nullptr) {
        return false;
    }
    NtpReply reply;
    udp_recv(pcb, ntpRecv, &reply);
    bool good = false;
    rttUs = INT64_MAX;
    for (int i = 0; i < kNtpTries; i++) {
        reply.got = false;
        pbuf* p = pbuf_alloc(PBUF_TRANSPORT, 48, PBUF_RAM);
        if (p == nullptr) {
            break;
        }
        uint8_t* req = (uint8_t*)p->payload;
        memset(req, 0, 48);
        req[0] = 0x23;  // No leap warning, version 4, client.
        cyw43_arch_lwip_begin();
        uint64_t sent = time_us_64();
        udp_sendto(pcb, p, &dns.addr, kNtpPort);
        cyw43_arch_lwip_end();
        pbuf_free(p);
        if (!pollUntil(reply.got, 1000000)) {
            continue;
        }
        const uint8_t* m = reply.msg;
        if ((m[0] & 0x07) != 4 || m[1] == 0) {
            continue;  // Not a server reply, or a kiss-of-death.
        }
        int64_t serverRx = ntpTimeUs(m + 32);
        int64_t serverTx = ntpTimeUs(m + 40);
        int64_t rtt = (int64_t)(reply.at - sent) - (serverTx - serverRx);
        if (rtt < 0 || rtt > kNtpMaxRttUs || rtt >= rttUs) {
            continue;
        }
        //
        // The usual NTP sum: assume the trip out took as long as the trip
        // back.
        rttUs = rtt;
        offsetUs = ((serverRx - (int64_t)sent) + (serverTx - (int64_t)reply.at)) / 2;
        good = true;
        sleep_ms(50);
    }
    udp_remove(pcb);
    if (good && offsetUs + (int64_t)time_us_64() <
                    toUnixSeconds(2026, 1, 1, 0, 0, 0) * kUsPerSecond) {
        printf("Time: NTP gave a time before 2026; ignoring it\n");
        good = false;
    }
    return good;
}

static bool wifiUp() {
    return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) ==
           CYW43_LINK_UP;
}

static bool wifiConnect() {
    printf("Time: joining WiFi %s\n", SHOW_WIFI_SSID);
    int r = cyw43_arch_wifi_connect_timeout_ms(
        SHOW_WIFI_SSID, SHOW_WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000);
    if (r != 0) {
        printf("Time: couldn't join WiFi (%d); trying again in a minute\n", r);
        return false;
    }
    printf("Time: on WiFi\n");
    return true;
}

static bool radioStart() {
    //
    // The RM2 sometimes needs a second go (see the processor board's
    // bring-up notes).
    for (int i = 0; i < 3; i++) {
        if (cyw43_arch_init() == 0) {
            cyw43_arch_enable_sta_mode();
            //
            // Power saving makes the radio slow to answer, which spoils NTP's
            // round trip times.
            cyw43_wifi_pm(&cyw43_state, CYW43_NONE_PM);
            return true;
        }
        printf("Time: the radio didn't start, trying again\n");
        sleep_ms(500);
    }
    printf("Time: no radio, so no NTP; going by the RTC alone\n");
    return false;
}

#endif  // SHOW_WIFI

static void doRtcRead() {
    int64_t offset;
    if (!rtcRead(offset)) {
        printf("Time: couldn't read the DS3231\n");
        return;
    }
    int64_t step = publish(offset, TimeSource::RTC);
    update([&](TimeStatus& s) { s.rtcStepUs = step; });
}

static void core1Main() {
    bool radio = false;
#if SHOW_WIFI
    radio = radioStart();
#endif
    update([&](TimeStatus& s) { s.radioOk = radio; });
    //
    // The radio has its PIO state machine; core 0 can claim the rest.
    multicore_fifo_push_blocking(kReady);

    bool rtcOk = rtcStart();
    update([&](TimeStatus& s) { s.rtcOk = rtcOk; });
    if (rtcOk) {
        doRtcRead();
        int y, mo, d, h, mi, s;
        localTime((int64_t)time_us_64() + currentOffset(), 0, y, mo, d, h, mi,
                  s);
        printf("Time: DS3231 says %04d-%02d-%02d %02d:%02d:%02d UTC\n", y, mo,
               d, h, mi, s);
    }

    uint64_t nextRtc = time_us_64() + kRtcReadEveryUs;
    uint64_t nextNtp = 0;
    uint64_t nextWifi = 0;
    while (true) {
        uint64_t now = time_us_64();
#if SHOW_WIFI
        if (radio) {
            cyw43_arch_poll();
            bool up = wifiUp();
            if (!up && now >= nextWifi) {
                up = wifiConnect();
                nextWifi = time_us_64() + kWifiRetryUs;
            }
            update([&](TimeStatus& s) { s.wifiUp = up; });
            if (up && (now >= nextNtp || ntpWanted)) {
                ntpWanted = false;
                int64_t offset, rtt;
                if (ntpSync(offset, rtt)) {
                    int64_t step = publish(offset, TimeSource::NTP);
                    update([&](TimeStatus& s) {
                        s.ntpGood++;
                        s.ntpAtUs = time_us_64();
                        s.ntpRttUs = rtt;
                        s.ntpStepUs = step;
                    });
                    printf("Time: NTP moved the clock %+.1f ms (round trip "
                           "%.1f ms)\n",
                           step / 1000.0, rtt / 1000.0);
                    if (rtcWrite(offset)) {
                        rtcOk = true;
                        update([&](TimeStatus& s) { s.rtcOk = true; });
                    }
                    nextNtp = time_us_64() + (rtcOk ? kNtpEveryUs : kNtpEveryNoRtcUs);
                } else {
                    update([&](TimeStatus& s) { s.ntpBad++; });
                    printf("Time: NTP failed; trying again in 5 minutes\n");
                    nextNtp = time_us_64() + kNtpRetryUs;
                }
            }
        }
#endif
        if (rtcOk && now >= nextRtc) {
            nextRtc = now + kRtcReadEveryUs;
            doRtcRead();
        }
#if SHOW_WIFI
        if (radio) {
            cyw43_arch_wait_for_work_until(make_timeout_time_ms(50));
            continue;
        }
#endif
        sleep_ms(50);
    }
}

void TimeService::start() {
    critical_section_init(&lock);
    multicore_launch_core1(core1Main);
    while (multicore_fifo_pop_blocking() != kReady) {
    }
}

//
// Core 0's clock.

bool ShowClock::valid() { return TimeService::status().source != TimeSource::NONE; }

int64_t ShowClock::nowUs() {
    TimeStatus s = TimeService::status();
    uint64_t t = time_us_64();
    if (s.source == TimeSource::NONE) {
        return (int64_t)t + testOffsetUs;
    }
    int64_t diff = s.offsetUs - appliedUs;
    if (!have || diff > 500000 || diff < -500000) {
        appliedUs = s.offsetUs;
        have = true;
        lastUs = t;
    } else {
        //
        // Ease towards it by up to 1 us for every ms that's passed (1 ms a
        // second).
        int64_t most = (int64_t)(t - lastUs) / 1000;
        if (most > 0) {
            appliedUs += diff > most ? most : (diff < -most ? -most : diff);
            lastUs += most * 1000;
        }
    }
    return (int64_t)t + appliedUs + testOffsetUs;
}

}  // namespace show
