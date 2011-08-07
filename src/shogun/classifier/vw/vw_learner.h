#ifndef _VW_LEARNER_H__
#define _VW_LEARNER_H__

#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/VwRegressor.h>

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
	VwLearner(CVwRegressor* regressor, VwEnvironment* vw_env): reg(regressor), env(vw_env)
	{
		SG_REF(reg);
	}

	/** 
	 * Destructor
	 */
	virtual ~VwLearner()
	{
		SG_UNREF(reg);
	}

	/** 
	 * Train on the example
	 * 
	 * @param ex example
	 * @param update update
	 */
	virtual void train(VwExample* &ex, float update) = 0;

protected:

	CVwRegressor *reg;

	VwEnvironment *env;
};

}
#endif // _VW_LEARNER_H__
