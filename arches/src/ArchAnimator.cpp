#include "ArchAnimator.h"

#include <stdio.h>

#include <algorithm>
#include <random>

ArchAnimator::~ArchAnimator() {
    for (auto a : owned) {
        delete a;
    }
}

void ArchAnimator::add(Archimation* a) { archimations.push_back(a); }

void ArchAnimator::addTimed(Archimation* a, int durationMS) {
    TimedArchimation* ta = new TimedArchimation(a, durationMS);
    owned.push_back(ta);
    archimations.push_back(ta);
}

void ArchAnimator::setFPS(int fps) {
    this->fps = fps;
    usPerFrame = 1e6 / fps;
}

void ArchAnimator::init() {
    pos = 0;
    if (archimations.empty()) {
        return;
    }
    archimationChanged();
    archimations[pos]->init();
    setFPS(archimations[pos]->getFPS());
}

void ArchAnimator::archimationChanged() {
    if (shuffle && pos == 0) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(archimations.begin(), archimations.end(), g);
    }
    printf("Archimation changed to %s\n", archimations[pos]->getName());
}

bool ArchAnimator::step() {
    if (archimations.empty()) {
        return false;
    }
    frameWatch.start();
    stepWatch.start();
    bool changed = false;
    if (!archimations[pos]->step()) {
        archimations[pos]->finish();
        pos = (pos + 1) % archimations.size();
        changed = true;
    }
    stepWatch.finish();
    showWatch.start();
    arches->show();
    showWatch.finish();
    frameWatch.finish();
    frameCount++;

    //
    // Done after the show to avoid a flash from the next archimation's init().
    if (changed) {
        archimationChanged();
        archimations[pos]->init();
        setFPS(archimations[pos]->getFPS());
    }

    uint64_t lus = frameWatch.getLastTime();
    if (lus < usPerFrame) {
        sleep_us(usPerFrame - lus);
    } else {
        missedFrames++;
    }
    return true;
}

void ArchAnimator::printStats() {
    uint64_t runTime = time_us_64() - startTime;
    printf(
        "%d frames run, running %.1fs at %d fps, %.2f us/step, %.2f us/show, "
        "%.2f us/frame, %d missed frames\n",
        frameCount, runTime / 1000000.0, fps, stepWatch.getAverageTime(),
        showWatch.getAverageTime(), frameWatch.getAverageTime(), missedFrames);
}
