#pragma once

#include <vector>

#include "Particle.h"
#include "QuadTree/QuadTree.h"
#include "../Engine/EngineGlobalVars.h"

namespace SOLVER {
    inline std::vector<Particle> Particles;

    inline int COLLISION_COUNT = 0;

    inline Quad::Tree QuadTree(Rect(FVector2(BOUNDS::X_NEG, BOUNDS::Y_NEG), FVector2((float)GRID::Amount.X * GRID::Size, (float)GRID::Amount.Y * GRID::Size)));

    void SolveCollision(Particle& One, Particle& Two);
}