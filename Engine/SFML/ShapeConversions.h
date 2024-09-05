#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

sf::VertexArray CircleToVertices(const sf::CircleShape& circle) {
    std::size_t count = circle.getPointCount();
    sf::VertexArray vertices(sf::PrimitiveType::Triangles, count * 3);

    // Center point
//    vertices[0].position = circle.getPosition();
//    vertices[0].color = circle.getFillColor();

    const sf::Vector2 circle_pos = circle.getPosition();
    const float circle_radius = circle.getRadius();
    const sf::Color circle_color = circle.getFillColor();

    const sf::Vertex center {{circle_pos.x + circle_radius,circle_pos.y + circle_radius}, circle_color};

    // Points around the circle
    for (std::size_t i = 0; i < count; i++) {
        const sf::Vector2 point = circle.getTransform().transformPoint(circle.getPoint(i));
        const sf::Vector2 next_point = circle.getTransform().transformPoint(circle.getPoint(i < count ? i+1 : 0));

        vertices[(i*3)] = center;
        vertices[(i*3)+1] = { point, circle_color };
        vertices[(i*3)+2] = { next_point, circle_color };
    }

    return vertices;
}

sf::VertexArray CircleOutlineToVertices(const sf::CircleShape& circle) {
    std::size_t pointCount = circle.getPointCount();
    float outlineThickness = circle.getOutlineThickness();
    sf::VertexArray outlineArray(sf::PrimitiveType::TriangleStrip, (pointCount + 1) * 2);

    for (std::size_t i = 0; i < pointCount; ++i) {
        sf::Vector2f innerPoint = circle.getTransform().transformPoint(circle.getPoint(i));
        sf::Vector2f outerPoint = circle.getTransform().transformPoint(circle.getPoint(i) + circle.getPoint(i) * (outlineThickness / circle.getRadius()));

        outlineArray[i * 2].position = innerPoint;
        outlineArray[i * 2].color = circle.getOutlineColor();

        outlineArray[i * 2 + 1].position = outerPoint;
        outlineArray[i * 2 + 1].color = circle.getOutlineColor();
    }

    // Closing the strip
    outlineArray[pointCount * 2].position = outlineArray[0].position;
    outlineArray[pointCount * 2].color = circle.getOutlineColor();

    outlineArray[pointCount * 2 + 1].position = outlineArray[1].position;
    outlineArray[pointCount * 2 + 1].color = circle.getOutlineColor();

    return outlineArray;
}
