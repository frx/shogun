#ifndef _VW_HASH_H__
#define _VW_HASH_H__

#include <stdint.h>	/* defines uint32_t etc */
#include <sys/types.h>	/* defines size_t */

namespace shogun
{

const uint32_t hash_base = 97562527;
uint32_t uniform_hash(const void *key, size_t length, uint32_t initval);

}

#endif
