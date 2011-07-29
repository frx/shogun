#ifndef _VW_CACHEREAD_H__
#define _VW_CACHEREAD_H__

#include <shogun/lib/common.h>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{

class VwCacheReader
{
public:

	/** 
	 * Constructor, opens file specified by filename
	 * 
	 * @param fname name of file to open
	 */
	VwCacheReader(const char* fname)
	{
		fd = open(fname, O_RDONLY);

		if (fd < 0)
			SG_SERROR("Error opening the file %s for reading from cache!\n");
	}

	/** 
	 * Constructor which takes an already opened file descriptor
	 * as argument.
	 * 
	 * @param f file descriptor
	 */
	VwCacheReader(int f)
	{
		fd = f;
	}
	
	/** 
	 * Destructor
	 */
	virtual ~VwCacheReader() {}

	/** 
	 * Function to read one example from the cache
	 * 
	 * @return read example
	 */
	virtual VwExample* read_cached_example() = 0;

protected:

	/// File descriptor
	int fd;
};

}
#endif
