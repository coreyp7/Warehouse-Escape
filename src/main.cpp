#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <fstream>
using namespace std;

// my objects
#include "Box.h"
#include "Text.h"
#include "Tile.h"

// Defining some global constants + other shit
const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;

//const int CAMERA_PADDING = 75;
const int CAMERA_PADDING = 125;

const int EMPTY_TILE = 0;
const int GROUND_TILE = 1;
const int END_TILE = 2;
const int SPAWN_TILE = 3;

const SDL_Color SDL_COLOR_BLACK = { 0, 0, 0, 255 }; // black
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_COLOR_GREEN = {72, 255, 39, 255};

// FPS Cap when rendering. 60 by default.
short int fpsCap = 60;

SDL_Window* mainWindow;
SDL_Renderer* renderer;


SDL_Texture* bgTexture;
int bgTextureHeight;
int bgTextureWidth;

SDL_Texture* boxTexture;
SDL_Texture* tileTexture;
SDL_Texture* endTileTexture;
TTF_Font* globalFont;
TTF_Font* timerFont;

const int NUMBER_OF_LEVELS = 5; // Change this when you add/remove levels.
int currentLevelIndex = 0;
vector<string> levelNames;
vector<Tile> *currentLevelTiles; // current level's tileset.
vector<Tile> levelTilesets[NUMBER_OF_LEVELS]; // array of every level's tileset.
vector<pair<int, int>> levelSpawnPoints;
int playerStartX, playerStartY;

int levelCompleted = false;
Uint32 timeOfStartEntireGameRTA = 0; // for entire run of whole game
bool completedRTA = false;
Uint32 timeOfStart = 0; // start of timer in level
int timeOfFinish = -1; // time when the user finished this level

bool gameComplete = false;

bool debug = false;

Mix_Music *music = NULL;
Mix_Chunk *forceSound = NULL;

void close(){
    SDL_DestroyTexture( boxTexture );
    SDL_DestroyTexture( tileTexture );
    SDL_DestroyTexture( endTileTexture );
    SDL_DestroyTexture( bgTexture );
    TTF_CloseFont(globalFont);
    TTF_CloseFont(timerFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);

    boxTexture = NULL;
    tileTexture = NULL;
    endTileTexture = NULL;
    bgTexture = NULL;
    globalFont = NULL;
    timerFont = NULL;
    renderer = NULL;
    mainWindow = NULL;

}

// For loading all png/font stuff into their SDL_Texture fields.
bool loadAssets(){
    bool success = true;

    boxTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/box_final5.png");
    if(boxTexture == NULL){
        printf("Couldn't load box texture. %s", IMG_GetError());
        success = false;
    }

    tileTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/tile_final.png");
    if(tileTexture == NULL){
        printf("Couldn't load tile texture. %s", IMG_GetError());
        success = false;
    }

    endTileTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/end2.png");
    if(tileTexture == NULL){
        printf("Couldn't load tile texture. %s", IMG_GetError());
        success = false;
    }

    bgTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/warehouse.png");
    if(bgTexture == NULL){
        printf("Couldn't load box texture. %s", IMG_GetError());
    }
    // store its height for scrolling purposes.
    SDL_QueryTexture(bgTexture, NULL, NULL, &bgTextureWidth, &bgTextureHeight);

    // load font into global font
    globalFont = TTF_OpenFont("img/primitive_sandbox/Lato-Black.ttf", 26);
    if(globalFont == NULL){
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        return -7;
    }

    timerFont = TTF_OpenFont("img/primitive_sandbox/AdvancedPixel.ttf", 30);
    if(timerFont == NULL){
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        return -8;
    }

    music = Mix_LoadMUS("img/primitive_sandbox/phoon.wav");
    if(music == NULL){
        printf("Failed to load music. %s\n", Mix_GetError());
        return -9;
    }

    return success;
}

/* 
    List of stuff it does:
    - SDL_Init (no vsync bc I do it manually)
    - create sdl window, renderer, etc.
    - calls loadAssets
*/
int init(){
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return -1;
    }

    mainWindow = SDL_CreateWindow("Warehouse Escape", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
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

    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		return -7;
    }

    // load all textures and fonts
    if (!loadAssets()){
        return -6;
    }

    // levelNames.push_back("00_21_10");
    levelNames.push_back("01_21_10");
    levelNames.push_back("02_21_19");
    levelNames.push_back("03_42_12");
    levelNames.push_back("04_33_25");
    levelNames.push_back("05_15_35");

    return 0;
}

// Loads level from levels directory by filename (no file extension).
vector<Tile> loadLevel(string filename){
    vector<Tile> tiles;
    int x = 0, y = 0; // tile offsets

    std::ifstream map("levels/"+filename+".level");
    int width = stoi(filename.substr(3, 2));
    int length = stoi(filename.substr(6, 2));

    if(map.fail()){
        //printf("Unable to load map 1.");
    } else {
        for(int i=0; i<(width*length); i++){
            int type = -1;

            map >> type;

            if(map.fail()){
                //printf("Unable to load map 2.");
            }

            if(type == EMPTY_TILE){
                // do nothing
            } 
            else if(type == GROUND_TILE){
                tiles.push_back(Tile(renderer, tileTexture, x, y));
            }
            else if(type == END_TILE){
                tiles.push_back(Tile(renderer, endTileTexture, x, y, true));
            }
            else if(type == SPAWN_TILE){
                // if there's more than one then this breaks
                std::pair<int, int> pos = {x, y};
                levelSpawnPoints.push_back(pos);
            } 

            x += 75;

            // move to next row if at end of current row (down)
            if(x/75 > (width-1)){ 
                x = 0;
                y += 75;
            }
        }
    }

    return tiles;
}

// Return current time since the given time param in ticks in timer format.
string getTimeFormatted(Uint32 time){
    Uint32 ms = (SDL_GetTicks() - time)/10;
    Uint32 seconds = (SDL_GetTicks() - time)/1000;
    Uint16 minutes = seconds / 60;
    seconds -= (minutes*60);

    string secondsStr;
    if(seconds > 9){
        secondsStr = to_string(seconds);
    } else {
        secondsStr = "0"+to_string(seconds);
    }

    string msString = to_string(ms);

    // Prevent 'std::out_of_range' happening.
    // basic_string::substr: __pos (which is 4294967295) > this->size() (which is 1)
    if(msString.size() > 2){
        msString = msString.substr(msString.size()-2);
    } else {
        msString = "0" + msString.substr(msString.size()-1);
    }

    return to_string(minutes)+":"+secondsStr+":"+msString;
}

int main( int argc, char* args[] ){

    int initValue = init();

    if(initValue != 0){
        printf("Problem when trying to initialize for main loop. error #%d", initValue);
        return initValue;
    }

    // Main loop starts
    bool quit = false;

    SDL_Event e; // Event handler

    // FPS Stuff
    Uint32 frameStart = 0;

    // How long it took to complete the last frame.
    int frameTimeToComplete = 0;

    // "Delta time"
    // Used to apply physics in terms of time instead of frames.
    float dt = 0.0;

    Uint32 lastPhysicsUpdate = 0;

    float avgFPS = 0.0;

    int countedFrames = 0; // total frames rendered while application running (for avg fps calculation)
    std::stringstream avgFpsStr;

    // UI text to show.
    Text timerText = Text(renderer, timerFont, SDL_COLOR_GREEN);
    Text timerTextRTA = Text(renderer, timerFont, SDL_COLOR_GREEN);

    Text levelBeatenText = Text(renderer, globalFont, SDL_COLOR_WHITE);
    Text gameDoneText = Text(renderer, globalFont, SDL_COLOR_WHITE);
    Text gameDoneText2 = Text(renderer, globalFont, SDL_COLOR_WHITE);

    // Debug text
    Text avgFpsText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    Text msText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    //Text fpsText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    Text boxText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    Text cameraText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    Text velocityText = Text(renderer, globalFont, SDL_COLOR_BLACK);
    Text offsetText = Text(renderer, globalFont, SDL_COLOR_BLACK);

    // Load all level files and put into vector.
    for(int i=0; i<levelNames.size(); i++){
        levelTilesets[i] = loadLevel(levelNames[i]);
    }
    currentLevelTiles = &levelTilesets[0]; // set current level to first level address
    playerStartX = levelSpawnPoints[0].first;
    playerStartY = levelSpawnPoints[0].second;

    // Player object is called box for some reason
    Box box = Box(renderer, boxTexture, playerStartX, playerStartY);

    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    int offsetY = 0; // for offsetting the background textures.
    int offsetX = 0;
    int newStartY = 0; // indicates where scrolling should begin again for bg.
    int newStartX = 0;

    Mix_PlayMusic(music, -1);

    // While game is running
    while(!quit){
        frameStart = SDL_GetTicks(); // mark time(in m/s) at start of this frame

        // Go through event queue
        while( SDL_PollEvent( &e ) != 0 ){
            switch (e.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_p){
                        debug = !debug;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If box is clicked, apply force to it.
                    int xmousepos = e.button.x;
                    int ymousepos = e.button.y;

                    int xbox = box.getX() - camera.x;
                    int ybox = box.getY() - camera.y;

                    // If click was inside the box
                    if(xmousepos > xbox && xmousepos < xbox+box.getWidth() &&
                    ymousepos > ybox && ymousepos < ybox+box.getHeight())
                    {
                        // apply vertical
                        int scaling = 20;

                        float yCenter = ybox + (box.getHeight()/2);
                        float yForce = 0.0;

                        if(ymousepos > yCenter){
                            // move up (negative)
                            yForce = -fabs(ymousepos - yCenter);
                        } else if(ymousepos < yCenter){
                            // move down (positive)
                            yForce = fabs(yCenter - ymousepos);
                        }


                        // apply horizontal
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
                            // exactly center (should never happen)
                        }

                        box.applyXYVelocity(xForce*scaling, yForce*scaling);
                    }
                    
                    break;
            }
        }

        ///////////////////////////
        //  PHYSICS STARTS HERE  //
        ///////////////////////////

        // (now - last physics update) / 1000 (put into seconds)
        dt = (SDL_GetTicks() - lastPhysicsUpdate) / 1000.0f;
        box.simulatePhysics(dt, *currentLevelTiles);
        lastPhysicsUpdate = SDL_GetTicks();

        if(box.completedLevel){
            // Stop RTA timer if totally done.
            if(currentLevelIndex+1 >= NUMBER_OF_LEVELS){
                completedRTA = true;
            }

            if(timeOfFinish == -1){
                timeOfFinish = SDL_GetTicks();
                camera.y -= 100;
            } else {

                if(timeOfFinish + 2000 < SDL_GetTicks()){

                    if(! (currentLevelIndex+1 >= NUMBER_OF_LEVELS)){
                        currentLevelIndex++; // go to next level
                        currentLevelTiles = &levelTilesets[currentLevelIndex];
                        box.xvelocity = 0;
                        box.yvelocity = 0;
                        box.x = levelSpawnPoints[currentLevelIndex].first;
                        box.y = levelSpawnPoints[currentLevelIndex].second;
                        box.completedLevel = false;
                        timeOfStart = SDL_GetTicks();
                        timeOfFinish = -1;
                    } else {
                        // Game completed, show text.
                        box.x = -500;
                        box.y = -500;
                        box.xvelocity = 0;
                        box.yvelocity = 0;
                        levelBeatenText.changeText("That's all the levels.");
                        gameDoneText.changeText("Thank you, I appreciate you playing this. ~Corey :)");
                        gameDoneText2.changeText("(you can press 'p' key to see debug info for fun)");

                        gameComplete = true;
                    }
                }
            }

        } else {
            // Adjust camera position depending on player pos
            if(box.getY() < camera.y + CAMERA_PADDING){
                int oldCamY = camera.y;
                camera.y = box.getY() - CAMERA_PADDING;
                offsetY += fabs(camera.y - oldCamY);
            } else if(box.y + box.BOX_HEIGHT > camera.y + camera.h - CAMERA_PADDING){
                int oldCamY = camera.y;
                int newBottom = (box.y + box.BOX_HEIGHT) + CAMERA_PADDING;
                camera.y = newBottom - camera.h;
                offsetY -= fabs(oldCamY - camera.y);
            }

            if(box.x + box.BOX_WIDTH > (camera.x + camera.w) - CAMERA_PADDING){ // right side
                int oldCamX = camera.x;
                int newRight = (box.x + box.BOX_WIDTH) + CAMERA_PADDING;
                camera.x = newRight - camera.w;
                offsetX += fabs(camera.x - oldCamX);
            } else if(box.x < (camera.x + CAMERA_PADDING)){ // left side
                int oldCamX = camera.x;
                camera.x = box.x - CAMERA_PADDING;
                offsetX -= fabs(oldCamX - camera.x);
            }
        }

        //Calculate avg fps
        avgFPS = countedFrames / ( SDL_GetTicks() / 1000.f );
        if( avgFPS > 2000000 )
        {
            avgFPS = 0;
        }

        ///////////////////////////
        // RENDERING STARTS HERE //
        ///////////////////////////
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(renderer);

        if(offsetY > bgTextureHeight){
            // When the first bg goes offscreen (offset > bgTextureHeight), 
            // the offset is restarted and the "newStart" (which indicates the new top of bg1) 
            // is set to the camera's y position.
            offsetY = 0;
            newStartY = camera.y;
        } else if(offsetY < -bgTextureHeight){
            offsetY = 0;
            newStartY = camera.y;
        }

        // Do the same in the x-axis
        if(offsetX > bgTextureWidth){
            offsetX = 0;
            newStartX = camera.x;
        } else if(offsetX < -bgTextureWidth){
            offsetX = 0;
            newStartX = camera.x;
        }

        
        /**
         * bg1-4 are four duplicates of the background image.
         * Depending on the direction the player is moving, bg2/4 will 
         * display on the left/right of bg1/3.
         * Complicated but works.
         * Currently works strictly with camera width/height, which could be changed
         * to just be the w/h of the image loaded.
         */
        SDL_Rect bg1 = {camera.x - (offsetX*2) - newStartX, 
            camera.y + (offsetY*2) - newStartY, 
            bgTextureWidth, 
            bgTextureHeight };
        SDL_Rect bg2 = {camera.x - (offsetX*2) - newStartX, 0, bgTextureWidth, bgTextureHeight};
        SDL_Rect bg3 = {0, bg1.y, bgTextureWidth, bgTextureHeight };

        if(offsetX >= 0){
            bg3.x = bg1.x + bgTextureWidth;
        } else {
            bg3.x = bg1.x - bgTextureWidth;
        }

        SDL_Rect bg4 = {bg3.x, bg3.y, bgTextureWidth, bgTextureHeight};

        if(offsetY >= 0){
            bg2.y = bg1.y - bgTextureHeight;
            bg4.y = bg1.y - bgTextureHeight;
        } else {
            bg2.y = bg1.y + bgTextureHeight;
            bg4.y = bg1.y + bgTextureHeight;
        }

        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg1, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg2, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg3, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, bgTexture, NULL, &bg4, 0, NULL, SDL_FLIP_NONE);


        // oldtodo: recall the confusion of this working.
        // printf it and camera.y + (offset*2) is the same value as newStart.
        // ACTUALLY: nevermind I figured out why this worked. I understood this in the moment
        // while troubleshooting but was astounded that a solution actually worked for once.
        // Nice job.
        
        // Render level
        for(int i = 0; i<currentLevelTiles->size(); i++){
            currentLevelTiles->at(i).render(camera.x, camera.y);
        }

        box.render(camera.x, camera.y);

        if(debug){
            // Update avg fps text
            avgFpsStr.str("");
            avgFpsStr << "Avg FPS: " << avgFPS;
            avgFpsText.changeText(avgFpsStr.str().c_str());
            avgFpsText.render(0, 0);
            msText.changeText("ms render frame: "+std::to_string(SDL_GetTicks() - frameStart));
            msText.render(0, avgFpsText.getHeight());
            //fpsText.render(0, avgFpsText.getHeight() + msText.getHeight());
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
            oss4 << "offsetY:" << offsetY << ", offsetX:" << offsetX;
            offsetText.changeText(oss4.str());
            offsetText.render(WINDOW_WIDTH - offsetText.getWidth(), boxText.getHeight() + cameraText.getHeight() + velocityText.getHeight());
        }

        // Update the timers if its still appropriate to.
        if(!box.completedLevel){
            timerText.changeText(getTimeFormatted(timeOfStart));
        }

        if(!completedRTA){
            timerTextRTA.changeText(getTimeFormatted(timeOfStartEntireGameRTA));
        }
        
        timerTextRTA.render((WINDOW_WIDTH - timerText.getWidth())/2, 10);
        timerText.render((WINDOW_WIDTH - timerText.getWidth())/2, timerTextRTA.getHeight() + 10);

        if(gameComplete){
            levelBeatenText.render( (WINDOW_WIDTH-levelBeatenText.getWidth())/2, (WINDOW_HEIGHT-levelBeatenText.getHeight())/2);
            gameDoneText.render((WINDOW_WIDTH-gameDoneText.getWidth())/2, (WINDOW_HEIGHT+gameDoneText.getHeight()+levelBeatenText.getHeight())/2);
            gameDoneText2.render((WINDOW_WIDTH-gameDoneText2.getWidth())/2, 
                (WINDOW_HEIGHT-gameDoneText2.getHeight()-150));
        } else if(box.completedLevel){
            levelBeatenText.changeText("Escaped level "+to_string(currentLevelIndex+1));
            levelBeatenText.render((WINDOW_WIDTH-levelBeatenText.getWidth())/2, (WINDOW_HEIGHT-levelBeatenText.getHeight())/2);
        }


        SDL_RenderPresent(renderer);
        countedFrames++;

        // Finished rendering, cap framerate.
        // If frame is finished early, wait remaining time.
        frameTimeToComplete = SDL_GetTicks() - frameStart;
        if(1000 / fpsCap > frameTimeToComplete){
            SDL_Delay((1000/fpsCap) - frameTimeToComplete);
        }

    }

    // Clean everything before closing application.
    avgFpsText.~Text();
    msText.~Text();
    boxText.~Text();
    cameraText.~Text();
    velocityText.~Text();
    offsetText.~Text();
    close();

    return 0;

}