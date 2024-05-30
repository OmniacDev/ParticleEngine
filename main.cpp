#include "raylib.h"

#include <cstdlib>
#include <ctime>

#include "Engine/EngineGlobalVars.h"

#include "Physics/Particle.h"
#include "Physics/Solver.h"
#include "Engine/Math/Viewport/Viewport.h"

int main()
{
    SetConfigFlags(ENGINE::ConfigFlags);
    SetTargetFPS(ENGINE::WindowFPS);

    InitWindow(ENGINE::WindowWidth, ENGINE::WindowHeight, "[FOE] Physics Engine");

    Particle StationaryParticle (FVector2(0.f, 200.f), GRAY, 64);

    StationaryParticle.Mass = 2000.f;
    StationaryParticle.Velocity = FVector2(0.f, 0.f);
    StationaryParticle.Acceleration = FVector2(0.f, -980.f);
    StationaryParticle.Elasticity = 0.75f;

    SOLVER::Particles.push_back(StationaryParticle);

    for (int i = 0; i < 100; i++) {
        Particle Water_Particle (FVector2(0.f, -100.f), SKYBLUE, 16);
        Water_Particle.Acceleration = FVector2(0.f, -980.f);
        Water_Particle.Elasticity = 0.75f;
        Water_Particle.Mass = 1.f;

        SOLVER::Particles.push_back(Water_Particle);
    }

    while (!WindowShouldClose())
    {
        float DeltaTime = GetFrameTime() > 1.f / ENGINE::WindowFPS ? 1.f / ENGINE::WindowFPS : GetFrameTime();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            VIEWPORT::TranslateViewport(IVector2(static_cast<int>(GetMouseDelta().x), static_cast<int>(GetMouseDelta().y)));
        }

        BeginDrawing();

        ClearBackground(BLACK);

        for(int i = (int)((GRID::Amount.X / -2)); i <= (int)(GRID::Amount.X / 2); i++) {
            int X = VIEWPORT::WorldToViewport(IVector2(i * GRID::Size, 0)).X;
            int Y1 = VIEWPORT::WorldToViewport(IVector2(0, ((GRID::Amount.Y / -2)) * GRID::Size)).Y;
            int Y2 = VIEWPORT::WorldToViewport(IVector2(0, ((GRID::Amount.Y / 2)) * GRID::Size)).Y;
            DrawLine(X, Y1, X, Y2, GRID::Color);
        }

        for(int i = (int)((GRID::Amount.Y / -2)); i <= (int)(GRID::Amount.Y / 2); i++) {
            int Y = VIEWPORT::WorldToViewport(IVector2(0, i * GRID::Size)).Y;
            int X1 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / -2)) * GRID::Size, 0)).X;
            int X2 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / 2)) * GRID::Size, 0)).X;
            DrawLine(X1, Y, X2, Y, GRID::Color);
        }

        for (int substep = 0; substep < 8; substep++) {
            for (int i = 0; i < SOLVER::Particles.size(); i++) {
                for (int j = 0; j < SOLVER::Particles.size(); j++) {
                    if (i == j) continue;

                    SOLVER::SolveCollision(SOLVER::Particles.at(i), SOLVER::Particles.at(j));
                }
            }
        }

        for (auto &Particle: SOLVER::Particles) {
            Particle.Update(DeltaTime);

            if (Particle.Position.X + Particle.Radius > BOUNDS::X_POS) {
                Particle.Position.X = BOUNDS::X_POS - Particle.Radius;
                Particle.Velocity.X = Particle.Velocity.X * -Particle.Elasticity;
            }
            if (Particle.Position.X - Particle.Radius < BOUNDS::X_NEG) {
                Particle.Position.X = BOUNDS::X_NEG + Particle.Radius;
                Particle.Velocity.X = Particle.Velocity.X * -Particle.Elasticity;
            }
            if (Particle.Position.Y + Particle.Radius > BOUNDS::Y_POS) {
                Particle.Position.Y = BOUNDS::Y_POS - Particle.Radius;
                Particle.Velocity.Y = Particle.Velocity.Y * -Particle.Elasticity;
            }
            if (Particle.Position.Y - Particle.Radius < BOUNDS::Y_NEG) {
                Particle.Position.Y = BOUNDS::Y_NEG + Particle.Radius;
                Particle.Velocity.Y = Particle.Velocity.Y * -Particle.Elasticity;
            }


            // Draw Particle
            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.Position.X), (int)(Particle.Position.Y)));
            DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius, {Particle.Color.r, Particle.Color.g, Particle.Color.b, 127});
            DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius, {Particle.Color.r, Particle.Color.g, Particle.Color.b, 255});
        }

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
