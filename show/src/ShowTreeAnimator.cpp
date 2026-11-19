#include "ShowTreeAnimator.h"

#include <stdio.h>

namespace show {

void ShowTopper::start(SceneId scene) {
    if (current != nullptr) {
        current->finish();
    }
    current = scenes[scene];
    topper->clear();
    owedUs = 0;
    if (current != nullptr) {
        current->init();
    }
}

void ShowTopper::frame(ShowDirector& director, uint32_t usPerFrame) {
    if (!director.on() || current == nullptr) {
        topper->clear();
    } else {
        int fps = current->getFPS();
        uint32_t period = 1000000 / (fps > 0 ? fps : 30);
        owedUs += usPerFrame;
        int n = 0;
        while (owedUs >= period && n < 4) {
            owedUs -= period;
            n++;
            if (!current->step()) {
                current->finish();
                current->init();
            }
        }
        if (owedUs >= period) {
            owedUs = 0;  // Too far behind; don't try to catch up.
        }
    }
    topper->setBrightness(director.faded(brightness));
    topper->show();
}

bool ShowTreeAnimator::step() {
    uint64_t now = clockUs();
    frameTime = now - now % usPerFrame;
    director.poll();

    frameWatch.start();
    stepWatch.start();
    if (director.update(frameTime)) {
        const Position& pos = director.position();
        if (current != nullptr) {
            current->finish();
        }
        current = scenes[pos.scene];
        canvas->clear();
        if (current != nullptr) {
            //
            // Start it when the scene started, so that it's where it would be
            // on any other prop, then tell it about the previous frame so its
            // first step moves it one frame rather than the whole way there.
            uint64_t start = (uint64_t)pos.sceneStartUs;
            startAnimation(current, start);
            if (frameTime > start + usPerFrame) {
                current->setFrameTime(frameTime - usPerFrame);
            }
        }
        if (topper != nullptr) {
            topper->start(pos.scene);
        }
    }
    if (!director.on()) {
        if (current != nullptr) {
            current->finish();
            current = nullptr;
        }
        canvas->clear();
    } else if (current != nullptr) {
        int n = stepsDue(current);
        for (int i = 0; i < n; i++) {
            current->setFrameTime(frameTime);
            if (!current->step()) {
                current->finish();
                startAnimation(current, frameTime);
                break;
            }
        }
    }
    stepWatch.finish();

    canvas->setBrightness(director.faded(brightness));
    showWatch.start();
    canvas->show();
    showWatch.finish();
    if (topper != nullptr) {
        topper->frame(director, usPerFrame);
    }
    frameWatch.finish();
    frameCount++;

    waitForNextFrame();
    return true;
}

}  // namespace show
