#include "TopperAnimator.h"

#include <stdio.h>

#include <algorithm>
#include <random>

TopperAnimator::~TopperAnimator() {
    for (auto a : owned) {
        delete a;
    }
}

void TopperAnimator::add(TopperAnimation* a) { animations.push_back(a); }

void TopperAnimator::addTimed(TopperAnimation* a, int durationMS) {
    TimedTopperAnimation* ta = new TimedTopperAnimation(a, durationMS);
    owned.push_back(ta);
    animations.push_back(ta);
}

void TopperAnimator::setFPS(int fps) {
    this->fps = fps;
    usPerFrame = 1e6 / fps;
}

void TopperAnimator::init() {
    pos = 0;
    if (animations.empty()) {
        return;
    }
    animationChanged();
    animations[pos]->init();
    setFPS(animations[pos]->getFPS());
}

void TopperAnimator::animationChanged() {
    if (shuffle && pos == 0) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(animations.begin(), animations.end(), g);
    }
    printf("Topper animation changed to %s\n", animations[pos]->getName());
}

bool TopperAnimator::step() {
    if (animations.empty()) {
        return false;
    }
    frameWatch.start();
    stepWatch.start();
    bool changed = false;
    if (!animations[pos]->step()) {
        animations[pos]->finish();
        pos = (pos + 1) % animations.size();
        changed = true;
    }
    stepWatch.finish();
    showWatch.start();
    topper->show();
    showWatch.finish();
    frameWatch.finish();
    frameCount++;

    //
    // Done after the show to avoid a flash from the next animation's init().
    if (changed) {
        animationChanged();
        animations[pos]->init();
        setFPS(animations[pos]->getFPS());
    }

    uint64_t lus = frameWatch.getLastTime();
    if (lus < usPerFrame) {
        sleep_us(usPerFrame - lus);
    } else {
        missedFrames++;
    }
    return true;
}

void TopperAnimator::printStats() {
    uint64_t runTime = time_us_64() - startTime;
    printf(
        "%d frames run, running %.1fs at %d fps, %.2f us/step, %.2f us/show, "
        "%.2f us/frame, %d missed frames\n",
        frameCount, runTime / 1000000.0, fps, stepWatch.getAverageTime(),
        showWatch.getAverageTime(), frameWatch.getAverageTime(), missedFrames);
}
