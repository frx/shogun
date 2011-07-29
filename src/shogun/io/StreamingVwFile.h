/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */
#ifndef __STREAMING_VWFILE_H__
#define __STREAMING_VWFILE_H__

#include <shogun/io/StreamingFile.h>
#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/vw_parser.h>
#include <shogun/lib/vw/protobuf_read.h>

namespace shogun
{
/** @brief Class StreamingVwFile to read vector-by-vector from VW files.
*/

class CStreamingVwFile: public CStreamingFile
{
public:
	/**
	 * Default constructor
	 *
	 */
	CStreamingVwFile() : CStreamingFile()
	{
		init();
	}

	/**
	 * Constructor taking file name argument
	 *
	 * @param fname file name
	 * @param rw read/write mode
	 * @param name name
	 */
	CStreamingVwFile(char* fname, char rw='r'): CStreamingFile(fname, rw)
	{
		init();
	}

	/**
	 * Destructor
	 */
	virtual ~CStreamingVwFile()
	{
		delete p;
		if (read_from_cache)
			delete cache_reader;
	}

	/**
	 * Returns the parsed example.
	 *
	 * The example contains the label if available, and
	 * also contains length of the feature vector.
	 * These parameters are redundant.
	 *
	 * @param ex examples as VwExample*, set by reference
	 * @param len length of vector, untouched
	 */
	void get_vector(VwExample* &ex, int32_t &len)
	{
		if (read_from_cache)
		{
			ex  = cache_reader->read_cached_example();
			if (ex == NULL)
				len = -1;
			else
				len = 1;
		}

		len = p->read_features(buf, ex);
		if (len == 0)
			len = -1;
	}

	/**
	 * Returns the parsed example.
	 *
	 * TODO: Make this fail if examples are found to be unlabelled.
	 *
	 * @param ex example as VwExample*, set by reference
	 * @param len length of vector, untouched
	 * @param label label, untouched
	 */
	void get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label)
	{
		label=1;
		len = p->read_features(buf, ex);
		if (len == 0)
			len = -1;

	}

	/**
	 * Set environment for vw
	 *
	 * @param env VwEnvironment* environment
	 */
	void set_env(VwEnvironment* env_to_use)
	{
		p->set_env(env_to_use);
	}

	/**
	 * Return the environment
	 *
	 * @return environment as VwEnvironment*
	 */
	VwEnvironment* get_env()
	{
		return env;
	}

	/**
	 * Whether the file is a cache file or not
	 *
	 * @param read_cache is file a cache file
	 */
	void set_read_from_cache(bool read_cache)
	{
		read_from_cache = read_cache;
		if (read_from_cache)
			cache_reader = new ProtobufCacheReader(buf->working_file);
	}

	/**
	 * Get whether file is a cache
	 *
	 * @return whether reading from a cache
	 */
	bool get_read_from_cache()
	{
		return read_from_cache;
	}

	/** @return object name */
	inline virtual const char* get_name() const
	{
		return "StreamingVwFile";

	}

private:
	/**
	 * Initialize members
	 */
	void init()
	{
		p = new VwParser();
		env = p->get_env();
	}

protected:
	/// Parser for vw format
	VwParser* p;

	/// Environment used for vw - used by parser
	VwEnvironment* env;

	/// Read the file as a cache file
	bool read_from_cache;

	/// Cache reader
	ProtobufCacheReader* cache_reader;
};
}
#endif //__STREAMING_VWFILE_H__
