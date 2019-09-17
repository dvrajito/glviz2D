EXEC	    = roadviz
CC          = gcc
CLINKER     = gcc
CCC         = g++
CCLINKER    = $(CCC)
INCLUDE_DIR = -I/usr/lib/glib/include -I/usr/lib/gnome-libs/include
LIB_LIST    = -lGL -lglut -lGLU
CFLAGS  = $(INCLUDE_DIR)
CCFLAGS = $(CFLAGS)
OPTFLAGS    = -g
LIB_PATH    =
LIBS = $(LIB_PATH) $(LIB_LIST)

objects = main.o road.o point3f.o gl_draw.o interface.o

default: $(EXEC)

$(EXEC): $(objects)
	$(CCLINKER) $(OPTFLAGS) -o $(EXEC) $(objects) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -w -c $*.c
.cc.o:
	$(CCC) $(CFLAGS) -w -c $*.cc

clean:
	rm $(EXEC)
	rm *.o
