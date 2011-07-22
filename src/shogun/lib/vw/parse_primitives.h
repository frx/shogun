#ifndef _VW_PARSEPRIMITIVES_H__
#define _VW_PARSEPRIMITIVES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <shogun/io/SGIO.h>
#include <shogun/lib/v_array.h>
#include <shogun/lib/vw/substring.h>

namespace shogun
{

//chop up the string into a v_array of substring.
void tokenize(char delim, substring s, v_array<substring> &ret);

inline char* safe_index(char *start, char v, char *max)
{
	while (start != max && *start != v)
		start++;
	return start;
}

}
#endif
