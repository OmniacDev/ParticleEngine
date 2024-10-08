#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <string>
#include <deque>

#include "Engine/EngineGlobalVars.h"

#include "Physics/Particle.h"
#include "Physics/Solver.h"
#include "Engine/Math/Viewport/Viewport.h"
#include "Engine/Math/Rect/Rect.h"
// #include "Engine/SFML/VertexArray.h"

// #include "Engine/Shaders/Shaders.h"

//bool FAST_RENDERING = false;
//bool RENDERING_ENABLED = true;

float AVG_FPS = 0.f;

std::deque<float> FPS_Queue;

//const FVector2 SearchSize (100.f, 100.f);

int main()
{
//    sf::Shader particle_shader;
//    particle_shader.loadFromMemory(particle_vs, particle_fs);

    sf::Texture particle_texture;
    particle_texture.loadFromFile("Resources/circle.png");
    particle_texture.generateMipmap();
    particle_texture.setSmooth(true);

    sf::VertexArray particle_vertices(sf::Quads);

    sf::ContextSettings window_settings;
    window_settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(ENGINE::WindowWidth, ENGINE::WindowHeight), "[FOE] ParticleEngine", sf::Style::Default, window_settings);

    if (!ImGui::SFML::Init(window)) {
        return -1;
    }

    sf::Clock delta_clock;

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

            if (event.type == sf::Event::Resized) {
                const sf::View old_view = window.getView();
                window.setView(sf::View(sf::FloatRect (0.f, 0.f, (float)event.size.width, (float)event.size.height)));
                const sf::Vector2f view_translation = window.getView().getCenter() - old_view.getCenter();
                VIEWPORT::TranslateViewport(IVector2((int)view_translation.x, (int)view_translation.y));
            }

            if (event.type == sf::Event::MouseMoved) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
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

        SOLVER::QuadTree.Cleanup();

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

        for (int substep = 0; substep < 12; substep++) {
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

        particle_vertices.resize(SOLVER::QuadTree.elements.Range() * 4);
        const float texture_size = 1024.f;

        for (int i = 0; i < SOLVER::QuadTree.elements.Range(); i++) {
            const int index = SOLVER::QuadTree.elements[i].index;
            Particle& Particle = SOLVER::Particles[index];

            Particle.Update(SafeDeltaTime);

            SOLVER::QuadTree.Remove(i);
            SOLVER::QuadTree.Insert({index, GetParticleArea(Particle)});

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

            const uint32 idx = i << 2;

            particle_vertices[idx + 0].position = {(float)ParticleWindowLocation.X - Particle.radius, (float)ParticleWindowLocation.Y - Particle.radius };
            particle_vertices[idx + 1].position = {(float)ParticleWindowLocation.X + Particle.radius, (float)ParticleWindowLocation.Y - Particle.radius };
            particle_vertices[idx + 2].position = {(float)ParticleWindowLocation.X + Particle.radius, (float)ParticleWindowLocation.Y + Particle.radius };
            particle_vertices[idx + 3].position = {(float)ParticleWindowLocation.X - Particle.radius, (float)ParticleWindowLocation.Y + Particle.radius };
            particle_vertices[idx + 0].texCoords = {0.0f        , 0.0f};
            particle_vertices[idx + 1].texCoords = {texture_size, 0.0f};
            particle_vertices[idx + 2].texCoords = {texture_size, texture_size};
            particle_vertices[idx + 3].texCoords = {0.0f        , texture_size};

            const sf::Color color = Particle.color;
            particle_vertices[idx + 0].color = color;
            particle_vertices[idx + 1].color = color;
            particle_vertices[idx + 2].color = color;
            particle_vertices[idx + 3].color = color;
        }

        sf::RenderStates particle_r_state;
        particle_r_state.texture = &particle_texture;
        
        window.draw(particle_vertices, particle_r_state);

//        particle_shader.setUniform("time", SafeDeltaTime);
//        window.draw(vertices);

//         window.draw(vertices);
//         window.draw(outline_vertices);

        static bool draw_quad = false;
        ImGui::Begin("Debug Info");
        ImGui::Text("%s", std::string("FPS: " + std::to_string((int)std::ceil(CURRENT_FPS))).c_str());
        ImGui::Text("%s", std::string("Avg FPS: " + std::to_string((int)std::floor(AVG_FPS))).c_str());
        ImGui::PlotLines("FPS Graph",std::vector<float>(FPS_Queue.begin(), FPS_Queue.end()).data(), (int)FPS_Queue.size());
//        ImGui::Spacing();
//        ImGui::Spacing();
//        ImGui::Spacing();
//        ImGui::Text(std::string("Vertices: " + std::to_string((int)vertices.getVertexCount())).c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("%s", std::string("Objects: " + std::to_string((int)SOLVER::Particles.size())).c_str());
        ImGui::Text("%s", std::string("Comparisons: " + std::to_string((int)ObjectComparisons)).c_str());
        ImGui::Text("%s", std::string("Collisions: " + std::to_string((int)SOLVER::COLLISION_COUNT)).c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("%s", std::string("Overlap Tests: " + std::to_string((int)RECT_PROF::OVERLAP_TESTS)).c_str());
        ImGui::Text("%s", std::string("Contain Tests: " + std::to_string((int)RECT_PROF::CONTAIN_TESTS)).c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Checkbox("Draw QuadTree", &draw_quad);
        ImGui::End();

        static float particle_radius = 8.f;
        static float particle_mass = 100.f;
        static float particle_color[4] = {0.f / 255, 191.f / 255, 255.f / 255, 255.f / 255};
        static float particle_position[2] = { 0.f, 0.f };
        static float particle_velocity[2] = { 0.f, 0.f};
        static float particle_acceleration[2] = { 0.f, -980.f };
        ImGui::Begin("Spawn Particle");
        ImGui::InputFloat("Radius", &particle_radius);
        ImGui::InputFloat("Mass", &particle_mass);
        ImGui::InputFloat2("Position", particle_position);
        ImGui::InputFloat2("Velocity", particle_velocity);
        ImGui::InputFloat2("Acceleration", particle_acceleration);
        ImGui::ColorEdit4("Color", particle_color);
        ImGui::Button("Spawn One");
        if (ImGui::IsItemClicked()) {
            sf::Color color = sf::Color((int)(particle_color[0] * 255), (int)(particle_color[1] * 255), (int)(particle_color[2] * 255), (int)(particle_color[3] * 255));
            Particle particle (FVector2(particle_position[0], particle_position[1]), color, particle_radius);

            particle.mass = particle_mass;
            particle.velocity = FVector2(particle_velocity[0], particle_velocity[1]);
            particle.acceleration = FVector2(particle_acceleration[0], particle_acceleration[1]);
            particle.elasticity = 1.f;

            SOLVER::Particles.push_back(particle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(particle)});
        }
        ImGui::Button("Spawn Multiple");
        if (ImGui::IsItemActive()) {
            sf::Color color = sf::Color((int)(particle_color[0] * 255), (int)(particle_color[1] * 255), (int)(particle_color[2] * 255), (int)(particle_color[3] * 255));
            Particle particle (FVector2(particle_position[0], particle_position[1]), color, particle_radius);

            particle.mass = particle_mass;
            particle.velocity = FVector2(particle_velocity[0], particle_velocity[1]);
            particle.acceleration = FVector2(particle_acceleration[0], particle_acceleration[1]);
            particle.elasticity = 1.f;

            SOLVER::Particles.push_back(particle);

            SOLVER::QuadTree.Insert({(int) SOLVER::Particles.size() - 1, GetParticleArea(particle)});
        }
        ImGui::End();

        static bool delete_particles = false;
        static float delete_radius = 100.f;
        ImGui::Begin("Interaction");
        ImGui::Checkbox("Delete", &delete_particles);
        ImGui::InputFloat("Delete Radius", &delete_radius);
        ImGui::End();

        if (delete_particles) {
            const IVector2 MouseWorldPos = VIEWPORT::ViewportToWorld(IVector2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
            const FVector2 MouseFloatWorldPos ((float)MouseWorldPos.X, (float)MouseWorldPos.Y);

            const Rect SearchRect (MouseFloatWorldPos - FVector2(delete_radius / 2, -delete_radius / 2), FVector2(delete_radius, delete_radius));

//        const auto& SearchResults = SOLVER::QuadTree.FindLeaves(SearchRect, SOLVER::QuadTree.root_data);
//        for (const auto& leaf : SearchResults) {
//            const QuadElement& Element = SOLVER::QuadTree.m_Elements[leaf];
//
//            const Particle& Particle = SOLVER::Particles[Element.m_Index];
//
//            IVector2 ParticleWindowLocation = VIEWPORT::WorldToViewport(IVector2((int)(Particle.position.X), (int)(Particle.position.Y)));
//
//            // DrawCircle(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 127});
//            DrawCircleLines(ParticleWindowLocation.X, ParticleWindowLocation.Y, Particle.radius,{0, 158, 47, 255});
//        }

            window.draw(Rect::DrawRect(VIEWPORT::WorldToViewport(SearchRect), {0, 127, 255, 255}));
        }

        if (draw_quad) {
            const auto& leaves = SOLVER::QuadTree.FindLeaves(SOLVER::QuadTree.root_data.rect, SOLVER::QuadTree.root_data);

            for (const auto& leaf : leaves) {
                const IVector2 viewport_pos = VIEWPORT::WorldToViewport(IVector2((int)leaf.rect.Position.X, (int)leaf.rect.Position.Y));

                sf::RectangleShape leaf_shape;
                leaf_shape.setSize({leaf.rect.Size.X,leaf.rect.Size.Y});
                leaf_shape.setPosition((float)viewport_pos.X, (float)viewport_pos.Y - leaf.rect.Size.Y);
                leaf_shape.setFillColor({0, 0, 0, 0});
                leaf_shape.setOutlineThickness(-1.0f);

                if (SOLVER::QuadTree.nodes[leaf.index].num > 4) {
                    leaf_shape.setOutlineColor({255, 0, 0, 127});
                }
                else if (SOLVER::QuadTree.nodes[leaf.index].num > 1) {
                    leaf_shape.setOutlineColor({255, 255, 0, 127});
                }
                else if (SOLVER::QuadTree.nodes[leaf.index].num == 1) {
                    leaf_shape.setOutlineColor({0, 255, 0, 127});
                }
                else {
                    leaf_shape.setOutlineColor({0, 255, 255, 64});
                }

                window.draw(leaf_shape);
            }
        }

        // end the current frame
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
