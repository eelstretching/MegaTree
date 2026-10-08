#include "Arch.h"

void Arch::set(int p, const RGB& color) {
    if (p < 0 || (uint)p >= numPixels) {
        return;
    }
    strip->putPixel(color, toStrip(p));
}

void Arch::setMirrored(int p, const RGB& color) {
    set(p, color);
    set(numPixels - 1 - p, color);
}

const RGB& Arch::get(int p) {
    if (p < 0 || (uint)p >= numPixels) {
        return stripBlack;
    }
    return strip->get(toStrip(p));
}

void Arch::fill(const RGB& color) { strip->fill(color, start, numPixels); }

void Arch::fill(const RGB& color, int p, int n) {
    for (int i = p; i < p + n; i++) {
        set(i, color);
    }
}

void Arch::rotate(Direction direction) {
    bool away = direction == RIGHT || direction == UP;
    //
    // On a reversed arch, moving away from pixel 0 is moving towards the
    // start of the segment on the strip.
    if (away != reversed) {
        strip->rotateRight(start, start + numPixels);
    } else {
        strip->rotateLeft(start, start + numPixels);
    }
}
