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

## Building

Open the folder in VS Code with the Raspberry Pi Pico extension and use
**Compile Project**, or from a terminal with the extension's tools on `PATH`:

```sh
cmake -B build -G Ninja
ninja -C build
```
