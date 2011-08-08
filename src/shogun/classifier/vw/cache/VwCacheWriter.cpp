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

#include <shogun/classifier/vw/cache_write.h>

using namespace shogun;

CVwCacheWriter::CVwCacheWriter()
{
	fd = -1;
	env = NULL;
}

CVwCacheWriter::CVwCacheWriter(const char* fname, CVwEnvironment* env_to_use)
{
	fd = open(fname, O_CREAT | O_TRUNC | O_RDWR, 0666);

	if (fd < 0)
		SG_SERROR("Error opening the file %s for writing cache!\n");

	env = env_to_use;
	SG_REF(env);
}

CVwCacheWriter::CVwCacheWriter(int f, CVwEnvironment* env_to_use)
{
	fd = f;
	env = env_to_use;
	SG_REF(env);
}

CVwCacheWriter::~CVwCacheWriter()
{
	if (env)
		SG_UNREF(env);
}

void CVwCacheReader::set_file(int f)
{
	fd = f;
}

void CVwCacheWriter::set_env(CVwEnvironment* env_to_use)
{
	env = env_to_use;
	SG_REF(env);
}

CVwEnvironment* CVwCacheWriter::get_env()
{
	SG_REF(env);
	return env;
}
