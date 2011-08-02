#ifndef _VW_NATIVECACHE_READ_H__
#define _VW_NATIVECACHE_READ_H__

#include <shogun/lib/vw/cache/cache_read.h>

#include <iostream>
#include <fstream>
#include <string>

namespace shogun
{

/// Packed structure for speed
struct one_float
{
	float f;
} __attribute__((packed));

/** @brief Class NativeCacheReader reads from a cache exactly as
 * that which has been produced by VW's default cache format.
 */
class NativeCacheReader: public VwCacheReader
{
public:
	/**
	 * Constructor, opens a file whose name is specified
	 *
	 * @param fname file name
	 */
	NativeCacheReader(const char* fname, VwEnvironment* env_to_use);

	/**
	 * Constructor, passed a file descriptor
	 *
	 * @param f descriptor of opened file
	 */
	NativeCacheReader(int f, VwEnvironment* env_to_use);

	/**
	 * Destructor
	 */
	virtual ~NativeCacheReader();

	/**
	 * Read one cached example
	 *
	 * @return example as VwExample*
	 */
	virtual VwExample* read_cached_example();

private:

	/**
	 * Initialize members
	 */
	void init();

	/**
	 * Check whether cache is readable
	 */
	void check_cache_metadata();

	/**
	 * Decode an int from RLE-encoded data
	 *
	 * @param p pointer to data
	 * @param i decoded int
	 *
	 * @return new pointer position
	 */
	char* run_len_decode(char *p, size_t& i);

	/**
	 * Decode a signed int from an encoded unsigned form
	 *
	 * @param n encoded unsigned int
	 *
	 * @return decoded signed int
	 */
	inline int32_t ZigZagDecode(uint32_t n)
	{
		return (n >> 1) ^ -static_cast<int32_t>(n & 1);
	}

	/**
	 * Read a label from the buffer, helper
	 *
	 * @param ld label pointer
	 * @param c pointer to buffer
	 *
	 * @return new pointer position
	 */
	char* bufread_label(VwLabel* ld, char* c);

	/**
	 * Read a label from the buffer
	 *
	 * @param ld label pointer
	 *
	 * @return number of bytes read
	 */
	size_t read_cached_label(VwLabel* ld);

	/**
	 * Read the tag of an example from the cache
	 *
	 * @param ae
	 *
	 * @return
	 */
	size_t read_cached_tag(VwExample* ae);


protected:
	CIOBuffer buf;

private:
	const size_t int_size;
	const size_t char_size;
	size_t neg_1;
	size_t general;
};

}
#endif // _VW_NATIVECACHE_READ_H__


