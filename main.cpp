#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <random>

using namespace std;

#include "constants.h"

SDL_Window* window = NULL;
SDL_Texture* agenttex = NULL;
SDL_Renderer* renderer = NULL;
struct Agent {
    SDL_Rect* rect = new SDL_Rect;
    double dir = 0;
    double speed = MOVE_SPEED;
    Uint32 starttick;
    pair<double, double> realpos;
    SDL_Texture* tex = agenttex;
    Agent(int x, int y, double d) {
        realpos.first = x;
        realpos.second = y;
        rect->x = realpos.first;
        rect->y = realpos.second;
        rect->h = AGENT_SIZE;
        rect->w = AGENT_SIZE;
        dir = d - 90;
        starttick = SDL_GetTicks();
    };
    int getDist(int x1, int y1, int x2, int y2) {
        int xdist = abs(x2 - x1);
        int ydist = abs(y2 - y1);
        return pow(xdist, 2) + pow(ydist, 2);
    }
    int getTurn(vector<Agent*> boid) {
        double align_amt = 1, avoid_amt = 0, cohere_amt = 1;
        double align = dir;
        pair<double, double> avoid = {0, 0};
        double avoid_angle = 0, cohere_angle = 0;
        pair<double, double> cohere = {realpos.first, realpos.second};
        pair<double, double> obstacle = {0, 0};
        double obstacle_angle = 0;
        for (Agent* b : boid) {
            if (b == this) continue;
            int dist = getDist(realpos.first, realpos.second, b->realpos.first, b->realpos.second);
            if (dist <= pow(ALIGNMENT_RADIUS, 2)) {
                align_amt ++;
                align += b->dir;
            }
            if (dist <= pow(AVOID_RADIUS, 2)) {
                avoid.first += (AVOID_RADIUS - (b->realpos.first - realpos.first)) * -1;
                avoid.second += (AVOID_RADIUS - (b->realpos.second - realpos.second)) * -1;
                avoid_amt ++;
            }
            if (dist <= pow(COHERENCE_RADIUS, 2)) {
                cohere.first += (b->realpos.first);
                cohere.second += (b->realpos.second);
                cohere_amt ++;
            }
        }
        for (int x = 0; x <= SDL_WINDOW_SIZE; x += SDL_WINDOW_SIZE) {
            if (abs(realpos.first - x) <= OBSTACLE_RADIUS) {
                obstacle.first += (OBSTACLE_RADIUS - (realpos.first - x)) * -1;
            }
        }
        for (int y = 0; y <= SDL_WINDOW_SIZE; y += SDL_WINDOW_SIZE) {
            if (abs(realpos.second - y) <= OBSTACLE_RADIUS) {
                obstacle.second += (OBSTACLE_RADIUS - (realpos.second - y)) * -1;
            }
        }
        align /= align_amt;
        double ans = dir;
        if (avoid_amt > 0) {
            avoid.first /= avoid_amt;
            avoid.second /= avoid_amt;
            avoid_angle = atan2(avoid.second, avoid.first) * 360 / M_PI;
            ans += (avoid_angle - dir) * AVOID_STRENGTH;
        }
        obstacle_angle = atan2(obstacle.second, obstacle.first) * 360 / M_PI;
        ans += (obstacle_angle - dir) * OBSTACLE_STRENGTH;
        ans += (align - dir) * ALIGNMENT_STRENGTH;
        if (cohere_amt > 1) {
            cohere.first /= cohere_amt;
            cohere.second /= cohere_amt;
            cohere_angle = atan2(cohere.second, cohere.first) * 360 / M_PI;
            ans += (cohere_angle - dir) * COHERENCE_STRENGTH;
        }
        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(-(DEVIATION_ANGLE / 2), DEVIATION_ANGLE / 2);
        return ans + dist(mt);
    }
    void update(vector<Agent*> boid) {
        dir += (getTurn(boid) - dir) / TURN_SPEED;
        double delta = max((SDL_GetTicks() - starttick) / 5.0, 0.01);
        double ny = realpos.second - sin(dir * M_PI / 180) * speed * delta;
        double nx = realpos.first + cos(dir * M_PI / 180) * speed * delta;
        starttick = SDL_GetTicks();
        // cout << nx << " " << ny << "\n";
        if (ny < 0) {
            ny += SDL_WINDOW_SIZE;
        }
        if (nx < 0) {
            nx += SDL_WINDOW_SIZE;
        }
        if (nx > SDL_WINDOW_SIZE) {
            nx -= SDL_WINDOW_SIZE;
        } 
        if (ny > SDL_WINDOW_SIZE) {
            ny -= SDL_WINDOW_SIZE;
        }
        realpos.first = nx;
        realpos.second = ny;
    };
    void render() {
        rect->x = realpos.first;
        rect->y = realpos.second;
        SDL_RenderCopyEx(renderer, tex, NULL, rect, 90 - dir, NULL, SDL_FLIP_NONE);
    }
};
vector<Agent*> boid;

bool load() {
    SDL_Surface* surf = SDL_LoadBMP("images/agent.bmp");
    if (surf == NULL) {
        cout << "Can't get image.\n";
        cout << SDL_GetError() << "\n";
        return false;
    }
    agenttex = SDL_CreateTextureFromSurface(renderer, surf);
    return true;
}

void destroy() {
    boid = {};
    SDL_DestroyTexture(agenttex);
    agenttex = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Can't init\n";
        cout << SDL_GetError() << "\n";
        return 0;
    }
    
    window = SDL_CreateWindow("Boids Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WINDOW_SIZE, SDL_WINDOW_SIZE, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cout << "Can't make window\n";
        cout << SDL_GetError() << "\n";
        return 0;
    }

    // fill window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Couldn't create renderer. \n";
        cout << SDL_GetError() << "\n";
        return 0;
    }
    SDL_SetRenderDrawColor(renderer, 0x99, 0x99, 0x99, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Rect rect;
    if (load()) {
        SDL_RenderCopy(renderer, agenttex, NULL, &rect);
        SDL_RenderPresent(renderer);
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (!init()) return 1;

    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0, SDL_WINDOW_SIZE);
    uniform_real_distribution<double> ang(0, 359);
    for (int i = 0; i < AGENT_AMOUNT; i ++) {
        boid.push_back(new Agent(dist(mt), dist(mt), 180));
    }
    SDL_Event e; bool quit = false; 
    while (!quit) { 
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true; 
        } 

        SDL_RenderClear(renderer);
        for (Agent* a : boid) {
            a->update(boid);
            a->render();
        }
        SDL_RenderPresent(renderer);
    }

    destroy();
    return 0;
}