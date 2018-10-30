#ifndef  TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class Texture{

    //The main class for handling textures
	//Textures are used to render images and texts onto a renderer
	
    public:

        //Automatically load a texture
        ~Texture();
        SDL_Texture* texture = nullptr;

        int width_size();
        int height_size();

        bool loadImage (SDL_Renderer* renderer, char* path, bool transparency = false, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
        bool loadText(TTF_Font* font_name, const char* text, SDL_Colour colour, SDL_Renderer* renderer);
        void setBlendMode (SDL_BlendMode blendMode);
        void setAlphaTransparency (uint8_t a);
        void render (SDL_Renderer* renderer, SDL_Rect* screenRect = nullptr, SDL_Rect* textureRect = nullptr);


    private:
        void free();
        int width, height;


};
#endif // TEXTURE_H_INCLUDED
