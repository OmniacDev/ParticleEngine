#include "raylib.h"

#include <string>

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
        Water_Particle.Acceleration = FVector2(0.f, 0.f);
        Water_Particle.Velocity = FVector2 (0.f, 0.0f);
        Water_Particle.Elasticity = 0.25f;
        Water_Particle.Mass = 1.f;

        SOLVER::Particles.push_back(Water_Particle);

        SOLVER::QuadTree.Insert(QuadElement((int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)));

        for (int j = 0; j < 4; j++) {
            Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
            Small_Water_Particle.Acceleration = FVector2(0.f, 0.f);
            Small_Water_Particle.Velocity = FVector2 (0.f, 0.0f);
            Small_Water_Particle.Elasticity = 0.25f;
            Small_Water_Particle.Mass = 1.f;

            SOLVER::Particles.push_back(Small_Water_Particle);

            SOLVER::QuadTree.Insert(QuadElement((int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)));
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
        QT_PROF::LEAF_SEARCH_COUNT = 0;

        SOLVER::QuadTree.Cleanup();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            VIEWPORT::TranslateViewport(IVector2(static_cast<int>(GetMouseDelta().x), static_cast<int>(GetMouseDelta().y)));
        }

        if (IsKeyDown(KEY_N)) {
            Particle Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 8);
            Water_Particle.Acceleration = FVector2(0.f, -980.f);
            Water_Particle.Velocity = FVector2 (0.f, 0.0f);
            Water_Particle.Elasticity = 0.25f;
            Water_Particle.Mass = 1.f;

            SOLVER::Particles.push_back(Water_Particle);

            SOLVER::QuadTree.Insert(QuadElement((int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)));

            for (int i = 0; i < 3; i++) {
                Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
                Small_Water_Particle.Acceleration = FVector2(0.f, -980.f);
                Small_Water_Particle.Velocity = FVector2 (0.f, 0.0f);
                Small_Water_Particle.Elasticity = 0.25f;
                Small_Water_Particle.Mass = 1.f;

                SOLVER::Particles.push_back(Small_Water_Particle);

                SOLVER::QuadTree.Insert(QuadElement((int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)));
            }
        }

        if (IsKeyPressed(KEY_H)) {
            Particle StationaryParticle (FVector2(0.f, 0.f), GRAY, 64);

            StationaryParticle.Mass = 1000.f;
            StationaryParticle.Velocity = FVector2(0.f, 0.f);
            StationaryParticle.Acceleration = FVector2(0.f, -980.f);
            StationaryParticle.Elasticity = 1.f;

            SOLVER::Particles.push_back(StationaryParticle);

            SOLVER::QuadTree.Insert(QuadElement((int) SOLVER::Particles.size() - 1, GetParticleArea(StationaryParticle)));
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
            const auto& Leaves = SOLVER::QuadTree.FindLeaves(SOLVER::QuadTree.m_RootData, SOLVER::QuadTree.m_Rect);

            for (const auto& Leaf : Leaves) {
                    const QuadNode* LeafNode = &SOLVER::QuadTree.m_Nodes[Leaf.m_Index];

                    std::vector<int> ElementIndices;

                    {
                        int j = LeafNode->m_FirstIndex;

                        while (j != -1) {
                            const QuadElementNode* ElementNode = &SOLVER::QuadTree.m_ElementNodes[j];

                            ElementIndices.push_back(ElementNode->m_Index);
                            j = ElementNode->m_NextIndex;
                        }
                    }

                    for (const auto& j : ElementIndices) {

                        const int FirstParticleIndex = SOLVER::QuadTree.m_Elements[j].m_Index;
                        Particle& FirstParticle = SOLVER::Particles[FirstParticleIndex];

                        for (const auto& k : ElementIndices) {
                            if (j == k) continue;

                            const int SecondParticleIndex = SOLVER::QuadTree.m_Elements[k].m_Index;
                            Particle& SecondParticle = SOLVER::Particles[SecondParticleIndex];

                            SOLVER::SolveCollision(FirstParticle, SecondParticle);

                            ObjectComparisons++;
                        }
                    }

            }
        }

        for (int i = 0; i < SOLVER::QuadTree.m_Elements.Range(); i++) {
            Particle& Particle = SOLVER::Particles[SOLVER::QuadTree.m_Elements[i].m_Index];

            Particle.Update(DeltaTime);

            if (true) {
                SOLVER::QuadTree.UpdateElement(i, GetParticleArea(Particle));
            }
            else {
                SOLVER::QuadTree.Remove(i);
                SOLVER::QuadTree.Insert(QuadElement(SOLVER::QuadTree.m_Elements[i].m_Index, GetParticleArea(Particle)));
            }


            // Keep particle in bounds
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

            if (RENDERING_ENABLED) {
                if (FAST_RENDERING) {
                    DrawRectangle(ParticleWindowLocation.X - (int) Particle.Radius,
                                  ParticleWindowLocation.Y - (int) Particle.Radius, 2 * (int) Particle.Radius,
                                  2 * (int) Particle.Radius,
                                  {Particle.Color.r, Particle.Color.g, Particle.Color.b, 127});
                    DrawRectangleLines(ParticleWindowLocation.X - (int) Particle.Radius,
                                       ParticleWindowLocation.Y - (int) Particle.Radius, 2 * (int) Particle.Radius,
                                       2 * (int) Particle.Radius,
                                       {Particle.Color.r, Particle.Color.g, Particle.Color.b, 255});
                }
                else {
                    DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,
                               {Particle.Color.r, Particle.Color.g, Particle.Color.b, 127});
                    DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,
                                    {Particle.Color.r, Particle.Color.g, Particle.Color.b, 255});
                }
            }
        }

        if (true) DrawQuadTree(SOLVER::QuadTree);

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
//            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.Position.X), (int)(Particle.Position.Y)));
//
//            // DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,{0, 158, 47, 127});
//            DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.Radius,{0, 158, 47, 255});
//        }
//        Rect::DrawRect(VIEWPORT::WorldToViewport(SearchRect), SKYBLUE);




        DrawRectangle(16, 16, 200, 168, {80, 80, 80, 127});
        DrawRectangleLines(16, 16, 200, 168, DARKGRAY);

        DrawText(std::string("FPS: " + std::to_string((int)std::ceil(CURRENT_FPS))).c_str(), 32, 32, 8, RAYWHITE);
        DrawText(std::string("Avg FPS: " + std::to_string((int)std::floor(AVG_FPS))).c_str(), 32, 48, 8, RAYWHITE);

        DrawText(std::string("Objects: " + std::to_string((int)SOLVER::Particles.size())).c_str(), 32, 64, 8, RAYWHITE);
        DrawText(std::string("Comparisons: " + std::to_string((int)ObjectComparisons)).c_str(), 32, 80, 8, YELLOW);
        DrawText(std::string("Collisions: " + std::to_string((int)SOLVER::COLLISION_COUNT)).c_str(), 32, 96, 8, ORANGE);
        DrawText(std::string("QuadTree Searches: " + std::to_string(QT_PROF::SEARCH_COUNT + QT_PROF::LEAF_SEARCH_COUNT)).c_str(), 32, 128, 8, RAYWHITE);
        DrawText(std::string("Overlap Tests: " + std::to_string((int)RECT_PROF::OVERLAP_TESTS)).c_str(), 32, 144, 8, RAYWHITE);
        DrawText(std::string("Contain Tests: " + std::to_string((int)RECT_PROF::CONTAIN_TESTS)).c_str(), 32, 160, 8, RAYWHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
