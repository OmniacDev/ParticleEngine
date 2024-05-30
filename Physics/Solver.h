#pragma once

#include <vector>

#include "Particle.h"

namespace SOLVER {
    inline std::vector<Particle> Particles;

    void SolveCollision(Particle& One, Particle& Two);
}