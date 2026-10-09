#include "WiringCheck.h"

WiringCheck::WiringCheck(Topper* topper, uint framesPerPixel)
    : TopperAnimation(topper, nullptr),
      framesPerPixel(framesPerPixel > 0 ? framesPerPixel : 1) {}

void WiringCheck::init() { frame = 0; }

bool WiringCheck::step() {
    static const RGB ringColors[] = {RGB(48, 0, 0), RGB(0, 48, 0),
                                     RGB(0, 0, 48)};
    static const RGB dotColors[] = {RGB::Red, RGB::Green, RGB::Blue};
    for (uint ring = 0; ring < topper->getNumRings(); ring++) {
        topper->fillRing(ring, ringColors[ring % 3]);
        topper->set(ring, frame / framesPerPixel, dotColors[ring % 3]);
        topper->set(ring, 0, RGB::White);
    }
    frame++;
    return true;
}
