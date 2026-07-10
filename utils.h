/* PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)
 * This header contains the global/common variables of the project.
 * You don't really need to change anything.
 * TIP: After completing your project, you can make changes for aesthetics.
 * */


 /*
Semester Project BSSE Fall 2025
Javeria Nadeem (2502876)
Iraj Nawaz (2502902)
*/


//---Height and Width of the Actual Interactive Game---//
const int M = 19;   //Number of rows for a piece to cover on the screen (not the entire screen) = 19
const int N = 10;   //Number of columns for a piece to cover on the screen (not the entire screen) = 10
const int cellSize=24;
const int offsetX = (320 - N * cellSize) / 2; 
const int offsetY = (480 - M * cellSize) / 2; 

//---The Actual Interactive Game Grid - Built Over (MxN)---//
int gameGrid[M][N] = {0};

//---To Hold the Coordinates of the Piee---//
int point_1[4][2], point_2[4][2];

//---Check Uncertain Conditions---//
bool anamoly() {
    for (int i = 0; i < 4; i++) {
        // Check horizontal bounds
        if (point_1[i][0] < 0 || point_1[i][0] >= N) return false;
        // Check vertical bounds
  if (point_1[i][1] >=M)
    return false;
        // Check collision with existing blocks in grid
        if (gameGrid[point_1[i][1]][point_1[i][0]] != 0) 
        return false;
    }
    return true;
}

int currentPiece;
  int score=0;
  bool gameOverFlag=false;


  int level = 1;          // current level
int linesClearedTotal = 0;
int highScores[10];
bool highScoreUpdated = false;


bool isPaused=false;

