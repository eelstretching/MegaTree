# Christmas 2026 plan

Snapshot of the live plan doc as of 2026-10-13. The detail behind it is in
[base-ring.md](base-ring.md) (the MegaTree base), [power.md](power.md) (banks, the ribbon, wire
sizes) and [mini-trees.md](mini-trees.md) (mini tree structure).

## Goal and scope

By Thanksgiving (Thu Nov 26) the yard has the MegaTree with its star topper, four leaping arches and
two mini trees, each running calm effects from its own perfboard Pico 2 W controller. That is five
controllers plus one spare, built over six working weeks with Thanksgiving week as buffer.

| Prop | Pixels | Controller | Outputs used |
| --- | --- | --- | --- |
| MegaTree | 32 strands x 200 = 6,400 | 1 (4 level shifters) | 32, plus the star topper (90 nodes) |
| Arches, group A | 2 arches (3 if six) | 1 | 2-3 |
| Arches, group B | 2 arches (3 if six) | 1 | 2-3 |
| Mini tree 1 | 13 x 100 = 1,300 | 1 (2 level shifters) | 13 |
| Mini tree 2 | 13 x 100 = 1,300 | 1 (2 level shifters) | 13 |
| Spare | none | 1 | built in week 5 if time allows |

Out of scope this year: the PCB layout (paused), the 270 degree topper plate, and anything that
needs the show to stream frames. Synced effects across props over PicoComm are a stretch goal; the
fallback is each prop running its own playlist on the same clock time, which already looks
deliberate with slow effects.

## How the weeks work

Each week is about 18 hours: four weeknight sessions of 1-2 hours and a weekend of about a day and a
half. That is roughly 100 hours to Thanksgiving, which is enough if the weekends carry the physical
work.

- **Weeknights are bench work.** Soldering one board section, flashing firmware, testing one strand,
  reviewing a PR, ordering parts. Each session ends with something tested, not half-soldered.
- **Weekends are yard and big builds.** Anything that needs daylight, a ladder, the lawn or a long
  uninterrupted soldering run.
- **Risky physical parts go first.** Power injection, fusing, enclosures and the tree's base are
  where surprises live, so the MegaTree's power and weatherproofing are done before any arch is
  built.
- **Build one, then copy.** The MegaTree controller is the template. The arch and mini-tree
  controllers are smaller copies of a board you already debugged.
- **Software rides along.** Effects and sync are the fun part and the part you want to do yourself,
  so they fill weeknights when soldering is done, never a weekend.
- **Get stakes in early.** The ground near Boston can freeze in late November, so the tree ring and
  arch stakes go in by mid-November.

## Week by week

Each week ends with a checkpoint that must be true before the next week starts. If a checkpoint
slips, the cut lines in the next section say what gives.

### Week 0, Oct 12-18: Ottawa, laptop only

Checkpoint: parts ordered and the MegaTree controller drawn on paper.

- [ ] Weeknight: do the power budget for every prop (pixels x watts per pixel at your brightness cap) and pick the supplies
- [ ] Weeknight: plan the base layout: the strands are fed from the bottom (0,0 is lower right), so the controller boxes sit by the pole and short leads run to the ring; only the star topper needs a 17 ft run up the pole
- [ ] Weeknight: draw the bank board (level shifter, 47 ohm resistors, 3-pin terminals, ground bus) with its fuse block beside it, and the small Pico board; see [Power architecture](#power-architecture)
- [ ] Weeknight: order fuses, fuse holders, enclosures, cable glands, wire and connectors (see the [parts checklist](#parts-checklist))
- [ ] Weeknight (fun): review and merge PicoLEDs#10 (40 fps clock) so all props share one frame rate
- [ ] Weekend: travel; nothing planned

### Week 1, Oct 19-25: MegaTree controller and power

Checkpoint: one MegaTree strand lit from the perfboard, through its fuse, off the real supply.

- [ ] Weeknight: build the Pico board and the first bank board (8 outputs); flash a test pattern.
- [ ] Weeknight: test the 8 outputs on the bench with one strand; scope or eyeball for flicker
- [ ] Weeknight: wire the supply to the bank board and check the fuse block and ground bus temperatures with 8 strands on
- [ ] Weekend: build the other 3 bank boards; test every output with one strand moved down the line
- [ ] Weekend: test 8 strands at once at your brightness cap; check the supply, wire and fuse temperatures after 30 minutes
- [ ] Weekend: decide power injection from that test (start of strand only, or start and end)

### Week 2, Oct 26-Nov 1: weatherproofing and the tree base (Halloween Saturday)

Checkpoint: the MegaTree controller and supply sealed in their enclosure, and the PEX ring built.

- [ ] Weeknight: mount the board and supply in the enclosure with glands, drip loops and strain relief
- [ ] Weeknight: make up the data and power leads for the 32 strands and label every one
- [ ] Weeknight: build the star topper output and run the WiringCheck from MegaTree#3 to find each ring's start node
- [ ] Weeknight (fun): move MegaTree onto the 40 fps clock and pick the calm playlist
- [ ] Sunday: cut and couple the 3/4 inch PEX half ring (20 ft), fit ball bungees and tension clips, test-fit on the lawn

### Week 3, Nov 2-8: MegaTree goes up; arch controllers

Checkpoint: the MegaTree lit on the flagpole at night, running the playlist.

- [ ] Weeknight: build arch controller A (copy of the MegaTree board, 1 level shifter)
- [ ] Weeknight: build arch controller B
- [ ] Weeknight: bench test LeapingArches on both controllers with one arch each
- [ ] Weekend: raise the topper and 32 strands, drive the 8 J-stakes, clip strands to the ring, fit the star
- [ ] Weekend: run the tree for an evening; look for dead strands, color order mistakes and wet connectors

### Week 4, Nov 9-15: arches

Checkpoint: four arches lit in the yard in two groups, stakes in the ground.

- [ ] Weeknight: put both arch controllers in enclosures; make up the leads
- [ ] Weeknight: decide 4 or 6 arches based on how weeks 1-3 went
- [ ] Weeknight (fun): PicoComm basement test with two props: leader plus one follower on the show router
- [ ] Weekend: build and stake the arches, run power and data, test both groups; drive the mini-tree rebar stakes
- [ ] Weekend: tune the leaping timing between groups by eye from the street

### Week 5, Nov 16-22: mini trees and the whole show

Checkpoint: every prop runs on its own schedule from dusk to bedtime without you touching it.

- [ ] Weeknight: test last year's pixels; count dead nodes and work out their color order
- [ ] Weeknight: build the two mini-tree controllers
- [ ] Weeknight: set the on and off times (DS3231 or a light sensor) on every controller
- [ ] Weekend: build and place the mini trees, open backs toward the house
- [ ] Weekend: put everything on PicoComm if the basement test passed; otherwise each prop runs its own playlist
- [ ] Weekend: build the spare controller if there is time

### Week 6, Nov 23-26: buffer and Thanksgiving

Checkpoint: lights on Thanksgiving night.

- [ ] Fix whatever broke in the first rain
- [ ] Swap in the spare controller if a board died
- [ ] Nothing new goes in this week

## Mini tree structure

Each mini tree is a cone about 48 inches tall: a 4 ft black-pipe pole, a 10 inch plywood hub on top
and a 36 inch PEX hoop at the bottom. The 13 strands fold over the hub, so each strand makes two of
the 26 columns and both of its ends land at the bottom, next to the controller.

The columns cover the 270 degrees facing the street and leave the back open toward the house, like
the MegaTree's 180. The visible side is about 28% denser than with the columns spread all the way
round, and nothing shines into the windows. Print the [hub
template](templates/mini-tree-hub-270.pdf) to mark the holes. A tree seen from every side can go
back to 360 degrees: 13.85 degrees between holes, which a compass set to 1.09 inches walks out, and
no back guys.

![Mini tree structure: side view and hub](images/mini-tree.svg)

| Part | Size | Why |
| --- | --- | --- |
| Columns | 26 over 270 degrees, 10.8 degrees apart, 50 pixels each at 1 inch | 13 strands of 100, folded at the top |
| Hub | 10 inch plywood disk, 26 holes over 270 degrees of a 9 inch circle, 0.85 inch apart | the fold between pixels 50 and 51 spans one gap |
| Base hoop | 36 inch circle of 1/2 inch PEX, about 9.5 ft | base about 0.74 x height, the same cone shape as the MegaTree; columns 3.4 inches apart at the bottom; the hoop stays a full circle to anchor the back guys |
| Height to hub | about 48.3 inches | 50 inch slant with the hoop 13 inches out from the hub edge |
| Lean | 15 degrees from vertical |  |

The fold has room to spare: these are [Wally's Lights 12 V bullet
pixels](https://wallyslights.com/products/12v-ws2811-xconnect-resistor-bullet-pixel?variant=39808961413278)
with 4 inches of wire between shells, and the gap between neighbouring columns on a 10 inch hub is
only 0.85 inch. Your 10 inch circles are the hubs; the 20 inch ones would leave a flat, wide top on
a 36 inch base.

### Building one

1. **Pole.** A 3/4 inch black pipe, 48 inches long and threaded. It slides over a 3 ft length of 1/2
   inch rebar driven 18 to 24 inches into the lawn (the pipe's bore is about 0.82 inch). Paint it,
   or buy galvanized, so it doesn't streak rust.
2. **Hub.** Screw a 3/4 inch floor flange to the centre of the plywood disk and thread it onto the
   pole. Print the hub template at actual size on letter paper and check its 2 inch bar with a
   ruler. Centre it on the disk with the street arrow facing the street, and centre-punch the 26
   holes through it. Drill 3/16 inch for zip ties, then seal every face and especially the edge with
   exterior paint or spar urethane.
3. **Strips.** Cut each mounting strip between pixels 50 and 51 without cutting the wire. Zip-tie
   the tops of the two halves to neighbouring holes so the wire loop sits over the rim.
4. **Hoop.** Close 9.5 ft of 1/2 inch PEX with one push-fit coupling, centre it on the pole and pin
   it with 4 landscape staples.
5. **Tension.** A small ball bungee from the bottom of each strip to the hoop, as on the MegaTree
   ring. With no strips at the back, they pull the top of the pole toward the street, so tie 2 or 3
   back guys of UV-resistant cord around the pipe just under the flange and run them to the back of
   the hoop, each with a ball bungee at the hoop end. Tension the guys until the pole stands plumb;
   every column leaning evenly is the check.
6. **Feed.** Each strand's pixel 1 is at the bottom of every other column. Run 13 three-wire leads
   (power, data and ground) from the controller box at the pole base.

### Ideas for the plywood

- The hub is the topper; nothing needs to be bought for it.
- For a crown, cut the spare 27th strand into two 50-pixel halves and set each around the rim of one
  of your 20 inch circles, mounted flat a few inches above the hub as a halo; the 4 inch wire lets
  the pixels sit 1.26 inches apart. It costs your only spare strand, and the second half needs a new
  pigtail.

### Power, outputs and software

- Wally's rates these pixels at 0.25 W each, so 1,300 pixels is about 325 W at full white and one
  LRS-350-12 per tree runs them at full brightness.
- 13 outputs of 100 pixels make two banks per tree (8 + 5 outputs), each with its own level shifter,
  and each tree gets its own Pico.
- The canvas is 26 x 50 with serpentine columns, so the MegaTree effects carry over.

Parts per tree: a 48 inch 3/4 inch black pipe, a 3/4 inch floor flange, 3 ft of 1/2 inch rebar, 10
ft of 1/2 inch PEX and one push-fit coupling, 4 landscape staples, 29 small ball bungees (26 strips,
3 back guys), about 15 ft of UV-resistant cord for the guys, UV-rated zip ties, and exterior paint.

## Power architecture

Every strand gets one 3-wire cable from one connector carrying fused 12 V, data and ground, the same
output as the PCB design, with a blade fuse in place of the PCB's polyfuse: a 47 ohm resistor, a
fuse and a 3-pin terminal per output. On perfboard those outputs live on a bank board, one per
supply with 8 outputs and its own level shifter.

![One bank: supply, fuse block, bank board, and one output in detail](images/power-bank.svg)

- **Bank board.** Perfboard with the level shifter, a 47 ohm resistor on each data line, and 8
  three-pin 5.08 mm screw terminals, one per strand. It carries data and ground; the 12 V for each
  terminal comes from the fuse block beside it.
- **Fuse block beside each bank board.** A blade fuse block with a negative bus (12-circuit for an
  8-output bank, 6-circuit for a small one; DaierTek, or Blue Sea ST for the name brand), mounted
  right next to the terminal row. Each 5 A fuse feeds its strand's +12 V terminal through a short 18
  AWG jumper, so every strand still leaves on one 3-wire cable and nothing depends on a fuse holder
  fitting perfboard. Spare circuits feed the 12 to 5 V buck and the star topper.
- **Heavy current stays off the perfboard copper.** 12 AWG from the supply to the fuse block, and a
  12 AWG solid ground bus along the back of the bank board tied to the block's negative bus. The
  pads only ever carry one strand's current.
- **Logic.** The Pico sits on its own small board with a 12 to 5 V buck, and reaches each bank board
  over a 16-pin ribbon: 8 data lines, 3.3 V for the level shifter's input side (from the Pico's 3V3
  pin), 5 V for its output side (from the buck), and 6 grounds, one beside each pair of data lines.
  The MegaTree has 4 banks, an arch group 1, and each mini tree 2 (8 + 5 outputs).
- **One ground, separate 12 V rails.** Tie every supply's negative and the Pico's ground together;
  never join two supplies' positives.
- **Fuse size.** Size each fuse to the strand's wire, not its load: 5 A suits 18 AWG pigtails and
  leads. A mini-tree strand draws 2.2 A at full white.
- **Fit.** Lay one bank (supply, fuse block, bank board) out on paper at full size before buying
  boxes; the tree may need more than 2 CG-1500s or one larger box.
- **AC side.** Lever connectors under a cover, and the cord through a gland with strain relief.

### Wire sizes

Keep the round-trip drop on any 12 V run under about 1 V: drop = 2 x length in feet x amps x ohms
per foot (18 AWG 0.0064, 16 AWG 0.0040, 14 AWG 0.0025, 12 AWG 0.0016).

| Run | Current | Wire | Why |
| --- | --- | --- | --- |
| Supply to fuse block, inside the box | up to 29 A (the LRS-350's limit) | 12 AWG, under 2 ft | carries the supply's full output with little heat |
| Bank board ground bus | up to 29 A | 12 AWG solid | the same current, a few inches long |
| Bank board to a strand | about 3.6 A for a MegaTree strand at 40%, 2.2 A for a mini-tree strand at full white | 18 AWG 3-wire (xConnect extensions) | 0.5 V drop at 4 A over 10 ft; fine for the tree's base and the mini trees |
| Long runs to the arches | about 3 A | 18 AWG up to about 25 ft, 16 AWG to about 40 ft | stays near 1 V of drop |
| Power injection | only if the week 1 test shows color shift at the far end | 16 AWG pair on its own 5 A fuse | most strands at your brightness won't need it |

## Risks and cut lines

The MegaTree is protected at all costs; everything else can shrink. If a checkpoint slips by more
than a weekend, cut from the bottom of this list up.

| Risk | Shows up in | What gives |
| --- | --- | --- |
| The star topper's power and data run 17 ft up the pole | Week 2 | Run one 3-wire lead up the pole in week 2 and test the topper on it before raising; 90 nodes need little current, so voltage drop is small |
| Supply or wiring runs hot at full brightness | Week 1 | Keep the brightness cap low (calm effects need little), add injection, or split onto two supplies |
| Water in a connector or enclosure | Weeks 2-3 | Glands, drip loops and dielectric grease; the week 3 test night is there to find it |
| The first bank board has a flaky output | Week 1 | Fix it before copying the design; the arch boards inherit every mistake |
| PicoComm sync isn't solid on the lawn | Week 4-5 | Drop it; each prop runs its own playlist |
| Last year's pixels are worn out | Week 5 | One mini tree instead of two |
| Time runs short | Any | Order of cuts: spare controller, then sync, then arches 5-6, then mini tree 2 |

The frozen-ground risk is handled by schedule: every stake is in by Nov 15.

## Parts checklist

Order these in week 0 so nothing waits on shipping. Quantities assume 5 controllers plus a spare.

**Amazon, one order**

- [ ] Blade fuse blocks with a negative bus: 6 x 12-way and 4 x 6-way (12-ways: 5 bought plus 2 from last year). [DaierTek](https://www.daierswitches.com/products/12-way-12v-24v-atc-ato-blade-fuse-block-holder-with-led-indicator) works (M5 studs take #10 rings, M4 circuit screws take #8; check the 6-way has a negative bus), or Blue Sea for the name brand: [12-circuit 5026](https://www.amazon.com/Blue-Sea-Systems-Blade-Block/dp/B001P6FTHC) and [6-circuit 5025](https://www.amazon.com/Blue-Sea-Systems-5025-Block/dp/B00KOEK0NE)
- [ ] 5 A ATO/ATC blade fuses: a [100-pack](https://www.amazon.com/clp/B07WS16MF9), or [Bussmann ATC-5](https://www.amazon.com/clp/B01MXE2PHR) for a name brand
- [ ] 12 to 5 V buck converters x6: buying two packs of the [AKEYSRC 6-pack](https://www.amazon.com/dp/B0GYJDP96Q) (5-30 V in, fixed 5 V, 3 A); check each one's output with a meter before soldering it in
- [ ] 16-pin (2x8) IDC sockets, about 25 (a [50-pack](https://www.amazon.com/PC-Accessories-Connectors-50-Pack-Connector/dp/B00E57QQVG) leaves spares for mistakes), about 20 shrouded 2x8 through-hole box headers (0.1 inch, keyed), such as [uxcell, 24 pieces](https://www.amazon.com/uxcell-16-Pin-Straight-Connector-Sockets/dp/B010V43ACO) or [XINYIYUAN, 30 pieces](https://www.amazon.com/XINYIYUAN-16-Pin-Straight-Connector-Sockets/dp/B08NB8S42K), and 16-wire ribbon
- [ ] Perfboard, at least 10 x 15 cm per bank board, for about 10 bank boards and 6 Pico boards
- [ ] 47 ohm resistors, about 75, if your stash is short
- [ ] Cable glands, about 70: one per strand pigtail and one per AC cord, with a cable range that covers the pigtail's jacket (a [PG7 to PG19 kit](https://www.amazon.com/AMPELE-Plastic-Waterproof-Connectors-Gaskets/dp/B08TCDZ8LN) or 1/2 inch NPT)
- [ ] [Wago 221 lever nut assortment](https://www.amazon.com/Compact-Splicing-Connector-Assortment-221-2401/dp/B0BN6MBR7X) for the AC side
- [ ] [4 inch ball bungees, 100-pack](https://www.amazon.com/clp/B001BNS5P2) (58 for the mini trees, 32 for the MegaTree), plus about 30 ft of UV-resistant paracord for the mini trees' back guys
- [ ] [UV-resistant 8 inch black zip ties, 1000](https://www.amazon.com/Resistant-Self-Locking-Plastic-Indoor-Outdoor/dp/B09WVNDV7Y)
- [ ] [Permatex dielectric grease](https://www.amazon.com/s?k=Permatex+22058), the 3 oz tube (22058), since a 0.33 oz tube won't cover about 70 outdoor joints, and an [adhesive-lined heat shrink kit](https://www.amazon.com/270-pcs-Adhesive-Assortment-MILAPEAK/dp/B0771K1Z7Q)
- [ ] Crimp consumables only (both crimpers already on hand): about 250 ferrules for every stranded wire into a bank-board screw terminal (all three wires of each pigtail, and the fuse jumpers), and insulated ring and fork terminals for the fuse block and supplies: #8 for the fuse block's circuit screws, #10 rings for its bus studs, and forks for the supplies' screw terminals
- [ ] 18 AWG stranded hookup wire, about 50 ft of red and 25 ft of black, for the fuse jumpers and buck feeds; 16 AWG 3-wire only if an arch sits more than 25 ft from its box

These links came from search; Amazon won't let me open its pages, so check the price and reviews on
each before buying.

**Digi-Key, added to the order you already have**

- [ ] Mean Well LRS-350-12 x7 (4 tree, 2 mini trees, 1 spare) and LRS-150-12 x2 (arches); all pixels are 12 V. Bravo Electro is about $275 for the same set if Digi-Key costs more. Not from Amazon: knockoffs turn up there.
- [ ] Nothing else; the fuse blocks come from Amazon

**A pixel shop (Wally's Lights or Holiday Coro)**

- [ ] CableGuard CG-1500 enclosures x6 (2 tree, 1 per other prop)
- [ ] xConnect pigtails with one bare end, one per strand (about 70): the bare end goes into the bank board's screw terminal, so there's no soldering; plus plug-in extension leads where a strand is far from its box

**Home Depot**

- [ ] Mini trees: 2 x 3/4 inch black pipe 48 inch threaded, 2 x 3/4 inch floor flanges, 1/2 inch rebar for two 3 ft stakes, 2 x 10 ft 1/2 inch PEX, 2 x 1/2 inch push-fit couplings, 8 landscape staples, exterior paint or spar urethane
- [ ] MegaTree base, if not bought yet: 20 ft of 3/4 inch PEX-B, 4 x 3/4 inch push-fit couplings, 8 rebar J-stakes
- [ ] 12 AWG: about 10 ft bare solid for the bank-board buses, and stranded red and black for the supply feeds
- [ ] Stakes and hardware for the arches

**Already on hand or ordered:** Pico 2 W x6 (2 + 4 ordered), level shifters x48 (4 + 44 coming; 11
needed), TSSOP breakouts, screw terminals (more ordered), a ferrule crimper and a ratcheting crimper
for insulated terminals.

The base design is in [base-ring.md](base-ring.md), with [power.md](power.md) and
[mini-trees.md](mini-trees.md) beside it.

### Supplies and boxes per prop

Sizing uses 0.54 W per 12 V node at full white, the figure the Christmas-lighting forums use for
Holiday Coro nodes. Calm effects sit far below that; your old MegaTree brightness of 32/255 is about
12%.

| Prop | Pixels | Full white | Supplies | Brightness cap for the supplies | Box |
| --- | --- | --- | --- | --- | --- |
| MegaTree | 6,400 | about 3,460 W | 4 x LRS-350-12, one per 8-strand level-shifter group | 40% of full white | 2 x CG-1500 (board + 2 supplies, 2 supplies) |
| Star topper | 90 | about 50 W | shares a tree supply | none needed | in the tree box |
| Arch group (each of 2) | up to about 270 | up to 150 W | 1 x LRS-150-12 (LRS-350-12 if the group is bigger) | none up to 270 px | 1 x CG-1500 |
| Mini tree (each of 2) | 1,300 (13 x 100) | about 325 W (0.25 W per pixel, per Wally's) | 1 x LRS-350-12 | none needed | 1 x CG-1500 |

The LRS supplies are not waterproof, so they live inside the box, mounted with cables entering from
the bottom and drip loops. Check the LRS-350's 115/230 V input switch before first power-up. Prices
are from [Bravo Electro LRS-350-12](https://www.bravoelectro.com/lrs-350-12.html) ($33.30) and
[LRS-150-12](https://www.bravoelectro.com/lrs-150-12.html) ($20.30); box sizes from [Pierce Lights'
enclosure page](https://www.piercelights.com/tech/enclosures); the per-node figure from
[auschristmaslighting.com](https://auschristmaslighting.com/threads/reference-of-current-draws-of-common-rgb-lights-used.3812/).
