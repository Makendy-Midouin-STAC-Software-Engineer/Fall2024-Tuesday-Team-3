#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>  // For debugging output

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 400), "Maze Game with Checkpoints");

    // Create walls using a vector
    std::vector<sf::RectangleShape> walls;
    sf::RectangleShape wall(sf::Vector2f(40, 40));
    wall.setFillColor(sf::Color::Blue);

    // Define the walls for the maze
    for (int i = 0; i < 10; ++i) {
        wall.setPosition(i * 40, 0);       // Top wall
        walls.push_back(wall);
        wall.setPosition(i * 40, 360);     // Bottom wall
        walls.push_back(wall);
    }

    for (int i = 1; i < 9; ++i) {
        wall.setPosition(0, i * 40);       // Left wall
        walls.push_back(wall);
        wall.setPosition(360, i * 40);     // Right wall
        walls.push_back(wall);
    }

    // Add some inner walls for the maze structure
    for (int i = 1; i < 4; ++i) {
        wall.setPosition(i * 40, 120);     // Horizontal inner wall
        walls.push_back(wall);
    }

    for (int i = 6; i < 9; ++i) {
        wall.setPosition(i * 40, 200);     // Another inner wall
        walls.push_back(wall);
    }

    // Checkpoint setup
    std::vector<sf::CircleShape> checkpoints;
    sf::CircleShape checkpoint(10);  // Radius of 10 pixels
    checkpoint.setFillColor(sf::Color::Yellow);

    // Place checkpoints in the maze
    checkpoint.setPosition(160, 160);  // Example checkpoint position
    checkpoints.push_back(checkpoint);
    checkpoint.setPosition(280, 280);
    checkpoints.push_back(checkpoint);

    // Player setup
    sf::RectangleShape player(sf::Vector2f(30, 30));
    player.setFillColor(sf::Color::Green);
    player.setPosition(40, 320);  // Starting position

    // Movement speed
    float speed = 100.0f;  // Pixels per second

    // Clock to track time between frames
    sf::Clock clock;

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Get elapsed time since the last frame
        float deltaTime = std::min(0.1f, clock.restart().asSeconds());  // Cap deltaTime

        // Player movement using arrow keys
        sf::Vector2f movement(0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement.y -= speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement.y += speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement.x -= speed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += speed * deltaTime;

        // Calculate the next position
        sf::FloatRect nextPos = player.getGlobalBounds();
        nextPos.left += movement.x;
        nextPos.top += movement.y;

        // Check collisions with walls
        bool canMoveX = true, canMoveY = true;
        for (const auto& w : walls) {
            if (w.getGlobalBounds().intersects({nextPos.left, player.getPosition().y, nextPos.width, nextPos.height})) {
                canMoveX = false;
            }
            if (w.getGlobalBounds().intersects({player.getPosition().x, nextPos.top, nextPos.width, nextPos.height})) {
                canMoveY = false;
            }
        }

        // Move the player only if there are no collisions in the respective direction
        if (canMoveX) player.move(movement.x, 0);
        if (canMoveY) player.move(0, movement.y);

        // Check if the player reached a checkpoint
        for (auto& cp : checkpoints) {
            if (player.getGlobalBounds().intersects(cp.getGlobalBounds())) {
                cp.setFillColor(sf::Color::Red);  // Change color to indicate it's reached
                std::cout << "Checkpoint reached at: (" << cp.getPosition().x << ", " << cp.getPosition().y << ")" << std::endl;
            }
        }

        // Render everything
        window.clear();
        for (const auto& w : walls) window.draw(w);
        for (const auto& cp : checkpoints) window.draw(cp);
        window.draw(player);
        window.display();
    }

    return 0;
}
