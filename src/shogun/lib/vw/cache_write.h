#ifndef _VW_CACHEWRITE_H__
#define _VW_CACHEWRITE_H__

#include <shogun/lib/common.h>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{

class VwCacheWriter
{
public:
	/** 
	 * Constructor, opens file specified by filename
	 * 
	 * @param fname name of file to open
	 */
	VwCacheWriter(const char* fname)
	{
		fd = open(fname, O_CREAT | O_TRUNC | O_RDWR);
 
		if (fd < 0)
			SG_SERROR("Error opening the file %s for writing cache!\n");
	}

	/** 
	 * Destructor
	 */
	virtual ~VwCacheWriter() {}

	/** 
	 * Function to cache one example to the file
	 * 
	 * @param ex example to cache
	 */
	virtual void cache_example(const VwExample* &ex) = 0;

protected:

	/// File descriptor
	int fd;
};

}
#endif
