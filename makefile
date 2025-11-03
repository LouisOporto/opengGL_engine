# Not functional with assimp (need to use CMAKE)
# CC = g++
# OBJ = src/main.cpp src/engine/engine.cpp src/timer/timer.cpp src/shader/shader.cpp src/camera/camera.cpp src/image_loader/image_loader.cpp src/mesh/mesh.cpp
# EXE = project
# INC = -IC:\\msys64\\ucrt64\\include -Iinclude
# LIB = -LC:\\msys64\\ucrt64\\lib
# LF = -lopengl32 -lglfw3 -lglew32

# .PHONY: all build run

# all: build run

# build:
# 	$(CC) $(OBJ) -o $(EXE) $(INC) $(LIB) $(LF)

# run:
# 	./$(EXE)

# clean:
# 	rm $(EXE).exe


.PHONY: all cmake_build build

all: cmake_build build run

cmake_build:
	cmake -S . -B build -G Ninja

build:
	cmake --build ./build

run: 
	./build/opengl_engine

remove_app:
	rm ./build/opengl_engine

clean:
	rm -rf ./build
