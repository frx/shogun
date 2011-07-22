#include <shogun/lib/vw/parse_primitives.h>

namespace shogun
{
	
void tokenize(char delim, substring s, v_array<substring>& ret)
{
	ret.erase();
	char *last = s.start;
	for (; s.start != s.end; s.start++)
	{
		if (*s.start == delim)
		{
			if (s.start != last)
			{
				substring temp = {last,s.start};
				ret.push(temp);
			}
			last = s.start+1;
		}
	}
	if (s.start != last)
	{
		substring final = {last, s.start};
		ret.push(final);
	}
}

}
