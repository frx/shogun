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

void CVowpalWabbit::set_learner()
{
	if (env->adaptive)
		learner = new VwAdaptiveMachine(reg, env);
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
