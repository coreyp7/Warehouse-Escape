#pragma once
#include <SDL.h>
#include <SDL_ttf.h> // for rendering text
#include <string>

/*
An object for easily rendering text to the screen (SDL).
*/
class Text{
    public:
        Text(SDL_Renderer* rend, TTF_Font* customFont, SDL_Color customColor);
        ~Text();

        void render(int x, int y);
        void changeText(std::string newText);
        void setXY(int x, int y);
        int getWidth();
        int getHeight();
        void free();
        

    private:
        SDL_Texture* textTexture;
        TTF_Font* font;
        SDL_Color color;
        SDL_Renderer* renderer;
        std::string text;

        int width;
        int height;

        int x;
        int y;

        SDL_Rect rect;
};