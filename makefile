CC = g++
OBJ = src/main.cpp src/engine/engine.cpp src/timer/timer.cpp src/shader/shader.cpp src/camera/camera.cpp src/image_loader/image_loader.cpp src/mesh/mesh.cpp
EXE = project
INC = -IC:\\msys64\\ucrt64\\include
LIB = -LC:\\msys64\\ucrt64\\lib
LF = -lopengl32 -lglfw3 -lglew32

.PHONY: all build run

all: build run

build:
	$(CC) $(OBJ) -o $(EXE) $(INC) $(LIB) $(LF)

run:
	./$(EXE)

clean:
	rm $(EXE).exe