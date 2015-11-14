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

const int ROWS = 12;
const int COLUMNS = 15;
const int TILESIZE = 40;
const int PADDING = 0;
const int STATUSLINE = 40;

// Types of tiles
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

// Randonly initialize the grid
void initGrid(){
	for(int i = 0; i < ROWS; i++)
		for(int j = 0; j < COLUMNS; j++)
			grid[i][j] = BLUE + (rand() % (int)(PAPER - BLUE + 1));
}

void initRendering() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void setColorFromType(int type) {
	switch(type) {
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
	// glClear (GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode (GL_PROJECTION);
	gluOrtho2D (0.0, COLUMNS, 0.0, ROWS + 1);

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
	//
}

void resize(int width, int height) {
    // Keep the window size constast
    glutReshapeWindow(winWidth, winHeight);
}

int main(int argc,char** argv) {]
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

	glutMainLoop();
	return 0;
}

