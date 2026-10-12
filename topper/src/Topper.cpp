#include "Topper.h"

#include <math.h>
#include <stdio.h>

Topper::Topper(Strip* strip, std::vector<uint> ringSizes, uint start,
               uint8_t brightness)
    : strip(strip),
      start(start),
      ringSizes(ringSizes),
      ringStarts(ringSizes.size(), 0),
      ringReversed(ringSizes.size(), false),
      renderer(brightness) {
    renderer.add(strip);
    for (uint n : ringSizes) {
        ringOffsets.push_back(numPixels);
        numPixels += n;
    }
    if (start + numPixels > strip->getNumPixels()) {
        printf("Topper needs %d pixels from %d, but the strip on pin %d has %d\n",
               numPixels, start, strip->getPin(), strip->getNumPixels());
    }
    layOut();
}

void Topper::layOut() {
    pixels.clear();
    uint nRings = ringSizes.size();
    for (uint ring = 0; ring < nRings; ring++) {
        //
        // Each ring is a star outline the same shape as the outer one, just
        // smaller. Its ten corners alternate tip, valley, tip... clockwise
        // from the top tip, 36 degrees apart.
        float tip = 1.0f - RING_STEP * (nRings - 1 - ring);
        float valley = tip * VALLEY_RATIO;
        uint n = ringSizes[ring];
        for (uint i = 0; i < n; i++) {
            //
            // Spread the nodes evenly along the ten edges, so a ring of 40
            // has 4 to an edge: one on the corner and three along it.
            float u = 10.0f * i / n;
            int edge = (int)u;
            float f = u - edge;
            float r0 = edge % 2 == 0 ? tip : valley;
            float r1 = edge % 2 == 0 ? valley : tip;
            float a0 = edge * (float)M_PI / 5;
            float a1 = (edge + 1) * (float)M_PI / 5;
            float x0 = r0 * sinf(a0), y0 = r0 * cosf(a0);
            float x1 = r1 * sinf(a1), y1 = r1 * cosf(a1);

            StarPixel px;
            px.x = x0 + (x1 - x0) * f;
            px.y = y0 + (y1 - y0) * f;
            px.ring = ring;

            float turn = atan2f(px.x, px.y) / (2 * (float)M_PI);
            if (turn < 0) {
                turn += 1;
            }
            px.angle = (uint8_t)((int)lroundf(turn * 256) & 0xFF);
            px.point = ((px.angle * 5 + 128) >> 8) % 5;

            float r = sqrtf(px.x * px.x + px.y * px.y);
            px.radius = (uint8_t)lroundf(fminf(r, 1.0f) * 255);
            pixels.push_back(px);
        }
    }
}

void Topper::setRingStart(uint ring, uint pos) {
    if (ring < ringStarts.size()) {
        ringStarts[ring] = pos % ringSizes[ring];
    }
}

void Topper::setRingReversed(uint ring, bool reversed) {
    if (ring < ringReversed.size()) {
        ringReversed[ring] = reversed;
    }
}

uint Topper::toStrip(uint ring, uint i) {
    uint n = ringSizes[ring];
    //
    // i is where the pixel is round the ring; the wire starts at ringStarts
    // and runs clockwise unless the ring is reversed.
    uint w = ringReversed[ring] ? (ringStarts[ring] + n - i) % n
                                : (i + n - ringStarts[ring]) % n;
    return start + ringOffsets[ring] + w;
}

void Topper::set(uint ring, int i, const RGB& color) {
    if (ring >= ringSizes.size()) {
        return;
    }
    int n = ringSizes[ring];
    i %= n;
    if (i < 0) {
        i += n;
    }
    strip->putPixel(color, toStrip(ring, i));
}

const RGB& Topper::get(uint ring, int i) {
    if (ring >= ringSizes.size()) {
        return stripBlack;
    }
    int n = ringSizes[ring];
    i %= n;
    if (i < 0) {
        i += n;
    }
    return strip->get(toStrip(ring, i));
}

void Topper::set(int p, const RGB& color) {
    if (p < 0 || (uint)p >= numPixels) {
        return;
    }
    uint ring = pixels[p].ring;
    set(ring, p - ringOffsets[ring], color);
}

const RGB& Topper::get(int p) {
    if (p < 0 || (uint)p >= numPixels) {
        return stripBlack;
    }
    uint ring = pixels[p].ring;
    return get(ring, p - ringOffsets[ring]);
}

void Topper::fillRing(uint ring, const RGB& color) {
    if (ring >= ringSizes.size()) {
        return;
    }
    strip->fill(color, start + ringOffsets[ring], ringSizes[ring]);
}

void Topper::fill(const RGB& color) { strip->fill(color, start, numPixels); }

void Topper::fade(uint8_t keep) {
    for (uint p = start; p < start + numPixels; p++) {
        strip->putPixel(strip->get(p).scale8(keep), p);
    }
}
