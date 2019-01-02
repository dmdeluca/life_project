#ifndef _UTILS_H_
#define _UTILS_H_

#pragma once
#include <stdio.h>
#include <stdarg.h>

#define PHI 0.61803398875
#define PI 3.14159265359

/*
get an integer from the user within certain bounds
@param lower_bound
the lowest acceptable integer
@param upper_bound
the highest acceptable integer
@param inclusive
1 or 0, whether the endpoints are themselves acceptable
*/
int getIntBounded(int pLowerBound, int pUpperBound, int isInclusive);


/*clear the Keyboard Buffer*/
void clearKeyboardBuffer(void);

/*Get the maximum of an arbitrary number of integers.*/
int	getMax(int num, ...);

/*Get the minimum of an arbitrary number of integers.*/
int	getMin(int num, ...);


#endif // !_UTILS_H_

