#ifndef _VW_MACHINE_H__
#define _VW_MACHINE_H__

#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/vw_regressor.h>

namespace shogun
{
	
class VwLearner
{
public:
	/** 
	 * Constructor, initializes regressor and environment
	 * 
	 * @param regressor regressor
	 * @param vw_env environment
	 */
	VwLearner(VwRegressor* regressor, VwEnvironment* vw_env)
	{
		reg = regressor;
		env = vw_env;
	}

	/** 
	 * Destructor
	 */
	virtual ~VwLearner()
	{
	}

	/** 
	 * Train on the example
	 * 
	 * @param ex example
	 * @param update update
	 */
	virtual void train(VwExample* &ex, float update) = 0;

protected:

	VwRegressor *reg;

	VwEnvironment *env;
};

}
#endif
