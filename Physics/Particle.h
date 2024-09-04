#pragma once

#include <SFML/Graphics.hpp>

#include "../Engine/Math/Vector/Vector.h"
#include "../Engine/Math/Types.h"

#include "../Engine/Math/Rect/Rect.h"

class Particle {

public:
    Particle(const FVector2& position, const sf::Color& color, const float radius) : position(position), color(color), radius(radius) {}

    FVector2 position = FVector2(0.0f, 0.0f);
    FVector2 velocity = FVector2(0.0f, 0.0f);
    FVector2 acceleration = FVector2(0.0f, 0.0f);

    sf::Color color = {245, 245, 245, 255 };

    float radius = 1.0f;
    float mass = 1.0f;
    float elasticity = 1.0f;

    void Update(float delta_time);

};

static Rect GetParticleArea(const Particle& p) {
    return {FVector2(p.position.X - p.radius, p.position.Y - p.radius), FVector2(p.radius * 2, p.radius * 2)};
}