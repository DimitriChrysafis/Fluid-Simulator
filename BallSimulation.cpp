#include "BallSimulation.hpp"

std::pair<int, int> getGridPosition(const sf::Vector2f& position) {
    return { static_cast<int>(position.x) / GRID_SIZE, static_cast<int>(position.y) / GRID_SIZE };
}

void handleCollision(Ball& ball1, Ball& ball2) {
    sf::Vector2f normal = ball1.position - ball2.position;
    float distanceSquared = normal.x * normal.x + normal.y * normal.y;
    if (distanceSquared < (BALL_RADIUS * 2) * (BALL_RADIUS * 2) && distanceSquared > 0) {
        float distance = std::sqrt(distanceSquared);
        normal /= distance;
        sf::Vector2f relativeVelocity = ball1.velocity - ball2.velocity;
        float dotProduct = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
        if (dotProduct < 0) {
            sf::Vector2f impulse = normal * dotProduct;
            ball1.velocity -= impulse;
            ball2.velocity += impulse;
        }
    }
}
