/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */
#ifndef _STREAMING_VWFEATURES__H__
#define _STREAMING_VWFEATURES__H__

#include <shogun/lib/common.h>
#include <shogun/mathematics/Math.h>
#include <shogun/features/StreamingDotFeatures.h>
#include <shogun/lib/DataType.h>
#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/util.h>
#include <shogun/io/StreamingVwFile.h>
#include <shogun/io/InputParser.h>

namespace shogun
{
/** @brief This class implements streaming features for use with VW.
 */
class CStreamingVwFeatures : public CStreamingDotFeatures
{
public:

	/**
	 * Default constructor.
	 *
	 * Sets the reading functions to be
	 * CStreamingFile::get_*_vector and get_*_vector_and_label
	 * depending on the type T.
	 */
	CStreamingVwFeatures()
		: CStreamingDotFeatures()
	{
		init();
		set_read_functions();
	}

	/**
	 * Constructor taking args.
	 * Initializes the parser with the given args.
	 *
	 * @param file StreamingFile object, input file.
	 * @param is_labelled Whether examples are labelled or not.
	 * @param size Number of example objects to be stored in the parser at a time.
	 */
	CStreamingVwFeatures(CStreamingFile* file,
			     bool is_labelled,
			     int32_t size)
		: CStreamingDotFeatures()
	{
		init(file, is_labelled, size);
		set_read_functions();
	}

	/**
	 * Destructor.
	 *
	 * Ends the parsing thread. (Waits for pthread_join to complete)
	 */
	~CStreamingVwFeatures()
	{
		parser.end_parser();
	}

	CStreamingVwFeatures* duplicate() const
	{
		SG_NOTIMPLEMENTED;
	}

	/**
	 * Sets the read function (in case the examples are
	 * unlabelled) to get_*_vector() from CStreamingFile.
	 *
	 * The exact function depends on type T.
	 *
	 * The parser uses the function set by this while reading
	 * unlabelled examples.
	 */
	virtual void set_vector_reader();

	/**
	 * Sets the read function (in case the examples are labelled)
	 * to get_*_vector_and_label from CStreamingFile.
	 *
	 * The exact function depends on type T.
	 *
	 * The parser uses the function set by this while reading
	 * labelled examples.
	 */
	virtual void set_vector_and_label_reader();

	/**
	 * Starts the parsing thread.
	 *
	 * To be called before trying to use any feature vectors from this object.
	 */
	virtual void start_parser();

	/**
	 * Ends the parsing thread.
	 *
	 * Waits for the thread to join.
	 */
	virtual void end_parser();

	virtual void reset_stream()
	{
		SG_NOTIMPLEMENTED;
	}

	/**
	 * Instructs the parser to return the next example.
	 *
	 * This example is stored as the current_example in this object.
	 *
	 * @return True on success, false if there are no more
	 * examples, or an error occurred.
	 */
	virtual bool get_next_example();

	/** 
	 * Returns the current example.
	 * 
	 * @return current example as VwExample*
	 */
	virtual VwExample* get_example();

	/**
	 * Return the label of the current example as a float.
	 *
	 * Examples must be labelled, otherwise an error occurs.
	 *
	 * @return The label as a float64_t.
	 */
	virtual float64_t get_label();

	/**
	 * Release the current example, indicating to the parser that
	 * it has been processed by the learning algorithm.
	 *
	 * The parser is then free to throw away that example.
	 */
	virtual void release_example();

	/** obtain the dimensionality of the feature space
	 *
	 * (not mix this up with the dimensionality of the input space, usually
	 * obtained via get_num_features())
	 *
	 * @return dimensionality
	 */
	virtual int32_t get_dim_feature_space();

	/**
	 * Dot product taken with another StreamingDotFeatures object.
	 *
	 * Currently only works if it is a CStreamingVwFeatures object.
	 * It takes the dot product of the current_vectors of both objects.
	 *
	 * @param df CStreamingDotFeatures object.
	 *
	 * @return Dot product.
	 */
	virtual float64_t dot(CStreamingDotFeatures *df);

	/**
	 * Dot product with another dense vector.
	 *
	 * @param vec2 The dense vector with which to take the dot product.
	 * @param vec2_len length of vector
	 * @return Dot product as a float64_t.
	 */
	virtual float64_t dense_dot(const float64_t* vec2, int32_t vec2_len)
	{
		SG_NOTIMPLEMENTED;
	}

	/**
	 * Add alpha*current_vector to another dense vector.
	 * Takes the absolute value of current_vector if specified.
	 *
	 * @param alpha alpha
	 * @param vec2 vector to add to
	 * @param vec2_len length of vector
	 * @param abs_val true if abs of current_vector should be taken
	 */
	virtual void add_to_dense_vec(float64_t alpha, float64_t* vec2, int32_t vec2_len , bool abs_val=false)
	{
		SG_NOTIMPLEMENTED;
	}

	/** get number of non-zero features in vector
	 *
	 * @return number of non-zero features in vector
	 */
	virtual inline int32_t get_nnz_features_for_vector()
	{
		return current_length;
	}

	/**
	 * Return the number of features in the current example.
	 *
	 * @return number of features as int
	 */
	int32_t get_num_features();

	/**
	 * Return the feature type, depending on T.
	 *
	 * @return Feature type as EFeatureType
	 */
	virtual inline EFeatureType get_feature_type();

	/**
	 * Return the feature class
	 *
	 * @return C_STREAMING_VW
	 */
	virtual EFeatureClass get_feature_class();

	/**
	 * Return the name.
	 *
	 * @return StreamingVwFeatures
	 */
	inline virtual const char* get_name() const { return "StreamingVwFeatures"; }

	/**
	 * Return the number of vectors stored in this object.
	 *
	 * @return 1 if current_vector exists, else 0.
	 */
	inline virtual int32_t get_num_vectors() const
	{
		SG_NOTIMPLEMENTED;
	}

	/**
	 * Return the size of one T object.
	 *
	 * @return Size of T.
	 */
	virtual int32_t get_size() { return sizeof(VwExample); }

private:
	/**
	 * Initializes members to null values.
	 * current_length is set to -1.
	 */
	void init();

	/**
	 * Calls init, and also initializes the parser with the given args.
	 *
	 * @param file StreamingFile to read from
	 * @param is_labelled whether labelled or not
	 * @param size number of examples in the parser's ring
	 */
	void init(CStreamingFile *file, bool is_labelled, int32_t size);

	void setup_example(VwExample* ae);

protected:

	/// The parser object, which reads from input and returns parsed example objects.
	CInputParser<VwExample> parser;

	size_t example_count;

	/// The current example's label.
	float64_t current_label;

	/// Number of features in current example.
	int32_t current_length;

	VwEnvironment* env;

	VwExample* current_example;
};

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

}
#endif // _STREAMING_VWFEATURES__H__
