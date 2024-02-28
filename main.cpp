#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 900;
const int NUM_BALLS = 1000;
const float BALL_RADIUS = 10.0f;
const float GRAVITY = 0.3f;
const float MAX_SPEED = 5.0f;
const float REPEL_DISTANCE = 50.0f;
const float FRICTION = 0.99f;
const float DISTURBANCE_FORCE = 200.0f;

struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
};

std::vector<Ball> balls;

void handleCollision(Ball& ball1, Ball& ball2) {
    sf::Vector2f normal = ball1.position - ball2.position;
    float distance = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    normal /= distance;

    sf::Vector2f relativeVelocity = ball1.velocity - ball2.velocity;
    float dotProduct = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

    if (dotProduct < 0) {
        sf::Vector2f impulse = normal * dotProduct;
        ball1.velocity -= impulse;
        ball2.velocity += impulse;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ball Simulation - Mouse Repel Distance: R (Hold to Activate Disturbance)", sf::Style::Default);
    window.setFramerateLimit(60);

    sf::CircleShape disturbanceBall(30.f);
    disturbanceBall.setFillColor(sf::Color::Red);
    disturbanceBall.setOrigin(30.f, 30.f);
    bool disturbanceActive = false;

    balls.resize(NUM_BALLS);
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
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    disturbanceActive = true;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    disturbanceActive = false;
            }
        }

        if (disturbanceActive) {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            for (auto& ball : balls) {
                sf::Vector2f direction = ball.position - static_cast<sf::Vector2f>(mousePosition);
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (distance < REPEL_DISTANCE) {
                    direction /= distance;
                    ball.velocity += direction * DISTURBANCE_FORCE / distance;
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

            // Calculate speed and adjust color
            float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
            // Interpolate between blue and red based on the speed
            int blueComponent = static_cast<int>(std::max(0.0f, 255.0f - speed * 20.0f));
            int redComponent = static_cast<int>(std::min(255.0f, speed * 20.0f));
            ball.color = sf::Color(redComponent, 0, blueComponent);
        }

        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                float dx = balls[i].position.x - balls[j].position.x;
                float dy = balls[i].position.y - balls[j].position.y;
                float distanceSquared = dx * dx + dy * dy;
                float minDistance = BALL_RADIUS * 2;
                if (distanceSquared < minDistance * minDistance) {
                    handleCollision(balls[i], balls[j]);
                }
            }
        }

        window.clear(sf::Color::White);

        if (disturbanceActive) {
            disturbanceBall.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
            window.draw(disturbanceBall);
        }

        sf::CircleShape ballShape(BALL_RADIUS);
        ballShape.setOrigin(BALL_RADIUS, BALL_RADIUS);
        for (const auto& ball : balls) {
            ballShape.setPosition(ball.position);
            ballShape.setFillColor(ball.color);
            window.draw(ballShape);
        }

        window.display();
    }

    return 0;
}