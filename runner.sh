#!/bin/bash
set -e

SOURCE_FILE="${1:-iMain.cpp}"
echo "Using source file: $SOURCE_FILE"

mkdir -p bin obj

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Install required packages
    sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev freeglut3-dev
    
    # Compile with SDL_mixer
    g++ -w -fexceptions -g \
        -I. \
        -I./MINGW/OpenGL/include \
        -I./MINGW/OpenGL/include/SDL2 \
        "$SOURCE_FILE" \
        -o bin/opengl \
        -lGL -lGLU -lglut -lSDL2 -lSDL2_mixer
    
    echo "Running program..."
    ./bin/opengl
else
    # Windows build
    g++ -w -fexceptions -g \
        -I. \
        -IOpenGL/include \
        -IOpenGL/include/SDL2 \
        "$SOURCE_FILE" \
        -o bin/opengl.exe \
        -static-libgcc \
        -static-libstdc++ \
        -lmingw32 \
        -lSDL2main \
        -lSDL2 \
        -lSDL2_mixer \
        -lOpenGL32 \
        -lfreeglut
    ./bin/opengl.exe
fi