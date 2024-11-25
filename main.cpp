#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "AI.h"

const int windowWidth = 800;
const int windowHeight = 600;
const int blockSize = 20;

int player1Score = 0;
int player2Score = 0;
int losingPlayer = 0;

enum class GameState { MainMenu, ModeSelection, ColorSelection, PlayerSolo, PlayerVsPlayer, PlayerVsBot, GameOver };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    Snake() {
        direction = Direction::Right;
        snakeBody.push_back(SnakeSegment(5, 5));
        snakeBody.push_back(SnakeSegment(4, 5));
        snakeBody.push_back(SnakeSegment(3, 5));
        color = sf::Color::Green;
        savedColor = sf::Color::Green;  // Инициализация savedColor
    }

    void Move() {
        for (int i = snakeBody.size() - 1; i > 0; --i) {
            snakeBody[i].x = snakeBody[i - 1].x;
            snakeBody[i].y = snakeBody[i - 1].y;
        }

        switch (direction) {
        case Direction::Up:
            --snakeBody[0].y;
            break;
        case Direction::Down:
            ++snakeBody[0].y;
            break;
        case Direction::Left:
            --snakeBody[0].x;
            break;
        case Direction::Right:
            ++snakeBody[0].x;
            break;
        }
    }

    void Grow() {
        snakeBody.push_back(SnakeSegment(snakeBody.back().x, snakeBody.back().y));
    }

    void SetDirection(Direction dir) { direction = dir; }

    Direction GetDirection() const { return direction; }

    const std::vector<SnakeSegment>& GetBody() const { return snakeBody; }

    void Reset() {
        snakeBody.clear();
        snakeBody.push_back(SnakeSegment(5, 5));
        snakeBody.push_back(SnakeSegment(4, 5));
        snakeBody.push_back(SnakeSegment(3, 5));
        direction = Direction::Right;
        color = savedColor.a == 0 ? sf::Color::Green
            : savedColor;  // Проверка на инициализацию
    }

    void SetColor(const sf::Color& newColor) { color = newColor; }

    const sf::Color& GetColor() const { return color; }

    void SetSavedColor(const sf::Color& newColor) { savedColor = newColor; }

private:
    std::vector<SnakeSegment> snakeBody;
    Direction direction;
    sf::Color color;
    sf::Color savedColor;
};

class Game {
public:
    Game()
        : window(sf::VideoMode(windowWidth, windowHeight), "Snake Game"),
        snake(),
        snakeAI(),
        apple(rand() % (windowWidth / blockSize),
            rand() % (windowHeight / blockSize)) {
        window.setFramerateLimit(10);
        font.loadFromFile("arial.ttf");

        mainMenuText.setFont(font);
        mainMenuText.setString("SNAKE GAMEEEEE");
        mainMenuText.setCharacterSize(60);
        mainMenuText.setFillColor(sf::Color::White);
        mainMenuText.setPosition(
            windowWidth / 2 - mainMenuText.getLocalBounds().width / 2,
            windowHeight / 4);

        mainMenuInstructions.setFont(font);
        mainMenuInstructions.setString(
            "Press Enter to Play\n\nUse W, A, S, D to Move\n\nPress Esc to Quit");
        mainMenuInstructions.setCharacterSize(30);
        mainMenuInstructions.setFillColor(sf::Color::White);
        mainMenuInstructions.setPosition(
            windowWidth / 2 - mainMenuInstructions.getLocalBounds().width / 2,
            windowHeight / 2);

        modeSelectionText.setFont(font);
        modeSelectionText.setString(
            "Choose Game Mode:\n\n1 - Player Solo\n2 - Player vs Player\n3 - Player vs Bot\n\nPress Enter to Confirm");
        modeSelectionText.setCharacterSize(30);
        modeSelectionText.setFillColor(sf::Color::White);
        modeSelectionText.setPosition(windowWidth / 6, windowHeight / 6);

        colorSelectionText.setFont(font);
        colorSelectionText.setString(
            "Choose Snake Color:\n\n1 - Blue\n2 - Green\n3 - Red\n4 - Purple\n5 - "
            "White\n6 - Pink\n\nPress Enter to Confirm");
        colorSelectionText.setCharacterSize(30);
        colorSelectionText.setFillColor(sf::Color::White);
        colorSelectionText.setPosition(windowWidth / 6, windowHeight / 6);

        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        player1ScoreText.setFont(font);
        player1ScoreText.setCharacterSize(20);
        player1ScoreText.setFillColor(sf::Color::White);
        player1ScoreText.setPosition(10, 40);

        player2ScoreText.setFont(font);
        player2ScoreText.setCharacterSize(20);
        player2ScoreText.setFillColor(sf::Color::White);
        player2ScoreText.setPosition(10, 70);

        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(
            windowWidth / 2 - gameOverText.getLocalBounds().width / 2,
            windowHeight / 3);

        gameOverInstructions.setFont(font);
        gameOverInstructions.setString(
            "Press Enter to Restart\nPress Backspace for Main Menu");
        gameOverInstructions.setCharacterSize(30);
        gameOverInstructions.setFillColor(sf::Color::Red);
        gameOverInstructions.setPosition(
            windowWidth / 2 - gameOverInstructions.getLocalBounds().width / 2,
            windowHeight / 2);

        player1LoseText.setFont(font);
        player1LoseText.setString("Player 1 Lose");
        player1LoseText.setCharacterSize(60);
        player1LoseText.setFillColor(sf::Color::Red);
        player1LoseText.setPosition(
            windowWidth / 2 - player1LoseText.getLocalBounds().width / 2,
            windowHeight / 3);

        player2LoseText.setFont(font);
        player2LoseText.setString("Player 2 Lose");
        player2LoseText.setCharacterSize(60);
        player2LoseText.setFillColor(sf::Color::Red);
        player2LoseText.setPosition(
            windowWidth / 2 - player2LoseText.getLocalBounds().width / 2,
            windowHeight / 3);

        // Инициализация второй змейки
        snakeAI.SetColor(getRandomColor());
        snakeAI.SetSavedColor(snakeAI.GetColor());
        snakeAI.Reset();
    }

    void Run() {
        while (window.isOpen()) {
            ProcessEvents();
            if (state == GameState::PlayerSolo || state == GameState::PlayerVsPlayer || state == GameState::PlayerVsBot) {
                Update();
            }
            Render();
        }
    }

private:
    sf::RenderWindow window;
    Snake snake;
    Snake snakeAI;
    sf::Vector2i apple;
    sf::Font font;
    sf::Text mainMenuText;
    sf::Text mainMenuInstructions;
    sf::Text modeSelectionText;
    sf::Text colorSelectionText;
    sf::Text scoreText;
    sf::Text player1ScoreText;
    sf::Text player2ScoreText;
    sf::Text gameOverText;
    sf::Text gameOverInstructions;
    sf::Text player1LoseText;
    sf::Text player2LoseText;

    int score = 0;
    GameState state = GameState::MainMenu;
    sf::Color selectedColor = sf::Color::Green;
    AI ai = AI(windowWidth / blockSize, windowHeight / blockSize);

    void ProcessEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (state == GameState::MainMenu) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        state = GameState::ModeSelection;
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
            else if (state == GameState::ModeSelection) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        state = GameState::ColorSelection;
                        selectedMode = GameState::PlayerSolo;
                    }
                    else if (event.key.code == sf::Keyboard::Num2) {
                        state = GameState::ColorSelection;
                        selectedMode = GameState::PlayerVsPlayer;
                    }
                    else if (event.key.code == sf::Keyboard::Num3) {
                        state = GameState::ColorSelection;
                        selectedMode = GameState::PlayerVsBot;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        state = GameState::ColorSelection;
                    }
                }
            }
            else if (state == GameState::ColorSelection) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        selectedColor = sf::Color::Blue;
                    }
                    else if (event.key.code == sf::Keyboard::Num2) {
                        selectedColor = sf::Color::Green;
                    }
                    else if (event.key.code == sf::Keyboard::Num3) {
                        selectedColor = sf::Color::Red;
                    }
                    else if (event.key.code == sf::Keyboard::Num4) {
                        selectedColor = sf::Color::Magenta;
                    }
                    else if (event.key.code == sf::Keyboard::Num5) {
                        selectedColor = sf::Color::White;
                    }
                    else if (event.key.code == sf::Keyboard::Num6) {
                        selectedColor = sf::Color::Cyan;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedMode == GameState::PlayerSolo) {
                            snake.SetColor(selectedColor);
                            snake.SetSavedColor(selectedColor);
                            state = GameState::PlayerSolo;
                            snake.Reset();
                            score = 0;
                            apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                                rand() % (windowHeight / blockSize));
                        }
                        else if (selectedMode == GameState::PlayerVsPlayer) {
                            if (player1Color == sf::Color::Transparent) {
                                player1Color = selectedColor;
                            }
                            else {
                                player2Color = selectedColor;
                                snake.SetColor(player1Color);
                                snake.SetSavedColor(player1Color);
                                snakeAI.SetColor(player2Color);
                                snakeAI.SetSavedColor(player2Color);
                                state = GameState::PlayerVsPlayer;
                                snake.Reset();
                                snakeAI.Reset();
                                score = 0;
                                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                                    rand() % (windowHeight / blockSize));
                            }
                        }
                        else if (selectedMode == GameState::PlayerVsBot) {
                            snake.SetColor(selectedColor);
                            snake.SetSavedColor(selectedColor);
                            state = GameState::PlayerVsBot;
                            snake.Reset();
                            snakeAI.Reset();
                            score = 0;
                            apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                                rand() % (windowHeight / blockSize));
                        }
                    }
                }
            }
            else if (state == GameState::PlayerSolo) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W &&
                        snake.GetDirection() != Direction::Down) {
                        snake.SetDirection(Direction::Up);
                    }
                    else if (event.key.code == sf::Keyboard::S &&
                        snake.GetDirection() != Direction::Up) {
                        snake.SetDirection(Direction::Down);
                    }
                    else if (event.key.code == sf::Keyboard::A &&
                        snake.GetDirection() != Direction::Right) {
                        snake.SetDirection(Direction::Left);
                    }
                    else if (event.key.code == sf::Keyboard::D &&
                        snake.GetDirection() != Direction::Left) {
                        snake.SetDirection(Direction::Right);
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
            else if (state == GameState::PlayerVsPlayer) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W &&
                        snake.GetDirection() != Direction::Down) {
                        snake.SetDirection(Direction::Up);
                    }
                    else if (event.key.code == sf::Keyboard::S &&
                        snake.GetDirection() != Direction::Up) {
                        snake.SetDirection(Direction::Down);
                    }
                    else if (event.key.code == sf::Keyboard::A &&
                        snake.GetDirection() != Direction::Right) {
                        snake.SetDirection(Direction::Left);
                    }
                    else if (event.key.code == sf::Keyboard::D &&
                        snake.GetDirection() != Direction::Left) {
                        snake.SetDirection(Direction::Right);
                    }
                    else if (event.key.code == sf::Keyboard::Up &&
                        snakeAI.GetDirection() != Direction::Down) {
                        snakeAI.SetDirection(Direction::Up);
                    }
                    else if (event.key.code == sf::Keyboard::Down &&
                        snakeAI.GetDirection() != Direction::Up) {
                        snakeAI.SetDirection(Direction::Down);
                    }
                    else if (event.key.code == sf::Keyboard::Left &&
                        snakeAI.GetDirection() != Direction::Right) {
                        snakeAI.SetDirection(Direction::Left);
                    }
                    else if (event.key.code == sf::Keyboard::Right &&
                        snakeAI.GetDirection() != Direction::Left) {
                        snakeAI.SetDirection(Direction::Right);
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
            else if (state == GameState::PlayerVsBot) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W &&
                        snake.GetDirection() != Direction::Down) {
                        snake.SetDirection(Direction::Up);
                    }
                    else if (event.key.code == sf::Keyboard::S &&
                        snake.GetDirection() != Direction::Up) {
                        snake.SetDirection(Direction::Down);
                    }
                    else if (event.key.code == sf::Keyboard::A &&
                        snake.GetDirection() != Direction::Right) {
                        snake.SetDirection(Direction::Left);
                    }
                    else if (event.key.code == sf::Keyboard::D &&
                        snake.GetDirection() != Direction::Left) {
                        snake.SetDirection(Direction::Right);
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
            else if (state == GameState::GameOver) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        state = GameState::MainMenu;
                    }
                    else if (event.key.code == sf::Keyboard::Backspace) {
                        state = GameState::MainMenu;
                    }
                }
            }
        }
    }

    void Update() {
        if (state == GameState::PlayerSolo) {
            snake.Move();

            if (snake.GetBody()[0].x == apple.x && snake.GetBody()[0].y == apple.y) {
                snake.Grow();
                score += 10;
                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                    rand() % (windowHeight / blockSize));
            }

            if (snake.GetBody()[0].x < 0 ||
                snake.GetBody()[0].x >= windowWidth / blockSize ||
                snake.GetBody()[0].y < 0 ||
                snake.GetBody()[0].y >= windowHeight / blockSize) {
                state = GameState::GameOver;
            }

            for (size_t i = 1; i < snake.GetBody().size(); ++i) {
                if (snake.GetBody()[0].x == snake.GetBody()[i].x &&
                    snake.GetBody()[0].y == snake.GetBody()[i].y) {
                    state = GameState::GameOver;
                }
            }
        }
        else if (state == GameState::PlayerVsPlayer) {
            snake.Move();
            snakeAI.Move();

            if (snake.GetBody()[0].x == apple.x && snake.GetBody()[0].y == apple.y) {
                snake.Grow();
                player1Score += 10;
                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                    rand() % (windowHeight / blockSize));
            }

            if (snakeAI.GetBody()[0].x == apple.x &&
                snakeAI.GetBody()[0].y == apple.y) {
                snakeAI.Grow();
                player2Score += 10;
                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                    rand() % (windowHeight / blockSize));
            }

            if (snake.GetBody()[0].x < 0 ||
                snake.GetBody()[0].x >= windowWidth / blockSize ||
                snake.GetBody()[0].y < 0 ||
                snake.GetBody()[0].y >= windowHeight / blockSize) {
                losingPlayer = 1;
                state = GameState::GameOver;
            }

            if (snakeAI.GetBody()[0].x < 0 ||
                snakeAI.GetBody()[0].x >= windowWidth / blockSize ||
                snakeAI.GetBody()[0].y < 0 ||
                snakeAI.GetBody()[0].y >= windowHeight / blockSize) {
                losingPlayer = 2;
                state = GameState::GameOver;
            }

            for (size_t i = 1; i < snake.GetBody().size(); ++i) {
                if (snake.GetBody()[0].x == snake.GetBody()[i].x &&
                    snake.GetBody()[0].y == snake.GetBody()[i].y) {
                    losingPlayer = 1;
                    state = GameState::GameOver;
                }
            }

            for (size_t i = 1; i < snakeAI.GetBody().size(); ++i) {
                if (snakeAI.GetBody()[0].x == snakeAI.GetBody()[i].x &&
                    snakeAI.GetBody()[0].y == snakeAI.GetBody()[i].y) {
                    losingPlayer = 2;
                    state = GameState::GameOver;
                }
            }
        }
        else if (state == GameState::PlayerVsBot) {
            snake.Move();
            std::vector<Point> snakeAIBody;
            for (const auto& segment : snakeAI.GetBody()) {
                snakeAIBody.push_back(Point(segment.x, segment.y));
            }
            snakeAI.SetDirection(
                ai.getNextDirection(snakeAIBody, Point(apple.x, apple.y)));
            snakeAI.Move();

            if (snake.GetBody()[0].x == apple.x && snake.GetBody()[0].y == apple.y) {
                snake.Grow();
                player1Score += 10;
                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                    rand() % (windowHeight / blockSize));
            }

            if (snakeAI.GetBody()[0].x == apple.x &&
                snakeAI.GetBody()[0].y == apple.y) {
                snakeAI.Grow();
                player2Score += 10;
                apple = sf::Vector2i(rand() % (windowWidth / blockSize),
                    rand() % (windowHeight / blockSize));
            }

            if (snake.GetBody()[0].x < 0 ||
                snake.GetBody()[0].x >= windowWidth / blockSize ||
                snake.GetBody()[0].y < 0 ||
                snake.GetBody()[0].y >= windowHeight / blockSize) {
                losingPlayer = 1;
                state = GameState::GameOver;
            }

            if (snakeAI.GetBody()[0].x < 0 ||
                snakeAI.GetBody()[0].x >= windowWidth / blockSize ||
                snakeAI.GetBody()[0].y < 0 ||
                snakeAI.GetBody()[0].y >= windowHeight / blockSize) {
                losingPlayer = 2;
                state = GameState::GameOver;
            }

            for (size_t i = 1; i < snake.GetBody().size(); ++i) {
                if (snake.GetBody()[0].x == snake.GetBody()[i].x &&
                    snake.GetBody()[0].y == snake.GetBody()[i].y) {
                    losingPlayer = 1;
                    state = GameState::GameOver;
                }
            }

            for (size_t i = 1; i < snakeAI.GetBody().size(); ++i) {
                if (snakeAI.GetBody()[0].x == snakeAI.GetBody()[i].x &&
                    snakeAI.GetBody()[0].y == snakeAI.GetBody()[i].y) {
                    losingPlayer = 2;
                    state = GameState::GameOver;
                }
            }
        }
    }

    void Render() {
        window.clear();

        if (state == GameState::MainMenu) {
            mainMenuText.setString("SNAKE GAME");
            mainMenuInstructions.setString(
                "Press Enter to Play\n\nUse W, A, S, D to Move\n\nPress Esc to Quit");
            window.draw(mainMenuText);
            window.draw(mainMenuInstructions);

            // Обводка для инструкций
            sf::RectangleShape rect(
                sf::Vector2f(mainMenuInstructions.getLocalBounds().width + 20,
                    mainMenuInstructions.getLocalBounds().height + 20));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::White);
            rect.setOutlineThickness(2);
            rect.setPosition(mainMenuInstructions.getPosition().x - 10,
                mainMenuInstructions.getPosition().y - 10);
            window.draw(rect);
        }
        else if (state == GameState::ModeSelection) {
            window.draw(modeSelectionText);
        }
        else if (state == GameState::ColorSelection) {
            window.draw(colorSelectionText);
        }
        else if (state == GameState::PlayerSolo || state == GameState::PlayerVsPlayer || state == GameState::PlayerVsBot) {
            // змейка
            for (const auto& segment : snake.GetBody()) {
                sf::RectangleShape block(sf::Vector2f(blockSize - 1, blockSize - 1));
                block.setFillColor(snake.GetColor());
                block.setPosition(segment.x * blockSize, segment.y * blockSize);
                window.draw(block);
            }

            // змейка ИИ (только для PlayerVsPlayer и PlayerVsBot)
            if (state == GameState::PlayerVsPlayer || state == GameState::PlayerVsBot) {
                for (const auto& segment : snakeAI.GetBody()) {
                    sf::RectangleShape block(sf::Vector2f(blockSize - 1, blockSize - 1));
                    block.setFillColor(snakeAI.GetColor());
                    block.setPosition(segment.x * blockSize, segment.y * blockSize);
                    window.draw(block);
                }
            }

            // яблоко
            sf::RectangleShape block(sf::Vector2f(blockSize - 1, blockSize - 1));
            block.setFillColor(sf::Color::Red);
            block.setPosition(apple.x * blockSize, apple.y * blockSize);
            window.draw(block);

            // счет
            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);

            // счет игрока 1
            if (state == GameState::PlayerVsPlayer || state == GameState::PlayerVsBot) {
                player1ScoreText.setString("Player 1 Score: " + std::to_string(player1Score));
                window.draw(player1ScoreText);
            }

            // счет игрока 2
            if (state == GameState::PlayerVsPlayer) {
                player2ScoreText.setString("Player 2 Score: " + std::to_string(player2Score));
                window.draw(player2ScoreText);
            }
        }
        else if (state == GameState::GameOver) {
            if (losingPlayer == 1) {
                window.draw(player1LoseText);
            }
            else if (losingPlayer == 2) {
                window.draw(player2LoseText);
            }
            else {
                window.draw(gameOverText);
            }
            window.draw(gameOverInstructions);

            // Обводка для инструкций
            sf::RectangleShape rect(
                sf::Vector2f(gameOverInstructions.getLocalBounds().width + 20,
                    gameOverInstructions.getLocalBounds().height + 20));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(2);
            rect.setPosition(gameOverInstructions.getPosition().x - 10,
                gameOverInstructions.getPosition().y - 10);
            window.draw(rect);
        }

        window.display();
    }

    sf::Color getRandomColor() {
        std::vector<sf::Color> colors = { sf::Color::Blue,   sf::Color::Green,
                                         sf::Color::Red,    sf::Color::Magenta,
                                         sf::Color::White,  sf::Color::Cyan,
                                         sf::Color::Yellow, sf::Color::Black };
        return colors[rand() % colors.size()];
    }

    GameState selectedMode = GameState::PlayerSolo;
    sf::Color player1Color = sf::Color::Transparent;
    sf::Color player2Color = sf::Color::Transparent;
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    Game game;
    game.Run();

    return 0;
}