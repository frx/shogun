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

float InvSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*)&i; // convert new bits into float
	x = x*(1.5f - xhalf*x*x); // One round of Newton's method
	return x;
}

}
