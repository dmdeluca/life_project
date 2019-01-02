#include "rgb2hsv.h"

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
		out.h = 0.0;                            // its now undefined
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
