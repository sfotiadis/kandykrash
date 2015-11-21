// a

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

const int ROWS = 2;
const int COLUMNS = 4;
const int COLORS = 3;
const int TILESIZE = 40;
const int PADDING = 0;
const int STATUSLINE = 40;

// Types of tiles
const int WHITE = 0;
const int BLUE = 1;
const int RED = 2;
const int ROCK = 3; // GREEN
const int SCISSORS = 4;
const int PAPER = 5;

// Width: 40 X 15 columns = 600 + padding (16*2) = 632
GLsizei winWidth = COLUMNS * TILESIZE + (COLUMNS + 1) * PADDING;
// Height: 40 X 12 rows = 480 + padding (13*2) + status line (20) = 526
GLsizei winHeight = ROWS * TILESIZE + (ROWS + 1) * PADDING + STATUSLINE;

// State variables
int grid[ROWS][COLUMNS];
bool firstClick;
// The coordinates of the tiles to be swapped
int firstTile[2], secondTile[2];
// Triad to be deleted
int triad[3][2];

// DEBUGGING TOOLS

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
    gluOrtho2D (0.0, COLUMNS, ROWS + 1, 0.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void setColorFromType(int type) {
    switch(type) {
        case WHITE:
            glColor3f(1,1,1);
            break;
        case BLUE:
            glColor3f(0,0,1);
            break;
        case RED:
            glColor3f(1,0,0);
            break;
        case ROCK:
            glColor3f(0,1,0);
            break;
        case SCISSORS:
            glColor3f(1,1,0);
            break;
        case PAPER:
            glColor3f(1,0,1);
            break;
    }
}

void displayQuad(int i, int j) {
    glBegin(GL_QUADS);
        glVertex2f(i, j);
        glVertex2f(i+1, j);
        glVertex2f(i+1, j+1);
        glVertex2f(i, j+1);
    glEnd();
}

void displayGrid() {
    glClear (GL_COLOR_BUFFER_BIT);

    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLUMNS; j++) {
            // TODO change this to displayTile() to add the pgm rendering
            setColorFromType(grid[i][j]);
            displayQuad(j, i);
        }
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity(); //Reset the drawing perspective

    displayGrid();
    //displayStatus();
    glFlush();
}


// Returns true after the second click
bool swapTiles(GLint x, GLint y) {
    if (firstClick) {
        // printf("firstClick\n");
        firstTile[0] = y / TILESIZE;
        firstTile[1] = x / TILESIZE;

        firstClick = false;
        // printf("%d %d\n", x, y);
        printf("Tile %d %d - Color %d\n", firstTile[0], firstTile[1], grid[firstTile[0]][firstTile[1]]);
    } else {
        // printf("secondClick\n");
        secondTile[0] = y / TILESIZE;
        secondTile[1] = x / TILESIZE;

        //TODO check if neighbors
        int buf = grid[firstTile[0]][firstTile[1]];
        grid[firstTile[0]][firstTile[1]] = grid[secondTile[0]][secondTile[1]];
        grid[secondTile[0]][secondTile[1]] = buf;

        firstClick = true;
        printf("Tile %d %d - Color %d\n", secondTile[0], secondTile[1], grid[secondTile[0]][secondTile[1]]);

        printGrid();

        return true;
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

void mouse(GLint button, GLint state, GLint x, GLint y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (swapTiles(x, y)) {
            if(findTriad()) {
                // Blink triad effect
                int oldColor = grid[triad[0][0]][triad[0][1]];
                recolorTriad(WHITE);
                glutTimerFunc(250, recolorTriad, oldColor);
                glutTimerFunc(500, recolorTriad, WHITE);
                glutTimerFunc(750, recolorTriad, oldColor);
                glutTimerFunc(1000, recolorTriad, WHITE);
            }
            displayGrid();
            glFlush();
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

void keyboard(GLubyte curvePlotKey, GLint xMouse, GLint yMouse)
{
    GLint x = xMouse;
    printf("xMouse = %d\n", x);
    GLint y = winHeight - yMouse;
    printf("yMouse = %d\n", y);
}

int main(int argc,char** argv) {
    initGrid();
    firstClick = true;

    glutInit(&argc, argv);
    // glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition (50, 50);
    glutInitWindowSize (winWidth, winHeight);
    glutCreateWindow ("Kandy Krash");

    initRendering();

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc (keyboard);

    glutMainLoop();
    return 0;
}

