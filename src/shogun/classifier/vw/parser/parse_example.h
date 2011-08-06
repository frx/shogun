#ifndef _VW_PARSEEXAMPLE_H__
#define _VW_PARSEEXAMPLE_H__

#include <shogun/io/SGIO.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/parser/vw_parser.h>

namespace shogun
{

	/**
	 * Get value of feature from a given substring.
	 * A default of 1 is assumed if no explicit value is specified.
	 *
	 * @param s substring, usually a feature:value string
	 * @param name returned array of substrings, split into name and value
	 * @param v value of feature, set by reference
	 */
	void feature_value(substring &s, v_array<substring>& name, float &v);

}
#endif
