#include <shogun/classifier/svm/VowpalWabbit.h>

using namespace shogun;

CVowpalWabbit::CVowpalWabbit(CStreamingVwFeatures* feat)
{
	init(feat);
}

void CVowpalWabbit::init(CStreamingVwFeatures* feat)
{
	features=feat;
	env=feat->get_env();
}

void CVowpalWabbit::set_learner()
{
	if (env->adaptive)
		learner = new VwAdaptiveMachine(reg, env);
}

float CVowpalWabbit::inline_l1_predict(VwExample* &ex)
{
	size_t thread_num = 0;
	
	float prediction = ex->ld.get_initial();

	float* weights = reg->weight_vectors[thread_num];
	size_t thread_mask = env->thread_mask;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		prediction += sd_truncadd(weights, thread_mask, ex->subsets[*i][thread_num], ex->subsets[*i][thread_num+1], env->l1_regularization * env->update_sum);
	}

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		if (ex->subsets[(int)(*i)[0]].index() > 0)
		{
			v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
			temp.begin = ex->subsets[(int)(*i)[0]][thread_num];
			temp.end = ex->subsets[(int)(*i)[0]][thread_num+1];
			for (; temp.begin != temp.end; temp.begin++)
				prediction += one_pf_quad_predict_trunc(weights, *temp.begin,
									ex->atomics[(int)(*i)[1]], thread_mask,
									env->l1_regularization * env->update_sum);
		}
	}

	return prediction;
}

float CVowpalWabbit::inline_predict(VwExample* &ex)
{
	size_t thread_num = 0;
	float prediction = ex->ld.get_initial();

	float* weights = reg->weight_vectors[thread_num];
	size_t thread_mask = env->thread_mask;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
		prediction += sd_add(weights, thread_mask,
				     ex->subsets[*i][thread_num],
				     ex->subsets[*i][thread_num+1]);

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		if (ex->subsets[(int)(*i)[0]].index() > 0)
		{
			v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
			temp.begin = ex->subsets[(int)(*i)[0]][thread_num];
			temp.end = ex->subsets[(int)(*i)[0]][thread_num+1];
			for (; temp.begin != temp.end; temp.begin++)
				prediction += one_pf_quad_predict(weights, *temp.begin,
								  ex->atomics[(int)(*i)[1]],
								  thread_mask);
		}
	}

	return prediction;
}
float CVowpalWabbit::predict(VwExample* ex)
{
	float prediction;
	if (env->l1_regularization != 0.)
		prediction = inline_l1_predict(ex);
	else
		prediction = inline_predict(ex);

	return prediction;
}
	
void CVowpalWabbit::train(CStreamingVwFeatures* feat)
{
	ASSERT(features);

	set_learner();
	
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

		learner->train(example, example->eta_round);
		predict(example);
		
		features->release_example();
	}
	features->end_parser();
	
	if (env->l1_regularization > 0.)
	{
		uint32_t length = 1 << env->num_bits;
		size_t stride = env->stride;
		float gravity = env->l1_regularization * env->update_sum;
		for (uint32_t i = 0; i < length; i++)
			reg->weight_vectors[0][stride*i] = real_weight(reg->weight_vectors[0][stride*i], gravity);
	}
	
}
