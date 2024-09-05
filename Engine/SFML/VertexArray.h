#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

sf::VertexArray CombineVertexArrays(const std::vector<sf::VertexArray>& arrays) {
    // Determine the total number of vertices
    std::size_t totalVertexCount = 0;
    for (const auto& array : arrays) {
        totalVertexCount += array.getVertexCount();
    }

    // Create a combined VertexArray
    sf::VertexArray combinedArray(sf::Triangles);
    combinedArray.resize(totalVertexCount);

    // Copy vertices from each array into the combined array
    std::size_t currentIndex = 0;
    for (const auto& array : arrays) {
        for (std::size_t i = 0; i < array.getVertexCount(); ++i) {
            combinedArray[currentIndex++] = array[i];
        }
    }

    return combinedArray;
}

void CombineIntoVertexArray(const sf::VertexArray& from, sf::VertexArray& into) {
    // Determine the total number of vertices
    std::size_t total_vertices = from.getVertexCount() + into.getVertexCount();

    // Create a combined VertexArray
    into.resize(total_vertices);

    // Copy vertices from array into the combined array
    for (std::size_t i = 0; i < from.getVertexCount(); i++) {
        into.append(from[i]);
    }
}
