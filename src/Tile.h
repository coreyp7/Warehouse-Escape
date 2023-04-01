#pragma once
#include <SDL.h>
class Box;
#include "Box.h"

//
class Tile {
    public:
        // Constructor that sets x y cords of the tile in worldspace.
        Tile(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, bool finish = false);
        Tile();
        ~Tile(); // destructor, cleans up everything for this box.

        // Constructor that sets this box's SDL_Texture field "texture"
        // with an alredy created SDL_Texture.
        //Box(SDL_Texture* boxTexture);
        
        // Render this box at the specified x,y location
        // Abstracts all the SDL calls for rendering stuff for us.
        // ( Will use SDL_RenderCopyEx() )
        void render();

        // Render this Tile relative to the camera's position.
        void render(int xcam, int ycam);

        int getWidth();
        int getHeight();
        float getX();
        float getY();

        // Given (x,y) of Box, returns if it is colliding
        // with this Tile.
        bool isColliding(float x, float y);
        bool isColliding(Box* box);
        // Box's size is hardcoded.
        // static const int BOX_WIDTH = 50;
        // static const int BOX_HEIGHT = 50;
        static const int TILE_WIDTH = 75;
        static const int TILE_HEIGHT = 75;

        // "hardware texture" for this tile
        SDL_Texture* texture;

        float x, y;

        SDL_Rect rect;
        SDL_Renderer* renderer;

        bool finish;
};