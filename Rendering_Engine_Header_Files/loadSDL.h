#ifndef LOADSDL_H_INCLUDED
#define LOADSDL_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>



bool initWindow (SDL_Window** window, const char* title, int posx, int posy, int width, int height, uint32_t flags);



bool initRenderer (SDL_Renderer** renderer, SDL_Window* window, int driver, uint32_t flags);

bool initSDLImage (uint32_t flags);
bool initialiseTTF();
bool loadFont (TTF_Font** font_ptr, const char* font, int font_size);

#endif // LOADSDL_H_INCLUDED
