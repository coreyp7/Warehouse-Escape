#pragma once
#include <SDL.h>
class Tile; // resolves circular dependency
#include "Tile.h"
#include <vector>
using namespace std;


/*
 * So what's our plan?
 * 
 * In main, we'll have a list of all created Box objects that are spawned and exist
 * in the window.
 * 
 * When a user left clicks:
 * - check if there's a box under the mouse
 * - if its under the mouse, then have this box follow the mouse while left click is held
 * 
 * - if user lets go of left click:
 * - release box from mouse and let gravity affect the box again. (hard part)
 * 
 * - if user right clicks, create box.
 * 
 * 
 * OOH, IDEA:
 * if I struggle to get the velocity of the mouse on release to affect the box,
 * i could just make this a primitive game where you click the object while its
 * in the air to have it bounce upwards (easy, just have a variable to specify
 * force to apply on left click.)
 * 
 * This could actually prove to be a simple game to create.
 */
class Box {
    public:
        Box(SDL_Renderer* renderer, SDL_Texture* texture, float startingPosX, float startingPosY);
        ~Box(); // destructor, cleans up everything for this box.
        
        // Render this box at the specified x,y location
        // Abstracts all the SDL calls for rendering stuff for us.
        // ( Will use SDL_RenderCopyEx() )
        // void render();
        // Render this Box relative to the camera's position.
        void render(int xcam, int ycam);
        // void renderTESTINGONLY(int posx, int posy);

        int getWidth();
        int getHeight();
        float getX();
        float getY();
        float getXVelocity();
        float getYVelocity();

        void simulatePhysics(float dt, vector<Tile> &tiles);
        // void applyForceUp();
        // void applyXVelocity(float force);
        void applyXYVelocity(float xForce, float yForce);

        static const int BOX_WIDTH = 75;
        static const int BOX_HEIGHT = 75;

        static constexpr float CLICK_VELOCITY = -500.0;

        static constexpr float GRAVITY = 700.0f; 
        static constexpr float X_FRICTION = 700.0f;

        SDL_Renderer* renderer;
        SDL_Texture* texture;

        float x, y;
        float xvelocity, yvelocity;

        SDL_Rect rect;

        bool completedLevel = false;
};