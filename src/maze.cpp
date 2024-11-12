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

    // Load wall texture from the assets folder
    sf::Texture wallTexture;
    if (!wallTexture.loadFromFile("assets/brick_texture.jpg")) {  // Load wall texture
        std::cerr << "Could not load wall texture" << std::endl;
        return -1;
    }

    // Load path texture from the assets folder
    sf::Texture pathTexture;
    if (!pathTexture.loadFromFile("assets/v2.jpg")) {  // Load path texture
        std::cerr << "Could not load path texture" << std::endl;
        return -1;
    }

    // Maze layout (1 = wall, 0 = path, 3 = endpoint)
    int maze[ROWS][COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1},
        {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 3, 1, 0, 1, 1},
        {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Create walls and paths based on the maze array
    std::vector<sf::RectangleShape> walls;
    std::vector<sf::RectangleShape> paths;

    sf::RectangleShape endpoint(sf::Vector2f(CELL_SIZE - 4, CELL_SIZE - 4));  // Smaller size for endpoint
    endpoint.setFillColor(sf::Color::Red);
    bool endpointExists = false;

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (maze[row][col] == 1) {
                // Create a wall and apply the wall texture
                sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                wall.setPosition(col * CELL_SIZE, row * CELL_SIZE);
                wall.setTexture(&wallTexture);
                walls.push_back(wall);
            } else if (maze[row][col] == 0) {
                // Create a path and apply the path texture
                sf::RectangleShape path(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                path.setPosition(col * CELL_SIZE, row * CELL_SIZE);
                path.setTexture(&pathTexture);
                paths.push_back(path);
            } else if (maze[row][col] == 3) {
                // Position endpoint
                endpoint.setPosition(col * CELL_SIZE + 2, row * CELL_SIZE + 2);
                endpointExists = true;
            }
        }
    }

    // Define checkpoint
    sf::CircleShape checkpoint(8);  // Reduced radius to fit smaller cells
    checkpoint.setFillColor(sf::Color::Yellow);
    checkpoint.setPosition(
        6 * CELL_SIZE + (CELL_SIZE / 2 - checkpoint.getRadius()),
        6 * CELL_SIZE + (CELL_SIZE / 2 - checkpoint.getRadius())
    );

    // Player setup
    sf::RectangleShape player(sf::Vector2f(15, 15));  // Reduced size to fit smaller cells
    player.setFillColor(sf::Color::Green);
    player.setPosition(CELL_SIZE + 7.5f, CELL_SIZE + 7.5f);  // Starting position at an open path

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
            checkpoint.setFillColor(sf::Color::Magenta);  // Change color to indicate it's reached
            std::cout << "Checkpoint reached at: (" << checkpoint.getPosition().x << ", " << checkpoint.getPosition().y << ")" << std::endl;
        }

        // Check if the player reached the endpoint
        if (endpointExists && player.getGlobalBounds().intersects(endpoint.getGlobalBounds())) {
            endpoint.setFillColor(sf::Color::Green);  // Change color to indicate the endpoint is reached
            std::cout << "Endpoint reached at: (" << endpoint.getPosition().x << ", " << endpoint.getPosition().y << ")" << std::endl;
            window.close();  // Close the game once the endpoint is reached
        }

        // Render everything
        window.clear();
        for (const auto& path : paths) window.draw(path);  // Draw paths with texture
        for (const auto& w : walls) window.draw(w);        // Draw walls with texture
        window.draw(checkpoint);
        if (endpointExists) window.draw(endpoint);
        window.draw(player);
        window.display();
    }

    return 0;
}
