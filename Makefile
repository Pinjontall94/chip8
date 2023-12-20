INCLUDES= -I ./include
LIBS= -lSDL2 -lSDL2main
FLAGS= -g -Wall -Wextra -std=c2x -pedantic

all:
	$(CC) ${FLAGS} ${INCLUDES} ./src/main.c ${LIBS} -o ./bin/main

run:
	./bin/main
