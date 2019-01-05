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
#include "life.h"

/*Constants*/
#define _NROWS 100
#define _NCOLS 100
#define _MAX_VALUE 1
#define _STRONG_THRESHOLD 8
#define _GRADUALLY_INCREASE_COLOR 0
#define _CHANCE_OF_MIRACLE 1000000000000000
#define _HAND_OF_GOD_SIZE 2
#define _PHI 0.61803398875
#define _PI 3.14159265359

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

	/*Set up game settings object*/
	LifeSetting game_settings;
	LS_SetBasicRules(&game_settings, 2, 3, 3, 1);
	LS_SetDisplayRules(&game_settings, 200, 200, 400, 400, 1, 1);
	LS_SetLiveCountPtr(&game_settings,&LIVE_COUNT);
	LS_SetChanceOfMiracle(&game_settings, 100000);
	LS_SetHandOfGodSize(&game_settings, 5);
	LS_SetDotStartValue(&game_settings, 1);
	LS_SetDotColor(&game_settings,cWhite);
	LS_SetSoilColor(&game_settings,cWhite);
	LS_SetSoilValLimit(&game_settings, 255);

	//	Begin loop.
	while ( gameRunning )
	{

		sessionRunning = 1;

		SDL_Init(SDL_INIT_VIDEO);

		srand((unsigned int)time(NULL));

		SDL_Window *window;
		SDL_Renderer *renderer;
		TTF_Font *font;
		TTF_Font *smaller_font;
		TTF_Font *title_font;

		//	Set up the window and renderer.
		window = SDL_CreateWindow(
			"The Game of Life - Alternate Version by David DeLuca",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			game_settings.SCREEN_WIDTH, game_settings.SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_UTILITY //| SDL_WINDOW_ALWAYS_ON_TOP
		);

		/* Check for null window. */
		if ( !window )
		{
			SDL_Quit( );
			exit(1);
		}

		/* Create renderer, check for null renderer. */
		renderer = SDL_CreateRenderer(
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
		font = TTF_OpenFont("Adore64.ttf", 16);
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

		smaller_font = TTF_OpenFont("Adore64.ttf", 8);
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

		title_font = TTF_OpenFont("Adore64.ttf", 24);
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

		int array_size = game_settings.N_ROWS*game_settings.N_COLS;


		Dot * mainLifeDotArray = Life_CreateDotArray(array_size);
		Dot * soilDotArray = Life_CreateDotArray(array_size);
		Dot * updatedLifeDotArray = Life_CreateDotArray(array_size);

		/*
		
		Dot * mainLifeDotArray = ( Dot* ) malloc(sizeof(Dot)*array_size);
		Dot * soilDotArray = ( Dot* ) malloc(sizeof(Dot)*array_size);
		Dot * updatedLifeDotArray = ( Dot* ) malloc(sizeof(Dot)*array_size);
		
		*/

		

		Life_RandomizeDots(mainLifeDotArray, array_size, 3, game_settings);
		Life_SetAllDots(soilDotArray, array_size, 128);

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
							LS_SetUPThresh(&game_settings, npUnderpopulationThreshold->currentValue);
							LS_SetOPThresh(&game_settings, npOverpopulationThreshold->currentValue);
							LS_SetNHSize(&game_settings, npNeighborhoodSize->currentValue);
							LS_SetRBNumber(&game_settings, npRebirthThreshold->currentValue);

/*
							game_settings.UP_THRESH = ;
							game_settings.OP_THRESH = npOverpopulationThreshold->currentValue;
							game_settings.

							UNDERPOP_NUMBER = npUnderpopulationThreshold->currentValue;
							OVERPOP_NUMBER = npOverpopulationThreshold->currentValue;
							FRIEND_RANGE = npNeighborhoodSize->currentValue;
	
	REBIRTH_NUMBER = npRebirthThreshold->currentValue;
	*/
							DELAY_VALUE = (int)(( double ) 1000 / npFrameRate->currentValue);
							if ( cbAntFarmEnable->state == CB_CHECKED )
							{
								LS_EnableAntFarm(&game_settings);
							}
							else
							{
								LS_DisableAntFarm(&game_settings);
							}
							if ( cbEnableColorGlow->state == CB_CHECKED )
							{
								LS_EnableGlow(&game_settings);
							}
							else
							{
								LS_DisableGlow(&game_settings);
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

			Life_RandomizeDots(mainLifeDotArray, array_size, 2, game_settings);

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
						Life_PutHandOfGodAtXY(mouse_x, mouse_y, mainLifeDotArray, game_settings);
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
				Life_IterateGeneration(mainLifeDotArray, updatedLifeDotArray,game_settings);
				if ( game_settings.ANTFARM )
					Life_UpdateSoil(mainLifeDotArray, soilDotArray,game_settings);

				/*Update the button(s)*/
				SDL_UpdateButton(mainMenuButton);

				/* Clear screen.*/
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);

				/*Draw everything*/
				if ( game_settings.ANTFARM )
					Life_DrawSoil(soilDotArray, renderer, game_settings);
				Life_DrawAllDots(mainLifeDotArray, renderer, game_settings);

				//	Draw text over the screen.
				char displayText[ 100 ];
				if ( timer[ 0 ] > ( double ) 1000 / DELAY_VALUE / 2 )
				{
					LIVE_COUNT_DISPLAY = *game_settings.LIVE_COUNT_PTR;
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

		/*Free arrays*/
		Life_DestroyDotArray(mainLifeDotArray);
		Life_DestroyDotArray(updatedLifeDotArray);
		Life_DestroyDotArray(soilDotArray);

		/*Quit/deallocate everything.*/
		SDL_DestroyInterface(pIf);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit( );
		SDL_DestroyButton(mainMenuButton);

		/*Close fonts*/
		if ( font )
			TTF_CloseFont(font);
		if ( smaller_font )
			TTF_CloseFont(smaller_font);
		if ( title_font )
			TTF_CloseFont(title_font);
		TTF_Quit( );
	}
	return 0;
}