#ifndef _VW_PARSER_H__
#define _VW_PARSER_H__

#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/hash.h>
#include <shogun/lib/vw/parse_primitives.h>
#include <shogun/lib/vw/parse_example.h>

namespace shogun
{

class VwParser
{
public:
	VwParser(VwEnvironment = NULL)
	{
		
	}
	
	hash_func_t hasher;
	
	int32_t read_features(CIOBuffer* buf, VwExample*& ex);

	void set_mm(float64_t label)
	{
		global.min_label = CMath::min(global.min_label, label);
		if (label != FLT_MAX)
			global.max_label = CMath::max(global.max_label, label);
	}
	
	void noop_mm(float64_t label) { }

	void set_minmax(float64_t label)
	{
		set_mm(label);
	}

public:
	VwEnvironment* env;
};

}
#endif
