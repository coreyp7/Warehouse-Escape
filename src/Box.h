#pragma once
#include <SDL.h>


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
    Box(); // this one will probably be ignored
    ~Box(); // destructor, cleans up everything for this box.

    // Constructor that sets this box's SDL_Texture field "texture"
    // with an alredy created SDL_Texture.
    Box(SDL_Texture* boxTexture);
    
    // Render this box at the specified x,y location
    // Abstracts all the SDL calls for rendering stuff for us.
    // ( Will use SDL_RenderCopyEx() )
    void render(int x, int y);

    

private:
    // Box's size is hardcoded.
    static const int BOX_WIDTH = 50;
    static const int BOX_HEIGHT = 50;

    // The box's current x,y position on screen.
    // Recall: y will be backwards, since origin
    // is top left of window.
    int positionX, positionY;

    // "hardware texture" for this box. Obtained from constructor.
    SDL_Texture* texture;


};