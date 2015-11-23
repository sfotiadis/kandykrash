// Fotiadis Efstathios
// AM 2009

// g++  -w -I/usr/include kandykrash.cpp -L/usr/lib/ -framework OpenGL -framework GLUT -o kandykrash

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

using namespace std;

const int ROWS = 8;
const int COLUMNS = 10;
const int COLORS = 5;
const int TILESIZE = 40;
const int PADDING = 0;
const int STATUSLINE = 40;

// Types of tiles
const int BLACK = 10;
const int WHITE = 0;
const int BLUE = 1;
const int RED = 2;
const GLuint ROCK = 3; // GREEN
const GLuint PAPER = 4;
const GLuint SCISSORS = 5;

// KEYS
const int ESC_KEY = 27;
const int B_KEY = 'b';

// Width: 40 X 15 columns = 600 + padding (16*2) = 632
GLsizei winWidth = COLUMNS * TILESIZE + (COLUMNS + 1) * PADDING;
// Height: 40 X 12 rows = 480 + padding (13*2) + status line (20) = 526
GLsizei winHeight = ROWS * TILESIZE + (ROWS + 1) * PADDING + STATUSLINE;

// State variables
bool gameStarted;
bool newGame;
int grid[ROWS][COLUMNS];
bool firstClick;
// A tile structure, to hold the selected tiles coordinates
struct Tile {
    int row;
    int col;
};
struct Tile tile1, tile2;
// Triad to be deleted
int triad[3][2];

// Icons
// GLubyte* ICON_ROCK;
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
            grid[i][j] = (i + j) % COLORS + 1; //BLUE + (rand() % (int)(PAPER - BLUE + 1));
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

    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLUMNS; j++) {
            displayTile(j, i); //
        }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity(); //Reset the drawing perspective

    if(gameStarted) {
        displayGrid();
        //TODO update score
        //displayStatus();
    }
    glutSwapBuffers();
}

bool isTileWithinBounds(int row, int col) {
    return (row >=0 && row < ROWS) && (col >=0 && col < COLUMNS);
}

// Returns true if the tile was correctly returned
bool getTileFromPixel(int x, int y, Tile* tile) {
    // TODO Does this need to be fixed with glViewPort?
    tile->row = y / TILESIZE; // Mouse reads from bottom-left
    tile->col = x / TILESIZE; // We are using top-left reference.
    if(isTileWithinBounds(tile->row, tile->col)) {
        printf("Selected Tile %d %d - Color %d\n", tile->row, tile->col, grid[tile->row][tile->col]);
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

        printGrid();
        return true;
    } else {
        printf("Not valid swap, select another tile\n");
    }

}


// Checkc first rows and then lines for a match-3. "Paints" the triad to be deleted as white.
bool findTriad() {
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

                return true;
            }
        }
    }

    return false;
}

void recolorTriad(int color) {
    for(int i = 0; i <3; i++) {
            grid[triad[i][0]][triad[i][1]] = color;
    }
    glutPostRedisplay();
}

void blinkTriad() {
    // Blink triad effect
    int oldColor = grid[triad[0][0]][triad[0][1]];
    recolorTriad(WHITE);
    glutTimerFunc(250, recolorTriad, oldColor);
    glutTimerFunc(500, recolorTriad, WHITE);
    glutTimerFunc(750, recolorTriad, oldColor);
    glutTimerFunc(1000, recolorTriad, WHITE);
    glutTimerFunc(1250, recolorTriad, BLACK);
}

void eatTiles() {

}

void mouse(GLint button, GLint state, GLint x, GLint y) {
    printf("MOUSE X: %d,\tY: %d\n", x, y);
    // Don't do anything until the game started
    if(!gameStarted)
        return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(firstClick) {
            if (getTileFromPixel(x, y, &tile1)) {
                printf("GOT 1ST TILE\n");
                firstClick = false;
            }
        } else {
            if (getTileFromPixel(x, y, &tile2)) {
                printf("GOT 2ND TILE\n");
                if(swapTiles()) {
                    if(findTriad()) {
                        blinkTriad();
                        eatTiles();
                    }

                    display();

                    firstClick = true;
                }
            }
        }
    }
}


void resize(int newWidth, int newHeight) {
    /*  Reset viewport and projection parameters  */
    // glViewport (0, 0, newWidth, newHeight);
    // glMatrixMode (GL_PROJECTION);
    // glLoadIdentity();
    // gluOrtho2D (0.0, COLUMNS, 0.0, ROWS + 1);

    /*  Reset display-window size parameters.  */
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
            printf("Starting Game\n");
            if(!gameStarted) {
                gameStarted = true;
                display();
            }
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
    ifstream file(filename);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TILESIZE, TILESIZE, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture);
}

void initState() {
    firstClick = true;
    gameStarted = false;
    newGame = false;

    ICON_ROCK = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/rock.pgm", ICON_ROCK);
    loadTexture(ROCK, ICON_ROCK);

    ICON_PAPER = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/paper.pgm", ICON_PAPER);
    loadTexture(PAPER, ICON_PAPER);

    ICON_SCISSORS = new GLubyte[TILESIZE * TILESIZE];
    readPGM("assets/scissors.pgm", ICON_SCISSORS);
    loadTexture(SCISSORS, ICON_SCISSORS);
}

int main(int argc,char** argv) {
    initGrid();
    // TODO Check gia pi8anes triades

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition (50, 50);
    glutInitWindowSize (winWidth, winHeight);
    glutCreateWindow ("Kandy Krash");

    initState();
    initRendering();

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc (keyboard);

    glutMainLoop();
    return 0;
}

