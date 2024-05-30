#include "Particle.h"

void Particle::Update(const float DeltaTime) {
    Velocity = Velocity + (Acceleration * DeltaTime);
    Position = Position + (Velocity * DeltaTime);
}