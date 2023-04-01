#include "Box.h"
#include <cmath>
#include <stdio.h>

Box::Box(SDL_Renderer* renderer, SDL_Texture* texture, float startingPosX, float startingPosY) {
    // constructor stuff go here
    this->texture = texture;

    x = startingPosX;
    y = startingPosY;
    xvelocity = 0;
    yvelocity = 0;

    this->renderer = renderer;

    lastPhysicsUpdate = 0;
    applyForceJump = false;

}


void Box::applyForceUp(){
    yvelocity = CLICK_VELOCITY;
}

void Box::applyXVelocity(float force){
    xvelocity = force;
}

void Box::applyXYVelocity(float xForce, float yForce){
    xvelocity = xForce;
    yvelocity = yForce;
}

void Box::renderTESTINGONLY(int posx, int posy){
    rect = {posx, posy, BOX_WIDTH, BOX_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void Box::render(int xcam, int ycam){
    rect = {static_cast<int>(round(x)) - xcam, static_cast<int>(round(y) - ycam), BOX_WIDTH, BOX_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void Box::render(){
    rect = {static_cast<int>(round(x)), static_cast<int>(round(y)), BOX_WIDTH, BOX_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_RenderDrawPoint(renderer, x, y);
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

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

    int xold = x;
    int yold = y;

    y += yvelocity * dt;
    x += xvelocity * dt;

    Tile* tile; // look into this, if its leaking memory

    for(int i=0; i<tiles.size(); i++){
        tile = &tiles[i];

        if(tile->isColliding(this)){
            if(tile->finish){
                printf("Finish reached.");
                yvelocity = -1500;
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

                if(xDistance < yDistance){
                    // fix x-axis of box only
                    if(xvelocity > 0){
                        x -= xDistance;
                    } else {
                        x += xDistance;
                    }
                    xvelocity = 0;
                } else if(xDistance > yDistance) {
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
                    // prioritize x for literally no reason
                    if(xvelocity > 0){
                        x -= xDistance;
                    } else {
                        x += xDistance;
                    }
                    xvelocity = 0;
                }
            }
        }
    }

    // Don't allow box to go outside x min/max bounds.
    // if(x > X_MAX_LIMIT){
    //     x = X_MAX_LIMIT;
    // }
    // else if(x < X_MIN_LIMIT){
    //     x = X_MIN_LIMIT;
    // }

    // CHANGE: this should happen on collision detection not hardcoded
    // if(y > 400.0){ // onground
    //     y = 400.0;
    //     yvelocity = 0;
    //     if(xvelocity > 4){
    //         xvelocity += dt * (-X_FRICTION);
    //     } else if(xvelocity < -4){
    //         xvelocity += dt * X_FRICTION;
    //     } else {
    //         xvelocity = 0;
    //     }
    //     //xvelocity = 0; // stop box on land
    // }

    lastPhysicsUpdate = SDL_GetTicks();
}

Box::~Box() {
    // destructor stuff go here
}

