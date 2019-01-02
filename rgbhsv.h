#ifndef _RGBHSV_H_
#define _RGBHSV_H_
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

extern  hsv  rgb2hsv(rgb);
extern rgb	hsv2rgb(hsv);

#endif // !_RGBHSV_H_
