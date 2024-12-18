#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

using namespace std;

int SDL_WINDOW_SIZE = 600;
int AGENT_SIZE = 30;

SDL_Window* window = NULL;
SDL_Texture* agenttex = NULL;
SDL_Renderer* renderer = NULL;
struct Agent {
    SDL_Rect rect;
    int dir = 0;
    SDL_Texture* tex = agenttex;
    Agent(int x, int y, int d) {
        rect.x = x;
        rect.y = y;
        rect.h = AGENT_SIZE;
        rect.w = AGENT_SIZE;
        dir = d;
    };
    void update() {
        // rect.x += 1;
    };
    void render() {
        SDL_RenderCopy(renderer, tex, NULL, &rect);
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

    boid.push_back(new Agent(50, 50, 0));
    boid.push_back(new Agent(50, 100, 0));
    boid.push_back(new Agent(50, 0, 0));

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
            a->update();
            a->render();
        }
        SDL_RenderPresent(renderer);
    }

    destroy();
    return 0;
}