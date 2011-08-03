#include <shogun/lib/vw/cache/nativecache_write.h>

using namespace shogun;

NativeCacheWriter::NativeCacheWriter(const char* fname, VwEnvironment* env_to_use)
	: VwCacheWriter(fname, env_to_use)
{
	init();
	buf.use_file(fd);

	string version = "5.1";
	size_t numbits = env->num_bits;
	size_t v_length = version.length()+1;

	// Version and numbits info
	buf.write_file(&v_length, sizeof(size_t));
	buf.write_file(version.c_str(),v_length);
	buf.write_file(&numbits, sizeof(size_t));
}

NativeCacheWriter::~NativeCacheWriter()
{
	buf.flush();
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


char* NativeCacheWriter::bufcache_label(VwLabel* ld, char* c)
{
	*(float *)c = ld->label;
	c += sizeof(ld->label);
	*(float *)c = ld->weight;
	c += sizeof(ld->weight);
	*(float *)c = ld->initial;
	c += sizeof(ld->initial);
	return c;
}

void NativeCacheWriter::cache_label(VwLabel* ld)
{
	char *c;
	buf.buf_write(c, sizeof(ld->label)+sizeof(ld->weight)+sizeof(ld->initial));
	c = bufcache_label(ld,c);
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
	cache_label(ex->ld);
	cache_tag(ex->tag);
	output_byte(ex->indices.index());
	for (size_t* b = ex->indices.begin; b != ex->indices.end; b++)
		output_features(*b, ex->atomics[*b].begin,ex->atomics[*b].end);
}

