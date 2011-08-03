/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */

#include <shogun/io/StreamingVwCacheFile.h>

using namespace shogun;

CStreamingVwCacheFile::CStreamingVwCacheFile(EVwCacheType cache_type)
	: CStreamingFile()
{
	init(cache_type);
}

CStreamingVwCacheFile::CStreamingVwCacheFile(char* fname, char rw, EVwCacheType cache_type)
	: CStreamingFile(fname, rw)
{
	init(cache_type);
}

CStreamingVwCacheFile::~CStreamingVwCacheFile()
{
	delete cache_reader;
}

void CStreamingVwCacheFile::get_vector(VwExample* &ex, int32_t& len)
{
	if (cache_reader->read_cached_example(ex))
		len = 1;
	else
		len = -1;
}

void CStreamingVwCacheFile::get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label)
{
	if (cache_reader->read_cached_example(ex))
		len = 1;
	else
		len = -1;
}

void CStreamingVwCacheFile::init(EVwCacheType cache_type)
{
	cache_format = cache_type;
	env = new VwEnvironment();
	if (cache_type == C_NATIVE)
		cache_reader = new NativeCacheReader(buf->working_file, env);
	else if (cache_type == C_PROTOBUF)
		cache_reader = new ProtobufCacheReader(buf->working_file, env);
}
