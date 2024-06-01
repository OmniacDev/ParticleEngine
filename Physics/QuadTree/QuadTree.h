#pragma once

#include <vector>
#include <array>
#include <memory>
#include <list>

#include "../Particle.h"
#include "../../Engine/Math/Rect/Rect.h"
#include "../../Engine/Math/Viewport/Viewport.h"

const int MAX_DEPTH = 8;
const int MAX_CHILDREN = 4;

namespace QT_PROF {
    inline int SEARCH_COUNT = 0;
}

class QuadTree {
protected:
    Rect Area;
    int Depth;

    std::vector<Particle*> ParticlePtr_Arr;
    std::array<Rect, 4> ChildRect_Arr;
    std::array<std::shared_ptr<QuadTree>, 4> ChildTree_Arr{};

public:
    QuadTree(Rect Area, int Depth) : Area(Area), Depth(Depth) {
        Resize(Area);
    }

    void Resize(const Rect& nSize) {
        Clear();

        Area = nSize;

        FVector2 ChildSize = Area.Size / 2.0f;

        ChildRect_Arr = {
                Rect(Area.Position, ChildSize),
                Rect(FVector2 (Area.Position.X + ChildSize.X, Area.Position.Y), ChildSize),
                Rect(FVector2 (Area.Position.X, Area.Position.Y + ChildSize.Y), ChildSize),
                Rect(FVector2 (Area.Position.X + ChildSize.X, Area.Position.Y + ChildSize.Y), ChildSize)
        };
    }

    void Clear() { // NOLINT(*-no-recursion)
        ParticlePtr_Arr.clear();

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) {
                ChildTree_Arr[i]->Clear();
            }
            ChildTree_Arr[i].reset();
        }
    }

    [[nodiscard]] int Size() const { // NOLINT(*-no-recursion)
        int Count = (int)ParticlePtr_Arr.size();

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) Count += ChildTree_Arr[i]->Size();
        }

        return Count;
    }

    void Insert(Particle& P) { // NOLINT(*-no-recursion)

        const Rect& ParticleSize = GetParticleArea(P);

        for (int i = 0; i < 4; i++) {
            if (ChildRect_Arr[i].Contains(ParticleSize)) {
                if (Depth + 1 < MAX_DEPTH) {
                    if (!ChildTree_Arr[i]) {
                        ChildTree_Arr[i] = std::make_shared<QuadTree>(ChildRect_Arr[i], Depth + 1);
                    }

                    ChildTree_Arr[i]->Insert(P);
                    return;
                }
            }
        }

        ParticlePtr_Arr.push_back(&P);
    }

    std::vector<Particle*> Search(const Rect& sArea) { // NOLINT(*-no-recursion)
        std::vector<Particle*> ParticleList;

        QT_PROF::SEARCH_COUNT++;

        // Check if area overlaps any particles in the current QuadTree, if they do, push them onto the list.

        // SLOW
        for (const auto& P : ParticlePtr_Arr) {
            if (sArea.Overlaps(GetParticleArea(*P))) {
                ParticleList.push_back(P);
            }
        }

        // Iterate over possible child QuadTrees
        for (int i = 0; i < 4; i++) {
            // If child exists.
            if (ChildTree_Arr[i]) {
                // This optimisation barely makes any difference, only increases the amount of Contain and Overlap tests.

                // If area fully contains child QuadTree, push all of its particles onto the list.
                if (sArea.Contains(ChildRect_Arr[i])) {
                    const std::vector<Particle*>& ChildParticles = ChildTree_Arr[i]->GetParticles();
                    ParticleList.insert(ParticleList.end(), ChildParticles.begin(), ChildParticles.end());
                }

                // Or if the area only overlaps a part of the child QuadTree, then keep searching through it.
                else if (sArea.Overlaps(ChildRect_Arr[i])) {
                    const std::vector<Particle*>& ChildSearchParticles = ChildTree_Arr[i]->Search(sArea);
                    ParticleList.insert(ParticleList.end(), ChildSearchParticles.begin(), ChildSearchParticles.end());
                }
            }
        }

        return ParticleList;
    }

    std::vector<Particle*> GetParticles() { // NOLINT(*-no-recursion)
        std::vector<Particle*> ParticleList;

        ParticleList.reserve(ParticlePtr_Arr.size());

        for (const auto& P : ParticlePtr_Arr) {
            ParticleList.push_back(P);
        }

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) {
                const std::vector<Particle*>& ChildParticles = ChildTree_Arr[i]->GetParticles();
                ParticleList.insert(ParticleList.end(), ChildParticles.begin(), ChildParticles.end());
            }
        }

        return ParticleList;
    }

    [[nodiscard]] Rect GetArea() const {
        return Area;
    }

    std::vector<QuadTree*> GetChildren() { // NOLINT(*-no-recursion)
        std::vector<QuadTree*> ChildTrees;

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) {
                ChildTrees.push_back(ChildTree_Arr[i].get());
            }
        }

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) {
                const std::vector<QuadTree*>& ChildQuadTrees = ChildTree_Arr[i]->GetChildren();
                ChildTrees.insert(ChildTrees.end(), ChildQuadTrees.begin(), ChildQuadTrees.end());
            }
        }

        return ChildTrees;
    }

    std::vector<Rect*> GetRects() { // NOLINT(*-no-recursion)
        std::vector<Rect*> ChildRects;

        ChildRects.reserve(ChildRect_Arr.size());

        for (auto& R : ChildRect_Arr) {
            ChildRects.push_back(&R);
        }

        for (int i = 0; i < 4; i++) {
            if (ChildTree_Arr[i]) {
                const std::vector<Rect*>& Rects = ChildTree_Arr[i]->GetRects();
                ChildRects.insert(ChildRects.end(), Rects.begin(), Rects.end());
            }
        }

        return ChildRects;
    }
};

static void DrawQuadTree(QuadTree& Tree) {

    // Draw Possible Trees

    if (false) {
        for (const auto& R : Tree.GetRects()) {
            const IVector2 WorldChildPos = VIEWPORT::WorldToViewport(IVector2((int)R->Position.X, (int)R->Position.Y));

            DrawRectangleLines(WorldChildPos.X, WorldChildPos.Y - (int)R->Size.Y, (int)R->Size.X, (int)R->Size.Y, ORANGE);
            DrawRectangle(WorldChildPos.X, WorldChildPos.Y - (int)R->Size.Y, (int)R->Size.X, (int)R->Size.Y, { 255, 161, 0, 1 });
        }
    }


    // Draw Trees
    const Rect& TreeArea = Tree.GetArea();

    const IVector2 WorldTreePos = VIEWPORT::WorldToViewport(IVector2((int)TreeArea.Position.X, (int)TreeArea.Position.Y));

    DrawRectangleLines(WorldTreePos.X, WorldTreePos.Y - (int)TreeArea.Size.Y, (int)TreeArea.Size.X, (int)TreeArea.Size.Y, MAGENTA);
    DrawRectangle(WorldTreePos.X, WorldTreePos.Y - (int)TreeArea.Size.Y, (int)TreeArea.Size.X, (int)TreeArea.Size.Y, {255, 0, 255, 5});

    for (const auto& T : Tree.GetChildren()) {
        const Rect& ChildArea = T->GetArea();

        const IVector2 WorldChildPos = VIEWPORT::WorldToViewport(IVector2((int)ChildArea.Position.X, (int)ChildArea.Position.Y));

        DrawRectangleLines(WorldChildPos.X, WorldChildPos.Y - (int)ChildArea.Size.Y, (int)ChildArea.Size.X, (int)ChildArea.Size.Y, MAGENTA);
        DrawRectangle(WorldChildPos.X, WorldChildPos.Y - (int)ChildArea.Size.Y, (int)ChildArea.Size.X, (int)ChildArea.Size.Y, {255, 0, 255, 5});
    }
}
