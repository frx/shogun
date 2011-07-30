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
#include <shogun/lib/vw/protobuf_read.h>

using namespace shogun;

CStreamingVwCacheFile::CStreamingVwCacheFile()
	: CStreamingFile()
{
	init();
}

CStreamingVwCacheFile::CStreamingVwCacheFile(char* fname, char rw)
	: CStreamingFile(fname, rw)
{
	init();
}

CStreamingVwCacheFile::~CStreamingVwCacheFile()
{
	delete cache_reader;
}

void CStreamingVwCacheFile::get_vector(VwExample* &ex, int32_t &len)
{
	ex = cache_reader->read_cached_example();
	if (ex == NULL)
		len = -1;
	else
		len = 1;
}

void CStreamingVwCacheFile::get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label)
{
	ex = cache_reader->read_cached_example();
	if (ex == NULL)
		len = -1;
	else
		len = 1;
}

void CStreamingVwCacheFile::init()
{
	cache_reader = new ProtobufCacheReader(buf->working_file);
}
