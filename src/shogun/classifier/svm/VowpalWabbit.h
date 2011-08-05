#ifndef _VOWPALWABBIT_H__
#define _VOWPALWABBIT_H__

#include <shogun/lib/vw/vw_types.h>
#include <shogun/lib/vw/adaptive.h>
#include <shogun/lib/vw/regular_machine.h>
#include <shogun/features/StreamingVwFeatures.h>

namespace shogun
{
	
class CVowpalWabbit
{
public:
	CVowpalWabbit(CStreamingVwFeatures* feat = NULL);

	~CVowpalWabbit();
	
	virtual void train(CStreamingVwFeatures* feat = NULL);

	virtual float predict(VwExample* ex);

	void set_adaptive(bool adaptive_learning);

	virtual VwEnvironment* get_env() { return env; }
private:
	/** 
	 * Sets the train/update methods depending on parameters 
	 * set, eg. adaptive or not
	 */
	virtual void set_learner();
	
	virtual void init(CStreamingVwFeatures* feat = NULL);

	virtual float inline_l1_predict(VwExample* &ex);

	virtual float inline_predict(VwExample* &ex);

	virtual float finalize_prediction(float ret);

	virtual void print_update(VwExample* &ex);
	
protected:
	/// Features
	CStreamingVwFeatures* features;
	/// Environment for VW, i.e., globals
	VwEnvironment* env;

	VwMachine* learner;
	
	VwRegressor* reg;
};

}
#endif
