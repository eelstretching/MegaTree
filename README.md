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
  80-159, and so on. Pixel 0 is the foot where data enters; `setReversed()`
  flips an arch hung the other way round.
- `ArchSet` holds all the arches (every arch the same length) and renders
  them, the way `Canvas` does for the tree. It carves each strip you `add()`
  into arches and also numbers every pixel across all arches, for effects that
  travel from one arch to the next.
- `Archimation` is the 1D counterpart of PicoLEDs' `Animation`: override
  `init()` and `step()`. `MultiArchimation` and `TimedArchimation` match
  their PicoLEDs namesakes, and `ArchAnimator` runs a list of them like
  `Animator`/`RandomAnimator` (`setShuffle(true)` for random order).
- Starter Archimations: `Leap` (a ball with a fading tail runs up and
  over each arch in turn), `ArchRise` (arches fill from both feet to the
  apex, one by one or all together) and `FillWipe` (arches fill from the feet
  with a set of colors, then black shifts in from the left foot, the right
  foot or the apex to clear them).

The `LeapingArches` program runs them on three 80-pixel arches on GPIO10.

## Building

Open the folder in VS Code with the Raspberry Pi Pico extension and use
**Compile Project**, or from a terminal with the extension's tools on `PATH`:

```sh
cmake -B build -G Ninja
ninja -C build
```
