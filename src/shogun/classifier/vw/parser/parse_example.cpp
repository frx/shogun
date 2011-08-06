#include <shogun/classifier/vw/substring.h>
#include <shogun/classifier/vw/parser/parse_primitives.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/parser/parse_example.h>
#include <shogun/classifier/vw/parser/vw_parser.h>

namespace shogun
{

	void feature_value(substring &s, v_array<substring>& name, float &v)
	{
		// Get the value of the feature in the substring
		tokenize(':', s, name);

		switch (name.index())
		{
		// If feature value is not specified, assume 1.0
		case 0:
		case 1:
			v = 1.;
			break;
		case 2:
			v = float_of_substring(name[1]);
			if (isnan(v))
				SG_SERROR("error NaN value for feature %s! Terminating!\n",
					  c_string_of_substring(name[0]));
			break;
		default:
			SG_SERROR("Examples with a weird name, i.e., '%s'\n",
				  c_string_of_substring(s));
		}
	}

}
