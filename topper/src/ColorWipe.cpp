#include "ColorWipe.h"

#include <math.h>

ColorWipe::ColorWipe(Topper* topper, ColorMap* colorMap, uint8_t nColors,
                     const uint8_t* colors, uint frames, uint holdFrames,
                     uint8_t turn)
    : TopperAnimation(topper, colorMap),
      colors(colors, colors + nColors),
      frames(frames > 0 ? frames : 1),
      holdFrames(holdFrames),
      turn(turn) {}

void ColorWipe::init() {
    frame = 0;
    color = 0;
    direction = 0;
    topper->clear();
}

bool ColorWipe::step() {
    if (frame < frames) {
        RGB c = colors.empty()
                    ? RGB::White
                    : colorMap->getColor(colors[color % colors.size()]);
        //
        // The line starts on the far side of the star from where it's
        // heading and moves across it. A pixel is covered once the line has
        // passed it.
        float a = direction * 2 * (float)M_PI / 256;
        float dx = -sinf(a), dy = cosf(a);
        float line = 1.0f - 2.1f * (frame + 1) / frames;
        for (uint p = 0; p < topper->getNumPixels(); p++) {
            const StarPixel& px = topper->getPixel(p);
            if (px.x * dx + px.y * dy >= line) {
                topper->set((int)p, c);
            }
        }
    }
    if (++frame >= frames + holdFrames) {
        frame = 0;
        color++;
        direction += turn;
    }
    return true;
}
