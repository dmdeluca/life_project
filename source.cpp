/*Welcome to life.*/

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

/*Constants*/
#define NROWS 100
#define NCOLS 100
#define MAX_VALUE 1
#define DELAY_VALUE 1
#define STRONG_THRESHOLD 8
#define GLOW_ENABLE 1
#define GRADUALLY_INCREASE_COLOR 0
#define CHANCE_OF_MIRACLE 1000000000000000
#define HAND_OF_GOD_SIZE 2
#define PHI 0.61803398875
#define PI 3.14159265359
#define	MAX_BUTTON_STRING_LENGTH 128

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

/* Button enums*/
enum ButtonMouseoverTintStyle
{
	BUTTON_MOUSEOVER_TINTSTYLE_GRADUAL,
	BUTTON_MOUSEOVER_TINTSTYLE_INSTANT,
	BUTTON_MOUSEOVER_TINTSTYLE_NONE
};

enum ButtonPositionStyle
{
	BUTTON_POSITION_UNDEFINED,
	BUTTON_POSITION_FIXED
};

enum ButtonTextAlignment
{
	BUTTON_TEXT_ALIGNMENT_LEFT,
	BUTTON_TEXT_ALIGNMENT_CENTERED,
	BUTTON_TEXT_ALIGNMENT_RIGHT
};

enum ButtonFadeState
{
	BUTTON_FADE_STATE_STATIC,
	BUTTON_FADE_STATE_FADING_IN,
	BUTTON_FADE_STATE_FADING_OUT
};

enum ButtonGeneralState
{
	BS_STATIC,
	BS_MOUSEOVER,
	BS_CLICK,
	BS_TOGGLE
};

/* Button custom data structure */
typedef struct
{
	int			x;
	int			y;
	int			w;
	int			h;
	int			buttonPadding;
	TTF_Font	*buttonFont;
	SDL_Color	buttonTextColor;
	SDL_Color	buttonBackgroundColor;
	SDL_Color	buttonShadowColor;
	SDL_Color	buttonOutlineColor;
	SDL_Color	buttonMouseoverTintColor;
	Uint8		buttonTotalOpacity;
	Uint8		shadowOpacity;
	int			fadeValue;
	int			fadeSpeed;
	enum		ButtonFadeState buttonFadeState;
	enum		ButtonGeneralState buttonState;
	bool		shadowEnabled;
	bool		outlineEnabled;
	bool		buttonMouseoverTintEnabled;
	enum		ButtonMouseoverTintStyle buttonMouseoverTintStyle;
	enum		ButtonTextAlignment pButtonTextAlignment;
	char		buttonText[ MAX_BUTTON_STRING_LENGTH ];
	int			shadowOffset;
	bool		isActive;
	SDL_Color	buttonMouseoverBackgroundColor;
	SDL_Color	buttonMouseoverTextColor;
	SDL_Color	buttonClickBackgroundColor;
	SDL_Color	buttonClickTextColor;
	SDL_Color	buttonToggleBackgroundColor;
	SDL_Color	buttonToggleTextColor;
} SDL_CustomButton;

typedef struct
{
	SDL_CustomButton *pIncreaseButton;
	SDL_CustomButton *pDecreaseButton;
	bool hasShadow;
	int currentValue;
	int maxValue;
	int minValue;
	int incrementSize;
	//int *variable;
	int			x;
	int			y;
	int			w;
	int			h;
	char caption[ MAX_BUTTON_STRING_LENGTH];
	TTF_Font * font;
} SDL_NumberPicker;

/*Button default values*/
TTF_Font	*DEFAULT_BUTTON_FONT;
const int	DEFAULT_BUTTON_PADDING = 2;
SDL_Color	DEFAULT_BUTTON_TEXT_COLOR = { 0,0,0,255 };
SDL_Color	DEFAULT_BUTTON_BG_COLOR = { 255,255,255,255 };
SDL_Color	DEFAULT_BUTTON_MO_TINT_COLOR = { 255,0,0,255 };
SDL_Color	DEFAULT_BUTTON_MO_OUTLINE_COLOR = { 128,128,128,255 };
SDL_Color	DEFAULT_BUTTON_SHADOW_COLOR = { 0,0,0,255 };
enum ButtonTextAlignment DEFAULT_BUTTON_TEXT_ALIGNMENT = BUTTON_TEXT_ALIGNMENT_CENTERED;

/*RGB AND HSV STRUCTURES*/
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

typedef struct
{
	SDL_CustomButton * pButtonHead;
	SDL_CustomButton * pButtonNext;
} SDL_CustomButtonListItem;

typedef struct
{
	SDL_CustomButton * pButtonHead;
	SDL_CustomButton * pButtonNext;
} SDL_NumberPickerListContainer;

typedef struct
{
	int menuItemType;
	void * menuItem;
} SDL_MenuItem;

typedef struct
{
	SDL_MenuItem * pMenuHead;
	int menuItemPadding;
	SDL_Color menuItemBGColor;
	int menuTotalWidth;
	int menuTotalHeight;
	int menuMargin;
	int menuItemCount;
	int menuColumnNumber;
	int menuLayoutType;
};

typedef struct
{
	SDL_CustomButtonListItem * pButtonListHead;
	SDL_NumberPickerListContainer * pNumberPickerListContainer;
} Menu;

static hsv  rgb2hsv(rgb);
static rgb	hsv2rgb(hsv);
int			getIntBounded(int, int, int);
void		clearKeyboardBuffer(void);
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
void		SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color);
void		SDL_DrawTextAtXYStretched(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color);
void		SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow);
int			getMax(int num, ...);
int			getMin(int num, ...);
void		SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int h, int w, SDL_Color color);
void		Life_FreeFriendMap(FriendMap* fmap);
void		Life_PutHandOfGodAtXY(int x, int y, Dot * dots);
void		Life_SetAllDots(Dot * dots, int nIndices, int value);
void		Life_UpdateSoil(Dot * dots, Dot * soil);
void		SDL_ColorAdjSat(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjVal(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjHue(SDL_Color * pColor, double mAmount);
void		SDL_ColorAdjHSV(SDL_Color * pColor, double mHueAmount, double pSatAmount, double pValAmount);
int			SDL_ColorGetVal(SDL_Color color);
int			SDL_ColorGetSat(SDL_Color color);
int			SDL_ColorGetHue(SDL_Color color);
SDL_Color	averageColor(int, ...);
SDL_Color	randomColor(void);
FriendMap	*Life_CreateFriendMap(Dot * dots, int i, int row_length, int array_size);
void Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, int pArraySize);
SDL_CustomButton * SDL_CreateStandardButton(int x, int y, int w, int h, const char * pButtonText);
SDL_CustomButton * SDL_CreateButtonDetailed(
	int							x,
	int							y,
	int							w,
	int							h,
	enum ButtonPositionStyle	pButtonXPositionStyle,
	enum ButtonPositionStyle	pButtonYPositionStyle,
	const char		 			*pButtonText,
	enum ButtonTextAlignment	pButtonTextAlignment,
	TTF_Font					* pButtonFont,
	int							pButtonPadding,
	SDL_Color					pButtonTextColor,
	SDL_Color					pButtonBackgroundColor,
	SDL_Color					pButtonMouseoverTintColor,
	SDL_Color					pButtonOutlineColor,
	SDL_Color					pButtonShadowColor,
	bool						pButtonShadowEnabled,
	bool						pButtonMouseoverTintEnabled,
	bool						pButtonOutlineEnabled,
	Uint8						pButtonTotalOpacity,
	int							pShadowOffset
);
void SDL_RenderButton(SDL_Renderer *pRenderer, SDL_CustomButton * pButton);
void SDL_UpdateButton(SDL_CustomButton * pButton);
int SDL_ButtonClicked(SDL_CustomButton * pButton, SDL_Event * e);
int SDL_MouseInButtonBounds(int x, int y, SDL_CustomButton * pButton);
void SDL_ButtonHandleMouseover(SDL_CustomButton * btn, int mouse_x, int mouse_y);
void SDL_DrawTextAtXYStretchedColorShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color, SDL_Color shadowColor, int offset);
SDL_NumberPicker * SDL_CreateNumberPicker(int x, int y, int w, int h, int minValue, int maxValue, TTF_Font * font, const char * caption);
void SDL_DestroyButton(SDL_CustomButton * pButton);
void SDL_DestroyNumberPicker(SDL_NumberPicker * pNpk);
void SDL_RenderNumberPicker(SDL_Renderer * pRenderer, SDL_NumberPicker * npk); 
void SDL_UpdateNumberPicker(SDL_NumberPicker * npk);
void SDL_NumberPickerHandleMouseover(SDL_NumberPicker * npk, int mouse_x, int mouse_y);
void SDL_NumberPickerHandleClick(SDL_NumberPicker * npk, SDL_Event *e);

/*Global variables*/
SDL_Event				event;
SDL_MouseButtonEvent	mb_event;
SDL_Color				SOIL_COLOR = { 50,50,100,255 };
SDL_Color				DOT_COLOR { 255,255,255,255 };
bool					gameRunning = true;
bool					sessionRunning = true;
int						OVERPOP_NUMBER;
int						UNDERPOP_NUMBER;
int						REBIRTH_NUMBER;
int						FRIEND_RANGE;
static const int		SCREEN_WIDTH = 400;
static const int		SCREEN_HEIGHT = 400;
int						LIVE_COUNT = 0;
static const int		SOIL_VALUE_LIMIT = 255;
bool					ANTFARM_ENABLED = 0;

/*Program*/
int main(int argc, char * argv[ ])
{
	//	Begin loop.
	while ( gameRunning )
	{
		sessionRunning = true;
		/*	
		printf("Welcome to a variation of Life by David DeLuca. \nEnter 0 to quit.\n");
		printf("Enter overpopulation number: ");
		OVERPOP_NUMBER = getIntBounded(0, 100, 1);
		if ( !OVERPOP_NUMBER )
		{
			sessionRunning = false;
			gameRunning = false;
		}
		printf("Enter underpopulation number: ");
		UNDERPOP_NUMBER = getIntBounded(0, 100, 1);
		if ( !UNDERPOP_NUMBER )
		{
			sessionRunning = false;
			gameRunning = false;
		}
		printf("Enter rebirth number: ");
		REBIRTH_NUMBER = getIntBounded(0, 100, 1);
		if ( !REBIRTH_NUMBER )
		{
			sessionRunning = false;
			gameRunning = false;
		}
		printf("Enter friend range number: ");
		FRIEND_RANGE = getIntBounded(0, 10, 1);
		if ( !FRIEND_RANGE )
		{
			sessionRunning = false;
			gameRunning = false;
		}

		printf("Enter 1 to display antfarm cell tracker or 0 for blank black background.");
		ANTFARM_ENABLED = getIntBounded(0, 1, 1);
		if ( ANTFARM_ENABLED )
		{
			printf("ANTFARM ENABLED.\n");
		}
		else
		{
			printf("BLACK BACKGROUND ENABLED.\n");
		} */

		/*Quit if zero value was entered.*/
			/*Run the program.*/
			SDL_Init(SDL_INIT_VIDEO);

			srand(time(NULL));

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
			int onMainMenu = true;

			SDL_CustomButton * mainMenuButton = SDL_CreateStandardButton(10, SCREEN_HEIGHT - 25, -1, -1, "<< MAIN MENU");
			mainMenuButton->fadeSpeed = 50;

			SDL_CustomButton * quitProgramButton = SDL_CreateStandardButton(SCREEN_WIDTH-104, SCREEN_HEIGHT - 25, 96, 32, "Exit Program");

			SDL_NumberPicker * npUnderpopulationThreshold = SDL_CreateNumberPicker(10, 150, 60, 20, 0, 9, font, "Underpopulation Threshold");
			npUnderpopulationThreshold->currentValue = UNDERPOP_NUMBER;
			SDL_NumberPicker * npOverpopulationThreshold = SDL_CreateNumberPicker(10, 150+25*1, 60, 20, 0, 9, font, "Overpopulation Threshold");
			npOverpopulationThreshold->currentValue = OVERPOP_NUMBER;
			SDL_NumberPicker * npRebirthThreshold = SDL_CreateNumberPicker(10, 150 + 25 * 2, 60, 20, 0, 9, font, "Birth Threshold");
			npRebirthThreshold->currentValue = REBIRTH_NUMBER;
			SDL_NumberPicker * npNeighborhoodSize = SDL_CreateNumberPicker(10, 150 + 25 * 3, 60, 20, 0, 9, font, "Neighborhood Size");
			npNeighborhoodSize->currentValue = FRIEND_RANGE;

			const int TITLEHEIGHT = 64;
			const int MARGIN = 10;
			const int ELEMENTSPACE = 8;
			const int SUBTITLEHEIGHT = 20;

			SDL_Color cWhite = { 255,255,255,255 };
			SDL_Color cGray = { 128,128,128,255 };
			SDL_Color cDkGray = { 64,64,64,255 };

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
						if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE )
						{
							/*Take the values from the number pickers and update our global variables.*/
							UNDERPOP_NUMBER = npUnderpopulationThreshold->currentValue;
							OVERPOP_NUMBER = npOverpopulationThreshold->currentValue;
							FRIEND_RANGE = npNeighborhoodSize->currentValue;
							REBIRTH_NUMBER = npRebirthThreshold->currentValue;
							onMainMenu = false;
							sessionRunning = true;
						}
						/*Handle mouseover event*/
						if ( event.type == SDL_MOUSEMOTION )
						{
							SDL_NumberPickerHandleMouseover(npUnderpopulationThreshold, mouse_x, mouse_y);
							SDL_NumberPickerHandleMouseover(npOverpopulationThreshold, mouse_x, mouse_y);
							SDL_NumberPickerHandleMouseover(npRebirthThreshold, mouse_x, mouse_y);
							SDL_NumberPickerHandleMouseover(npNeighborhoodSize, mouse_x, mouse_y);
							SDL_ButtonHandleMouseover(quitProgramButton, mouse_x, mouse_y);
						}
						/*Handle click events*/
						if ( event.type == SDL_MOUSEBUTTONDOWN )
						{
							SDL_NumberPickerHandleClick(npUnderpopulationThreshold, &event);
							SDL_NumberPickerHandleClick(npOverpopulationThreshold, &event);
							SDL_NumberPickerHandleClick(npRebirthThreshold, &event);
							SDL_NumberPickerHandleClick(npNeighborhoodSize, &event);
							if ( SDL_ButtonClicked(quitProgramButton, &event) )
							{
								gameRunning = false;
								break;
							}
						}
					}

					SDL_UpdateNumberPicker(npUnderpopulationThreshold);
					SDL_UpdateNumberPicker(npOverpopulationThreshold);
					SDL_UpdateNumberPicker(npRebirthThreshold);
					SDL_UpdateNumberPicker(npNeighborhoodSize);
					SDL_UpdateButton(quitProgramButton);

					/* Clear screen.*/
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderClear(renderer);

					/*Draw everything*/
					SDL_DrawTextAtXYStretchedColorShadow(renderer, "Conway's", title_font, MARGIN, MARGIN, 128, MARGIN * 2 - 4, cWhite, cGray, 1);
					SDL_DrawTextAtXYStretchedColorShadow(renderer, "GAME OF LIFE", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE, SCREEN_WIDTH - MARGIN * 2, TITLEHEIGHT, cWhite, cGray, 4);
					SDL_DrawTextAtXYStretchedColorShadow(renderer, "IN FULL TEKNIKOLOR", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE + TITLEHEIGHT, SCREEN_WIDTH - MARGIN * 2, SUBTITLEHEIGHT, cWhite, cGray, 2);

					SDL_RenderButton(renderer, quitProgramButton);
					SDL_RenderNumberPicker(renderer, npUnderpopulationThreshold);
					SDL_RenderNumberPicker(renderer, npOverpopulationThreshold);
					SDL_RenderNumberPicker(renderer, npRebirthThreshold);
					SDL_RenderNumberPicker(renderer, npNeighborhoodSize);
					SDL_RenderPresent(renderer);
				}

				/* Life session loop */

				while ( sessionRunning && gameRunning )
				{
					//	Go through the life session.

					time_t start = time(NULL);

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
								sessionRunning = false;
								onMainMenu = true;
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
					SDL_Color cWhite = { 255,255,255 };
					char displayText[ 100 ];
					sprintf_s(displayText, "live cells: %d", LIVE_COUNT);
					SDL_DrawTextShadow(renderer, displayText, font, 2, 2, cWhite, 1);

					SDL_RenderButton(renderer, mainMenuButton);
					SDL_RenderPresent(renderer);
					/*Delay for specified ms.*/
					SDL_Delay(DELAY_VALUE - ( time(NULL) - start ));
				}
			}

			/*Quit everything.*/
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit( );
			SDL_DestroyButton(mainMenuButton);
			SDL_DestroyNumberPicker(npUnderpopulationThreshold);
			SDL_DestroyNumberPicker(npOverpopulationThreshold);
			SDL_DestroyNumberPicker(npRebirthThreshold);
			SDL_DestroyNumberPicker(npNeighborhoodSize);
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
		if (pLifeDotArray[i].value) {
			pSoilDotArray[ i ].value = 0;
		}
		else if (pSoilDotArray[i ].value < SOIL_VALUE_LIMIT )
		{
			pSoilDotArray[ i ].value += 1;
		}
	}
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
int getIntBounded(int pLowerBound, int pUpperBound, int isInclusive)
{
	int isValidInput = 0;
	int userGeneratedInteger;
	while ( !isValidInput )
	{
		isValidInput = scanf_s("%d", &userGeneratedInteger);
		if ( !isValidInput )
		{
			clearKeyboardBuffer( );
			printf("Sorry, that's not a valid input. Try again: ");
		}
		else
		{
			 //check for out-of-bounds inclusive
			if ( isInclusive && ( ( userGeneratedInteger > pUpperBound || userGeneratedInteger < pLowerBound ) ) )
			{
				printf("Sorry, that's out of bounds [%d-%d].", pLowerBound, pUpperBound);
				//enable next iteration
				isValidInput = 0;
				//check for out-of-bounds exclusive
			}
			else if ( !isInclusive && ( ( userGeneratedInteger >= pUpperBound || userGeneratedInteger <= pLowerBound ) ) )
			{
				printf("Sorry, that's out of bounds (%d-%d).", pLowerBound, pUpperBound);
				//enable next iteration
				isValidInput = 0;
			}
		}
	}
	clearKeyboardBuffer( );
	return userGeneratedInteger;
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
	SDL_DrawTextAtXYStretched(renderer, string, font, x+offset, y+offset, w, h, shadowColor);
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

/*Convert RGB to HSV - copied from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both */
hsv rgb2hsv(rgb pInputColor)
{
	hsv         out;
	double      min, max, delta;

	min = pInputColor.r < pInputColor.g ? pInputColor.r : pInputColor.g;
	min = min < pInputColor.b ? min : pInputColor.b;

	max = pInputColor.r > pInputColor.g ? pInputColor.r : pInputColor.g;
	max = max > pInputColor.b ? max : pInputColor.b;

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
	if ( pInputColor.r >= max )                           // > is bogus, just keeps compilor happy
		out.h = ( pInputColor.g - pInputColor.b ) / delta;        // between yellow & magenta
	else
		if ( pInputColor.g >= max )
			out.h = 2.0 + ( pInputColor.b - pInputColor.r ) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + ( pInputColor.r - pInputColor.g ) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if ( out.h < 0.0 )
		out.h += 360.0;

	return out;
}

/*Set all dots in an array of dots to a certain specified value.*/
void Life_SetAllDots(Dot * dots, int nIndices, int value)
{
	for ( int i = 0; i < nIndices; i++ )
	{
		dots[ i ].value = value;
		dots[ i ].color = { 255,255,255,255 };
	}
}

/*Convert HSV to RGB - copied from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both */
rgb hsv2rgb(hsv pInputColor)
{

	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if ( pInputColor.s <= 0.0 )
	{       // < is bogus, just shuts up warnings
		out.r = pInputColor.v;
		out.g = pInputColor.v;
		out.b = pInputColor.v;
		return out;
	}
	hh = pInputColor.h;
	if ( hh >= 360.0 )
		hh = 0.0;
	hh /= 60.0;
	i = ( long ) hh;
	ff = hh - i;
	p = pInputColor.v * ( 1.0 - pInputColor.s );
	q = pInputColor.v * ( 1.0 - ( pInputColor.s * ff ) );
	t = pInputColor.v * ( 1.0 - ( pInputColor.s * ( 1.0 - ff ) ) );

	switch ( i )
	{
	case 0:
		out.r = pInputColor.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = pInputColor.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = pInputColor.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = pInputColor.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = pInputColor.v;
		break;
	case 5:
	default:
		out.r = pInputColor.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

/*Average an arbitrary number of colors.*/
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

/*Generate a random color.*/
SDL_Color randomColor( )
{
	SDL_Color c;
	c.a = 255;
	c.r = rand( ) % 255;
	c.g = rand( ) % 255;
	c.b = rand( ) % 255;
	return c;
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

/*Get the maximum of an arbitrary number of integers.*/
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

/*Get the minimum of an arbitrary number of integers.*/
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

SDL_NumberPicker * SDL_CreateNumberPicker(int x, int y, int w, int h, int minValue, int maxValue, TTF_Font * font, const char * caption)
{
	SDL_NumberPicker * npk = ( SDL_NumberPicker* ) malloc(sizeof(SDL_NumberPicker));
	npk->h = h;
	npk->w = w;
	npk->x = x;
	npk->y = y;
	npk->font = font;
	npk->minValue = minValue;
	npk->maxValue = maxValue;
	npk->currentValue = minValue;
	npk->incrementSize = 1;
	sprintf_s(npk->caption, MAX_BUTTON_STRING_LENGTH,"%s",caption);
	npk->hasShadow = 1;
	int padding = 2;
	npk->pDecreaseButton = SDL_CreateStandardButton(npk->x+padding, npk->y+padding, (int)((double) npk->w / 3)-padding*2, npk->h-padding*2, "-");
	npk->pDecreaseButton->buttonPadding = padding;
	npk->pIncreaseButton = SDL_CreateStandardButton(npk->x + padding + (int)((double)npk->w / 3), npk->y + padding, (int)((double) npk->w / 3-padding*2), npk->h-padding*2, "+");
	npk->pIncreaseButton->buttonPadding = padding;
	return npk;
}

void SDL_RenderNumberPicker(SDL_Renderer * pRenderer, SDL_NumberPicker * npk)
{
	if ( npk->hasShadow )
	{
		/*Render number picker background*/
		SDL_Color shadowColor = { 100,100,100,(Uint8)128 };
		SDL_DrawFillRectHelper(pRenderer, npk->x+2, npk->y+2, npk->w, npk->h, shadowColor);
	}
	/*Render number picker background*/
	SDL_DrawFillRectHelper(pRenderer, npk->x, npk->y, npk->w, npk->h, {100,100,100,255});
	SDL_RenderButton(pRenderer, npk->pIncreaseButton);
	SDL_RenderButton(pRenderer, npk->pDecreaseButton);
	char * string = (char*)malloc(4*sizeof(char));
	_ltoa_s(npk->currentValue, string,4, 10);
	SDL_DrawTextAtXYStretched(
		pRenderer, 
		string, 
		npk->font, 
		npk->pIncreaseButton->buttonPadding+npk->x + ( int ) ( ( double ) npk->w * 2 / ( double ) 3 ), 
		npk->pIncreaseButton->buttonPadding+npk->y,
		( int ) ( ( double ) npk->w / ( double ) 3 ) - npk->pIncreaseButton->buttonPadding * 2,
		npk->h - npk->pIncreaseButton->buttonPadding * 2, 
		npk->pIncreaseButton->buttonTextColor
	);
	int text_width, text_height;
	TTF_SizeText(npk->font, npk->caption, &text_width, &text_height);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		npk->caption,
		npk->font,
		npk->x + npk->w +npk->pIncreaseButton->buttonPadding*2+4,
		npk->pIncreaseButton->buttonPadding*2 + npk->y,
		text_width/2,
		npk->h-npk->pIncreaseButton->buttonPadding*2,
		npk->pIncreaseButton->buttonBackgroundColor
	);
	free(string);
}

void SDL_DestroyButton(SDL_CustomButton * pButton)
{
	if ( pButton )
	{
		free(pButton);
	}
}

void SDL_DestroyNumberPicker(SDL_NumberPicker * pNpk)
{
	free(pNpk->pIncreaseButton);
	free(pNpk->pDecreaseButton);
	free(pNpk);
}

/* (WRAPPER) Create and return a button*/
SDL_CustomButton * SDL_CreateStandardButton(int x, int y, int w, int h, const char * pButtonText)
{
	SDL_CustomButton * btn = SDL_CreateButtonDetailed(
		x,y,w,h,
		BUTTON_POSITION_FIXED,
		BUTTON_POSITION_FIXED,
		pButtonText,
		DEFAULT_BUTTON_TEXT_ALIGNMENT,
		DEFAULT_BUTTON_FONT,
		DEFAULT_BUTTON_PADDING,
		DEFAULT_BUTTON_TEXT_COLOR,
		DEFAULT_BUTTON_BG_COLOR,
		DEFAULT_BUTTON_MO_TINT_COLOR,
		DEFAULT_BUTTON_MO_OUTLINE_COLOR,
		DEFAULT_BUTTON_SHADOW_COLOR,
		true,
		true,
		false,
		255,
		5
	);
	return btn;
}

/*Create and return a button with a lot of customizable features*/
SDL_CustomButton * SDL_CreateButtonDetailed(
	int							x, 
	int							y, 
	int							w, 
	int							h,
	enum ButtonPositionStyle	pButtonXPositionStyle,
	enum ButtonPositionStyle	pButtonYPositionStyle,
	const char					*pButtonText,
	enum ButtonTextAlignment	pButtonTextAlignment,
	TTF_Font					*pButtonFont,
	int							pButtonPadding,
	SDL_Color					pButtonTextColor,
	SDL_Color					pButtonBackgroundColor,
	SDL_Color					pButtonMouseoverTintColor,
	SDL_Color					pButtonOutlineColor,
	SDL_Color					pButtonShadowColor,
	bool						pButtonShadowEnabled,
	bool						pButtonMouseoverTintEnabled,
	bool						pButtonOutlineEnabled,
	Uint8						pButtonTotalOpacity,
	int							pShadowOffset
)
{

	SDL_CustomButton *btn = ( SDL_CustomButton* ) malloc(sizeof(SDL_CustomButton));

	/*Set colors*/
	btn->buttonTextColor = pButtonTextColor;
	btn->buttonBackgroundColor = pButtonBackgroundColor;
	btn->buttonClickBackgroundColor = pButtonTextColor;
	btn->buttonClickTextColor = pButtonBackgroundColor;
	btn->buttonMouseoverBackgroundColor = pButtonTextColor;
	btn->buttonMouseoverTextColor = pButtonBackgroundColor;
	btn->buttonToggleBackgroundColor = pButtonTextColor;
	btn->buttonToggleTextColor = pButtonBackgroundColor;
	btn->buttonMouseoverTintColor = pButtonMouseoverTintColor;
	btn->buttonOutlineColor = pButtonOutlineColor;
	btn->buttonShadowColor.a = ( int ) ( ( double ) pButtonTotalOpacity / 255 * ( double ) 128 / 255 );

	/*Set font*/
	btn->buttonFont = pButtonFont;

	/*Set formatting flags*/
	btn->buttonMouseoverTintEnabled = pButtonMouseoverTintEnabled;
	btn->buttonTotalOpacity = pButtonTotalOpacity;
	btn->shadowEnabled = pButtonShadowEnabled;
	btn->buttonPadding = getMin(0,pButtonPadding);
	btn->outlineEnabled = pButtonOutlineEnabled;
	btn->shadowOffset = pShadowOffset;

	/*Set fade values*/
	btn->buttonFadeState = BUTTON_FADE_STATE_FADING_IN;
	btn->fadeSpeed = -1;
	btn->fadeValue = 0;

	/*Set text*/
	if ( !strcmp(pButtonText, "") )
	{
		sprintf_s(btn->buttonText, 128, "%s", "[...]");
	}
	else
	{
		sprintf_s(btn->buttonText, 128, "%s", pButtonText);
	}

	/*Set width and height*/
	int tmpW, tmpH;
	TTF_SizeText(pButtonFont, pButtonText, &tmpW, &tmpH); //	might use these
	if ( w < 0 )
	{
		btn->w = tmpW + pButtonPadding * 2;
	}
	else
	{
		btn->w = w;
	}
	if ( h < 0 )
	{
		btn->h = tmpH + pButtonPadding * 2;
	}
	else
	{
		btn->h = h;
	}

	/*Set x position*/
	if ( pButtonXPositionStyle = BUTTON_POSITION_UNDEFINED )
	{
		btn->x = SCREEN_WIDTH / 2 - btn->w / 2;
	}
	else
	{
		btn->x = x;
	}

	/*Set y position*/
	if ( pButtonYPositionStyle = BUTTON_POSITION_UNDEFINED )
	{
		btn->y = SCREEN_HEIGHT / 2 - btn->h / 2;
	}
	else
	{
		btn->y = y;
	}

	/*Return the button*/
	return btn;
}

/*Render a button*/
void SDL_RenderButton(SDL_Renderer *pRenderer, SDL_CustomButton * pButton)
{
	/*Handle mouseover changes*/

	////////////////
	//	code here //
	////////////////

	/*Identify fade factor*/
	double fadeFactor = ( double ) pButton->fadeValue / 255;
	/*Draw shadow if applicable*/
	if ( pButton->shadowEnabled )
	{
		SDL_Color actualShadowColor = pButton->buttonShadowColor;
		actualShadowColor.a *= fadeFactor;
		actualShadowColor.a *= (double)(pButton->buttonTotalOpacity) / 255;
		/*Draw the shadow behind the button*/
		SDL_DrawFillRectHelper(pRenderer, pButton->x + pButton->shadowOffset, pButton->y + pButton->shadowOffset, pButton->w, pButton->h, actualShadowColor);
	}
	/*Create new colors for button text and button body depending on fade state and total opacity*/
	SDL_Color actualButtonColor;
	SDL_Color actualButtonTextColor;
	switch ( pButton->buttonState )
	{
	case BS_MOUSEOVER:
		actualButtonColor = pButton->buttonMouseoverBackgroundColor;
		actualButtonTextColor = pButton->buttonMouseoverTextColor;
		break;
	case BS_CLICK:
		actualButtonColor = pButton->buttonClickBackgroundColor;
		actualButtonTextColor = pButton->buttonClickTextColor;
		break;
	case BS_TOGGLE: 
		actualButtonColor = pButton->buttonToggleBackgroundColor;
		actualButtonTextColor = pButton->buttonToggleTextColor;
		break;
	case BS_STATIC:
	default: 
		actualButtonColor = pButton->buttonBackgroundColor;
		actualButtonTextColor = pButton->buttonTextColor;
		break;
	}
	Uint8 actualOpacity = ( Uint8 ) ( pButton->buttonTotalOpacity*fadeFactor );
	actualButtonColor.a = actualOpacity;
	actualButtonTextColor.a = actualOpacity;
	/*Draw the actual button body*/
	SDL_DrawFillRectHelper(pRenderer, pButton->x, pButton->y, pButton->w, pButton->h, actualButtonColor);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		pButton->buttonText,
		pButton->buttonFont,
		pButton->x + pButton->buttonPadding,
		pButton->y + pButton->buttonPadding,
		pButton->w - pButton->buttonPadding * 2,
		pButton->h - pButton->buttonPadding * 2,
		actualButtonTextColor
	);
	return;
}

/*Handle mouseover events for number pickers*/
void SDL_NumberPickerHandleMouseover(SDL_NumberPicker * npk, int mouse_x, int mouse_y)
{
	SDL_ButtonHandleMouseover(npk->pIncreaseButton, mouse_x, mouse_y);
	SDL_ButtonHandleMouseover(npk->pDecreaseButton, mouse_x, mouse_y);
}

/*Handle mouseover events for buttons*/
void SDL_ButtonHandleMouseover(SDL_CustomButton * pButton, int mouse_x, int mouse_y)
{
	if ( SDL_MouseInButtonBounds(mouse_x, mouse_y, pButton) && pButton->buttonState != BS_CLICK)
	{
		pButton->buttonState = BS_MOUSEOVER;
	}
	else
	{
		pButton->buttonState = BS_STATIC;
	}
}

void SDL_UpdateNumberPicker(SDL_NumberPicker * npk)
{
	SDL_UpdateButton(npk->pIncreaseButton);
	SDL_UpdateButton(npk->pDecreaseButton);
}

void SDL_UpdateButton(SDL_CustomButton * pButton)
{
	/*Handle button fades*/

	switch ( pButton->buttonFadeState )
	{
	case BUTTON_FADE_STATE_FADING_IN:
		if ( pButton->fadeValue+pButton->fadeSpeed < 255 && pButton->fadeSpeed > 0 )
		{
			pButton->fadeValue += pButton->fadeSpeed;
		}
		else
		{
			pButton->fadeValue = 255;
			pButton->buttonFadeState = BUTTON_FADE_STATE_STATIC;
		}
		break;
	case BUTTON_FADE_STATE_FADING_OUT:
		if ( pButton->fadeValue-pButton->fadeSpeed > 0 && pButton->fadeSpeed > 0 )
		{
			pButton->fadeValue -= pButton->fadeSpeed;
		}
		else
		{
			pButton->fadeValue = 0;
			pButton->buttonFadeState = BUTTON_FADE_STATE_STATIC;
			pButton->isActive = false;
		}
		break;
	case BUTTON_FADE_STATE_STATIC:
	default:
		break;
	}
}

/*Returns 1 if button specified has been clicked*/
int SDL_ButtonClicked( SDL_CustomButton * pButton, SDL_Event * e) 
{
	int mouse_x, mouse_y;
	if ( e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT )
	{
		SDL_GetMouseState(&mouse_x, &mouse_y);
		if ( SDL_MouseInButtonBounds(mouse_x, mouse_y, pButton) )
		{
			return 1;
		}
	}
	return 0;
}

void SDL_NumberPickerHandleClick(SDL_NumberPicker * npk, SDL_Event *e)
{
	if ( SDL_ButtonClicked(npk->pIncreaseButton, e) )
	{
		npk->currentValue += npk->incrementSize;
	}
	if ( SDL_ButtonClicked(npk->pDecreaseButton, e) )
	{
		npk->currentValue -= npk->incrementSize;
	}
	if ( npk->currentValue > npk->maxValue ) npk->currentValue = npk->maxValue;
	if ( npk->currentValue < npk->minValue ) npk->currentValue = npk->minValue;
}

/*Returns 1 if mouse is over button specified*/
int SDL_MouseInButtonBounds(int x, int y, SDL_CustomButton * pButton)
{
	if (
		x >= pButton->x &&
		x <= pButton->x + pButton->w &&
		y >= pButton->y &&
		y <= pButton->y + pButton->h
		)
	{
		return 1;
	}
	return 0;
}

/* Calculate new value for a single dot in the array.*/
Dot Life_CalculateNewDotValue(Dot * dots, int i)
{
	/*Capture the dot of interest.*/
	Dot newDot = dots[ i ];
	/*Store the old value and color of the dot.*/
	int oldValue = dots[ i ].value;
	SDL_Color newColor = dots[ i ].color;
	/*Create a friendmap structure of nearby friends so that color averaging is possible later.*/
	FriendMap * fMap = Life_CreateFriendMap(dots, i, NCOLS, NROWS*NCOLS);
	/*Capture number of friends, then evaluate.*/
	int numFriends = fMap->count;
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
		if ( numFriends == REBIRTH_NUMBER  )
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
			SDL_ColorAdjHSV(&newDot.color, 10,0.1,0.1);
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

/*Boost a color's saturation. (Really a wrapper for SDL_ColorBoostHSV)*/
void SDL_ColorAdjSat(SDL_Color * pColor, double mAmount)
{
	SDL_ColorAdjHSV(pColor, 0, mAmount, 0);
}

/*Boost a color's value.(Really a wrapper for SDL_ColorBoostHSV)*/
void SDL_ColorAdjVal(SDL_Color * pColor, double mAmount)
{
	SDL_ColorAdjHSV(pColor, 0, 0, mAmount);
}

/*Boost a color's hue. (Really a wrapper for SDL_ColorBoostHSV)*/
void SDL_ColorAdjHue(SDL_Color * pColor, double mAmount)
{
	SDL_ColorAdjHSV(pColor, mAmount, 0, 0);
}

/*Boost a color's hue, sat, or val.*/
void SDL_ColorAdjHSV(SDL_Color * pColor, double pHueAmount, double pSatAmount, double mValAmount)
{
	rgb colorAsRGB = {
		( double ) pColor->r / 255.0,
		( double ) pColor->g / 255.0,
		( double ) pColor->b / 255.0
	};

	/*Convert to HSV and increase V.*/
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	colorAsHSV.v += mValAmount;
	colorAsHSV.s += pSatAmount;
	colorAsHSV.h += pHueAmount;

	/*Limit maximum values to looping through colors.*/
	/*VALUE*/
	if ( colorAsHSV.v > 1.0 )
		colorAsHSV.v = 1.0;
	if ( colorAsHSV.v < 0.0 )
		colorAsHSV.v = 0.0;
	/*SATURATION*/
	if ( colorAsHSV.s > 1.0 )
		colorAsHSV.s = 1.0;
	if ( colorAsHSV.s < 0.0 )
		colorAsHSV.s = 0.0;
	/*HUE*/
	while ( colorAsHSV.h > 360 )
		colorAsHSV.h -= 360;
	while ( colorAsHSV.h < 0 )
		colorAsHSV.h += 360;

	/*Convert back to RGB and assign new RGB values to pColor*/
	colorAsRGB = hsv2rgb(colorAsHSV);
	pColor->r = ( Uint8 ) ( colorAsRGB.r * 255 );
	pColor->g = ( Uint8 ) ( colorAsRGB.g * 255 );
	pColor->b = ( Uint8 ) ( colorAsRGB.b * 255 );

	return;
}

/*Get a color's hue.*/
int SDL_ColorGetHue(SDL_Color pColor)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 });
	return ( int ) (colorAsHSV.h/360 * 255);
}

/*Get a color's value*/
int SDL_ColorGetVal(SDL_Color pColor)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 });
	return ( int ) (colorAsHSV.v * 255);
}

/*Get a color's saturation*/
int SDL_ColorGetSat(SDL_Color pColor)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 });
	return ( int ) (colorAsHSV.s * 255);
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
			empty_dot.color = { 0,0,0,255 };
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
		int alpha = (int)(255*PHI*PHI); // #goldenratio?!??

		/*Draw two extra rectangles behind the white part to simulate glow.*/
		if ( GLOW_ENABLE && !isSoil )
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