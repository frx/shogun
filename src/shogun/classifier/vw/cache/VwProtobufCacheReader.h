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

#ifndef _VW_PROTOBUF_READ_H__
#define _VW_PROTOBUF_READ_H__

#include <shogun/classifier/vw/cache/VwCacheReader.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "proto/example.pb.pb.h"

using namespace google::protobuf::io;

namespace shogun
{

/** @brief CVwProtobufCacheReader is a class which reads examples from
 * a cache file specified by a .proto file and serialized using
 * Protocol Buffers.
 *
 * The cache file should have been created using the CVwProtobufCacheWriter
 * class.
 */
class CVwProtobufCacheReader: public CVwCacheReader
{
public:
	/**
	 * Default constructor
	 */
	CVwProtobufCacheReader();

	/**
	 * Constructor taking file name and environment
	 * as arguments.
	 *
	 * @param fname file name
	 * @param env_to_use environment
	 */
	CVwProtobufCacheReader(const char* fname, CVwEnvironment* env_to_use);

	/**
	 * Constructor taking file descriptor and
	 * environment as arguments.
	 *
	 * @param f file descriptor of cache
	 * @param env_to_use environment
	 */
	CVwProtobufCacheReader(int f, CVwEnvironment* env_to_use);

	/**
	 * Destructor
	 */
	virtual ~CVwProtobufCacheReader();

	/**
	 * Set the file descriptor to use
	 *
	 * @param f descriptor of cache file
	 */
	virtual void set_file(int f);

	/**
	 * Read one example from the cache
	 *
	 * @param ae example object whose members
	 * are set by reference
	 *
	 * @return success or failure as bool
	 */
	virtual bool read_cached_example(VwExample* const ae);

	/**
	 * Return the name of the object.
	 *
	 * @return VwProtobufCacheReader
	 */
	virtual const char* get_name() const { return "VwProtobufCacheReader"; }

private:
	/**
	 * Initialize members
	 *
	 * @param f file descriptor
	 */
	virtual void init(int f);

protected:
	/// FileInputStream from the opened file
	FileInputStream* file_stream;

	/// Coded stream which wraps the FileInputStream
	CodedInputStream* coded_stream;
};

}
#endif // _VW_PROTOBUF_READ_H__
