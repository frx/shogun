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
 * Adaptation of Vowpal Wabbit v5.1.
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society.
 */

#ifndef _VW_PARSER_H__
#define _VW_PARSER_H__

#include <shogun/io/SGIO.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/hash.h>
#include <shogun/classifier/vw/parser/parse_primitives.h>
#include <shogun/classifier/vw/parser/parse_example.h>
#include <shogun/classifier/vw/cache/VwProtobufCacheWriter.h>
#include <shogun/classifier/vw/cache/VwNativeCacheWriter.h>

namespace shogun
{

class VwParser
{
public:
	/**
	 * Default constructor
	 */
	VwParser();

	/**
	 * Constructor taking environment as parameter.
	 *
	 * @param env_to_use CVwEnvironment to use
	 */
	VwParser(CVwEnvironment* env_to_use);

	/**
	 * Destructor
	 */
	~VwParser();

	/**
	 * Get the environment
	 *
	 * @return environment as CVwEnvironment*
	 */
	CVwEnvironment* get_env()
	{
		SG_REF(env);
		return env;
	}

	/**
	 * Set the environment
	 *
	 * @param env_to_use environment as CVwEnvironment*
	 */
	void set_env(CVwEnvironment* env_to_use)
	{
		env = env_to_use;
		SG_REF(env);
	}

	/**
	 * Reads input from the buffer and parses it into a VwExample
	 *
	 * @param buf IOBuffer which contains input
	 * @param ex parsed example
	 *
	 * @return number of characters read for this example
	 */
	int32_t read_features(CIOBuffer* buf, VwExample*& ex);

	/**
	 * Read an example from an SVMLight file
	 *
	 * @param buf IOBuffer which contains input
	 * @param ae parsed example
	 *
	 * @return number of characters read for this example
	 */
	int32_t read_svmlight_features(CIOBuffer* buf, VwExample*& ae);

	/**
	 * Read an example from a file with dense vectors
	 *
	 * @param buf IOBuffer which contains input
	 * @param ae parsed example
	 *
	 * @return number of characters read for this example
	 */
	int32_t read_dense_features(CIOBuffer* buf, VwExample*& ae);

	/**
	 * Set whether to write cache file or not
	 *
	 * @param wr_cache write cache or not
	 */
	void set_write_cache(bool wr_cache)
	{
		write_cache = wr_cache;
	}

	/**
	 * Return whether cache will be written or not
	 *
	 * @return will cache be written?
	 */
	bool get_write_cache()
	{
		return write_cache;
	}

	/**
	 * Update min and max labels seen in the environment
	 *
	 * @param label current label based on which to update
	 */
	void set_mm(float64_t label)
	{
		env->min_label = CMath::min(env->min_label, label);
		if (label != FLT_MAX)
			env->max_label = CMath::max(env->max_label, label);
	}

	/**
	 * A dummy function performing no operation in case training
	 * is not to be performed.
	 *
	 * @param label label
	 */
	void noop_mm(float64_t label)
	{
	}

	/**
	 * Function which is actually called to update min and max labels
	 * Should be set to one of the functions implemented for this.
	 *
	 * @param label label based on which to update
	 */
	void set_minmax(float64_t label)
	{
		set_mm(label);
	}

	/**
	 * Return the name of the object
	 *
	 * @return VwParser
	 */
	virtual const char* get_name() const { return "VwParser"; }

public:
	/// Hash function to use, of type hash_func_t
	hash_func_t hasher;

protected:
	/// Environment of VW - used by parser
	CVwEnvironment* env;

	CVwCacheWriter* cache_writer;

	bool write_cache;

private:
	/// Used during parsing
	v_array<substring> channels;
	v_array<substring> words;
	v_array<substring> name;
};
}
#endif
