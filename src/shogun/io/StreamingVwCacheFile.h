/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */
#ifndef __STREAMING_VWCACHEFILE_H__
#define __STREAMING_VWCACHEFILE_H__

#include <shogun/io/StreamingFile.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/cache/VwCacheReader.h>
#include <shogun/classifier/vw/cache/VwProtobufCacheReader.h>
#include <shogun/classifier/vw/cache/VwNativeCacheReader.h>

namespace shogun
{
enum EVwCacheType
{
	C_NATIVE = 0,
	C_PROTOBUF = 1
};

/** @brief Class StreamingVwCacheFile to read vector-by-vector from VW
 * cache files.
 * It reads the example and label into one object of
 * VwExample type.
 */
class CStreamingVwCacheFile: public CStreamingFile
{
public:
	/**
	 * Default constructor
	 *
	 */
	CStreamingVwCacheFile(EVwCacheType cache_type = C_NATIVE);

	/**
	 * Constructor taking file name argument
	 *
	 * @param fname file name
	 * @param rw read/write mode
	 * @param name name
	 */
	CStreamingVwCacheFile(const char* fname, char rw='r', EVwCacheType cache_type = C_NATIVE);

	/**
	 * Destructor
	 */
	virtual ~CStreamingVwCacheFile();

	/**
	 * Returns the parsed example.
	 *
	 * The example contains the label if available, and
	 * also contains length of the feature vector.
	 * These parameters, passed separately are redundant.
	 *
	 * @param ex examples as VwExample*, set by reference
	 * @param len length of vector, untouched
	 */
	virtual void get_vector(VwExample* &ex, int32_t &len);

	/**
	 * Returns the parsed example.
	 *
	 * TODO: Make this fail if examples are found to be unlabelled.
	 *
	 * @param ex example as VwExample*, set by reference
	 * @param len length of vector, untouched
	 * @param label label, untouched
	 */
	virtual void get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label);

	/** @return object name */
	inline virtual const char* get_name() const
	{
		return "StreamingVwCacheFile";

	}

	/**
	 * Set environment for vw
	 *
	 * @param env CVwEnvironment* environment
	 */
	void set_env(CVwEnvironment* env_to_use)
	{
		if (env)
			SG_UNREF(env);
		delete cache_reader;
		env = env_to_use;
		SG_REF(env);

		if (cache_format == C_NATIVE)
			cache_reader = new CVwNativeCacheReader(buf->working_file, env);
		else if (cache_format == C_PROTOBUF)
			cache_reader = new CVwProtobufCacheReader(buf->working_file, env);
	}

	/**
	 * Return the environment
	 *
	 * @return environment as CVwEnvironment*
	 */
	CVwEnvironment* get_env()
	{
		return env;
	}

	/**
	 * Whether this stream is seekable
	 *
	 * @return true, since caches can be reset
	 */
	bool is_seekable() { return true; }

	/**
	 * Reset cache file back to first example.
	 *
	 * Used when multiple passes are to be performed
	 */
	void reset_stream();

private:
	/**
	 * Initialize members
	 */
	virtual void init(EVwCacheType cache_type);

protected:
	/// Cache reader
	CVwCacheReader* cache_reader;

	/// Environment used for vw
	CVwEnvironment* env;

	/// Cache type
	EVwCacheType cache_format;
};
}
#endif //__STREAMING_VWCACHEFILE_H__
