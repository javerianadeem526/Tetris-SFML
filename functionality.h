/*
Semester Project BSSE Fall 2025
Javeria Nadeem (2502876)
Iraj Nawaz (2502902)
*/

#include<fstream>
/////////////////////////////////////////////
///*** START CODING YOUR FUNTIONS HERE ***///

void spawnPiece(int arr[7][4][4], int &currentPiece, int point_1[4][2], int &colorNum)
{
    if(level == 1)
        currentPiece = rand() % 4;
    else
        currentPiece = rand() % 7;

    // Random color independent of piece type
    colorNum = (rand() % 7)+1;

    int i = 0;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (arr[currentPiece][r][c] == 1)
            {
                point_1[i][0] = c + N / 2 - 2; // spawn column
                point_1[i][1] = r;             // spawn row
                i++;
            }
        }
    }
}




bool gameOver()
{
    for (int j = 0; j < N; j++)  
    {
        if (gameGrid[0][j] != 0) 
            return true;          
    }
    return false;                 
}



void clearLines()
{
    int linesCleared = 0;

    for (int i = M - 1; i >= 0; i--)   // bottom to top
    {
        bool fullLine = true;

        for (int j = 0; j < N; j++)
        {
            if (gameGrid[i][j] == 0)
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            linesCleared++;

            // move all rows above down
            for (int k = i; k > 0; k--)
            {
                for (int j = 0; j < N; j++)
                    gameGrid[k][j] = gameGrid[k - 1][j];
            }

            // clear top row
            for (int j = 0; j < N; j++)
                gameGrid[0][j] = 0;

            i++; // recheck same row
        }
    }

 
  linesClearedTotal += linesCleared;

if (linesCleared == 1) score += 10 * level;
else if (linesCleared == 2) score += 30 * level;
else if (linesCleared == 3) score += 60 * level;
else if (linesCleared >= 4) score += 100 * level;


if (linesClearedTotal >= 10 && level == 1)
{
    level = 2;
}
}







void drawShadow(sf::RenderWindow &window, sf::Sprite &sprite)
{
    // Copy current piece coordinates
    int shadow[4][2];
    for (int i = 0; i < 4; i++)
    {
        shadow[i][0] = point_1[i][0];
        shadow[i][1] = point_1[i][1];
    }

    // Move shadow down until collision
    bool canMove = true;
    while (canMove)
    {
        for (int i = 0; i < 4; i++)
            shadow[i][1]++;

        // Check collision
        for (int i = 0; i < 4; i++)
        {
            int x = shadow[i][0];
            int y = shadow[i][1];

            if (y >= M || gameGrid[y][x] != 0)
            {
                canMove = false;
                break;
            }
        }
    }

    // Move back one step
    for (int i = 0; i < 4; i++)
        shadow[i][1]--;

    // Draw shadow blocks (light color)
    sprite.setColor(sf::Color(255, 255, 255, 80)); // transparent

    for (int i = 0; i < 4; i++)
    {
        sprite.setPosition(sf::Vector2f(shadow[i][0] * cellSize+offsetX,shadow[i][1] * cellSize+offsetY));
        window.draw(sprite);
    }

    // Reset sprite color
    sprite.setColor(sf::Color::White);
}





//---Piece Starts to Fall When Game Starts---//
void fallingPiece(float &timer, float &delay, int &currentPiece, int &colorNum)
{
    if (timer > delay)
    {
        // save current positions
        for (int i = 0; i < 4; i++)
        {
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1; // move down
        }

        // check collision AFTER moving down
        if (!anamoly())
        {
            // revert
            for (int i = 0; i < 4; i++)
                point_1[i][1] = point_2[i][1];

            // lock piece
            for (int i = 0; i < 4; i++)
                gameGrid[point_1[i][1]][point_1[i][0]] = colorNum;

            clearLines();

            if (gameOver())
            {
                gameOverFlag = true;
                return;
            }

            spawnPiece(Pieces, currentPiece, point_1, colorNum);
        }

        timer = 0;
    }
}



void movePiece(int dx)
{
    for (int i = 0; i < 4; i++)
        point_1[i][0] += dx;

    if (!anamoly())
    { // check collision with walls or blocks
        for (int i = 0; i < 4; i++)
            point_1[i][0] -= dx; // undo move if invalid
    }
}

void rotatePiece()
{
    int cx = point_1[1][0]; // pivot
    int cy = point_1[1][1];

    int temp[4][2];
    for (int i = 0; i < 4; i++)
    {
        int x = point_1[i][0];
        int y = point_1[i][1];

        temp[i][0] = cx - (y - cy); // new x
        temp[i][1] = cy + (x - cx); // new y
    }

    // check if rotation is valid
    bool valid = true;
    for (int i = 0; i < 4; i++)
    {
        int x = temp[i][0], y = temp[i][1];
        if (x < 0 || x >= N || y >= M || gameGrid[y][x] != 0)
        {
            valid = false;
            break;
        }
    }

    if (valid)
        for (int i = 0; i < 4; i++)
        {
            point_1[i][0] = temp[i][0];
            point_1[i][1] = temp[i][1];
        }
}




// high scores
void loadHighScores()
{
    std::ifstream file("highScores.txt");

    if (!file)
    {
        // If file doesn't exist, initialize with zeros
        for (int i = 0; i < 10; i++)
            highScores[i] = 0;
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        if (!(file >> highScores[i]))
            highScores[i] = 0;
    }

    file.close();
}

void saveHighScores()
{
    std::ofstream file("highScores.txt");

    for (int i = 0; i < 10; i++)
        file << highScores[i] << std::endl;

    file.close();
}



void updateHighScores(int currentScore)
{
    // Check if score qualifies
    if (currentScore <= highScores[9])
        return;

    // Replace lowest score
    highScores[9] = currentScore;

    // Sort array in descending order (bubble sort)
    for (int i = 0; i < 9; i++)
    {
        for (int j = i + 1; j < 10; j++)
        {
            if (highScores[i] < highScores[j])
            {
                int temp = highScores[i];
                highScores[i] = highScores[j];
                highScores[j] = temp;
            }
        }
    }

    saveHighScores();
}


void showHighScores()
{
    std::cout << "\n--- HIGH SCORES ---\n";
    for (int i = 0; i < 10; i++)
        std::cout << i + 1 << ". " << highScores[i] << std::endl;
}






///*** YOUR FUNCTIONS END HERE ***///
/////////////////////////////////////