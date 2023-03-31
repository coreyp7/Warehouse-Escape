#include <SDL.h>
#include <SDL_image.h> // for loading pngs
#include <SDL_ttf.h> // for rendering text
#include <stdio.h>
#include <string> // TODO: look into how this shit works at all
#include <sstream>
#include <cmath>
#include <vector>

using namespace std;

// local object stuff
#include "Box.h" // api for box object stuff
#include "Text.h"
#include "Tile.h"

// Defining some global constants + other shit
// const int WINDOW_WIDTH = 640;
// const int WINDOW_HEIGHT = 480;
const int WINDOW_WIDTH = 720;
const int WINDOW_HEIGHT = 480;

const int CAMERA_PADDING = 75;

// FPS Cap when rendering. 60 by default.
short int fpsCap = 60;

SDL_Window* mainWindow;
SDL_Renderer* renderer;

SDL_Texture* boxTexture;
SDL_Texture* tileTexture;

TTF_Font* globalFont;

SDL_Texture* bgTexture;
int bgTextureHeight;

vector<Tile> tiles;

//Tile* tiles[1];


void close(){
    SDL_DestroyTexture( boxTexture );
    SDL_DestroyTexture( bgTexture );
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    boxTexture = NULL;
    renderer = NULL;
    mainWindow = NULL;
}

// For loading all png/font stuff into their SDL_Texture fields.
bool loadMedia(){
    bool success = true;

    boxTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/boxBig.png");
    if(boxTexture == NULL){
        printf("Couldn't load box texture. %s", IMG_GetError());
        success = false;
    }

    tileTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/tile.png");
    if(tileTexture == NULL){
        printf("Couldn't load tile texture. %s", IMG_GetError());
        success = false;
    }

    bgTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/clouds.png");
    if(bgTexture == NULL){
        printf("Couldn't load box texture. %s", IMG_GetError());
        success = false;
    }
    // store its height for scrolling purposes.
    SDL_QueryTexture(bgTexture, NULL, NULL, NULL, &bgTextureHeight);

    // load font into global font
    globalFont = TTF_OpenFont("img/primitive_sandbox/Lato-Black.ttf", 26);
    if(globalFont == NULL){
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        return -7;
    }

    return success;
}

/* 
    List of shit it does:
    - SDL_Init (no vsync)
    - create sdl window & renderer
    - initialize IMG/font stuff and load the files (boxTexture/globalFont/etc)
*/
int init(){
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return -1;
    }

    mainWindow = SDL_CreateWindow("Physics based off time", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
    if(mainWindow == NULL){
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return -2;
    }

    // todo: create renderer
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return -3;
    }

    // todo: set renderer draw color to black/default
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // todo: initialize sdl_image
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init(imgFlags) & imgFlags ) ){ // TODO: look back at lazyfoo to see wtf this is
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return -4;
    }

    if( TTF_Init() == -1){
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        return -5;
    }

    // load box texture
    if (!loadMedia()){
        return -6;
    }

    return 0;
}

int main( int argc, char* args[] ){

    int initValue = init();

    if(initValue != 0){
        printf("Problem when trying to initialize for main loop. error #%d", initValue);
        return initValue;
    }

    // Main loop starts
    bool quit = false; // Main loop flag

    SDL_Event e; // Event handler

    // FPS Stuff
    Uint32 frameStart = 0;
    // Keep track of the time of each frame.
    int frameTimeToComplete = 0;

    float avgFPS = 0.0;

    float dt = 0.0;

    int countedFrames = 0; // keeps track of total frames rendered while application running
    std::stringstream avgFpsStr; // string for displaying counted

    // Text objects to show.
    SDL_Color textColor = { 0, 0, 0, 255 }; // black
    Text avgFpsText = Text(renderer, globalFont, textColor);
    Text msText = Text(renderer, globalFont, textColor);
    Text fpsText = Text(renderer, globalFont, textColor);
    fpsText.changeText("FPS cap: " +std::to_string(fpsCap));
    Text boxText = Text(renderer, globalFont, textColor);
    Text cameraText = Text(renderer, globalFont, textColor);
    Text velocityText = Text(renderer, globalFont, textColor);
    Text offsetText = Text(renderer, globalFont, textColor);

    Box box = Box(renderer, boxTexture, 
    (WINDOW_WIDTH-50)/2, 
    0);

    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    int offset = 0; // for offsetting the background images.
    int newStart = 0; // indicates where scrolling should begin again.

    // tiles.push_back(Tile(renderer, tileTexture, (WINDOW_WIDTH-50)/3, 250));
    tiles.push_back(Tile(renderer, tileTexture, 0, 0));

    tiles.push_back(Tile(renderer, tileTexture, 0, 75));
    tiles.push_back(Tile(renderer, tileTexture, 0, 75*2));
    tiles.push_back(Tile(renderer, tileTexture, 0, 75*3));
    tiles.push_back(Tile(renderer, tileTexture, 0, 75*4));
    // tiles.push_back(Tile(renderer, tileTexture, 0, 75*5));

    tiles.push_back(Tile(renderer, tileTexture, 75, 75*5));
    tiles.push_back(Tile(renderer, tileTexture, 75*2, 75*5));
    tiles.push_back(Tile(renderer, tileTexture, 75*3, 75*5));
    tiles.push_back(Tile(renderer, tileTexture, 75*4, 75*5));
    tiles.push_back(Tile(renderer, tileTexture, 75*5, 75*5));

    tiles.push_back(Tile(renderer, tileTexture, 75*4, 75*5));
    tiles.push_back(Tile(renderer, tileTexture, 75*8, 75*3));

    tiles.push_back(Tile(renderer, tileTexture, 75*6, 75*3));


    // While game is running
    while(!quit){
        frameStart = SDL_GetTicks(); // mark time(in m/s) at start of this frame

        // Go through event queue
        while( SDL_PollEvent( &e ) != 0 ){
            
            // // User requests quit
            // if (e.type == SDL_QUIT){
            //     quit = true;
            // }
            switch (e.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If box is clicked, apply upward force to it.
                    int xmousepos = e.button.x;
                    int ymousepos = e.button.y;

                    int xbox = box.getX() - camera.x;
                    int ybox = box.getY() - camera.y;

                    // If click was inside the box
                    if(xmousepos > xbox && xmousepos < xbox+box.getWidth() &&
                    ymousepos > ybox && ymousepos < ybox+box.getHeight())
                    {
                        // apply vertical
                        //printf("box.applyForceUp()");
                        //box.applyForceUp();
                        int scaling = 20;

                        float yCenter = ybox + (box.getHeight()/2);
                        float yForce = 0.0;

                        if(ymousepos > yCenter){
                            // move up (negative)
                            yForce = -fabs(ymousepos - yCenter);
                            //yForce = -20;
                        } else if(ymousepos < yCenter){
                            // move down (positive)
                            yForce = fabs(yCenter - ymousepos);
                            //yForce = 20;
                        }
                        //printf("yCenter:%f, ymousepos:%i, yForce:%f", yCenter, ymousepos, yForce);
                        //printf("yforce:%d", yForce);

                        // apply horizontal
                        
                        // center (x only) of box
                        float center = xbox + (box.getWidth()/2);

                        // Figure out how much force to apply to xvelocity.
                        // (further from the center, the more force).
                        float xForce = 0.0;

                        if(xmousepos < center){
                            xForce = ((box.getWidth()/2) + xbox) - xmousepos;
                        } else if(xmousepos > center){
                            // negative bc its going right
                            xForce = -(xmousepos - ((box.getWidth()/2) + xbox));
                        } else { 
                            // exactly center (should never really happen)
                            //TODO
                        }

                        //box.applyXVelocity(xForce*scaling);
                        box.applyXYVelocity(xForce*scaling, yForce*scaling);
                    }
                    
                    break;
            }
        }

        ///////////////////////////
        //  PHYSICS STARTS HERE  //
        ///////////////////////////

        dt = (SDL_GetTicks() - box.lastPhysicsUpdate) / 1000.0f;
        box.simulatePhysics(dt, tiles);
        

        // Adjust camera position depending on player pos
        if(box.getY() < camera.y + CAMERA_PADDING){
            int oldCamY = camera.y;
            camera.y = box.getY() - CAMERA_PADDING;
            offset += fabs(camera.y - oldCamY);
        } else if(box.y + box.BOX_HEIGHT > camera.y + camera.h - CAMERA_PADDING){
            // int oldCamY = camera.y;
            // camera.y = box.y + box.BOX_HEIGHT + CAMERA_PADDING;
            // offset -= fabs(oldCamY - camera.y);
            // printf("camera_padding applied");
            int oldCamY = camera.y;
            int newBottom = (box.y + box.BOX_HEIGHT) + CAMERA_PADDING;
            camera.y = newBottom - camera.h;
            offset -= fabs(oldCamY - camera.y);
        }

        //Calculate avg fps
        avgFPS = countedFrames / ( SDL_GetTicks() / 1000.f );
        if( avgFPS > 2000000 )
        {
            avgFPS = 0;
        }

        // Update avg fps text
        avgFpsStr.str("");
        avgFpsStr << "Avg FPS: " << avgFPS;
        avgFpsText.changeText(avgFpsStr.str().c_str());

        ///////////////////////////
        // RENDERING STARTS HERE //
        ///////////////////////////
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(renderer);

        if(offset > bgTextureHeight){
            // When the first bg goes offscreen (offset > bgTextureHeight), 
            // the offset is restarted and the "newStart" (which indicates the new top of bg1) 
            // is set to the camera's y position.
            offset = 0;
            newStart = camera.y;
            //printf("camera.y + (offset*2):%d, newStart:%d", camera.y + (offset*2), newStart);
        } else if(offset < -bgTextureHeight){
            offset = 0;
            newStart = camera.y;
        }

        SDL_Rect bg1 = {0, camera.y + (offset*2) - newStart, camera.w, camera.h };
        SDL_Rect bg2;

        // Put bg2 above/below bg1 depending on offset
        if(offset >= 0){
            bg2 = {0, bg1.y - bgTextureHeight, camera.w, camera.h }; 
        }else {
            bg2 = {0, bg1.y + bgTextureHeight, camera.w, camera.h };    
        }

        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg1, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg2, 0, NULL, SDL_FLIP_NONE);

        // oldtodo: recall the confusion of this working.
        // printf it and camera.y + (offset*2) is the same value as newStart.
        // ACTUALLY: nevermind I figured out why this worked. I understood this in the moment
        // while troubleshooting but was astounded that a solution actually worked for once.
        // Nice job.
        // TODO: initialize these variables outside while loop so you're not creating it everytime.
        

        for(int i = 0; i<tiles.size(); i++){
            tiles[i].render(camera.x, camera.y);
        }

        //box.render();
        box.render(camera.x, camera.y);

        // show avg fps text & ms text
        avgFpsText.render(0, 0);
        msText.changeText("ms render frame: "+std::to_string(SDL_GetTicks() - frameStart));
        msText.render(0, avgFpsText.getHeight());
        fpsText.render(0, avgFpsText.getHeight() + msText.getHeight());
        std::ostringstream oss;
        oss << "x:" << box.getX() << ", y:" << box.getY();
        boxText.changeText(oss.str());
        boxText.render(WINDOW_WIDTH - boxText.getWidth(), 0);
        std::ostringstream oss2;
        oss2 << "cam | x:" << camera.x << ", y:" << camera.y;
        cameraText.changeText(oss2.str());
        cameraText.render(WINDOW_WIDTH - cameraText.getWidth(), boxText.getHeight());
        std::ostringstream oss3;
        oss3 << "velocity: (" << box.getXVelocity() << ", " << box.getYVelocity() << ")";
        velocityText.changeText(oss3.str());
        velocityText.render(WINDOW_WIDTH - velocityText.getWidth(), boxText.getHeight() + cameraText.getHeight());
        std::ostringstream oss4;
        oss4 << "offset:" << offset;
        offsetText.changeText(oss4.str());
        offsetText.render(WINDOW_WIDTH - offsetText.getWidth(), boxText.getHeight() + cameraText.getHeight() + velocityText.getHeight());


        SDL_RenderPresent(renderer);
        countedFrames++; // NOTE: not sure if this should be lower in loop

        // Finished rendering, cap framerate.
        // If frame is finished early, wait remaining time.
        frameTimeToComplete = SDL_GetTicks() - frameStart;
        if(1000 / fpsCap > frameTimeToComplete){
            SDL_Delay((1000/fpsCap) - frameTimeToComplete);
        }

    }

    //Free resources and close SDL
    close();

    return 0;

}