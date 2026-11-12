#include "ArchGlow.h"

#include <math.h>

ArchGlow::ArchGlow(ArchSet* arches, const std::vector<RGB>& colors,
                   uint32_t periodMs, uint8_t low, uint8_t lag)
    : Archimation(arches, nullptr, 0),
      colors(colors),
      periodMs(periodMs > 0 ? periodMs : 1),
      low(low),
      lag(lag) {}

bool ArchGlow::step() {
    if (colors.empty()) {
        return true;
    }
    uint64_t periodUs = (uint64_t)periodMs * 1000;
    float phase = (float)(elapsedUs() % periodUs) / periodUs;
    uint n = arches->getNumArches();
    uint first = arches->getFirstShowArch();
    for (uint i = 0; i < n; i++) {
        uint a = first + i;
        //
        // A smooth breath: a cosine from low up to full and back.
        float t = phase - a * lag / 256.0f;
        float swell = 0.5f - 0.5f * cosf(2 * (float)M_PI * t);
        uint8_t level = low + (uint8_t)((255 - low) * swell);
        RGB c = colors[a % colors.size()];
        arches->getArch(i)->fill(c.scale8(level));
    }
    return true;
}
