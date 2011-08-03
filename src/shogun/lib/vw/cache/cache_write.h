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
	VwCacheWriter(const char* fname, VwEnvironment* env_to_use)
	{
		fd = open(fname, O_CREAT | O_TRUNC | O_RDWR, 0666);

		if (fd < 0)
			SG_SERROR("Error opening the file %s for writing cache!\n");

		env = env_to_use;
	}

	/**
	 * Destructor
	 */
	virtual ~VwCacheWriter() {}

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
	 * Function to cache one example to the file
	 *
	 * @param ex example to cache
	 */
	virtual void cache_example(VwExample* &ex) = 0;

protected:

	/// File descriptor
	int fd;

	/// Environment
	VwEnvironment* env;
};

}
#endif