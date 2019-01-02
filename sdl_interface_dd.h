#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include "handycolors.h"
#include "utilities_dd.h"
#include "va_minmax.h"
#include "sdl_color_ops_dd.h"
#include "sdl_helpers_dd.h"
#include <stdlib.h>

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

/*Button default values*/
extern TTF_Font		*DEFAULT_BUTTON_FONT;
extern const int	DEFAULT_BUTTON_PADDING;
extern SDL_Color	DEFAULT_BUTTON_TEXT_COLOR;
extern SDL_Color	DEFAULT_BUTTON_BG_COLOR;
extern SDL_Color	DEFAULT_BUTTON_MO_TINT_COLOR;
extern SDL_Color	DEFAULT_BUTTON_MO_OUTLINE_COLOR;
extern SDL_Color	DEFAULT_BUTTON_SHADOW_COLOR;
extern enum ButtonTextAlignment DEFAULT_BUTTON_TEXT_ALIGNMENT;


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
	char		buttonText[ 128 ];
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
	char 		caption[ 128 ];
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
	char caption[ 128 ];
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


/*Button function declarations*/
extern SDL_CustomButton * SDL_CreateStandardButton(int x, int y, int w, int h, const char * pButtonText);
extern SDL_CustomButton * SDL_CreateButtonDetailed(
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
extern void SDL_RenderButton(SDL_Renderer *pRenderer, SDL_CustomButton * pButton);
extern void SDL_UpdateButton(SDL_CustomButton * pButton);
extern int SDL_ButtonClicked(SDL_CustomButton * pButton, SDL_Event * e);
extern void SDL_ButtonHandleMouseover(SDL_CustomButton * btn, int mouse_x, int mouse_y);
extern void SDL_DestroyButton(SDL_CustomButton * pButton);

/*Mouse helper functions declarations*/
extern int SDL_MouseInButtonBounds(int x, int y, SDL_CustomButton * pButton);
extern int SDL_MouseInNumberPickerBounds(int x, int y, SDL_NumberPicker * npk);
extern int SDL_MouseInCheckBoxBounds(int x, int y, SDL_CheckBox * chb);
extern int SDL_MouseInBox(int mouse_x, int mouse_y, int x1, int y1, int x2, int y2);

/*Text declarations*/
extern void SDL_DrawTextAtXYStretchedColorShadow(SDL_Renderer * renderer, const char * string, TTF_Font * font, int x, int y, int w, int h, SDL_Color color, SDL_Color shadowColor, int offset);

/*Number Picker declarations*/
extern SDL_NumberPicker * SDL_CreateNumberPicker(int x, int y, int w, int h, int minValue, int maxValue, TTF_Font * font, const char * caption);
extern void SDL_DestroyNumberPicker(SDL_NumberPicker * pNpk);
extern void SDL_RenderNumberPicker(SDL_Renderer * pRenderer, SDL_NumberPicker * npk);
extern void SDL_UpdateNumberPicker(SDL_NumberPicker * npk);
extern void SDL_NumberPickerHandleMouseover(SDL_NumberPicker * npk, int mouse_x, int mouse_y);
extern void SDL_NumberPickerHandleClick(SDL_NumberPicker * npk, SDL_Event *e);
extern void SDL_NumberPickerAutoAdjustButtons(SDL_NumberPicker *npk);

/*Check box declarations*/
extern SDL_CheckBox * SDL_CreateCheckBox(int x, int y, int w, int h, enum CheckBoxState startValue, TTF_Font * font, const char * caption);
extern void SDL_DestroyCheckBox(SDL_CheckBox * chb);
extern void SDL_RenderCheckBox(SDL_Renderer * pRenderer, SDL_CheckBox * chb);
extern void SDL_UpdateCheckBox(SDL_CheckBox * chb);
extern int	SDL_CheckHandleClick(SDL_CheckBox *chb, SDL_Event *e);

/*Other declarations*/
extern void SDL_DrawBoxGrid(SDL_Renderer * pRenderer, int x, int y, int wh, int n, int space, SDL_Color pBoxColor, SDL_Color pTextColor, TTF_Font *font);

/*Interface and interface container declarations*/
extern SDL_InterfaceItemContainer * SDL_CreateInterfaceItemContainer(enum SDL_InterfaceItemType type);
extern SDL_InterfaceItemContainer * SDL_GetInterfaceTail(SDL_Interface *interface);
extern void SDL_InterfaceAutoArrangeItems(SDL_Interface *interface);
extern void SDL_DestroyInterface(SDL_Interface *interface);
extern void SDL_DestroyInterfaceItemContainer(SDL_InterfaceItemContainer *ctn);
extern SDL_Interface * SDL_InterfaceAddNewButton(SDL_Interface *interface);
extern SDL_Interface * SDL_InterfaceAddNewNumberPicker(SDL_Interface *interface);
extern SDL_Interface * SDL_InterfaceAddNewGenericItem(SDL_Interface *interface, enum SDL_InterfaceItemType type);
extern int SDL_InterfaceCountItems(SDL_Interface *interface);
extern void SDL_DrawInterface(SDL_Renderer *pRenderer, SDL_Interface * interface);
extern void SDL_DrawInterfaceItem(SDL_Renderer *pRenderer, SDL_InterfaceItemContainer * ctn);
extern SDL_Interface * SDL_CreateInterfaceAtXY(int x, int y, int w, int h);
extern SDL_Interface * SDL_CreateInterface(int w, int h);
extern SDL_Interface * SDL_InterfaceAddButton(SDL_Interface * interface, SDL_CustomButton * button);
extern SDL_Interface * SDL_InterfaceAddNumberPicker(SDL_Interface * interface, SDL_NumberPicker * npk);
extern void SDL_UpdateInterface(SDL_Interface * pInterface);
extern void SDL_InterfaceHandleMouseover(SDL_Interface *pInterface, int mx, int my);
extern SDL_Interface * SDL_InterfaceAddCheckBox(SDL_Interface * interface, SDL_CheckBox * chkbx);

