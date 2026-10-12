# Power and controllers

Every strand gets one 3-wire cable from one screw terminal carrying fused 12 V, data and ground,
the same output as the universal-strip-controller PCB design (with a blade fuse in place of the
PCB's polyfuse). Until the PCB is finished, the controllers are perfboard: one **bank** per power
supply, made of a fuse block mounted beside a bank board with 8 outputs and its own level shifter.

![One bank: supply, fuse block, bank board, and one output in detail](images/power-bank.svg)

- **Bank board.** Perfboard with an SN74LVC8T245 level shifter (on a TSSOP-24 breakout), a 47 ohm
  resistor on each data line, and 8 three-pin 5.08 mm screw terminals, one per strand. It carries
  data and ground; the 12 V for each terminal comes from the fuse block beside it.
- **Fuse block beside each bank board.** A blade fuse block with a negative bus (Blue Sea ST
  5026, 12 circuits, for an 8-output bank; 5025, 6 circuits, for a small one), mounted right next
  to the terminal row. Each 5 A fuse feeds its strand's +12 V terminal through a short 18 AWG
  jumper, so the fuse comes before the terminal and protects the jumper, the terminal and the
  whole strand cable. Spare circuits feed the 12 to 5 V buck and the star topper.
- **Heavy current stays off the perfboard copper.** 12 AWG from the supply to the fuse block, and
  a 12 AWG solid ground bus along the back of the bank board tied to the block's negative bus. The
  pads only ever carry one strand's current.
- **Logic.** The Pico 2 W sits on its own small board with a 12 to 5 V buck, and reaches each
  bank board over a 10-wire ribbon (8 data lines and ground).
- **One ground, separate 12 V rails.** Tie every supply's negative and the Pico's ground
  together; never join two supplies' positives.
- **Fuse size.** Size each fuse to the strand's wire, not its load: 5 A suits 18 AWG pigtails and
  leads.
- **Strand cables.** xConnect pigtails with one bare end: the bare end, with a crimped ferrule,
  goes into the screw terminal and the plug goes out through a cable gland. No soldering.
- **AC side.** Lever connectors under a cover, and the cord through a gland with strain relief.
  Check the LRS-350's 115/230 V input switch before first power-up.

## Banks per prop

All pixels are 12 V. MegaTree sizing uses 0.54 W per node at full white, the figure the
Christmas-lighting forums use for Holiday Coro nodes; the mini trees use Wally's Lights' rating of
0.25 W for their bullet pixels. Calm effects sit far below full white: the old MegaTree
brightness of 32/255 is about 12%.

| Prop | Pixels | Full white | Supplies | Banks | Brightness cap for the supplies |
| --- | --- | --- | --- | --- | --- |
| MegaTree | 32 x 200 = 6,400 | about 3,460 W | 4 x Mean Well LRS-350-12 | 4 x 8 outputs | 40% |
| Star topper | 90 | about 50 W | shares a tree supply | a spare fuse block circuit | none |
| Arch group (each of 2) | up to about 270 | up to 150 W | 1 x LRS-150-12 (LRS-350-12 if bigger) | 1 small bank | none |
| Mini tree (each of 2) | 13 x 100 = 1,300 | about 325 W | 1 x LRS-350-12 | 8 + 5 outputs | none |

The LRS supplies aren't waterproof, so they live inside the enclosure (CableGuard CG-1500 or
similar) with cables entering from the bottom and drip loops. Lay one bank out at full size before
buying boxes; the tree may need more than two CG-1500s, or one larger box.

## Wire sizes

Keep the round-trip drop on any 12 V run under about 1 V:
drop = 2 x length in feet x amps x ohms per foot (18 AWG 0.0064, 16 AWG 0.0040, 14 AWG 0.0025,
12 AWG 0.0016).

| Run | Current | Wire | Why |
| --- | --- | --- | --- |
| Supply to fuse block, inside the box | up to 29 A (the LRS-350's limit) | 12 AWG, under 2 ft | carries the supply's full output with little heat |
| Bank board ground bus | up to 29 A | 12 AWG solid | the same current, a few inches long |
| Fuse to terminal | one strand | 18 AWG jumper | short, and the fuse protects it |
| Bank board to a strand | about 3.6 A for a MegaTree strand at 40%, 2.2 A for a mini-tree strand at full white | 18 AWG 3-wire (xConnect) | 0.5 V drop at 4 A over 10 ft |
| Long runs to the arches | about 3 A | 18 AWG to about 25 ft, 16 AWG to about 40 ft | stays near 1 V of drop |
| Power injection | only if a test shows color shift at the far end | 16 AWG pair on its own 5 A fuse | most strands at calm brightness won't need it |

## Sources

- Mean Well [LRS-350-12](https://www.bravoelectro.com/lrs-350-12.html) and
  [LRS-150-12](https://www.bravoelectro.com/lrs-150-12.html) (Bravo Electro)
- Enclosure sizes: [Pierce Lights](https://www.piercelights.com/tech/enclosures)
- Per-node current: [auschristmaslighting.com](https://auschristmaslighting.com/threads/reference-of-current-draws-of-common-rgb-lights-used.3812/)
- Mini-tree pixels: [Wally's Lights 12 V WS2811 xConnect bullet pixel](https://wallyslights.com/products/12v-ws2811-xconnect-resistor-bullet-pixel?variant=39808961413278)
