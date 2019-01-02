#ifndef _SDL_GUI_DD_H_
#define _SDL_GUI_DD_H_

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <SDL_ttf.h>
#include <string.h>
//#include "colors.h"

#define MAX_BUTTON_STRING_LENGTH 128


/*SDL GUI Constants*/

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
	int		shadowEnabled;
	int		outlineEnabled;
	int		buttonMouseoverTintEnabled;
	enum		ButtonMouseoverTintStyle buttonMouseoverTintStyle;
	enum		ButtonTextAlignment pButtonTextAlignment;
	char		buttonText[ MAX_BUTTON_STRING_LENGTH ];
	int			shadowOffset;
	int		isActive;
	SDL_Color	buttonMouseoverBackgroundColor;
	SDL_Color	buttonMouseoverTextColor;
	SDL_Color	buttonClickBackgroundColor;
	SDL_Color	buttonClickTextColor;
	SDL_Color	buttonToggleBackgroundColor;
	SDL_Color	buttonToggleTextColor;
	int		cornersRounded;
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
	int		shadowEnabled;
	SDL_Color	shColor;
	TTF_Font	*font;
	char 		caption[ MAX_BUTTON_STRING_LENGTH ];
	int			padding;
} SDL_CheckBox;

typedef struct
{
	SDL_CustomButton *pIncreaseButton;
	SDL_CustomButton *pDecreaseButton;
	int hasShadow;
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


/*Drawing utility declarations*/
void		SDL_DrawTextAtXY(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color);
void		SDL_DrawTextAtXYStretched(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color);
void		SDL_DrawTextShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, SDL_Color color, int doShadow);
void		SDL_DrawFillRectHelper(SDL_Renderer *renderer, int x, int  y, int h, int w, SDL_Color color);
void		SDL_DrawFillRectHelperColorless(SDL_Renderer *pRenderer, int x, int  y, int w, int h);
void		SDL_DrawFillRectRoundedHelper(SDL_Renderer *pRenderer, int x, int y, int w, int h, SDL_Color pColor, int radius);
void		SDL_ButtonsSetDefaultFont(TTF_Font *font);

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
	int						pButtonShadowEnabled,
	int						pButtonMouseoverTintEnabled,
	int						pButtonOutlineEnabled,
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

#endif // !_SDL_GUI_DD_H_


