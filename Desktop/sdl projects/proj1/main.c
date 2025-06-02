#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_BALLS 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRAVITY 0.5f
#define BOUNCE_DAMPING 0.7f
#define COLLISION_DAMPING 0.9f
#define MIN_RADIUS 10
#define MAX_RADIUS 30

typedef struct {
    float x, y;          // Position
    float vx, vy;        // Velocity
    float radius;        // Radius
    SDL_Color color;     // Color
    int active;          // Is the ball active?
} Ball;

// Initialize a new ball at a random position at the top of the screen
void initBall(Ball* ball, int x) {
    ball->x = x;
    ball->y = 50;  // Start near the top
    ball->vx = (rand() % 10) - 5;  // Random horizontal velocity
    ball->vy = 0;
    ball->radius = MIN_RADIUS + (rand() % (MAX_RADIUS - MIN_RADIUS));
    ball->color.r = rand() % 256;
    ball->color.g = rand() % 256;
    ball->color.b = rand() % 256;
    ball->color.a = 255;
    ball->active = 1;
}

// Check for collision between two balls
int checkBallCollision(Ball* a, Ball* b) {
    if (!a->active || !b->active) return 0;
    
    // Calculate distance between centers
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Check if balls are overlapping
    return distance < (a->radius + b->radius);
}

// Resolve collision between two balls
void resolveBallCollision(Ball* a, Ball* b) {
    // Calculate vector between centers
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Calculate normalized collision vector
    float nx = dx / distance;
    float ny = dy / distance;
    
    // Calculate relative velocity
    float dvx = b->vx - a->vx;
    float dvy = b->vy - a->vy;
    
    // Calculate velocity along the collision normal
    float velocityAlongNormal = dvx * nx + dvy * ny;
    
    // Don't resolve if balls are moving away from each other
    if (velocityAlongNormal > 0) return;
    
    // Calculate impulse scalar
    float restitution = COLLISION_DAMPING;
    float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    impulseScalar /= 1/a->radius + 1/b->radius;
    
    // Apply impulse
    float impulseX = impulseScalar * nx;
    float impulseY = impulseScalar * ny;
    
    // Update velocities
    a->vx -= impulseX / a->radius;
    a->vy -= impulseY / a->radius;
    b->vx += impulseX / b->radius;
    b->vy += impulseY / b->radius;
    
    // Prevent balls from sticking together by moving them apart
    float overlap = (a->radius + b->radius) - distance;
    float moveX = nx * overlap * 0.5f;
    float moveY = ny * overlap * 0.5f;
    
    a->x -= moveX;
    a->y -= moveY;
    b->x += moveX;
    b->y += moveY;
}

// Update ball physics
void updateBall(Ball* ball) {
    if (!ball->active) return;
    
    // Apply gravity
    ball->vy += GRAVITY;
    
    // Update position
    ball->x += ball->vx;
    ball->y += ball->vy;
    
    // Boundary collision - bottom
    if (ball->y + ball->radius > WINDOW_HEIGHT) {
        ball->y = WINDOW_HEIGHT - ball->radius;
        ball->vy = -ball->vy * BOUNCE_DAMPING;
        
        // Add some friction when hitting the ground
        ball->vx *= 0.95f;
    }
    
    // Boundary collision - left and right
    if (ball->x - ball->radius < 0) {
        ball->x = ball->radius;
        ball->vx = -ball->vx * BOUNCE_DAMPING;
    } else if (ball->x + ball->radius > WINDOW_WIDTH) {
        ball->x = WINDOW_WIDTH - ball->radius;
        ball->vx = -ball->vx * BOUNCE_DAMPING;
    }
}

// Draw a ball
void drawBall(SDL_Renderer* renderer, const Ball* ball) {
    if (!ball->active) return;
    
    // Set the color
    SDL_SetRenderDrawColor(renderer, ball->color.r, ball->color.g, ball->color.b, ball->color.a);
    
    // Draw a filled circle
    for (int w = 0; w < ball->radius * 2; w++) {
        for (int h = 0; h < ball->radius * 2; h++) {
            int dx = ball->radius - w;
            int dy = ball->radius - h;
            if ((dx*dx + dy*dy) <= (ball->radius * ball->radius)) {
                SDL_RenderDrawPoint(renderer, ball->x + dx, ball->y + dy);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Gravity Balls",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create balls array
    Ball balls[MAX_BALLS];
    int ballCount = 0;
    
    // Initialize all balls as inactive
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = 0;
    }

    int running = 1;
    SDL_Event event;

    // Main loop
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case SDLK_SPACE:
                        // Spawn a new ball if we haven't reached the limit
                        if (ballCount < MAX_BALLS) {
                            int mouseX, mouseY;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            initBall(&balls[ballCount], mouseX);
                            ballCount++;
                        }
                        break;
                }
            }
        }

        // Update ball physics
        for (int i = 0; i < ballCount; i++) {
            updateBall(&balls[i]);
        }
        
        // Check for collisions between all pairs of balls
        for (int i = 0; i < ballCount; i++) {
            for (int j = i + 1; j < ballCount; j++) {
                if (checkBallCollision(&balls[i], &balls[j])) {
                    resolveBallCollision(&balls[i], &balls[j]);
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Draw all balls
        for (int i = 0; i < ballCount; i++) {
            drawBall(renderer, &balls[i]);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Cap frame rate
        SDL_Delay(16); // Approx 60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
