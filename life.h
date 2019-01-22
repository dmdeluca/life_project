#pragma once
#include "SDL.h"
#include "sdl_color_ops_dd.h"
#include "handycolors.h"

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

/*Life Simulation Settings Structure*/
typedef struct
{
	int UP_THRESH;
	int OP_THRESH;
	int RB_NUMBER;
	int NH_SIZE;
	int N_ROWS;
	int N_COLS;
	int SOIL_VALUE_LIMIT;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	int HOGSIZE;
	int *LIVE_COUNT_PTR;
	int CHANCE_OF_MIRACLE;
	int DOT_STARTING_VALUE;
	int GLOW_ENABLED;
	int MAX_VALUE;
	int ANTFARM;
	SDL_Color DOT_COLOR;
	SDL_Color SOIL_COLOR;
} LifeSetting;

/*Life settings functions*/
void LS_SetBasicRules(LifeSetting *ls, int up, int op, int rb, int nh);
void LS_SetUPThresh(LifeSetting *ls, int input);
void LS_SetOPThresh(LifeSetting *ls, int input);
void LS_SetRBNumber(LifeSetting *ls, int input);
void LS_SetNHSize(LifeSetting *ls, int input);

/*Color functions*/
void LS_SetDotColor(LifeSetting *ls, SDL_Color color);
void LS_SetSoilColor(LifeSetting *ls, SDL_Color color);

void LS_SetDisplayRules(LifeSetting *ls, int nrows, int ncols, int sw, int sh, int af, int gl);
void LS_SetNRows(LifeSetting *ls, int input);
void LS_SetMaxValue(LifeSetting *ls, int input);
void LS_EnableGlow(LifeSetting *ls);
void LS_DisableGlow(LifeSetting *ls);
void LS_EnableAntFarm(LifeSetting *ls);
void LS_DisableAntFarm(LifeSetting *ls);
void LS_SetNCols(LifeSetting *ls, int input);
void LS_SetScreenWidth(LifeSetting *ls, int input);
void LS_SetScreenHeight(LifeSetting *ls, int input);

void LS_SetSoilValLimit(LifeSetting *ls, int input);
void LS_SetHandOfGodSize(LifeSetting *ls, int input);
void LS_SetLiveCountPtr(LifeSetting *ls, int * input);
void LS_SetChanceOfMiracle(LifeSetting *ls, int input);
void LS_SetDotStartValue(LifeSetting *ls, int input);

/*Friend map declarations*/
FriendMap * Life_CreateFriendMap(Dot * pDotArray, int pIndex, int pRowLength, LifeSetting *settings);
void Life_FillFriendMap(FriendMap * pFriendMap, int pFriendRange, Dot * pDotArray, int pIndex, int pRowLength, LifeSetting *settings);
void		Life_FreeFriendMap(FriendMap* fmap);

Dot Life_CalculateNewDotValue(Dot * dots, int i, LifeSetting *settings);

/*Life declarations*/
void Life_RandomizeDots(Dot * dots, int nIndices, int max, LifeSetting *settings);
void Life_IterateGeneration(Dot * dots, Dot * new_dots, LifeSetting *settings);
void Life_CalculateAllNewDotValues(Dot * dots, Dot * new_dots, LifeSetting *settings);
void Life_UpdateDotValues(Dot * pDotArray, Dot * pUpdateDotArray, LifeSetting *settings);
void Life_DrawAllDots(Dot * dots, SDL_Renderer * renderer, LifeSetting *settings);
void Life_DrawDot(Dot * dots, int i, SDL_Renderer * pRenderer, int rows, int cols, int screenw, int screenh, int max_val, SDL_Color dotcolor, SDL_Color soilColor, int isSoil, int glow);
void Life_ActivateDotByIndex(int i, Dot * dots, LifeSetting *settings);
void Life_DrawSoil(Dot * dots, SDL_Renderer * renderer, LifeSetting *settings);
void Life_KillDotByIndex(int i, Dot * dots, LifeSetting *settings);
int Life_GetIndexByXY(int x, int y, LifeSetting *settings);
void Life_PutHandOfGodAtXY(int x, int y, Dot * dots, LifeSetting *settings);
void Life_SetAllDots(Dot * dots, int nIndices, int value);
void Life_UpdateSoil(Dot * pLifeDotArray, Dot * pSoilDotArray, LifeSetting *settings);
Dot * Life_CreateDotArray(int array_size);
void Life_DestroyDotArray(Dot * dotArray);
void Life_DrawAllDots_Internal(Dot * pDotArray, SDL_Renderer * pRenderer, int rows, int cols, int screenw, int screenh, int max_val, SDL_Color dotcolor, SDL_Color soilColor, int isSoil, int glow);