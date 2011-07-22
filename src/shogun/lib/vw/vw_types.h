#ifndef _VW_PARSEHELPERS_H__
#define _VW_PARSEHELPERS_H__

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <shogun/lib/common.h>
#include <shogun/lib/v_array.h>
#include <shogun/io/IOBuffer.h>
#include <shogun/mathematics/Math.h>
#include <shogun/lib/vw/substring.h>
#include <shogun/lib/vw/parse_primitives.h>

namespace shogun
{

typedef size_t (*hash_func_t)(substring, unsigned long);

const int quadratic_constant = 27942141;
const int constant = 11650396;

/* VwEnvironment should be shared between VW objects as a replacement
 * for global data */
class VwEnvironment
{
public:
	VwEnvironment()
	{
		num_bits = 18;
		mask = (1 << num_bits) - 1;
	}

	~VwEnvironment()
	{
	}
	
	double get_min_label() { return min_label; }
	
	double get_max_label() { return max_label; }

	size_t get_num_bits() { return num_bits; }
	
	void set_num_bits(size_t bits)
	{
		num_bits = bits;
	}
	
	size_t get_mask() { return mask; }

	void set_mask(size_t m)
	{
		mask = m;
	}

public:
	
	double min_label;
	double max_label;

	size_t num_bits;
	size_t mask;
};

class VwLabel
{

public:
	/** 
	 * Parse a substring to get a label
	 * 
	 * @param words substrings, each representing a token in the label data of the format
	 */
	void parse_label(v_array<substring>& words)
	{
		switch(words.index())
		{
		case 0:
			break;
		case 1:
			label = float_of_substring(words[0]);
			break;
		case 2:
			label = float_of_substring(words[0]);
			weight = float_of_substring(words[1]);
			break;
		case 3:
			label = float_of_substring(words[0]);
			weight = float_of_substring(words[1]);
			initial = float_of_substring(words[2]);
			break;
		default:
			SG_SERROR("malformed example!\n"
				  "words.index() = %d\n", words.index());
		}
		/* CALL TO FUNCTION USING GLOBAL DATA! */
		/* SHIFTED TO PARSE CLASS */
		//set_minmax(label);
	}

	float get_weight()
	{
		return weight;
	}

	float get_initial()
	{
		return initial;
	}
	

public:
	/// Label value
	float64_t label;

	/// Weight of example
	float64_t weight;

	/// Initial approximation
	float64_t initial;

};

class VwFeature
{
public:
	float64_t x;
	uint32_t weight_index;
	bool operator==(VwFeature j) { return weight_index == j.weight_index; }
};

class VwExample
{
public:
	VwExample(VwEnvironment* env = NULL)
	{
		pass = 0;
		partial_prediction = 0.;
		num_features = 0;
		total_sum_feat_sq = 1;
		example_counter = 0;
		global_weight = 0;
		example_t = 0;

		size_t constant_namespace = 128;
		ae->indices.push(constant_namespace);
	}
	
	VwLabel ld;
	v_array<char> tag;
	size_t example_counter;

	v_array<size_t> indices;
	v_array<VwFeature> atomics[256];

	v_array<VwFeature*> subsets[256];

	size_t num_features;
	size_t pass;
	float64_t partial_prediction;
	float64_t final_prediction;
	float64_t global_prediction;
	float64_t loss;
	float64_t eta_round;
	float64_t eta_global;
	float64_t global_weight;
	float64_t example_t;
	float64_t sum_feat_sq[256];
	float64_t total_sum_feat_sq;
	float64_t revert_weight;
};

}
#endif
