#ifndef _VW_TYPES_H__
#define _VW_TYPES_H__

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <shogun/lib/common.h>
#include <shogun/lib/v_array.h>
#include <shogun/io/IOBuffer.h>
#include <shogun/mathematics/Math.h>
#include <shogun/lib/vw/substring.h>
#include <shogun/lib/vw/parse_primitives.h>
#include <shogun/losses/LossFunction.h>
#include <shogun/losses/SquaredLoss.h>

namespace shogun
{
	using std::string;
	using std::vector;

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
			partition_bits = 0;
			mask = (1 << num_bits) - 1;
			stride = 1;

			min_label = 0.;
			max_label = 1.;

			// Inferred from parse_args
			eta = 10.;
			eta_decay_rate = 1.;

			adaptive = false;
			random_weights = false;
			initial_weight = 0.;

			l1_regularization = 0.;
			update_sum = 0.;

			power_t = 0.5;
			t = 1.;
			initial_t = 1.;

			example_number = 0;
			weighted_examples = 0.;
			weighted_unlabeled_examples = 0.;
			weighted_labels = 0.;
			total_features = 0;
			sum_loss = 0.;
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

		size_t num_threads()
		{
			return 1 << thread_bits;
		}

		size_t num_partitions()
		{
			return 1 << partition_bits;
		}

		size_t length()
		{
			return 1 << num_bits;
		}

	public:
		double min_label;
		double max_label;

		/// log_2 of the number of features
		size_t num_bits;
		/// log_2 of the number of partitions of features
		size_t partition_bits;
		///
		size_t thread_bits;
		/// Mask used for hashing
		size_t mask;
		size_t thread_mask;

		/// Whether some namespaces are ignored
		bool ignore_some;
		/// Which namespaces to ignore
		bool ignore[256];
		/// Pairs of features to cross
		std::vector<string> pairs;

		size_t stride;

		size_t passes_complete;

		bool sort_features;

		size_t rank;

		float eta;
		float eta_decay_rate;

		float l1_regularization;
		float update_sum;

		bool adaptive;

		bool random_weights;
		float initial_weight;

		float power_t;
		float t;

		double initial_t;
		long long int example_number;
		double weighted_examples;
		double weighted_unlabeled_examples;
		double weighted_labels;
		size_t total_features;
		double sum_loss;

	};

	class VwLabel
	{

	public:
		/**
		 * Parse a substring to get a label
		 *
		 * @param words substrings, each representing a token in the label data of the format
		 */
		VwLabel()
		{
			label = FLT_MAX;
			weight = 1.;
			initial = 0.;
		}

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
		/**
		 * Constructor, taking environment as optional argument
		 *
		 * @param env env as VwEnvironment*, optional
		 */
		VwExample(VwEnvironment* env = NULL)
		{
			pass = 0;
			partial_prediction = 0.;
			num_features = 0;
			total_sum_feat_sq = 1;
			example_counter = 0;
			global_weight = 0;
			example_t = 0;
		}

		/**
		 * Resets the members so the values can be updated
		 * directly.
		 */
		void reset_members()
		{
			partial_prediction = 0.;
			num_features = 0;
			total_sum_feat_sq = 1;
			example_counter = 0;
			global_weight = 0;
			example_t = 0;

			/* THIS PART IS FROM parse_atomic_example */
			/* Erase features, subsets */
			for (size_t* i = indices.begin; i != indices.end; i++)
			{
				atomics[*i].erase();
				subsets[*i].erase();
				sum_feat_sq[*i]=0;
			}

			indices.erase();
			tag.erase();
			sorted = false;
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

		size_t ngram;
		size_t skips;

		bool sorted;
	};

	class VwRegressor
	{
	public:
		VwRegressor(VwEnvironment* env = NULL)
		{
			weight_vectors = NULL;
			loss = new CSquaredLoss();
			init(env);
		}

		~VwRegressor()
		{
			if (weight_vectors)
				delete[] weight_vectors;
		}

		void init(VwEnvironment* env = NULL)
		{
			size_t length = ((size_t) 1) << env->num_bits;
			env->thread_mask = (env->stride * (length >> env->thread_bits)) - 1;

			size_t num_threads = 1;
			weight_vectors = new float*[num_threads];

			for (size_t i=0; i<num_threads; i++)
			{
				/* Initialize vectors to zero */
				weight_vectors[i] = new float[env->stride * length / num_threads]();

				if (env->random_weights)
				{
					if (env->rank > 0)
						for (size_t j = 0; j < env->stride*length/num_threads; j++)
							weight_vectors[i][j] = (double) 0.1 * rand() / ((double) RAND_MAX + 1.0);
					else
						for (size_t j = 0; j < length/num_threads; j++)
							weight_vectors[i][j] = drand48() - 0.5;
				}

				if (env->initial_weight != 0.)
					for (size_t j = 0; j< env->stride*length/num_threads; j+=env->stride)
						weight_vectors[i][j] = env->initial_weight;

				if (env->adaptive)
					for (size_t j = 1; j< env->stride*length/num_threads; j+=env->stride)
						weight_vectors[i][j] = 1;
			}
		}


	public:
		float** weight_vectors;

		CLossFunction* loss;
	};

}
#endif
