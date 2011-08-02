#include <shogun/lib/vw/nativecache_read.h>

using namespace shogun;

NativeCacheReader::NativeCacheReader(const char* fname)
	: VwCacheReader(fname), int_size(6), char_size(2)
{
	init();
	buf.use_file(fd);
	check_cache_metadata();
}

NativeCacheReader::NativeCacheReader(int f)
	: VwCacheReader(f), int_size(6), char_size(2)
{
	init();
	buf.use_file(fd);
	check_cache_metadata();
}

NativeCacheReader::~NativeCacheReader()
{
	buf.close_file();
}

void NativeCacheReader::init()
{
	neg_1 = 1;
	general = 2;
}

void NativeCacheReader::check_cache_metadata()
{
	string version = "5.1";
	size_t numbits = 18;	// TODO: Pass this to the class
		
	size_t v_length;
	buf.read_file((char*)&v_length, sizeof(v_length));
	if(v_length > 29)
		SG_SERROR("Cache version too long, cache file is probably invalid.\n");
	
	char t[v_length];
	buf.read_file(t,v_length);
	if (strcmp(t,version.c_str()) != 0)
		SG_SERROR("Cache has possibly incompatible version!\n");
  
	int total = sizeof(size_t);
	char* p[total];
	if (buf.read_file(p, total) < total) 
		return;

	size_t cache_numbits = *(size_t *)p;
	if (cache_numbits != numbits)
		SG_SERROR("Bug encountered in caching! Bits used for weight in cache: %d.\n", cache_numbits);
}

char* NativeCacheReader::run_len_decode(char *p, size_t& i)
{
	// Read an int 7 bits at a time.
	size_t count = 0;
	while(*p & 128)\
		i = i | ((*(p++) & 127) << 7*count++);
	i = i | (*(p++) << 7*count);
	return p;
}

char* NativeCacheReader::bufread_label(VwLabel* ld, char* c)
{
	ld->label = *(float *)c;
	c += sizeof(ld->label);
	ld->weight = *(float *)c;
	c += sizeof(ld->weight);
	ld->initial = *(float *)c;
	c += sizeof(ld->initial);
	
	return c;
}

size_t NativeCacheReader::read_cached_label(VwLabel* ld)
{
	char *c;
	size_t total = sizeof(ld->label)+sizeof(ld->weight)+sizeof(ld->initial);
	if (buf.buf_read(c, total) < total) 
		return 0;
	c = bufread_label(ld,c);

	return total;
}

size_t NativeCacheReader::read_cached_tag(VwExample* ae)
{
	char* c;
	size_t tag_size;
	if (buf.buf_read(c, sizeof(tag_size)) < sizeof(tag_size))
		return 0;
	tag_size = *(size_t*)c;
	c += sizeof(tag_size);
  
	buf.set(c);
	if (buf.buf_read(c, tag_size) < tag_size) 
		return 0;
  
	ae->tag.erase();
	ae->tag.push_many(c, tag_size);
	return tag_size+sizeof(tag_size);
}

VwExample* NativeCacheReader::read_cached_example()
{
	VwExample* ae = new VwExample();
	// TODO: Use the env to get mask
	size_t mask =  (1<<18) - 1;
	size_t total = read_cached_label(&ae->ld);
	if (total == 0)
		return 0;
	if (read_cached_tag(ae) == 0)
		return 0;

	char* c;
	unsigned char num_indices = 0;
	if (buf.buf_read(c, sizeof(num_indices)) < sizeof(num_indices))
		return 0;
	num_indices = *(unsigned char*)c;
	c += sizeof(num_indices);

	buf.set(c);

	for (; num_indices > 0; num_indices--)
	{
		size_t temp;
		unsigned char index = 0;
		temp = buf.buf_read(c, sizeof(index) + sizeof(size_t));
		
		if (temp < sizeof(index) + sizeof(size_t))
			SG_SERROR("Truncated example! %d < %d bytes expected.\n",
				  temp, char_size + sizeof(size_t));

		index = *(unsigned char*) c;
		c += sizeof(index);
		ae->indices.push((size_t) index);

		v_array<VwFeature>* ours = ae->atomics+index;
		float64_t* our_sum_feat_sq = ae->sum_feat_sq+index;
		size_t storage = *(size_t *)c;
		c += sizeof(size_t);

		buf.set(c);
		total += storage;
		if (buf.buf_read(c, storage) < storage)
			SG_SERROR("Truncated example! Wanted %d bytes!\n", storage);

		char *end = c + storage;

		size_t last = 0;

		for (; c!=end; )
		{
			VwFeature f = {1., 0};
			temp = f.weight_index;
			c = run_len_decode(c, temp);
			f.weight_index = temp;

			if (f.weight_index & neg_1)
				f.x = -1.;
			else if (f.weight_index & general)
			{
				f.x = ((one_float *)c)->f;
				c += sizeof(float);
			}

			*our_sum_feat_sq += f.x*f.x;

			size_t diff = f.weight_index >> 2;
			int32_t s_diff = ZigZagDecode(diff);
			if (s_diff < 0)
				ae->sorted = false;

			f.weight_index = last + s_diff;
			last = f.weight_index;
			f.weight_index = f.weight_index & mask;

			ours->push(f);
		}
		buf.set(c);
	}

	return ae;
}
