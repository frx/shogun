#ifndef _VOWPALWABBIT_H__
#define _VOWPALWABBIT_H__

#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/sparse_dense.h>
#include <shogun/classifier/vw/learners/adaptive.h>
#include <shogun/classifier/vw/learners/nonadaptive.h>
#include <shogun/classifier/vw/vw_regressor.h>
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

	//virtual float64_t apply(SGSparseVector<float64_t> vec);
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
