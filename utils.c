#include "utils.h"

/*
get an integer from the user within certain bounds
@param lower_bound
the lowest acceptable integer
@param upper_bound
the highest acceptable integer
@param inclusive
1 or 0, whether the endpoints are themselves acceptable
*/
int getIntBounded(int pLowerBound, int pUpperBound, int isInclusive)
{
	int isValidInput = 0;
	int userGeneratedInteger;
	while ( !isValidInput )
	{
		isValidInput = scanf_s("%d", &userGeneratedInteger);
		if ( !isValidInput )
		{
			clearKeyboardBuffer( );
			printf("Sorry, that's not a valid input. Try again: ");
		}
		else
		{
			//check for out-of-bounds inclusive
			if ( isInclusive && ( ( userGeneratedInteger > pUpperBound || userGeneratedInteger < pLowerBound ) ) )
			{
				printf("Sorry, that's out of bounds [%d-%d].", pLowerBound, pUpperBound);
				//enable next iteration
				isValidInput = 0;
				//check for out-of-bounds exclusive
			}
			else if ( !isInclusive && ( ( userGeneratedInteger >= pUpperBound || userGeneratedInteger <= pLowerBound ) ) )
			{
				printf("Sorry, that's out of bounds (%d-%d).", pLowerBound, pUpperBound);
				//enable next iteration
				isValidInput = 0;
			}
		}
	}
	clearKeyboardBuffer( );
	return userGeneratedInteger;
}


/*clear the Keyboard Buffer*/
void clearKeyboardBuffer(void)
{
	char ch = 'a';
	while ( ch != '\n' )
	{
		scanf_s("%c", &ch);
	}
}

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