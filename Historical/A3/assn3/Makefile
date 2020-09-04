OBJECT_FILES=main.o XYZ.o

CFLAGS=-g -Wall
LIBS=-lGL -lGLU -lglut -lm

all: $(OBJECT_FILES)
	gcc $(CFLAGS) -o volumeRenderer $(OBJECT_FILES) $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $<

vector.c : XYZ.h
main.c : XYZ.h

clean:
	rm -f *.o
	rm -f volumeRenderer
