#ifndef _VW_MACHINE_H__
#define _VW_MACHINE_H__

#include <shogun/lib/vw/vw_types.h>

namespace shogun
{
	
class VwMachine
{
public:
	VwMachine()
	{
		reg = NULL;
	}

	virtual ~VwMachine()
	{
	}

	virtual void train(VwExample* &ex, float update) = 0;
	
	virtual void perform_update(float* weights, VwFeature& page_feature,
				    v_array<VwFeature> &offer_features, size_t mask,
				    float update, float g, VwExample* ex, size_t& ctr) = 0;

protected:

	VwRegressor *reg;

	VwEnvironment *env;
};

}
#endif
