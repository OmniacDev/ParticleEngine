#include "Particle.h"

void Particle::Update(const float delta_time) {
    velocity = velocity + (acceleration * delta_time);
    position = position + (velocity * delta_time);
}