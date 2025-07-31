#!/bin/bash

SRC="iMain.cpp"
OUT_WIN="game.exe"
OUT_UNIX="game"

MINGW_COMPILER="x86_64-w64-mingw32-g++"
NATIVE_COMPILER="g++"

SDL2_CFLAGS=$(sdl2-config --cflags 2>/dev/null)
SDL2_LIBS=$(sdl2-config --libs 2>/dev/null)
SDL2_MIXER_LIBS="-lSDL2_mixer"
OPENGL_LIBS="-lGL -lGLU -lglut"

MINGW_SDL2_CFLAGS="-I/usr/x86_64-w64-mingw32/include/SDL2"
MINGW_SDL2_LIBS="-L/usr/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -mwindows"

function check_command() {
    command -v "$1" >/dev/null 2>&1 || { echo >&2 "Error: $1 is not installed or not in PATH."; exit 1; }
}

function compile_windows() {
    check_command "$MINGW_COMPILER"
    echo "Compiling for Windows..."
    $MINGW_COMPILER $SRC -o $OUT_WIN $MINGW_SDL2_CFLAGS $MINGW_SDL2_LIBS -static-libgcc -static-libstdc++ -std=c++11
    if [ $? -eq 0 ]; then
        echo "Windows executable created: $OUT_WIN"
    else
        echo "Windows compilation failed."
        exit 1
    fi
}

function compile_unix() {
    check_command "$NATIVE_COMPILER"
    if [ -z "$SDL2_CFLAGS" ] || [ -z "$SDL2_LIBS" ]; then
        echo "Error: SDL2 development files not found. Please install SDL2 development packages."
        exit 1
    fi
    echo "Compiling for Unix..."
    $NATIVE_COMPILER $SRC -o $OUT_UNIX $SDL2_CFLAGS $SDL2_LIBS $SDL2_MIXER_LIBS $OPENGL_LIBS -std=c++11
    if [ $? -eq 0 ]; then
        echo "Unix executable created: $OUT_UNIX"
    else
        echo "Unix compilation failed."
        exit 1
    fi
}

if [ "$1" == "win" ]; then
    compile_windows
elif [ "$1" == "unix" ]; then
    compile_unix
else
    echo "Usage: $0 [win|unix]"
    echo "  win  - compile Windows .exe using mingw-w64"
    echo "  unix - compile native Unix executable"
    exit 1
fi