#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 920
#define HEIGHT 480
#define WHITE 0xffffffff
#define GRAY  0X52525252
#define NUM_OF_CIRCLES 10
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define GRAVITY 0.4
#define DEMPENING 0.8
#define FRICTION 0.5

void initializeWindow(SDL_Window** window, SDL_Surface** surface){
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        exit -1;
    }

    // Create SDL Window
    *window = SDL_CreateWindow(
        "Window Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN
    );

    if (!*window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        exit -1;
    }

    *surface = SDL_GetWindowSurface(*window);
}

struct Circle{
    double x;
    double y;
    double radius;
    Uint32 color;
    double x_speed;
    double y_speed;
};

void drawCircle(struct Circle circle, SDL_Surface* surface){
    double sqrdRadius = pow(circle.radius, 2);
    double xDist, yDist;
    for (int i = 0; i <= circle.radius; i++){
        int j = 0;
        int outOfBoundaries = 0;
        while (j <= circle.radius && !outOfBoundaries){
            xDist = pow(j,2);
            yDist = pow(i,2);
            outOfBoundaries = (xDist + yDist > sqrdRadius);
            j++;
        }

        SDL_Rect rect = (SDL_Rect) {circle.x-j,circle.y+i,j*2,1};
        SDL_FillRect(surface,&rect,circle.color);
    
        rect = (SDL_Rect) {circle.x-j,circle.y-i,j*2,1};
        SDL_FillRect(surface,&rect,circle.color);
    }
}

void addCircle(struct Circle circles[NUM_OF_CIRCLES], int* circles_count, 
        double x, double y, double radius, Uint32 color, double x_speed, double y_speed){
    if (*circles_count >= NUM_OF_CIRCLES) return;

    x = MAX(x, radius);
    x = MIN(x, WIDTH - radius);
    y = MAX(y, radius);
    y = MIN(y, HEIGHT - radius);
    struct Circle circle;
    circle.x = x;
    circle.y = y;
    circle.radius = radius;
    circle.color = color;
    circle.x_speed = x_speed;
    circle.y_speed = y_speed;
    circles[*circles_count] = circle;
    *circles_count = *circles_count + 1;
}

void drawCircles(struct Circle circles[], int circles_count, 
                SDL_Surface* surface){
    for(int i = 0; i < circles_count; i++)
        drawCircle(circles[i],surface);
}

int yOutOfBoundaries(struct Circle* circle, double y){
    double radius = circle->radius;
    if (y - radius < 0){
        circle->y = radius;
        circle->y_speed *= -1*DEMPENING;// -circle->y_speed;
        return 1;
    } else if (y + radius > HEIGHT){
        circle->y = HEIGHT - radius;
        circle->y_speed *= -1*DEMPENING;// -circle->y_speed;
        return 1;
    }
    return 0;
}

int xOutOfBoundaries(struct Circle* circle, double x){
    double radius = circle->radius;
    if (x - radius < 0) {
        circle->x = radius;
        circle->x_speed *= -1*DEMPENING;//circle->x_speed;
        return 1;
    } else if (x + radius > WIDTH){
        circle->x = WIDTH - radius;
        circle->x_speed *= -1*DEMPENING;//-circle->x_speed; 
        return 1;
    }
    return 0;
}

void checkCollision(struct Circle circles[], int circles_count, int lastX, int lastY, int self){
    double xSum, ySum, minDistance, sin, cos, y, x, hip;
    struct Circle* this = &circles[self];
    struct Circle* other;
    for (int i = 0; i < circles_count; i++){
        if (i != self){
            other = &circles[i];
            xSum = pow(this->x - other->x, 2); //tirei o abs do - 
            ySum = pow(this->y - other->y, 2);
            minDistance = pow(this->radius + other->radius, 2);
            if (minDistance > xSum + ySum){
                printf("ball %f hit ball %f\n", this->radius, other->radius);
                hip = sqrt((pow(lastX - other->x, 2)) + (pow(lastY - other->y, 2)));
                cos = (other->x - lastX) / hip;       //checar ordem
                sin = ((other->y - lastY) / hip);   //checar ordem e - 
                printf("cos %f, sin %f\n", cos, sin);
                this->x = other->x + (cos * (this->radius + other->radius));
                this->y = other->y + (sin * (this->radius + other->radius));
                this->x_speed = -1 * this->x_speed;
                this->y_speed = -1 * this->y_speed;
                printf("new x %f, new y %f\n", this->x, this->y);
            }
        }
    }
}

void checkAllCollisions(struct Circle circles[], int circles_count){
    double xSum, ySum, minDistance, sin, cos, y, x;
    struct Circle* other;
    for(int self = 0; self < circles_count; self++){
        struct Circle* this = &circles[self];
        for (int i = 0; i < circles_count; i++){
            if (i != self){
                other = &circles[i];
                xSum = pow(abs(this->x - other->x), 2);
                ySum = pow(abs(this->y - other->y), 2);
                minDistance = pow(this->radius + other->radius, 2);
                if (minDistance > xSum + ySum){
                    // printf("i: %d bf: x: %f y: %f\n",i, this->x, this->y);
                    // SDL_Delay(2000);
                    // double overlapping = sqrt(minDistance) - sqrt(xSum + ySum);
                    // if (overlapping != 0){
                    //     double current_distance = sqrt(xSum + ySum);
                    //     sin = current_distance / (other->y - this->y);
                    //     cos = current_distance / (other->x - this->x);
                    //     y = this->y + (overlapping * sin * -1);
                    //     x = this->x + (overlapping * cos);
                    //     if (!xOutOfBoundaries(this, x))
                    //         this->x = x;
                    //     if (!yOutOfBoundaries(this, y))
                    //         this->y = y;
                    // }
                    this->x_speed *= -1;
                    this->y_speed *= -1;
                    // printf("i: %d af: x: %f y: %f\n",i, this->x, this->y);
                    // SDL_Delay(2000);
                }
            }
        }
    }
}



void step(struct Circle circles[], int circles_count){
    for(int i = 0; i < circles_count; i++){
        struct Circle* circle = &(circles[i]);
        double lastX = circle->x; 
        double lastY = circle->y;
        double x = lastX + circle->x_speed;
        double y = lastY + circle->y_speed;
        double radius = circle->radius;
        if (!xOutOfBoundaries(circle, x))
            circle->x = x;
        if (!yOutOfBoundaries(circle, y)){
            circle->y = y;
            double y_speed = circle->y_speed + GRAVITY;
            if (y_speed * circle->y_speed < 0)
                circle->y_speed = 0;
            else
                circle->y_speed = y_speed;
        }
        if (abs(circle->y_speed) < circle->radius*0.01 && (double)((double)HEIGHT - circle->y) < circle->radius * 1.04){
            circle->y_speed = 0;
            circle->y = HEIGHT-circle->radius;
        }
    }
    checkAllCollisions(circles, circles_count);
}

void cleanScreen(SDL_Surface* surface, SDL_Rect blackScreen, Uint32 color){
    SDL_FillRect(surface,&blackScreen,color);
}

int main() {

    SDL_Window* window;
    SDL_Surface* surface;
    initializeWindow(&window, &surface);

    SDL_Rect blackScreen = (SDL_Rect) {0,0,WIDTH,HEIGHT};
    struct Circle circles[NUM_OF_CIRCLES];
    int circles_count = 0;

    addCircle(circles, &circles_count, WIDTH/2, HEIGHT/2, 30, WHITE, 50, 50);
    addCircle(circles, &circles_count, 0, 0, 50, WHITE, 50, 50);

    SDL_Event event;
    int simulation_is_running = 1;
    while (simulation_is_running) {
        while (SDL_PollEvent(&event)) {  
            switch (event.type) {
                case SDL_QUIT: 
                    simulation_is_running = 0;
                    break;
            }
        }
        cleanScreen(surface, blackScreen, GRAY);
        step(circles, circles_count);
        drawCircles(circles, circles_count,surface);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/*

void checkCollision2(struct Circle circles[], int circles_count, int self){
    double xSum, ySum, minDistance, sin, cos, y, x;
    struct Circle* other;
    struct Circle* this = &circles[self];
    for (int i = 0; i < circles_count; i++){
        if (i != self){
            other = &circles[i];
            xSum = pow(abs(this->x - other->x), 2);
            ySum = pow(abs(this->y - other->y), 2);
            minDistance = pow(this->radius + other->radius, 2);
            if (minDistance > xSum + ySum){
                printf("i: %d bf: x: %f y: %f\n",i, this->x, this->y);
                // SDL_Delay(2000);
                double overlapping = sqrt(minDistance) - sqrt(xSum + ySum);
                if (overlapping != 0){
                    double current_distance = sqrt(xSum + ySum);
                    sin = current_distance / (other->y - this->y);
                    cos = current_distance / (other->x - this->x);
                    y = this->y + (overlapping * sin * -1);
                    x = this->x + (overlapping * cos);
                    if (!xOutOfBoundaries(this, x))
                        this->x = x;
                    if (!yOutOfBoundaries(this, y))
                        this->y = y;
                }
                this->x_speed *= -1;
                this->y_speed *= -1;
                printf("i: %d af: x: %f y: %f\n",i, this->x, this->y);
                // SDL_Delay(2000);
            }
        }
    }
}

*/
