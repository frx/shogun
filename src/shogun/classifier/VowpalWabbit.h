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
	/* This func is used only by adaptive_inline_train.. Consider shifting */
	void one_pf_quad_adaptive_update(float* weights, VwFeature& page_feature,
					 v_array<VwFeature> &offer_features, size_t mask,
					 float update, float g, VwExample* ex, size_t& ctr);
	
	void adaptive_inline_train(VwExample* &ex, float update);

	void init(CStreamingVwFeatures* feat = NULL);
	
protected:
	/// Features
	CStreamingVwFeatures* features;
	/// Environment for VW, i.e., globals
	VwEnvironment* env;

	VwRegressor* reg;
};
