#ifndef _VW_PARSEPRIMITIVES_H__
#define _VW_PARSEPRIMITIVES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <shogun/lib/v_array.h>
#include <shogun/lib/vw/substring.h>

using namespace std;

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

inline void print_substring(substring s)
{
	printf("%s\n", s.start,s.end - s.start);
}

inline float float_of_substring(substring s)
{
	char* endptr = s.end;
	float f = strtof(s.start,&endptr);
	if (endptr == s.start && s.start != s.end)
	{
		printf("error: %s is not a float!\n", string(s.start, s.end-s.start).c_str());
		exit(1);
	}
	return f;
}

inline float double_of_substring(substring s)
{
	char* endptr = s.end;
	float f = strtod(s.start,&endptr);
	if (endptr == s.start && s.start != s.end)
	{
		cout << "error: " << string(s.start, s.end-s.start).c_str() << " is not a double" << endl;
		exit(1);
	}
	return f;
}

inline int int_of_substring(substring s)
{
	return atoi(string(s.start, s.end-s.start).c_str());
}

inline unsigned long ulong_of_substring(substring s)
{
	return strtoul(string(s.start, s.end-s.start).c_str(),NULL,10);
}

inline unsigned long ss_length(substring s)
{
	return (s.end - s.start);
}

}
#endif
