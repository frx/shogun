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
		switch(words.index()) {
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
	
	VwLabel ld;
	v_array<char> tag;

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
