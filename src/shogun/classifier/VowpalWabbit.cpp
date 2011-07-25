#include <shogun/classifier/VowpalWabbit.h>

using namespace shogun;

void CVowpalWabbit::CVowpalWabbit(CStreamingVwFeatures* feat)
{
	init(feat);
}

void CVowpalWabbit::init(CStreamingVwFeatures* feat)
{
	features=feat;
	env=feat->get_env();
}

void CVowpalWabbit::one_pf_quad_adaptive_update(float* weight, feature& page_feature,
						v_array<VwFeature> 
void CVowpalWabbit::adaptive_inline_train(example* &ex, float update)
{
	// Hack
	size_t thread_num = 0;
	
	if (fabs(update) == 0.)
		return;

	size_t thread_mask = env->thread_mask;
	float* weights = reg->weight_vectors[thread_num];

	float g = reg->loss->getSquareGrad(ex->final_prediction, ex->ld->label) * ex->ld->weight;
	size_t ctr = 0;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		VwFeature* f = ex->subsets[*i][thread_num];
		for (; f != ex->subsets[*i][thread_num + 1]; f++)
		{
			float* w = &weights[f->weight_index & thread_mask];
			w[1] += g * f->x * f->x;
			float t = f->x * InvSqrt(w[1]);
			w[0] += update * t;
		}
	}

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		if (ex->subsets[(int)(*i)[0]].index() > 0)
		{
			v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
			temp.begin = ex->subsets[(int)(*i)[0]][thread_num];
			temp.end = ex->subsets[(int)(*i)[0]][thread_num+1];
			for (; temp.begin != temp.end; temp.begin++)
				one_pf_quad_adaptive_update(weights, *temp.begin, ex->atomics[(int)(*i)[1]], thread_mask, update, g, ex, ctr);
		}
	}
}

void CVowpalWabbit::train(CStreamingVwFeatures* feat)
{
	ASSERT(features);

	VwExample* example = NULL;
	int32_t current_pass = 0;

	features->start_parser();
	while (features->get_next_example())
	{
		example = features->get_example();

		if (example->pass != current_pass)
		{
			env->eta *= env->eta_decay_rate;
			current_pass = example->pass;
		}

		if (env->adaptive)
			adaptive_inline_train(example, example->eta_round);
		else
			inline_train(example, example->eta_round);

		release_example();
	}
	features->end_parser();
	
	if (env->l1_regularization > 0.)
	{
		uint32_t length = 1 << env->num_bits;
		size_t stride = env->stride;
		float gravity = env->l1_regularization * env->update_sum;
		for (uint32_t i = 0; i < length; i++)
			reg->weight_vectors[0][stride*i] = real_weight(reg->weight_vectors[0][stride*i].gravity);
	}
	
}
