#include <shogun/classifier/svm/VowpalWabbit.h>

using namespace shogun;

CVowpalWabbit::CVowpalWabbit(CStreamingVwFeatures* feat)
{
	reg=NULL;
	learner=NULL;
	init(feat);
}

CVowpalWabbit::~CVowpalWabbit()
{
	if (reg)
		delete reg;
	if (learner)
		delete learner;
}

void CVowpalWabbit::init(CStreamingVwFeatures* feat)
{
	env=feat->get_env();
	features=feat;
	reg=new VwRegressor(env);
}

void CVowpalWabbit::set_learner()
{
	if (env->adaptive)
		learner = new VwAdaptiveMachine(reg, env);
	else
		learner = new VwRegularMachine(reg, env);
}

float CVowpalWabbit::inline_l1_predict(VwExample* &ex)
{
	size_t thread_num = 0;

	float prediction = ex->ld->get_initial();

	float* weights = reg->weight_vectors[thread_num];
	size_t thread_mask = env->thread_mask;

	prediction += features->dense_dot_truncated(weights, ex, env->l1_regularization * env->update_sum);

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
		temp.begin = ex->atomics[(int)(*i)[0]].begin;
		temp.end = ex->atomics[(int)(*i)[0]].end;
		for (; temp.begin != temp.end; temp.begin++)
			prediction += one_pf_quad_predict_trunc(weights, *temp.begin,
								ex->atomics[(int)(*i)[1]], thread_mask,
								env->l1_regularization * env->update_sum);
	}

	return prediction;
}

float CVowpalWabbit::inline_predict(VwExample* &ex)
{
	size_t thread_num = 0;
	float prediction = ex->ld->initial;

	float* weights = reg->weight_vectors[thread_num];
	size_t thread_mask = env->thread_mask;
	prediction += features->dense_dot(weights, 0);

	for (vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end(); i++)
	{
		v_array<VwFeature> temp = ex->atomics[(int)(*i)[0]];
		temp.begin = ex->atomics[(int)(*i)[0]].begin;
		temp.end = ex->atomics[(int)(*i)[0]].end;
		for (; temp.begin != temp.end; temp.begin++)
			prediction += one_pf_quad_predict(weights, *temp.begin,
							  ex->atomics[(int)(*i)[1]],
							  thread_mask);
	}

	return prediction;
}

float CVowpalWabbit::finalize_prediction(float ret)
{
	if (isnan(ret))
		return 0.5;
	if (ret > env->max_label)
		return env->max_label;
	if (ret < env->min_label)
		return env->min_label;

	return ret;
}

float CVowpalWabbit::predict(VwExample* ex)
{
	float prediction;
	if (env->l1_regularization != 0.)
		prediction = inline_l1_predict(ex);
	else
		prediction = inline_predict(ex);

	ex->final_prediction = 0;
	ex->final_prediction += prediction;
	ex->final_prediction = finalize_prediction(ex->final_prediction);
	float t = ex->example_t;

	if (ex->ld->label != FLT_MAX)
	{
		ex->loss = reg->get_loss(ex->final_prediction, ex->ld->label) * ex->ld->weight;
		double update = 0.;
		update = (env->eta)/pow(t, env->power_t) * ex->ld->weight;
		ex->eta_round = reg->get_update(ex->final_prediction, ex->ld->label, update, ex->total_sum_feat_sq);
		env->update_sum += update;
	}

	return prediction;
}

void CVowpalWabbit::train(CStreamingVwFeatures* feat)
{
	ASSERT(features);

	set_learner();

	VwExample* example = NULL;
	int32_t current_pass = 0;
	float dump_interval = exp(1.);

	const char * header_fmt = "%-10s %-10s %8s %8s %10s %8s %8s\n";
	fprintf(stdout, header_fmt,
		"average", "since", "example", "example",
		"current", "current", "current");
	fprintf(stdout, header_fmt,
		"loss", "last", "counter", "weight", "label", "predict", "features");

	int cnt = 0;
	features->start_parser();
	while (features->get_next_example())
	{
		example = features->get_example();

		if (example->pass != current_pass)
		{
			env->eta *= env->eta_decay_rate;
			current_pass = example->pass;
		}

		cnt++;
		example->eta_round = 0.;
		example->final_prediction = 0.;
		
		float out = predict(example);
		//printf("eta round is: %f.\n", example->eta_round);
		learner->train(example, example->eta_round);
		//printf ("\nExample %d: Prediction = %f. eta_round = %f.\n", cnt, example->final_prediction, example->eta_round);
		example->eta_round = 0.;

		if (env->weighted_examples + example->ld->weight > dump_interval)
		{
			print_update(example);
			dump_interval *= 2;
		}

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

void CVowpalWabbit::set_adaptive(bool adaptive_learning)
{
	if (adaptive_learning)
	{
		env->adaptive = true;
		env->stride = 2;
		env->power_t = 0.;
	}
	else
		env->adaptive = false;
}

void CVowpalWabbit::print_update(VwExample* &ex)
{

	printf("%-10.6f %-10.6f %8lld %8.1f %8.4f %8.4f %8lu\n",
	       env->sum_loss/env->weighted_examples,
	       0.0,
	       env->example_number,
	       env->weighted_examples,
	       ex->ld->label,
	       ex->final_prediction,
	       (long unsigned int)ex->num_features);
}
