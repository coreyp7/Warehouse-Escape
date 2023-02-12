
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const int BALL_SIZE = 10;

typedef struct Ball {
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    int size;
} Ball;

typedef struct Player {
    int score;
    float yPosition;
} Player;

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;
const float PLAYER_MOVE_SPEED = 250.0f; //150.0f;

bool served = false;
Ball ball;
Player player1;
Player player2;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool Initialize(void);
void Update(float);
void Shutdown(void);
Ball MakeBall(int size);
void UpdateBall(Ball *ball, float elapsed);
void RenderBall(const Ball*);

Player MakePlayer(void);
void UpdatePlayers(float elapsed);
void RenderPlayers(void);

void UpdateScore(int player, int points);

int main(int argc, char * argv[]){
    atexit(Shutdown); // weird look this up for more info

    if(!Initialize()){
        exit(1);
    }

    bool quit = false;
    SDL_Event event;

    Uint32 lastTick = SDL_GetTicks();

    while(!quit){
        // get input and shit
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
            }
        }

        // Now update our render of the gamestate
        Uint32 curTick = SDL_GetTicks();
        Uint32 diff = curTick - lastTick;
        float elapsed = diff / 1000.0f; // convert to seconds
        Update(elapsed);
        lastTick = curTick;
    }

    SDL_Quit();
}

bool Initialize(void){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
            printf("Failed to initialize SDL");
            return false;
        }

        window = SDL_CreateWindow("Pong",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        if(!window){
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(!window){
            return false;
        }

        ball = MakeBall(BALL_SIZE);
        player1 = MakePlayer();
        player2 = MakePlayer();

        return true;
}

void Update(float elapsed){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // update the "gamestate" of the ball
        UpdateBall(&ball, elapsed);
        // then render the ball with its new position
        RenderBall(&ball);

        UpdatePlayers(elapsed);
        RenderPlayers();

        SDL_RenderPresent(renderer);
}

void Shutdown(void){
    if(renderer){
        SDL_DestroyRenderer(renderer);
    }
    if(window){
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

Ball MakeBall(int size) {
    const float SPEED = 120;
    Ball ball = {
        .x = WIDTH / 2 - BALL_SIZE / 2,
        .y = HEIGHT / 2 - BALL_SIZE / 2,
        .size = size,
        .xSpeed = SPEED * 2.5,
        .ySpeed = SPEED * 2.5,
        };
    return ball;
}

void RenderBall(const Ball* ball){
    int size = ball->size;
    int halfSize = size / 2;
    SDL_Rect rect = {
        .x = ball->x - halfSize,
        .y = ball->y - halfSize,
        .w = size,
        .h = size
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void UpdateBall(Ball *ball, float elapsed){
    if(!served){
        ball->x = WIDTH /2;
        ball->y = HEIGHT / 2;
        return;
    }
    ball->x += ball->xSpeed * elapsed;
    ball->y += ball->ySpeed * elapsed;

    
    // check left wall
    if(ball->x/2 < BALL_SIZE / 2){
        //ball->xSpeed = fabs(ball->xSpeed);
        UpdateScore(2, 100);
    }
    // check right wall
    if(ball->x > WIDTH - BALL_SIZE / 2){
        //ball->xSpeed = -fabs(ball->xSpeed);
        UpdateScore(1, 100);
    }

    // check top wall
    if(ball->y < BALL_SIZE / 2){
        ball->ySpeed = fabs(ball->ySpeed);
    }
    // check bottom wall
    if(ball->y > HEIGHT - BALL_SIZE / 2){
        ball->ySpeed = -fabs(ball->ySpeed);
    }
}

Player MakePlayer(void){
    Player player = { // sets score to 0 default
        .yPosition = HEIGHT / 2,
    };
    return player;
}

void UpdatePlayers(float elapsed) {
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    if(keyboardState[SDL_SCANCODE_SPACE]){
        served = true;
    }

    if(keyboardState[SDL_SCANCODE_W]){
        player1.yPosition -= PLAYER_MOVE_SPEED * elapsed;
    }
    if(keyboardState[SDL_SCANCODE_S]){
        player1.yPosition += PLAYER_MOVE_SPEED * elapsed;
    }

    if(keyboardState[SDL_SCANCODE_UP]){
        player2.yPosition -= PLAYER_MOVE_SPEED * elapsed;
    }
    if(keyboardState[SDL_SCANCODE_DOWN]){
        player2.yPosition += PLAYER_MOVE_SPEED * elapsed;
    }

    if(player1.yPosition < PLAYER_HEIGHT / 2){
        player1.yPosition = PLAYER_HEIGHT / 2;
    }

    if(player1.yPosition > HEIGHT - PLAYER_HEIGHT / 2){
        player1.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }

    if(player2.yPosition < PLAYER_HEIGHT / 2){
        player2.yPosition = PLAYER_HEIGHT / 2;
    }

    if(player2.yPosition > HEIGHT - PLAYER_HEIGHT / 2){
        player2.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }

    // Check if ball rect  overlaps with players

    SDL_Rect ballRect = {
        .x = ball.x - ball.size / 2,
        .y = ball.y - ball.size / 2,
        .w = ball.size,
        .h = ball.size
    };
    
    SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    if(SDL_HasIntersection(&ballRect, &player1Rect)){
        ball.xSpeed = fabs(ball.xSpeed); // make ball go right
    }

    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    if(SDL_HasIntersection(&ballRect, &player2Rect)){
        ball.xSpeed = -fabs(ball.xSpeed); // make ball go left
    }
}

void RenderPlayers(void){
    // Render Player 1 (left, red)
    SDL_SetRenderDrawColor(renderer, 255, 0,0, 255);
    SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    SDL_RenderFillRect(renderer, &player1Rect);

    // Render Player 2 (right, blue)
    SDL_SetRenderDrawColor(renderer, 0, 0,255, 255);
    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    SDL_RenderFillRect(renderer, &player2Rect);
}

void UpdateScore(int player, int points){
    served = false;

    if(player == 1){
        player1.score += points;
    }
    if(player == 2){
        player2.score += points;
    }

    char *fmt = "Player 1: %d | Player 2: %d";
    int len = snprintf(NULL, 0, fmt, player1.score, player2.score);
    char buf[len+1];
    snprintf(buf, len + 1, fmt, player1.score, player2.score);
    SDL_SetWindowTitle(window, buf);
}