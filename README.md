# MegaTree

The Christmas mega-tree animation from PicoLEDs' `examples/MegaTree`, as a
standalone Raspberry Pi Pico VS Code extension project that builds for the
universal-strip-controller board.

## Dependencies

Both are expected as sibling checkouts next to this one; override either
path at configure time if they live elsewhere.

| What | Default path | CMake override |
|---|---|---|
| [PicoLEDs](https://github.com/eelstretching/PicoLEDs) (animation library) | `../PicoLEDs` | `-DPICOLEDS_PATH=...` |
| universal-strip-controller firmware (board headers, `StatusLights` driver) | `../universal-strip-controller/firmware` | `-DUSC_FIRMWARE_PATH=...` |

## Board

`PICO_BOARD` defaults to `universal_strip_controller`. Use
`-DPICO_BOARD=waveshare_core2350b_prototype` for the prototype breakout.

## Changes from the PicoLEDs example

- Strips start at GPIO0 rather than GPIO2. The Pico build avoided GPIO0/1
  for UART0; on this board they're LED outputs and stdio UART is on GPIO36/37.
- The startup "I'm alive" blink uses the board's first status light via
  `StatusLights` instead of the SDK's `pico_status_led`.
- Only the MegaTree program is included. `MicroTree` and `CanvasTest` stay
  in PicoLEDs.

## Leaping arches

`arches/` holds support for leaping arches, built as the `Arches` library:

- `Arch` is one arch: a segment of a `Strip`. Several arches wired end to end
  share one strip, so with 80-pixel arches, arch 0 is pixels 0-79, arch 1 is
  80-159, and so on. Positions are in show order: pixel 0 is the left foot,
  seen from the street, whichever foot the data comes in at (`ArchFeed`).
- `ArchSet` holds all the arches (every arch the same length) and renders
  them, the way `Canvas` does for the tree. It carves each strip you `add()`
  into arches and also numbers every pixel across all arches, for effects that
  travel from one arch to the next. Arches go in the show in the order their
  strips are added, whatever their pins, and `add(strip, feed)` says which
  end each strip is fed from: `ArchFeed::LEFT_FOOT` or
  `ArchFeed::RIGHT_FOOT`. Fed from the right, each arch on the strip is
  flipped and, with several arches on it, they're taken from the far end of
  the wire first.
- `Archimation` is the 1D counterpart of PicoLEDs' `Animation`: override
  `init()` and `step()`. `MultiArchimation` and `TimedArchimation` match
  their PicoLEDs namesakes, and `ArchAnimator` runs a list of them like
  `Animator`/`RandomAnimator` (`setShuffle(true)` for random order).
  Like PicoLEDs' `Animator`, `ArchAnimator` draws at one fixed show rate (40
  fps by default) on frame boundaries of `clockUs()`, which a subclass can
  point at PicoComm's shared show clock. Archimations go by the clock
  (`getFPS()` 0, moving by `getFrameUs()`/`elapsedUs()`), so speeds and holds
  are given per second or in milliseconds and look the same at any frame
  rate. One with a non-zero `getFPS()` is stepped that many times a second.
- Starter Archimations: `Leap` (a ball with a fading tail runs up and
  over each arch in turn), `ArchRise` (arches fill from both feet to the
  apex, one by one or all together), `FillWipe` (arches fill from the feet
  with a set of colors, then black shifts in from the left foot, the right
  foot or the apex to clear them), `Cylon` (a red eye with a fading
  trail sweeps foot to foot and back on every arch, 1978 style) and
  `ArchFire` (a cozy hearth: flames flicker up both legs from the feet, and
  the fire now and then crackles and throws an ember that drifts up the leg).
- `ArchWiringCheck` is for setting up: each arch glows red, green or blue
  for its place in the show, its show-start foot is white, and a dot runs
  from that foot to the other. From the street the colors should run red,
  green, blue from the left with every white foot on the left.

The `LeapingArches` program runs them on a group of three 80-pixel arches with
the controller in the middle and one output per arch (GP2-GP4, the left arch
fed from its right foot). Edit the `archWiring` table at the top to match how
a group is wired, and set `ARCH_WIRING_CHECK` to 1 to check it in the yard.

## Star topper

`topper/` holds support for a star tree topper, built as the `Topper` library.
It's laid out for HolidayCoro's [24" 3 row star](https://www.holidaycoro.com/product-p/123-24.htm)
(123-24): 90 nodes in three star-shaped rings of 20, 30 and 40, wired
innermost ring first on one output.

- `Topper` is the star: a run of pixels on a `Strip`, with its own
  `Renderer` like `ArchSet`. Each ring is a star outline with a node on every
  tip and valley and the same number along each edge. The topper works out
  where every pixel is (`getPixel()` gives x and y, the angle round from the
  top, the distance from the centre, the ring, and the nearest point), so
  animations can draw by position.
- Pixels are numbered clockwise round each ring from the top tip, as seen
  from the front, whatever the wiring. `setRingStart()` and
  `setRingReversed()` say where each ring's wire really starts and which way
  it runs. HolidayCoro wires them clockwise from the front, but doesn't say
  where they start, so the default (every ring starting at the top tip) is a
  guess.
- `TopperAnimation`, `MultiTopperAnimation`, `TimedTopperAnimation` and
  `TopperAnimator` are the topper's versions of the arch classes.
- Animations: `RadiatingRainbow` (rainbow rings radiating out from the
  middle, or falling in), `Pinwheel` (spinning blades of color, curled into a
  spiral with a twist: a rainbow spiral, a peppermint), `Shockwave` (bursts of
  color racing out to the tips), `RingChase` (a comet round each ring, the
  rings taking turns to go each way), `TopperStarlight` (a breathing gold
  glow with white glints), `PointChase` (the five points light in turn) and
  `ColorWipe` (colors sweep across the star from a new direction each time).
- `WiringCheck` shows whether the ring starts and directions are right; its
  header says how to read it.

The `StarTopper` program runs them on GPIO11. Set `WIRING_CHECK` to 1 in
`StarTopper.cpp` to run only the wiring check.

## Building

Open the folder in VS Code with the Raspberry Pi Pico extension and use
**Compile Project**, or from a terminal with the extension's tools on `PATH`:

```sh
cmake -B build -G Ninja
ninja -C build
```

## New Year's Eve countdown

`NewYear` is a separate firmware for New Year's Eve, built from `NewYear.cpp`
and the `newyear/` library. One pixel goes dark per second for the last
6,400 seconds (1 h 46 m 40 s) before midnight, working down from the top of
the tree. Twinkles play in the part that's still lit. The last minute pulses
with the seconds shown big, the last ten seconds zoom in digit by digit, and
midnight brings a flash, fireworks and the new year.

The time comes from the board's DS3231 RTC, which keeps UTC. Set
`UTC_OFFSET_MINUTES` in `NewYear.cpp` to the tree's time zone. Over serial:

| Command | What it does |
|---|---|
| `T 2026-12-31 22:00:00` | Set the local time (and the RTC) |
| `R` | Rehearse the whole countdown at 60x, in under 2 minutes |
| `R 15 1` | Rehearse the last 15 seconds in real time |
| `N` | Stop rehearsing and go back to the real time |
| `S` | Print the status |

Status light 0 is green when the show knows the time and red when it doesn't.

It needs PicoLEDs with `TwinkleFox` and `Fireworks2D`
(eelstretching/PicoLEDs#2).

## Show docs

Build notes for the 2026 show, in `docs/`:

- [base-ring.md](docs/base-ring.md): the PEX half ring at the bottom of the
  tree that the strands tension against.
- [power.md](docs/power.md): power supplies, fuse blocks and the perfboard
  bank boards that give every strand one 3-wire cable, with wire sizes.
- [mini-trees.md](docs/mini-trees.md): the two mini trees built from last
  year's pixels.
- [christmas-2026-plan.md](docs/christmas-2026-plan.md): the week-by-week
  plan to Thanksgiving and the parts list, as of 2026-10-12.
