#pragma once

#include <vector>
#include <array>
#include <list>
#include "../../Engine/Math/Rect/Rect.h"
#include "../../Engine/FreeList/FreeList.h"
#include "../../Engine/Math/Viewport/Viewport.h"

namespace QT_PROF {
    inline int SEARCH_COUNT = 0;
    inline int LEAF_SEARCH_COUNT = 0;
}

namespace QUAD_CONFIG {
    // Max amount of children a QuadNode can contain before it needs to subdivide.
    inline const int MAX_CHILDREN = 4;

    // Max amount of nested QuadNodes a QuadTree can have.
    inline const int MAX_DEPTH = 6;
}

static std::array<Rect, 4> QuadSubdivideRect(const Rect& nRect){
    const FVector2 SubSize = nRect.Size / 2;

    return {
            Rect(nRect.Position, SubSize),
            Rect(FVector2(nRect.Position.X + SubSize.X, nRect.Position.Y), SubSize),
            Rect(FVector2 (nRect.Position.X, nRect.Position.Y + SubSize.Y), SubSize),
            Rect(nRect.Position + SubSize, SubSize)
    };
}

class QuadElement {
public:
    // Index of Element (OUTSIDE).
    int m_Index;

    // Rect of Element.
    Rect m_Rect;

    QuadElement(int Index, const Rect& Rect) : m_Index(Index), m_Rect(Rect) {}
    QuadElement() : m_Index(), m_Rect() {}
};

class QuadElementNode {
public:
    // Index of referenced QuadElement.
    int m_Index;

    // Index of next QuadElementNode in list, or -1 if this is the last Element in list.
    int m_NextIndex;

    QuadElementNode(int Index, int NextIndex): m_Index(Index), m_NextIndex(NextIndex) {}
    QuadElementNode() : m_Index(), m_NextIndex() {}
};

class QuadNode {
public:
    // Index of first QuadElementNode (leaf) or QuadNode (branch).
    int m_FirstIndex;

    // Branch: N = -1, Leaf: N >= 0
    int m_Size;

    QuadNode(int FirstIndex, int Size) : m_FirstIndex(FirstIndex), m_Size(Size) {}
    QuadNode() : m_FirstIndex(), m_Size() {}
};

class QuadNodeData {
public:
    // Index of QuadNode
    int m_Index;

    // Depth of QuadNode
    int m_Depth;

    // Rect of QuadNode
    Rect m_Rect;

    QuadNodeData(int Index, int Depth, const Rect& Rect) : m_Index(Index), m_Depth(Depth), m_Rect(Rect) {}
    QuadNodeData() : m_Index(), m_Depth(), m_Rect() {}
};

class QuadTree {
public:
    // QuadNodes in tree, first node is always this tree's root.
    FreeList<QuadNode> m_Nodes;

    // QuadElements in tree.
    FreeList<QuadElement> m_Elements;

    // QuadElementNodes in tree, this is the actual value "stored" by the nodes.
    FreeList<QuadElementNode> m_ElementNodes;

    // QuadTree's rect.
    Rect m_Rect;

    // QuadTree's Root Data.
    QuadNodeData m_RootData;

public:
    explicit QuadTree(const Rect& Rect) : m_Rect(Rect) {
        QuadNode Root (-1, 0);

        m_RootData = QuadNodeData(0, 0, m_Rect);

        m_Nodes.Insert(Root);
    }

    void Insert(const QuadElement& Element) {
        const int ElementIndex = m_Elements.Insert(Element);

        NodeInsert(ElementIndex, m_RootData);
    }

    void Remove(const int ElementIndex) {
        std::vector<QuadNodeData> Leaves = FindLeaves(m_RootData, m_Elements[ElementIndex].m_Rect);

        for (const auto& Leaf : Leaves) {
            LeafRemove(ElementIndex, Leaf);
        }

        m_Elements.Erase(ElementIndex);
    }

    std::vector<int> Search(const Rect& Rect, int IgnoredElement = -1) {
        QT_PROF::SEARCH_COUNT++;

        std::vector<int> ElementIndices;
        std::vector<QuadNodeData> Leaves = FindLeaves(m_RootData, Rect);

        for (const auto& Leaf : Leaves) {
            int ElementNodeIndex = m_Nodes[Leaf.m_Index].m_FirstIndex;
            while (ElementNodeIndex != -1) {
                const int ElementIndex = m_ElementNodes[ElementNodeIndex].m_Index;
                if (ElementIndex != IgnoredElement && Rect.Overlaps(m_Elements[ElementIndex].m_Rect)) {
                    ElementIndices.push_back(ElementIndex);
                }
                ElementNodeIndex = m_ElementNodes[ElementNodeIndex].m_NextIndex;
            }
        }

        return ElementIndices;
    }

    void UpdateElement(const int ElementIndex, const Rect& NewRect) {
        QuadElement& Element = m_Elements[ElementIndex];
        const Rect& OldRect = Element.m_Rect;

        const Rect& CombinedRect = Rect::Combine(OldRect, NewRect);

        // Rect::DrawRect(VIEWPORT::WorldToViewport(CombinedRect), LIME, true);

        const auto& Leaves = FindLeaves(m_RootData, CombinedRect);

        for (const auto& Leaf : Leaves) {
            const bool NewOverlap = NewRect.Overlaps(Leaf.m_Rect);
            const bool OldOverlap = OldRect.Overlaps(Leaf.m_Rect);

            if (NewOverlap && OldOverlap) {
                // Rect::DrawRect(VIEWPORT::WorldToViewport(Leaf.m_Rect), YELLOW);
                continue;
            }

            else if (NewOverlap) {
                LeafInsert(ElementIndex, Leaf);
                // Rect::DrawRect(VIEWPORT::WorldToViewport(Leaf.m_Rect), {0, 228, 48, 127}, true);
            }
            else {
                LeafRemove(ElementIndex, Leaf);
                // Rect::DrawRect(VIEWPORT::WorldToViewport(Leaf.m_Rect), {230, 41, 55, 127}, true);
            }
        }

        Element.m_Rect = NewRect;
    }

    void Cleanup() {
        if (m_Nodes[0].m_Size == -1) {
            std::vector<int> ToProcess;

            ToProcess.push_back(0);

            while (!ToProcess.empty()) {
                const int Index = ToProcess.back();
                ToProcess.pop_back();

                QuadNode& Node = m_Nodes[Index];

                int EmptyLeaves = 0;
                for (int i = 0; i < 4; i++) {
                    const int ChildIndex = Node.m_FirstIndex + i;
                    const QuadNode& ChildNode = m_Nodes[ChildIndex];

                    if (ChildNode.m_Size == 0)
                        EmptyLeaves++;
                    else if (ChildNode.m_Size == -1)
                        ToProcess.push_back(ChildIndex);
                }

                if (EmptyLeaves == 4) {
                    for (int i = 3; i >= 0; i--) {
                        m_Nodes.Erase(Node.m_FirstIndex + i);
                    }

                    Node.m_Size = 0;
                    Node.m_FirstIndex = -1;
                }
            }
        }
    }

    std::vector<QuadNodeData> FindLeaves(const QuadNodeData& NodeData, const Rect& nRect) {
        std::vector<QuadNodeData> Leaves, ToProcess;

        ToProcess.push_back(NodeData);

        while (!ToProcess.empty())
        {
            QT_PROF::LEAF_SEARCH_COUNT++;

            const QuadNodeData nNodeData = ToProcess.back();
            ToProcess.pop_back();

            // If this node is a leaf, insert it to the list.
            QuadNode* Node = &m_Nodes[nNodeData.m_Index];

            if (Node->m_Size != -1) {
                Leaves.push_back(nNodeData);
            }

            // Otherwise push the children that intersect the rectangle
            else
            {
                const std::array<Rect, 4> ChildRects = QuadSubdivideRect(nNodeData.m_Rect);

                for (int i = 0; i < 4; i++) {
                    if (nRect.Overlaps(ChildRects[i])) {
                        QuadNodeData ChildNodeData;

                        ChildNodeData.m_Index = Node->m_FirstIndex + i;
                        ChildNodeData.m_Depth = nNodeData.m_Depth + 1;
                        ChildNodeData.m_Rect = ChildRects[i];

                        ToProcess.push_back(ChildNodeData);
                    }
                }
            }
        }

        return Leaves;
    }

    void LeafInsert(const int ElementIndex, const QuadNodeData& NodeData) { // NOLINT(*-no-recursion)
        // Get reference to node from QuadNodeData
        QuadNode* Node = &m_Nodes[NodeData.m_Index];

        const int FirstIndex = Node->m_FirstIndex;

        // Insert new element to the front of linked list
        Node->m_FirstIndex = m_ElementNodes.Insert(QuadElementNode(ElementIndex, FirstIndex));
        // Then update node's size
        Node->m_Size++;


        // If leaf is full, subdivide it
        if (Node->m_Size > QUAD_CONFIG::MAX_CHILDREN && NodeData.m_Depth < QUAD_CONFIG::MAX_DEPTH) {
            std::vector<int> ElementIndices;

            // Transfer indices of referenced elements to temporary array
            while (Node->m_FirstIndex != -1) {
                const int Index = Node->m_FirstIndex; // Get index of element node

                ElementIndices.push_back(m_ElementNodes[Index].m_Index); // Get element index from element node and push it into array

                const int NextIndex = m_ElementNodes[Index].m_NextIndex; // Get next index from element node

                // Remove element node from leaf
                Node->m_FirstIndex = NextIndex;

                // Then erase element node from tree
                m_ElementNodes.Erase(Index);
            }

            // Create 4 child nodes
            const int FirstChildIndex = m_Nodes.Insert(QuadNode(-1, 0));
            m_Nodes.Insert(QuadNode(-1, 0));
            m_Nodes.Insert(QuadNode(-1, 0));
            m_Nodes.Insert(QuadNode(-1, 0));

            // Get a new ptr cause old one is dirty after inserting new elements
            Node = &m_Nodes[NodeData.m_Index];
            Node->m_FirstIndex = FirstChildIndex;

            // Set leaf to branch
            Node->m_Size = -1;

            // Transfer elements from previous leaf into new branch's children
            for (const auto& nElementIndex : ElementIndices) {
                NodeInsert(nElementIndex, NodeData);
            }
        }
    }

    void LeafRemove(const int ElementIndex, const QuadNodeData& Leaf) {
        int NodeIndex = m_Nodes[Leaf.m_Index].m_FirstIndex;
        int PreviousIndex = -1;

        while (NodeIndex != -1 && m_ElementNodes[NodeIndex].m_Index != ElementIndex) {
            PreviousIndex = NodeIndex;
            NodeIndex = m_ElementNodes[NodeIndex].m_NextIndex;
        }

        if (NodeIndex != -1) {
            const int NextIndex = m_ElementNodes[NodeIndex].m_NextIndex;

            if (PreviousIndex == -1) {
                m_Nodes[Leaf.m_Index].m_FirstIndex = NextIndex;
            }
            else {
                m_ElementNodes[PreviousIndex].m_NextIndex = NextIndex;
            }
            m_ElementNodes.Erase(NodeIndex);

            m_Nodes[Leaf.m_Index].m_Size--;
        }
    }

    void NodeInsert(const int ElementIndex, const QuadNodeData& NodeData) { // NOLINT(*-no-recursion)
        const QuadElement& Element = m_Elements[ElementIndex];

        std::vector<QuadNodeData> Leaves = FindLeaves(NodeData, Element.m_Rect);

        for (const auto& Leaf : Leaves) {
            LeafInsert(ElementIndex, Leaf);
        }
    }

    std::vector<int> GetValidElementIndices() {
        std::vector<int> ElementIndices;
    }
};

static void DrawQuadTree(QuadTree& Tree) {
    std::vector<QuadNodeData> Leaves = Tree.FindLeaves(Tree.m_RootData, Tree.m_Rect);

    for (const auto& Leaf : Leaves) {
        const IVector2 ViewportPosition = VIEWPORT::WorldToViewport(IVector2((int) Leaf.m_Rect.Position.X, (int) Leaf.m_Rect.Position.Y));

        DrawRectangleLines(ViewportPosition.X, ViewportPosition.Y - (int) Leaf.m_Rect.Size.Y, (int) Leaf.m_Rect.Size.X, (int) Leaf.m_Rect.Size.Y, { 200, 200, 200, 127 });
    }
}