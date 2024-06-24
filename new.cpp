#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <thread>
#include <mutex>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 900;
const int NUM_BALLS = 5000;
const float BALL_RADIUS = 5.0f;
const float GRAVITY = 0.2f;
const float FRICTION = 0.98f;
const float REPEL_DISTANCE = 50.0f;
const float REPEL_DISTANCE_SQUARED = REPEL_DISTANCE * REPEL_DISTANCE;
const float DISTURBANCE_FORCE = 150.0f;
const int GRID_SIZE = 50;
const int GRID_COLUMNS = WINDOW_WIDTH / GRID_SIZE + 1;
const int GRID_ROWS = WINDOW_HEIGHT / GRID_SIZE + 1;
const int NUM_THREADS = std::thread::hardware_concurrency();

struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity;
};

std::pair<int, int> getGridPosition(const sf::Vector2f& position) {
    return { static_cast<int>(position.x) / GRID_SIZE, static_cast<int>(position.y) / GRID_SIZE };
}

void handleCollision(Ball& ball1, Ball& ball2) {
    sf::Vector2f normal = ball1.position - ball2.position;
    float distanceSquared = normal.x * normal.x + normal.y * normal.y;
    if (distanceSquared < 4 * BALL_RADIUS * BALL_RADIUS && distanceSquared > 0) {
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

void updateBallPhysics(std::vector<Ball>& balls, int start, int end) {
    for (int i = start; i < end; ++i) {
        balls[i].velocity.y += GRAVITY;
        balls[i].velocity *= FRICTION;
        balls[i].position += balls[i].velocity;

        if (balls[i].position.x < BALL_RADIUS) {
            balls[i].position.x = BALL_RADIUS;
            balls[i].velocity.x = -balls[i].velocity.x;
        }
        if (balls[i].position.x > WINDOW_WIDTH - BALL_RADIUS) {
            balls[i].position.x = WINDOW_WIDTH - BALL_RADIUS;
            balls[i].velocity.x = -balls[i].velocity.x;
        }
        if (balls[i].position.y < BALL_RADIUS) {
            balls[i].position.y = BALL_RADIUS;
            balls[i].velocity.y = -balls[i].velocity.y;
        }
        if (balls[i].position.y > WINDOW_HEIGHT - BALL_RADIUS) {
            balls[i].position.y = WINDOW_HEIGHT - BALL_RADIUS;
            balls[i].velocity.y = -balls[i].velocity.y;
        }
    }
}

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
    }

    // Prepare SFML objects for batch rendering
    std::vector<sf::CircleShape> ballShapes(NUM_BALLS);
    for (int i = 0; i < NUM_BALLS; ++i) {
        ballShapes[i].setRadius(BALL_RADIUS);
        ballShapes[i].setOrigin(BALL_RADIUS, BALL_RADIUS);
        ballShapes[i].setPosition(balls[i].position);
        ballShapes[i].setFillColor(sf::Color::Blue);
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
                if (distanceSquared < REPEL_DISTANCE_SQUARED && distanceSquared > 0) {
                    float distance = std::sqrt(distanceSquared);
                    direction /= distance;
                    ball.velocity += direction * DISTURBANCE_FORCE / distance;
                }
            }
        }

        std::vector<std::vector<int>> grid(GRID_COLUMNS * GRID_ROWS);

        for (int i = 0; i < NUM_BALLS; ++i) {
            auto gridPos = getGridPosition(balls[i].position);
            grid[gridPos.first + gridPos.second * GRID_COLUMNS].push_back(i);
        }

        std::vector<std::thread> threads;

        auto collisionTask = [&](int start, int end) {
            for (int x = start; x < end; ++x) {
                for (int y = 0; y < GRID_ROWS; ++y) {
                    int cellIndex = x + y * GRID_COLUMNS;
                    auto& cellBalls = grid[cellIndex];

                    for (size_t i = 0; i < cellBalls.size(); ++i) {
                        for (size_t j = i + 1; j < cellBalls.size(); ++j) {
                            handleCollision(balls[cellBalls[i]], balls[cellBalls[j]]);
                        }
                    }

                    for (int dx = -1; dx <= 1; ++dx) {
                        for (int dy = -1; dy <= 1; ++dy) {
                            if (dx == 0 && dy == 0) continue;
                            int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && nx < GRID_COLUMNS && ny >= 0 && ny < GRID_ROWS) {
                                int neighborIndex = nx + ny * GRID_COLUMNS;
                                auto& neighborBalls = grid[neighborIndex];
                                for (auto& ballIndex1 : cellBalls) {
                                    for (auto& ballIndex2 : neighborBalls) {
                                        handleCollision(balls[ballIndex1], balls[ballIndex2]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };

        int rowsPerThread = GRID_COLUMNS / NUM_THREADS;
        for (int i = 0; i < NUM_THREADS; ++i) {
            int start = i * rowsPerThread;
            int end = (i == NUM_THREADS - 1) ? GRID_COLUMNS : start + rowsPerThread;
            threads.emplace_back(collisionTask, start, end);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::vector<std::thread> physicsThreads;
        int ballsPerThread = NUM_BALLS / NUM_THREADS;
        for (int i = 0; i < NUM_THREADS; ++i) {
            int start = i * ballsPerThread;
            int end = (i == NUM_THREADS - 1) ? NUM_BALLS : start + ballsPerThread;
            physicsThreads.emplace_back(updateBallPhysics, std::ref(balls), start, end);
        }

        for (auto& thread : physicsThreads) {
            thread.join();
        }

        window.clear(sf::Color::White);

        if (disturbanceActive) {
            disturbanceBall.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
            window.draw(disturbanceBall);
        }

        // Draw all balls in batch
        for (int i = 0; i < NUM_BALLS; ++i) {
            ballShapes[i].setPosition(balls[i].position);
            window.draw(ballShapes[i]);
        }

        window.display();
    }

    return 0;
}
