#include "Solver.h"

#include <cmath>

void SOLVER::SolveCollision(Particle& one, Particle& two) {
    // C^2 = A^2 + B^2
    const FVector2 diff = one.position - two.position;
    const float sqr_dist = diff.X * diff.X + diff.Y * diff.Y;
    const float dist = std::sqrt(sqr_dist);

    const float min_dist = one.radius + two.radius;

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
    const float total_radius = one.radius+ two.radius;

    const float one_ratio = two.radius / total_radius;
    const float two_ratio = one.radius / total_radius;

    one.position -= dir * overlap * one_ratio; // Add offset to particle one's position based on its radius ratio and the current overlap. Dir negated to face from particle two's position to particle one's position.
    two.position += dir * overlap * two_ratio; // Add offset to particle two's position based on its radius ratio and the current overlap.

    const FVector2 pos_diff_one = one.position - two.position;
    const FVector2 pos_diff_two = two.position - one.position;
    const float pos_diff_one_len_sq = pos_diff_one.X * pos_diff_one.X + pos_diff_one.Y * pos_diff_one.Y;
    const float pos_diff_two_len_sq = pos_diff_two.X * pos_diff_two.X + pos_diff_two.Y * pos_diff_two.Y;

    const float mass_ratio_one = 2 * two.mass / (one.mass + two.mass);
    const float mass_ratio_two = 2 * one.mass / (one.mass + two.mass);
    const float elasticity_ratio_one = 2 * one.elasticity / (one.elasticity + two.elasticity);
    const float elasticity_ratio_two = 2 * two.elasticity / (one.elasticity + two.elasticity);

    const FVector2 final_vel_one = one.velocity - (pos_diff_one * mass_ratio_one * (Dot(one.velocity - two.velocity, pos_diff_one) / pos_diff_one_len_sq) * elasticity_ratio_one);
    const FVector2 final_vel_two = two.velocity - (pos_diff_two * mass_ratio_two * (Dot(two.velocity - one.velocity, pos_diff_two) / pos_diff_two_len_sq) * elasticity_ratio_two);

    one.velocity = final_vel_one;
    two.velocity = final_vel_two;

    SOLVER::COLLISION_COUNT++;
}
