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

	env = new VwEnvironment();
}

void CStreamingVwFeatures::init(CStreamingFile* file,
				    bool is_labelled,
				    int32_t size)
{
	init();
	has_labels = is_labelled;
	working_file = file;
	parser.init(file, is_labelled, size);
	seekable=false;

	/* Get environment from the StreamingVwFile */
	env = ((CStreamingVwFile*) file)->get_env();
}

void CStreamingVwFeatures::setup_example(VwExample* ae)
{
	/* Shift the necessary stuff to the VwExample constructor */
	// ae->reset_members();

	/* TODO: DO THE NECESSARY FOR BELOW */
	/*
	if(env->sort_features && ae->sorted == false)
		unique_sort_features(ae);

	if(env->ngram > 1)
		generateGrams(env->ngram, env->skips, ae);
	*/

	ae->pass = env->passes_complete;
	ae->partial_prediction = 0;
	ae->num_features = 0;
	ae->total_sum_feat_sq = 1;
	ae->example_counter = ++example_count;
	ae->global_weight = (ae->ld).get_weight();
	//p->t += ae->global_weight; THIS SHOULD BE IN THE MAIN ALGORITHM
	//ae->example_t = p->t; AS SHOULD THIS

	/* If some namespaces should be ignored, remove them */
	if (env->ignore_some)
	{
		for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
			if (env->ignore[*i])
			{
				// Delete namespace
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

	/* Partition the features according to number of experts */
	// Should loop through the features to determine the boundaries
	size_t length = env->mask + 1;
	size_t expert_size = env->stride * (length >> env->partition_bits); //#features/expert
	for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
	{
		// Subsets is already erased just before parsing.
		VwFeature* f = ae->atomics[*i].begin;
		ae->subsets[*i].push(f);
		size_t current = expert_size;
		while (current <= length * env->stride)
		{
			VwFeature* ret = f;
			if (ae->atomics[*i].end > f)
				ret = search(f, current, ae->atomics[*i].end);
			ae->subsets[*i].push(ret);
			f = ret;
			current += expert_size;
		}
		ASSERT(f == ae->atomics[*i].end);
		ae->num_features += ae->atomics[*i].end - ae->atomics[*i].begin;
		ae->total_sum_feat_sq += ae->sum_feat_sq[*i];
	}

	if (env->rank == 0)
		for (std::vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end();i++)
		{
			ae->num_features
				+= (ae->atomics[(int)(*i)[0]].end - ae->atomics[(int)(*i)[0]].begin)
				*(ae->atomics[(int)(*i)[1]].end - ae->atomics[(int)(*i)[1]].begin);

			ae->total_sum_feat_sq += ae->sum_feat_sq[(int)(*i)[0]]*ae->sum_feat_sq[(int)(*i)[1]];

		}
	else
		for (std::vector<string>::iterator i = env->pairs.begin(); i != env->pairs.end();i++)
		{
			ae->num_features
				+= (ae->atomics[(int)(*i)[0]].end - ae->atomics[(int)(*i)[0]].begin)
				*env->rank;
			ae->num_features
				+= (ae->atomics[(int)(*i)[1]].end - ae->atomics[(int)(*i)[1]].begin)
				*env->rank;
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



int32_t CStreamingVwFeatures::get_num_features()
{
	return current_length;
}


EFeatureClass CStreamingVwFeatures::get_feature_class()
{
	return C_STREAMING_SIMPLE;
}
