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
    this->fps = fps > 0 ? fps : kDefaultFPS;
    usPerFrame = 1000000 / this->fps;
}

void ArchAnimator::init() {
    pos = 0;
    if (archimations.empty()) {
        return;
    }
    archimationChanged();
    uint64_t now = clockUs();
    startArchimation(archimations[pos], now - now % usPerFrame);
}

void ArchAnimator::startArchimation(Archimation* a, uint64_t at) {
    a->startClock(at);
    a->init();
    //
    // Owed exactly one step on its first frame.
    int sfps = a->getFPS();
    uint32_t period = sfps > 0 ? 1000000 / sfps : 0;
    stepOwedUs = period > usPerFrame ? period - usPerFrame : 0;
}

int ArchAnimator::stepsDue(Archimation* a) {
    int sfps = a->getFPS();
    if (sfps <= 0) {
        return 1;
    }
    uint32_t period = 1000000 / sfps;
    stepOwedUs += usPerFrame;
    int n = stepOwedUs / period;
    stepOwedUs -= n * period;
    return n > kMaxStepsPerFrame ? kMaxStepsPerFrame : n;
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
    uint64_t now = clockUs();
    frameTime = now - now % usPerFrame;

    frameWatch.start();
    stepWatch.start();
    bool changed = false;
    Archimation* a = archimations[pos];
    int n = stepsDue(a);
    for (int i = 0; i < n; i++) {
        a->setFrameTime(frameTime);
        if (!a->step()) {
            a->finish();
            pos = (pos + 1) % archimations.size();
            changed = true;
            break;
        }
    }
    stepWatch.finish();
    showWatch.start();
    arches->show();
    showWatch.finish();
    frameWatch.finish();
    frameCount++;

    //
    // Done after the show to avoid a flash from the next archimation's init().
    // The new archimation's first step is on the next frame.
    if (changed) {
        archimationChanged();
        startArchimation(archimations[pos], frameTime + usPerFrame);
    }

    waitForNextFrame();
    return true;
}

void ArchAnimator::waitForNextFrame() {
    uint64_t next = frameTime + usPerFrame;
    uint64_t now = clockUs();
    if (now >= next) {
        //
        // Late. The next step() starts right away, on the frame we're in now.
        missedFrames++;
        return;
    }
    if (next - now > usPerFrame) {
        //
        // The clock jumped backwards (a shared clock stepping to correct
        // itself). Line up with the new clock's next frame.
        next = now - now % usPerFrame + usPerFrame;
    }
    //
    // If we're dithering, spend the time until the next frame re-sending this
    // one, since it's the average over sends that gets the in-between levels
    // right. Stop when another send wouldn't finish before the next frame.
    if (arches->getRenderer()->getDithering()) {
        uint64_t last = 0;
        while (now + last < next) {
            arches->show();
            uint64_t after = clockUs();
            last = after - now;
            now = after;
        }
    }
    now = clockUs();
    if (now < next) {
        sleep_us(next - now);
    }
}

void ArchAnimator::printStats() {
    uint64_t runTime = time_us_64() - startTime;
    printf(
        "%d frames run, running %.1fs at %d fps, %.2f us/step, %.2f us/show, "
        "%.2f us/frame, %d missed frames\n",
        frameCount, runTime / 1000000.0, fps, stepWatch.getAverageTime(),
        showWatch.getAverageTime(), frameWatch.getAverageTime(), missedFrames);
}
