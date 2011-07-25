#ifndef _VOWPALWABBIT_H__
#define _VOWPALWABBIT_H__

#include <shogun/lib/vw/vw_types.h>
#include <shogun/features/StreamingVwFeatures.h>

//namespace shogun
//{

class CVowpalWabbit
{
public:
	CVowpalWabbit(CStreamingVwFeatures* feat = NULL);
	
	void train(CStreamingVwFeatures* feat = NULL);

private:
	/** 
	 * Sets the train/update methods depending on parameters 
	 * set, eg. adaptive or not
	 */
	void set_learner();
	
	void init(CStreamingVwFeatures* feat = NULL);
	
protected:
	/// Features
	CStreamingVwFeatures* features;
	/// Environment for VW, i.e., globals
	VwEnvironment* env;

	VwMachine* learner;
	
	VwRegressor* reg;
};
