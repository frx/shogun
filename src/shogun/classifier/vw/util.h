#ifndef _VW_UTIL_H__
#define _VW_UTIL_H__

#include <shogun/classifier/vw/vw_common.h>

namespace shogun
{

/**
 * Return the smallest position >= value, never referencing end.
 *
 * @param begin position to begin search from
 * @param value value to search for
 * @param end last address
 *
 * @return position of match
 */
VwFeature* search(VwFeature* begin, size_t value, VwFeature* end);

/** 
 * Get the inverse square root using a fast Newton's approximation
 * 
 * @param x number to operate on
 * 
 * @return approx inverse square root of number
 */
float InvSqrt(float x);

}
#endif
