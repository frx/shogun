#include <shogun/lib/vw/regular_machine.h>
#include <shogun/lib/vw/util.h>

using namespace shogun;

VwRegularMachine::VwRegularMachine(VwRegressor* regressor, VwEnvironment* vw_env)
	: VwMachine(regressor, vw_env)
{
}

void VwRegularMachine::train(VwExample* &ex, float update)
{
	if (fabs(update) == 0.)
		return;
	size_t thread_mask = env->thread_mask;
	// Hack
	size_t thread_num = 0;
	float* weights = reg->weight_vectors[thread_num];

	int j=0;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
		{
			j++;
			//printf("j=%d.\n", j++);
			weights[f->weight_index & thread_mask] += update * f->x;
		}
	}

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
		temp.begin = ex->atomics[(int)(*i)[0]].begin;
		temp.end = ex->atomics[(int)(*i)[0]].end;
		for (; temp.begin != temp.end; temp.begin++)
			perform_update(weights, *temp.begin, ex->atomics[(int)(*i)[1]], thread_mask, update);
	}
}

void VwRegularMachine::perform_update(float* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask, float update)
{
	size_t halfhash = quadratic_constant * page_feature.weight_index;
	update *= page_feature.x;
	for (VwFeature* elem = offer_features.begin; elem != offer_features.end; elem++)
		weights[(halfhash + elem->weight_index) & mask] += update * elem->x;
}
