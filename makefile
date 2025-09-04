CC = g++
OBJ = src/main.cpp
EXE = project
INC = C:\\msys64\\ucrt64\\include
LIB = C:\\msys64\\ucrt64\\lib
LF = -lopengl32 -lglfw3 -lglew32

.PHONY = all build run

all: build run

build:
	$(CC) $(OBJ) -o $(EXE) $(INC) $(LIB) $(LF)

run:
	./$(EXE)

clean:
	rm $(EXE).exe