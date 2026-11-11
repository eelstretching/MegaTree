#include "Arch.h"

void Arch::set(int p, const RGB& color) {
    if (p < 0 || (uint)p >= numPixels) {
        return;
    }
    for (const auto& s : strands) {
        s.strip->putPixel(color, s.toStrip(p, numPixels));
    }
}

void Arch::setMirrored(int p, const RGB& color) {
    set(p, color);
    set(numPixels - 1 - p, color);
}

const RGB& Arch::get(int p) {
    if (p < 0 || (uint)p >= numPixels) {
        return stripBlack;
    }
    const ArchStrand& s = strands[0];
    return s.strip->get(s.toStrip(p, numPixels));
}

void Arch::fill(const RGB& color) {
    for (const auto& s : strands) {
        s.strip->fill(color, s.start, numPixels);
    }
}

void Arch::fill(const RGB& color, int p, int n) {
    for (int i = p; i < p + n; i++) {
        set(i, color);
    }
}

void Arch::rotate(const ArchStrand& s, bool away) {
    //
    // On a strand fed at its right foot, moving away from pixel 0 is moving
    // towards the start of the segment on the strip.
    if (away != (s.feed == ArchFeed::RIGHT_FOOT)) {
        s.strip->rotateRight(s.start, s.start + numPixels);
    } else {
        s.strip->rotateLeft(s.start, s.start + numPixels);
    }
}

void Arch::rotate(Direction direction) {
    bool away = direction == RIGHT || direction == UP;
    for (const auto& s : strands) {
        rotate(s, away);
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
    // apex pixel belongs to both legs. Reading from the first strand and
    // writing to all of them keeps every strand the same.
    int legLength = (numPixels + 1) / 2;
    int last = numPixels - 1;
    for (int i = 0; i < legLength - 1; i++) {
        set(i, get(i + 1));
        set(last - i, get(last - i - 1));
    }
    setMirrored(legLength - 1, color);
}
