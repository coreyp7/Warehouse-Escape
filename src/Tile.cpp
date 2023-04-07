#include "Tile.h"
#include <cmath>

Tile::Tile(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool finish){
    this->renderer = renderer;
    this->texture = texture;
    this->x = x;
    this->y = y;
    this->finish = finish;
}

// render tile relative to camera.
void Tile::render(int xcam, int ycam){
    rect = {static_cast<int>(round(x)) - xcam, static_cast<int>(round(y) - ycam), TILE_WIDTH, TILE_HEIGHT};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

bool Tile::isColliding(Box* box){
    bool collisionX = ((x + TILE_WIDTH) > box->x) 
        && ((box->x + box->BOX_WIDTH) > x);

    bool collisionY = ((y + TILE_HEIGHT) > box->y)
        && ((box->y + box->BOX_HEIGHT) > y);

    return collisionX && collisionY;
}

bool Tile::isColliding(float xpos, float ypos, int width, int height){
    bool collisionX = ((x + TILE_WIDTH) > xpos) && ((xpos + width) > x);
    bool collisionY = ((y + TILE_HEIGHT) > ypos) && ((ypos + height) > y);
    return collisionX && collisionY;
}

Tile::~Tile(){
    // There's no dynamically allocated memory in here.
    // The two pointers in Tile (renderer and texture) are
    // properly cleaned up in main, so don't need to
    // do anything in here.
}