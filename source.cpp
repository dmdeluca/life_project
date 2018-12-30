#include <iostream>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <SDL_ttf.h>

using namespace std;

#define ROWS 200
#define COLS 200
#define MAX_VALUE 1
#define DELAY_VALUE 1
#define STRONG_THRESHOLD 8
#define GLOW 1
#define GRADUALLY_INCREASE_COLOR 0
#define CHANCE_OF_MIRACLE 1000000000000000
#define HAND_OF_GOD_SIZE 2

static const int SCREEN_WIDTH = 400;
static const int SCREEN_HEIGHT = 400;
int live_count = 0;

struct dot
{
	int value;
	SDL_Color color;
}; typedef struct dot Dot;

struct friend_map
{
	int size;
	double count;
	Dot * dots;
}; typedef struct friend_map FriendMap;

typedef struct
{
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct
{
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

int overpopulation_number;
int underpopulation_number;
int rebirth_number;
int FRIEND_RANGE;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

//utility function declarations
int		getIntBounded(int, int, int);
void	clearKeyboardBuffer(void);
void	Life_RandomizeDots(Dot * dots, int nIndices, int max);
void	Life_IterateGeneration(Dot * dots, Dot * new_dots);
void	Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots);
void	Life_UpdateDotValues(Dot * dots, Dot * new_dots);
void	Life_DrawAllDots(Dot * dots, SDL_Renderer * renderer);
void	Life_DrawDot(Dot * dots, int i, SDL_Renderer * renderer);
Dot 	Life_CalculateNewDotValue(Dot *dots, int i);
void	Life_ActivateDotByIndex(int i, Dot * dots);
void	Life_KillDotByIndex(int i, Dot * dots);
int		Life_GetIndexByXY(int x, int y, int screenw, int screenh, int cols_per_row);
void	SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color);
void	SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow);
int		SDL_ColorGetHue(SDL_Color color);
int		getMax(int num, ...);
int		getMin(int num, ...);
void	SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int h, int w, SDL_Color color);
//int		has_strong_friend(int * dots, int index, int row_length, int array_size);
SDL_Color averageColor(int, ...);
SDL_Color randomColor(void);
FriendMap * Life_CreateFriendMap(Dot * dots, int i, int row_length, int array_size);
void	Life_FreeFriendMap(FriendMap* fmap);
void	Life_PutHandOfGodAtXY(int x, int y, Dot * dots);
void	SDL_ColorBoostSaturation(SDL_Color * pColor, double mAmount);
void	SDL_ColorBoostValue(SDL_Color * pColor, double mAmount);

int main(int argc, char * argv[ ])
{

	bool running = true, session_running = true;

	//	Begin loop.
	while ( running )
	{

		printf("Welcome to a variation of Life by David DeLuca. \n");
		printf("Enter overpopulation number: ");
		overpopulation_number = getIntBounded(0, 100, 1);
		printf("Enter underpopulation number: ");
		underpopulation_number = getIntBounded(0, 100, 1);
		printf("Enter rebirth number: ");
		rebirth_number = getIntBounded(0, 100, 1);
		printf("Enter friend range number: ");
		FRIEND_RANGE = getIntBounded(0, 10, 1);


		if (
			overpopulation_number < 1 ||
			underpopulation_number < 1 ||
			rebirth_number < 1 ||
			FRIEND_RANGE < 1
			)
		{
			session_running = false;
			running = false;
		}
		else
		{
			session_running = true;
		}

		SDL_Init(SDL_INIT_VIDEO);

		srand(time(NULL));

		SDL_Event event;

		Dot dots[ ROWS*COLS ];

		Dot new_dots[ ROWS*COLS ];
		Life_RandomizeDots(dots, ROWS*COLS, MAX_VALUE + 1);

		//	Set up the window and renderer.
		SDL_Window *window = SDL_CreateWindow(
			"The Game of Life - Alternate Version by David DeLuca",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL
		);

		/* Check for null window. */
		if ( !window )
		{
			SDL_Quit( );
			exit(1);
		}

		/* Create renderer, check for null renderer. */
		SDL_Renderer *renderer = SDL_CreateRenderer(
			window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);

		if ( !renderer )
		{
			SDL_DestroyWindow(window);
			SDL_Quit( );
			exit(2);
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		int ttf_success = TTF_Init( );

		TTF_Font *font = TTF_OpenFont("Adore64.ttf", 16);
		if ( !font )
		{
			printf("error was %s\n", TTF_GetError( ));
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit( );
			exit(3);
		}

		//	Declare mouse tracking variables.
		int mouse_x, mouse_y;
		int key_released = 0;

		while ( session_running && running )
		{
//	Go through the life session.


			time_t start = time(NULL);

			SDL_PollEvent(&event);

			if ( event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN )
			{
				if ( event.button.clicks == SDL_BUTTON_LEFT )
				{
					session_running = false;
				}
			}

			Life_IterateGeneration(dots, new_dots);

			if ( event.type == SDL_MOUSEMOTION )
			{
				SDL_GetMouseState(&mouse_x, &mouse_y);
				Life_PutHandOfGodAtXY(mouse_x, mouse_y, dots);
			}

			//	draw everything
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			//	draw dots
			Life_DrawAllDots(dots, renderer);
			//draw(dots2, renderer);

			//	Draw text over the screen.
			SDL_Color White = { 255,255,255 };
			char displayText[ 100 ];
			sprintf_s(displayText, "live cells: %d", live_count);

			SDL_DrawTextShadow(renderer, displayText, font, 2, 2, White, 1);

			SDL_RenderPresent(renderer);

			SDL_Delay(DELAY_VALUE - ( time(NULL) - start ));
		}

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit( );
		TTF_Quit( );

	}



	return 0;
}

/*
get an integer from the user within certain bounds
@param lower_bound
the lowest acceptable integer
@param upper_bound
the highest acceptable integer
@param inclusive
true or false, whether the endpoints are themselves acceptable
*/
int getIntBounded(int lower_bound, int upper_bound, int inclusive)
{
	int valid_input = 0;
	int user_generated_integer;
	while ( !valid_input )
	{
		valid_input = scanf_s("%d", &user_generated_integer);
		if ( !valid_input )
		{
			clearKeyboardBuffer( );
			printf("Sorry, that's not a valid input. Try again: ");
		}
		else
		{
			 //check for out-of-bounds inclusive
			if ( inclusive && ( ( user_generated_integer > upper_bound || user_generated_integer < lower_bound ) ) )
			{
				printf("Sorry, that's out of bounds [%d-%d].", lower_bound, upper_bound);
				//enable next iteration
				valid_input = 0;
				//check for out-of-bounds exclusive
			}
			else if ( !inclusive && ( ( user_generated_integer >= upper_bound || user_generated_integer <= lower_bound ) ) )
			{
				printf("Sorry, that's out of bounds (%d-%d).", lower_bound, upper_bound);
				//enable next iteration
				valid_input = 0;
			}
		}
	}
	clearKeyboardBuffer( );
	return user_generated_integer;
}

/*clear the Keyboard Buffer*/
void clearKeyboardBuffer(void)
{
	char ch = 'a';
	while ( ch != '\n' )
	{
		scanf_s("%c", &ch);
	}
}

/*Draw text on the screen*/
void SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color)
{

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, string, color);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	TTF_SizeText(font, string, &Message_rect.w, &Message_rect.h);

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	SDL_DestroyTexture(Message);
	SDL_FreeSurface(surfaceMessage);
}

void SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow)
{

	if ( doShadow )
	{
		SDL_Color ShadowColor = { 0,0,0,128 };
		SDL_DrawTextAtXY(renderer, string, font, x + 2, y + 2, ShadowColor);
	}
	SDL_DrawTextAtXY(renderer, string, font, x, y, color);

}

void Life_PutHandOfGodAtXY(int x, int y, Dot * dots)
{

	int index = Life_GetIndexByXY(x, y, SCREEN_WIDTH, SCREEN_HEIGHT, COLS);
	for ( int i = 0; i < HAND_OF_GOD_SIZE*HAND_OF_GOD_SIZE; i++ )
	{
		Life_ActivateDotByIndex(getMin(2, index + i / HAND_OF_GOD_SIZE * COLS + i % HAND_OF_GOD_SIZE, ROWS*COLS - 1), dots);
	}
}

int Life_GetIndexByXY(int x, int y, int screenw, int screenh, int cols_per_row)
{
	return ( int ) ceil(x / ( double ) SCREEN_WIDTH*( COLS )) + ( int ) ceil(y / ( double ) SCREEN_HEIGHT*( ROWS ))*COLS;
}

void Life_ActivateDotByIndex(int i, Dot * dots)
{
	dots[ i ].value = 1;
	live_count++;
}
void Life_KillDotByIndex(int i, Dot * dots)
{
	dots[ i ].value = 0;
	live_count--;
}

//	copied from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

hsv rgb2hsv(rgb in)
{

	hsv         out;
	double      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min < in.b ? min : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max > in.b ? max : in.b;

	out.v = max;                                // v
	delta = max - min;
	if ( delta < 0.00001 )
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if ( max > 0.0 )
	{ // NOTE: if Max is == 0, this divide would cause a crash
		out.s = ( delta / max );                  // s
	}
	else
	{
	 // if max is 0, then r = g = b = 0              
	 // s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if ( in.r >= max )                           // > is bogus, just keeps compilor happy
		out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	else
		if ( in.g >= max )
			out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if ( out.h < 0.0 )
		out.h += 360.0;

	return out;
}


rgb hsv2rgb(hsv in)
{

	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if ( in.s <= 0.0 )
	{       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if ( hh >= 360.0 )
		hh = 0.0;
	hh /= 60.0;
	i = ( long ) hh;
	ff = hh - i;
	p = in.v * ( 1.0 - in.s );
	q = in.v * ( 1.0 - ( in.s * ff ) );
	t = in.v * ( 1.0 - ( in.s * ( 1.0 - ff ) ) );

	switch ( i )
	{
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

SDL_Color averageColor(int num, ...)
{
	va_list vl;
	SDL_Color return_color;
	int sumR = 0, sumG = 0, sumB = 0;
	va_start(vl, num);
	for ( int i = 0; i < num; i++ )
	{
		SDL_Color c = va_arg(vl, SDL_Color);
		sumR += c.r;
		sumB += c.b;
		sumG += c.g;
	}
	va_end(vl);
	return_color.r = sumR / num;
	return_color.g = sumG / num;
	return_color.b = sumB / num;
	return return_color;
}

SDL_Color randomColor( )
{
	SDL_Color c;
	c.a = 255;
	c.r = rand( ) % 255;
	c.g = rand( ) % 255;
	c.b = rand( ) % 255;
	return c;
}


void Life_RandomizeDots(Dot * dots, int nIndices, int max)
{
	for ( int i = 0; i < nIndices; i++ )
	{
		dots[ i ].value = rand( ) % max;
		if ( dots[ i ].value )
			live_count++;
		dots[ i ].color = randomColor( );
	}
}

int	getMax(int num, ...)
{
	va_list vl;
	va_start(vl, num);
	int max_num = va_arg(vl, int), n;
	for ( int i = 1; i < num; i++ )
	{
		n = va_arg(vl, int);
		if ( n > max_num )
		{
			max_num = n;
		}
	}
	va_end(vl);
	return max_num;
}

int	getMin(int num, ...)
{
	va_list vl;
	va_start(vl, num);
	int min_num = va_arg(vl, int), n;
	for ( int i = 1; i < num; i++ )
	{
		n = va_arg(vl, int);
		if ( n < min_num )
		{
			min_num = n;
		}
	}
	va_end(vl);
	return min_num;
}


void Life_IterateGeneration(Dot * dots, Dot * new_dots)
{
	Life_CalculateAllNewDotValues(dots, new_dots);
	Life_UpdateDotValues(dots, new_dots);
}

void Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots)
{
	Dot new_dot;
	for ( int i = 0; i < ROWS*COLS; i++ )
	{
		new_dot = Life_CalculateNewDotValue(dots, i);
		new_dots[ i ].value = new_dot.value;
		new_dots[ i ].color = new_dot.color;
	}
}

Dot Life_CalculateNewDotValue(Dot * dots, int i)
{
	Dot new_dot = dots[ i ];
	int new_value = dots[ i ].value;
	SDL_Color new_color = dots[ i ].color;
	FriendMap * fmap = Life_CreateFriendMap(dots, i, COLS, ROWS*COLS);
	int friends = fmap->count;
	if ( new_value )
	{
		if ( friends < underpopulation_number || friends > overpopulation_number )
		{
			new_dot.value = 0;
			live_count--;
		}
		else
		{
			new_dot.value = 1;
			rgb c1 = { new_dot.color.r,new_dot.color.g,new_color.b };
			hsv c2 = rgb2hsv(c1);
			c2.s -= 0.001;
			c2.v -= 0.001;
			c1 = hsv2rgb(c2);
			new_dot.color.r = c1.r;
			new_dot.color.g = c1.g;
			new_dot.color.b = c1.b;

			//	CELLS DIE DUE TO OLD AGE
			if ( c2.v < 0.05 || c2.s < 0.05 )
			{
				new_dot.value = 0;
				live_count--;
			}
		}
	}
	else
	{
		if ( friends == rebirth_number )
		{
			new_dot.value = 1;
			live_count++;
			SDL_Color *friend_colors = ( SDL_Color* ) malloc(rebirth_number * sizeof(SDL_Color));
			int friend_c_index = 0;
			for ( int k = 0; k < fmap->size && friend_c_index < rebirth_number; k++ )
			{
				if ( fmap->dots[ k ].value )
				{
					friend_colors[ friend_c_index ] = fmap->dots[ k ].color;
					friend_c_index++;
				}
			}
			free(friend_colors);
			/*Not sure how to do this*/
			switch ( rebirth_number )
			{
			case 1:
				new_dot.color = friend_colors[ 0 ];
				break;
			case 2:
				new_dot.color = averageColor(
					rebirth_number,
					friend_colors[ 0 ],
					friend_colors[ 1 ]
				);
				break;
			case 3:
				new_dot.color = averageColor(
					rebirth_number,
					friend_colors[ 0 ],
					friend_colors[ 1 ],
					friend_colors[ 2 ]
				);				break;
			case 4:
				new_dot.color = averageColor(
					rebirth_number,
					friend_colors[ 0 ],
					friend_colors[ 1 ],
					friend_colors[ 2 ],
					friend_colors[ 3 ]
				);				break;
			case 5:
				new_dot.color = averageColor(
					rebirth_number,
					friend_colors[ 0 ],
					friend_colors[ 1 ],
					friend_colors[ 2 ],
					friend_colors[ 3 ],
					friend_colors[ 4 ]
				);				break;

			case 6:
			case 7:
			case 8:
			case 9:
			default:
				new_dot.color = averageColor(
					rebirth_number,
					friend_colors[ 0 ],
					friend_colors[ 1 ],
					friend_colors[ 2 ],
					friend_colors[ 3 ],
					friend_colors[ 4 ],
					friend_colors[ 5 ]
				);				break;
			}
			new_dot.color = averageColor(3, friend_colors[ 0 ], friend_colors[ 1 ], friend_colors[ 2 ]);
			SDL_ColorBoostSaturation(&new_dot.color, 0.1);
			SDL_ColorBoostValue(&new_dot.color, 0.1);
		}
		else if ( rand( ) % ( CHANCE_OF_MIRACLE + 1 ) < CHANCE_OF_MIRACLE - 1 )
		{
			new_dot.value = 0;
			//live_count--;
		}
		else
		{
			new_dot.value = 1;
			live_count++;
			new_dot.color = randomColor( );
		}
	}
	Life_FreeFriendMap(fmap);
	return new_dot;
}

void SDL_ColorBoostSaturation(SDL_Color * pColor, double mAmount)
{
	rgb colorAsRGB = {
		pColor->r / 255.0,
		pColor->g / 255.0,
		pColor->b / 255.0
	};

	/*Convert to HSV and increase S.*/
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	colorAsHSV.s += mAmount;

	/*Limit maximum s value to 1.0 to avoid looping through colors.*/
	if ( colorAsHSV.s > 1 )
		colorAsHSV.s = 1;

	/*Convert back to RGB and assign new RGB values to pColor*/
	colorAsRGB = hsv2rgb(colorAsHSV);
	pColor->r = ( Uint8 ) colorAsRGB.r * 255;
	pColor->g = ( Uint8 ) colorAsRGB.g * 255;
	pColor->b = ( Uint8 ) colorAsRGB.b * 255;

	return;
}

void SDL_ColorBoostValue(SDL_Color * pColor, double mAmount)
{
	rgb colorAsRGB = {
		pColor->r / 255.0,
		pColor->g / 255.0,
		pColor->b / 255.0
	};

	/*Convert to HSV and increase V.*/
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	colorAsHSV.v += mAmount;

	/*Limit maximum v value to 1.0 to avoid looping through colors.*/
	if ( colorAsHSV.v > 1 )
		colorAsHSV.v = 1;

	/*Convert back to RGB and assign new RGB values to pColor*/
	colorAsRGB = hsv2rgb(colorAsHSV);
	pColor->r = ( Uint8 ) colorAsRGB.r * 255;
	pColor->g = ( Uint8 ) colorAsRGB.g * 255;
	pColor->b = ( Uint8 ) colorAsRGB.b * 255;

	return;
}

int SDL_ColorGetHue(SDL_Color color)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) color.r / 255,( double ) color.g / 255,( double ) color.b / 255 });
	return ( int ) colorAsHSV.h * 255;
}

void Life_FreeFriendMap(FriendMap* fmap)
{
	free(fmap->dots);
	free(fmap);
}

FriendMap * Life_CreateFriendMap(Dot * dots, int index, int row_length, int array_size)
{
	FriendMap * fmap = ( FriendMap* ) malloc(sizeof(FriendMap));
	fmap->size = ( 2 * FRIEND_RANGE + 1 )*( 2 * FRIEND_RANGE + 1 );
	/*If friend range is 1, size is 9. If friend range is 2, size is 25*/
	fmap->count = 0;
	fmap->dots = ( Dot* ) malloc(fmap->size * sizeof(Dot));
	int fmap_width = 2 * FRIEND_RANGE + 1;
	for ( int i = 0; i < fmap->size; i++ )
	{
		int offset_index = ( index - FRIEND_RANGE + i % fmap_width + ( i / fmap_width - FRIEND_RANGE )*row_length + array_size ) % array_size;
		if ( i != fmap->size / 2 && dots[ offset_index ].value )
		{
			fmap->dots[ i ] = dots[ offset_index ];
			fmap->count++;
		}
		else
		{
			Dot empty_dot;
			empty_dot.color = { 0,0,0,255 };
			empty_dot.value = 0;
			fmap->dots[ i ] = empty_dot;
		}
	}
	return fmap;
}

/*
int has_strong_friend(int * dots, int index, int row_length, int array_size) {
	int friend_count = 0;
	for ( int i = -FRIEND_RANGE + 1; i < FRIEND_RANGE; i++ ) {
		for ( int j = -FRIEND_RANGE + 1; j < FRIEND_RANGE; j++ ) {
			if ( index + i * row_length + j < 0 || index + i * row_length + j >= array_size ) {
				continue;	//	don't do the rest of the block
			}
			//if ( i == -2 || i == 2 || j == -2 || j == 2 ) {
			//	friend_count += ( dots[ index + i * row_length + j ] )>2;
			//} else {
			if ( dots[ index + i * row_length + j ] > STRONG_THRESHOLD ) {
				return 1;
			}
		}
	}
	return 0;
}
*/


void Life_UpdateDotValues(Dot * dots, Dot * new_dots)
{
	for ( int i = 0; i < ROWS*COLS; i++ )
	{
//calc new value
		dots[ i ].value = new_dots[ i ].value;
		dots[ i ].color = new_dots[ i ].color;
	}
}

void Life_DrawAllDots(Dot * dots, SDL_Renderer * renderer)
{
	for ( int i = 0; i < ROWS*COLS; i++ )
	{
		Life_DrawDot(dots, i, renderer);
	}
}

void Life_DrawDot(Dot * dots, int i, SDL_Renderer * pRenderer)
{

	if ( dots[ i ].value )
	{

		int alpha = 180;

		/*Draw two extra rectangles behind the white part to simulate glow.*/
		if ( GLOW )
		{

		/*Draw a 1x3 rectangle*/
			SDL_Color color = { dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, dots[ i ].color.a / 2 };
			SDL_DrawFillRectHelper(
				pRenderer,
				( SCREEN_WIDTH / COLS )*( ( i - 1 ) % COLS ),
				( SCREEN_HEIGHT / ROWS )*( ( i ) / COLS ),
				SCREEN_WIDTH / COLS * 3,
				SCREEN_HEIGHT / ROWS,
				color
			);

			/*Draw a 3x1 rectangle*/
			SDL_DrawFillRectHelper(
				pRenderer,
				( SCREEN_WIDTH / COLS )*( ( i ) % COLS ),
				( SCREEN_HEIGHT / ROWS )*( ( i ) / COLS - 1 ),
				SCREEN_WIDTH / COLS,
				SCREEN_HEIGHT / ROWS * 3,
				color
			);
		}
		else
		{
			SDL_SetRenderDrawColor(pRenderer, dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, 255 / MAX_VALUE);
		}

		/*Draw the white part of the dot.*/
		SDL_Color color = { 255,255,255,255 };
		SDL_DrawFillRectHelper(
			pRenderer,
			( SCREEN_WIDTH / COLS )*( ( i ) % COLS ),
			( SCREEN_HEIGHT / ROWS )*( i / COLS ),
			SCREEN_WIDTH / COLS,
			SCREEN_HEIGHT / ROWS,
			color
		);

	}

}

void SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int w, int h, SDL_Color color)
{
	SDL_Rect rect;
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;
	SDL_SetRenderDrawColor(
		renderer,
		color.r,
		color.g,
		color.b,
		color.a
	);
	SDL_RenderFillRect(renderer, &rect);
}