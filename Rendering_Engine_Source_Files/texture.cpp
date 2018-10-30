#include "texture.h"

void Texture::free(){

    SDL_DestroyTexture(texture);
    width = 0;
    height = 0;
}

void Texture::setBlendMode (SDL_BlendMode blendMode){

    if (texture)
        SDL_SetTextureBlendMode(texture,blendMode);
}

void Texture::setAlphaTransparency (uint8_t a){

    SDL_SetTextureAlphaMod(texture,a);
}

int Texture::width_size(){
    return width;
}

int Texture::height_size(){

    return height;
}

bool Texture::loadImage (SDL_Renderer* renderer, char* path, bool transparency, uint8_t r, uint8_t g, uint8_t b ){

    free();
	
	//Load an image
    SDL_Surface* temp = IMG_Load (path);
	
	//This isn't required in this program, but it adds a tinge of a certain colour
    if (transparency){
        SDL_SetColorKey (temp,SDL_TRUE,SDL_MapRGB(temp->format,r,g,b));
    }

    if (!temp)
        return false;
	
	//Store width and height of an image
    width = temp->w;
    height = temp->h;
	
	//Convert SDL_Surface to SDL_Texture
    texture = SDL_CreateTextureFromSurface (renderer,temp);

    if (!texture)
        return false;

    return true;

}

bool Texture::loadText(TTF_Font* font_name, const char* text, SDL_Colour colour, SDL_Renderer* renderer){

    free();
	//Loads a text that can be rendered
    SDL_Surface* temp = TTF_RenderUTF8_Blended(font_name,text,colour);

    if (!temp)
        return false;

	//Store width and height of text
	//This won't be easily available when its converted into a texture
    width = temp -> w;
    height = temp -> h;
	
	//Convert text to SDL_Texture so text can be rendered on a renderer
    texture = SDL_CreateTextureFromSurface (renderer, temp);
	
	//Free the temporary SDL_Surface*
	/**Note: I don't use surfaces, as texture rendering is often faster**/
    SDL_FreeSurface(temp);

    if (!texture)
        return false;


    return true;
}

void  Texture::render (SDL_Renderer* renderer, SDL_Rect* screenRect, SDL_Rect* textureRect){
	
	//Render texture. Fourth argument is the amount of the texture to consider for rendering
	//Passing null implies the entire texture. 
	//Third argument is the amount of the screen to take up when rendering texture
	//This means a texture can be resized
    SDL_RenderCopy (renderer,texture,textureRect,screenRect);
}

Texture::~Texture(){
    free();
}

