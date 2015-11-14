// g++  -I/usr/include kandykrash.cpp -L/usr/lib/ -framework OpenGL -framework GLUT -o kandykrash


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

const int ROWS = 12;
const int COLUMNS = 15;
const int TILESIZE = 40;
const int PADDING = 0;
const int STATUSLINE = 40;

// Types of tiles
const int WHITE = 0;
const int BLUE = 1;
const int RED = 2;
const int ROCK = 3;
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


// DEBUGGING TOOLS

void printGrid(){
	printf("\n");
	for(int i = ROWS - 1; i >= 0 ; i--) {
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
			grid[i][j] = (i + j) % 5 + 1; //BLUE + (rand() % (int)(PAPER - BLUE + 1));
}

void initRendering() {
	glMatrixMode (GL_PROJECTION);
	gluOrtho2D (0.0, COLUMNS, 0.0, ROWS + 1);
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

/*
glutMouseFunc(void (*func)(int button, int state, int x, int y))
mouse function callback
button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
state: GLUT_UP, GLUT_DOWN
x, y: mouse coordinates
*/
void swapTiles(GLint button, GLint state, GLint x, GLint y) {
	printf("\nmouse ");

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (firstClick) {
			printf("firstClick\n");
			firstTile[0] = x / TILESIZE;
			firstTile[1] = (winHeight - y) / TILESIZE;

			firstClick = false;
			// printf("%d %d\n", x, y);
			printf("%d %d", firstTile[0], firstTile[1]);
		} else {
			printf("secondClick\n");
			secondTile[0] = x / TILESIZE;
			secondTile[1] = (winHeight - y) / TILESIZE;

			//TODO check if neighbors
			int buf = grid[firstTile[0]][firstTile[1]];
			grid[firstTile[0]][firstTile[1]] = grid[secondTile[0]][secondTile[1]];
			grid[secondTile[0]][secondTile[1]] = grid[firstTile[0]][firstTile[1]];

			firstClick = true;
			printf("%d %d", secondTile[0], secondTile[1]);

			printGrid();
		}
	}
}

void resize(int newWidth, int newHeight) {
	/*  Reset viewport and projection parameters  */
	glViewport (0, 0, newWidth, newHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0.0, COLUMNS, 0.0, ROWS + 1);

	/*  Reset display-window size parameters.  */
	winWidth  = newWidth;
	winHeight = newHeight;
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
	glutMouseFunc(swapTiles);
	glutKeyboardFunc (keyboard);

	glutMainLoop();
	return 0;
}

