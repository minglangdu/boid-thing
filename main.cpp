#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <random>

using namespace std;

const int SDL_WINDOW_SIZE = 700;
const int AGENT_SIZE = 15;
const int AGENT_AMOUNT = 30;
const int TURN_SPEED = 15; // larger values are slower turns
const int MOVE_SPEED = 1;
const int SIGHT_RADIUS = 150;
const int DEVIATION_ANGLE = 70;

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
        vector<Agent*> group = {this};
        double avgdir = dir;
        for (Agent* b : boid) {
            if ((pow(realpos.first - b->realpos.first, 2) + pow(realpos.second - b->realpos.second, 2)) <= pow(SIGHT_RADIUS, 2)) {
                group.push_back(b);
                avgdir += b->dir;
            }
        }
        avgdir /= (double) group.size();
        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(-(DEVIATION_ANGLE / 2), DEVIATION_ANGLE / 2);
        return avgdir + dist(mt);
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
    for (int i = 0; i < AGENT_AMOUNT; i ++) {
        boid.push_back(new Agent(dist(mt), dist(mt), (int)dist(mt) % 360));
    }
    //Hack to get window to stay up
    SDL_Event e; bool quit = false; 
    while (!quit) { 
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true; 
            // if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
            //     rect.x += 5;
            // }
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