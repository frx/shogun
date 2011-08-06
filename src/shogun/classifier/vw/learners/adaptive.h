#ifndef _VW_ADAPTIVE_H__
#define _VW_ADAPTIVE_H__

#include <shogun/lib/vw/vw_machine.h>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{
	class VwAdaptiveMachine: public VwMachine
	{

	public:
		/** 
		 * Constructor, initializes regressor and environment
		 * 
		 * @param regressor regressor to use
		 * @param vw_env environment to use
		 */
		VwAdaptiveMachine(VwRegressor* regressor, VwEnvironment* vw_env);
		
		void train(VwExample* &ex, float update);

		void perform_update(float* weights, VwFeature& page_feature,
				    v_array<VwFeature> &offer_features, size_t mask,
				    float update, float g, VwExample* ex, size_t& ctr);
	};
}

#endif
