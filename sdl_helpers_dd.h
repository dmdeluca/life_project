#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <string.h>


/*Drawing utility declarations*/
extern void		SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color);
extern void		SDL_DrawTextAtXYStretched(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color);
extern void		SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow);
extern void		SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int h, int w, SDL_Color color);
extern void		SDL_DrawFillRectHelperColorless(SDL_Renderer *pRenderer, int x, int  y, int w, int h);
extern void		SDL_DrawFillRectRoundedHelper(SDL_Renderer *pRenderer, int x, int y, int w, int h, SDL_Color pColor, int radius);

