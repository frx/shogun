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

CStreamingVwCacheFile::CStreamingVwCacheFile(const char* fname, char rw, EVwCacheType cache_type)
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

void CStreamingVwCacheFile::reset_stream()
{
	buf->reset_file();

	if (cache_format == C_NATIVE)
		((CVwNativeCacheReader*) cache_reader)->check_cache_metadata();
}

void CStreamingVwCacheFile::init(EVwCacheType cache_type)
{
	cache_format = cache_type;
	env = new CVwEnvironment();
	if (cache_type == C_NATIVE)
		cache_reader = new CVwNativeCacheReader(buf->working_file, env);
	else if (cache_type == C_PROTOBUF)
		cache_reader = new CVwProtobufCacheReader(buf->working_file, env);
}
