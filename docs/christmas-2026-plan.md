# Christmas 2026 plan

Snapshot of the build plan as of 2026-10-12. By Thanksgiving (Thu Nov 26) the yard has the
MegaTree with its star topper, four leaping arches and two mini trees, each running calm effects
from its own perfboard Pico 2 W controller. That is five controllers plus a spare, built over six
working weeks with Thanksgiving week as buffer.

| Prop | Pixels | Controller | Outputs |
| --- | --- | --- | --- |
| MegaTree | 32 strands x 200 = 6,400 | 1 Pico, 4 banks | 32, plus the star topper (90 nodes) |
| Arches, group A | 2 arches (3 if six) | 1 Pico, 1 small bank | 2-3 |
| Arches, group B | 2 arches (3 if six) | 1 Pico, 1 small bank | 2-3 |
| Mini tree 1 | 13 x 100 = 1,300 | 1 Pico, 2 banks | 13 |
| Mini tree 2 | 13 x 100 = 1,300 | 1 Pico, 2 banks (or shares mini tree 1's Pico) | 13 |
| Spare | none | 1 | built in week 5 if time allows |

Out of scope this year: the universal-strip-controller PCB layout (paused), the 270 degree topper
plate, and anything that streams frames. Synced effects across props over PicoComm are a stretch
goal; the fallback is each prop running its own playlist on the same clock time.

Related docs: [base-ring.md](base-ring.md) (the MegaTree base), [power.md](power.md) (banks,
supplies, wire sizes), [mini-trees.md](mini-trees.md) (mini tree structure).

## How the weeks work

Each week is about 18 hours: four weeknight sessions of 1-2 hours and a weekend of about a day and
a half, roughly 100 hours to Thanksgiving.

- **Weeknights are bench work.** Soldering one board, flashing firmware, testing one strand,
  reviewing a PR, ordering parts. Each session ends with something tested, not half-soldered.
- **Weekends are yard and big builds.** Anything that needs daylight, a ladder, the lawn or a long
  soldering run.
- **Risky physical parts go first.** Power, fusing, enclosures and the tree's base are where
  surprises live, so the MegaTree's power and weatherproofing are done before any arch is built.
- **Build one, then copy.** The first bank board is the template for every other one.
- **Software rides along.** Effects and sync fill weeknights when soldering is done.
- **Get stakes in early.** The ground near Boston can freeze in late November, so every stake is
  in by Nov 15.

## Week by week

Each week ends with a checkpoint that must be true before the next week starts.

### Week 0, Oct 12-18: Ottawa, laptop only

Checkpoint: parts ordered and the bank board drawn on paper.

- [x] Do the power budget for every prop and pick the supplies (see [power.md](power.md))
- [ ] Plan the MegaTree base layout: controller boxes by the pole, short leads to the ring; only the star topper needs a run up the pole
- [ ] Draw the bank board (level shifter, 47 ohm resistors, 3-pin terminals, ground bus) with its fuse block beside it, and the small Pico board
- [ ] Order parts (see the list below)
- [ ] Review and merge PicoLEDs#10 (40 fps clock) so all props share one frame rate
- [ ] Weekend: travel

### Week 1, Oct 19-25: MegaTree controller and power

Checkpoint: one MegaTree strand lit from the perfboard, through its fuse, off the real supply.

- [ ] Build the Pico board and the first bank board; flash a test pattern
- [ ] Test the 8 outputs on the bench with one strand; scope or eyeball for flicker
- [ ] Wire the supply to the fuse block and bank board; check the fuse block and ground bus temperatures with 8 strands on
- [ ] Weekend: build the other 3 bank boards; test every output with one strand moved down the line
- [ ] Weekend: test 8 strands at once at the brightness cap; check supply, wire and fuse temperatures after 30 minutes
- [ ] Weekend: decide power injection from that test

### Week 2, Oct 26-Nov 1: weatherproofing and the tree base (Halloween Saturday)

Checkpoint: the MegaTree controller and supplies sealed in their enclosures, and the PEX ring built.

- [ ] Mount the banks and supplies in the enclosures with glands, drip loops and strain relief
- [ ] Make up the 32 strand cables and label every one
- [ ] Run the star topper lead and run the WiringCheck from MegaTree#3 to find each ring's start node
- [ ] Move MegaTree onto the 40 fps clock and pick the calm playlist
- [ ] Sunday: cut and couple the 3/4 inch PEX half ring, fit ball bungees and tension clips, test-fit on the lawn

### Week 3, Nov 2-8: MegaTree goes up; arch controllers

Checkpoint: the MegaTree lit on the flagpole at night, running the playlist.

- [ ] Build arch controllers A and B (a Pico board and a small bank each)
- [ ] Bench test LeapingArches on both controllers with one arch each
- [ ] Weekend: raise the topper and 32 strands, drive the 8 J-stakes, clip strands to the ring, fit the star
- [ ] Weekend: run the tree for an evening; look for dead strands, color order mistakes and wet connectors

### Week 4, Nov 9-15: arches

Checkpoint: four arches lit in the yard in two groups, every stake in the ground.

- [ ] Put both arch controllers in enclosures; make up the leads
- [ ] Decide 4 or 6 arches based on how weeks 1-3 went
- [ ] PicoComm basement test with two props: leader plus one follower on the show router
- [ ] Weekend: build and stake the arches, run power and data, test both groups; drive the mini-tree rebar stakes
- [ ] Weekend: tune the leaping timing between groups by eye from the street

### Week 5, Nov 16-22: mini trees and the whole show

Checkpoint: every prop runs on its own schedule from dusk to bedtime without anyone touching it.

- [ ] Test last year's pixels; count dead nodes and work out their color order
- [ ] Build the mini-tree controllers
- [ ] Set the on and off times (DS3231 or a light sensor) on every controller
- [ ] Weekend: build and place the mini trees (270 degrees, open back toward the house; print the [hub template](templates/mini-tree-hub-270.pdf))
- [ ] Weekend: put everything on PicoComm if the basement test passed; otherwise each prop runs its own playlist
- [ ] Weekend: build the spare controller if there is time

### Week 6, Nov 23-26: buffer and Thanksgiving

Checkpoint: lights on Thanksgiving night.

- [ ] Fix whatever broke in the first rain
- [ ] Swap in the spare controller if a board died
- [ ] Nothing new goes in this week

## Risks and cut lines

The MegaTree is protected at all costs; everything else can shrink.

| Risk | Shows up in | What gives |
| --- | --- | --- |
| The star topper's power and data run 17 ft up the pole | Week 2 | Run one 3-wire lead and test the topper on it before raising; 90 nodes need little current |
| Supply or wiring runs hot | Week 1 | Keep the brightness cap low, add injection, or split onto more supplies |
| Water in a connector or enclosure | Weeks 2-3 | Glands, drip loops and dielectric grease; the week 3 test night is there to find it |
| The first bank board has a flaky output | Week 1 | Fix it before copying; every other bank inherits its mistakes |
| PicoComm sync isn't solid on the lawn | Weeks 4-5 | Drop it; each prop runs its own playlist |
| Last year's pixels are worn out | Week 5 | One mini tree instead of two |
| Time runs short | Any | Order of cuts: spare controller, then sync, then arches 5-6, then mini tree 2 |

## Parts, in four orders

Already on hand or ordered: Pico 2 W x6, SN74LVC8T245 level shifters x48 (11 needed), TSSOP
breakouts, screw terminals.

**Amazon** (links found by search; check price and reviews before buying)

- [ ] Blade fuse blocks with negative bus: 6 x [Blue Sea 5026, 12-circuit](https://www.amazon.com/Blue-Sea-Systems-Blade-Block/dp/B001P6FTHC) and 4 x [Blue Sea 5025, 6-circuit](https://www.amazon.com/Blue-Sea-Systems-5025-Block/dp/B00KOEK0NE)
- [ ] 5 A ATO/ATC blade fuses: a [100-pack](https://www.amazon.com/clp/B07WS16MF9), or [Bussmann ATC-5](https://www.amazon.com/clp/B01MXE2PHR)
- [ ] 12 to 5 V buck converters x6, such as [this potted 3 A module](https://www.amazon.com/Converter-Voltage-Waterproof-Regulator-Step-Down/dp/B07Y2V1F8V)
- [ ] [10-pin IDC sockets](https://www.amazon.com/CablesOnline-10-Pack-10-Pin-Connectors-FC-010-10/dp/B07W5C4P6Z) and 10-wire ribbon
- [ ] Perfboard, at least 10 x 15 cm per bank board, for about 10 bank boards and 6 Pico boards
- [ ] 47 ohm resistors, about 75
- [ ] [Cable gland kit, PG7 to PG19](https://www.amazon.com/AMPELE-Plastic-Waterproof-Connectors-Gaskets/dp/B08TCDZ8LN)
- [ ] [Wago 221 lever nut assortment](https://www.amazon.com/Compact-Splicing-Connector-Assortment-221-2401/dp/B0BN6MBR7X) for the AC side
- [ ] [4 inch ball bungees, 100-pack](https://www.amazon.com/clp/B001BNS5P2) (58 for the mini trees, 32 for the MegaTree)
- [ ] UV-resistant paracord, about 30 ft, for the mini trees' back guys
- [ ] [UV-resistant 8 inch black zip ties](https://www.amazon.com/Resistant-Self-Locking-Plastic-Indoor-Outdoor/dp/B09WVNDV7Y)
- [ ] [Permatex dielectric grease](https://amazon.com/Permatex-81150-Dielectric-Grease-33/dp/B01LRDBSCW) and an [adhesive-lined heat shrink kit](https://www.amazon.com/270-pcs-Adhesive-Assortment-MILAPEAK/dp/B0771K1Z7Q)
- [ ] [Ferrule crimper with ferrules](https://www.amazon.com/IWISS-End-sleeves-Crimping-Electrician-Contractors/dp/B07LCF39W9)
- [ ] 16 AWG 3-wire, only if an arch sits more than 25 ft from its box

**Digi-Key or Bravo Electro** (not Amazon: Mean Well knockoffs turn up there)

- [ ] Mean Well LRS-350-12 x7 (4 tree, 2 mini trees, 1 spare) and LRS-150-12 x2 (arches), about $275 at Bravo Electro

**A pixel shop (Wally's Lights or Holiday Coro)**

- [ ] CableGuard CG-1500 enclosures (count after laying out one bank)
- [ ] xConnect pigtails with one bare end, one per strand (about 70), plus extension leads where a strand is far from its box

**Home Depot**

- [ ] Mini trees: 2 x 3/4 inch black pipe 48 inch threaded, 2 x 3/4 inch floor flanges, 1/2 inch rebar for two 3 ft stakes, 2 x 10 ft 1/2 inch PEX, 2 x 1/2 inch push-fit couplings, 8 landscape staples, exterior paint or spar urethane
- [ ] MegaTree base: 20 ft of 3/4 inch PEX-B, 4 x 3/4 inch push-fit couplings, 8 rebar J-stakes
- [ ] 12 AWG: about 10 ft bare solid for the ground buses, and stranded red and black for the supply feeds
- [ ] Stakes and hardware for the arches
