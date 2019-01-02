#include "sdl_interface_dd.h"

/*Button default values*/
 TTF_Font		*DEFAULT_BUTTON_FONT = NULL;
 const int	DEFAULT_BUTTON_PADDING = 2;
 SDL_Color	DEFAULT_BUTTON_TEXT_COLOR = { 0,0,0,255 };
 SDL_Color	DEFAULT_BUTTON_BG_COLOR = { 255,255,255,255 };
 SDL_Color	DEFAULT_BUTTON_MO_TINT_COLOR = { 255,0,0,255 };
 SDL_Color	DEFAULT_BUTTON_MO_OUTLINE_COLOR = { 128,128,128,255 };
 SDL_Color	DEFAULT_BUTTON_SHADOW_COLOR = { 64,64,64,255 };
 enum ButtonTextAlignment DEFAULT_BUTTON_TEXT_ALIGNMENT = BTA_CENT;

SDL_CheckBox * SDL_CreateCheckBox(int x, int y, int w, int h, enum CheckBoxState startValue, TTF_Font * font, const char * caption)
{
	SDL_CheckBox *chb = ( SDL_CheckBox* ) malloc(sizeof(SDL_CheckBox));
	chb->bgColor = DEFAULT_BUTTON_BG_COLOR;
	chb->ckColor = DEFAULT_BUTTON_TEXT_COLOR;
	chb->fgColor = cWhite;
	chb->h = h;
	chb->w = w;
	chb->padding = DEFAULT_BUTTON_PADDING;
	chb->x = x;
	chb->y = y;
	chb->font = DEFAULT_BUTTON_FONT;
	chb->shadowEnabled = 1;
	chb->shColor = DEFAULT_BUTTON_SHADOW_COLOR;
	sprintf_s(chb->caption, 128, "%s", caption);
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
		chb->x + chb->w + chb->padding * 2,
		chb->padding + chb->y,
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
		SDL_DrawInterfaceItem(pRenderer, head);
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
			ctn->button->y = ( int ) ( interface->y + ( double ) interface->interfaceTotalHeight * ( double ) itemsArranged / itemsToArrange + interface->interfaceMargin );
			ctn->button->h = ( int ) ( ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2 );
		}
		else if ( ctn->npk )
		{
			ctn->npk->x = interface->x + interface->interfaceMargin;
			//	[Do not change width for now.]
			ctn->npk->y = ( int ) ( interface->y + ( double ) interface->interfaceTotalHeight * ( double ) itemsArranged / itemsToArrange + interface->interfaceMargin );
			ctn->npk->h = ( int ) ( ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2 );
			SDL_NumberPickerAutoAdjustButtons(ctn->npk);
		}
		else if ( ctn->chkbx )
		{
			ctn->chkbx->x = interface->x + interface->interfaceMargin;
			//	[Do not change width for now.]
			ctn->chkbx->y = ( int ) ( interface->y + ( double ) interface->interfaceTotalHeight * ( double ) itemsArranged / itemsToArrange + interface->interfaceMargin );
			ctn->chkbx->h = ( int ) ( ( double ) interface->interfaceTotalHeight / itemsToArrange - interface->interfaceItemPadding * 2 );
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
	if ( npk )
	{
		npk->pDecreaseButton->w = ( int ) round(( double ) npk->w / 3 - npk->padding * 2);
		npk->pDecreaseButton->h = npk->h - npk->padding * 2;
		npk->pDecreaseButton->y = npk->y + npk->padding;
		npk->pDecreaseButton->x = npk->x + npk->padding;
		npk->pIncreaseButton->h = npk->h - npk->padding * 2;
		npk->pIncreaseButton->y = npk->y + npk->padding;
		npk->pIncreaseButton->w = npk->pDecreaseButton->w;
		npk->pIncreaseButton->x = npk->x + npk->pDecreaseButton->w + npk->padding * 2;
	}
}

SDL_Interface * SDL_CreateInterfaceAtXY(int x, int y, int w, int h)
{
	SDL_Interface * inf = SDL_CreateInterface(w, h);
	inf->x = x;
	inf->y = y;
	return inf;
}

SDL_Interface * SDL_CreateInterface(int w, int h)
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
	newIntf->interfaceTotalWidth = w;
	newIntf->interfaceTotalHeight = h;
	/*
	if ( w == -1 )
	{
		newIntf->interfaceTotalWidth = SCREEN_WIDTH / 2;
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
	*/
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
				( int ) ( x + j * ( ( double ) wh ) / n + ( double ) space / 2 ),
				( int ) ( y + i * ( ( double ) wh ) / n + ( double ) space / 2 ),
				( int ) ( ( ( double ) wh ) / n - ( double ) space / 2 ),
				( int ) ( ( ( double ) wh ) / n - ( double ) space / 2 ),
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
	sprintf_s(npk->caption, 128, "%s", caption);
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
	SDL_DrawFillRectHelper(pRenderer, npk->x, npk->y, npk->w, npk->h, cGray);
	SDL_RenderButton(pRenderer, npk->pIncreaseButton);
	SDL_RenderButton(pRenderer, npk->pDecreaseButton);
	char * string = ( char* ) malloc(4 * sizeof(char));
	_ltoa_s(npk->currentValue, string, 4, 10);
	SDL_DrawTextAtXYStretched(
		pRenderer,
		string,
		npk->font,
		npk->padding + npk->pIncreaseButton->buttonPadding + npk->x + ( int ) ( ( double ) npk->w * 2 / ( double ) 3 ),
		npk->padding + npk->pIncreaseButton->buttonPadding + npk->y,
		( int ) ( ( double ) npk->w / ( double ) 3 ) - npk->pIncreaseButton->buttonPadding * 2 - npk->padding * 2,
		npk->h - npk->pIncreaseButton->buttonPadding * 2 - npk->padding * 2,
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
		npk->h - npk->pIncreaseButton->buttonPadding * 2 - npk->padding * 2,
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
		1,
		1,
		0,
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
	int						pButtonShadowEnabled,
	int						pButtonMouseoverTintEnabled,
	int						pButtonOutlineEnabled,
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
	/*
		if ( pButtonXPositionStyle = BUTTON_POSITION_UNDEFINED )
	{
		btn->x = SCREEN_WIDTH / 2 - btn->w / 2;
	}
	else
	{
		btn->x = x;
	}

	// Set y position
	if ( pButtonYPositionStyle = BUTTON_POSITION_UNDEFINED )
	{
		btn->y = SCREEN_HEIGHT / 2 - btn->h / 2;
	}
	else
	{
		btn->y = y;
	}
	
	*/


	btn->x = x;
	btn->y = y;


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
		actualShadowColor.a = ( Uint8 ) ( int ) ( actualShadowColor.a*fadeFactor );
		actualShadowColor.a = ( Uint8 ) ( actualShadowColor.a*( double ) ( pButton->buttonTotalOpacity ) / 255 );
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
			( int ) ( pButton->x + pButton->buttonPadding + ( double ) pButton->cornerRadius / 2 ),
			( int ) ( pButton->y + pButton->buttonPadding + ( double ) pButton->cornerRadius / 2 ),
			( int ) ( pButton->w - pButton->buttonPadding * 2 - ( double ) pButton->cornerRadius / 2 ),
			( int ) ( pButton->h - pButton->buttonPadding * 2 - ( double ) pButton->cornerRadius / 2 ),
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
			pButton->isActive = 0;
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
