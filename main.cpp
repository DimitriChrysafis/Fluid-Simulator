#include "BallSimulation.hpp"

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <unordered_map>

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ball Simulation", sf::Style::Default);
    window.setFramerateLimit(60);

    sf::CircleShape disturbanceBall(30.f);
    disturbanceBall.setFillColor(sf::Color::Red);
    disturbanceBall.setOrigin(30.f, 30.f);
    bool disturbanceActive = false;

    std::vector<Ball> balls(NUM_BALLS);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> x_dist(0, WINDOW_WIDTH);
    std::uniform_real_distribution<float> y_dist(0, WINDOW_HEIGHT);

    for (auto& ball : balls) {
        ball.position.x = x_dist(gen);
        ball.position.y = y_dist(gen);
        ball.velocity = sf::Vector2f(0.f, 0.f);
        ball.color = sf::Color::Blue; // Initialize with blue color
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                disturbanceActive = true;
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                disturbanceActive = false;
        }

        if (disturbanceActive) {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            for (auto& ball : balls) {
                sf::Vector2f direction = ball.position - static_cast<sf::Vector2f>(mousePosition);
                float distanceSquared = direction.x * direction.x + direction.y * direction.y;
                if (distanceSquared < REPEL_DISTANCE * REPEL_DISTANCE && distanceSquared > 0) {
                    float distance = std::sqrt(distanceSquared);
                    direction /= distance;
                    ball.velocity += direction * DISTURBANCE_FORCE / distance;
                }
            }
        }

        std::unordered_map<std::pair<int, int>, std::vector<Ball*>, PairHash> grid;

        for (auto& ball : balls) {
            auto gridPos = getGridPosition(ball.position);
            grid[gridPos].push_back(&ball);
        }

        for (auto& cell : grid) {
            auto& cellBalls = cell.second;
            for (size_t i = 0; i < cellBalls.size(); ++i) {
                for (size_t j = i + 1; j < cellBalls.size(); ++j) {
                    handleCollision(*cellBalls[i], *cellBalls[j]);
                }
            }

            auto [x, y] = cell.first;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    auto neighbor = std::make_pair(x + dx, y + dy);
                    if (grid.find(neighbor) != grid.end()) {
                        auto& neighborBalls = grid[neighbor];
                        for (auto& ball1 : cellBalls) {
                            for (auto& ball2 : neighborBalls) {
                                handleCollision(*ball1, *ball2);
                            }
                        }
                    }
                }
            }
        }

        for (auto& ball : balls) {
            ball.velocity.y += GRAVITY;
            ball.velocity *= FRICTION;
            ball.position += ball.velocity;

            if (ball.position.x < BALL_RADIUS) {
                ball.position.x = BALL_RADIUS;
                ball.velocity.x = -ball.velocity.x;
            }
            if (ball.position.x > WINDOW_WIDTH - BALL_RADIUS) {
                ball.position.x = WINDOW_WIDTH - BALL_RADIUS;
                ball.velocity.x = -ball.velocity.x;
            }
            if (ball.position.y < BALL_RADIUS) {
                ball.position.y = BALL_RADIUS;
                ball.velocity.y = -ball.velocity.y;
            }
            if (ball.position.y > WINDOW_HEIGHT - BALL_RADIUS) {
                ball.position.y = WINDOW_HEIGHT - BALL_RADIUS;
                ball.velocity.y = -ball.velocity.y;
            }
        }

        window.clear(sf::Color::White);

        if (disturbanceActive) {
            disturbanceBall.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
            window.draw(disturbanceBall);
        }

        sf::CircleShape ballShape(BALL_RADIUS);
        ballShape.setOrigin(BALL_RADIUS, BALL_RADIUS);
        ballShape.setFillColor(sf::Color::Blue);
        for (const auto& ball : balls) {
            ballShape.setPosition(ball.position);
            window.draw(ballShape);
        }

        window.display();
    }

    return 0;
}
