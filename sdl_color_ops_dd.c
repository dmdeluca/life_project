#include "sdl_color_ops_dd.h"

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
	rgb colorAsRGB = { ( double ) pColor.r / 255, ( double ) pColor.g / 255, ( double ) pColor.b / 255 };
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	return ( int ) ( colorAsHSV.h / 360 * 255 );
}

/*Get a color's value*/
int SDL_ColorGetVal(SDL_Color pColor)
{
	rgb colorAsRGB = { ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 };
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	return ( int ) ( colorAsHSV.v * 255 );
}

/*Get a color's saturation*/
int SDL_ColorGetSat(SDL_Color pColor)
{
	rgb colorAsRGB = { ( double ) pColor.r / 255,( double ) pColor.g / 255,( double ) pColor.b / 255 };
	hsv colorAsHSV = rgb2hsv(colorAsRGB);
	return ( int ) ( colorAsHSV.s * 255 );
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
