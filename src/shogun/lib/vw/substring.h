#ifndef _VW_SUBSTRING_H__
#define _VW_SUBSTRING_H__

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <shogun/io/SGIO.h>

namespace shogun
{
	/**
	 * Substring, specified by start position
	 * and end position.
	 *
	 * Used to mark strings in a buffer, where they
	 * need not be delimited by NUL characters.
	 */
	struct substring
	{
		char *start;
		char *end;
	};

	/**
	 * Return a C string from the substring
	 *
	 * @param s substring
	 *
	 * @return new C string representation
	 */
	inline char* c_string_of_substring(substring s)
	{
		size_t len = s.end - s.start+1;
		char* ret = (char *)calloc(len,sizeof(char));
		memcpy(ret,s.start,len-1);
		return ret;
	}

	/**
	 * Print the substring
	 *
	 * @param s substring
	 */
	inline void print_substring(substring s)
	{
		SG_SPRINT("%s\n", s.start,s.end - s.start);
	}

	/**
	 * Get value of substring as float
	 * (if possible)
	 *
	 * @param s substring
	 *
	 * @return float value of substring
	 */
	inline float float_of_substring(substring s)
	{
		char* endptr = s.end;
		float f = strtof(s.start,&endptr);
		if (endptr == s.start && s.start != s.end)
			SG_SERROR("error: %s is not a float!\n", std::string(s.start, s.end-s.start).c_str());

		return f;
	}

	/**
	 * Return value of substring as double
	 *
	 * @param s substring
	 *
	 * @return substring as double
	 */
	inline float double_of_substring(substring s)
	{
		char* endptr = s.end;
		float f = strtod(s.start,&endptr);
		if (endptr == s.start && s.start != s.end)
			SG_SERROR("Error!:%s is not a double!\n", std::string(s.start, s.end-s.start).c_str());

		return f;
	}

	/**
	 * Integer value of substring
	 *
	 * @param s substring
	 *
	 * @return int value of substring
	 */
	inline int int_of_substring(substring s)
	{
		return atoi(std::string(s.start, s.end-s.start).c_str());
	}

	/**
	 * Unsigned long value of substring
	 *
	 * @param s substring
	 *
	 * @return unsigned long value of substring
	 */
	inline unsigned long ulong_of_substring(substring s)
	{
		return strtoul(std::string(s.start, s.end-s.start).c_str(),NULL,10);
	}

	/**
	 * Length of substring
	 *
	 * @param s substring
	 *
	 * @return length of substring
	 */
	inline unsigned long ss_length(substring s)
	{
		return (s.end - s.start);
	}

}
#endif