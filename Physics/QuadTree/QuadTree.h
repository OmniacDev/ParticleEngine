#pragma once

#include <vector>
#include <array>
#include <memory>
#include <list>

#include "../Particle.h"
#include "../../Engine/Math/Rect/Rect.h"

const int MAX_DEPTH = 4;

class QuadTree {
protected:
    Rect Area;
    int Depth;

    std::vector<Particle*> Particles;
    std::array<Rect, 4> ChildRects;
    std::array<std::shared_ptr<QuadTree>, 4> Children{};

public:
    QuadTree(Rect Area, int Depth) : Area(Area), Depth(Depth) {}

    void Resize(const Rect& nSize) {
        Clear();

        Area = nSize;

        FVector2 ChildSize = Area.Size / 2.0f;

        ChildRects = {
                Rect(Area.Position, ChildSize),
                Rect(FVector2 (Area.Position.X + ChildSize.X, Area.Position.Y), ChildSize),
                Rect(FVector2 (Area.Position.X, Area.Position.Y + ChildSize.Y), ChildSize),
                Rect(Area.Position + ChildSize, ChildSize)
        };
    }

    void Clear() { // NOLINT(*-no-recursion)
        Particles.clear();

        for (int i = 0; i < 4; i++) {
            if (Children[i]) {
                Children[i]->Clear();
            }
            Children[i].reset();
        }
    }

    [[nodiscard]] int Size() const { // NOLINT(*-no-recursion)
        int Count = (int)Particles.size();

        for (int i = 0; i < 4; i++) {
            if (Children[i]) Count += Children[i]->Size();
        }

        return Count;
    }

    void Insert(Particle& P) { // NOLINT(*-no-recursion)

        const Rect ParticleSize(FVector2(P.Position.X - P.Radius, P.Position.Y - P.Radius), FVector2(P.Position.X + P.Radius, P.Position.Y + P.Radius));

        for (int i = 0; i < 4; i++) {
            if (ChildRects[i].Contains(ParticleSize)) {
                if (Depth + 1 < MAX_DEPTH) {
                    if (!Children[i]) {
                        Children[i] = std::make_shared<QuadTree>(ChildRects[i], Depth + 1);
                    }

                    Children[i]->Insert(P);
                    return;
                }
            }
        }

        Particles.push_back(&P);
    }

    std::list<Particle*> Search(const Rect& sArea) { // NOLINT(*-no-recursion)
        std::list<Particle*> ParticleList;

        for (const auto& P : Particles) {

            const Rect ParticleSize(FVector2(P->Position.X - P->Radius, P->Position.Y - P->Radius), FVector2(P->Position.X + P->Radius, P->Position.Y + P->Radius));

            if (sArea.Overlaps(ParticleSize)) {
                ParticleList.push_back(P);
            }
        }

        for (int i = 0; i < 4; i++) {
            if (Children[i]) {
                if (sArea.Contains(ChildRects[i])) {
                    ParticleList.merge(Children[i]->GetParticles());
                }

                else if (sArea.Overlaps(ChildRects[i])) {
                    ParticleList.merge(Children[i]->Search(sArea));
                }
            }
        }

        return ParticleList;
    }

    std::list<Particle*> GetParticles() { // NOLINT(*-no-recursion)
        std::list<Particle*> ParticleList;
        for (const auto& P : Particles) {
            ParticleList.push_back(P);
        }

        for (int i = 0; i < 4; i++) {
            if (Children[i]) {
                ParticleList.merge(Children[i]->GetParticles());
            }
        }

        return ParticleList;
    }

    [[nodiscard]] Rect GetArea() const {
        return Area;
    }
};
