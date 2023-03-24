#include "Box.h"

Box::Box(SDL_Renderer* renderer, SDL_Texture* texture) {
    // constructor stuff go here
    this->texture = texture;

    x = 0;
    y = 0;
    xvelocity = 0;
    yvelocity = 0;

    this->renderer = renderer;
}

void Box::render(int posx, int posy){
    rect = {posx, posy, BOX_WIDTH, BOX_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

Box::~Box() {
    // destructor stuff go here
}

int Box::getWidth(){
    return BOX_WIDTH;
}

int Box::getHeight(){
    return BOX_HEIGHT;
}

