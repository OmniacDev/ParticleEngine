#pragma once

#include "../Engine/Math/Vector/Vector.h"
#include "../Engine/Math/Types.h"

#include "raylib.h"
#include "../Engine/Math/Rect/Rect.h"

class Particle {

public:
    Particle(FVector2 Position, Color Color, float Radius) : Position(Position), Color(Color), Radius(Radius) {}

    FVector2 Position = FVector2(0.0f, 0.0f);
    FVector2 Velocity = FVector2(0.0f, 0.0f);
    FVector2 Acceleration = FVector2(0.0f, 0.0f);

    Color Color = { 245, 245, 245, 255 };

    float Radius = 1.0f;
    float Mass = 1.0f;
    float Elasticity = 1.0f;

    void Update(float DeltaTime);

};

static Rect GetParticleArea(Particle& P) {
    return Rect(FVector2(P.Position.X - P.Radius, P.Position.Y - P.Radius), FVector2(P.Radius * 2, P.Radius * 2));
}