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

void Arch::shift(Direction direction, const RGB& color) {
    rotate(direction);
    bool away = direction == RIGHT || direction == UP;
    set(away ? 0 : numPixels - 1, color);
}

void Arch::shiftFromApex(const RGB& color) {
    //
    // Each leg runs from a foot to the apex. With an odd number of pixels the
    // apex pixel belongs to both legs.
    int legLength = (numPixels + 1) / 2;
    int last = numPixels - 1;
    for (int i = 0; i < legLength - 1; i++) {
        set(i, get(i + 1));
        set(last - i, get(last - i - 1));
    }
    setMirrored(legLength - 1, color);
}
