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

#ifndef _VW_PROTOBUF_WRITE_H__
#define _VW_PROTOBUF_WRITE_H__

#include <shogun/classifier/vw/cache/VwCacheWriter.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "proto/example.pb.h"

using namespace google::protobuf::io;

namespace shogun
{

/** @brief CVwProtobufCacheWriter implements a cache writing
 * class which uses the Protocol Buffers serialization
 * library with the specification detailed in a .pb
 * file.
 *
 * The cache generated should be read by the CVwProtobufCacheReader
 * class.
 */
class CVwProtobufCacheWriter: public CVwCacheWriter
{
public:
	/**
	 * Default constructor
	 */
	CVwProtobufCacheWriter();

	/**
	 * Constructor taking file name and environment
	 * as arguments.
	 *
	 * @param fname file name
	 * @param env_to_use environment
	 */
	CVwProtobufCacheWriter(const char* fname, CVwEnvironment* env_to_use);

	/**
	 * Destructor
	 */
	virtual ~CVwProtobufCacheWriter();

	/**
	 * Set file descriptor to use
	 *
	 * @param f file descriptor
	 */
	virtual void set_file(int f);

	/**
	 * Write one example into the cache
	 *
	 * @param ex example
	 */
	virtual void cache_example(VwExample* &ex);

	/**
	 * Return the name of the object.
	 *
	 * @return VwProtobufCacheWriter
	 */
	virtual const char* get_name() const { return "VwProtobufCacheWriter"; }

private:
	/**
	 * Initialize members
	 *
	 * @param f file descriptor
	 */
	virtual void init(int f);

protected:
	/// FileOutputStream from the opened file
	FileOutputStream* file_stream;

	/// Coded stream which wraps the FileOutputStream
	CodedOutputStream* coded_stream;
};

}

#endif // _VW_PROTOBUF_WRITE_H__
