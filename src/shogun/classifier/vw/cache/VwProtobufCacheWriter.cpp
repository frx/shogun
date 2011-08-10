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

#include <shogun/classifier/vw/cache/VwProtobufCacheWriter.h>

using namespace shogun;

CVwProtobufCacheWriter::CVwProtobufCacheWriter()
	: CVwCacheWriter()
{
	file_stream = NULL;
	coded_stream = NULL;
}

CVwProtobufCacheWriter::CVwProtobufCacheWriter(char * fname, CVwEnvironment* env_to_use)
	: CVwCacheWriter(fname, env_to_use)
{
	init(fd);
}

CVwProtobufCacheWriter::~CVwProtobufCacheWriter()
{
	delete coded_stream;
	delete file_stream;
	close(fd);
}

void CVwProtobufCacheWriter::init(int32_t f)
{
	file_stream = new FileOutputStream(f);
	coded_stream = new CodedOutputStream(file_stream);
}

void CVwProtobufCacheWriter::set_file(int32_t f)
{
	if (coded_stream)
		delete coded_stream;
	if (file_stream)
		delete file_stream;
	if (fd > 0)
		close(fd);

	init(f);
}

void CVwProtobufCacheWriter::cache_example(VwExample*& ex)
{
	vwcache::Example ex_cached;

	// Store the label
	vwcache::LabelData* ld = ex_cached.mutable_ld();
	ld->set_label(ex->ld->label);
	ld->set_weight(ex->ld->weight);
	ld->set_initial(ex->ld->initial);

	// Store the tag

	if (ex->tag.index() > 0)
	{
		char* tag = new char[ex->tag.index() + 1];
		memcpy(tag, ex->tag.begin, ex->tag.index());
		tag[ex->tag.index()]='\0';
		*(ex_cached.mutable_tag()) = tag;
	}

	// Loop over namespaces
	for (size_t* i = ex->indices.begin; i != ex->indices.end; i++)
	{
		vwcache::Channel* channel = ex_cached.add_channels();
		channel->set_namespace_index(*i);
		// Loop over features
		for (VwFeature* f = ex->atomics[*i].begin; f != ex->atomics[*i].end; f++)
		{
			channel->add_diff_index(f->weight_index);
			channel->add_x(f->x);
		}
	}

	google::protobuf::uint32 cached_size = ex_cached.ByteSize();
	coded_stream->WriteVarint32(cached_size);

	if (!ex_cached.SerializeToCodedStream(coded_stream))
		SG_SERROR("Error serializing to coded output stream!\n");
}
