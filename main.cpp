#include "raylib.h"

#include <string>

#include "Engine/EngineGlobalVars.h"

#include "Physics/Particle.h"
#include "Physics/Solver.h"
#include "Engine/Math/Viewport/Viewport.h"
#include "Engine/Math/Rect/Rect.h"

const bool FAST_RENDERING = false;
const bool RENDERING_ENABLED = true;

int main()
{
    SetConfigFlags(ENGINE::ConfigFlags);
    SetTargetFPS(ENGINE::WindowFPS);

    InitWindow(ENGINE::WindowWidth, ENGINE::WindowHeight, "[FOE] Physics Engine");

    while (!WindowShouldClose())
    {
        SOLVER::QuadTree.Clear();

        float DeltaTime = GetFrameTime() > 1.f / ENGINE::WindowMinFPS ? 1.f / ENGINE::WindowMinFPS : GetFrameTime();

        int ObjectComparisons = 0;
        QT_PROF::SEARCH_COUNT = 0;
        RECT_PROF::OVERLAP_TESTS = 0;
        RECT_PROF::CONTAIN_TESTS = 0;
        SOLVER::COLLISION_COUNT = 0;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            VIEWPORT::TranslateViewport(IVector2(static_cast<int>(GetMouseDelta().x), static_cast<int>(GetMouseDelta().y)));
        }

        if (IsKeyDown(KEY_N)) {
            Particle Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 8);
            Water_Particle.Acceleration = FVector2(0.f, 0.f);
            Water_Particle.Velocity = FVector2 (0.f, 0.0f);
            Water_Particle.Elasticity = 0.25f;
            Water_Particle.Mass = 1.f;

            SOLVER::Particles.push_back(Water_Particle);

            for (int i = 0; i < 3; i++) {
                Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
                Small_Water_Particle.Acceleration = FVector2(0.f, 0.f);
                Small_Water_Particle.Velocity = FVector2 (0.f, 0.0f);
                Small_Water_Particle.Elasticity = 0.25f;
                Small_Water_Particle.Mass = 1.f;

                SOLVER::Particles.push_back(Small_Water_Particle);
            }
        }

        if (IsKeyPressed(KEY_H)) {
            Particle StationaryParticle (FVector2(0.f, 0.f), GRAY, 64);

            StationaryParticle.Mass = 1000.f;
            StationaryParticle.Velocity = FVector2(0.f, 0.f);
            StationaryParticle.Acceleration = FVector2(0.f, 0.f);
            StationaryParticle.Elasticity = 1.f;

            SOLVER::Particles.push_back(StationaryParticle);
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

            SOLVER::QuadTree.Insert(Particle);


            // Draw Particle
            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.Position.X), (int)(Particle.Position.Y)));

            if (RENDERING_ENABLED) {
                if (!FAST_RENDERING) {
                    DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,
                               {Particle.Color.r, Particle.Color.g, Particle.Color.b, 127});
                    DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,
                                    {Particle.Color.r, Particle.Color.g, Particle.Color.b, 255});
                } else if (FAST_RENDERING) {
                    DrawRectangle(ParticleWindowLocation.X - (int) Particle.Radius,
                                  ParticleWindowLocation.Y - (int) Particle.Radius, 2 * (int) Particle.Radius,
                                  2 * (int) Particle.Radius,
                                  {Particle.Color.r, Particle.Color.g, Particle.Color.b, 127});
                    DrawRectangleLines(ParticleWindowLocation.X - (int) Particle.Radius,
                                       ParticleWindowLocation.Y - (int) Particle.Radius, 2 * (int) Particle.Radius,
                                       2 * (int) Particle.Radius,
                                       {Particle.Color.r, Particle.Color.g, Particle.Color.b, 255});
                }
            }
        }

        for (int substep = 0; substep < 10; substep++) {
            for (auto & i : SOLVER::Particles) {

                const std::vector<Particle*> OtherParticles = SOLVER::QuadTree.Search(GetParticleArea(i));

                for (const auto& P : OtherParticles) {

                    if (&i == P) continue;

                    SOLVER::SolveCollision(i, *P);

                    ObjectComparisons++;
                }
            }
        }

        if (true) DrawQuadTree(SOLVER::QuadTree);

        DrawRectangle(16, 16, 200, 168, {80, 80, 80, 100});
        DrawRectangleLines(16, 16, 200, 168, DARKGRAY);

        DrawText(std::string("FPS: " + std::to_string((int)std::ceil(1.f/GetFrameTime()))).c_str(), 32, 32, 8, RAYWHITE);

        DrawText(std::string("Objects: " + std::to_string((int)SOLVER::Particles.size())).c_str(), 32, 64, 8, RAYWHITE);
        DrawText(std::string("Comparisons: " + std::to_string((int)ObjectComparisons)).c_str(), 32, 80, 8, YELLOW);
        DrawText(std::string("Collisions: " + std::to_string((int)SOLVER::COLLISION_COUNT)).c_str(), 32, 96, 8, ORANGE);
        DrawText(std::string("QuadTree Searches: " + std::to_string((int)QT_PROF::SEARCH_COUNT)).c_str(), 32, 128, 8, RAYWHITE);
        DrawText(std::string("Overlap Tests: " + std::to_string((int)RECT_PROF::OVERLAP_TESTS)).c_str(), 32, 144, 8, RAYWHITE);
        DrawText(std::string("Contain Tests: " + std::to_string((int)RECT_PROF::CONTAIN_TESTS)).c_str(), 32, 160, 8, RAYWHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
