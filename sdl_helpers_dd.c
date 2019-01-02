#include "sdl_helpers_dd.h"

/*Helper function for drawing rectangles.*/
void SDL_DrawFillRectHelper(SDL_Renderer *pRenderer, int x, int  y, int w, int h, SDL_Color pColor)
{
	SDL_Rect rect;
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;
	SDL_SetRenderDrawColor(
		pRenderer,
		pColor.r,
		pColor.g,
		pColor.b,
		pColor.a
	);
	SDL_RenderFillRect(pRenderer, &rect);
}

/*Helper function for drawing rectangles.*/
void SDL_DrawFillRectHelperColorless(SDL_Renderer *pRenderer, int x, int  y, int w, int h)
{
	SDL_Rect rect;
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;
	SDL_RenderFillRect(pRenderer, &rect);
}

void SDL_DrawFillCircleHelper(SDL_Renderer *pRenderer, int x, int y, int r)
{
	int r_squared = r * r;
	for ( int i = -r - 1; i < r + 1; i++ )
	{
		for ( int j = -r - 1; j < r + 1; j++ )
		{
			if ( i * i + j * j < r_squared )
			{
				SDL_RenderDrawPoint(pRenderer, x + i, y + j);
			}
		}
	}
}

void SDL_DrawFillRectRoundedHelper(SDL_Renderer *pRenderer, int x, int y, int w, int h, SDL_Color pColor, int radius)
{
	SDL_SetRenderDrawColor(
		pRenderer,
		pColor.r,
		pColor.g,
		pColor.b,
		pColor.a
	);
	SDL_DrawFillRectHelperColorless(pRenderer, x + radius, y, w - ( radius * 2 ), h);
	SDL_DrawFillRectHelperColorless(pRenderer, x, y + radius, w, h - ( radius * 2 ));
	SDL_DrawFillCircleHelper(pRenderer, x + radius, y + radius, radius);
	SDL_DrawFillCircleHelper(pRenderer, x + radius, y + h - radius, radius);
	SDL_DrawFillCircleHelper(pRenderer, x + w - radius, y + radius, radius);
	SDL_DrawFillCircleHelper(pRenderer, x + w - radius, y + h - radius, radius);
}

/* WRAPPER Draw text on the screen*/
void SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color)
{
	int w, h;
	TTF_SizeText(font, string, &w, &h);
	SDL_DrawTextAtXYStretched(renderer, string, font, x, y, w, h, color);
}

/* Draw text on the screen stretched to a certain width w and height h in pixels.*/
void SDL_DrawTextAtXYStretched(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color)
{

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, string, color);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
						//TTF_SizeText(font, string, &Message_rect.w, &Message_rect.h);
	Message_rect.w = w;
	Message_rect.h = h;

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	SDL_DestroyTexture(Message);
	SDL_FreeSurface(surfaceMessage);
}

void SDL_DrawTextAtXYStretchedColorShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color, SDL_Color shadowColor, int offset)
{
	/*Draw shadow*/
	SDL_DrawTextAtXYStretched(renderer, string, font, x + offset, y + offset, w, h, shadowColor);
	/*Draw foreground*/
	SDL_DrawTextAtXYStretched(renderer, string, font, x, y, w, h, color);
}

/*Draw text on the screen with an optional shadow.*/
void	SDL_DrawTextShadow(
	SDL_Renderer * renderer,
	const char * string,
	TTF_Font * font,
	int x, int y,
	SDL_Color color,
	int doShadow)
{
	if ( doShadow )
	{
		SDL_Color ShadowColor = { 0,0,0,128 };
		SDL_DrawTextAtXY(renderer, string, font, x + 2, y + 2, ShadowColor);
	}
	SDL_DrawTextAtXY(renderer, string, font, x, y, color);
}
