#include "Solver.h"

#include <cmath>

void SOLVER::SolveCollision(Particle& One, Particle& Two) {
    // C^2 = A^2 + B^2
    const FVector2 diff = One.Position - Two.Position;
    const float sqr_dist = std::pow(diff.X, 2.0f) + std::pow(diff.Y, 2.0f);
    const float dist = std::sqrt(sqr_dist);

    const float min_dist = One.Radius + Two.Radius;

    if (dist >= min_dist) return;

    // Resolve overlap
    FVector2 dir(0.0f, 0.0f);

    if (dist == 0.0f) {
        const float X = 0.707f;
        const float Y = std::sqrt(1.0f - std::pow(X, 2.0f));
        dir = FVector2(X, Y);
    }
    else {
        dir = diff / dist;
    }

    const float overlap = dist - min_dist; // current overlap of particles

    One.Position += dir * -1 * overlap * (Two.Radius / (One.Radius + Two.Radius)); // Add offset to particle one's position based on its radius ratio and the current overlap. Dir negated to face from particle two's position to particle one's position.
    Two.Position += dir * overlap * (One.Radius / (One.Radius + Two.Radius)); // Add offset to particle two's position based on its radius ratio and the current overlap.

    // Calculate final momentum using Law of Conservation of Momentum.
    // Momentum is symbolised by "p".

    const FVector2 final_vel_one = One.Velocity - ((One.Position - Two.Position) * (2 * (Two.Mass) / (One.Mass + Two.Mass)) * (Dot(One.Velocity - Two.Velocity, One.Position - Two.Position) / std::pow(Length(One.Position - Two.Position), 2.0f)) * (2 * One.Elasticity / (One.Elasticity + Two.Elasticity)));
    const FVector2 final_vel_two = Two.Velocity - ((Two.Position - One.Position) * (2 * (One.Mass) / (One.Mass + Two.Mass)) * (Dot(Two.Velocity - One.Velocity, Two.Position - One.Position) / std::pow(Length(Two.Position - One.Position), 2.0f)) * (2 * Two.Elasticity / (One.Elasticity + Two.Elasticity)));

    One.Velocity = final_vel_one;
    Two.Velocity = final_vel_two;

}
