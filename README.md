# Ball Simulation

A simple ball simulation program implemented in C++ using SFML.

## Description

Each ball is subject to gravity, friction, and collisions with walls and other balls. Additionally, a disturbance feature allows the user to repel the balls by holding the mouse button. Supposed to be a fluid sim.

## Features

- Simulate the behavior of multiple balls in a window.
- Gravity and friction affect the motion of the balls.
- Balls collide with each other and with the walls of the window.
- Disturbance feature allows the user to repel balls by holding the mouse button.

## Dependencies

- [SFML](https://www.sfml-dev.org/) - Simple and Fast Multimedia Library (goated)


## Code Explanation

### Ball Structure

```cpp
struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
};
