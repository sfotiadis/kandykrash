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

const int ROWS = 12;
const int COLUMNS = 15;
const int TILESIZE = 40;
const int PADDING = 2;
const int STATUSLINE = 20;

// Width: 40 X 15 columns = 600 + padding (16*2) = 632
GLsizei winWidth = COLUMNS * TILESIZE + (COLUMNS + 1) * PADDING;
// Height: 40 X 12 rows = 480 + padding (13*2) + status line (20) = 526
GLsizei winHeight = ROWS * TILESIZE + (ROWS + 1) * PADDING + STATUSLINE;

void initRendering() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void display() {
   glFlush();
}

int main(int argc,char** argv) {
	glutInit(&argc, argv);
	// glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition (50, 50);
	glutInitWindowSize (winWidth, winHeight);
	glutCreateWindow ("Kandy Krash");
	initRendering();
   	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}