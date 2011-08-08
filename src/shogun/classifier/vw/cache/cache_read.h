#ifndef _VW_CACHEREAD_H__
#define _VW_CACHEREAD_H__

#include <shogun/lib/common.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/io/IOBuffer.h>

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
	VwCacheReader(const char* fname, CVwEnvironment* env_to_use)
	{
		fd = open(fname, O_RDONLY);

		if (fd < 0)
			SG_SERROR("Error opening the file %s for reading from cache!\n");
		env = env_to_use;
		SG_REF(env);
	}

	/**
	 * Constructor which takes an already opened file descriptor
	 * as argument.
	 *
	 * @param f file descriptor
	 */
	VwCacheReader(int f, CVwEnvironment* env_to_use)
	{
		fd = f;
		env = env_to_use;
		SG_REF(env);
	}

	/**
	 * Destructor
	 */
	virtual ~VwCacheReader() { SG_UNREF(env); }

	/**
	 * Set the environment
	 *
	 * @param env_to_use environment
	 */
	virtual void set_env(CVwEnvironment* env_to_use) { env = env_to_use; }

	/**
	 * Get the environment
	 *
	 * @return environment
	 */
	virtual CVwEnvironment* get_env() { SG_REF(env); return env; }

	/**
	 * Function to read one example from the cache
	 *
	 * @return read example
	 */
	virtual bool read_cached_example(VwExample* const ae) = 0;

protected:

	/// File descriptor
	int fd;

	/// Environment
	CVwEnvironment* env;
};

}
#endif
