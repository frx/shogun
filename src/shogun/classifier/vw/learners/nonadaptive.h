#ifndef _VW_NONADAPTIVE_H__
#define _VW_NONADAPTIVE_H__

#include <shogun/classifier/vw/vw_learner.h>
#include <shogun/classifier/vw/vw_common.h>

namespace shogun
{
	class VwNonAdaptiveLearner: public VwLearner
	{

	public:
		/**
		 * Constructor, initializes regressor and environment
		 *
		 * @param regressor regressor to use
		 * @param vw_env environment to use
		 */
		VwNonAdaptiveLearner(CVwRegressor* regressor, VwEnvironment* vw_env);

		void train(VwExample* &ex, float update);

	private:
		void quad_update(float* weights, VwFeature& page_feature,
				 v_array<VwFeature> &offer_features, size_t mask,
				 float update);
	};
}

#endif // _VW_NONADAPTIVE_H__
