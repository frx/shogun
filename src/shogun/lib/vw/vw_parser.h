#ifndef _VW_PARSER_H__
#define _VW_PARSER_H__

#include <shogun/lib/vw/substring.h>
#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/hash.h>
#include <shogun/lib/vw/parse_primitives.h>
#include <shogun/lib/vw/parse_example.h>
#include <shogun/lib/vw/protobuf_write.h>
#include <shogun/lib/vw/nativecache_write.h>

namespace shogun
{

	class VwParser
	{
	public:
		/**
		 * Constructor taking environment as parameter (optional)
		 * If not specified, a default environment is used.
		 *
		 * @param env_to_use VwEnvironment to use
		 */
		VwParser(VwEnvironment* env_to_use = NULL)
		{
			if (env_to_use == NULL)
				env = new VwEnvironment();
			else
				env = env_to_use;

			hasher = hashstring;
			
			cache_writer = new NativeCacheWriter("cache_shogun.dat", env);
			write_cache = true;
		}

		/**
		 * Destructor
		 */
		~VwParser()
		{
			delete env;
			delete cache_writer;
		}

		/**
		 * Get the environment
		 *
		 * @return environment as VwEnvironment*
		 */
		VwEnvironment* get_env()
		{
			return env;
		}

		/**
		 * Set the environment
		 *
		 * @param env_to_use environment as VwEnvironment*
		 */
		void set_env(VwEnvironment* env_to_use)
		{
			env = env_to_use;
		}

		/**
		 * Reads input from the buffer and parses it into a VwExample
		 *
		 * @param buf IOBuffer which contains input
		 * @param ex parsed example
		 *
		 * @return number of characters read for this example
		 */
		int32_t read_features(CIOBuffer* buf, VwExample*& ex);

		/** 
		 * Set whether to write cache file or not
		 * 
		 * @param wr_cache write cache or not
		 */
		void set_write_cache(bool wr_cache)
		{
			write_cache = wr_cache;
		}

		/** 
		 * Return whether cache will be written or not
		 * 
		 * @return will cache be written?
		 */
		bool get_write_cache()
		{
			return write_cache;
		}
		
		/**
		 * Update min and max labels seen in the environment
		 *
		 * @param label current label based on which to update
		 */
		void set_mm(float64_t label)
		{
			env->min_label = CMath::min(env->min_label, label);
			if (label != FLT_MAX)
				env->max_label = CMath::max(env->max_label, label);
		}

		/**
		 * A dummy function performing no operation in case training
		 * is not to be performed.
		 *
		 * @param label label
		 */
		void noop_mm(float64_t label)
		{
		}

		/**
		 * Function which is actually called to update min and max labels
		 * Should be set to one of the functions implemented for this.
		 *
		 * @param label label based on which to update
		 */
		void set_minmax(float64_t label)
		{
			set_mm(label);
		}

	public:
		/// Hash function to use, of type hash_func_t
		hash_func_t hasher;

	protected:
		/// Environment of VW - used by parser
		VwEnvironment* env;

		VwCacheWriter* cache_writer;

		bool write_cache;
	};

}
#endif
