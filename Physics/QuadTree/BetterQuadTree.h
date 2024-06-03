#pragma once

#include <vector>
#include <array>
#include <list>
#include "../../Engine/Math/Rect/Rect.h"
#include "../../Engine/FreeList/FreeList.h"

namespace QUAD_CONFIG {
    // Max amount of children a QuadNode can contain before it needs to subdivide.
    inline const int MAX_CHILDREN = 4;

    // Max amount of nested QuadNodes a QuadTree can have.
    inline const int MAX_DEPTH = 8;
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

struct QuadElement {
    // Index of Element (OUTSIDE).
    int m_Index;

    // Rect of Element.
    Rect m_Rect;
};

struct QuadElementNode {
    // Index of referenced QuadElement.
    int m_Index;

    // Index of next QuadElementNode in list, or -1 if this is the last Element in list.
    int m_NextIndex;
};

struct QuadNode {
    // Index of first QuadElementNode (leaf) or QuadNode (branch).
    int m_FirstIndex;

    // Branch: N = -1, Leaf: N >= 0
    int m_Size;
};

struct QuadNodeData {
    // Index of QuadNode
    int m_Index;

    // Depth of QuadNode
    int m_Depth;

    // Rect of QuadNode
    Rect m_Rect;
};

class QuadTree {
protected:
    // QuadNodes in tree, first node is always this tree's root.
    std::vector<QuadNode> m_Nodes;

    // QuadElements in tree.
    FreeList<QuadElement> m_Elements;

    // QuadElementNodes in tree, this is the actual value "stored" by the nodes.
    FreeList<QuadElementNode> m_ElementNodes;

    // QuadTree's rect.
    Rect m_Rect;

    // QuadTree's max depth.
    int m_MaxDepth;

public:
    QuadTree(const Rect& Rect, int MaxDepth) : m_Rect(Rect), m_MaxDepth(MaxDepth) {
        m_Nodes[0] = QuadNode();
    }

    void Insert(QuadElement nElement) {

        QuadNode& Root = m_Nodes[0];

        //<<< Add QuadElement to tree:
        const int nElementIndex = m_Elements.Insert(nElement); // Insert new QuadElement into m_Elements FreeList.
        //>>>

        //<<< Create QuadElementNode to reference new QuadElement:
        const QuadElementNode nElementNode{nElementIndex, -1};
        //>>>

        const bool InsertSuccess = TryInsertElementInLeaf(nElementNode, Root);

        if (!InsertSuccess) {
            if (!IsLeaf(Root)) {
                const auto& BranchChildren = GetBranchChildren(Root);
            }
        }


        // just writing some ideas, has nothing to do with this function.
        QuadNode& Node = m_Nodes[0];

        if (Node.m_Size > QUAD_CONFIG::MAX_CHILDREN) {
            // Elements that need to be reprocessed after the node subdivision.
            std::vector<int> ElementIndices;

            { // scope
                int t_Index = Node.m_FirstIndex;

                for (int i = 0; i < Node.m_Size; i++) {
                    ElementIndices.push_back(m_ElementNodes[t_Index].m_Index);

                    t_Index = m_ElementNodes[t_Index].m_NextIndex;
                }
            }

            // Set node's size to -1 to signify that it is now a branch.
            Node.m_Size = -1;

            // Create subdivided nodes and then push them onto m_Nodes.
            { // scope
                const int t_FirstIndex = (int) m_Nodes.size();

                for (int i = 0; i < 4; i++) {
                    m_Nodes.push_back({0, 0});
                }

                Node.m_FirstIndex = t_FirstIndex;
            }

            std::array<QuadNode*, 4> Nodes{};

            for (int i = 0; i < 4; i++) {
                Nodes[i] = &m_Nodes[i + Node.m_FirstIndex];
            }

            const std::array<Rect, 4> Rects = QuadSubdivideRect(m_Rect);

            for (const auto& Index : ElementIndices) {
                for (int i = 0; i < 4; i++) {
                    const QuadElement& Element = m_Elements[Index];

                    if (Element.m_Rect.Overlaps(Rects[i])) {

                    }
                }
            }
        }


    }

    std::list<QuadNodeData> GetLeaves(const QuadNodeData& Root, const Rect& nRect) {
        std::list<QuadNodeData> Leaves, ToProcess;

        ToProcess.push_back(Root);

        while (!ToProcess.empty())
        {
            const QuadNodeData NodeData = ToProcess.back();
            ToProcess.pop_back();

            // If this node is a leaf, insert it to the list.
            if (m_Nodes[NodeData.m_Index].m_Size != -1) {
                Leaves.push_back(NodeData);
            }

            // Otherwise push the children that intersect the rectangle
            else
            {
                const std::array<Rect, 4> ChildRects = QuadSubdivideRect(NodeData.m_Rect);

                for (int i = 0; i < 4; i++) {
                    if (nRect.Overlaps(ChildRects[i])) {
                        QuadNodeData ChildNodeData;

                        ChildNodeData.m_Index = m_Nodes[NodeData.m_Index].m_FirstIndex + i;
                        ChildNodeData.m_Depth = NodeData.m_Depth + 1;
                        ChildNodeData.m_Rect = ChildRects[i];

                        ToProcess.push_back(ChildNodeData);
                    }
                }
            }
        }

        return Leaves;
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
                    for (int i = 0; i < 4; i++) {

                    }

                    Node.m_Size = 0;
                    Node.m_FirstIndex = -1;
                }
            }
        }
    }

    // Tries to insert QuadElementNode into a QuadNode, returns true if successful. Returning false means the QuadNode is or needs* to be a branch (* it must subdivide to fit new children)
    bool TryInsertElementInLeaf(QuadElementNode Element, QuadNode& Leaf) {
        if (CanInsertToLeaf(Leaf)) {
            Element.m_NextIndex = -1; // Make sure the new Element is the last in the list.

            const int ElementNodeIndex = m_ElementNodes.Insert(Element); // Push new Element into the m_Elements array.

            const int LastElementNodeIndex = LastElementNodeIndexInLeaf(Leaf);

            if (LastElementNodeIndex == -1) return false; // Make sure that the last Element is valid.

            QuadElementNode& LastElementNode = m_ElementNodes[LastElementNodeIndex]; // Get the leaf's last Element.
            LastElementNode.m_NextIndex = ElementNodeIndex; // Point the leaf's last Element to the newly inserted one.

            Leaf.m_Size++; // Increase leaf size after Element has been inserted.

            return true; // Insertion succeeded, return true.
        }
        return false; // Unable to insert, return false.
    }

    // Returns true if QuadNode is a leaf, and if adding a child will NOT cause it to subdivide.
    static bool CanInsertToLeaf(QuadNode& Leaf) {
        return (IsLeaf(Leaf) && !IsLeafFull(Leaf));
    }

    static bool IsLeaf(QuadNode& Leaf) {
        return (Leaf.m_Size != -1);
    }

    static bool IsLeafFull(QuadNode& Leaf) {
        return (Leaf.m_Size == QUAD_CONFIG::MAX_CHILDREN);
    }

    // Returns the index of the last QuadElementNode in the leaf, or -1 otherwise.
    int LastElementNodeIndexInLeaf(QuadNode& Leaf) {
        if (IsLeaf(Leaf)) {
            {
                int Index = Leaf.m_FirstIndex;
                for (int i = 0; i < Leaf.m_Size; i++) {
                    if (m_ElementNodes[Index].m_NextIndex == -1) break;

                    Index = m_ElementNodes[Index].m_NextIndex;
                }
                return Index;
            }
        }
        return -1;
    }

    std::array<QuadNode*, 4> GetBranchChildren(QuadNode& Branch) {
        std::array<QuadNode*, 4> BranchChildren{};
        for (int i = 0; i < 4; i++) {
            BranchChildren[i] = &m_Nodes[i + Branch.m_FirstIndex];
        }
        return BranchChildren;
    }

//    void NodeInsert(QuadElementNode Element, QuadNode& Node) {
//
//        const int ElementNodeIndex = (int)m_ElementNodes.size();
//        m_ElementNodes.push_back(Element);
//
//        if (Node.m_Size == 0) {
//            Node.m_FirstIndex = ElementNodeIndex;
//        }
//
//        else {
//
//            { // scope
//                int Index = Node.m_FirstIndex;
//                for (int i = 0; i < Node.m_Size; i++) {
//                    if (m_ElementNodes[Index].m_NextIndex == -1) break;
//
//                    Index = m_ElementNodes[Index].m_NextIndex;
//                }
//
//                QuadElementNode& LastElementNode = m_ElementNodes[Index];
//
//                LastElementNode.m_NextIndex = ElementNodeIndex;
//            }
//
//        }
//    }

};