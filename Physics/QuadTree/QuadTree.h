#pragma once

#include <vector>
#include <array>
#include <memory>
#include <list>
#include <string>

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
    Rect m_Rect;
    int m_Depth;

    std::array<std::shared_ptr<QuadTree>, 4> m_TreePtrs{};
    std::vector<Particle*> m_ParticlePtrs;

public:
    QuadTree(Rect Area, int Depth) : m_Rect(Area), m_Depth(Depth) {}

    void Clear() { // NOLINT(*-no-recursion)
        m_ParticlePtrs.clear();

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) {
                m_TreePtrs[i]->Clear();
            }
            m_TreePtrs[i].reset();
        }
    }

    [[nodiscard]] int Size() const { // NOLINT(*-no-recursion)
        int Count = (int)m_ParticlePtrs.size();

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) Count += m_TreePtrs[i]->Size();
        }

        return Count;
    }

    void Insert(Particle& P) { // NOLINT(*-no-recursion)

        const Rect& ParticleSize = GetParticleArea(P);

        const FVector2 TreeSize = m_Rect.Size / 2;
        const std::array<Rect, 4> TreeRects = {
                Rect(m_Rect.Position, TreeSize),
                Rect(FVector2(m_Rect.Position.X + TreeSize.X, m_Rect.Position.Y), TreeSize),
                Rect(FVector2 (m_Rect.Position.X, m_Rect.Position.Y + TreeSize.Y), TreeSize),
                Rect(m_Rect.Position + TreeSize, TreeSize)
        };

        for (int i = 0; i < 4; i++) {
            if (TreeRects[i].Contains(ParticleSize)) {
                if (m_Depth + 1 < MAX_DEPTH) {
                    if (!m_TreePtrs[i]) {
                        m_TreePtrs[i] = std::make_shared<QuadTree>(TreeRects[i], m_Depth + 1);
                    }

                    m_TreePtrs[i]->Insert(P);
                    return;
                }
            }
        }

        m_ParticlePtrs.push_back(&P);
    }

    std::vector<Particle*> Search(const Rect& sArea) { // NOLINT(*-no-recursion)
        std::vector<Particle*> ParticleList;

        QT_PROF::SEARCH_COUNT++;

        // Check if area overlaps any particles in the current QuadTree, if they do, push them onto the list.

        // SLOW
        for (const auto& P : m_ParticlePtrs) {
            if (sArea.Overlaps(GetParticleArea(*P))) {
                ParticleList.push_back(P);
            }
        }

        const FVector2 TreeSize = m_Rect.Size / 2;
        const std::array<Rect, 4> TreeRects = {
                Rect(m_Rect.Position, TreeSize),
                Rect(FVector2(m_Rect.Position.X + TreeSize.X, m_Rect.Position.Y), TreeSize),
                Rect(FVector2 (m_Rect.Position.X, m_Rect.Position.Y + TreeSize.Y), TreeSize),
                Rect(m_Rect.Position + TreeSize, TreeSize)
        };

        // Iterate over possible child QuadTrees
        for (int i = 0; i < 4; i++) {
            // If child exists.
            if (m_TreePtrs[i]) {
                // This optimisation barely makes any difference, only increases the amount of Contain and Overlap tests.

                // If area fully contains child QuadTree, push all of its particles onto the list.
                if (sArea.Contains(TreeRects[i])) {
                    const std::vector<Particle*>& ChildParticles = m_TreePtrs[i]->GetParticles();
                    ParticleList.insert(ParticleList.end(), ChildParticles.begin(), ChildParticles.end());
                }

                // Or if the area only overlaps a part of the child QuadTree, then keep searching through it.
                else if (sArea.Overlaps(TreeRects[i])) {
                    const std::vector<Particle*>& ChildSearchParticles = m_TreePtrs[i]->Search(sArea);
                    ParticleList.insert(ParticleList.end(), ChildSearchParticles.begin(), ChildSearchParticles.end());
                }
            }
        }

        return ParticleList;
    }

    [[nodiscard]] std::vector<Particle*> GetParticles() const { // NOLINT(*-no-recursion)
        std::vector<Particle*> ParticleList;

        ParticleList.reserve(m_ParticlePtrs.size());

        for (const auto& P : m_ParticlePtrs) {
            ParticleList.push_back(P);
        }

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) {
                const std::vector<Particle*>& ChildParticles = m_TreePtrs[i]->GetParticles();
                ParticleList.insert(ParticleList.end(), ChildParticles.begin(), ChildParticles.end());
            }
        }

        return ParticleList;
    }

    [[nodiscard]] Rect GetArea() const {
        return m_Rect;
    }

    [[nodiscard]] std::vector<QuadTree*> GetChildren() const { // NOLINT(*-no-recursion)
        std::vector<QuadTree*> ChildTrees;

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) {
                ChildTrees.push_back(m_TreePtrs[i].get());
            }
        }

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) {
                const std::vector<QuadTree*>& ChildQuadTrees = m_TreePtrs[i]->GetChildren();
                ChildTrees.insert(ChildTrees.end(), ChildQuadTrees.begin(), ChildQuadTrees.end());
            }
        }

        return ChildTrees;
    }

    [[nodiscard]] std::vector<Rect> GetRects() const { // NOLINT(*-no-recursion)
        std::vector<Rect> ChildRects;

        const FVector2 TreeSize = m_Rect.Size / 2;
        const std::array<Rect, 4> TreeRects = {
                Rect(m_Rect.Position, TreeSize),
                Rect(FVector2(m_Rect.Position.X + TreeSize.X, m_Rect.Position.Y), TreeSize),
                Rect(FVector2 (m_Rect.Position.X, m_Rect.Position.Y + TreeSize.Y), TreeSize),
                Rect(m_Rect.Position + TreeSize, TreeSize)
        };

        ChildRects.reserve(TreeRects.size());

        for (auto& R : TreeRects) {
            ChildRects.push_back(R);
        }

        for (int i = 0; i < 4; i++) {
            if (m_TreePtrs[i]) {
                const std::vector<Rect>& Rects = m_TreePtrs[i]->GetRects();
                ChildRects.insert(ChildRects.end(), Rects.begin(), Rects.end());
            }
        }

        return ChildRects;
    }

    [[nodiscard]] int GetDepth() const {
        return m_Depth;
    }

    [[nodiscard]] std::vector<Particle*> GetDirectParticles() const {
        return m_ParticlePtrs;
    }
};

static void DrawQuadTree(QuadTree& Tree) {

    // Draw Possible Trees
    if (true) {
        for (const auto& R : Tree.GetRects()) {
            const IVector2 WorldChildPos = VIEWPORT::WorldToViewport(IVector2((int)R.Position.X, (int)R.Position.Y));

            DrawRectangleLines(WorldChildPos.X, WorldChildPos.Y - (int)R.Size.Y, (int)R.Size.X, (int)R.Size.Y, ORANGE);
            DrawRectangle(WorldChildPos.X, WorldChildPos.Y - (int)R.Size.Y, (int)R.Size.X, (int)R.Size.Y, { 255, 161, 0, 1 });
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

        for (const auto& P : T->GetDirectParticles()) {
            const IVector2 ViewportParticlePos = VIEWPORT::WorldToViewport(IVector2((int)P->Position.X, (int)P->Position.Y));

            DrawText(std::string(std::to_string(T->GetDepth())).c_str(), ViewportParticlePos.X - 4, ViewportParticlePos.Y - 4, 8, WHITE);
        }
    }

    if (false) for (const auto& P : Tree.GetParticles()) {
        const Rect& ParticleRect = GetParticleArea(*P);

        const IVector2 ViewportParticlePos = VIEWPORT::WorldToViewport(IVector2((int)ParticleRect.Position.X, (int)ParticleRect.Position.Y));

        DrawRectangleLines(ViewportParticlePos.X, ViewportParticlePos.Y - (int)ParticleRect.Size.Y, (int)ParticleRect.Size.X, (int)ParticleRect.Size.Y, GREEN);
        DrawRectangle(ViewportParticlePos.X, ViewportParticlePos.Y - (int)ParticleRect.Size.Y, (int)ParticleRect.Size.X, (int)ParticleRect.Size.Y, {0, 228, 48, 5});
    }
}
