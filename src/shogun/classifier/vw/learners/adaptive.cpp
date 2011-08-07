#include <shogun/classifier/vw/learners/adaptive.h>
#include <shogun/classifier/vw/util.h>

using namespace shogun;

VwAdaptiveLearner::VwAdaptiveLearner(VwRegressor* regressor, VwEnvironment* vw_env)
	: VwLearner(regressor, vw_env)
{
}

void VwAdaptiveLearner::train(VwExample* &ex, float update)
{
	if (fabs(update) == 0.)
		return;

	// Hack
	size_t thread_num = 0;

	size_t thread_mask = env->thread_mask;
	float* weights = reg->weight_vectors[thread_num];

	float g = reg->loss->get_square_grad(ex->final_prediction, ex->ld->label) * ex->ld->weight;
	size_t ctr = 0;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		for (VwFeature *f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
		{
			float* w = &weights[f->weight_index & thread_mask];
			w[1] += g * f->x * f->x;
			float t = f->x*InvSqrt(w[1]);
			w[0] += update * t;
		}
	}
	for (int32_t k = 0; k < env->pairs.get_num_elements(); k++)
	{
		char* i = env->pairs.get_element(k);

		v_array<VwFeature> temp = ex->atomics[(int)(i[0])];
		temp.begin = ex->atomics[(int)(i[0])].begin;
		temp.end = ex->atomics[(int)(i[0])].end;
		for (; temp.begin != temp.end; temp.begin++)
			quad_update(weights, *temp.begin, ex->atomics[(int)(i[1])], thread_mask, update, g, ex, ctr);
	}
}

void VwAdaptiveLearner::quad_update(float* weights, VwFeature& page_feature,
				    v_array<VwFeature> &offer_features, size_t mask,
				    float update, float g, VwExample* ex, size_t& ctr)
{
	size_t halfhash = quadratic_constant * page_feature.weight_index;
	update *= page_feature.x;
	float update2 = g * page_feature.x * page_feature.x;

	for (VwFeature* elem = offer_features.begin; elem != offer_features.end; elem++)
	{
		float* w = &weights[(halfhash + elem->weight_index) & mask];
		w[1] += update2 * elem->x * elem->x;
		float t = elem->x * InvSqrt(w[1]);
		w[0] += update * t;
	}
}
