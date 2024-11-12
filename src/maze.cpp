#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>  // For std::min

int main() {
    // Set up the window to match the size of the maze
    const int ROWS = 10;
    const int COLS = 15;
    const int CELL_SIZE = 30;

    sf::RenderWindow window(sf::VideoMode(COLS * CELL_SIZE, ROWS * CELL_SIZE), "Maze Game with Checkpoints");

    // Maze layout (1 = wall, 0 = path)
    int maze[ROWS][COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1},
        {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Create walls based on the maze array
    std::vector<sf::RectangleShape> walls;
    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));  // Reduced size for thinner appearance
    wall.setFillColor(sf::Color::Blue);

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (maze[row][col] == 1) {
                wall.setPosition(col * CELL_SIZE, row * CELL_SIZE);
                walls.push_back(wall);
            }
        }
    }

    // Define checkpoint
    sf::CircleShape checkpoint(10);  // Radius of 10 pixels
    checkpoint.setFillColor(sf::Color::Yellow);
    checkpoint.setPosition(
        6 * CELL_SIZE + (CELL_SIZE / 2 - checkpoint.getRadius()),
        6 * CELL_SIZE + (CELL_SIZE / 2 - checkpoint.getRadius())
    );

    // Player setup
    sf::RectangleShape player(sf::Vector2f(15, 15));  // Reduced size to fit smaller cells
    player.setFillColor(sf::Color::Green);
    player.setPosition(CELL_SIZE + 7.5f, CELL_SIZE + 7.5f);  // Updated starting position for centering player within the cell

    // Movement speed
    float speed = 100.0f;  // Pixels per second

    // Clock to track time between frames
    sf::Clock clock;

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Get elapsed time since the last frame
        float deltaTime = clock.restart().asSeconds();
        deltaTime = std::min(deltaTime, 0.1f);  // Cap deltaTime

        // Player movement using arrow keys
        sf::Vector2f movement(0.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement.y -= speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement.y += speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement.x -= speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += speed * deltaTime;

        // Attempt to move player in X direction
        sf::Vector2f newPosition = player.getPosition() + sf::Vector2f(movement.x, 0.0f);
        sf::FloatRect playerBounds = player.getGlobalBounds();
        playerBounds.left = newPosition.x;
        bool collisionX = false;

        for (const auto& w : walls) {
            if (w.getGlobalBounds().intersects(playerBounds)) {
                collisionX = true;
                break;
            }
        }

        if (!collisionX) {
            player.move(movement.x, 0.0f);
        }

        // Attempt to move player in Y direction
        newPosition = player.getPosition() + sf::Vector2f(0.0f, movement.y);
        playerBounds = player.getGlobalBounds();
        playerBounds.top = newPosition.y;
        bool collisionY = false;

        for (const auto& w : walls) {
            if (w.getGlobalBounds().intersects(playerBounds)) {
                collisionY = true;
                break;
            }
        }

        if (!collisionY) {
            player.move(0.0f, movement.y);
        }

        // Check if the player reached the checkpoint
        if (player.getGlobalBounds().intersects(checkpoint.getGlobalBounds())) {
            checkpoint.setFillColor(sf::Color::Red);  // Change color to indicate it's reached
            std::cout << "Checkpoint reached at: (" << checkpoint.getPosition().x << ", " << checkpoint.getPosition().y << ")" << std::endl;
        }

        // Render everything
        window.clear();
        for (const auto& w : walls) window.draw(w);
        window.draw(checkpoint);
        window.draw(player);
        window.display();
    }

    return 0;
}
