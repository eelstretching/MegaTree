# MegaTree base ring

The base for the flagpole MegaTree: one PEX hoop that all 32 tension clips bungee onto, held
down by a handful of J-stakes instead of 32 separate ones.

The strands are wired from the bottom (pixel 0,0 is the lower right of the tree), so the
controller boxes sit at the base of the pole and short leads run out to the ring. Only the star
topper needs a lead run 17 ft up the pole. See [power.md](power.md) for the controller side.

## This year's plan (QT7 topper, decided 2026-10-09)

The QT7 has 64 holes. 32 strands over 270° don't divide evenly into that, so the choices are 24
strands over 270° or all 32 over 180°. **The recommendation is all 32 strands over 180°**, using
every hole on the street-facing half, with the half-arc ring described below. A custom aluminium
plate that spreads 32 strands over 270° is a possible upgrade for next year.

**Size.** The strands are 200" long and the topper sits at about 206" (17 ft 2"). On an
almost-vertical cone like this, the gap between the bottom of each strand and the ring (clip plus
bungee) is what sets the ring size:

| Strand-to-ring gap | Ring diameter | PEX (half arc) | Clip spacing | Strand angle from vertical |
|---|---|---|---|---|
| 12" | 8.3 ft | 13 ft | 4.9" | 14° |
| 15" | 10.3 ft | 16 ft | 6.0" | 17° |
| **18"** | **11.9 ft** | **19 ft** | **7.0"** | **19°** |
| 21" | 13.3 ft | 21 ft | 7.9" | 21° |
| 24" | 14.7 ft | 23 ft | 8.6" | 23° |

The recommendation is the **18" gap, a 12 ft ring (radius 6 ft) and a 20 ft length of PEX**. Each
inch of gap moves the ring about 3" in radius. So lay the arc at a 6 ft radius and use the bungee
stretch to take up the difference.

## The design in one paragraph

Lay a ring of **3/4" PEX-B** on the lawn, centred on the flagpole. Join it with **push-fit
couplings** so it comes apart into 4 arcs for storage. Pin it down with **8 rebar J-stakes**
hooked over the pipe. Each tension clip gets a **ball bungee** looped around the ring. This is the
same base HolidayCoro uses for its own trees (3/4" PEX-B ring, U/J stakes, one clip per string).
The only change is a bungee where they use an 8" zip tie, so the strands have some give in the
wind.

Why this beats one stake per line: the ring fixes the spacing and the circle once, so you don't
have to line up 32 stakes by eye. You only drive 8 stakes, and the whole thing lifts off in four
pieces when the season ends.

## Sizing the ring

The ring diameter comes from how high the topper sits on the flagpole (**H**, ground to topper).

- **D ≈ 0.72 × H** gives the classic MegaTree cone, with strands about 70° off the ground
  (20° half-angle).
- For a slimmer tree use D ≈ 0.55 × H. For a squatter one use D ≈ 0.9 × H.
- Each strand, from topper to ring, has to cover **√(H² + (D/2)²)**. That is the pixel run plus
  the leader plus the clip and bungee. If your strands are a fixed length, solve the other way:
  D = 2 × √(L² − H²).

| Topper height H | Ring diameter D | PEX needed (πD) | Clip spacing (full ring) | Strand slant length |
|---|---|---|---|---|
| 12 ft | 8.6 ft | 27 ft | 10.2" | 12.8 ft |
| 14 ft | 10.1 ft | 32 ft | 11.9" | 14.9 ft |
| 16 ft | 11.5 ft | 36 ft | 13.6" | 17.0 ft |
| 18 ft | 13.0 ft | 41 ft | 15.3" | 19.1 ft |
| 20 ft | 14.4 ft | 45 ft | 17.0" | 21.3 ft |

The worked numbers below assume a topper at **16 ft**, which gives an 11.5 ft ring.

## Parts list (full ring, 16 ft topper)

| Qty | Part | Notes |
|---|---|---|
| 1 | 3/4" PEX-B, 50 ft coil (white) | Use about 36 ft. A coil already curves, so it wants to be a circle. Get 1" PEX if the ring is over ~14 ft across; it's stiffer. |
| 4 | 3/4" push-fit couplings (SharkBite-style), plus the disconnect clip | They let the ring come apart into 4 arcs. Barb couplings with crimp rings work too, but they're permanent. |
| 8 | 18" rebar J-stakes (3/8" or 1/2") | Hook them over the pipe. Use 12 if the ring is over ~14 ft. |
| 32 | 6" ball bungees, black | One per tension clip. |
| 1 roll | Electrical tape (a bright colour) | Marks the 32 clip positions. |
| — | Mason's line plus a can of marking paint | Used to scribe the circle. |

Rough cost is $60–90. Most of that is the PEX and the stakes.

## Building it

1. **Scribe the circle.** Tie a loose loop of mason's line around the flagpole and mark it at the
   radius (D/2, which is 5 ft 9" for the 16 ft example). Walk it around the pole and paint the
   circle. Before you drive anything 18" into the lawn, check where your sprinkler lines run.
2. **Cut and assemble.** Cut the PEX into 4 equal arcs (πD/4, about 9 ft each) and push-fit them
   into a ring. Lay it on the painted circle.
3. **Mark the clip positions.** Measure along the pipe and put a tape band every πD/32 (13.6"). Put
   the first one in line with the topper's string 1, so the strands hang straight down rather than
   spiralling.
4. **Stake it.** Put a J-stake every 4th clip mark, so 8 in all, and place each one *between* two
   clips so it isn't in a strand's way. Drive them until the hook just bears on the pipe. The
   strands pull the ring **up and inward**, so the stakes are there to hold it down. The inward
   pull on a closed hoop is balanced all the way round, so the ring holds its shape by itself.
5. **Attach the clips.** For each strand, pass a ball bungee through the hole at the bottom of the
   tension clip. Wrap it around the ring at its tape mark, then hook the ball. Tension each strand
   until it's straight, with the bungee stretched about a third. That leaves room to take up
   gusts and the strands' sag in the cold. If clips creep along the pipe, wrap the bungee around
   the ring twice. Or put a small zip tie on each side of the mark as a stop.

## If the tree is only the street-facing half (180°)

You use the same radius and half the PEX: a semicircle of πD/2, about 18 ft for the example. The
32 clips go across 180°, so the spacing halves to πD/64 (6.8" in the example). An open arc isn't
balanced the way a hoop is. The strand tension will try to curl the two ends in toward the pole,
so **double-stake each end**: two J-stakes within the last foot. Also put a stake every ~4 clips
along the arc. Stop the pipe about 6" past the last clip on each end.

Which topper you bought tells you the layout. On HolidayCoro's chart, 32 strings is the QT5 at
360° or the QT7 at 180°.

## If the tree is three-quarters (270°), open toward the house

You use the same radius, with an arc of 0.75 × πD (about 27 ft for the example). Put the open 90°
gap facing the front windows. The two ends need the same double stakes as the half arc. A 270°
arc mostly balances the pull, but its ends still try to curl inward.

The topper sets how many strands you can use. These numbers are inferred from HolidayCoro's chart
("32 at 360°", "16 at 180°"), which implies evenly spaced holes all the way round the plate:

- **QT5** (32 holes, 11.25° apart): 270° gives you 24 strands, so 8 of your clips go spare.
- **QT7** (64 holes, 5.6° apart): 270° is 48 holes. 32 strands don't divide evenly into that, so
  use every other hole for 24 strands. You can also put all 32 in a fuller 180°.
- **A custom topper plate** with 32 holes at 8.44° apart (270/32) gives you all 32 strands over
  270°. It could be drilled aluminium or a 3D print.

With all 32 strands over 270°, the clips sit πD × 0.75 / 32 apart (10.2" in the example).

## Sources

- HolidayCoro Seed Pixel MegaTree Topper page (base ring, stakes, tension clip):
  https://www.holidaycoro.com/Seed-Pixel-MegaTree-Topper-System-For-MegaTrees-p/751.htm
- Instructables, Flag Pole Mega Tree: https://instructables.com/Flag-Pole-Mega-Tree
- Light-O-Rama forum, mega tree methods: https://forums.lightorama.com/topic/37042-mega-tree-methods/
