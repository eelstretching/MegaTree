#ifndef SHOWTREEANIMATOR_H
#define SHOWTREEANIMATOR_H

#pragma once

#include "Animator.h"
#include "ShowDirector.h"
#include "Topper.h"
#include "TopperAnimation.h"

namespace show {

/// @brief Runs the star topper's part of the show alongside the tree, in the
/// tree's frame loop (so the two never wait on each other). Topper
/// animations step once per their own frame, 30 a second by default.
class ShowTopper {
   protected:
    Topper* topper;
    uint8_t brightness;
    TopperAnimation* scenes[NUM_SCENES] = {};
    TopperAnimation* current = nullptr;
    uint32_t owedUs = 0;

   public:
    ShowTopper(Topper* topper, uint8_t brightness)
        : topper(topper), brightness(brightness) {}

    /// @brief Says what the topper shows in a scene. Leave a scene unset to
    /// keep the topper dark in it.
    void set(SceneId scene, TopperAnimation* a) { scenes[scene] = a; }

    /// @brief Starts a scene.
    void start(SceneId scene);

    /// @brief Steps (as often as the animation's due), fades and shows the
    /// topper for one frame of the tree.
    void frame(ShowDirector& director, uint32_t usPerFrame);
};

/// @brief Runs the tree's part of the show: plays each scene's animation
/// when the schedule says, on the shared show clock, fading through black at
/// every change. Optionally runs the star topper in the same frame loop.
///
/// Animations that go by the clock are started at the moment their scene
/// started, as every prop sees it, so they're in the same place on every
/// prop and after a reboot. An animation that finishes before its scene is
/// over starts again.
class ShowTreeAnimator : public Animator {
   protected:
    ShowDirector& director;
    uint8_t brightness;
    Animation* scenes[NUM_SCENES] = {};
    Animation* current = nullptr;
    ShowTopper* topper = nullptr;

   public:
    /// @param canvas the tree
    /// @param director where the show is
    /// @param brightness the brightness to show at, when not fading
    /// @param fps the show's frame rate
    ShowTreeAnimator(Canvas* canvas, ShowDirector& director,
                     uint8_t brightness, int fps = kDefaultFPS)
        : Animator(canvas, fps), director(director), brightness(brightness) {}

    /// @brief Says what the tree shows in a scene. Leave a scene unset to
    /// keep the tree dark in it.
    void set(SceneId scene, Animation* a) { scenes[scene] = a; }

    void setTopper(ShowTopper* t) { topper = t; }

    uint64_t clockUs() override { return director.clockUs(); }

    void init() override {}

    /// @brief Draws and shows one frame, then waits for the next.
    bool step() override;
};

}  // namespace show

#endif
