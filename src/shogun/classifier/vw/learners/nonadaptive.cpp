#include <shogun/classifier/vw/learners/nonadaptive.h>
#include <shogun/classifier/vw/util.h>

using namespace shogun;

VwNonAdaptiveLearner::VwNonAdaptiveLearner(CVwRegressor* regressor, VwEnvironment* vw_env)
	: VwLearner(regressor, vw_env)
{
}

void VwNonAdaptiveLearner::train(VwExample* &ex, float update)
{
	if (fabs(update) == 0.)
		return;
	size_t thread_mask = env->thread_mask;
	// Hack
	size_t thread_num = 0;
	float* weights = reg->weight_vectors[thread_num];

	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
			weights[f->weight_index & thread_mask] += update * f->x;
	}
	for (int32_t k = 0; k < env->pairs.get_num_elements(); k++)
	{
		char* i = env->pairs.get_element(k);

		v_array<VwFeature> temp = ex->atomics[(int)(i[0])];
		temp.begin = ex->atomics[(int)(i[0])].begin;
		temp.end = ex->atomics[(int)(i[0])].end;
		for (; temp.begin != temp.end; temp.begin++)
			quad_update(weights, *temp.begin, ex->atomics[(int)(i[1])], thread_mask, update);
	}
}

void VwNonAdaptiveLearner::quad_update(float* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask, float update)
{
	size_t halfhash = quadratic_constant * page_feature.weight_index;
	update *= page_feature.x;
	for (VwFeature* elem = offer_features.begin; elem != offer_features.end; elem++)
		weights[(halfhash + elem->weight_index) & mask] += update * elem->x;
}
