#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

const float GRAVITY = -10;

typedef struct Player {
    float jumpSpeed;
    float walkSpeed;
    float velocity;

} Player;

bool Initialize(void);
void MainUpdate(float elapsed);

int main(int argc, char *argv[]){
    if(!Initialize()){
        exit(1);
    }

    bool quit = false;
    SDL_Event event;

    Uint32 lastTick = SDL_GetTicks();

    while(!quit){
        // detect events like closing the window
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
            }
        }
        // Get current "elapsed" tick amount.
        Uint32 curTick = SDL_GetTicks();
        Uint32 diff = curTick - lastTick;
        float elapsed = diff / 1000.0f; // convert to seconds
        MainUpdate(elapsed);
        lastTick = curTick;
    }

    SDL_Quit();
}

void MainUpdate(float elapsed){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect ex = {
        .x = 150,//WINDOW_WIDTH / 2,
        .y = 150,//WINDOW_HEIGHT / 2,
        .w = 50,
        .h = 50
    };
    SDL_RenderFillRect(renderer, &ex);

    SDL_RenderPresent(renderer);
}

bool Initialize(void){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
            printf("Failed to initialize SDL");
            return false;
        }

        window = SDL_CreateWindow("Pong",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if(!window){
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(!window){
            return false;
        }

        return true;
}