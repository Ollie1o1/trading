// Simple 2D SDL2 Ray Visualization – circle “sun” casting rays onto a square
// ---------------------------------------------------------------
//  * 2D scene: a movable circle (sun) and a static axis-aligned square.
//  * Casts random rays in 360° from the circle’s center.
//    Rays hitting the square are drawn opaque and bold; rays missing are faint.
//  * Move the circle with WASD keys.

//  Build on macOS/Homebrew (M-series):
//      brew install sdl2
//      gcc ray2d_sdl2.c -std=c17 \
//          -I/opt/homebrew/include/SDL2 \
//          -L/opt/homebrew/lib -lSDL2 -lm -o ray2d

//  Run: ./ray2d
//  Controls: W/A/S/D = move circle; Esc = quit.
// ---------------------------------------------------------------

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define W 800
#define H 600
#define MAX_RAYS 200
#define CIRCLE_RADIUS 30
#define MOVE_SPEED 5

// 2D vector
typedef struct { float x, y; } Vec2;
// Axis-aligned rectangle
typedef struct { float x, y, w, h; } Rect;

// Cast a ray from origin in direction dir. Returns true if it hits rect, and t at intersection.
bool intersect_rect(Vec2 origin, Vec2 dir, Rect r, float *t_out) {
    float tmin = INFINITY;
    // Vertical sides
    if (fabsf(dir.x) > 1e-6f) {
        float tx1 = (r.x - origin.x) / dir.x;
        float ty1 = origin.y + tx1 * dir.y;
        if (tx1 > 0 && ty1 >= r.y && ty1 <= r.y + r.h) {
            if (tx1 < tmin) tmin = tx1;
        }
        float tx2 = (r.x + r.w - origin.x) / dir.x;
        float ty2 = origin.y + tx2 * dir.y;
        if (tx2 > 0 && ty2 >= r.y && ty2 <= r.y + r.h) {
            if (tx2 < tmin) tmin = tx2;
        }
    }
    // Horizontal sides
    if (fabsf(dir.y) > 1e-6f) {
        float ty1 = (r.y - origin.y) / dir.y;
        float tx1 = origin.x + ty1 * dir.x;
        if (ty1 > 0 && tx1 >= r.x && tx1 <= r.x + r.w) {
            if (ty1 < tmin) tmin = ty1;
        }
        float ty2 = (r.y + r.h - origin.y) / dir.y;
        float tx2 = origin.x + ty2 * dir.x;
        if (ty2 > 0 && tx2 >= r.x && tx2 <= r.x + r.w) {
            if (ty2 < tmin) tmin = ty2;
        }
    }
    if (tmin < INFINITY) {
        *t_out = tmin;
        return true;
    }
    return false;
}

// Draw a filled circle using SDL_RenderDrawPoint
void draw_filled_circle(SDL_Renderer *ren, int cx, int cy, int radius) {
    for (int dx = -radius; dx <= radius; dx++) {
        int h = (int)(sqrtf(radius*radius - dx*dx));
        for (int dy = -h; dy <= h; dy++) {
            SDL_RenderDrawPoint(ren, cx + dx, cy + dy);
        }
    }
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("2D Ray Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    bool running = true;
    SDL_Event e;

    // Circle (sun) initial position
    Vec2 circle = { W/4.0f, H/2.0f };
    // Rectangle (cube) fixed at right side
    Rect rect = { W*0.6f, H*0.3f, W*0.2f, H*0.4f };
    // Diagonal length for ray misses
    float diag = sqrtf(W*W + H*H);
    // Seed random
    srand((unsigned)SDL_GetTicks());

    while (running) {
        // Input handling
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: circle.y -= MOVE_SPEED; break;
                    case SDLK_s: circle.y += MOVE_SPEED; break;
                    case SDLK_a: circle.x -= MOVE_SPEED; break;
                    case SDLK_d: circle.x += MOVE_SPEED; break;
                    default: break;
                }
            }
        }
        // Clamp circle
        if (circle.x < CIRCLE_RADIUS) circle.x = CIRCLE_RADIUS;
        if (circle.x > W - CIRCLE_RADIUS) circle.x = W - CIRCLE_RADIUS;
        if (circle.y < CIRCLE_RADIUS) circle.y = CIRCLE_RADIUS;
        if (circle.y > H - CIRCLE_RADIUS) circle.y = H - CIRCLE_RADIUS;

        // Clear screen (dark)
        SDL_SetRenderDrawColor(ren, 10, 10, 30, 255);
        SDL_RenderClear(ren);

        // Draw rectangle (cube) in grey
        SDL_Rect sdlRect = { (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h };
        SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
        SDL_RenderFillRect(ren, &sdlRect);

        // Cast and draw rays
        for (int i = 0; i < MAX_RAYS; i++) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
            Vec2 dir = { cosf(angle), sinf(angle) };
            Vec2 origin = circle;
            float t_hit;
            bool hit = intersect_rect(origin, dir, rect, &t_hit);
            Vec2 end;
            if (hit) {
                end = (Vec2){ origin.x + dir.x * t_hit, origin.y + dir.y * t_hit };
                // Draw opaque thick ray
                SDL_SetRenderDrawColor(ren, 255, 230, 0, 255);
                SDL_RenderDrawLine(ren, (int)origin.x, (int)origin.y, (int)end.x, (int)end.y);
                SDL_RenderDrawLine(ren, (int)origin.x+1, (int)origin.y, (int)end.x+1, (int)end.y);
                // Draw hit marker
                for (int dx = -4; dx <= 4; dx++) {
                    for (int dy = -4; dy <= 4; dy++) {
                        if (dx*dx + dy*dy <= 16) {
                            SDL_RenderDrawPoint(ren, (int)end.x + dx, (int)end.y + dy);
                        }
                    }
                }
            } else {
                // Miss: extend to diagonal length
                end = (Vec2){ origin.x + dir.x * diag, origin.y + dir.y * diag };
                // Draw faint thin ray
                SDL_SetRenderDrawColor(ren, 255, 230, 0, 80);
                SDL_RenderDrawLine(ren, (int)origin.x, (int)origin.y, (int)end.x, (int)end.y);
            }
        }

        // Draw circle (sun) in bright yellow
        SDL_SetRenderDrawColor(ren, 255, 240, 100, 255);
        draw_filled_circle(ren, (int)circle.x, (int)circle.y, CIRCLE_RADIUS);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
