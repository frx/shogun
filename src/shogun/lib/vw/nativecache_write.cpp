#include <shogun/lib/vw/nativecache_write.h>

using namespace shogun;

NativeCacheWriter::NativeCacheWriter(const char* fname):
	VwCacheWriter(fname)
{
	init();
	buf.use_file(fd);
}

NativeCacheWriter::~NativeCacheWriter()
{
	buf.close_file();
}

void NativeCacheWriter::init()
{
	neg_1 = 1;
	general = 2;
	int_size = 6;
}

char* NativeCacheWriter::run_len_encode(char *p, size_t i)
{
	while (i >= 128)
	{
		*(p++) = (i & 127) | 128;
		i = i >> 7;
	}
	*(p++) = (i & 127);

	return p;
}

void NativeCacheWriter::cache_tag(v_array<char> tag)
{
	// Store the size of the tag and the tag itself
	char *c;
	
	buf.buf_write(c, sizeof(size_t)+tag.index());
	*(size_t*)c = tag.index();
	c += sizeof(size_t);
	memcpy(c, tag.begin, tag.index());
	c += tag.index();
	
	buf.set(c);
}

void NativeCacheWriter::output_byte(unsigned char s)
{
	char *c;

	buf.buf_write(c, 1);
	*(c++) = s;
	buf.set(c);
}

void NativeCacheWriter::output_features(unsigned char index, VwFeature* begin, VwFeature* end)
{
	size_t int_size = 6;
	
	char* c;
	size_t storage = (end-begin) * int_size;
	for (VwFeature* i = begin; i != end; i++)
		if (i->x != 1. && i->x != -1.)
			storage+=sizeof(float);
  
	buf.buf_write(c, sizeof(index) + storage + sizeof(size_t));
	*(unsigned char*)c = index;
	c += sizeof(index);

	char *storage_size_loc = c;
	c += sizeof(size_t);
  
	size_t last = 0;

	// Store the differences in hashed feature indices
	for (VwFeature* i = begin; i != end; i++)
	{
		int32_t s_diff = (i->weight_index - last);
		size_t diff = ZigZagEncode(s_diff) << 2;
		last = i->weight_index;
		
		if (i->x == 1.) 
			c = run_len_encode(c, diff);
		else if (i->x == -1.) 
			c = run_len_encode(c, diff | neg_1);
		else
		{
			c = run_len_encode(c, diff | general);
			*(float *)c = i->x;
			c += sizeof(float);
		}
	}
	buf.set(c);
	*(size_t*)storage_size_loc = c - storage_size_loc - sizeof(size_t);  
}

void NativeCacheWriter::cache_example(VwExample* &ex)
{
	/*
	cache_tag(cache,ae->tag);
	output_byte(cache, ae->indices.index());
	for (size_t* b = ae->indices.begin; b != ae->indices.end; b++)
		output_features(cache, *b, ae->atomics[*b].begin,ae->atomics[*b].end);
	*/

	
}

