#include <shogun/features/StreamingVwFeatures.h>

using namespace shogun;

void CStreamingVwFeatures::set_vector_reader()
{
	parser.set_read_vector(&CStreamingFile::get_vector);
}

void CStreamingVwFeatures::set_vector_and_label_reader()
{
	parser.set_read_vector_and_label(&CStreamingFile::get_vector_and_label);
}

inline EFeatureType CStreamingVwFeatures::get_feature_type()
{
	return F_DREAL;
}

void CStreamingVwFeatures::init()
{
	working_file=NULL;
	seekable=false;
	current_length=-1;

	example_count = 0;
}

void CStreamingVwFeatures::init(CStreamingFile* file, bool is_labelled, int32_t size)
{
	init();
	has_labels = is_labelled;
	working_file = file;
	parser.init(file, is_labelled, size);
	parser.set_do_delete(false);
	seekable=false;

	/* Get environment from the StreamingVwFile */
	env = ((CStreamingVwFile*) file)->get_env();
}

void CStreamingVwFeatures::setup_example(VwExample* ae)
{
	ae->pass = env->passes_complete;
	ae->num_features = 0;
	ae->total_sum_feat_sq = 1;
	ae->example_counter = ++example_count;
	ae->global_weight = ae->ld->weight;
	env->t += ae->global_weight;
	ae->example_t = env->t;

	/* If some namespaces should be ignored, remove them */
	if (env->ignore_some)
	{
		for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
			if (env->ignore[*i])
			{
				ae->atomics[*i].erase();
				memmove(i,i+1,(ae->indices.end - (i+1))*sizeof(size_t));
				ae->indices.end--;
				i--;
			}
	}

	/* Constant feature is already added by the constructor */
	size_t constant_namespace = 128;
	VwFeature temp = {1,constant & env->mask};
	ae->indices.push(constant_namespace);
	ae->atomics[constant_namespace].push(temp);

	if(env->stride != 1)
	{
		// Make room for per-feature information.
		size_t stride = env->stride;
		for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
			for(VwFeature* j = ae->atomics[*i].begin; j != ae->atomics[*i].end; j++)
				j->weight_index = j->weight_index*stride;
	}

	for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
	{
		ae->num_features += ae->atomics[*i].end - ae->atomics[*i].begin;
		ae->total_sum_feat_sq += ae->sum_feat_sq[*i];
	}

	for (std::vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end();i++)
	{
		ae->num_features
			+= (ae->atomics[(int)(*i)[0]].end - ae->atomics[(int)(*i)[0]].begin)
			*(ae->atomics[(int)(*i)[1]].end - ae->atomics[(int)(*i)[1]].begin);

		ae->total_sum_feat_sq += ae->sum_feat_sq[(int)(*i)[0]]*ae->sum_feat_sq[(int)(*i)[1]];

	}
}

void CStreamingVwFeatures::start_parser()
{
	if (!parser.is_running())
		parser.start_parser();
}

void CStreamingVwFeatures::end_parser()
{
	parser.end_parser();
}

bool CStreamingVwFeatures::get_next_example()
{
	bool ret_value;
	ret_value = (bool) parser.get_next_example(current_example,
						   current_length,
						   current_label);
	if (current_length < 1)
		return false;

	if (ret_value)
		setup_example(current_example);
	else
		return false;

	current_label = current_example->ld->label;
	current_length = current_example->num_features;

	return ret_value;
}

VwExample* CStreamingVwFeatures::get_example()
{
	return current_example;
}

float64_t CStreamingVwFeatures::get_label()
{
	ASSERT(has_labels);

	return current_label;
}

void CStreamingVwFeatures::release_example()
{
	env->example_number++;
	env->weighted_examples += current_example->ld->weight;

	if (current_example->ld->label == FLT_MAX)
		env->weighted_labels += 0;
	else
		env->weighted_labels += current_example->ld->label * current_example->ld->weight;

	env->total_features += current_example->num_features;
	env->sum_loss += current_example->loss;

	current_example->reset_members();
	parser.finalize_example();
}

int32_t CStreamingVwFeatures::get_dim_feature_space()
{
	return current_length;
}

float64_t CStreamingVwFeatures::dot(CStreamingDotFeatures* df)
{
	SG_NOTIMPLEMENTED;
}

float CStreamingVwFeatures::dense_dot(VwExample* &ex, const float* vec2)
{
	float64_t ret = 0.;
	for (size_t* i = ex->indices.begin; i!= ex->indices.end; i++)
	{
		for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
			ret += vec2[f->weight_index & env->thread_mask] * f->x;
	}
	return ret;
}

float CStreamingVwFeatures::dense_dot(const float* vec2, int32_t vec2_len)
{
	return dense_dot(current_example, vec2);
}

float CStreamingVwFeatures::dense_dot(SGSparseVector<float64_t>* vec1, const float* vec2)
{
	float64_t ret = 0.;
	for (int32_t i = 0; i < vec1->num_feat_entries; i++)
		ret += vec1->features[i].entry * vec2[(constant + vec1->features[i].feat_index) & env->mask];
	
	return ret;
}

float CStreamingVwFeatures::dense_dot_truncated(const float* vec2, VwExample* &ex, float gravity)
{
	float ret = 0.;
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		for (VwFeature* f = ex->atomics[*i].begin; f!= ex->atomics[*i].end; f++)
		{
			float w = vec2[f->weight_index & env->thread_mask];
			float wprime = real_weight(w,gravity);
			ret += wprime*f->x;
		}
	}
	return ret;
}

void CStreamingVwFeatures::add_to_dense_vec(float alpha, VwExample* &ex, float* vec2, int32_t vec2_len, bool abs_val)
{
	if (abs_val)
	{
		for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
		{
			for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
				vec2[f->weight_index & env->thread_mask] += alpha * abs(f->x);
		}
	}
	else
	{
		for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
		{
			for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
				vec2[f->weight_index & env->thread_mask] += alpha * f->x;
		}
	}
}

void CStreamingVwFeatures::add_to_dense_vec(float alpha, float* vec2, int32_t vec2_len, bool abs_val)
{
	add_to_dense_vec(alpha, current_example, vec2, vec2_len, abs_val);
}

int32_t CStreamingVwFeatures::get_num_features()
{
	return current_length;
}


EFeatureClass CStreamingVwFeatures::get_feature_class()
{
	return C_STREAMING_SIMPLE;
}
