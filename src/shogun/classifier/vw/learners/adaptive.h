#ifndef _VW_ADAPTIVE_H__
#define _VW_ADAPTIVE_H__

#include <shogun/classifier/vw/vw_learner.h>
#include <shogun/classifier/vw/vw_common.h>

namespace shogun
{
	class VwAdaptiveLearner: public VwLearner
	{

	public:
		/**
		 * Constructor, initializes regressor and environment
		 *
		 * @param regressor regressor to use
		 * @param vw_env environment to use
		 */
		VwAdaptiveLearner(VwRegressor* regressor, VwEnvironment* vw_env);

		void train(VwExample* &ex, float update);

	private:

		void quad_update(float* weights, VwFeature& page_feature,
				 v_array<VwFeature> &offer_features, size_t mask,
				 float update, float g, VwExample* ex, size_t& ctr);
	};
}

#endif // _VW_ADAPTIVE_H__
