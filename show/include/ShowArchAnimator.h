#ifndef SHOWARCHANIMATOR_H
#define SHOWARCHANIMATOR_H

#pragma once

#include "ArchAnimator.h"
#include "ShowDirector.h"

namespace show {

/// @brief Runs a group of arches' part of the show: the arch counterpart of
/// ShowTreeAnimator. Each scene's archimation starts at the moment the scene
/// started, as every prop sees it, so archimations that go by the clock (Leap,
/// ArchGlow) line up across controllers.
class ShowArchAnimator : public ArchAnimator {
   protected:
    ShowDirector& director;
    uint8_t brightness;
    Archimation* scenes[NUM_SCENES] = {};
    Archimation* current = nullptr;

   public:
    ShowArchAnimator(ArchSet* arches, ShowDirector& director,
                     uint8_t brightness, int fps = kDefaultFPS)
        : ArchAnimator(arches, fps), director(director), brightness(brightness) {}

    /// @brief Says what the arches show in a scene. Leave a scene unset to
    /// keep them dark in it.
    void set(SceneId scene, Archimation* a) { scenes[scene] = a; }

    uint64_t clockUs() override { return director.clockUs(); }

    void init() override {}

    /// @brief Draws and shows one frame, then waits for the next.
    bool step() override;
};

}  // namespace show

#endif
