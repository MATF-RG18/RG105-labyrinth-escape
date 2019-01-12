PROGRAM = Labyrinth
CC = gcc
CFLAGS = -Wall
LDFLAGS = -lGLU -lglut -lGL -lm

$(PROGRAM): main.o image.o
	$(CC) -o $(PROGRAM) main.o image.o $(LDFLAGS)

.PHONY: clean

clean: 
	rm *.o