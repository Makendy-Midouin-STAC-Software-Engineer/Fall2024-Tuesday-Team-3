#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>  // For debugging output

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 400), "Maze Game Demo");

    // Create walls using vector
    std::vector<sf::RectangleShape> walls;
    sf::RectangleShape wall(sf::Vector2f(40, 40));
    wall.setFillColor(sf::Color::Blue);

    // Define a few walls to form a simple maze
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

        // Debug: Print player position and movement
        std::cout << "Player Pos: (" << player.getPosition().x << ", " 
                  << player.getPosition().y << "), Movement: (" 
                  << movement.x << ", " << movement.y << ")" << std::endl;

        // Prevent player from moving outside the window
        if (nextPos.left < 0) movement.x = 0;
        if (nextPos.top < 0) movement.y = 0;
        if (nextPos.left + nextPos.width > window.getSize().x) movement.x = 0;
        if (nextPos.top + nextPos.height > window.getSize().y) movement.y = 0;

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

        // Render everything
        window.clear();
        for (const auto& w : walls) window.draw(w);
        window.draw(player);
        window.display();
    }

    return 0;
}
