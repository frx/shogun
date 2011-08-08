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

#include <shogun/classifier/vw/cache/VwProtobufCacheReader.h>

using namespace shogun;

CVwProtobufCacheReader::CVwProtobufCacheReader()
	: CVwCacheReader()
{
	file_stream = NULL;
	coded_stream = NULL;
}

CVwProtobufCacheReader::CVwProtobufCacheReader(const char* fname, CVwEnvironment* env_to_use)
	: CVwCacheReader(fname, env_to_use)
{
	init(fd);
}

CVwProtobufCacheReader::CVwProtobufCacheReader(int f, CVwEnvironment* env_to_use)
	: CVwCacheReader(f, env_to_use)
{
	init(fd);
}

CVwProtobufCacheReader::~CVwProtobufCacheReader()
{
	delete coded_stream;
	delete file_stream;
	close(fd);
}

void CVwProtobufCacheReader::init(int f)
{
	file_stream = new FileInputStream(f);
	coded_stream = new CodedInputStream(file_stream);
	coded_stream->SetTotalBytesLimit(INT_MAX, -1);
}

void CVwProtobufCacheReader::set_file(int f)
{
	if (coded_stream)
		delete coded_stream;
	if (file_stream)
		delete file_stream;
	if (fd > 0)
		close(fd);

	fd = f;
	init(fd);
}

bool CVwProtobufCacheReader::read_cached_example(VwExample* const ex)
{
	vwcache::Example ex_cached;

	google::protobuf::uint32 read_size;
	if (!coded_stream->ReadVarint32(&read_size))
	{
		SG_SINFO("Failed to read object size from cached input!\n");
		return NULL;
	}

	CodedInputStream::Limit lim = coded_stream->PushLimit(read_size);
	if (!ex_cached.ParseFromCodedStream(coded_stream))
	{
		SG_SINFO("Failed to parse from cached input!\n");
		return NULL;
	}

	coded_stream->PopLimit(lim);

	// Read label
	ex->ld->label = ex_cached.ld().label();
	ex->ld->weight = ex_cached.ld().weight();
	ex->ld->initial = ex_cached.ld().initial();

	// Read tag
	ex->tag.push_many(ex_cached.mutable_tag()->c_str(), ex_cached.mutable_tag()->length());

	// Read namespaces
	for (int i = 0; i < ex_cached.channels_size(); i++)
	{
		google::protobuf::uint32 index = ex_cached.channels(i).index();
		ex->indices.push(index);
		v_array<VwFeature>* ours = ex->atomics+index;
		float64_t* our_sum_feat_sq = ex->sum_feat_sq+index;

		// Read feature object
		for (int j = 0; j < ex_cached.channels(i).x_size(); j++)
		{
			VwFeature f = {ex_cached.channels(i).x(j), ex_cached.channels(i).diff_index(j)};
			*our_sum_feat_sq += f.x*f.x;
			ours->push(f);
		}
	}

	return true;
}
