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
#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/cache_read.h>

namespace shogun
{
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
	CStreamingVwCacheFile();

	/**
	 * Constructor taking file name argument
	 *
	 * @param fname file name
	 * @param rw read/write mode
	 * @param name name
	 */
	CStreamingVwCacheFile(char* fname, char rw='r');

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

private:
	/**
	 * Initialize members
	 */
	virtual void init();

protected:
	/// Cache reader
	VwCacheReader* cache_reader;
};
}
#endif //__STREAMING_VWCACHEFILE_H__
