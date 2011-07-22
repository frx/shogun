#ifndef _VW_SUBSTRING_H__
#define _VW_SUBSTRING_H__

#include <string.h>

namespace shogun
{
	
struct substring
{
	char *start;
	char *end;
};

char* c_string_of_substring(substring);

void print_substring(substring);

float double_of_substring(substring);

int int_of_substring(substring s);

unsigned long ulong_of_substring(substring);

unsigned long ss_length(substring s);

}
#endif
