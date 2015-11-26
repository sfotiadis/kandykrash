// Fotiadis Efstathios
// AM 2009
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void checkForTriad();
void deleteTriad();

const int ROWS = 12;
const int COLUMNS = 15;
const int COLORS = 5;
int TILESIZE = 40;
const int PADDING = 0;
const int STATUSLINE = 40;
const int MARK_THICKNESS = 5;

// Types of tiles
const int BLACK = 10;
const int WHITE = 0;
const int BLUE = 1;
const int RED = 2;
const GLuint ROCK = 3; // GREEN
const GLuint PAPER = 4;
const GLuint SCISSORS = 5;
const int DELETE_COLOR = BLACK;
const int MARK_COLOR = WHITE;
// Orientation
const int HORIZONTAL = 0;
const int VERTICAL = 1;

// KEYS
const int ESC_KEY = 27;
const int B_KEY = 'b';

// Width: 40 X 15 columns = 600 + padding (16*2) = 632
GLsizei winWidth = COLUMNS * TILESIZE + (COLUMNS + 1) * PADDING;
// Height: 40 X 12 rows = 480 + padding (13*2) + status line (20) = 526
GLsizei winHeight = ROWS * TILESIZE + (ROWS + 1) * PADDING + STATUSLINE;

// State variables
bool gameStarted;
int grid[ROWS][COLUMNS];
bool firstClick;
int moves;
int initialMoves; // For restarting the game with the same number of moves
long int score;
char status[50];
// A tile structure, to hold the selected tiles coordinates
typedef struct Tile {
    int row;
    int col;
} Tile;
Tile tile1, tile2;
// Triad to be deleted
int triad[3][2];
int triadOrientation;
// Icons
GLubyte* ICON_ROCK;
GLubyte* ICON_PAPER;
GLubyte* ICON_SCISSORS;

// For debugging
void printGrid(){
    printf("\n");
    for(int i = 0; i < ROWS ; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            printf("%d\t",grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Randonly initialize the grid
void initGrid(){
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLUMNS; j++)
            // grid[i][j] = (i + j) % COLORS + 1;
            grid[i][j] = BLUE + (rand() % (int)(SCISSORS - BLUE + 1));
}

void initRendering() {
    glMatrixMode (GL_PROJECTION);
    // void gluOrtho2D ( left, right, bottom, top);
    gluOrtho2D (0.0, COLUMNS, ROWS + 1, 0.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
}

void setTextureFromType(int type) {
    switch(type) {
        case WHITE:
            glDisable(GL_TEXTURE_2D);
            glColor3f(1,1,1);
            break;
        case BLUE:
            glDisable(GL_TEXTURE_2D);
            glColor3f(0,0,1);
            break;
        case RED:
            glDisable(GL_TEXTURE_2D);
            glColor3f(1,0,0);
            break;
        case ROCK:
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, ROCK);
            glColor3f(0,1,0);
            break;
        case PAPER:
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, PAPER);
            glColor3f(1,0,1);
            break;
        case SCISSORS:
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, SCISSORS);
            glColor3f(1,1,0);
            break;
        case BLACK:
            glDisable(GL_TEXTURE_2D);
            glColor3f(0,0,0);
            break;
    }
}

void displayTile(int i, int j) {
    setTextureFromType(grid[j][i]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(i, j);
        glTexCoord2f (1, 0);
        glVertex2f(i+1, j);
        glTexCoord2f (1, 1);
        glVertex2f(i+1, j+1);
        glTexCoord2f (0, 1);
        glVertex2f(i, j+1);
    glEnd();
}

void displayGrid() {
    // glClear (GL_COLOR_BUFFER_BIT);

    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            displayTile(j, i); //
        }
    }
}

void drawString (char *s, float x, float y){
     unsigned int i;
     glRasterPos2f(x, y);

     for (i = 0; i < strlen(s); i++)
          glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, s[i]);
}

void displayStatusBar() {
    glColor3f(1,1,1);
    if(moves > 0) {
        sprintf(status, "Moves %d \t Score %d", moves, score);
    } else {
        sprintf(status, "Final Score %d - Press 'b' to start again", score);
    }

    drawString(status, 0.5 , ROWS + 0.5);
}

void displayPressKey() {
    glColor3f(1,1,1);
    drawString("Press 'b' to start", COLUMNS/2 - 2 , ROWS/2);
}

void markFirstTile() {
    setTextureFromType(MARK_COLOR);
    glLineWidth((GLfloat)MARK_THICKNESS);
    glBegin(GL_LINE_LOOP);
        glVertex2i(tile1.col, tile1.row);
        glVertex2i(tile1.col + 1, tile1.row);
        glVertex2i(tile1.col + 1, tile1.row + 1);
        glVertex2i(tile1.col, tile1.row + 1);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity(); //Reset the drawing perspective

    if(gameStarted) {
        displayGrid();
        displayStatusBar();
        if(gameStarted && (firstClick == false)) {
            markFirstTile();
        }
    } else {
        displayPressKey();
    }
    glutSwapBuffers();
}


bool isTileWithinBounds(int row, int col) {
    return (row >=0 && row < ROWS) && (col >=0 && col < COLUMNS);
}

// Returns true if the tile was correctly returned
bool getTileFromPixel(int x, int y, Tile* tile) {
    tile->row = y / TILESIZE; // Mouse reads from bottom-left
    tile->col = x / TILESIZE; // We are using top-left reference.
    if(isTileWithinBounds(tile->row, tile->col)) {
        // printf("Selected Tile %d %d - Color %d\n", tile->row, tile->col, grid[tile->row][tile->col]);
        return true;
    } else {
        printf("Tile out of bounds, select another tile\n");
        return false;
    }
}

bool areTilesNeighbours() {
    return ((tile1.row - tile2.row) == 0 && abs(tile1.col - tile2.col) == 1) || // same row neigbhors
            (abs(tile1.row - tile2.row) == 1 && (tile1.col - tile2.col) == 0);   // same column neighbors
}

// Returns true if a swap happened
bool swapTiles() {

    if(areTilesNeighbours()) {
        int buf = grid[tile1.row][tile1.col];
        grid[tile1.row][tile1.col] = grid[tile2.row][tile2.col];
        grid[tile2.row][tile2.col] = buf;

        // printGrid();
        return true;
    } else {
        printf("Not valid swap, select another tile\n");
        return false;
    }

}

void recolorTriad(int color) {
    for(int i = 0; i <3; i++) {
            grid[triad[i][0]][triad[i][1]] = color;
    }
    glutPostRedisplay();
}

// if there is not black tile bellow --> it stopped falling
bool tileStoppedFalling(int r, int c) {
    for(int i = r + 1; r < ROWS; r++) {
        if(grid[i][c] == DELETE_COLOR) {
            return false;
        }
    }
    return true;
}

void eatTileSurroundings(int i, int j) {
    int type = grid[i][j];

    int eats;

    if(type == BLUE || type == RED) {
        return;
    } else if(type == ROCK) {
        eats = SCISSORS;
    } else if(type == PAPER) {
        eats = ROCK;
    } else if(type == SCISSORS) {
        eats = PAPER;
    }

    // eat above
    int k = i - 1;
    if(k >= 0) {
        if(grid[k][j] == eats){
            grid[k][j] = DELETE_COLOR;
        }
    }

    // eat below
    k = i + 1;
    if(k < ROWS) {
        if(grid[k][j] == eats){
            grid[k][j] = DELETE_COLOR;
        }
    }

    // eat left
    k = j - 1;
    if(k >= 0) {
        if(grid[i][k] == eats){
            grid[i][k] = DELETE_COLOR;
        }
    }

    // eat right
    k = j + 1;
    if(k < COLUMNS) {
        if(grid[i][k] == eats){
            grid[i][k] = DELETE_COLOR;
        }
    }
}

void dropTiles(int dummy) {
    bool rerun = false;
    int buf;
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            if(grid[i][j] == DELETE_COLOR) {
                // If top row then create new tile
                if(i == 0) {
                    grid[i][j] = BLUE + (rand() % (int)(SCISSORS - BLUE + 1));
                } else {
                    buf = grid[i][j];
                    grid[i][j] = grid[i - 1][j];
                    grid[i - 1][j] = buf;
                    // When stop falling eat elligible surroundings tiles
                    if(tileStoppedFalling(i, j)) {
                        eatTileSurroundings(i, j);
                    }
                    rerun = true;
                }
            }
        }
    }

    glutPostRedisplay();

    if(rerun) {
        dropTiles(NULL);
    } else { // All tiles dropped. Check again for triads.
        checkForTriad();
    }

    glutPostRedisplay();
}

void deleteTriad() {
    // printf("Deleting Triad\n");
    float totalDelay = 1500;

    // Blink triad effect
    int oldColor = grid[triad[0][0]][triad[0][1]];
    recolorTriad(DELETE_COLOR);
    glutTimerFunc(1.0/5.0 * totalDelay, recolorTriad, oldColor);
    glutTimerFunc(2.0/5.0 * totalDelay, recolorTriad, DELETE_COLOR);
    glutTimerFunc(3.0/5.0 * totalDelay, recolorTriad, oldColor);
    glutTimerFunc(4.0/5.0 * totalDelay, recolorTriad, DELETE_COLOR);
    // glutTimerFunc(1250, recolorTriad, BLACK);
    glutTimerFunc(5.0/5.0 * totalDelay, dropTiles, NULL);
}

// Returns the upper-left and bottom-right tiles of the affected neighborhood
void getNeighborhood(Tile* hood) {
        //top left coordinates
        hood[0].row = max(triad[0][0] - 3, 0);
        hood[0].col = max(triad[0][1] - 3, 0);

        //bottom right coordinates
        hood[1].row = min(triad[2][0] + 3, ROWS - 1);
        hood[1].col = min(triad[2][1] + 3, COLUMNS - 1);
}

int distFromTriad(int r, int c) {
    int dx1 = abs(triad[0][0] - r);
    int dx2 = abs(triad[1][0] - r);
    int dx3 = abs(triad[2][0] - r);
    int dx = min(dx1, min(dx2, dx3));

    int dy1 = abs(triad[0][1] - c);
    int dy2 = abs(triad[1][1] - c);
    int dy3 = abs(triad[2][1] - c);
    int dy = min(dy1, min(dy2, dy3));

    return max(dx, dy);
}

void eatTriadSurroundings() {
    int type = grid[triad[0][0]][triad[0][1]];
    int eats;
    int getsEatenBy;

    if(type == BLUE || type == RED) {
        return;
    } else if(type == ROCK) {
        eats = SCISSORS;
        getsEatenBy = PAPER;
    } else if(type == PAPER) {
        eats = ROCK;
        getsEatenBy = SCISSORS;
    } else if(type == SCISSORS) {
        eats = PAPER;
        getsEatenBy = ROCK;
    }

    int dist;

    Tile hood[2];
    getNeighborhood(hood);

    // printf("Neighborhood:\n");
    // printf("UPPER-LEFT %d %d\n", hood[0].row, hood[0].col);
    // printf("BOTTOM-RIGHT %d %d\n", hood[1].row, hood[1].col);

    for(int i = hood[0].row; i <= hood[1].row; i++){
        for(int j = hood[0].col; j <= hood[1].col; j++) {
            // regions 2 & 3 (yellow & blue)
            dist = distFromTriad(i, j);
            if(dist == 2 || dist == 3)  {
                if(grid[i][j] == eats) {
                    grid[i][j] = DELETE_COLOR;
                    score += 2;
                } else {
                    score -= 1;
                }
            } else if(dist == 1) { //region 1
                if(grid[i][j] != getsEatenBy) {
                    grid[i][j] = DELETE_COLOR;
                    score += 3;
                }
            }
        }
    }
}

// Check first rows and then lines for a match-3. "Paints" the triad to be deleted as white.
void checkForTriad() {
    bool found = false;
    int c = 0;
    int v;

    // First look at rows
    for(int i = 0; i < ROWS ; i++) {
        v = grid[i][0]; // first column of the row
        c = 1;
        for(int j = 1; j < COLUMNS; j++) {

            // if same as previous add to counter
            if(grid[i][j] == v) {
                c++;
            } else { // start again
                c = 1;
                v = grid[i][j];
            }

            // triad found
            if(c == 3) {
                triad[0][0] = i; triad[0][1] = j - 2;
                triad[1][0] = i; triad[1][1] = j - 1;
                triad[2][0] = i; triad[2][1] = j;

                found = true;
                triadOrientation = HORIZONTAL;
            }
        }
    }

    // Then look at columns
    for(int j = 0; j < COLUMNS ; j++) {
        v = grid[0][j]; // first row of the column
        c = 1;
        for(int i = 1; i < ROWS; i++) {

            // if same as previous add to counter
            if(grid[i][j] == v) {
                c++;
            } else { // start again
                c = 1;
                v = grid[i][j];
            }

            // triad found
            if(c == 3) {
                triad[0][0] = i - 2; triad[0][1] = j;
                triad[1][0] = i - 1; triad[1][1] = j;
                triad[2][0] = i; triad[2][1] = j;

                found = true;
                triadOrientation = VERTICAL;
            }
        }
    }

    if(found) {
        // printf("Found triad %d %d - %d %d - %d %d\n",
        // triad[0][0], triad[0][1],
        // triad[1][0], triad[1][1],
        // triad[2][0], triad[2][1] );

        score += 10;
        eatTriadSurroundings();
        deleteTriad();
    }
}

void mouse(GLint button, GLint state, GLint x, GLint y) {
    // printf("MOUSE X: %d,\tY: %d\n", x, y);
    // Don't do anything until the game started
    if(!gameStarted || moves == 0)
        return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(firstClick) {
            if (getTileFromPixel(x, y, &tile1)) {
                firstClick = false;
                glutPostRedisplay();
            }
        } else {
            if (getTileFromPixel(x, y, &tile2)) {
                if(swapTiles()) {
                    checkForTriad();
                    moves -= 1;
                    glutPostRedisplay();
                    // display(); // equivalent ?

                    firstClick = true;
                }
            }
        }
    }
}


void resize(int newWidth, int newHeight) {
    // Keep the window fixed in size
    glutReshapeWindow( winWidth, winHeight);

    // /*  Reset viewport and projection parameters  */
    // glViewport (0, 0, newWidth, newWidth);
    // glMatrixMode (GL_PROJECTION);
    // glLoadIdentity();
    // gluOrtho2D (0.0, COLUMNS, ROWS + 1, 0);

    // TILESIZE = newWidth/winWidth * TILESIZE;

    // /*  Reset display-window size parameters.  */
    // winWidth  = newWidth;
    // winHeight = newHeight;
}

void keyboard(GLubyte key, GLint xMouse, GLint yMouse)
{
    // GLint x = xMouse;
    // printf("xMouse = %d\n", x);
    // GLint y = winHeight - yMouse;
    // printf("yMouse = %d\n", y);
    switch(key)
    {
        case B_KEY:
            if(!gameStarted) {
                printf("Starting Game\n");
                gameStarted = true;
                checkForTriad();
            } else if(moves == 0) {
                printf("Re-starting Game\n");
                moves = initialMoves;
                score = 0;
                initGrid();
                checkForTriad();
            }
            glutPostRedisplay();
            break;
        case ESC_KEY:
            printf("User pressed ESC. Quiting!\n");
            exit(0);
            break;
        default:
            break;
    }

}

void readPGM(string filename, GLubyte* texArray) {
    ifstream file(filename.c_str());

    if (file.is_open()) {
        // Ignoring the first three lines
        string dummyLine;
        getline(file, dummyLine);
        // TODO only ignore if the the first i #
        getline(file, dummyLine);
        getline(file, dummyLine);

        int grayScaleValue;
        for(int k = 0; k < TILESIZE; k++) {
            for (int l = 0; l < TILESIZE; l+=1) {
                file >> grayScaleValue;
                texArray[k * TILESIZE + l]= (GLubyte) grayScaleValue;
            }
         }
    } else {
        cout << "File: " << filename << " could not be opened";
        exit(0);
    }

    file.close();
}

void loadTexture(int texName, GLubyte* texture) {
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_DECAL or GL_MODULATE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TILESIZE, TILESIZE, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture);
}

void initState() {
    firstClick = true;
    gameStarted = false;

    ICON_ROCK = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/rock.pgm", ICON_ROCK);
    loadTexture(ROCK, ICON_ROCK);

    ICON_PAPER = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/paper.pgm", ICON_PAPER);
    loadTexture(PAPER, ICON_PAPER);

    ICON_SCISSORS = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/scissors.pgm", ICON_SCISSORS);
    loadTexture(SCISSORS, ICON_SCISSORS);

    score = 0;
    printf("Please give number of moves:");
    // TODO remove the fixed value
    // scanf("%d", &moves);
    initialMoves = 20;
    moves = initialMoves;
}

int main(int argc,char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition (50, 50);
    glutInitWindowSize (winWidth, winHeight);
    glutCreateWindow ("Kandy Krash");

    initGrid();
    initState();
    initRendering();

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc (keyboard);

    glutMainLoop();
    return 0;
}

