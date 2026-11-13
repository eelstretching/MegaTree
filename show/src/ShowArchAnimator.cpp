#include "ShowArchAnimator.h"

namespace show {

bool ShowArchAnimator::step() {
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
        arches->clear();
        if (current != nullptr) {
            //
            // Start it when the scene started (see ShowTreeAnimator::step()).
            uint64_t start = (uint64_t)pos.sceneStartUs;
            startArchimation(current, start);
            if (frameTime > start + usPerFrame) {
                current->setFrameTime(frameTime - usPerFrame);
            }
        }
    }
    if (!director.on()) {
        if (current != nullptr) {
            current->finish();
            current = nullptr;
        }
        arches->clear();
    } else if (current != nullptr) {
        int n = stepsDue(current);
        for (int i = 0; i < n; i++) {
            current->setFrameTime(frameTime);
            if (!current->step()) {
                current->finish();
                startArchimation(current, frameTime);
                break;
            }
        }
    }
    stepWatch.finish();

    arches->setBrightness(director.faded(brightness));
    showWatch.start();
    arches->show();
    showWatch.finish();
    frameWatch.finish();
    frameCount++;

    waitForNextFrame();
    return true;
}

}  // namespace show
