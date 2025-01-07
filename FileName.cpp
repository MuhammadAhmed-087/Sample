
#include <SFML/Graphics.hpp>
#include <ctime>
#include <string>
using namespace sf;
using namespace std;

const int ROWS = 20;
const int COLS = 10;

int gameGrid[ROWS][COLS] = { 0 };

struct Block {
    int x, y;
} currentPiece[4], tempPiece[4];

// Tetromino shapes
int shapes[7][4] = {
    1, 3, 5, 7, // I-shape
    2, 4, 5, 7, // Z-shape
    3, 5, 4, 6, // S-shape
    3, 5, 4, 7, // T-shape
    2, 3, 5, 7, // L-shape
    3, 5, 7, 6, // J-shape
    2, 3, 4, 5  // O-shape
};

bool isValidPosition() {
    for (int i = 0; i < 4; i++) {
        if (currentPiece[i].x < 0 || currentPiece[i].x >= COLS || currentPiece[i].y >= ROWS || gameGrid[currentPiece[i].y][currentPiece[i].x])
            return false;
    }
    return true;
}

void initializeGame(int& score, int& blockColor, int gameGrid[ROWS][COLS], Block currentPiece[4], int shapes[7][4]) {
    score = 0;
    blockColor = 1;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            gameGrid[i][j] = 0;

    int shapeIndex = rand() % 7;
    int startX = rand() % (COLS - 2);
    for (int i = 0; i < 4; i++) {
        currentPiece[i].x = shapes[shapeIndex][i] % 2 + startX;
        currentPiece[i].y = shapes[shapeIndex][i] / 2;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(320, 480), "Tetris");

    Texture tileTexture, backgroundTexture, frameTexture, startScreenTexture, gameOverTexture;
    tileTexture.loadFromFile("images/tiles.png");
    backgroundTexture.loadFromFile("images/background.png");
    frameTexture.loadFromFile("images/frame.png");
    startScreenTexture.loadFromFile("images/gamestart.png");
    gameOverTexture.loadFromFile("images/gameover.png");

    Sprite tileSprite(tileTexture), backgroundSprite(backgroundTexture), frameSprite(frameTexture), startScreenSprite(startScreenTexture), gameOverSprite(gameOverTexture);

    startScreenSprite.setScale(
        window.getSize().x / static_cast<float>(startScreenTexture.getSize().x),
        window.getSize().y / static_cast<float>(startScreenTexture.getSize().y));
    gameOverSprite.setScale(
        window.getSize().x / static_cast<float>(gameOverTexture.getSize().x),
        window.getSize().y / static_cast<float>(gameOverTexture.getSize().y));

    window.clear();
    window.draw(startScreenSprite);
    window.display();
    sf::sleep(sf::seconds(4));

    int movementX = 0;
    bool shouldRotate = false;
    int blockColor = 1;
    float timeCounter = 0, dropDelay = 0.3f;
    int playerScore = 0;

    Font regularFont, boldFont;
    regularFont.loadFromFile("fonts/arial.ttf");
    boldFont.loadFromFile("fonts/arial.ttf");

    Text scoreDisplay;
    scoreDisplay.setFont(regularFont);
    scoreDisplay.setCharacterSize(20);
    scoreDisplay.setFillColor(Color::Black);
    scoreDisplay.setPosition(250, 20);

    Text replayPrompt;
    replayPrompt.setFont(boldFont);
    replayPrompt.setCharacterSize(20);
    replayPrompt.setFillColor(Color::White);
    replayPrompt.setString("Replay (R) or Exit (E)?");
    replayPrompt.setPosition(25, 290);

    Clock gameClock;

    int shapeIndex = rand() % 7;
    int startX = rand() % (COLS - 2);
    for (int i = 0; i < 4; i++) {
        currentPiece[i].x = shapes[shapeIndex][i] % 2 + startX;
        currentPiece[i].y = shapes[shapeIndex][i] / 2;
    }

    while (window.isOpen()) {
        float elapsedTime = gameClock.getElapsedTime().asSeconds();
        gameClock.restart();
        timeCounter += elapsedTime;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) shouldRotate = true;
                else if (event.key.code == Keyboard::Left) movementX = -1;
                else if (event.key.code == Keyboard::Right) movementX = 1;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) dropDelay = 0.05f;

        for (int i = 0; i < 4; i++) {
            tempPiece[i] = currentPiece[i];
            currentPiece[i].x += movementX;
        }
        if (!isValidPosition())
            for (int i = 0; i < 4; i++) currentPiece[i] = tempPiece[i];

        if (shouldRotate) {
            Block center = currentPiece[1];
            for (int i = 0; i < 4; i++) {
                int x = currentPiece[i].y - center.y;
                int y = currentPiece[i].x - center.x;
                currentPiece[i].x = center.x - x;
                currentPiece[i].y = center.y + y;
            }
            if (!isValidPosition())
                for (int i = 0; i < 4; i++) currentPiece[i] = tempPiece[i];
        }

        if (timeCounter > dropDelay) {
            for (int i = 0; i < 4; i++) {
                tempPiece[i] = currentPiece[i];
                currentPiece[i].y += 1;
            }

            if (!isValidPosition()) {
                for (int i = 0; i < 4; i++)
                    gameGrid[tempPiece[i].y][tempPiece[i].x] = blockColor;

                blockColor = 1 + rand() % 7;
                shapeIndex = rand() % 7;
                startX = rand() % (COLS - 2);
                for (int i = 0; i < 4; i++) {
                    currentPiece[i].x = shapes[shapeIndex][i] % 2 + startX;
                    currentPiece[i].y = shapes[shapeIndex][i] / 2;
                }

                for (int i = 0; i < 4; i++) {
                    if (gameGrid[currentPiece[i].y][currentPiece[i].x] != 0) {
                        window.clear();
                        window.draw(gameOverSprite);
                        window.draw(replayPrompt);
                        window.display();

                        bool waitingForInput = true;
                        while (waitingForInput) {
                            while (window.pollEvent(event)) {
                                if (event.type == Event::KeyPressed) {
                                    if (event.key.code == Keyboard::R) {
                                        initializeGame(playerScore, blockColor, gameGrid, currentPiece, shapes);
                                        waitingForInput = false;
                                    }
                                    if (event.key.code == Keyboard::E) {
                                        return 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            timeCounter = 0;
        }

        int rowToCopy = ROWS - 1;
        for (int i = ROWS - 1; i >= 0; i--) {
            int filledCells = 0;
            for (int j = 0; j < COLS; j++) {
                if (gameGrid[i][j]) filledCells++;
                gameGrid[rowToCopy][j] = gameGrid[i][j];
            }
            if (filledCells < COLS)
                rowToCopy--;
            else
                playerScore += 10;
        }

        movementX = 0;
        shouldRotate = false;
        dropDelay = 0.3f;

        scoreDisplay.setString("Score:\n" + to_string(playerScore));

        window.clear(Color::White);
        window.draw(backgroundSprite);

        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++) {
                if (gameGrid[i][j] == 0) continue;
                tileSprite.setTextureRect(IntRect(gameGrid[i][j] * 18, 0, 18, 18));
                tileSprite.setPosition(j * 18, i * 18);
                tileSprite.move(28, 31);
                window.draw(tileSprite);
            }

        for (int i = 0; i < 4; i++) {
            tileSprite.setTextureRect(IntRect(blockColor * 18, 0, 18, 18));
            tileSprite.setPosition(currentPiece[i].x * 18, currentPiece[i].y * 18);
            tileSprite.move(28, 31);
            window.draw(tileSprite);
        }

        window.draw(frameSprite);
        window.draw(scoreDisplay);
        window.display();
    }

    return 0;
}
