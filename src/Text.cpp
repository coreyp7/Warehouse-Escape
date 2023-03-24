#include "Text.h"

//TODO: add x y to constructor
Text::Text(SDL_Renderer* rend, TTF_Font* customFont, SDL_Color customColor){
    textTexture = NULL;
    width = -1;
    height = -1;
    x = -1;
    y = -1;

    font = customFont;
    color = customColor;
    renderer = rend;
    
    // initialize texture
    // SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, , color);
    // if(textSurface != NULL){
    //     textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    //     if(textTexture == NULL){
    //         printf("SOMETHING WENT WRONG IN Text::changeText");
    //     }

    //     width = textSurface->w;
    //     height = textSurface->h;
    //     printf("width set to %d", width);

    //     SDL_FreeSurface(textSurface);
    // } else {
    //     printf("NULL");
    // }
}

Text::~Text(){
    free();
}

void Text::free(){
//Free texture if it exists
	if( textTexture != NULL )
	{
		SDL_DestroyTexture( textTexture );
		textTexture = NULL;
		width = -1;
        height = -1;
	}
}

// Changes text of this object.
void Text::changeText(std::string newText){
    
    free();

    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, newText.c_str(), color);
    if(textSurface != NULL){
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(textTexture == NULL){
            printf("SOMETHING WENT WRONG IN Text::changeText");
        }

        width = textSurface->w;
        height = textSurface->h;
        //printf("width set to %d", width);

        SDL_FreeSurface(textSurface);
    }

}

void Text::setXY(int newx, int newy){
    x = newx;
    y = newy;  
}

// Renders this text on renderer.
void Text::render(int posx, int posy){
    rect = {posx, posy, width, height};
    SDL_RenderCopyEx(renderer, textTexture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

int Text::getHeight(){
    return height;
}

int Text::getWidth(){
    return width;
}
