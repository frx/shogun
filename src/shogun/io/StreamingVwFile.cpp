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
#include <shogun/classifier/vw/parser/vw_parser.h>

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
	delete parser;
}

void CStreamingVwFile::set_parser_type(E_VW_PARSER_TYPE type)
{
	switch (type)
	{
	case T_VW:
		parse_example = &VwParser::read_features;
		parser_type = T_VW;
		break;
	case T_SVMLIGHT:
		parse_example = &VwParser::read_svmlight_features;
		parser_type = T_SVMLIGHT;
		break;
	case T_DENSE:
		parse_example = &VwParser::read_dense_features;
		parser_type = T_DENSE;
		break;
	default:
		SG_SERROR("Unrecognized parser type!\n");
	}
}

void CStreamingVwFile::get_vector(VwExample* &ex, int32_t &len)
{
	len = (parser->*parse_example)(buf, ex);
	if (len == 0)
		len = -1;	// indicates failure
}

void CStreamingVwFile::get_vector_and_label(VwExample* &ex, int32_t &len, float64_t &label)
{
	len = (parser->*parse_example)(buf, ex);
	if (len == 0)
		len = -1;	// indicates failure
}

void CStreamingVwFile::init()
{
	parser = new VwParser();
	env = parser->get_env();

	set_parser_type(T_VW);
	write_to_cache = true;
}
