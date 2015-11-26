CC = g++

# GCCFLAGS = -fpcc-struct-return
# CFLAGS = -g
GCCFLAGS = -w

ifeq ($(OS), mac)
INCLUDE = -I/usr/include
LDFLAGS = -L/usr/lib/
LDLIBS = -lm -framework OpenGL -framework GLUT
else
INCLUDE = -I/usr/local/include
LDFLAGS = -L/usr/local/lib/
LDLIBS =  -lGL -lGLU -lglut -lm
endif

PROG=kandykrash

kandy:
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) $(PROG).cpp $(LDFLAGS) $(LDLIBS) -o $(PROG)
default:
	kandy

	// g++  -w -I/usr/include kandykrash.cpp -L/usr/lib/ -framework OpenGL -framework GLUT -o kandykrash
