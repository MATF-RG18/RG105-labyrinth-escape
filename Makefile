PROGRAM = Labyrinth
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lGLU -lglut -lGL 

$(PROGRAM): main.o
	$(CC) -o $(PROGRAM) main.o $(LDFLAGS)

.PHONY: clean

clean: 
	rm *.o