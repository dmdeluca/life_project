#include "va_minmax.h"

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