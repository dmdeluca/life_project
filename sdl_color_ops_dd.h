#pragma once
#include "SDL.h"
#include "rgb2hsv.h"
#include "va_minmax.h"
#include "handycolors.h"
#include <stdlib.h>

/*Color declarations*/
void		SDL_ColorAdjSat(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjVal(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjHue(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjHSV(SDL_Color * pColor, double mHueAmount, double pSatAmount, double pValAmount);
int			SDL_ColorGetVal(SDL_Color pColor);
int			SDL_ColorGetSat(SDL_Color pColor);
int			SDL_ColorGetHue(SDL_Color pColor);
SDL_Color	averageColor(int, ...);
SDL_Color	randomColor(void);