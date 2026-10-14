# Power and controllers

Every strand gets one 3-wire cable from one screw terminal carrying fused 12 V, data and ground,
the same output as the universal-strip-controller PCB design (with a through-hole PTC in place of
the PCB's surface-mount polyfuse). Until the PCB is finished, the controllers are perfboard: one
**bank** per power supply, made of a bank board with 8 outputs, a PTC on each, and its own level
shifter.

![One bank: a supply feeding a bank board with a PTC on every output, and one output in detail](images/power-bank.svg)

- **Bank board.** Perfboard with an SN74LVC8T245 level shifter (on a TSSOP-24 breakout), a 47 ohm
  resistor on each data line, and 8 three-pin 5.08 mm screw terminals, one per strand. It carries
  12 V as well as data and ground: each terminal's +12 V comes from the board's 12 V bus through
  its own PTC.
- **A PTC on each output.** A Littelfuse RUEF400 resettable fuse (4 A hold, 8 A trip, 30 V) sits in
  series with each terminal's +12 V, its leads 5.08 mm (0.2 inch) apart in the perfboard grid. It
  is rated for 100 A of fault current; Bourns' similar MF-R400 is rated for only 40 A, less than an
  LRS-350's 150% peak of about 44 A. A tripped PTC stays open and warm until the fault is cleared
  and the power is cycled. The star topper gets an RUEF500 (5 A hold) since it draws about 4.2 A at
  full white, and every controller's first bank board carries one more PTC for the Pico board's
  buck.
- **Heavy current stays off the perfboard copper.** 12 AWG runs from the supply to the board. A bare
  12 AWG solid +12 V bus runs along the copper side, soldered to every PTC's input, and each PTC's
  output links to its terminal's + pin. A 12 AWG solid ground bus runs along the top side with a
  short leg down to each ground pin. The pads only ever carry one strand's current. Zip-tie the 12
  AWG feed to the panel so it can't pull on the bus.
- **Logic.** The Pico 2 W sits on its own small board with a 12 to 5 V buck, and reaches each
  bank board over a 16-pin ribbon that carries the 8 data lines and both of the level shifter's
  supplies (see [the ribbon](#the-ribbon-and-the-level-shifter) below).
- **The topper's data line.** The four level shifters' 32 channels all go to strands, so the star
  topper's data gets a shifter of its own on the MegaTree's Pico board (see
  [the topper's buffer](#the-toppers-buffer) below). A short 2-wire lead (data and ground) takes
  the signal to the topper's 3-pin terminal on bank board 1, beside its RUEF500.
- **One ground, separate 12 V rails.** Tie every supply's negative and the Pico's ground
  together; never join two supplies' positives.
- **PTC size.** Size each PTC to the strand's wire, not its load. An RUEF400 is sure to trip by
  8 A, which 18 AWG leads carry safely, and holds 4 A. A MegaTree strand at the 40% cap draws
  3.6 A, close to that hold once the box warms, so lower the cap if a strand trips; at the usual
  12% it draws about 1.1 A. A mini-tree strand draws 2.2 A at full white.
- **Strand cables.** Each MegaTree strand gets an 18 AWG 3-conductor round-jacket lead (NAOEVO)
  soldered to its end and sealed with adhesive-lined heat shrink: about 7.5 ft from the strand end
  to the terminal, and 17 ft for the topper. The other end goes into the screw terminal with a
  crimped ferrule on each wire, so the terminal is the disconnect. The mini trees and arches keep
  last year's xConnect pixels: an xConnect pigtail's bare end, ferruled, goes into the terminal,
  and last year's extension leads cover the runs. Label both ends of every lead.
- **Cable glands.** Every lead enters its box through a gland sized to its jacket (PG7 takes about
  3-6.5 mm, PG9 about 4-8 mm), low on a flat side wall, with a drip loop. Measure the NAOEVO
  cable's outside diameter and a pigtail's before buying glands.
- **AC side.** Lever connectors under a cover, and the cord through a gland with strain relief.
  Check the LRS-350's 115/230 V input switch before first power-up.

## How the supplies tie together

![How the supplies tie together: each supply feeds only its own bank board](images/power-supplies.svg)

- **Each supply feeds only its own bank board,** on its own 12 AWG pair to the board's 12 V and
  ground buses. Never join two supplies' +12 V: tied together they fight over the load, and one
  can end up carrying both.
- **Every ground joins.** The MegaTree's supplies are daisy-chained with 12 AWG from each supply's
  spare -V terminal to the next (an LRS-350 has three +V and three -V terminals), so the four
  supplies, the bank boards and the Pico share one ground without leaning on the ribbon's thin
  wires. The tie between supplies 2 and 3 crosses from one box to the other in the conduit link
  with ribbons 3 and 4.
- **The Pico board** takes 12 V through the extra PTC on bank board 1, and ground beside it, and
  sends 3.3 V, 5 V and ground to every bank board over the ribbons.
- **One supply, two bank boards.** A mini tree's LRS-350 feeds both of its bank boards, each on a
  separate 12 AWG pair from the supply's spare terminals. That is one rail, so no tie is needed.
- **Wall power.** An LRS-350 draws up to 6.8 A from the wall at full load, so one cord feeds at most
  two of them and the tree's two cords go on different circuits. At last year's brightness they
  draw far less. Lever nuts split each cord's line, neutral and earth to the supplies' AC/L, AC/N
  and FG terminals.

## The ribbon and the level shifter

The SN74LVC8T245 needs two supplies: 3.3 V on its A side, so it reads the Pico's 3.3 V signals
correctly, and 5 V on its B side, so the pixels get a full 5 V data signal. Both draw only a few
milliamps, so the Pico board sends them down the ribbon: 3.3 V from the Pico's 3V3 pin and 5 V
from the buck. The bank board then needs nothing but 12 V for the strands.

The ribbon is 16-wire, with a 16-pin (2x8) IDC socket on each end and a shrouded 2x8 box header
on each board, so it only plugs in one way. Crimp both sockets onto the same face of the ribbon
with the socket's triangle on the red stripe, so pin 1 lands on pin 1 at both ends, and check
each new cable with a continuity tester. A clamp and two scraps of wood press a socket closed
evenly; pliers close one end first.

Every data line has a ground on the wire next to it:

| Pin | Signal | Pin | Signal |
| --- | --- | --- | --- |
| 1 | ground | 2 | data 1 |
| 3 | data 2 | 4 | ground |
| 5 | data 3 | 6 | data 4 |
| 7 | ground | 8 | data 5 |
| 9 | data 6 | 10 | ground |
| 11 | data 7 | 12 | data 8 |
| 13 | ground | 14 | 3.3 V |
| 15 | 5 V | 16 | ground |

On the 2x8 header the odd pins are one row and the even pins the other.

Level shifter pins (from TI's SN74LVC8T245 datasheet):

| Shifter pin | Connect to |
| --- | --- |
| 1, VCCA | ribbon 3.3 V |
| 2, DIR | VCCA, so data always runs A to B |
| 3-10, A1-A8 | ribbon data 1-8 |
| 11, 12, 13, GND | ground |
| 14-21, B8-B1 | through a 47 ohm resistor each to terminals 8-1 (B1 is pin 21, so the order runs backwards) |
| 22, OE | ground, so the outputs are always on |
| 23, 24, VCCB | ribbon 5 V |

Put a 0.1 uF ceramic capacitor from VCCA to ground and another from VCCB to ground, right at the
chip.

### The topper's buffer

The topper's data comes from a 74AHCT125, a through-hole quad buffer (DIP-14, TI's SN74AHCT125N
or Adafruit's product 1787), on the MegaTree's Pico board. It runs on the buck's 5 V, and its
TTL-level inputs read the Pico's 3.3 V as a clean high. Only gate 1 is used:

| Buffer pin | Connect to |
| --- | --- |
| 1, 1OE | ground, so gate 1 is always on |
| 2, 1A | the Pico's topper pin |
| 3, 1Y | through a 47 ohm resistor to the topper lead's data wire |
| 4, 10, 13 (2OE, 3OE, 4OE) | ground |
| 5, 9, 12 (2A, 3A, 4A) | ground, so the spare inputs don't float |
| 6, 8, 11 (2Y, 3Y, 4Y) | nothing |
| 7, GND | ground |
| 14, VCC | 5 V from the buck, with a 0.1 uF capacitor to ground at the chip |

## Banks per prop

All pixels are 12 V. MegaTree sizing uses 0.54 W per node at full white, the figure the
Christmas-lighting forums use for Holiday Coro nodes; the mini trees use Wally's Lights' rating of
0.25 W for their bullet pixels. Calm effects sit far below full white: the old MegaTree
brightness of 32/255 is about 12%.

| Prop | Pixels | Full white | Supplies | Banks | Brightness cap for the supplies | Box |
| --- | --- | --- | --- | --- | --- | --- |
| MegaTree | 32 x 200 = 6,400 | about 3,460 W | 4 x Mean Well LRS-350-12 | 4 x 8 outputs | 40% | 2 x Sterilite 80 qt Gasket Box |
| Star topper | 90 | about 50 W | shares a tree supply | an RUEF500 on bank board 1, data from a 74AHCT125 on the Pico board | none | in the tree box |
| Arch group (each of 2) | up to about 270 | up to 150 W | 1 x LRS-150-12 (LRS-350-12 if bigger) | 1 small bank | none | 1 x Sterilite 8.5 gallon Gasket Tote |
| Mini tree (each of 2) | 13 x 100 = 1,300 | about 325 W | 1 x LRS-350-12 | 8 + 5 outputs | none | 1 x Sterilite 80 qt Gasket Box |

## Boxes

The LRS supplies aren't waterproof, so everything lives in gasketed Sterilite boxes, lying flat:

- **MegaTree: two 80 qt Gasket Boxes** (24 x 18 x 15.25 inches outside, 19 x 14-5/8 x 14-3/8
  inches inside at the floor). Each holds two banks with the supplies standing on edge beside the
  boards, with about 4 inches of width to spare; two supplies lying flat don't fit. Box 1 also
  holds the Pico board. A 1-1/4 inch conduit link between the boxes carries ribbons 3 and 4 and
  the ground tie.
- **Mini tree: one 80 qt Gasket Box.** A mini tree needs only about 15 x 11.5 inches with its
  supply lying flat. Set the box behind the tree on the house side, where the tree hides it from
  the street.
- **Arch group: one 8.5 gallon Gasket Tote** (13-5/8 x 11 inches inside at the floor, 17.75 x
  14.75 inches outside). An arch group needs about 12 x 10.5 inches.
- **Fallback.** Bud NBF-32032 (18.7 x 14.7 x 5.7 inches inside) is the hard-box option. CableGuard
  CG-1500s are too small, and the NBF-32046 has an 8-week lead time.

Mounting is the same in every box:

- Everything sits on a 1/2 inch exterior plywood backboard on the box's floor, not screwed through
  the walls.
- Cables come in through glands low on a flat stretch of side wall, with drip loops, and never
  through the lid.
- Drill a weep hole in the lowest corner and set each box on two pavers so it never sits in water.
- If a tarp goes over a box, hold it clear so condensation can get out.

## Wire sizes

Keep the round-trip drop on any 12 V run under about 1 V:
drop = 2 x length in feet x amps x ohms per foot (18 AWG 0.0064, 16 AWG 0.0040, 14 AWG 0.0025,
12 AWG 0.0016).

| Run | Current | Wire | Why |
| --- | --- | --- | --- |
| Supply to bank board, inside the box | up to 29 A (the LRS-350's limit) | 12 AWG stranded, under 2 ft | carries the supply's full output with little heat |
| Bank board 12 V and ground buses | up to 29 A | 12 AWG bare solid | the same current, a few inches long |
| PTC to terminal | one strand | a short link on the copper side | the PTC protects it and everything after it |
| Bank board to a MegaTree strand | about 3.6 A at the 40% cap | 18 AWG 3-conductor round-jacket cable, soldered to the strand, about 7.5 ft | 0.35 V drop at 3.6 A over 7.5 ft |
| Bank board to the star topper | about 4.2 A at full white | the same cable, 17 ft up the pole | 0.9 V drop at full white, far less at calm brightness |
| Bank board to a mini-tree strand | 2.2 A at full white | xConnect pigtail and last year's extension leads | 0.3 V drop at 2.2 A over 10 ft of 18 AWG |
| Long runs to the arches | about 3 A | 18 AWG to about 25 ft, 16 AWG to about 40 ft | stays near 1 V of drop |
| Pico board feed | well under 1 A | 18 AWG red and black hookup wire, from the extra PTC | short, and the PTC protects it |
| Power injection | only if a test shows color shift at the far end | 16 AWG pair on its own PTC | most strands at calm brightness won't need it |

## Sources

- Mean Well [LRS-350-12](https://www.bravoelectro.com/lrs-350-12.html) and
  [LRS-150-12](https://www.bravoelectro.com/lrs-150-12.html) (Bravo Electro)
- LRS-350 AC current and terminals: [Mean Well LRS-350 datasheet](https://www.meanwell.com/Upload/PDF/LRS-350/LRS-350-SPEC.PDF)
- PTCs: Littelfuse [RUEF400](https://www.digikey.com/en/products/detail/littelfuse-inc/RUEF400/5015918)
  and [RUEF500](https://www.digikey.com/en/products/detail/littelfuse-inc/RUEF500/5015921) (Digi-Key)
- Arch-group box: [Sterilite 8.5 gallon Gasket Tote](https://www.sterilite.com/?p=21679)
- Per-node current: [auschristmaslighting.com](https://auschristmaslighting.com/threads/reference-of-current-draws-of-common-rgb-lights-used.3812/)
- Level shifter pinout: [TI SN74LVC8T245 datasheet](https://www.ti.com/lit/ds/symlink/sn74lvc8t245.pdf)
- Topper buffer pinout: [TI SN74AHCT125 datasheet](https://www.ti.com/lit/ds/symlink/sn74ahct125.pdf)
- Mini-tree pixels: [Wally's Lights 12 V WS2811 xConnect bullet pixel](https://wallyslights.com/products/12v-ws2811-xconnect-resistor-bullet-pixel?variant=39808961413278)
