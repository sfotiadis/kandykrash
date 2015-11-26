CC = g++

# GCCFLAGS = -fpcc-struct-return
# CFLAGS = -g
GCCFLAGS = -w

ifeq ($(OSTYPE),solaris)
INCLUDE = -I/usr/local/include
LDFLAGS = -L/usr/local/lib/
LDLIBS =  -lGL -lglut -lm
else
INCLUDE = -I/usr/include
LDFLAGS = -L/usr/lib/
LDLIBS = -lm -framework OpenGL -framework GLUT
endif

PROG=kandykrash

kandy:
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) $(PROG).cpp $(LDFLAGS) $(LDLIBS) -o $(PROG)
default:
	kandy

	// g++  -w -I/usr/include kandykrash.cpp -L/usr/lib/ -framework OpenGL -framework GLUT -o kandykrash
