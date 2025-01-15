you gotta have SDL2 installed to compile it
also you need to link the math library

gcc -o bouncyBalls bouncyBalls.c `sdl2-config --cflags --libs` -lm

its balls, they bounce, collision doesnt work that well tho
