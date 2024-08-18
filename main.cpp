#include "raylib.h"

#include <string>
#include <set>

#include "Engine/EngineGlobalVars.h"

#include "Physics/Particle.h"
#include "Physics/Solver.h"
#include "Engine/Math/Viewport/Viewport.h"
#include "Engine/Math/Rect/Rect.h"

bool FAST_RENDERING = false;
bool RENDERING_ENABLED = true;

int AVG_FPS = 0;

std::array<int, 256> FPS_Arr;
unsigned char FPS_Arr_Index = 0;

const FVector2 SearchSize (100.f, 100.f);

int main()
{
    SetConfigFlags(ENGINE::ConfigFlags);
    SetTargetFPS(ENGINE::WindowFPS);

    InitWindow(ENGINE::WindowWidth, ENGINE::WindowHeight, "[FOE] Physics Engine");

    for (int i = 0; i < 200; i++) {
        Particle Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 8);
        Water_Particle.acceleration = FVector2(0.f, 0.f);
        Water_Particle.velocity = FVector2 (0.f, 0.0f);
        Water_Particle.elasticity = 0.25f;
        Water_Particle.mass = 1.f;

        SOLVER::Particles.push_back(Water_Particle);

        SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)});

        for (int j = 0; j < 4; j++) {
            Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
            Small_Water_Particle.acceleration = FVector2(0.f, 0.f);
            Small_Water_Particle.velocity = FVector2 (0.f, 0.0f);
            Small_Water_Particle.elasticity = 0.25f;
            Small_Water_Particle.mass = 1.f;

            SOLVER::Particles.push_back(Small_Water_Particle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)});
        }
    }

    while (!WindowShouldClose())
    {
        float DeltaTime = GetFrameTime() > 1.f / ENGINE::WindowMinFPS ? 1.f / ENGINE::WindowMinFPS : GetFrameTime();

        int CURRENT_FPS = (int)(1 / (GetFrameTime() <= 0 ? 1.f : GetFrameTime()));

        FPS_Arr[FPS_Arr_Index] = CURRENT_FPS;
        FPS_Arr_Index++;

        AVG_FPS = 0;
        for (const auto& FPS : FPS_Arr) {
            AVG_FPS += FPS;
        }
        AVG_FPS /= 256;


        int ObjectComparisons = 0;
        RECT_PROF::OVERLAP_TESTS = 0;
        RECT_PROF::CONTAIN_TESTS = 0;
        SOLVER::COLLISION_COUNT = 0;
        QT_PROF::SEARCH_COUNT = 0;

        SOLVER::QuadTree.Cleanup();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            VIEWPORT::TranslateViewport(IVector2(static_cast<int>(GetMouseDelta().x), static_cast<int>(GetMouseDelta().y)));
        }

        if (IsKeyDown(KEY_N)) {
            Particle Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 8);
            Water_Particle.acceleration = FVector2(0.f, -980.f);
            Water_Particle.velocity = FVector2 (0.f, 0.0f);
            Water_Particle.elasticity = 0.25f;
            Water_Particle.mass = 1.f;

            SOLVER::Particles.push_back(Water_Particle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)});

            for (int i = 0; i < 3; i++) {
                Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
                Small_Water_Particle.acceleration = FVector2(0.f, -980.f);
                Small_Water_Particle.velocity = FVector2 (0.f, 0.0f);
                Small_Water_Particle.elasticity = 0.25f;
                Small_Water_Particle.mass = 1.f;

                SOLVER::Particles.push_back(Small_Water_Particle);

                SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)});
            }
        }

        if (IsKeyPressed(KEY_H)) {
            Particle StationaryParticle (FVector2(0.f, 0.f), GRAY, 64);

            StationaryParticle.mass = 1000.f;
            StationaryParticle.velocity = FVector2(0.f, 0.f);
            StationaryParticle.acceleration = FVector2(0.f, -980.f);
            StationaryParticle.elasticity = 1.f;

            SOLVER::Particles.push_back(StationaryParticle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(StationaryParticle)});
        }

        BeginDrawing();

        ClearBackground(BLACK);

        for(int i = (GRID::Amount.X / -2); i <= (GRID::Amount.X / 2); i++) {
            int X = VIEWPORT::WorldToViewport(IVector2(i * GRID::Size, 0)).X;
            int Y1 = VIEWPORT::WorldToViewport(IVector2(0, (GRID::Amount.Y / -2) * GRID::Size)).Y;
            int Y2 = VIEWPORT::WorldToViewport(IVector2(0, (GRID::Amount.Y / 2) * GRID::Size)).Y;
            DrawLine(X, Y1, X, Y2, GRID::Color);
        }

        for(int i = (GRID::Amount.Y / -2); i <= (GRID::Amount.Y / 2); i++) {
            int Y = VIEWPORT::WorldToViewport(IVector2(0, i * GRID::Size)).Y;
            int X1 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / -2)) * GRID::Size, 0)).X;
            int X2 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / 2)) * GRID::Size, 0)).X;
            DrawLine(X1, Y, X2, Y, GRID::Color);
        }

        for (int substep = 0; substep < 1; substep++) {
            for (int i = 0; i < SOLVER::QuadTree.nodes.Range(); i++) {
                if (SOLVER::QuadTree.nodes[i].num == -1) continue;

                std::vector<int> indices;
                Quad::GetElementIndices(&SOLVER::QuadTree, &SOLVER::QuadTree.nodes[i], indices);

                for (const int j : indices) {
                    for (const int k : indices) {
                        if (j == k) continue;

                        Particle& FirstParticle = SOLVER::Particles[SOLVER::QuadTree.elements[j].index];
                        Particle& SecondParticle = SOLVER::Particles[SOLVER::QuadTree.elements[k].index];

                        SOLVER::SolveCollision(FirstParticle, SecondParticle);
                        ObjectComparisons++;
                    }
                }
            }
        }

        for (int i = 0; i < SOLVER::QuadTree.elements.Range(); i++) {
            Particle& Particle = SOLVER::Particles[SOLVER::QuadTree.elements[i].index];

            Particle.Update(DeltaTime);

            SOLVER::QuadTree.Remove(i);
            SOLVER::QuadTree.Insert({SOLVER::QuadTree.elements[i].index, GetParticleArea(Particle)});

            // Keep particle in bounds
            if (Particle.position.X + Particle.radius > BOUNDS::X_POS) {
                Particle.position.X = BOUNDS::X_POS - Particle.radius;
                Particle.velocity.X = Particle.velocity.X * -Particle.elasticity;
            }
            if (Particle.position.X - Particle.radius < BOUNDS::X_NEG) {
                Particle.position.X = BOUNDS::X_NEG + Particle.radius;
                Particle.velocity.X = Particle.velocity.X * -Particle.elasticity;
            }
            if (Particle.position.Y + Particle.radius > BOUNDS::Y_POS) {
                Particle.position.Y = BOUNDS::Y_POS - Particle.radius;
                Particle.velocity.Y = Particle.velocity.Y * -Particle.elasticity;
            }
            if (Particle.position.Y - Particle.radius < BOUNDS::Y_NEG) {
                Particle.position.Y = BOUNDS::Y_NEG + Particle.radius;
                Particle.velocity.Y = Particle.velocity.Y * -Particle.elasticity;
            }

            // Draw Particle
            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.position.X), (int)(Particle.position.Y)));

            if (RENDERING_ENABLED) {
                if (FAST_RENDERING) {
                    DrawRectangle(ParticleWindowLocation.X - (int) Particle.radius,
                                  ParticleWindowLocation.Y - (int) Particle.radius, 2 * (int) Particle.radius,
                                  2 * (int) Particle.radius,
                                  {Particle.color.r, Particle.color.g, Particle.color.b, 127});
                    DrawRectangleLines(ParticleWindowLocation.X - (int) Particle.radius,
                                       ParticleWindowLocation.Y - (int) Particle.radius, 2 * (int) Particle.radius,
                                       2 * (int) Particle.radius,
                                       {Particle.color.r, Particle.color.g, Particle.color.b, 255});
                }
                else {
                    DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,
                               {Particle.color.r, Particle.color.g, Particle.color.b, 127});
                    DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,
                                    {Particle.color.r, Particle.color.g, Particle.color.b, 255});
                }
            }
        }

//        if (true) DrawQuadTree(SOLVER::QuadTree);

//        const IVector2 MouseWorldPos = VIEWPORT::ViewportToWorld(IVector2(GetMouseX(), GetMouseY()));
//        const FVector2 MouseFloatWorldPos ((float)MouseWorldPos.X, (float)MouseWorldPos.Y);
//
//        const Rect SearchRect (MouseFloatWorldPos - (SearchSize / 2), SearchSize);
//
//        const std::vector<int> SearchResults = SOLVER::QuadTree.Search(SearchRect);
//
//        for (const auto& Result : SearchResults) {
//            const QuadElement& Element = SOLVER::QuadTree.m_Elements[Result];
//
//            const Particle& Particle = SOLVER::Particles[Element.m_Index];
//
//            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.position.X), (int)(Particle.position.Y)));
//
//            // DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 127});
//            DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 255});
//        }
//        Rect::DrawRect(VIEWPORT::WorldToViewport(SearchRect), SKYBLUE);




        DrawRectangle(16, 16, 200, 168, {80, 80, 80, 127});
        DrawRectangleLines(16, 16, 200, 168, DARKGRAY);

        DrawText(std::string("FPS: " + std::to_string((int)std::ceil(CURRENT_FPS))).c_str(), 32, 32, 8, RAYWHITE);
        DrawText(std::string("Avg FPS: " + std::to_string((int)std::floor(AVG_FPS))).c_str(), 32, 48, 8, RAYWHITE);

        DrawText(std::string("Objects: " + std::to_string((int)SOLVER::Particles.size())).c_str(), 32, 64, 8, RAYWHITE);
        DrawText(std::string("Comparisons: " + std::to_string((int)ObjectComparisons)).c_str(), 32, 80, 8, YELLOW);
        DrawText(std::string("Collisions: " + std::to_string((int)SOLVER::COLLISION_COUNT)).c_str(), 32, 96, 8, ORANGE);
        DrawText(std::string("QuadTree Searches: " + std::to_string(QT_PROF::SEARCH_COUNT)).c_str(), 32, 128, 8, RAYWHITE);
        DrawText(std::string("Overlap Tests: " + std::to_string((int)RECT_PROF::OVERLAP_TESTS)).c_str(), 32, 144, 8, RAYWHITE);
        DrawText(std::string("Contain Tests: " + std::to_string((int)RECT_PROF::CONTAIN_TESTS)).c_str(), 32, 160, 8, RAYWHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
