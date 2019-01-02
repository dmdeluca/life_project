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
#define STRONG_THRESHOLD 8
#define GRADUALLY_INCREASE_COLOR 0
#define CHANCE_OF_MIRACLE 1000000000000000
#define HAND_OF_GOD_SIZE 2
#define PHI 0.61803398875
#define PI 3.14159265359
#define	MAX_BUTTON_STRING_LENGTH 128

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
	BTA_LEFT,
	BTA_CENT,
	BTA_RIGHT
};

enum ButtonFadeState
{
	BFS_STATIC,
	BFS_FADEIN,
	BFS_FADEOUT
};

enum ButtonGeneralState
{
	BS_STATIC,
	BS_MOUSEOVER,
	BS_CLICK,
	BS_TOGGLE
};

enum CheckBoxState
{
	CB_CHECKED,
	CB_UNCHECKED
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
	bool		cornersRounded;
	int			cornerRadius;
} SDL_CustomButton;

typedef struct
{
	int			x;
	int			y;
	int			w;
	int			h;
	enum CheckBoxState state;
	SDL_Color	bgColor;
	SDL_Color	fgColor;
	SDL_Color	ckColor;
	bool		shadowEnabled;
	SDL_Color	shColor;
	TTF_Font	*font;
	char 		caption[MAX_BUTTON_STRING_LENGTH];
	int			padding;
} SDL_CheckBox;

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
	int			padding;
	char caption[ MAX_BUTTON_STRING_LENGTH ];
	TTF_Font * font;
} SDL_NumberPicker;

/*Button default values*/
TTF_Font	*DEFAULT_BUTTON_FONT;
const int	DEFAULT_BUTTON_PADDING = 2;
SDL_Color	DEFAULT_BUTTON_TEXT_COLOR = { 0,0,0,255 };
SDL_Color	DEFAULT_BUTTON_BG_COLOR = { 255,255,255,255 };
SDL_Color	DEFAULT_BUTTON_MO_TINT_COLOR = { 255,0,0,255 };
SDL_Color	DEFAULT_BUTTON_MO_OUTLINE_COLOR = { 128,128,128,255 };
SDL_Color	DEFAULT_BUTTON_SHADOW_COLOR = { 64,64,64,255 };
enum ButtonTextAlignment DEFAULT_BUTTON_TEXT_ALIGNMENT = BTA_CENT;


/*Interface item container structure*/
typedef struct interface_item_container
{
	enum SDL_InterfaceItemType type;
	SDL_CustomButton * button;
	SDL_NumberPicker * npk;
	SDL_CheckBox * chkbx;
	struct interface_item_container * pNext;
} SDL_InterfaceItemContainer;

enum SDL_InterfaceItemType
{
	IIT_NONE,
	IIT_NPK,
	IIT_BTN,
	IIT_CHB
};

enum SDL_InterfaceLayoutType
{
	IL_GRID,
	IL_INLINE,
	IL_LIST
};


/*Interface structure*/
typedef struct
{
	SDL_InterfaceItemContainer * pHead;
	enum SDL_InterfaceItemType mItemType;
	int interfaceItemPadding;
	SDL_Color interfaceItemBGColor;
	SDL_Color	interfaceItemTextColor;
	int interfaceTotalWidth;
	int interfaceTotalHeight;
	TTF_Font * interfaceFont;
	int	x;
	int y;
	int interfaceMargin;
	int interfaceItemCount;
	int interfaceColumnNumber;
	enum SDL_InterfaceLayoutType interfaceLayoutType;
	int t;
} SDL_Interface;

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

/*Utility declarations*/
int			getMax(int num, ...);
int			getMin(int num, ...);
int			getIntBounded(int, int, int);
void		clearKeyboardBuffer(void);

/*Drawing utility declarations*/
void		SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color);
void		SDL_DrawTextAtXYStretched(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color);
void		SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow);
void		SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int h, int w, SDL_Color color);
void		SDL_DrawFillRectHelperColorless(SDL_Renderer *pRenderer, int x, int  y, int w, int h);
void		SDL_DrawFillRectRoundedHelper(SDL_Renderer *pRenderer, int x, int y, int w, int h, SDL_Color pColor, int radius);

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
static hsv  rgb2hsv(rgb);
static rgb	hsv2rgb(hsv);

/*Friend map declarations*/
FriendMap	*Life_CreateFriendMap(Dot * dots, int i, int row_length, int array_size);
void Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, int pArraySize);
void		Life_FreeFriendMap(FriendMap* fmap);



/*Button function declarations*/
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
void SDL_ButtonHandleMouseover(SDL_CustomButton * btn, int mouse_x, int mouse_y);
void SDL_DestroyButton(SDL_CustomButton * pButton);

/*Mouse helper functions declarations*/
int SDL_MouseInButtonBounds(int x, int y, SDL_CustomButton * pButton);
int SDL_MouseInNumberPickerBounds(int x, int y, SDL_NumberPicker * npk);
int SDL_MouseInCheckBoxBounds(int x, int y, SDL_CheckBox * chb);
int SDL_MouseInBox(int mouse_x, int mouse_y, int x1, int y1, int x2, int y2);

/*Text declarations*/
void SDL_DrawTextAtXYStretchedColorShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color, SDL_Color shadowColor, int offset);

/*Number Picker declarations*/
SDL_NumberPicker * SDL_CreateNumberPicker(int x, int y, int w, int h, int minValue, int maxValue, TTF_Font * font, const char * caption);
void SDL_DestroyNumberPicker(SDL_NumberPicker * pNpk);
void SDL_RenderNumberPicker(SDL_Renderer * pRenderer, SDL_NumberPicker * npk);
void SDL_UpdateNumberPicker(SDL_NumberPicker * npk);
void SDL_NumberPickerHandleMouseover(SDL_NumberPicker * npk, int mouse_x, int mouse_y);
void SDL_NumberPickerHandleClick(SDL_NumberPicker * npk, SDL_Event *e);
void SDL_NumberPickerAutoAdjustButtons(SDL_NumberPicker *npk);

/*Check box declarations*/
SDL_CheckBox * SDL_CreateCheckBox(int x, int y, int w, int h, enum CheckBoxState startValue, TTF_Font * font, const char * caption);
void SDL_DestroyCheckBox(SDL_CheckBox * chb);
void SDL_RenderCheckBox(SDL_Renderer * pRenderer, SDL_CheckBox * chb);
void SDL_UpdateCheckBox(SDL_CheckBox * chb);
int	SDL_CheckHandleClick(SDL_CheckBox *chb, SDL_Event *e);

/*Other declarations*/
void SDL_DrawBoxGrid(SDL_Renderer * pRenderer, int x, int y, int wh, int n, int space, SDL_Color pBoxColor, SDL_Color pTextColor, TTF_Font *font);

/*Interface and interface container declarations*/
SDL_InterfaceItemContainer * SDL_CreateInterfaceItemContainer(enum SDL_InterfaceItemType type);
SDL_InterfaceItemContainer * SDL_GetInterfaceTail(SDL_Interface *interface);
void SDL_InterfaceAutoArrangeItems(SDL_Interface *interface);
void SDL_DestroyInterface(SDL_Interface *interface);
void SDL_DestroyInterfaceItemContainer(SDL_InterfaceItemContainer *ctn);
SDL_Interface * SDL_InterfaceAddNewButton(SDL_Interface *interface);
SDL_Interface * SDL_InterfaceAddNewNumberPicker(SDL_Interface *interface);
SDL_Interface * SDL_InterfaceAddNewGenericItem(SDL_Interface *interface, enum SDL_InterfaceItemType type);
int SDL_InterfaceCountItems(SDL_Interface *interface);
void SDL_DrawInterface(SDL_Renderer *pRenderer, SDL_Interface * interface);
void SDL_DrawInterfaceItem(SDL_Renderer *pRenderer, SDL_InterfaceItemContainer * ctn);
SDL_Interface * SDL_CreateInterfaceAtXY(int x, int y, int w, int h);
SDL_Interface * SDL_CreateInterface(int w, int h);
SDL_Interface * SDL_InterfaceAddButton(SDL_Interface * interface, SDL_CustomButton * button);
SDL_Interface * SDL_InterfaceAddNumberPicker(SDL_Interface * interface, SDL_NumberPicker * npk);
void SDL_UpdateInterface(SDL_Interface * pInterface);
void SDL_InterfaceHandleMouseover(SDL_Interface *pInterface, int mx, int my);
SDL_Interface * SDL_InterfaceAddCheckBox(SDL_Interface * interface, SDL_CheckBox * chkbx);

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
int						LIVE_COUNT_DISPLAY = 0;
static const int		SOIL_VALUE_LIMIT = 255;
bool					ANTFARM_ENABLED = 0;
int						DELAY_VALUE = 30;
int						timer[ 5 ] = {0};
int						GLOW_ENABLED = 1;


/*Program*/
int main(int argc, char * argv[ ])
{
	//	Begin loop.
	while ( gameRunning )
	{
		sessionRunning = true;
	
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

		SDL_Interface *pIf = SDL_CreateInterfaceAtXY(10,150,SCREEN_WIDTH*.9, SCREEN_HEIGHT*.6);
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
							gameRunning = false;
							break;
						}
						if ( SDL_ButtonClicked(startButton, &event) )
						{
							/*Take the values from the number pickers and update our global variables.*/
							UNDERPOP_NUMBER = npUnderpopulationThreshold->currentValue;
							OVERPOP_NUMBER = npOverpopulationThreshold->currentValue;
							FRIEND_RANGE = npNeighborhoodSize->currentValue;
							REBIRTH_NUMBER = npRebirthThreshold->currentValue;
							DELAY_VALUE = (double)1000/npFrameRate->currentValue;
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
							onMainMenu = false;
							sessionRunning = true;
							break;
						}
						SDL_CheckHandleClick(cbAntFarmEnable, &event);
						SDL_CheckHandleClick(cbEnableColorGlow, &event);
					}
				}

				SDL_UpdateInterface(pIf);

				/* Clear screen with cool fade effect.*/
				SDL_DrawFillRectHelper(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, { 0,0,0,20 });

				/*Draw words*/
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Conway's", title_font, MARGIN, MARGIN, 100, MARGIN * 2 - 8, cWhite, cGray, 1);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "GAME OF LIFE", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE, SCREEN_WIDTH - MARGIN * 2, TITLEHEIGHT, cWhite, cGray, 4);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "IN FULL TEKNIKOLOR", title_font, MARGIN, MARGIN * 2 + ELEMENTSPACE + TITLEHEIGHT, SCREEN_WIDTH - MARGIN * 2, SUBTITLEHEIGHT, cWhite, cGray, 2);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Rules:", font, MARGIN, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 60, 16, cWhite, cGray, 1);
				SDL_DrawTextAtXYStretchedColorShadow(renderer, "Neighborhood", font, SCREEN_WIDTH-MARGIN-100, MARGIN + TITLEHEIGHT + SUBTITLEHEIGHT + ELEMENTSPACE * 4, 100, 16, cWhite, cGray, 1);

				/*Draw menu*/
				SDL_DrawInterface(renderer,pIf);
				SDL_DrawBoxGrid(renderer, SCREEN_WIDTH-MARGIN-100, 150, 100, 2 * npNeighborhoodSize->currentValue + 1, 3, cGray, cWhite, font);

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
				if ( timer[ 0 ] > ( double ) 1000 / DELAY_VALUE / 2 )
				{
					LIVE_COUNT_DISPLAY = LIVE_COUNT;
					timer[ 0 ] = 0;
				}
				sprintf_s(displayText, "live cells: %d", LIVE_COUNT_DISPLAY);
				SDL_DrawTextShadow(renderer, displayText, font, 2, 2, cWhite, 1);

				SDL_RenderButton(renderer, mainMenuButton);
				SDL_RenderPresent(renderer);
				/*Delay for specified ms.*/
				SDL_Delay(DELAY_VALUE - ( time(NULL) - start ));
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

SDL_CheckBox * SDL_CreateCheckBox(int x, int y, int w, int h, enum CheckBoxState startValue, TTF_Font * font, const char * caption)
{
	SDL_CheckBox *chb = ( SDL_CheckBox* ) malloc(sizeof(SDL_CheckBox));
	chb->bgColor = DEFAULT_BUTTON_BG_COLOR;
	chb->ckColor = DEFAULT_BUTTON_TEXT_COLOR;
	chb->fgColor = { 255,255,255,255 };
	chb->h = h;
	chb->w = w;
	chb->padding = DEFAULT_BUTTON_PADDING;
	chb->x = x;
	chb->y = y;
	chb->font = DEFAULT_BUTTON_FONT;
	chb->shadowEnabled = true;
	chb->shColor = DEFAULT_BUTTON_SHADOW_COLOR;
	sprintf_s(chb->caption, MAX_BUTTON_STRING_LENGTH, "%s", caption);
	return chb;
}

void SDL_DestroyCheckBox(SDL_CheckBox * chb)
{
	free(chb);
}

void SDL_RenderCheckBox(SDL_Renderer * pRenderer, SDL_CheckBox * chb)
{
	/*Render the empty box*/
	SDL_DrawFillRectHelper(
		pRenderer, 
		chb->x, 
		chb->y, 
		chb->w, 
		chb->h, 
		chb->bgColor
	);
	SDL_DrawFillRectHelper(
		pRenderer, 
		chb->x + chb->padding, 
		chb->y + chb->padding, 
		chb->w - chb->padding * 2, 
		chb->h - chb->padding * 2, 
		chb->fgColor
	);
	if ( chb->state == CB_CHECKED )
	{
		SDL_DrawTextAtXYStretched(
			pRenderer,
			"X",
			chb->font,
			chb->x + chb->padding * 2,
			chb->y + chb->padding * 2,
			chb->w - chb->padding * 2 * 2,
			chb->h - chb->padding * 2 * 2,
			chb->ckColor
		);
	}
	else
	{
		//	Do Nothing!
	}
	int text_width, text_height;
	TTF_SizeText(chb->font, chb->caption, &text_width, &text_height);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		chb->caption,
		chb->font,
		chb->x + chb->w +chb->padding*2,
		chb->padding+chb->y,
		text_width,
		chb->h - chb->padding,
		chb->bgColor
	);

}

void SDL_UpdateCheckBox(SDL_CheckBox * chb)
{
	//	Not much to do here.
}

int	SDL_CheckHandleClick(SDL_CheckBox *chb, SDL_Event *e)
{
	int mouse_x, mouse_y;
	if ( e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT )
	{
		SDL_GetMouseState(&mouse_x, &mouse_y);
		if ( SDL_MouseInCheckBoxBounds(mouse_x, mouse_y, chb) )
		{
			if ( chb->state == CB_CHECKED ) chb->state = CB_UNCHECKED;
			else chb->state = CB_CHECKED;
			return 1;
		}
	}
	return 0;
}

void SDL_InterfaceHandleMouseover(SDL_Interface *pInterface, int mx, int my)
{
	SDL_InterfaceItemContainer * ctn = pInterface->pHead;
	while ( ctn )
	{
		if ( ctn->button )
			SDL_ButtonHandleMouseover(ctn->button, mx, my);
		if ( ctn->npk )
			SDL_NumberPickerHandleMouseover(ctn->npk, mx, my);
		ctn = ctn->pNext;
	}
}

/*Add existing button to interface*/
SDL_Interface * SDL_InterfaceAddButton(SDL_Interface * interface, SDL_CustomButton * button)
{
	SDL_InterfaceItemContainer *pTailCtn = SDL_GetInterfaceTail(interface);
	SDL_InterfaceItemContainer *newCtn = SDL_CreateInterfaceItemContainer(IIT_BTN);
	newCtn->button = button;
	if ( !pTailCtn )
		interface->pHead = newCtn;
	pTailCtn->pNext = newCtn;
	SDL_InterfaceAutoArrangeItems(interface);
	return interface;
}

/*Add existing number picker to interface*/
SDL_Interface * SDL_InterfaceAddNumberPicker(SDL_Interface * interface, SDL_NumberPicker * npk)
{
	SDL_InterfaceItemContainer *pTailCtn = SDL_GetInterfaceTail(interface);
	SDL_InterfaceItemContainer *newCtn = SDL_CreateInterfaceItemContainer(IIT_NPK);
	newCtn->npk = npk;
	if ( !pTailCtn )
		interface->pHead = newCtn;
	else
		pTailCtn->pNext = newCtn;
	SDL_InterfaceAutoArrangeItems(interface);
	return interface;
}


/*Add existing check box to interface*/
SDL_Interface * SDL_InterfaceAddCheckBox(SDL_Interface * interface, SDL_CheckBox * chkbx)
{
	SDL_InterfaceItemContainer *pTailCtn = SDL_GetInterfaceTail(interface);
	SDL_InterfaceItemContainer *newCtn = SDL_CreateInterfaceItemContainer(IIT_CHB);
	newCtn->chkbx = chkbx;
	if ( !pTailCtn )
		interface->pHead = newCtn;
	else
		pTailCtn->pNext = newCtn;
	SDL_InterfaceAutoArrangeItems(interface);
	return interface;
}


/* (WRAPPER) Add a button to the interface*/
SDL_Interface * SDL_InterfaceAddNewButton(SDL_Interface *interface)
{
	SDL_InterfaceAddNewGenericItem(interface, IIT_BTN);
	return interface;
}

/* (WRAPPER) Add a number picker to the interface*/
SDL_Interface * SDL_InterfaceAddNewNumberPicker(SDL_Interface *interface)
{
	SDL_InterfaceAddNewGenericItem(interface, IIT_NPK);
	return interface;
}

/* (WRAPPER) Add a number picker to the interface*/
SDL_Interface * SDL_InterfaceAddNewCheckBox(SDL_Interface *interface)
{
	SDL_InterfaceAddNewGenericItem(interface, IIT_CHB);
	return interface;
}

/*Add an item to the interface*/
SDL_Interface * SDL_InterfaceAddNewGenericItem(SDL_Interface *interface, enum SDL_InterfaceItemType type)
{
	SDL_InterfaceItemContainer * tail = SDL_GetInterfaceTail(interface);
	SDL_InterfaceItemContainer * newCtn = SDL_CreateInterfaceItemContainer(type);
	/*Attach list item to interface.*/
	if ( !tail )
	{
		interface->pHead = newCtn;
	}
	else
	{
		tail->pNext = newCtn;
	}
	SDL_InterfaceAutoArrangeItems(interface);
	return interface;
}

/* Return the number of items in the interface*/
int SDL_InterfaceCountItems(SDL_Interface *interface)
{
	SDL_InterfaceItemContainer * head = interface->pHead;
	int count = 0;
	while ( head )
	{
		head = head->pNext;
		count++;
	}
	return count;
}

void SDL_DrawInterface(SDL_Renderer *pRenderer, SDL_Interface * interface)
{
	SDL_InterfaceItemContainer * head = interface->pHead;
	while ( head )
	{
		SDL_DrawInterfaceItem(pRenderer,head);
		head = head->pNext;
	}
}

void SDL_DrawInterfaceItem(SDL_Renderer *pRenderer, SDL_InterfaceItemContainer * ctn)
{
	switch ( ctn->type )
	{
	case IIT_BTN:
		if ( ctn->button )
			SDL_RenderButton(pRenderer, ctn->button);
		break;
	case IIT_NPK:
		if ( ctn->npk )
			SDL_RenderNumberPicker(pRenderer, ctn->npk);
		break;
	case IIT_CHB:
		if ( ctn->chkbx )
			SDL_RenderCheckBox(pRenderer, ctn->chkbx);
		break;
	case IIT_NONE:
	default:
		break;
	}
}

SDL_InterfaceItemContainer * SDL_CreateInterfaceItemContainer(enum SDL_InterfaceItemType type)
{
	SDL_InterfaceItemContainer * ctn = ( SDL_InterfaceItemContainer* ) malloc(sizeof(SDL_InterfaceItemContainer));
	ctn->type = type;
	ctn->button = NULL;
	ctn->pNext = NULL;
	ctn->npk = NULL;
	/*Create contained item*/
	switch ( type )
	{
	case IIT_BTN:
		// Create button. Attributes to be updated later.
		ctn->button = SDL_CreateStandardButton(0, 0, 0, 0, "Empty Button");
		break;
	case IIT_NPK:
		// Create number picker. Attributes to be updated later.
		ctn->npk = SDL_CreateNumberPicker(0, 0, 0, 0, 1, 10, DEFAULT_BUTTON_FONT, "No Caption");
		break;
	case IIT_NONE:
		/*Do nothing.*/
	default:
		break;
	}
	/*Return container*/
	return ctn;
}

void SDL_InterfaceAutoArrangeItems(SDL_Interface *interface)
{
	//printf("SDL_InterfaceAutoArrangeItems was called.");
	SDL_InterfaceItemContainer *ctn = interface->pHead;
	int itemsArranged = 0;
	int itemsToArrange = SDL_InterfaceCountItems(interface);
	while ( ctn )
	{
		if ( ctn->button )
		{
			ctn->button->x = interface->x + interface->interfaceMargin;
			//	[Do not change width for now.]
			ctn->button->y = interface->y + ( double ) interface->interfaceTotalHeight * (double)itemsArranged/ itemsToArrange + interface->interfaceMargin;
			ctn->button->h = ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2;
		}
		else if ( ctn->npk )
		{
			ctn->npk->x = interface->x + interface->interfaceMargin;
			//	[Do not change width for now.]
			ctn->npk->y = interface->y + ( double ) interface->interfaceTotalHeight * ( double ) itemsArranged / itemsToArrange + interface->interfaceMargin;
			ctn->npk->h = ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2;
			SDL_NumberPickerAutoAdjustButtons(ctn->npk);
		}
		else if ( ctn->chkbx )
		{
			ctn->chkbx->x = interface->x + interface->interfaceMargin;
			//	[Do not change width for now.]
			ctn->chkbx->y = interface->y + ( double ) interface->interfaceTotalHeight * ( double ) itemsArranged / itemsToArrange + interface->interfaceMargin;
			ctn->chkbx->h = ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2;
			ctn->chkbx->w = ctn->chkbx->h;
		}
		itemsArranged++;
		ctn = ctn->pNext;
	}
	return;
}

void SDL_UpdateInterface(SDL_Interface * pInterface)
{
	SDL_InterfaceItemContainer * ctn = pInterface->pHead;
	while ( ctn )
	{
		/*Update the button*/
		if ( ctn->button )
			SDL_UpdateButton(ctn->button);
		if ( ctn->npk )
			SDL_UpdateNumberPicker(ctn->npk);
		if ( ctn->chkbx )
			SDL_UpdateCheckBox(ctn->chkbx);
		ctn = ctn->pNext;
	}
}

void SDL_NumberPickerAutoAdjustButtons(SDL_NumberPicker *npk)
{
	if ( npk ) {
		npk->pDecreaseButton->w = ( double ) npk->w / 3 - npk->padding * 2;
		npk->pDecreaseButton->h = npk->h - npk->padding * 2;
		npk->pDecreaseButton->y = npk->y + npk->padding;
		npk->pDecreaseButton->x = npk->x + npk->padding;
		npk->pIncreaseButton->h = npk->h - npk->padding * 2;
		npk->pIncreaseButton->y = npk->y + npk->padding;
		npk->pIncreaseButton->w = npk->pDecreaseButton->w;
		npk->pIncreaseButton->x = npk->x + npk->pDecreaseButton->w + npk->padding*2;
	}
}

SDL_Interface * SDL_CreateInterfaceAtXY(int x, int y, int w, int h)
{
	SDL_Interface * inf = SDL_CreateInterface(w, h);
	inf->x = x;
	inf->y = y;
	return inf;
}

SDL_Interface * SDL_CreateInterface( int w, int h )
{
	SDL_Interface * newIntf = ( SDL_Interface * ) malloc(sizeof(SDL_Interface));
	newIntf->interfaceColumnNumber = 0;
	newIntf->interfaceItemBGColor = DEFAULT_BUTTON_BG_COLOR;
	newIntf->interfaceItemTextColor = DEFAULT_BUTTON_TEXT_COLOR;
	newIntf->interfaceFont = DEFAULT_BUTTON_FONT;
	newIntf->interfaceItemCount = 0;
	newIntf->interfaceItemPadding = DEFAULT_BUTTON_PADDING;
	newIntf->interfaceLayoutType = IL_LIST;
	newIntf->interfaceMargin = DEFAULT_BUTTON_PADDING;
	if ( w == -1 )
	{
		newIntf->interfaceTotalWidth = SCREEN_WIDTH/2;
	}
	else
	{
		newIntf->interfaceTotalWidth = w;
	}
	if ( h == -1 )
	{
		newIntf->interfaceTotalHeight = SCREEN_HEIGHT / 2;
	}
	else
	{
		newIntf->interfaceTotalHeight = h;
	}
	newIntf->mItemType = IIT_NONE;
	newIntf->pHead = NULL;
	return newIntf;
}

void SDL_DestroyInterface(SDL_Interface *interface)
{
	/*Free the contents of the interface*/
	SDL_InterfaceItemContainer *tmp, *head;
	head = interface->pHead;
	while ( head )
	{
		tmp = head;
		head = head->pNext;
		SDL_DestroyInterfaceItemContainer(tmp);
	}
	/*Free the interface.*/
	free(interface);
}

void SDL_DestroyInterfaceItemContainer(SDL_InterfaceItemContainer *ctn)
{
	switch ( ctn->type )
	{
	case IIT_BTN:
		SDL_DestroyButton(ctn->button);
		break;
	case IIT_NPK:
		SDL_DestroyNumberPicker(ctn->npk);
		break;
	case IIT_CHB:
		SDL_DestroyCheckBox(ctn->chkbx);
		break;
	case IIT_NONE:
	default:
		break;
	}
	free(ctn);
}

/*Find the tail (last item) of the interface.*/
SDL_InterfaceItemContainer * SDL_GetInterfaceTail(SDL_Interface *interface)
{
	SDL_InterfaceItemContainer * tail = interface->pHead;
	/*Advance through the list until there is no next item.*/
	while ( tail && tail->pNext )
	{
		tail = tail->pNext;
	}
	/*Return the tail of the list.*/
	return tail;
}


/*Draw a box grid to illustration neighborhood definition*/
void SDL_DrawBoxGrid(SDL_Renderer * pRenderer, int x, int y, int wh, int n, int space, SDL_Color pBoxColor, SDL_Color pTextColor, TTF_Font *font)
{
	SDL_Color pBGColor = pBoxColor;
	SDL_ColorAdjHSV(&pBGColor, 0, 0, -.2);
	SDL_DrawFillRectHelper(pRenderer, x + 2, y + 2, wh, wh, pBGColor);
	for ( int i = 0; i < n; i++ )
	{
		for ( int j = 0; j < n; j++ )
		{
			SDL_DrawFillRectHelper(
				pRenderer,
				x + j * ( ( double ) wh ) / n + ( double ) space / 2,
				y + i * ( ( double ) wh ) / n + ( double ) space / 2,
				( ( double ) wh ) / n - ( double ) space / 2,
				( ( double ) wh ) / n - ( double ) space / 2,
				pBoxColor
			);
			if ( ( i != n / 2 ) || ( j != n / 2 ) )
				SDL_DrawTextAtXYStretched(
					pRenderer,
					"N",
					font,
					( int ) ( x + j * ( ( double ) wh ) / n + ( double ) space ),
					( int ) ( y + i * ( ( double ) wh ) / n + ( double ) space ),
					( int ) ( ( ( double ) wh ) / n - ( double ) space * 2 ),
					( int ) ( ( ( double ) wh ) / n - ( double ) space * 2 ),
					pTextColor
				);

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
	npk->padding = DEFAULT_BUTTON_PADDING;
	npk->y = y;
	npk->font = font;
	npk->minValue = minValue;
	npk->maxValue = maxValue;
	npk->currentValue = minValue;
	npk->incrementSize = 1;
	sprintf_s(npk->caption, MAX_BUTTON_STRING_LENGTH, "%s", caption);
	npk->hasShadow = 1;
	int padding = 2;
	npk->pDecreaseButton = SDL_CreateStandardButton(npk->x + padding, npk->y + padding, ( int ) ( ( double ) npk->w / 3 ) - padding * 2, npk->h - padding * 2, "-");
	npk->pDecreaseButton->buttonPadding = padding;
	npk->pIncreaseButton = SDL_CreateStandardButton(npk->x + padding + ( int ) ( ( double ) npk->w / 3 ), npk->y + padding, ( int ) ( ( double ) npk->w / 3 - padding * 2 ), npk->h - padding * 2, "+");
	npk->pIncreaseButton->buttonPadding = padding;
	return npk;
}

void SDL_RenderNumberPicker(SDL_Renderer * pRenderer, SDL_NumberPicker * npk)
{
	if ( npk->hasShadow )
	{
		/*Render number picker background*/
		SDL_Color shadowColor = { 100,100,100,( Uint8 ) 128 };
		/*Do not need padding for shadow. "Don't pad the shads." - Jesus*/
		SDL_DrawFillRectHelper(pRenderer, npk->x + 2, npk->y + 2, npk->w, npk->h, shadowColor);
	}
	/*Render number picker background*/
	SDL_DrawFillRectHelper(pRenderer, npk->x, npk->y , npk->w , npk->h , { 100,100,100,255 });
	SDL_RenderButton(pRenderer, npk->pIncreaseButton);
	SDL_RenderButton(pRenderer, npk->pDecreaseButton);
	char * string = ( char* ) malloc(4 * sizeof(char));
	_ltoa_s(npk->currentValue, string, 4, 10);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		string,
		npk->font,
		npk->padding+npk->pIncreaseButton->buttonPadding + npk->x + ( int ) ( ( double ) npk->w * 2 / ( double ) 3 ),
		npk->padding + npk->pIncreaseButton->buttonPadding + npk->y,
		( int ) ( ( double ) npk->w / ( double ) 3 ) - npk->pIncreaseButton->buttonPadding * 2 - npk->padding * 2,
		npk->h - npk->pIncreaseButton->buttonPadding * 2- npk->padding *2,
		npk->pIncreaseButton->buttonTextColor
	);
	int text_width, text_height;
	TTF_SizeText(npk->font, npk->caption, &text_width, &text_height);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		npk->caption,
		npk->font,
		npk->x + npk->w + npk->pIncreaseButton->buttonPadding * 2 + npk->padding,
		npk->pIncreaseButton->buttonPadding * 2 + npk->y + npk->padding,
		text_width / 2,
		npk->h - npk->pIncreaseButton->buttonPadding * 2-npk->padding*2,
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
	if ( pNpk )
	{
		if ( pNpk->pIncreaseButton )
			free(pNpk->pIncreaseButton);
		if ( pNpk->pDecreaseButton )
			free(pNpk->pDecreaseButton);
		free(pNpk);
	}
}

/* (WRAPPER) Create and return a button*/
SDL_CustomButton * SDL_CreateStandardButton(int x, int y, int w, int h, const char * pButtonText)
{
	SDL_CustomButton * btn = SDL_CreateButtonDetailed(
		x, y, w, h,
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
	btn->buttonPadding = getMin(0, pButtonPadding);
	btn->outlineEnabled = pButtonOutlineEnabled;
	btn->shadowOffset = pShadowOffset;

	/*Set fade values*/
	btn->buttonFadeState = BFS_FADEIN;
	btn->fadeSpeed = -1;
	btn->fadeValue = 0;

	/*Set corner rounded values.*/
	btn->cornersRounded = 0;
	btn->cornerRadius = 0;

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
		actualShadowColor.a *= ( double ) ( pButton->buttonTotalOpacity ) / 255;
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
	if ( pButton->cornersRounded )
	{
		SDL_DrawFillRectRoundedHelper(pRenderer, pButton->x, pButton->y, pButton->w, pButton->h, actualButtonColor, pButton->cornerRadius);
		SDL_DrawTextAtXYStretched(
			pRenderer,
			pButton->buttonText,
			pButton->buttonFont,
			pButton->x + pButton->buttonPadding + ( double ) pButton->cornerRadius / 2,
			pButton->y + pButton->buttonPadding + ( double ) pButton->cornerRadius / 2,
			pButton->w - pButton->buttonPadding * 2 - ( double ) pButton->cornerRadius / 2,
			pButton->h - pButton->buttonPadding * 2 - ( double ) pButton->cornerRadius / 2,
			actualButtonTextColor
		);

	}
	else
	{
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


	}
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
	if ( SDL_MouseInButtonBounds(mouse_x, mouse_y, pButton) && pButton->buttonState != BS_CLICK )
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
	case BFS_FADEIN:
		if ( pButton->fadeValue + pButton->fadeSpeed < 255 && pButton->fadeSpeed > 0 )
		{
			pButton->fadeValue += pButton->fadeSpeed;
		}
		else
		{
			pButton->fadeValue = 255;
			pButton->buttonFadeState = BFS_STATIC;
		}
		break;
	case BFS_FADEOUT:
		if ( pButton->fadeValue - pButton->fadeSpeed > 0 && pButton->fadeSpeed > 0 )
		{
			pButton->fadeValue -= pButton->fadeSpeed;
		}
		else
		{
			pButton->fadeValue = 0;
			pButton->buttonFadeState = BFS_STATIC;
			pButton->isActive = false;
		}
		break;
	case BFS_STATIC:
	default:
		break;
	}
}

/*Returns 1 if button specified has been clicked*/
int SDL_ButtonClicked(SDL_CustomButton * pButton, SDL_Event * e)
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
	return SDL_MouseInBox(
		x,
		y,
		pButton->x,
		pButton->y,
		pButton->x + pButton->w,
		pButton->y + pButton->h
	);
}

/*Returns 1 if mouse is over number picker specified*/
int SDL_MouseInNumberPickerBounds(int x, int y, SDL_NumberPicker * npk)
{
	return SDL_MouseInBox(
		x,
		y,
		npk->x,
		npk->y,
		npk->x + npk->w,
		npk->y + npk->h
	);
}

/*Returns 1 if mouse is over check box specified*/
int SDL_MouseInCheckBoxBounds(int x, int y, SDL_CheckBox * chb)
{
	return SDL_MouseInBox(
		x,
		y,
		chb->x,
		chb->y,
		chb->x + chb->w,
		chb->y + chb->h
	);
}

/*Returns 1 if the mouse is in the box*/
int SDL_MouseInBox(int mouse_x, int mouse_y, int x1, int y1, int x2, int y2)
{
	if (
		mouse_x >= x1 &&
		mouse_x <= x2 &&
		mouse_y >= y1 &&
		mouse_y <= y2
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
	return ( int ) ( colorAsHSV.h / 360 * 255 );
}

/*Get a color's value*/
int SDL_ColorGetVal(SDL_Color pColor)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 });
	return ( int ) ( colorAsHSV.v * 255 );
}

/*Get a color's saturation*/
int SDL_ColorGetSat(SDL_Color pColor)
{
	hsv colorAsHSV = rgb2hsv({ ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 });
	return ( int ) ( colorAsHSV.s * 255 );
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
	for ( int i = -r-1; i < r+1; i++ )
	{
		for ( int j = -r-1; j < r+1; j++ )
		{
			if ( i * i + j * j < r_squared )
			{
				SDL_RenderDrawPoint(pRenderer, x+i, y+j);
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