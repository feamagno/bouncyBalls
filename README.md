you gotta have SDL2 installed to compile it
also you need to link the math library
gcc -o bouncyBalls bouncyBalls.c `sdl2-config --cflags --libs` -lm
