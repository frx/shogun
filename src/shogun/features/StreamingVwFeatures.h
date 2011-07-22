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
		set_read_functions();
		init();
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
		if (seekable)
		{
			((CStreamingVwFile*) working_file)->reset_stream();
			parser.exit_parser();
			parser.init(working_file, has_labels, 1);
			parser.set_do_delete(false);
			parser.start_parser();
		}
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
	 * Return the current feature vector as an SGVector<T>.
	 * 
	 * @return The vector as SGVector<T>
	 */
	SGVector<VwExample> get_vector();

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
	 * Dot product using the current vector and another vector, passed as arg.
	 * 
	 * @param vec The vector with which to calculate the dot product.
	 * 
	 * @return Dot product as a float64_t
	 */
	virtual float64_t dot(SGVector<T> vec);

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
		ASSERT(vec2_len==current_length);
		float64_t result=0;
		
		for (int32_t i=0; i<current_length; i++)
			result+=current_vector[i]*vec2[i];
		
		return result;
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
		ASSERT(vec2_len==current_length);
		
		if (abs_val)
		{
			for (int32_t i=0; i<current_length; i++)
				vec2[i]+=alpha*CMath::abs(current_vector[i]);
		}
		else
		{
			for (int32_t i=0; i<current_length; i++)
				vec2[i]+=alpha*current_vector[i];
		}
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
	 * Duplicate the object.
	 * 
	 * @return a duplicate object as CFeatures*
	 */
	virtual CFeatures* duplicate() const
	{
		return new CStreamingVwFeatures<T>(*this);
	}

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
		if (current_vector)
			return 1;
		return 0;
	}

	/** 
	 * Return the size of one T object.
	 * 
	 * @return Size of T.
	 */
	virtual int32_t get_size() { return sizeof(T); }

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

	/// The current example's feature vector as a T*.
	T* current_vector;

	/// The current example's label.
	float64_t current_label;

	/// Number of features in current example.
	int32_t current_length;

	VwEnvironment* env;
};
	
void CStreamingVwFeatures<T>::set_vector_reader()
{
	parser.set_read_vector(&CStreamingFile::get_vector);
}

void CStreamingVwFeatures<T>::set_vector_and_label_reader()
{
	parser.set_read_vector_and_label(&CStreamingFile::get_vector_and_label);
}

#define GET_FEATURE_TYPE(f_type, sg_type)				\
template<> inline EFeatureType CStreamingVwFeatures<sg_type>::get_feature_type() \
{									\
	return f_type;							\
}

GET_FEATURE_TYPE(F_BOOL, bool)
GET_FEATURE_TYPE(F_CHAR, char)
GET_FEATURE_TYPE(F_BYTE, uint8_t)
GET_FEATURE_TYPE(F_BYTE, int8_t)
GET_FEATURE_TYPE(F_SHORT, int16_t)
GET_FEATURE_TYPE(F_WORD, uint16_t)
GET_FEATURE_TYPE(F_INT, int32_t)
GET_FEATURE_TYPE(F_UINT, uint32_t)
GET_FEATURE_TYPE(F_LONG, int64_t)
GET_FEATURE_TYPE(F_ULONG, uint64_t)
GET_FEATURE_TYPE(F_SHORTREAL, float32_t)
GET_FEATURE_TYPE(F_DREAL, float64_t)
GET_FEATURE_TYPE(F_LONGREAL, floatmax_t)
#undef GET_FEATURE_TYPE

	
template <class T>
void CStreamingVwFeatures<T>::init()
{
	working_file=NULL;
	current_vector=NULL;
	seekable=false;
	current_length=-1;
}

template <class T>
void CStreamingVwFeatures<T>::init(CStreamingFile* file,
				    bool is_labelled,
				    int32_t size)
{
	init();
	has_labels = is_labelled;
	working_file = file;
	parser.init(file, is_labelled, size);
	seekable=false;
}


void setup_example(VwExample* ae)
{
	/* Shift the necessary stuff to the VwExample constructor */
	ae->pass = global.passes_complete;

	ae->example_counter = ++example_count;
	ae->global_weight = (ae->ld)->get_weight();
	//p->t += ae->global_weight; THIS SHOULD BE IN THE MAIN ALGORITHM
	//ae->example_t = p->t; AS SHOULD THIS

	if (global.ignore_some)
	{
		for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
			if (global.ignore[*i])
			{//delete namespace
				ae->atomics[*i].erase();
				memmove(i,i+1,(ae->indices.end - (i+1))*sizeof(size_t));
				ae->indices.end--;
				i--;
			}
	}

	//add constant feature
	size_t constant_namespace = 128;
	push(ae->indices,constant_namespace);
	feature temp = {1,constant & global.mask};
	push(ae->atomics[constant_namespace], temp);

	if(global.stride != 1) //make room for per-feature information.
	{
		size_t stride = global.stride;
		for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
			for(feature* j = ae->atomics[*i].begin; j != ae->atomics[*i].end; j++)
				j->weight_index = j->weight_index*stride;
		if (global.audit)
			for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
				for(audit_data* j = ae->audit_features[*i].begin; j != ae->audit_features[*i].end; j++)
					j->weight_index = j->weight_index*stride;
	}

	//Should loop through the features to determine the boundaries
	size_t length = global.mask + 1;
	size_t expert_size = global.stride*(length >> global.partition_bits); //#features/expert
	for (size_t* i = ae->indices.begin; i != ae->indices.end; i++)
	{
		//subsets is already erased just before parsing.
		feature* f = ae->atomics[*i].begin;
		push(ae->subsets[*i],f);
		size_t current = expert_size;
		while (current <= length*global.stride)
		{
			feature* ret = f;
			if (ae->atomics[*i].end > f)
				ret = search(f, current, ae->atomics[*i].end);
			push(ae->subsets[*i],ret);
			f = ret;
			current += expert_size;
		}
		assert(f == ae->atomics[*i].end);
		ae->num_features += ae->atomics[*i].end - ae->atomics[*i].begin;
		ae->total_sum_feat_sq += ae->sum_feat_sq[*i];
	}


	if (global.rank == 0)
		for (vector<string>::iterator i = global.pairs.begin(); i != global.pairs.end();i++)
		{
			ae->num_features
				+= (ae->atomics[(int)(*i)[0]].end - ae->atomics[(int)(*i)[0]].begin)
				*(ae->atomics[(int)(*i)[1]].end - ae->atomics[(int)(*i)[1]].begin);

			ae->total_sum_feat_sq += ae->sum_feat_sq[(int)(*i)[0]]*ae->sum_feat_sq[(int)(*i)[1]];

		}
	else
		for (vector<string>::iterator i = global.pairs.begin(); i != global.pairs.end();i++)
		{
			ae->num_features
				+= (ae->atomics[(int)(*i)[0]].end - ae->atomics[(int)(*i)[0]].begin)
				*global.rank;
			ae->num_features
				+= (ae->atomics[(int)(*i)[1]].end - ae->atomics[(int)(*i)[1]].begin)
				*global.rank;
		}


}

template <class T>
void CStreamingVwFeatures<T>::start_parser()
{
	if (!parser.is_running())
		parser.start_parser();
}

template <class T>
void CStreamingVwFeatures<T>::end_parser()
{
	parser.end_parser();
}

template <class T>
bool CStreamingVwFeatures<T>::get_next_exampl(e)
{
	bool ret_value;
	ret_value = (bool) parser.get_next_example(current_vector,
						   current_length,
						   current_label);
	if (ret_value)
		setup_example(current_vector);
	
	return ret_value;
}

template <class T>
SGVector<T> CStreamingVwFeatures<T>::get_vector()
{
	current_sgvector.vector=current_vector;
	current_sgvector.vlen=current_length;

	return current_sgvector;
}

template <class T>
float64_t CStreamingVwFeatures<T>::get_label()
{
	ASSERT(has_labels);

	return current_label;
}
	
template <class T>
void CStreamingVwFeatures<T>::release_example()
{
	parser.finalize_example();
}

template <class T>
int32_t CStreamingVwFeatures<T>::get_dim_feature_space()
{
	return current_length;
}

template <class T>
	float64_t CStreamingVwFeatures<T>::dot(CStreamingDotFeatures* df)
{
	ASSERT(df);
	ASSERT(df->get_feature_type() == get_feature_type());
	ASSERT(df->get_feature_class() == get_feature_class());
	CStreamingVwFeatures<T>* sf = (CStreamingVwFeatures<T>*) df;

	SGVector<T> other_vector=sf->get_vector();

	float64_t result = CMath::dot(current_vector, other_vector.vector, current_length);

	return result;
}

template <class T>
float64_t CStreamingVwFeatures<T>::dot(SGVector<T> sgvec1)
{
	int32_t len1;
	len1=sgvec1.vlen;
				
	if (len1 != current_length)
		SG_ERROR("Lengths %d and %d not equal while computing dot product!\n", len1, current_length);

	float64_t result=CMath::dot(current_vector, sgvec1.vector, len1);
	return result;
}

template <class T>
int32_t CStreamingVwFeatures<T>::get_num_features()
{
	return current_length;
}

template <class T>
EFeatureClass CStreamingVwFeatures<T>::get_feature_class()
{
	return C_STREAMING_VW;
}

}
#endif // _STREAMING_VWFEATURES__H__
