#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <string>
#include <set>
#include <array>
#include <deque>

#include "Engine/EngineGlobalVars.h"

#include "Physics/Particle.h"
#include "Physics/Solver.h"
#include "Engine/Math/Viewport/Viewport.h"
#include "Engine/Math/Rect/Rect.h"

bool FAST_RENDERING = false;
bool RENDERING_ENABLED = true;

float AVG_FPS = 0.f;

std::deque<float> FPS_Queue;

const FVector2 SearchSize (100.f, 100.f);

int main()
{
    sf::ContextSettings window_settings;
    window_settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(ENGINE::WindowWidth, ENGINE::WindowHeight), "[FOE] ParticleEngine", sf::Style::Default, window_settings);

    if (!ImGui::SFML::Init(window)) {
        return -1;
    }

    sf::Clock delta_clock;

    for (int i = 0; i < 100; i++) {
        Particle Water_Particle (FVector2(0.f, 0.f), sf::Color(0, 191, 255), 8);
        Water_Particle.acceleration = FVector2(0.f, 0.f);
        Water_Particle.velocity = FVector2 (0.f, 0.0f);
        Water_Particle.elasticity = 0.25f;
        Water_Particle.mass = 1.f;

        SOLVER::Particles.push_back(Water_Particle);

        SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)});

        for (int j = 0; j < 4; j++) {
            Particle Small_Water_Particle (FVector2(0.f, 0.f), sf::Color(0, 191, 255), 4);
            Small_Water_Particle.acceleration = FVector2(0.f, 0.f);
            Small_Water_Particle.velocity = FVector2 (0.f, 0.0f);
            Small_Water_Particle.elasticity = 0.25f;
            Small_Water_Particle.mass = 1.f;

            SOLVER::Particles.push_back(Small_Water_Particle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)});
        }
    }

    IVector2 last_mouse_pos = IVector2(0, 0);

    while (window.isOpen())
    {
        sf::Time DeltaTime = delta_clock.restart();
        float SafeDeltaTime = DeltaTime.asSeconds() > 1.f / ENGINE::WindowMinFPS ? 1.f / ENGINE::WindowMinFPS : DeltaTime.asSeconds();

        sf::Event event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    const IVector2 mouse_delta = IVector2(event.mouseMove.x, event.mouseMove.y) - last_mouse_pos;
                    VIEWPORT::TranslateViewport(mouse_delta);
                }
                last_mouse_pos = IVector2(event.mouseMove.x, event.mouseMove.y);
            }
        }

        window.clear(sf::Color::Black);

        ImGui::SFML::Update(window, DeltaTime);


        float CURRENT_FPS = (1 / (DeltaTime.asSeconds() <= 0 ? 1.f : DeltaTime.asSeconds()));

        if (FPS_Queue.size() >= 256) {
            FPS_Queue.pop_front();
        }
        FPS_Queue.push_back(CURRENT_FPS);

        AVG_FPS = 0.f;
        for (const auto& FPS : FPS_Queue) {
            AVG_FPS += FPS;
        }
        AVG_FPS /= 256;


        int ObjectComparisons = 0;
        RECT_PROF::OVERLAP_TESTS = 0;
        RECT_PROF::CONTAIN_TESTS = 0;
        SOLVER::COLLISION_COUNT = 0;
        QT_PROF::SEARCH_COUNT = 0;

        SOLVER::QuadTree.Cleanup();

//            if (IsKeyDown(KEY_N)) {
//                for (int i = 0; i < 8; i ++) {
//                    Particle Water_Particle(FVector2(0.f, 0.f), SKYBLUE, 8);
//                    // Water_Particle.acceleration = FVector2(0.f, -980.f);
//                    Water_Particle.velocity = FVector2(0.f, 0.0f);
//                    Water_Particle.elasticity = 0.25f;
//                    Water_Particle.mass = 1.f;
//
//                    SOLVER::Particles.push_back(Water_Particle);
//
//                    SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Water_Particle)});
//                }
//
////            for (int i = 0; i < 3; i++) {
////                Particle Small_Water_Particle (FVector2(0.f, 0.f), SKYBLUE, 4);
////                Small_Water_Particle.acceleration = FVector2(0.f, -980.f);
////                Small_Water_Particle.velocity = FVector2 (0.f, 0.0f);
////                Small_Water_Particle.elasticity = 0.25f;
////                Small_Water_Particle.mass = 1.f;
////
////                SOLVER::Particles.push_back(Small_Water_Particle);
////
////                SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(Small_Water_Particle)});
////            }
//            }

//            if (IsKeyPressed(KEY_H)) {
//                Particle StationaryParticle (FVector2(0.f, 0.f), GRAY, 64);
//
//                StationaryParticle.mass = 1000.f;
//                StationaryParticle.velocity = FVector2(0.f, 0.f);
//                StationaryParticle.acceleration = FVector2(0.f, -980.f);
//                StationaryParticle.elasticity = 1.f;
//
//                SOLVER::Particles.push_back(StationaryParticle);
//
//                SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(StationaryParticle)});
//            }

        for(int i = (GRID::Amount.X / -2); i <= (GRID::Amount.X / 2); i++) {
            int X = VIEWPORT::WorldToViewport(IVector2(i * GRID::Size, 0)).X;
            int Y1 = VIEWPORT::WorldToViewport(IVector2(0, (GRID::Amount.Y / -2) * GRID::Size)).Y;
            int Y2 = VIEWPORT::WorldToViewport(IVector2(0, (GRID::Amount.Y / 2) * GRID::Size)).Y;

            sf::Vertex one(sf::Vector2f((float)X, (float)Y1));
            sf::Vertex two(sf::Vector2f((float)X, (float)Y2));

            one.color = GRID::Color;
            two.color = GRID::Color;

            sf::Vertex line[] =
                    {
                            one,
                            two
                    };

            window.draw(line, 2, sf::Lines);
        }

        for(int i = (GRID::Amount.Y / -2); i <= (GRID::Amount.Y / 2); i++) {
            int Y = VIEWPORT::WorldToViewport(IVector2(0, i * GRID::Size)).Y;
            int X1 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / -2)) * GRID::Size, 0)).X;
            int X2 = VIEWPORT::WorldToViewport(IVector2(((GRID::Amount.X / 2)) * GRID::Size, 0)).X;

            sf::Vertex one(sf::Vector2f((float)X1, (float)Y));
            sf::Vertex two(sf::Vector2f((float)X2, (float)Y));
            one.color = GRID::Color;
            two.color = GRID::Color;

            sf::Vertex line[] =
                    {
                            one,
                            two
                    };

            window.draw(line, 2, sf::Lines);
        }

        for (int substep = 0; substep < 4; substep++) {
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

            Particle.Update(SafeDeltaTime);

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

            sf::CircleShape particle(Particle.radius-1.0f);
            particle.setPosition((float)ParticleWindowLocation.X - Particle.radius, (float)ParticleWindowLocation.Y - Particle.radius);
            particle.setFillColor({Particle.color.r, Particle.color.g, Particle.color.b, 127});
            particle.setOutlineThickness(1.0f);
            particle.setOutlineColor({Particle.color.r, Particle.color.g, Particle.color.b, 255});

            window.draw(particle);
        }

//            if (true) DrawQuadTree(SOLVER::QuadTree);

//            const IVector2 MouseWorldPos = VIEWPORT::ViewportToWorld(IVector2(GetMouseX(), GetMouseY()));
//            const FVector2 MouseFloatWorldPos ((float)MouseWorldPos.X, (float)MouseWorldPos.Y);
//
//            const Rect SearchRect (MouseFloatWorldPos - (SearchSize / 2), SearchSize);
//
//            const std::vector<int> SearchResults = SOLVER::QuadTree.Search(SearchRect);
//
//            for (const auto& Result : SearchResults) {
//                const QuadElement& Element = SOLVER::QuadTree.m_Elements[Result];
//
//                const Particle& Particle = SOLVER::Particles[Element.m_Index];
//
//                IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.position.X), (int)(Particle.position.Y)));
//
//                // DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 127});
//                DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 255});
//            }
//            Rect::DrawRect(VIEWPORT::WorldToViewport(SearchRect), SKYBLUE);

        ImGui::Begin("Debug Info");
        ImGui::Text(std::string("FPS: " + std::to_string((int)std::ceil(CURRENT_FPS))).c_str());
        ImGui::Text(std::string("Avg FPS: " + std::to_string((int)std::floor(AVG_FPS))).c_str());
        ImGui::PlotLines("FPS Graph",std::vector<float>(FPS_Queue.begin(), FPS_Queue.end()).data(), (int)FPS_Queue.size());
        ImGui::Spacing();
        ImGui::Text(std::string("Objects: " + std::to_string((int)SOLVER::Particles.size())).c_str());
        ImGui::Text(std::string("Comparisons: " + std::to_string((int)ObjectComparisons)).c_str());
        ImGui::Text(std::string("Collisions: " + std::to_string((int)SOLVER::COLLISION_COUNT)).c_str());
        ImGui::Text(std::string("QuadTree Searches: " + std::to_string(QT_PROF::SEARCH_COUNT)).c_str());
        ImGui::Spacing();
        ImGui::Text(std::string("Overlap Tests: " + std::to_string((int)RECT_PROF::OVERLAP_TESTS)).c_str());
        ImGui::Text(std::string("Contain Tests: " + std::to_string((int)RECT_PROF::CONTAIN_TESTS)).c_str());
        ImGui::End();

        // end the current frame
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
