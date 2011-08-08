#ifndef _VW_CACHEWRITE_H__
#define _VW_CACHEWRITE_H__

#include <shogun/lib/common.h>
#include <shogun/classifier/vw/vw_common.h>
#include <shogun/io/IOBuffer.h>

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
	VwCacheWriter(const char* fname, CVwEnvironment* env_to_use)
	{
		fd = open(fname, O_CREAT | O_TRUNC | O_RDWR, 0666);

		if (fd < 0)
			SG_SERROR("Error opening the file %s for writing cache!\n");

		env = env_to_use;
		SG_REF(env);
	}

	/**
	 * Destructor
	 */
	virtual ~VwCacheWriter() { SG_UNREF(env); }

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
	 * Function to cache one example to the file
	 *
	 * @param ex example to cache
	 */
	virtual void cache_example(VwExample* &ex) = 0;

protected:

	/// File descriptor
	int fd;

	/// Environment
	CVwEnvironment* env;
};

}
#endif
