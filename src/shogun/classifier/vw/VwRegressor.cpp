/*
 * Copyright (c) 2009 Yahoo! Inc.  All rights reserved.  The copyrights
 * embodied in the content of this file are licensed under the BSD
 * (revised) open source license.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society.
 */

#include <shogun/classifier/vw/VwRegressor.h>
#include <shogun/loss/SquaredLoss.h>

using namespace shogun;

CVwRegressor::CVwRegressor()
	: CSGObject()
{
	weight_vectors = NULL;
	loss = new CSquaredLoss();
	init(NULL);
}

CVwRegressor::CVwRegressor(CVwEnvironment* env)
	: CSGObject()
{
	weight_vectors = NULL;
	loss = new CSquaredLoss();
	init(env);
}

CVwRegressor::~CVwRegressor()
{
	SG_FREE(weight_vectors);
	SG_UNREF(loss);
}

void CVwRegressor::init(CVwEnvironment* env)
{
	// For each feature, there should be 'stride' number of
	// elements in the weight vector
	index_t length = ((index_t) 1) << env->num_bits;
	env->thread_mask = (env->stride * (length >> env->thread_bits)) - 1;

	// Only one learning thread for now
	index_t num_threads = 1;
	weight_vectors = SG_MALLOC(float32_t*, num_threads);

	for (index_t i = 0; i < num_threads; i++)
	{
		weight_vectors[i] = SG_CALLOC(float32_t, env->stride * length / num_threads);

		if (env->random_weights)
		{
			for (index_t j = 0; j < length/num_threads; j++)
				weight_vectors[i][j] = drand48() - 0.5;
		}

		if (env->initial_weight != 0.)
			for (index_t j = 0; j < env->stride*length/num_threads; j+=env->stride)
				weight_vectors[i][j] = env->initial_weight;

		if (env->adaptive)
			for (index_t j = 1; j < env->stride*length/num_threads; j+=env->stride)
				weight_vectors[i][j] = 1;
	}
}

void CVwRegressor::dump_regressor(char* reg_name, bool as_text)
{
	CIOBuffer io_temp;
	int32_t f = io_temp.open_file(reg_name,'w');

	if (f < 0)
		SG_SERROR("Can't open: %s for writing! Exiting.\n", reg_name);

	char* version = env->version;
	size_t v_length = env->v_length;

	if (!as_text)
	{
		// Write version info
		io_temp.write_file((char*)&v_length, sizeof(v_length));
		io_temp.write_file(version,v_length);

		// Write max and min labels
		io_temp.write_file((char*)&env->min_label, sizeof(env->min_label));
		io_temp.write_file((char*)&env->max_label, sizeof(env->max_label));

		// Write weight vector bits information
		io_temp.write_file((char *)&env->num_bits, sizeof(env->num_bits));
		io_temp.write_file((char *)&env->thread_bits, sizeof(env->thread_bits));

		// For paired namespaces forming quadratic features
		int32_t len = env->pairs.get_num_elements();
		io_temp.write_file((char *)&len, sizeof(len));

		for (int32_t k = 0; k < env->pairs.get_num_elements(); k++)
			io_temp.write_file(env->pairs.get_element(k), 2);

		// ngram and skips information
		io_temp.write_file((char*)&env->ngram, sizeof(env->ngram));
		io_temp.write_file((char*)&env->skips, sizeof(env->skips));
	}
	else
	{
		// Write as human readable form
		char buff[512];
		int32_t len;

		len = sprintf(buff, "Version %s\n", version);
		io_temp.write_file(buff, len);
		len = sprintf(buff, "Min label:%f max label:%f\n", env->min_label, env->max_label);
		io_temp.write_file(buff, len);
		len = sprintf(buff, "bits:%d thread_bits:%d\n", (int32_t)env->num_bits, (int32_t)env->thread_bits);
		io_temp.write_file(buff, len);
		
		if (env->pairs.get_num_elements() > 0)
		{
			len = sprintf(buff, "\n");
			io_temp.write_file(buff, len);
		}
		
		len = sprintf(buff, "ngram:%d skips:%d\nindex:weight pairs:\n", (int32_t)env->ngram, (int32_t)env->skips);
		io_temp.write_file(buff, len);
	}

	uint32_t length = 1 << env->num_bits;
	size_t num_threads = env->num_threads();
	size_t stride = env->stride;

	// Write individual weights
	for(uint32_t i = 0; i < length; i++)
	{
		float32_t v;
		v = weight_vectors[i%num_threads][stride*(i/num_threads)];
		if (v != 0.)
		{
			if (!as_text)
			{
				io_temp.write_file((char *)&i, sizeof (i));
				io_temp.write_file((char *)&v, sizeof (v));
			}
			else
			{
				char buff[512];
				int len = sprintf(buff, "%d:%f\n", i, v);
				io_temp.write_file(buff, len);
			}
		}
	}

	io_temp.close_file();
}

void CVwRegressor::read_vector(const char* file, regressor& r, bool& initialized, bool reg_vector)
{
	ifstream source(file);
	if (!source.is_open())
	{
		cout << "can't open " << file << endl << " ... exiting." << endl;
		exit(1);
	}
  
	size_t v_length;
	source.read((char*)&v_length, sizeof(v_length));
	char t[v_length];
	source.read(t,v_length);
	if (strcmp(t,version.c_str()) != 0)
	{
		cout << "source has possibly incompatible version!" << endl;
		exit(1);
	}
  
	source.read((char*)&global.min_label, sizeof(global.min_label));
	source.read((char*)&global.max_label, sizeof(global.max_label));
  
	size_t local_num_bits;
	source.read((char *)&local_num_bits, sizeof(local_num_bits));
	if (!initialized){
		if (global.default_bits != true && global.num_bits != local_num_bits)
		{
			cout << "Wrong number of bits for source!" << endl;
			exit (1);
		}
		global.default_bits = false;
		global.num_bits = local_num_bits;
	}
	else 
		if (local_num_bits != global.num_bits)
		{
			cout << "can't combine sources with different feature number!" << endl;
			exit (1);
		}
  
	size_t local_thread_bits;
	source.read((char*)&local_thread_bits, sizeof(local_thread_bits));
	if (!initialized){
		global.thread_bits = local_thread_bits;
		global.partition_bits = global.thread_bits;
	}
	else 
		if (local_thread_bits != global.thread_bits)
		{
			cout << "can't combine sources trained with different numbers of threads!" << endl;
			exit (1);
		}
  
	int len;
	source.read((char *)&len, sizeof(len));
  
	vector<string> local_pairs;
	for (; len > 0; len--)
	{
		char pair[2];
		source.read(pair, sizeof(char)*2);
		string temp(pair, 2);
		local_pairs.push_back(temp);
	}
	if (!initialized)
	{
		global.pairs = local_pairs;
		initialize_regressor(r);
	}
	else
		if (local_pairs != global.pairs)
		{
			cout << "can't combine sources with different features!" << endl;
			for (size_t i = 0; i < local_pairs.size(); i++)
				cout << local_pairs[i] << " " << local_pairs[i].size() << " ";
			cout << endl;
			for (size_t i = 0; i < global.pairs.size(); i++)
				cout << global.pairs[i] << " " << global.pairs[i].size() << " ";
			cout << endl;
			exit (1);
		}
	size_t local_ngram;
	source.read((char*)&local_ngram, sizeof(local_ngram));
	size_t local_skips;
	source.read((char*)&local_skips, sizeof(local_skips));
	if (!initialized)
	{
		global.ngram = local_ngram;
		global.skips = local_skips;
		initialized = true;
	}
	else
		if (global.ngram != local_ngram || global.skips != local_skips)
		{
			cout << "can't combine sources with different ngram features!" << endl;
			exit(1);
		}
	size_t stride = global.stride;
	while (source.good())
	{
		uint32_t hash;
		source.read((char *)&hash, sizeof(hash));
		weight w = 0.;
		source.read((char *)&w, sizeof(float));
      
		size_t num_threads = global.num_threads();
		if (source.good())
		{
			if (global.rank != 0)
				r.weight_vectors[hash % num_threads][hash/num_threads] = w;
			else if (global.lda == 0)
				if (reg_vector)
					r.regularizers[hash % num_threads][hash/num_threads] = w;
				else
					r.weight_vectors[hash % num_threads][(hash*stride)/num_threads] 
						= r.weight_vectors[hash % num_threads][(hash*stride)/num_threads] + w;
			else
				r.weight_vectors[hash % num_threads][hash/num_threads] 
					= r.weight_vectors[hash % num_threads][hash/num_threads] + w;
		}      
	}
	source.close();
}
