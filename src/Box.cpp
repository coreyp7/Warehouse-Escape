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

void Box::simulatePhysics(float dt){
    yvelocity += dt * GRAVITY;
    y += yvelocity * dt;
    x += xvelocity * dt;

    if(x < 0){
        x = 0;
    }
    if(x > X_LIMIT){
        x = X_LIMIT;
    }

    // CHANGE: this should happen on collision detection not hardcoded
    if(y > 400.0){
        y = 400.0;
        xvelocity = 0; // stop box on land
    }

    lastPhysicsUpdate = SDL_GetTicks();
}

void Box::applyForceUp(){
    yvelocity = CLICK_VELOCITY;
}

void Box::applyXVelocity(float force){
    xvelocity = force;
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

Box::~Box() {
    // destructor stuff go here
}

