#include "life.h"

void LS_SetBasicRules(LifeSetting *ls, int up, int op, int rb, int nh)
{
	LS_SetOPThresh(ls, op);
	LS_SetUPThresh(ls, up);
	LS_SetRBNumber(ls, rb);
	LS_SetNHSize(ls, nh);
}

Dot * Life_CreateDotArray(int array_size)
{
	return ( Dot* ) malloc(sizeof(Dot)*array_size);
}

void Life_DestroyDotArray(Dot * dotArray)
{
	if ( dotArray )
		free(dotArray);
}

void LS_SetDisplayRules(LifeSetting *ls, int nrows, int ncols, int sw, int sh, int af, int gl)
{

	/*Set game and display dimensions*/
	LS_SetNRows(ls, nrows);
	LS_SetNCols(ls, ncols);
	LS_SetScreenWidth(ls, sw);
	LS_SetScreenHeight(ls, sh);

	/*Ant Farm Settings*/
	if ( af )
	{
		LS_EnableAntFarm(ls);
	}
	else
	{
		LS_DisableAntFarm(ls);
	}
	/*Glow*/
	if ( gl )
	{
		LS_EnableGlow(ls);
	}
	else
	{
		LS_DisableGlow(ls);
	}
}

void LS_EnableGlow(LifeSetting *ls)
{
	ls->GLOW_ENABLED = 1;
}

void LS_DisableGlow(LifeSetting *ls)
{
	ls->GLOW_ENABLED = 0;
}

void LS_SetMaxValue(LifeSetting *ls, int input)
{
	ls->MAX_VALUE = input;
}

void LS_EnableAntFarm(LifeSetting *ls)
{
	ls->ANTFARM = 1;
}

void LS_DisableAntFarm(LifeSetting *ls)
{
	ls->ANTFARM = 0;
}

void LS_SetUPThresh(LifeSetting *ls, int input)
{
	ls->UP_THRESH = input;
}
void LS_SetOPThresh(LifeSetting *ls, int input)
{
	ls->OP_THRESH = input;
}
void LS_SetRBNumber(LifeSetting *ls, int input)
{
	ls->RB_NUMBER = input;
}
void LS_SetNHSize(LifeSetting *ls, int input)
{
	ls->NH_SIZE= input;
}
void LS_SetNRows(LifeSetting *ls, int input)
{
	ls->N_ROWS = input;
}
void LS_SetNCols(LifeSetting *ls, int input)
{
	ls->N_COLS = input;
}
void LS_SetSoilValLimit(LifeSetting *ls, int input)
{
	ls->SOIL_VALUE_LIMIT = input;
}
void LS_SetScreenWidth(LifeSetting *ls, int input)
{
	ls->SCREEN_WIDTH = input;
}
void LS_SetScreenHeight(LifeSetting *ls, int input)
{
	ls->SCREEN_HEIGHT = input;
}
void LS_SetHandOfGodSize(LifeSetting *ls, int input)
{
	ls->HOGSIZE = input;
}
void LS_SetLiveCountPtr(LifeSetting *ls, int * input)
{
	ls->LIVE_COUNT_PTR = input;
}
void LS_SetChanceOfMiracle(LifeSetting *ls, int input)
{
	ls->CHANCE_OF_MIRACLE = input;
}
void LS_SetDotStartValue(LifeSetting *ls, int input)
{
	ls->DOT_STARTING_VALUE = input;
}

/*Color functions*/
void LS_SetDotColor(LifeSetting *ls, SDL_Color color)
{
	ls->DOT_COLOR.r = color.r;
	ls->DOT_COLOR.g = color.g;
	ls->DOT_COLOR.b = color.b;
	ls->DOT_COLOR.a = color.a;
}
void LS_SetSoilColor(LifeSetting *ls, SDL_Color color)
{
	ls->SOIL_COLOR.r = color.r;
	ls->SOIL_COLOR.g = color.g;
	ls->SOIL_COLOR.b = color.b;
	ls->SOIL_COLOR.a = color.a;
}

void Life_UpdateSoil(Dot * pLifeDotArray, Dot * pSoilDotArray, LifeSetting *settings)
{
	for ( int i = 0; i < settings->N_ROWS*settings->N_COLS; i++ )
	{
		if ( pLifeDotArray[ i ].value )
		{
			pSoilDotArray[ i ].value = 0;
		}
		else if ( pSoilDotArray[ i ].value < settings->SOIL_VALUE_LIMIT)
		{
			pSoilDotArray[ i ].value += 1;
		}
	}
}

/* Revive a square at (x,y). */
void Life_PutHandOfGodAtXY(int x, int y, Dot * dots, LifeSetting *settings)
{
	int index = Life_GetIndexByXY(x, y, settings);
	for ( int i = 0; i < settings->HOGSIZE*settings->HOGSIZE; i++ )
	{
		Life_ActivateDotByIndex(getMin(2, index + i / settings->HOGSIZE * settings->N_COLS + i % settings->HOGSIZE, settings->N_ROWS*settings->N_COLS - 1), dots, settings);
	}
}

/*Find the index of a dot in the array by its XY value.*/
int Life_GetIndexByXY(int x, int y, LifeSetting *settings)
{
	return ( int ) ceil(x / ( double ) settings->SCREEN_WIDTH*( settings->N_COLS )) + ( int ) ceil(y / ( double ) settings->SCREEN_HEIGHT*( settings->N_ROWS ))*settings->N_COLS;
}

/*Revive a dot at a certain index in the dot array.*/
void Life_ActivateDotByIndex(int i, Dot * dots, LifeSetting *settings)
{
	if ( !dots[ i ].value )
	{
		dots[ i ].value = 1;
		*(settings->LIVE_COUNT_PTR) += 1;
	}
	else
	{
		/*Do nothing!*/
	}
}

/*Kill a dot at a certain index in the dot array.*/
void Life_KillDotByIndex(int i, Dot * dots, LifeSetting *settings)
{
	dots[ i ].value = 0;
	*( settings->LIVE_COUNT_PTR ) -= 1;
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
void Life_RandomizeDots(Dot * dots, int nIndices, int max, LifeSetting *settings)
{
	*(settings->LIVE_COUNT_PTR) = 0;
	for ( int i = 0; i < nIndices; i++ )
	{
		dots[ i ].value = rand( ) % max;
		if ( dots[ i ].value )
			*(settings->LIVE_COUNT_PTR) += 1;
		dots[ i ].color = randomColor( );
	}
}

/*Calculate new values for current dot array and then update the values.*/
void Life_IterateGeneration(Dot * dots, Dot * new_dots, LifeSetting *settings)
{
	Life_CalculateAllNewDotValues(dots, new_dots,settings);
	Life_UpdateDotValues(dots, new_dots, settings);
}

/*Calculate new values for all dots in the array.*/
void Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots, LifeSetting *settings)
{
	Dot new_dot;
	for ( int i = 0; i < settings->N_ROWS*settings->N_COLS; i++ )
	{
		new_dot = Life_CalculateNewDotValue(dots, i,settings);
		new_dots[ i ].value = new_dot.value;
		new_dots[ i ].color = new_dot.color;
	}
}


/* Calculate new value for a single dot in the array.*/
Dot Life_CalculateNewDotValue(Dot * dots, int i, LifeSetting *settings)
{
	/*Capture the dot of interest.*/
	Dot newDot = dots[ i ];
	/*Store the old value and color of the dot.*/
	int oldValue = dots[ i ].value;
	/*Create a friendmap structure of nearby friends so that color averaging is possible later.*/
	FriendMap * fMap = Life_CreateFriendMap(dots, i, settings->N_COLS,settings);
	/*Capture number of friends, then evaluate.*/
	int numFriends = ( int ) round(fMap->count);
	if ( oldValue != 0 ) //	meaning the cell is ALIVE
	{
		if ( numFriends < settings->UP_THRESH || numFriends > settings->OP_THRESH )
		{
			/*Cell has died due to under- or overpopulation.*/
			newDot.value = 0;
			*(settings->LIVE_COUNT_PTR)-= 1;
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
				*(settings->LIVE_COUNT_PTR) -= 1;
			}
		}
	}
	else
	{
		/*Cell is currently dead.*/
		if ( numFriends == settings->RB_NUMBER)
		{
			/*Cell is born.*/
			newDot.value = 1;
			*(settings->LIVE_COUNT_PTR) += 1;

			/*Cell becomes the average of its [rebirth_number] parents colors.*/
			SDL_Color *friendColorArray = ( SDL_Color* ) malloc(settings->RB_NUMBER * sizeof(SDL_Color));
			int friendColorIndex = 0;
			for ( int k = 0; k < fMap->size && friendColorIndex < settings->RB_NUMBER; k++ )
			{
				if ( fMap->dotArray[ k ].value )
				{
					friendColorArray[ friendColorIndex ] = fMap->dotArray[ k ].color;
					friendColorIndex++;
				}
			}
			/*Not sure how to do this*/
			switch ( settings->RB_NUMBER )
			{
			case 1:
				newDot.color = friendColorArray[ 0 ];
				break;
			case 2:
				newDot.color = averageColor(
					settings->RB_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ]
				);
				break;
			case 3:
				newDot.color = averageColor(
					settings->RB_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ]
				);				break;
			case 4:
				newDot.color = averageColor(
					settings->RB_NUMBER,
					friendColorArray[ 0 ],
					friendColorArray[ 1 ],
					friendColorArray[ 2 ],
					friendColorArray[ 3 ]
				);				break;
			case 5:
				newDot.color = averageColor(
					settings->RB_NUMBER,
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
					settings->RB_NUMBER,
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
		else if ( rand( ) % ( settings->CHANCE_OF_MIRACLE+ 1 ) < settings->CHANCE_OF_MIRACLE - 1 )
		{
			/*Cell remains dead.*/
			newDot.value = 0;
			newDot.color = randomColor( );
		}
		else
		{
			/*Immaculate conception.*/
			newDot.value = 1;
			*(settings->LIVE_COUNT_PTR)+= 1;
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
FriendMap * Life_CreateFriendMap(Dot * pDotArray, int pIndex, int pRowLength, LifeSetting *settings)
{
	/*Create the friend map.*/
	FriendMap * fMap = ( FriendMap* ) malloc(sizeof(FriendMap));
	/*Populate the friend map.*/
	Life_FillFriendMap(fMap, settings->NH_SIZE, pDotArray, pIndex, pRowLength, settings);
	/*Return the friend map.*/
	return fMap;
}

void Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, LifeSetting *settings)
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
		int offset_index = ( pIndex - pFriendRange + i % fmap_width + ( i / fmap_width - pFriendRange )*pRowLength + settings->N_ROWS*settings->N_COLS ) % ( settings->N_ROWS*settings->N_COLS );
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
void Life_UpdateDotValues(Dot * pDotArray, Dot * pUpdateDotArray, LifeSetting *settings)
{
	for ( int i = 0; i < settings->N_ROWS*settings->N_COLS; i++ )
	{
		/* Calculate new values. */
		pDotArray[ i ].value = pUpdateDotArray[ i ].value;
		pDotArray[ i ].color = pUpdateDotArray[ i ].color;
	}
}

/*Draw all dots in dot array.*/
void Life_DrawAllDots(Dot * pDotArray, SDL_Renderer * pRenderer, LifeSetting *settings)
{
	Life_DrawAllDots_Internal(
		pDotArray, 
		pRenderer, 
		settings->N_ROWS, 
		settings->N_COLS, 
		settings->SCREEN_WIDTH, 
		settings->SCREEN_HEIGHT, 
		settings->MAX_VALUE, 
		settings->DOT_COLOR,
		settings->SOIL_COLOR,
		0,
		settings->GLOW_ENABLED
	);
}

void Life_DrawAllDots_Internal(
	Dot * pDotArray,
	SDL_Renderer * pRenderer,
	int rows,
	int cols,
	int screenw,
	int screenh,
	int max_val,
	SDL_Color dotcolor,
	SDL_Color soilColor,
	int isSoil,
	int glow)
{
	for ( int i = 0; i < rows*cols; i++ )
	{
		Life_DrawDot(
			pDotArray, 
			i, 
			pRenderer, 
			rows,
			cols,
			screenw,
			screenh,
			max_val,
			dotcolor,
			soilColor,
			isSoil,
			glow
		);
	}
}

/*Draw a dot on the screen from a dot or soil array.*/
void Life_DrawDot(Dot * dots, int i,   SDL_Renderer * pRenderer,  int rows,	  int cols, int screenw, int screenh, int max_val,  SDL_Color dotcolor, SDL_Color soilColor,  int isSoil, int glow)
{

	if ( dots[ i ].value )
	{
		int alpha = ( int ) ( 255 * 0.6108*0.6108 ); // #goldenratio?!??

		/*Draw two extra rectangles behind the white part to simulate glow.*/
		if ( glow && !isSoil )
		{
			/*Draw a 1x3 rectangle*/
			SDL_Color color = { dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, alpha };
			SDL_DrawFillRectHelper(
				pRenderer,
				( int ) ( ( double ) screenw / cols )*( ( i - 1 ) % cols ),
				( int ) ( ( double ) screenh / rows )*( ( i ) / cols ),
				( int ) ( ( double ) screenw / cols * 3 ),
				( int ) ( ( double ) screenh / rows ),
				color
			);
			/*Draw a 3x1 rectangle*/
			SDL_DrawFillRectHelper(
				pRenderer,
				( int ) ( ( double ) screenw / cols )*( ( i ) % cols ),
				( int ) ( ( double ) screenh / rows )*( ( i ) / cols - 1 ),
				( int ) ( ( double ) screenw / cols ),
				( int ) ( ( double ) screenh / rows * 3 ),
				color
			);
		}
		else
		{
			SDL_SetRenderDrawColor(pRenderer, dots[ i ].color.r, dots[ i ].color.g, dots[ i ].color.b, 255 / max_val);
		}

		/*Draw the white part of the dot.*/
		SDL_Color color;
		if ( isSoil )
		{
			color = soilColor;
			color.a = dots[ i ].value;
		}
		else
		{
			/*Blinding white*/
			color = dotcolor;
		}
		SDL_DrawFillRectHelper(
			pRenderer,
			( int ) ( ( double ) screenw / cols )*( ( i ) % cols ),
			( int ) ( ( double ) screenh / rows )*( i / cols ),
			( int ) ( ( double ) screenw / cols ),
			( int ) ( ( double ) screenh / rows ),
			color
		);

	}

}

/*Draw soil in soil array.*/
void Life_DrawSoil(Dot * soilArray, SDL_Renderer * pRenderer, LifeSetting *settings)
{
	Life_DrawAllDots_Internal(
		soilArray,
		pRenderer,
		settings->N_ROWS,
		settings->N_COLS,
		settings->SCREEN_WIDTH,
		settings->SCREEN_HEIGHT,
		settings->MAX_VALUE,
		settings->DOT_COLOR,
		settings->SOIL_COLOR,
		1,
		settings->GLOW_ENABLED
	);
}

