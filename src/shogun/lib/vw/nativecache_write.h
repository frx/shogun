#ifndef _VW_NATIVECACHE_WRITE_H__
#define _VW_NATIVECACHE_WRITE_H__

#include <shogun/lib/vw/cache_write.h>

#include <iostream>
#include <fstream>
#include <string>

namespace shogun
{
/** @brief Class NativeCacheWriter writes a cache exactly as
 * that which would be produced by VW's default cache format.
 */
class NativeCacheWriter: public VwCacheWriter
{
public:
	/**
	 * Constructor, opens a file whose name is specified
	 *
	 * @param fname file name
	 */
	NativeCacheWriter(const char* fname, VwEnvironment* env_to_use);

	/**
	 * Destructor
	 */
	virtual ~NativeCacheWriter();

	/**
	 * Cache one example
	 *
	 * @param ex example to write to cache
	 */
	virtual void cache_example(VwExample* &ex);

private:
	/**
	 * Initialize members
	 */
	void init();

	/**
	 * Use run-length encoding on an int
	 *
	 * @param p compressed data ptr
	 * @param i int to compress
	 *
	 * @return ptr to compressed data
	 */
	char* run_len_encode(char *p, size_t i);

	/**
	 * Encode a signed int into an unsigned representation
	 *
	 * @param n signed int
	 *
	 * @return unsigned int
	 */
	inline uint32_t ZigZagEncode(int32_t n)
	{
		uint32_t ret = (n << 1) ^ (n >> 31);

		return ret;
	}

	/**
	 * Cache a label into the buffer, helper function
	 *
	 * @param ld label
	 * @param c pointer to last written buffer position
	 *
	 * @return new position of pointer
	 */
	char* bufcache_label(VwLabel* ld, char* c);

	/**
	 * Cache label into buffer
	 *
	 * @param ld label
	 */
	void cache_label(VwLabel* ld);

	/**
	 * Write the tag into the buffer
	 *
	 * @param tag tag
	 */
	void cache_tag(v_array<char> tag);

	/**
	 * Write a byte into the buffer
	 *
	 * @param s byte of data
	 */
	void output_byte(unsigned char s);

	/**
	 * Write the features into the buffer
	 *
	 * @param index namespace index
	 * @param begin first feature
	 * @param end pointer to end of features
	 */
	void output_features(unsigned char index, VwFeature* begin, VwFeature* end);

protected:
	CIOBuffer buf;

private:
	size_t neg_1;

	size_t general;

	size_t int_size;
};

}
#endif // _VW_NATIVECACHE_WRITE_H__
