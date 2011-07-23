#include <shogun/lib/vw/util.h>

namespace shogun
{
	
VwFeature* search(VwFeature* begin, size_t value, VwFeature* end)
{
	// Return the smallest position >= value, never referencing end.
	size_t diff = end-begin;
	if (diff <= 1)
		if (begin->weight_index >= value)
			return begin;
		else
			return end;
	else
	{
		VwFeature* middle = begin + (diff >> 1);
		if (middle->weight_index >= value)
			return search(begin, value, middle);
		else
			return search(middle, value, end);
	}
}
	
}
