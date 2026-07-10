/*
Semester Project BSSE Fall 2025
Javeria Nadeem (2502876)
Iraj Nawaz (2502902)
*/



#include <iostream>
#include <cstring>
#include <SFML/Graphics.hpp>
#include <time.h>
#include "utils.h"
#include "pieces.h"
#include "functionality.h"
using namespace sf;
using namespace std;


void startGame();


// --- COLORS (Neon Palette) ---
const Color COL_BG_TOP(15, 10, 30);      // Deep Purple
const Color COL_BG_BOT(0, 0, 0);         // Black
const Color COL_BTN_IDLE(40, 40, 60, 200);
const Color COL_BTN_HOVER(60, 150, 255, 230); // Neon Cyan
const Color COL_TEXT_GLOW(255, 255, 255);
const Color COL_GRID_LINE(30, 30, 50);

// Draw a gradient background
void drawBackground(RenderWindow &window) {
    VertexArray bg(PrimitiveType::TriangleStrip, 4);
    bg[0].position = Vector2f(0, 0);                 bg[0].color = COL_BG_TOP;
    bg[1].position = Vector2f(0, window.getSize().y); bg[1].color = COL_BG_BOT;
    bg[2].position = Vector2f(window.getSize().x, 0); bg[2].color = COL_BG_TOP;
    bg[3].position = Vector2f(window.getSize().x, window.getSize().y); bg[3].color = COL_BG_BOT;
    window.draw(bg);
}

// Modern Neon Button
bool drawButton(RenderWindow &window, Font &font, String text, int yPos, int width = 200) {
    int xPos = (window.getSize().x - width) / 2;
    RectangleShape btn(Vector2f(width, 50));
    btn.setPosition(Vector2f(xPos, yPos));
    btn.setOutlineThickness(2);
    
    // Hover 
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    bool hovered = btn.getGlobalBounds().contains(mouse);

    if (hovered) {
        btn.setFillColor(COL_BTN_HOVER);
        btn.setOutlineColor(Color::White);
    } else {
        btn.setFillColor(COL_BTN_IDLE);
        btn.setOutlineColor(Color(100, 100, 150));
    }

    Text btnText(font);
    btnText.setString(text);
    btnText.setCharacterSize(22);
    btnText.setFillColor(Color::White);
    
    // Center text
    FloatRect textRect = btnText.getLocalBounds();
    btnText.setOrigin(Vector2f(textRect.position.x + textRect.size.x / 2.0f,
                               textRect.position.y + textRect.size.y / 2.0f));
    btnText.setPosition(Vector2f(xPos + width / 2.0f, yPos + 25));

    window.draw(btn);
    window.draw(btnText);
    
    // Return true if clicked
    return hovered && Mouse::isButtonPressed(Mouse::Button::Left);
}

// Procedural Neon Block (Replaces Sprite)
void drawNeonBlock(RenderWindow &window, int x, int y, int colorIdx, int alpha = 255) {
    // Colors mapped to index
    Color c;
    switch(colorIdx) {
        case 1: c = Color::Cyan; break;
        case 2: c = Color::Blue; break;
        case 3: c = Color(255, 165, 0); break; // Orange
        case 4: c = Color::Yellow; break;
        case 5: c = Color::Green; break;
        case 6: c = Color::Magenta; break;
        case 7: c = Color::Red; break;
        default: c = Color::White;
    }
    
    RectangleShape rect(Vector2f(cellSize - 2, cellSize - 2));
    rect.setPosition(Vector2f(x * cellSize + offsetX + 1, y * cellSize + offsetY + 1));
    
    // Glassy look
    Color fill = c; 
    fill.a = (alpha == 255) ? 180 : 50; // High transparency if ghost
    rect.setFillColor(fill);
    
    // Neon Outline
    rect.setOutlineThickness(1.5f);
    Color outline = c;
    outline.a = (alpha == 255) ? 255 : 100;
    rect.setOutlineColor(outline);
    
    window.draw(rect);
}
// help screen
void showHelp(sf::Font &font) {
    sf::RenderWindow window(VideoMode({400, 600}), "Instructions");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
        }

        drawBackground(window);

        Text title(font, "HOW TO PLAY", 35);
        title.setStyle(Text::Bold);
        title.setFillColor(Color::Cyan);
        title.setPosition(Vector2f(80, 40));
        window.draw(title);

        vector<string> lines = {
            "ARROWS to Move",
            "UP to Rotate",
            "DOWN for Speed",
            "10 Lines = Level Up",
            "Don't stack to top!"
        };

        int y = 120;
        for(const string& line : lines) {
            Text t(font, line, 20);
            t.setPosition(Vector2f(60, y));
            window.draw(t);
            y += 50;
        }

        if (drawButton(window, font, "BACK", 500, 150)) window.close();

        window.display();
    }
}

void showHighScores(Font &font) {
    sf::RenderWindow window(VideoMode({400, 600}), "High Scores");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
        }

        drawBackground(window);

        Text title(font, "HALL OF FAME", 32);
        title.setStyle(Text::Bold);
        title.setFillColor(Color::Magenta);
        title.setPosition(Vector2f(80, 40));
        window.draw(title);

        int y = 120;
        for (int i = 0; i < 10; i++) {
            if (highScores[i] == 0) continue;
            Text t(font, to_string(i + 1) + ". " + to_string(highScores[i]), 22);
            t.setPosition(Vector2f(100, y));
            window.draw(t);
            y += 35;
        }

        if (drawButton(window, font, "BACK", 520, 150)) window.close();
        window.display();
    }
}

// pause screen
int showPauseMenu(sf::Font &font, sf::RenderWindow &parent) {

    RectangleShape overlay(Vector2f(parent.getSize()));
    overlay.setFillColor(Color(0, 0, 0, 150));
    parent.draw(overlay);
    parent.display();

    RenderWindow window(VideoMode({300, 500}), "Paused"); // height increased for extra buttons
    window.setFramerateLimit(60);

    while(window.isOpen()) {
        while(const optional e = window.pollEvent()) {
            if (e->is<Event::Closed>()) return 1; // treat close as Exit
        }

        drawBackground(window);

        Text t(font, "PAUSED", 30);
        t.setPosition(Vector2f(90, 30));
        window.draw(t);

        if (drawButton(window, font, "RESUME", 100)) { window.close(); return 0; }
        if (drawButton(window, font, "NEW GAME", 170)) { window.close(); startGame(); return 2; }
        if (drawButton(window, font, "HIGH SCORES", 240)) { showHighScores(font); }
        if (drawButton(window, font, "HELP", 310)) { showHelp(font); }
        if (drawButton(window, font, "EXIT", 380)) { window.close(); return 1; }

        window.display();
    }
    return 0;
}


// loading
void showLoading(sf::Font &font)
{
    sf::RenderWindow window(sf::VideoMode({400, 600}), "Loading");
    window.setFramerateLimit(60);

    sf::Clock clock;

    while (window.isOpen())
    {
        while (auto e = window.pollEvent())
        {
            if (e->is<sf::Event::Closed>())
                window.close();
        }

        // Auto close after short delay
        if (clock.getElapsedTime().asSeconds() > 1.5f)
            window.close();

        window.clear();
        drawBackground(window);

        sf::Text title(font, "LOADING...", 40);
        title.setFillColor(sf::Color::Cyan);
        title.setPosition({80, 120});
        window.draw(title);

        drawButton(window, font, "PLEASE WAIT", 260);

        window.display();
    }
}

// game over screen
void showGameOver(sf::Font &font, int score)
{
    sf::RenderWindow window(sf::VideoMode({400, 600}), "Game Over");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (auto e = window.pollEvent())
        {
            if (e->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        drawBackground(window);

        sf::Text title(font, "GAME OVER", 45);
        title.setFillColor(sf::Color::Red);
        title.setStyle(sf::Text::Bold);
        title.setPosition({60, 80});
        window.draw(title);

        sf::Text sc(font, "Score: " + std::to_string(score), 26);
        sc.setPosition({120, 160});
        window.draw(sc);


        if (drawButton(window, font, "PLAY AGAIN", 260))
        {
            window.close();
            showLoading(font);
            startGame();
            return;
        }

        if (drawButton(window, font, "MAIN MENU", 330))
        {
           window.close();
            return;
        }

        if (drawButton(window, font, "EXIT", 400))
        {
            window.close();
            exit(0);
        }

        window.display();
    }
}



int main() {
    sf::RenderWindow window(VideoMode({400, 600}), "Tetris Main Menu");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.openFromFile("ARIAL.TTF")) return 0;

    loadHighScores();

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
        }

        window.clear();
        drawBackground(window);
   

        // Title with Shadow
        Text titleShadow(font, "TETRIS", 55);
        titleShadow.setStyle(Text::Bold);
        titleShadow.setFillColor(Color(100, 0, 100)); // Dark shadow
        titleShadow.setPosition(Vector2f(105, 55));
        
        Text title(font, "TETRIS", 55);
        title.setStyle(Text::Bold);
        title.setFillColor(Color::Cyan);
        title.setPosition(Vector2f(100, 50));
        
        window.draw(titleShadow);
        window.draw(title);

        // Draw Buttons
        if (drawButton(window, font, "PLAY", 200)||(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)||(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
)
)
        {
            showLoading(font);
            startGame();
        }
        if (drawButton(window, font, "HIGH SCORES", 270)) showHighScores(font);
        if (drawButton(window, font, "HELP", 340)) showHelp(font);
        if (drawButton(window, font, "EXIT", 410)) window.close();

       

        window.display();
    }

    return 0;
}


// start game definition
void startGame()
{

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            gameGrid[i][j] = 0;

    gameOverFlag = false;
    level = 1;
    linesClearedTotal = 0;
    score = 0;
    srand(time(0));

    RenderWindow window(VideoMode({320, 480}), "SFML");
    Texture tiles, backgroundTex, frameTex;
    if (!tiles.loadFromFile("img/tiles.png") ||
        !backgroundTex.loadFromFile("img/background.png"))
    {
        cout << "Error loading textures\n";
        return;
    }

    Sprite sprite(tiles), background(backgroundTex), frame(frameTex);
    Font font("ARIAL.TTF");
    Text scoreText(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString(std::to_string(score));
    scoreText.setPosition(Vector2f(10, 10));
    RectangleShape frameRect(Vector2f(N * cellSize, M * cellSize));
    frameRect.setPosition(Vector2f(offsetX, offsetY));
    frameRect.setFillColor(Color::Transparent);
    frameRect.setOutlineThickness(3);
    frameRect.setOutlineColor(Color(255, 215, 0));
    RectangleShape cell(Vector2f(cellSize, cellSize));
    cell.setFillColor(Color::Transparent);
    cell.setOutlineThickness(1);
    cell.setOutlineColor(Color(120, 120, 120));
    int delta_x = 0, colorNum = 1;
    float timer = 0;
    Clock clock;

    spawnPiece(Pieces, currentPiece, point_1, colorNum);

 

    // Main loop
    while (window.isOpen() && !gameOverFlag)
    {
        float time = clock.restart().asSeconds();
        timer += time;
        float delay = 0.8f;

        // --- Event handling ---
        while (const std::optional e = window.pollEvent())
        {
            if (e->is<Event::Closed>())
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))

            {
                isPaused = true;
                int choice = showPauseMenu(font, window);
                isPaused = false;

                if (choice == 1)
                {
                    window.close();
                    return; 
                }
            }
            if (!isPaused)
            {
                if (Keyboard::isKeyPressed(Keyboard::Key::Up))
                    rotatePiece();
                else if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                    movePiece(-1);
                else if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                    movePiece(1);

                // --- Real-time key for DOWN (fast drop) ---
                if (Keyboard::isKeyPressed(Keyboard::Key::Down))
                    delay = 0.1;
            }
        }
      

     

        if (gameOverFlag && !highScoreUpdated)
        {
            updateHighScores(score);
            highScoreUpdated = true;
            showGameOver(font,score);
            window.close();
        }

        scoreText.setString("Score: " + to_string(score));

        // --- Draw everything ---
        window.clear(Color::Black);
        window.draw(background);
        window.draw(frameRect);
  // --- Shadow ---
        drawShadow(window, sprite);
            fallingPiece(timer, delay, currentPiece, colorNum);
        // drawing grid lines
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                cell.setPosition(Vector2f(j * cellSize + offsetX, i * cellSize + offsetY));
                window.draw(cell);
            }
        }

        // --- Draw grid ---
        for (int i = 0; i < M; i++)
{
    for (int j = 0; j < N; j++)
    {
        int tile = gameGrid[i][j];
        if (tile <= 0) continue; 

        sprite.setColor(Color::White);
        sprite.setTextureRect(IntRect({(tile-1)*cellSize, 0}, {cellSize, cellSize}));
        sprite.setPosition(Vector2f(j*cellSize + offsetX, i*cellSize + offsetY));
        window.draw(sprite);
    }
}


       


        // --- Current piece ---
        for (int i = 0; i < 4; i++)
        {
            sprite.setTextureRect(IntRect({(colorNum - 1) * cellSize, 0}, {cellSize, cellSize}));
            sprite.setPosition(Vector2f(point_1[i][0] * cellSize + offsetX, point_1[i][1] * cellSize + offsetY));
            window.draw(sprite);
        }

        window.draw(scoreText);
        window.display();
    }
}
