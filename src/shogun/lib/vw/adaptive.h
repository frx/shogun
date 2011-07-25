#ifndef _VW_ADAPTIVE_H__
#define _VW_ADAPTIVE_H__

#include <shogun/lib/vw/vw_machine.h>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{
	class VwAdaptiveMachine: public VwMachine
	{

	public:
		void train(VwExample* &ex, float update);

		void perform_update(float* weights, VwFeature& page_feature,
				    v_array<VwFeature> &offer_features, size_t mask,
				    float update, float g, VwExample* ex, size_t& ctr);
	};
}

#endif
