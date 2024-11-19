#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <SFML/Audio.hpp>

enum GameState { MENU, ENTER_NAME, PLAY, LEADERBOARD, EXIT };

struct LeaderboardEntry {
    std::string name;
    float time;
};

int main() {
    const int ROWS = 20;
    const int COLS = 30;
    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 768;

    const int CELL_SIZE = std::min(WINDOW_WIDTH / COLS, WINDOW_HEIGHT / ROWS);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Maze Game with Leaderboard");
    GameState gameState = MENU;

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Could not load font" << std::endl;
        return -1;
    }
     // Load and play background music
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("assets/mu.ogg")) {
        std::cerr << "Could not load background music" << std::endl;
        return -1;
    }
    backgroundMusic.setLoop(true); // Set looping
    backgroundMusic.play();       // Play the music

    sf::Texture menuBackgroundTexture;
    if (!menuBackgroundTexture.loadFromFile("assets/235.jpg")) {
        std::cerr << "Could not load menu background image" << std::endl;
        return -1;
    }
    sf::Sprite menuBackground(menuBackgroundTexture);
    menuBackground.setScale(
        static_cast<float>(WINDOW_WIDTH) / menuBackgroundTexture.getSize().x,
        static_cast<float>(WINDOW_HEIGHT) / menuBackgroundTexture.getSize().y
    );

    // Main menu title
    sf::Text title("Get Out", font, 160);
    title.setFillColor(sf::Color(255, 215, 0)); // Gold color
    title.setStyle(sf::Text::Bold);
    title.setPosition(WINDOW_WIDTH / 2.0f - title.getGlobalBounds().width / 2.0f, 50);

    // Glow effect for title
    sf::Text titleGlow = title;
    titleGlow.setFillColor(sf::Color(255, 215, 0, 100)); // Semi-transparent gold for glow
    titleGlow.setCharacterSize(170);                    // Slightly larger for glow effect
    titleGlow.setPosition(WINDOW_WIDTH / 2.0f - titleGlow.getGlobalBounds().width / 2.0f, 45);

    sf::Text startText("Start Game", font, 50);
    startText.setFillColor(sf::Color::Yellow);
    startText.setPosition(WINDOW_WIDTH / 2.0f - startText.getGlobalBounds().width / 2.0f, 300);

    sf::Text leaderboardText("Leaderboard", font, 50);
    leaderboardText.setFillColor(sf::Color::Yellow);
    leaderboardText.setPosition(WINDOW_WIDTH / 2.0f - leaderboardText.getGlobalBounds().width / 2.0f, 380);

    sf::Text exitText("Exit", font, 50);
    exitText.setFillColor(sf::Color::Yellow);
    exitText.setPosition(WINDOW_WIDTH / 2.0f - exitText.getGlobalBounds().width / 2.0f, 460);

    std::string username;
    sf::Text enterNamePrompt("Enter your name (4 letters):", font, 35);
    enterNamePrompt.setPosition(WINDOW_WIDTH / 2.0f - enterNamePrompt.getGlobalBounds().width / 2.0f, 200);

    sf::Text nameInput("_", font, 40);
    nameInput.setPosition(WINDOW_WIDTH / 2.0f - 100, 300);

    sf::Text startGamePrompt("Press Enter to Start", font, 30);
    startGamePrompt.setPosition(WINDOW_WIDTH / 2.0f - startGamePrompt.getGlobalBounds().width / 2.0f, 400);

    sf::Text timerText("", font, 40);
    timerText.setFillColor(sf::Color::Black);
    timerText.setPosition(10, 10);

    sf::Texture wallTexture, pathTexture;
    if (!wallTexture.loadFromFile("assets/brick_texture.jpg") ||
        !pathTexture.loadFromFile("assets/v2.jpg")) {
        std::cerr << "Could not load textures" << std::endl;
        return -1;
    }

        // Maze layout (1 = wall, 0 = path, 3 = endpoint)
    int maze[ROWS][COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
        {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
        {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1},
        {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 3, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    

    std::vector<sf::RectangleShape> walls, paths;
    sf::RectangleShape endpoint(sf::Vector2f(CELL_SIZE - 4, CELL_SIZE - 4));
    endpoint.setFillColor(sf::Color::Red);

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (maze[row][col] == 1) {
                sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                wall.setPosition(col * CELL_SIZE, row * CELL_SIZE);
                wall.setTexture(&wallTexture);
                walls.push_back(wall);
            } else if (maze[row][col] == 0) {
                sf::RectangleShape path(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                path.setPosition(col * CELL_SIZE, row * CELL_SIZE);
                path.setTexture(&pathTexture);
                paths.push_back(path);
            } else if (maze[row][col] == 3) {
                endpoint.setPosition(col * CELL_SIZE + 2, row * CELL_SIZE + 2);
            }
        }
    }

    sf::RectangleShape player(sf::Vector2f(CELL_SIZE / 2, CELL_SIZE / 2));
    player.setFillColor(sf::Color::Green);
    player.setPosition(CELL_SIZE + 7.5f, CELL_SIZE + 7.5f);

    sf::Clock clock;
    sf::Clock gameTimer;

    std::vector<LeaderboardEntry> leaderboard;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == MENU && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (startText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameState = ENTER_NAME;
                } else if (leaderboardText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameState = LEADERBOARD;
                } else if (exitText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.close();
                }
            }

            if (gameState == ENTER_NAME) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r') {
                        if (username.size() < 4) {
                            username += static_cast<char>(event.text.unicode);
                        }
                    } else if (event.text.unicode == '\b' && !username.empty()) {
                        username.pop_back();
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    if (username.size() == 4) {
                        gameState = PLAY;
                        player.setPosition(CELL_SIZE + 7.5f, CELL_SIZE + 7.5f);
                        gameTimer.restart();
                    }
                }
            }

            if (gameState == LEADERBOARD && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Text backToMenu("Back to Menu", font, 40);
                backToMenu.setPosition(WINDOW_WIDTH / 2.0f - backToMenu.getGlobalBounds().width / 2.0f, WINDOW_HEIGHT - 100);
                if (backToMenu.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameState = MENU;
                }
            }
        }

        window.clear();

        if (gameState == MENU) {
            window.draw(menuBackground);
            window.draw(titleGlow);
            window.draw(title);
            window.draw(startText);
            window.draw(leaderboardText);
            window.draw(exitText);
        } else if (gameState == ENTER_NAME) {
            window.draw(menuBackground);
            nameInput.setString(username + "_");
            window.draw(enterNamePrompt);
            window.draw(nameInput);
            window.draw(startGamePrompt);
        } else if (gameState == PLAY) {
            float deltaTime = clock.restart().asSeconds();
            deltaTime = std::min(deltaTime, 0.1f);

            sf::Vector2f movement(0.0f, 0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement.y -= 200 * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement.y += 200 * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement.x -= 200 * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += 200 * deltaTime;

            sf::Vector2f newPosition = player.getPosition() + sf::Vector2f(movement.x, 0.0f);
            sf::FloatRect playerBounds = player.getGlobalBounds();
            playerBounds.left = newPosition.x;
            bool collisionX = false;

            for (const auto& wall : walls) {
                if (wall.getGlobalBounds().intersects(playerBounds)) {
                    collisionX = true;
                    break;
                }
            }
            if (!collisionX) player.move(movement.x, 0.0f);

            newPosition = player.getPosition() + sf::Vector2f(0.0f, movement.y);
            playerBounds = player.getGlobalBounds();
            playerBounds.top = newPosition.y;
            bool collisionY = false;

            for (const auto& wall : walls) {
                if (wall.getGlobalBounds().intersects(playerBounds)) {
                    collisionY = true;
                    break;
                }
            }
            if (!collisionY) player.move(0.0f, movement.y);

            std::stringstream timerStream;
            timerStream << std::fixed << std::setprecision(2) << gameTimer.getElapsedTime().asSeconds();
            timerText.setString("Time: " + timerStream.str());

            if (player.getGlobalBounds().intersects(endpoint.getGlobalBounds())) {
                float finishTime = gameTimer.getElapsedTime().asSeconds();
                leaderboard.push_back({ username, finishTime });
                std::sort(leaderboard.begin(), leaderboard.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                    return a.time < b.time;
                });
                if (leaderboard.size() > 5) leaderboard.resize(5); // Keep only the top 5
                gameState = LEADERBOARD;
            }

            for (const auto& path : paths) window.draw(path);
            for (const auto& wall : walls) window.draw(wall);
            window.draw(endpoint);
            window.draw(player);
            window.draw(timerText);
        } else if (gameState == LEADERBOARD) {
            window.draw(menuBackground);

            sf::Text leaderboardTitle("Leaderboard", font, 50);
            leaderboardTitle.setFillColor(sf::Color::Yellow);
            leaderboardTitle.setPosition(WINDOW_WIDTH / 2.0f - leaderboardTitle.getGlobalBounds().width / 2.0f, 50);
            window.draw(leaderboardTitle);

            float yPosition = 150;
            for (size_t i = 0; i < leaderboard.size(); ++i) {
                std::ostringstream entryStream;
                entryStream << (i + 1) << ". " << leaderboard[i].name << " - " << leaderboard[i].time << "s";
                sf::Text entryText(entryStream.str(), font, 30);
                entryText.setFillColor(sf::Color::White);
                entryText.setPosition(100, yPosition);
                yPosition += 50;
                window.draw(entryText);
            }

            sf::Text backToMenu("Back to Menu", font, 40);
            backToMenu.setFillColor(sf::Color::Yellow);
            backToMenu.setPosition(WINDOW_WIDTH / 2.0f - backToMenu.getGlobalBounds().width / 2.0f, WINDOW_HEIGHT - 100);
            window.draw(backToMenu);
        }

        window.display();
    }

    return 0;
}
