#ifndef BALL_SIMULATION_HPP
#define BALL_SIMULATION_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <unordered_map>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 900;
const int NUM_BALLS = 5000;
const float BALL_RADIUS = 4.0f;
const float GRAVITY = 0.2f;
const float REPEL_DISTANCE = 50.0f;
const float FRICTION = 0.98f;
const float DISTURBANCE_FORCE = 150.0f;
const int GRID_SIZE = 50;

struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
};

// Custom hash function for std::pair<int, int>
struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

std::pair<int, int> getGridPosition(const sf::Vector2f& position);
void handleCollision(Ball& ball1, Ball& ball2);

#endif // BALL_SIMULATION_HPP
