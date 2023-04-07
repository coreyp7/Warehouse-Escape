#include "Box.h"
#include <cmath>
#include <stdio.h>

Box::Box(SDL_Renderer* renderer, SDL_Texture* texture, float startingPosX, float startingPosY) {
    this->renderer = renderer;
    this->texture = texture;

    x = startingPosX;
    y = startingPosY;
    xvelocity = 0;
    yvelocity = 0;
}

// void Box::applyForceUp(){
//     yvelocity = CLICK_VELOCITY;
// }

// void Box::applyXVelocity(float force){
//     xvelocity = force;
// }

void Box::applyXYVelocity(float xForce, float yForce){
    xvelocity = xForce;
    yvelocity = yForce;
}

// void Box::renderTESTINGONLY(int posx, int posy){
//     rect = {posx, posy, BOX_WIDTH, BOX_HEIGHT};
//     SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
// }

void Box::render(int xcam, int ycam){
    rect = {static_cast<int>(round(x)) - xcam, static_cast<int>(round(y) - ycam), BOX_WIDTH, BOX_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

// void Box::render(){
//     rect = {static_cast<int>(round(x)), static_cast<int>(round(y)), BOX_WIDTH, BOX_HEIGHT};
//     SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
// }

int Box::getWidth(){
    return BOX_WIDTH;
}

int Box::getHeight(){
    return BOX_HEIGHT;
}

float Box::getX(){
    return x;
}

float Box::getY(){
    return y;
}

float Box::getYVelocity(){
    return yvelocity;
}

float Box::getXVelocity(){
    return xvelocity;
}

void Box::simulatePhysics(float dt, vector<Tile> &tiles){

    yvelocity += dt * GRAVITY;

    float xold = x;
    float yold = y;

    y += yvelocity * dt;
    x += xvelocity * dt;

    // If level is completed, just ignore collision.
    if(completedLevel) {
        return;
    }

    Tile* tile; 

    for(int i=0; i<tiles.size(); i++){
        tile = &tiles[i];

        if(tile->isColliding(this)){

            if(tile->finish){
                yvelocity = -1500;
                completedLevel = true;
            } else {
                
                float xDistance, yDistance;
                if(x < tile->x){
                    xDistance = ((x + BOX_WIDTH) - tile->x);
                } else { // x >= tile->x
                    xDistance = ((tile->x + tile->TILE_WIDTH) - x);
                }

                if(y < tile->y){
                    yDistance = ((y + BOX_HEIGHT) - tile->y);
                } else { // y >= tile->y
                    yDistance = ((tile->y + tile->TILE_HEIGHT) - y);
                }

                // Check if this old x position isn't colliding with any of the
                // tiles next to the current tile. Prevents player from getting
                // stuck on ground while moving.
                if(!tile->isColliding(xold, y, BOX_WIDTH, BOX_HEIGHT) &&
                    ! tiles[i-1].isColliding(xold, y, BOX_WIDTH, BOX_HEIGHT) &&
                    ! tiles[i+1].isColliding(xold, y, BOX_WIDTH, BOX_HEIGHT)){
                    if(xvelocity > 0){
                        x -= xDistance;
                    } else {
                        x += xDistance;
                    }
                    xvelocity = 0;
                    //printf("xold:%f, yold:%f, x:%f, y:%f\n", xold, yold, x, y);
                } else if (!tile->isColliding(x, yold, BOX_WIDTH, BOX_HEIGHT)){
                    // keep yold, move on to next tile
                    //y = yold;
                    // fix y-axis of box only
                    if(yvelocity > 0){
                        y -= yDistance;
                    } else {
                        y += yDistance;
                    }
                    yvelocity = 0;
                    if(xvelocity > 4){
                        xvelocity += dt * (-X_FRICTION);
                    } else if(xvelocity < -4){
                        xvelocity += dt * X_FRICTION;
                    } else {
                        xvelocity = 0;
                    }
                    
                } else {
                    // keep both old vars
                    x = xold;
                    y = yold;
                }
            }
        }
    }
    
    tile = NULL;
}

Box::~Box() {
    // There's no dynamically allocated memory in here.
    // The two pointers in Box (renderer and texture) are
    // properly cleaned up in main, so don't need to
    // do anything in here.
}

