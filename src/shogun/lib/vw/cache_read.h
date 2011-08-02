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
	VwCacheReader(const char* fname, VwEnvironment* env_to_use)
	{
		fd = open(fname, O_RDONLY);

		if (fd < 0)
			SG_SERROR("Error opening the file %s for reading from cache!\n");
		env = env_to_use;
	}

	/**
	 * Constructor which takes an already opened file descriptor
	 * as argument.
	 *
	 * @param f file descriptor
	 */
	VwCacheReader(int f, VwEnvironment* env_to_use)
	{
		fd = f;
		env = env_to_use;
	}

	/**
	 * Destructor
	 */
	virtual ~VwCacheReader() {}

	/**
	 * Set the environment
	 *
	 * @param env_to_use environment
	 */
	virtual void set_env(VwEnvironment* env_to_use) { env = env_to_use; }

	/**
	 * Get the environment
	 *
	 * @return environment
	 */
	virtual VwEnvironment* get_env() { return env; }

	/**
	 * Function to read one example from the cache
	 *
	 * @return read example
	 */
	virtual VwExample* read_cached_example() = 0;

protected:

	/// File descriptor
	int fd;

	/// Environment
	VwEnvironment* env;
};

}
#endif
