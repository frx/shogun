#ifndef _VW_HASH_H__
#define _VW_HASH_H__

#include <stdint.h>	/* defines uint32_t etc */
#include <sys/types.h>	/* defines size_t */
#include <iostream>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{

const uint32_t hash_base = 97562527;
size_t hashstring (substring s, unsigned long h);
size_t hashall (substring s, unsigned long h);
hash_func_t getHasher(const std::string& s);
uint32_t uniform_hash(const void *key, size_t length, uint32_t initval);

}

#endif
