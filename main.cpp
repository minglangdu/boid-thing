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
    int getTurn(vector<Agent*> boid) {
        double align_amt = 1, avoid_amt = 0, cohere_amt = 1;
        double align = dir;
        pair<double, double> avoid = {0, 0};
        double avoid_angle = 0, cohere_angle = 0;
        pair<double, double> cohere = {realpos.first, realpos.second};
        for (Agent* b : boid) {
            if (b == this) continue;
            if ((pow(realpos.first - b->realpos.first, 2) + 
            pow(realpos.second - b->realpos.second, 2)) <= pow(ALIGNMENT_RADIUS, 2)) {
                align_amt ++;
                align += b->dir;
            }
            if ((pow(realpos.first - b->realpos.first, 2) + 
            pow(realpos.second - b->realpos.second, 2)) <= pow(AVOID_RADIUS, 2)) {
                avoid.first += (b->realpos.first - realpos.first);
                avoid.second += (b->realpos.second - realpos.second);
                avoid_amt ++;
            }
            if ((pow(realpos.first - b->realpos.first, 2) + 
            pow(realpos.second - b->realpos.second, 2)) <= pow(COHERENCE_RADIUS, 2)) {
                cohere.first += (b->realpos.first);
                cohere.second += (b->realpos.second);
                cohere_amt ++;
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