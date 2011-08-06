#ifndef _VW_REGULARMACHINE_H__
#define _VW_REGULARMACHINE_H__

#include <shogun/classifier/vw/vw_machine.h>
#include <shogun/classifier/vw/vw_common.h>

namespace shogun
{
	class VwRegularMachine: public VwMachine
	{

	public:
		/** 
		 * Constructor, initializes regressor and environment
		 * 
		 * @param regressor regressor to use
		 * @param vw_env environment to use
		 */
		VwRegularMachine(VwRegressor* regressor, VwEnvironment* vw_env);
		
		void train(VwExample* &ex, float update);

		void perform_update(float* weights, VwFeature& page_feature,
				    v_array<VwFeature> &offer_features, size_t mask,
				    float update);
	};
}

#endif
