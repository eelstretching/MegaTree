# The coordinated show

Every prop runs the same schedule and works out what to show from the time of
day, so the props change scene together without talking to each other.
PicoComm can take over the clock later without changing the schedule.

## The schedule

`include/ShowSchedule.h` holds it. The show runs from 4:30 to 10:30 pm Eastern
(UTC-5 all season, no daylight saving to worry about). It's a 20 minute loop
of scenes, counted from local midnight, so Hearth starts on the hour, at :20
and at :40. Lights on at 4:30 lands at the start of Icicles, and lights off at
10:30 at the end of Classic.

| Starts (min past :00, :20, :40) | Scene | Length | MegaTree | Topper | Arches | Mini trees |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | Hearth | 6 min | Starlight | TopperStarlight | ArchFire | TwinkleFox |
| 6 | Classic | 4 min | ClassicLights multicolor slo-glo | slow RingChase | ArchGlow, warm white | ClassicLights multicolor twinkle |
| 10 | Icicles | 3 min | Icicles | white ColorWipe | FillWipe, white and ice blue | Icicles |
| 13 | Leap | 2 min | red and green spirals | Pinwheel | Leap, arch 1 to 6 across both controllers | red and green RotatingColumns |
| 15 | Candy cane | 3 min | red and white Marquees | Peppermint | ArchRise, red and white | red and white Marquees |
| 18 | Greetings | 2 min | ScrollTexts | gold PointChase | ArchGlow, red and green | red and green TwinkleFox |

Every scene fades in from black and out to black over 0.75 s, which hides
small differences between the props' clocks. Effects that go by the clock
(Starlight, Leap, ArchGlow, ArchFire...) start at the moment their scene
started, as every prop sees it, so they're in the same place on every prop
and pick up where they should after a reboot.

To change the show, change `kScenes` (lengths have to add up to a whole
number of loops a day) and what each program `set()`s for each scene.

## Keeping time

Core 1 does it (`src/ShowTime.cpp`):

- At start-up it reads the DS3231 right as its seconds tick over, which gets
  the time to about a millisecond instead of to the second.
- With WiFi, it asks `pool.ntp.org` at start-up and every 4 hours (best of 4
  tries), and sets the DS3231 from that, so the DS3231 is right for the next
  boot even if the internet goes away. Without a DS3231 it asks every 30
  minutes.
- It re-reads the DS3231 every 10 minutes, since the DS3231 keeps better time
  than the Pico's own crystal.

Core 0 eases into each correction at up to 1 ms a second, so nothing
stutters; it only jumps for corrections over half a second. Until it knows
the time, a prop stays dark.

The radio has to claim its PIO state machine before the LEDs claim theirs, so
`TimeService::start()` comes before `setup()` on the canvas, arches or topper.

## WiFi

Put the show network in `show/wifi_secrets.cmake` (it's gitignored):

```cmake
set(SHOW_WIFI_SSID "myssid")
set(SHOW_WIFI_PASSWORD "mypassword")
```

or pass `-DSHOW_WIFI_SSID=... -DSHOW_WIFI_PASSWORD=...` to cmake. With no SSID
the props build without the network and go by their DS3231s alone.

## Building and flashing

```sh
cmake --preset processor-board      # or the usual cmake line with -DPICO_BOARD=led_processor_board
ninja -C build-pb ShowTree ShowArches1 ShowArches2
picotool load -f -x build-pb/ShowTree.uf2
```

| Firmware | Prop |
| --- | --- |
| `ShowTree.uf2` | MegaTree (GP0-31) and star topper (GP35, J2 pin 2) |
| `ShowArches1.uf2` | Arches 1-3, the left group seen from the street |
| `ShowArches2.uf2` | Arches 4-6, the right group |
| `ShowMiniTree.uf2` | Either mini tree (13 strands on GP0-12) |

Arch wiring is LeapingArches': two strands per arch on neighbouring outputs.
`ShowArches*` also build for a Pico 2 W (outputs from GP2, DS3231 on
GP26/27). In `ShowMiniTree` each folded 100-pixel strand is two 50-pixel columns, a
50 x 26 canvas. It also builds for a Pico 2 W (strands on GP2-14), where with
no DS3231 it needs WiFi: it stays dark until NTP gives it the time.

## Testing

Over USB serial (`tio /dev/cu.usbmodem...`), one command per line:

| Command | Does |
| --- | --- |
| `S` | Status: local time, scene, fade level, where the time came from, the last NTP and RTC corrections |
| `A` | Run all the time, ignoring show hours, for daytime testing. Still in step with the other props. |
| `G n` | Jump to scene n (1-6) now. Puts this prop out of step until `N`. |
| `N` | Back to normal |
| `T` | Ask for NTP now |

Each prop also prints a status line at every scene change and once a minute.
The "NTP moved the clock" figure is roughly how far the DS3231 drifted since
the last sync.

To check sync, run `A` on two props and watch the Leap scene (at :13, :33 and
:53): the ball should go straight from arch 3 to arch 4.

The schedule has tests that run on a computer:

```sh
c++ -std=c++17 -Wall -Wextra -Ishow/include show/src/ShowSchedule.cpp show/test/test_schedule.cpp -o /tmp/test_schedule
/tmp/test_schedule
```
