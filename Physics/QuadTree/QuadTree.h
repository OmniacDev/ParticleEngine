#pragma once

#include <immintrin.h>
#include <avxintrin.h>

#include <vector>
#include <array>
#include <memory>
#include <list>
#include <string>

#include "../Particle.h"
#include "../../Engine/Math/Rect/Rect.h"
#include "../../Engine/Math/Viewport/Viewport.h"
#include "../../Engine/FreeList/FreeList.h"
#include "../Solver.h"

namespace Quad {
    const int MAX_CHILDREN = 4;
    const int MAX_DEPTH = 4;

    struct Node {
        int start_index{};
        int num{};
    };

    struct NodeData {
        int index{};
        int depth{};
        Rect rect;
    };

    struct NodeElement {
        int index{};
        int next{};
    };

    struct Element {
        int index{};
        Rect rect;
    };

    static std::array<Rect, 4> Subdivide(const Rect& rect){
        const FVector2 child_size = rect.Size / 2;

        return {
            Rect(rect.Position, child_size),
            Rect(FVector2(rect.Position.X + child_size.X, rect.Position.Y), child_size),
            Rect(FVector2 (rect.Position.X, rect.Position.Y + child_size.Y), child_size),
            Rect(rect.Position + child_size, child_size)
        };
    }

    class Tree {
    public:
        FreeList<Node> nodes;
        FreeList<NodeElement> node_elements;
        FreeList<Element> elements;

        NodeData root_data;

    public:
        explicit Tree(const Rect& tree_rect) {
            nodes.Insert({ -1, 0});
            root_data = { 0, 0, tree_rect };
        }

        void Insert(const Element& element) {
            const int element_index = elements.Insert(element);
            NodeInsert(element_index, root_data);
        }

        void Remove(const int element_index) {
            std::vector<NodeData> leaves = FindLeaves(elements[element_index].rect, root_data);

            for (const auto& leaf : leaves) {
                LeafRemove(element_index, leaf);
            }

            elements.Erase(element_index);
        }

        void Cleanup() {
            if (nodes[0].num == -1) {
                std::vector<int> queue;

                queue.push_back(0);

                while (!queue.empty()) {
                    const int index = queue.back();

                    Node* node = &nodes[index];

                    queue.pop_back();

                    int empty_children = 0;
                    for (int i = 0; i < 4; i++) {
                        const int child_index = node->start_index + i;
                        const Node* child_node = &nodes[child_index];

                        if (child_node->num == 0)
                            empty_children++;
                        else if (child_node->num == -1)
                            queue.push_back(child_index);
                    }

                    if (empty_children == 4) {
                        for (int i = 3; i >= 0; i--) {
                            nodes.Erase(node->start_index + i);
                        }

                        node->num = 0;
                        node->start_index = -1;
                    }
                }
            }
        }

        void NodeInsert(const int element_index, const NodeData& node_data) {
            std::vector<NodeData> leaves = FindLeaves(elements[element_index].rect, node_data);

            for (const auto& leaf : leaves) {
                LeafInsert(element_index, leaf);
            }
        }

        void LeafInsert(const int element_index, const NodeData& node_data) {
            Node* node = &nodes[node_data.index];

            node->start_index = node_elements.Insert({ element_index, node->start_index });
            node->num++;

            if (node->num > Quad::MAX_CHILDREN && node_data.depth < Quad::MAX_DEPTH) {
                std::vector<int> indices;

                while (node->start_index != -1) {
                    const int index = node->start_index;
                    const NodeElement& node_element = node_elements[index];
                    indices.push_back(node_element.index);
                    node->start_index = node_element.next;
                    node_elements.Erase(index);
                }

                const int start_index = nodes.Insert({-1, 0});
                nodes.Insert({-1, 0});
                nodes.Insert({-1, 0});
                nodes.Insert({-1, 0});

                node = &nodes[node_data.index];

                node->start_index = start_index;
                node->num = -1;

                for (const auto& index : indices) {
                    std::vector<NodeData> leaves = FindLeaves(elements[element_index].rect, node_data);

                    for (const auto& leaf : leaves) {
                        LeafInsert(element_index, leaf);
                    }
                }
            }
        }

        std::vector<NodeData> FindLeaves(const Rect& rect, const NodeData& node_data) {
            std::vector<NodeData> leaves, queue;

            queue.push_back(node_data);

            while (!queue.empty())
            {
                const NodeData queue_node_data = queue.back();

                // If this node is a leaf, insert it to the list.
                Node* node = &nodes[queue_node_data.index];

                queue.pop_back();

                if (node->num != -1) {
                    leaves.push_back(queue_node_data);
                }
                else
                {
                    const std::array<Rect, 4> child_rects = Subdivide(queue_node_data.rect);

                    for (int i = 0; i < 4; i++) {
                        if (child_rects[i].Overlaps(rect)) {
                            NodeData child_node_data = {node->start_index + i, queue_node_data.depth + 1, child_rects[i]};
                            queue.push_back(child_node_data);
                        }
                    }
                }
            }

            return leaves;
        }

        void LeafRemove(const int element_index, const NodeData& node_data) {
            int en_index = nodes[node_data.index].start_index;
            int prev_index = -1;

            while (en_index != -1 && node_elements[en_index].index != element_index) {
                prev_index = en_index;
                en_index = node_elements[en_index].next;
            }

            if (en_index != -1) {
                const int NextIndex = node_elements[en_index].next;

                if (prev_index == -1) {
                    nodes[node_data.index].start_index = NextIndex;
                }
                else {
                    node_elements[prev_index].next = NextIndex;
                }
                node_elements.Erase(en_index);

                nodes[node_data.index].num--;
            }
        }
    };

    void GetElementIndices(const Tree *tree, const Node *node, std::vector<int> &indices);
}

inline void Quad::GetElementIndices(const Quad::Tree* tree, const Quad::Node* node, std::vector<int>& indices) {
    if (node->num == -1) return;

    indices.reserve(indices.size() + node->num);

    int i = node->start_index;
    while (i != -1) {
        const Quad::NodeElement* node_element = &tree->node_elements[i];

        indices.push_back(node_element->index);
        i = node_element->next;
    }
}
