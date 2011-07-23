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
};
}
#endif //__STREAMING_VWFILE_H__
