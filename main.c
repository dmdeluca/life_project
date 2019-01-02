#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <SDL_ttf.h>
#include <string.h>

/*My header files*/
#include "va_minmax.h"
#include "handycolors.h"
#include "utilities_dd.h"
#include "rgb2hsv.h"
#include "sdl_interface_dd.h"
#include "sdl_color_ops_dd.h"
#include "sdl_helpers_dd.h"

/*Constants*/
#define NROWS 100
#define NCOLS 100
#define MAX_VALUE 1
#define STRONG_THRESHOLD 8
#define GRADUALLY_INCREASE_COLOR 0
#define CHANCE_OF_MIRACLE 1000000000000000
#define HAND_OF_GOD_SIZE 2
#define PHI 0.61803398875
#define PI 3.14159265359

/*Dot Structure*/
typedef struct
{
	int value;
	SDL_Color color;
} Dot;

/*Friend Map Structure*/
typedef struct
{
	int size;
	double count;
	Dot * dotArray;
} FriendMap;

/*Life declarations*/
void		Life_RandomizeDots(Dot * dots, int nIndices, int max);
void		Life_IterateGeneration(Dot * dots, Dot * new_dots);
void		Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots);
void		Life_UpdateDotValues(Dot * dots, Dot * new_dots);
void		Life_DrawAllDots(Dot * dots, SDL_Renderer * renderer);
void		Life_DrawDot(Dot * dots, int i, SDL_Renderer * renderer, int isSoil);
Dot			Life_CalculateNewDotValue(Dot *dots, int i);
void		Life_ActivateDotByIndex(int i, Dot * dots);
void		Life_DrawSoil(Dot * dots, SDL_Renderer * renderer);
void		Life_KillDotByIndex(int i, Dot * dots);
int			Life_GetIndexByXY(int x, int y, int screenw, int screenh, int cols_per_row);
void		Life_PutHandOfGodAtXY(int x, int y, Dot * dots);
void		Life_SetAllDots(Dot * dots, int nIndices, int value);
void		Life_UpdateSoil(Dot * dots, Dot * soil);

/*Friend map declarations*/
FriendMap	*Life_CreateFriendMap(Dot * dots, int i, int row_length, int array_size);
void		Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, int pArraySize);
void		Life_FreeFriendMap(FriendMap* fmap);

/*Global variables*/
SDL_Event				event;
SDL_MouseButtonEvent	mb_event;
SDL_Color				SOIL_COLOR = { 50,50,100,255 };
SDL_Color				DOT_COLOR = { 255,255,255,255 };
int					gameRunning = 1;
int					sessionRunning = 1;
int						OVERPOP_NUMBER;
int						UNDERPOP_NUMBER;
int						REBIRTH_NUMBER;
int						FRIEND_RANGE;
static const int		SCREEN_WIDTH = 400;
static const int		SCREEN_HEIGHT = 400;
int						LIVE_COUNT = 0;
int						LIVE_COUNT_DISPLAY = 0;
static const int		SOIL_VALUE_LIMIT = 255;
int					ANTFARM_ENABLED = 0;
int						DELAY_VALUE = 30;
int						timer[ 5 ] = { 0 };
int						GLOW_ENABLED = 1;


/*Program*/
int main(int argc, char * argv[ ])
{
	//	Begin loop.
	while ( gameRunning )
	{
		sessionRunning = 1;

		SDL_Init(SDL_INIT_VIDEO);

		srand((unsigned int)time(NULL));

		//	Set up the window and renderer.
		SDL_Window *window = SDL_CreateWindow(
			"The Game of Life - Alternate Version by David DeLuca",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_UTILITY //| SDL_WINDOW_ALWAYS_ON_TOP
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

		/*Abort if renderer failed to open.*/
		if ( !renderer )
		{
			SDL_DestroyWindow(window);
			SDL_Quit( );
			exit(2);
		}

		/*Necessary for blending by alpha values.*/
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		/*Initialize TTF and check for null font.*/
		TTF_Init( );
		TTF_Font *font = TTF_OpenFont("Adore64.ttf", 16);
		if ( !font )
		{
			printf("error was %s\n", TTF_GetError( ));
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit( );
			TTF_Quit( );
			exit(3);
		}
		else
		{
			/* Do nothing! */
		}

		TTF_Font *smaller_font = TTF_OpenFont("Adore64.ttf", 8);
		if ( !smaller_font )
		{
			printf("error was: %s\n", TTF_GetError( ));
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			TTF_CloseFont(font);
			TTF_Quit( );
			SDL_Quit( );
			exit(4);
		}
		else
		{
			DEFAULT_BUTTON_FONT = smaller_font;
		}

		TTF_Font *title_font = TTF_OpenFont("Adore64.ttf", 24);
		if ( !title_font )
		{
			printf("error was: %s\n", TTF_GetError( ));
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			TTF_CloseFont(font);
			TTF_Quit( );
			SDL_Quit( );
			exit(4);
		}


		Dot mainLifeDotArray[ NROWS*NCOLS ];
		Dot soilDotArray[ NROWS*NCOLS ];
		Dot updatedLifeDotArray[ NROWS*NCOLS ];
		Life_RandomizeDots(mainLifeDotArray, NROWS*NCOLS, MAX_VALUE + 1);
		Life_SetAllDots(soilDotArray, NROWS*NCOLS, 128);

		//	Declare mouse tracking variables.
		int mouse_x, mouse_y;
		int key_released = 0;
		int onMainMenu = 1;

		SDL_Interface *pIf = SDL_CreateInterfaceAtXY(10, 150, ( int ) ( SCREEN_WIDTH*.9 ), ( int ) ( SCREEN_HEIGHT*.6 ));
		pIf->interfaceColumnNumber = 1;
		pIf->interfaceItemPadding = 3;
		pIf->interfaceLayoutType = IL_LIST;

		/*Create menu items*/
		SDL_NumberPicker * npUnderpopulationThreshold = SDL_CreateNumberPicker(10, 150, 60, 20, 0, 9, font, "Underpopulation Threshold");
		npUnderpopulationThreshold->currentValue = 1;
		SDL_NumberPicker * npFrameRate = SDL_CreateNumberPicker(10, 150, 60, 20, 10, 50, font, "Max Generations Per Second");
		npFrameRate->currentValue = 30;
		npFrameRate->incrementSize = 10;
		SDL_NumberPicker * npOverpopulationThreshold = SDL_CreateNumberPicker(10, 150 + 25 * 1, 60, 20, 0, 9, font, "Overpopulation Threshold");
		npOverpopulationThreshold->currentValue = 1;
		SDL_NumberPicker * npRebirthThreshold = SDL_CreateNumberPicker(10, 150 + 25 * 2, 60, 20, 0, 9, font, "Birth Threshold");
		npRebirthThreshold->currentValue = 1;
		SDL_NumberPicker * npNeighborhoodSize = SDL_CreateNumberPicker(10, 150 + 25 * 3, 60, 20, 1, 5, font, "Neighborhood Size");
		npNeighborhoodSize->currentValue = 1;
		SDL_CustomButton * mainMenuButton = SDL_CreateStandardButton(10, SCREEN_HEIGHT - 25, -1, -1, "<< MAIN MENU");
		mainMenuButton->fadeSpeed = 50;
		SDL_CheckBox * cbAntFarmEnable = SDL_CreateCheckBox(10, 10, 10, 10, CB_UNCHECKED, font, "Enable ant farm");
		SDL_CheckBox * cbEnableColorGlow = SDL_CreateCheckBox(10, 10, 10, 10, CB_UNCHECKED, font, "Teknikolor");
		SDL_CustomButton * quitProgramButton = SDL_CreateStandardButton(SCREEN_WIDTH - 128, SCREEN_HEIGHT - 25, -1, -1, "Exit Program");
		SDL_CustomButton * startButton = SDL_CreateStandardButton(10, SCREEN_HEIGHT - 25, -1, -1, "Start Session");

		/*Add items to interface*/
		SDL_InterfaceAddNumberPicker(pIf, npUnderpopulationThreshold);
		SDL_InterfaceAddNumberPicker(pIf, npOverpopulationThreshold);
		SDL_InterfaceAddNumberPicker(pIf, npRebirthThreshold);
		SDL_InterfaceAddNumberPicker(pIf, npNeighborhoodSize);
		SDL_InterfaceAddCheckBox(pIf, cbAntFarmEnable);
		SDL_InterfaceAddCheckBox(pIf, cbEnableColorGlow);
		SDL_InterfaceAddNumberPicker(pIf, npFrameRate);
		SDL_InterfaceAddButton(pIf, startButton);
		SDL_InterfaceAddButton(pIf, quitProgramButton);

		const int TITLEHEIGHT = 64;
		const int MARGIN = 10;
		const int ELEMENTSPACE = 8;
		const int SUBTITLEHEIGHT = 20;

		while ( gameRunning )
		{
			while ( onMainMenu )
			{
				/* MAIN MENU GUI*/
				/*Get the event.*/
				if ( SDL_PollEvent(&event) )
				{
					/*Track the mouse position.*/
					SDL_GetMouseState(&mouse_x, &mouse_y);
					/*Handle mouseover event*/
					if ( event.type == SDL_MOUSEMOTION )
					{
						SDL_InterfaceHandleMouseover(pIf, mouse_x, mouse_y);
					}
					/*Handle click events*/
					if ( event.type == SDL_MOUSEBUTTONDOWN )
					{
						SDL_NumberPickerHandleClick(npUnderpopulationThreshold, &event);
						SDL_NumberPickerHandleClick(npOverpopulationThreshold, &event);
						SDL_NumberPickerHandleClick(npRebirthThreshold, &event);
						SDL_NumberPickerHandleClick(npNeighborhoodSize, &event);
						SDL_NumberPickerHandleClick(npFrameRate, &event);
						if ( SDL_ButtonClicked(quitProgramButton, &event) )
						{
							gameRunning = 0;
							break;
						}
						if ( SDL_ButtonClicked(startButton, &event) )
						{
							/*Take the values from the number pickers and update our global variables.*/
							UNDERPOP_NUMBER = npUnderpopulationThreshold->currentValue;
							OVERPOP_NUMBER = npOverpopulationThreshold->currentValue;
							FRIEND_RANGE = npNeighborhoodSize->currentValue;
							REBIRTH_NUMBER = npRebirthThreshold->currentValue;
							DELAY_VALUE = (int)(( double ) 1000 / npFrameRate->currentValue);
							if ( cbAntFarmEnable->state == CB_CHECKED )
							{
								ANTFARM_ENABLED = 1;
							}
							else
							{
								ANTFARM_ENABLED = 0;
							}
							if ( cbEnableColorGlow->state == CB_CHECKED )
							{
								GLOW_ENABLED = 1;
							}
							else
							{
								GLOW_ENABLED = 0;
							}
							onMainMenu = 0;
							sessionRunning = 1;
							break;
						}
						SDL_CheckHandleClick(cbAntFarmEnable, &event);
						SDL_CheckHandleClick(cbEnableColorGlow, &event);
					}
				}

				SDL_UpdateInterface(pIf);

				/* Clear screen with cool fade effect.*/
				SDL_DrawFillRectHelper(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cBlack);

				/*Draw words*/
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Conway's", title_font, MARGIN, MARGIN, 100, MARGIN * 2 - 8, cWhite, cGray, 1);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "GAME OF LIFE", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE, SCREEN_WIDTH - MARGIN * 2, TITLEHEIGHT, cWhite, cGray, 4);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "IN FULL TEKNIKOLOR", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE + TITLEHEIGHT, SCREEN_WIDTH - MARGIN * 2, SUBTITLEHEIGHT, cWhite, cGray, 2);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Rules:", font, MARGIN, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 60, 16, cWhite, cGray, 1);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Neighborhood", font, SCREEN_WIDTH - MARGIN - 100, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 100, 16, cWhite, cGray, 1);

				/*Draw menu*/
				SDL_DrawInterface(renderer, pIf);
				SDL_DrawBoxGrid(renderer, SCREEN_WIDTH - MARGIN - 100, 150, 100, 2 * npNeighborhoodSize->currentValue + 1, 3, cGray, cWhite, font);

				SDL_RenderPresent(renderer);
			}

			/* Life session loop */

			Life_RandomizeDots(mainLifeDotArray, NROWS*NCOLS, 2);

			while ( sessionRunning && gameRunning )
			{
				//	Go through the life session.

				time_t start = time(NULL);
				timer[ 0 ]++;

				/*Get the event.*/
				if ( SDL_PollEvent(&event) )
				{
					/*Track the mouse position.*/
					SDL_GetMouseState(&mouse_x, &mouse_y);
					if ( event.type == SDL_MOUSEMOTION )
					{
						SDL_ButtonHandleMouseover(mainMenuButton, mouse_x, mouse_y);
						Life_PutHandOfGodAtXY(mouse_x, mouse_y, mainLifeDotArray);
					}
					/*Handle click events*/
					if ( event.type == SDL_MOUSEBUTTONDOWN )
					{
						if ( SDL_ButtonClicked(mainMenuButton, &event) )
						{
							sessionRunning = 0;
							onMainMenu = 1;
							break;
						}
					}
				}

				/* Update all dots. */
				Life_IterateGeneration(mainLifeDotArray, updatedLifeDotArray);
				if ( ANTFARM_ENABLED )
					Life_UpdateSoil(mainLifeDotArray, soilDotArray);

				/*Update the button(s)*/
				SDL_UpdateButton(mainMenuButton);

				/* Clear screen.*/
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);

				/*Draw everything*/
				if ( ANTFARM_ENABLED )
					Life_DrawSoil(soilDotArray, renderer);
				Life_DrawAllDots(mainLifeDotArray, renderer);

				//	Draw text over the screen.
				char displayText[ 100 ];
				if ( timer[ 0 ] > ( double ) 1000 / DELAY_VALUE / 2 )
				{
					LIVE_COUNT_DISPLAY = LIVE_COUNT;
					timer[ 0 ] = 0;
				}
				sprintf_s(displayText, (size_t)100, "live cells: %d", LIVE_COUNT_DISPLAY);
				SDL_DrawTextShadow(renderer, displayText, font, 2, 2, cWhite, 1);

				SDL_RenderButton(renderer, mainMenuButton);
				SDL_RenderPresent(renderer);
				/*Delay for specified ms.*/
				SDL_Delay(DELAY_VALUE - (Uint32)( time(NULL) - start ));
			}
		}

		/*Quit everything.*/
		SDL_DestroyInterface(pIf);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit( );
		SDL_DestroyButton(mainMenuButton);
		//SDL_DestroyButton(startButton);
		//SDL_DestroyButton(quitProgramButton);
		//SDL_DestroyNumberPicker(npUnderpopulationThreshold);
		//SDL_DestroyNumberPicker(npOverpopulationThreshold);
		//SDL_DestroyNumberPicker(npRebirthThreshold);
		//SDL_DestroyNumberPicker(npNeighborhoodSize);
		TTF_CloseFont(font);
		TTF_CloseFont(smaller_font);
		TTF_Quit( );
	}
	return 0;
}

void Life_UpdateSoil(Dot * pLifeDotArray, Dot * pSoilDotArray)
{
	for ( int i = 0; i < NROWS*NCOLS; i++ )
	{
		if ( pLifeDotArray[ i ].value )
		{
			pSoilDotArray[ i ].value = 0;
		}
		else if ( pSoilDotArray[ i ].value < SOIL_VALUE_LIMIT )
		{
			pSoilDotArray[ i ].value += 1;
		}
	}
}

/* Revive a square at (x,y). */
void Life_PutHandOfGodAtXY(int x, int y, Dot * dots)
{
	int index = Life_GetIndexByXY(x, y, SCREEN_WIDTH, SCREEN_HEIGHT, NCOLS);
	for ( int i = 0; i < HAND_OF_GOD_SIZE*HAND_OF_GOD_SIZE; i++ )
	{
		Life_ActivateDotByIndex(getMin(2, index + i / HAND_OF_GOD_SIZE * NCOLS + i % HAND_OF_GOD_SIZE, NROWS*NCOLS - 1), dots);
	}
}

/*Find the index of a dot in the array by its XY value.*/
int Life_GetIndexByXY(int x, int y, int screenw, int screenh, int cols_per_row)
{
	return ( int ) ceil(x / ( double ) SCREEN_WIDTH*( NCOLS )) + ( int ) ceil(y / ( double ) SCREEN_HEIGHT*( NROWS ))*NCOLS;
}

/*Revive a dot at a certain index in the dot array.*/
void Life_ActivateDotByIndex(int i, Dot * dots)
{
	if ( !dots[ i ].value )
	{
		dots[ i ].value = 1;
		LIVE_COUNT++;
	}
	else
	{
		/*Do nothing!*/
	}
}

/*Kill a dot at a certain index in the dot array.*/
void Life_KillDotByIndex(int i, Dot * dots)
{
	dots[ i ].value = 0;
	LIVE_COUNT--;
}

/*Set all dots in an array of dots to a certain specified value.*/
void Life_SetAllDots(Dot * dots, int nIndices, int value)
{
	for ( int i = 0; i < nIndices; i++ )
	{
		dots[ i ].value = value;
		dots[ i ].color = cWhite;
	}
}

/*Set all dots to a random value between 0 and @param max.*/
void Life_RandomizeDots(Dot * dots, int nIndices, int max)
{
	LIVE_COUNT = 0;
	for ( int i = 0; i < nIndices; i++ )
	{
		dots[ i ].value = rand( ) % max;
		if ( dots[ i ].value )
			LIVE_COUNT++;
		dots[ i ].color = randomColor( );
	}
}

/*Calculate new values for current dot array and then update the values.*/
void Life_IterateGeneration(Dot * dots, Dot * new_dots)
{
	Life_CalculateAllNewDotValues(dots, new_dots);
	Life_UpdateDotValues(dots, new_dots);
}

/*Calculate new values for all dots in the array.*/
void Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots)
{
	Dot new_dot;
	for ( int i = 0; i < NROWS*NCOLS; i++ )
	{
		new_dot = Life_CalculateNewDotValue(dots, i);
		new_dots[ i ].value = new_dot.value;
		new_dots[ i ].color = new_dot.color;
	}
}


/* Calculate new value for a single dot in the array.*/
Dot Life_CalculateNewDotValue(Dot * dots, int i)
{
	/*Capture the dot of interest.*/
	Dot newDot = dots[ i ];
	/*Store the old value and color of the dot.*/
	int oldValue = dots[ i ].value;
	/*Create a friendmap structure of nearby friends so that color averaging is possible later.*/
	FriendMap * fMap = Life_CreateFriendMap(dots, i, NCOLS, NROWS*NCOLS);
	/*Capture number of friends, then evaluate.*/
	int numFriends = (int)round(fMap->count);
	if ( oldValue != 0 ) //	meaning the cell is ALIVE
	{
		if ( numFriends < UNDERPOP_NUMBER || numFriends > OVERPOP_NUMBER )
		{
			/*Cell has died due to under- or overpopulation.*/
			newDot.value = 0;
			LIVE_COUNT--;
		}
		else
		{
			/*Cell remains alive.*/
			newDot.value = 1;

			/*Decrease vitality slowly with age as if due to inbreeding.*/
			SDL_ColorAdjHSV(&newDot.color, 0, -.001, -.001);

			/*Get saturation and value values.*/
			double s = SDL_ColorGetSat(newDot.color);
			double v = SDL_ColorGetVal(newDot.color);

			/*Evaluate for loss of vitality.*/
			if ( v < 0.05 || s < 0.05 )
			{
				/*Cell has died due to loss of vitality.*/
				newDot.value = 0;
				LIVE_COUNT--;
			}
		}
	}
	else
	{
		/*Cell is currently dead.*/
		if ( numFriends == REBIRTH_NUMBER )
		{
			/*Cell is born.*/
			newDot.value = 1;
			LIVE_COUNT++;

			/*Cell becomes the average of its [rebirth_number] parents colors.*/
			SDL_Color *friendColorArray = ( SDL_Color* ) malloc(REBIRTH_NUMBER * sizeof(SDL_Color));
			int friendColorIndex = 0;
			for ( int k = 0; k < fMap->size && friendColorIndex < REBIRTH_NUMBER; k++ )
			{
				if ( fMap->dotArray[ k ].value )
				{
					friendColorArray[ friendColorIndex ] = fMap->dotArray[ k ].color;
					friendColorIndex++;
				}
			}
			/*Not sure how to do this*/
			switch ( REBIRTH_NUMBER )
			{
			case 1:
				newDot.color = friendColorArray[ 0 ];
				break;
			case 2:
				newDot.color = averageColor(
					REBIRTH_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ]
				);
				break;
			case 3:
				newDot.color = averageColor(
					REBIRTH_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ]
				);				break;
			case 4:
				newDot.color = averageColor(
					REBIRTH_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ],
					friendColorArray[ 3 ]
				);				break;
			case 5:
				newDot.color = averageColor(
					REBIRTH_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ],
					friendColorArray[ 3 ],
					friendColorArray[ 4 ]
				);				break;

			case 6:
			case 7:
			case 8:
			case 9:
			default:
				newDot.color = averageColor(
					REBIRTH_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ],
					friendColorArray[ 3 ],
					friendColorArray[ 4 ],
					friendColorArray[ 5 ]
				);				break;
			}
			/*Free resources.*/
			SDL_ColorAdjHSV(&newDot.color, 10, 0.1, 0.1);
			free(friendColorArray);
		}
		else if ( rand( ) % ( CHANCE_OF_MIRACLE + 1 ) < CHANCE_OF_MIRACLE - 1 )
		{
			/*Cell remains dead.*/
			newDot.value = 0;
			newDot.color = randomColor( );
		}
		else
		{
			/*Immaculate conception.*/
			newDot.value = 1;
			LIVE_COUNT++;
			newDot.color = randomColor( );
		}
	}
	/*Free friend map resource and return new dot structure.*/
	Life_FreeFriendMap(fMap);
	return newDot;
}

/*Deallocate memory for friend map and dot array internal to friend map structure.*/
void Life_FreeFriendMap(FriendMap* pFriendMap)
{
	free(pFriendMap->dotArray);
	free(pFriendMap);
}

/*Allocate memory for friend map structure, fill it, and return it.*/
FriendMap * Life_CreateFriendMap(Dot * pDotArray, int pIndex, int pRowLength, int pArraySize)
{
	/*Create the friend map.*/
	FriendMap * fMap = ( FriendMap* ) malloc(sizeof(FriendMap));
	/*Populate the friend map.*/
	Life_FillFriendMap(fMap, FRIEND_RANGE, pDotArray, pIndex, pRowLength, pArraySize);
	/*Return the friend map.*/
	return fMap;
}

void Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, int pArraySize)
{
	pFriendMap->size = ( 2 * pFriendRange + 1 )*( 2 * pFriendRange + 1 );
	/*EG If friend range is 1, size is 9. If friend range is 2, size is 25*/
	/*
	#####
	#####
	##X##
	#####
	#####
	*/
	pFriendMap->count = 0;
	pFriendMap->dotArray = ( Dot* ) malloc(pFriendMap->size * sizeof(Dot));
	int fmap_width = 2 * pFriendRange + 1;
	for ( int i = 0; i < pFriendMap->size; i++ )
	{
		int offset_index = ( pIndex - pFriendRange + i % fmap_width + ( i / fmap_width - pFriendRange )*pRowLength + pArraySize ) % pArraySize;
		if ( i != pFriendMap->size / 2 && pDotArray[ offset_index ].value )
		{
			pFriendMap->dotArray[ i ] = pDotArray[ offset_index ];
			pFriendMap->count++;
		}
		else
		{
			Dot empty_dot;
			empty_dot.color = cBlack;
			empty_dot.value = 0;
			pFriendMap->dotArray[ i ] = empty_dot;
		}
	}
}

/*Update all values for dot array.*/
void Life_UpdateDotValues(Dot * pDotArray, Dot * pUpdateDotArray)
{
	for ( int i = 0; i < NROWS*NCOLS; i++ )
	{
		/* Calculate new values. */
		pDotArray[ i ].value = pUpdateDotArray[ i ].value;
		pDotArray[ i ].color = pUpdateDotArray[ i ].color;
	}
}

/*Draw all dots in dot array.*/
void Life_DrawAllDots(Dot * pDotArray, SDL_Renderer * pRenderer)
{
	for ( int i = 0; i < NROWS*NCOLS; i++ )
	{
		Life_DrawDot(pDotArray, i, pRenderer, 0);
	}
}

/*Draw soil in soil array.*/
void Life_DrawSoil(Dot * soilArray, SDL_Renderer * pRenderer)
{
	for ( int i = 0; i < NROWS*NCOLS; i++ )
	{
		Life_DrawDot(soilArray, i, pRenderer, 1);
	}
}

/*Draw a dot on the screen from a dot or soil array.*/
void Life_DrawDot(Dot * dots, int i, SDL_Renderer * pRenderer, int isSoil)
{

	if ( dots[ i ].value )
	{
		int alpha = ( int ) ( 255 * PHI*PHI ); // #goldenratio?!??

											   /*Draw two extra rectangles behind the white part to simulate glow.*/
		if ( GLOW_ENABLED && !isSoil )
		{
			/*Draw a 1x3 rectangle*/
			SDL_Color color = { dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, alpha };
			SDL_DrawFillRectHelper(
				pRenderer,
				( int ) ( ( double ) SCREEN_WIDTH / NCOLS )*( ( i - 1 ) % NCOLS ),
				( int ) ( ( double ) SCREEN_HEIGHT / NROWS )*( ( i ) / NCOLS ),
				( int ) ( ( double ) SCREEN_WIDTH / NCOLS * 3 ),
				( int ) ( ( double ) SCREEN_HEIGHT / NROWS ),
				color
			);
			/*Draw a 3x1 rectangle*/
			SDL_DrawFillRectHelper(
				pRenderer,
				( int ) ( ( double ) SCREEN_WIDTH / NCOLS )*( ( i ) % NCOLS ),
				( int ) ( ( double ) SCREEN_HEIGHT / NROWS )*( ( i ) / NCOLS - 1 ),
				( int ) ( ( double ) SCREEN_WIDTH / NCOLS ),
				( int ) ( ( double ) SCREEN_HEIGHT / NROWS * 3 ),
				color
			);
		}
		else
		{
			SDL_SetRenderDrawColor(pRenderer, dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, 255 / MAX_VALUE);
		}

		/*Draw the white part of the dot.*/
		SDL_Color color;
		if ( isSoil )
		{
			color = SOIL_COLOR;
			color.a = dots[ i ].value;
		}
		else
		{
			/*Blinding white*/
			color = DOT_COLOR;
		}
		SDL_DrawFillRectHelper(
			pRenderer,
			( int ) ( ( double ) SCREEN_WIDTH / NCOLS )*( ( i ) % NCOLS ),
			( int ) ( ( double ) SCREEN_HEIGHT / NROWS )*( i / NCOLS ),
			( int ) ( ( double ) SCREEN_WIDTH / NCOLS ),
			( int ) ( ( double ) SCREEN_HEIGHT / NROWS ),
			color
		);

	}

}

