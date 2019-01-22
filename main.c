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

/*Global variables*/
SDL_Event				event;
SDL_MouseButtonEvent	mb_event;
int						gameRunning = 1;
int						sessionRunning = 1;
int						LIVE_COUNT = 0;
int						LIVE_COUNT_DISPLAY = 0;
static const int		SOIL_VALUE_LIMIT = 255;
int						DELAY_VALUE = 30;
int						timer[ 5 ] = { 0 };

/*Program*/
int main(int argc, char * argv[ ])
{

	/*Set up game settings object*/
	LifeSetting game_settings;
	LS_SetBasicRules(&game_settings, 2, 3, 3, 1);
	LS_SetDisplayRules(&game_settings, 200, 200, 400, 400, 1, 1);
	LS_SetLiveCountPtr(&game_settings, &LIVE_COUNT);
	LS_SetChanceOfMiracle(&game_settings, 100000);
	LS_SetHandOfGodSize(&game_settings, 5);
	LS_SetDotStartValue(&game_settings, 1);
	LS_SetDotColor(&game_settings, cWhite);
	LS_SetSoilColor(&game_settings, cCyan);
	LS_SetSoilValLimit(&game_settings, 128);

	sessionRunning = 1;

	SDL_Init(SDL_INIT_VIDEO);

	srand(( unsigned int ) time(NULL));

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
		gameRunning = 0;
	}

	/* Create renderer, check for null renderer. */
	renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	/*Abort if renderer failed to open.*/
	if ( !renderer )
	{
		gameRunning = 0;
	}
	else
	{
		/*Necessary for blending by alpha values.*/
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	}

	/*Initialize TTF and check for null font.*/
	TTF_Init( );
	font = TTF_OpenFont("Adore64.ttf", 16);

	smaller_font = TTF_OpenFont("Adore64.ttf", 8);

	title_font = TTF_OpenFont("Adore64.ttf", 24);
	if ( !title_font || !smaller_font || !font )
	{
		gameRunning = 0;
	}
	else
	{
		DEFAULT_BUTTON_FONT = smaller_font;
	}

	if ( !gameRunning )
		return 0;

	const int TITLEHEIGHT = 64;
	const int MARGIN = 10;
	const int ELEMENTSPACE = 8;
	const int SUBTITLEHEIGHT = 20;

	SDL_Interface *ifMain;
	SDL_NumberPicker * npUPT, *npOPT, *npFR, *npRBT, *npNHS, *npDim;
	SDL_CustomButton * btMainMenu, *btStart, *btQuitProgram;
	SDL_CheckBox * cbAntFarmEnable, *cbEnableColorGlow;

	ifMain = SDL_CreateInterfaceAtXY(10, 150, ( int ) ( game_settings.SCREEN_WIDTH*.9 ), ( int ) ( game_settings.SCREEN_HEIGHT*.6 ));
	ifMain->interfaceColumnNumber = 1;
	ifMain->interfaceItemPadding = 3;
	ifMain->interfaceLayoutType = IL_LIST;

	/*Create menu items*/
	npUPT = SDL_CreateNumberPicker(10, 150, 60, 20, 0, 9, font, "Underpopulation Threshold");
	npDim = SDL_CreateNumberPicker(10, 150, 60, 20, 50, 200, font, "Size of World (n)");
	npDim->currentValue = 50;
	npDim->incrementSize = 10;
	npFR = SDL_CreateNumberPicker(10, 150, 60, 20, 10, 50, font, "Max Generations Per Second");
	npFR->currentValue = 30;
	npFR->incrementSize = 10;
	npOPT = SDL_CreateNumberPicker(10, 150 + 25 * 1, 60, 20, 0, 9, font, "Overpopulation Threshold");
	npOPT->currentValue = 1;
	npRBT = SDL_CreateNumberPicker(10, 150 + 25 * 2, 60, 20, 0, 9, font, "Birth Threshold");
	npRBT->currentValue = 1;
	npNHS = SDL_CreateNumberPicker(10, 150 + 25 * 3, 60, 20, 1, 5, font, "Neighborhood Size");
	npNHS->currentValue = 1;
	btMainMenu = SDL_CreateStandardButton(10, game_settings.SCREEN_HEIGHT - 25, -1, -1, "<< MAIN MENU");
	btMainMenu->fadeSpeed = 50;
	cbAntFarmEnable = SDL_CreateCheckBox(10, 10, 10, 10, CB_UNCHECKED, font, "Enable ant farm");
	cbEnableColorGlow = SDL_CreateCheckBox(10, 10, 10, 10, CB_UNCHECKED, font, "Teknikolor");
	btQuitProgram = SDL_CreateStandardButton(game_settings.SCREEN_WIDTH - 128, game_settings.SCREEN_HEIGHT - 25, -1, -1, "Exit Program");
	btStart = SDL_CreateStandardButton(10, game_settings.SCREEN_HEIGHT - 25, -1, -1, "Start Session");

	/*Add items to interface*/
	SDL_InterfaceAddNumberPicker(ifMain, npUPT);
	SDL_InterfaceAddNumberPicker(ifMain, npOPT);
	SDL_InterfaceAddNumberPicker(ifMain, npRBT);
	SDL_InterfaceAddNumberPicker(ifMain, npNHS);
	SDL_InterfaceAddCheckBox(ifMain, cbAntFarmEnable);
	SDL_InterfaceAddCheckBox(ifMain, cbEnableColorGlow);
	SDL_InterfaceAddNumberPicker(ifMain, npFR);
	SDL_InterfaceAddNumberPicker(ifMain, npDim);
	SDL_InterfaceAddButton(ifMain, btStart);
	SDL_InterfaceAddButton(ifMain, btQuitProgram);

	//	Declare mouse tracking variables.
	int mouse_x, mouse_y;
	int key_released = 0;
	int onMainMenu = 1;

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
					SDL_InterfaceHandleMouseover(ifMain, mouse_x, mouse_y);
				}
				/*Handle click events*/
				if ( event.type == SDL_MOUSEBUTTONDOWN )
				{
					SDL_NumberPickerHandleClick(npUPT, &event);
					SDL_NumberPickerHandleClick(npOPT, &event);
					SDL_NumberPickerHandleClick(npRBT, &event);
					SDL_NumberPickerHandleClick(npNHS, &event);
					SDL_NumberPickerHandleClick(npFR, &event);
					SDL_NumberPickerHandleClick(npDim, &event);
					if ( SDL_ButtonClicked(btQuitProgram, &event) )
					{
						gameRunning = 0;
						break;
					}
					if ( SDL_ButtonClicked(btStart, &event) )
					{
						/*Take the values from the number pickers and update our global variables.*/
						LS_SetUPThresh(&game_settings, npUPT->currentValue);
						LS_SetOPThresh(&game_settings, npOPT->currentValue);
						LS_SetNHSize(&game_settings, npNHS->currentValue);
						LS_SetRBNumber(&game_settings, npRBT->currentValue);
						LS_SetNCols(&game_settings, npDim->currentValue);
						LS_SetNRows(&game_settings, npDim->currentValue);

						DELAY_VALUE = ( int ) ( ( double ) 1000 / npFR->currentValue );
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

			SDL_UpdateInterface(ifMain);

			/* Clear screen with cool fade effect.*/
			SDL_DrawFillRectHelper(renderer, 0, 0, game_settings.SCREEN_WIDTH, game_settings.SCREEN_HEIGHT, cBlack);

			/*Draw words*/
			SDL_DrawTextAtXYStretchedColorShadow(renderer, "Conway's", title_font, MARGIN, MARGIN, 100, MARGIN * 2 - 8, cWhite, cGray, 1);
			SDL_DrawTextAtXYStretchedColorShadow(renderer, "GAME OF LIFE", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE, game_settings.SCREEN_WIDTH - MARGIN * 2, TITLEHEIGHT, cWhite, cGray, 4);
			SDL_DrawTextAtXYStretchedColorShadow(renderer, "IN FULL TEKNIKOLOR", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE + TITLEHEIGHT, game_settings.SCREEN_WIDTH - MARGIN * 2, SUBTITLEHEIGHT, cWhite, cGray, 2);
			SDL_DrawTextAtXYStretchedColorShadow(renderer, "Rules:", font, MARGIN, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 60, 16, cWhite, cGray, 1);
			SDL_DrawTextAtXYStretchedColorShadow(renderer, "Neighborhood", font, game_settings.SCREEN_WIDTH - MARGIN - 100, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 100, 16, cWhite, cGray, 1);

			/*Draw menu*/
			SDL_DrawInterface(renderer, ifMain);
			SDL_DrawBoxGrid(renderer, game_settings.SCREEN_WIDTH - MARGIN - 100, 150, 100, 2 * npNHS->currentValue + 1, 3, cGray, cWhite, font);

			SDL_RenderPresent(renderer);
		}

		/* Life session loop */

		Dot * mainLifeDotArray = NULL;
		Dot * soilDotArray = NULL;
		Dot * updatedLifeDotArray = NULL;

		int array_size;

		array_size = game_settings.N_ROWS*game_settings.N_COLS;

		mainLifeDotArray = Life_CreateDotArray(array_size);
		soilDotArray = Life_CreateDotArray(array_size);
		updatedLifeDotArray = Life_CreateDotArray(array_size);

		Life_RandomizeDots(mainLifeDotArray, array_size, 3, &game_settings);
		Life_SetAllDots(soilDotArray, array_size, 128);


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
					SDL_ButtonHandleMouseover(btMainMenu, mouse_x, mouse_y);
					Life_PutHandOfGodAtXY(mouse_x, mouse_y, mainLifeDotArray, &game_settings);
				}
				/*Handle click events*/
				if ( event.type == SDL_MOUSEBUTTONDOWN )
				{
					if ( SDL_ButtonClicked(btMainMenu, &event) )
					{
						sessionRunning = 0;
						onMainMenu = 1;
						break;
					}
				}
			}

			/* Update all dots. */
			Life_IterateGeneration(mainLifeDotArray, updatedLifeDotArray, &game_settings);
			if ( game_settings.ANTFARM )
				Life_UpdateSoil(mainLifeDotArray, soilDotArray, &game_settings);

			/*Update the button(s)*/
			SDL_UpdateButton(btMainMenu);

			/* Clear screen.*/
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			/*Draw everything*/
			if ( game_settings.ANTFARM )
				Life_DrawSoil(soilDotArray, renderer, &game_settings);
			Life_DrawAllDots(mainLifeDotArray, renderer, &game_settings);

			//	Draw text over the screen.
			char displayText[ 100 ];
			if ( timer[ 0 ] > ( double ) 1000 / DELAY_VALUE / 2 )
			{
				LIVE_COUNT_DISPLAY = *game_settings.LIVE_COUNT_PTR;
				timer[ 0 ] = 0;
			}
			sprintf_s(displayText, ( size_t ) 100, "live cells: %d", LIVE_COUNT_DISPLAY);
			SDL_DrawTextShadow(renderer, displayText, font, 2, 2, cWhite, 1);

			SDL_RenderButton(renderer, btMainMenu);
			SDL_RenderPresent(renderer);
			/*Delay for specified ms.*/
			SDL_Delay(DELAY_VALUE - ( Uint32 ) ( time(NULL) - start ));
		}

		/*Free arrays*/
		
		Life_DestroyDotArray(mainLifeDotArray);
		Life_DestroyDotArray(updatedLifeDotArray);
		Life_DestroyDotArray(soilDotArray);

	}

	/*Quit/deallocate everything.*/
	SDL_DestroyInterface(ifMain);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit( );
	SDL_DestroyButton(btMainMenu);

	/*Close fonts*/
	if ( font )
		TTF_CloseFont(font);
	if ( smaller_font )
		TTF_CloseFont(smaller_font);
	if ( title_font )
		TTF_CloseFont(title_font);
	TTF_Quit( ); 
	
	return 0;
}