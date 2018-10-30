#include "loadSDL.h"

//Initialises a window, taking into account the following parameters
//uint32_t flags represents anything we want the window to do (such as being displayable on the screen)
//The function takes a pointer to a pointer so it can directly change an SDL_Window pointer
bool initWindow (SDL_Window** window, const char* title, int posx, int posy, int width, int height, uint32_t flags){

    *window = SDL_CreateWindow (title,
                                posx,
                                posy,
                                width,
                                height,
                                flags);

    //Function returns true if window was successfully created, otherwise false
    if (! (*window))
        return false;

    return true;
}

//Function initialises the SDL rendering systems
bool initRenderer (SDL_Renderer** renderer, SDL_Window* window, int driver, uint32_t flags){

    *renderer = SDL_CreateRenderer (window,driver,flags);

    if (! (*renderer))
        return false;

    return true;

}

bool initSDLImage (uint32_t flags){
	/**
		Initialises sub system
		for image handling of non-bmp images
		
		The pause, stop and resume icons are png images
	**/
    if (! (IMG_Init(flags) & flags))
        return false;

    return true;

}

bool initialiseTTF(){

	/**TTF for true type fonts.
	   This initialises the subsystems for that
	   so it can be used with SDL**/
    if (TTF_Init() == -1)
        return false;

    return true;
}

bool loadFont (TTF_Font** font_ptr, const char* font, int font_size){
	//Functions open a specific font after being given a path filebuf
	//TTF must have been initialised
    *font_ptr = TTF_OpenFont (font,font_size);

    if (! (*font_ptr))
        return false;

    return true;
}


