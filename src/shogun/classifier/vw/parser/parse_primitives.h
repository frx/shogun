#ifndef _VW_PARSEPRIMITIVES_H__
#define _VW_PARSEPRIMITIVES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shogun/io/SGIO.h>
#include <shogun/lib/v_array.h>
#include <shogun/io/SGIO.h>

namespace shogun
{
	/**
	 * Split a given substring into an array of substrings
	 * based on a specified delimiter
	 *
	 * @param delim delimiter to use
	 * @param s substring to tokenize
	 * @param ret array of substrings, returned
	 */
	void tokenize(char delim, substring s, v_array<substring> &ret);

	/**
	 * Get the index of a character in a memory location
	 * taking care not to go beyond the max pointer.
	 *
	 * @param start start memory location, char*
	 * @param v character to search for
	 * @param max last location to look in
	 *
	 * @return index of found location as char*
	 */
	inline char* safe_index(char *start, char v, char *max)
	{
		while (start != max && *start != v)
			start++;
		return start;
	}

}
#endif
