#pragma once
#include <SDL.h>
class Box;
#include "Box.h"

//
class Tile {
    public:
        // Constructor that sets x y cords of the tile in worldspace.
        Tile(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool finish = false);
        ~Tile();

        // Render this Tile relative to the camera's position.
        void render(int xcam, int ycam);

        int getWidth();
        int getHeight();
        float getX();
        float getY();

        // Given (x,y) of Box, returns if it is colliding
        // with this Tile.
        bool isColliding(Box* box);

        static const int TILE_WIDTH = 75;
        static const int TILE_HEIGHT = 75;

        // "hardware texture" for this tile
        SDL_Texture* texture;

        float x, y;

        SDL_Rect rect;
        SDL_Renderer* renderer;

        bool finish;
};