/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */

#include <shogun/io/StreamingVwFile.h>
#include <shogun/lib/vw/vw_parser.h>

using namespace shogun;

CStreamingVwFile::CStreamingVwFile()
	: CStreamingFile()
{
	init();
}

CStreamingVwFile::CStreamingVwFile(char* fname, char rw)
	: CStreamingFile(fname, rw)
{
	init();
}

CStreamingVwFile::~CStreamingVwFile()
{
	delete p;
}

void CStreamingVwFile::set_parser_type(E_VW_PARSER_TYPE type)
{
	p->set_parser_type(type);
}

void CStreamingVwFile::get_vector(VwExample* &ex, int32_t &len)
{
	len = p->read_features(buf, ex);
	if (len == 0)
		len = -1;	// indicates failure
}

void CStreamingVwFile::get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label)
{
	len = (p->*parse_example)(buf, ex);
	if (len == 0)
		len = -1;	// indicates failure
}

void CStreamingVwFile::init()
{
	p = new VwParser();
	env = p->get_env();

	write_to_cache = true;
}
