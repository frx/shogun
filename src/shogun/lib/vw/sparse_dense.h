#ifndef _VW_SPARSEDENSE_H__
#define _VW_SPARSEDENSE_H__

#include <math.h>
#include <shogun/lib/vw/vw_types.h>

namespace shogun
{
	inline float sign(float w){ if (w < 0.) return -1.; else  return 1.;}

	inline float real_weight(float w,float gravity)
	{
		float wprime = 0.;
		if (gravity < fabsf(w))
			wprime = sign(w)*(fabsf(w) - gravity);
		return wprime;
	}

	float sd_add(float* weights, size_t mask, VwFeature* begin, VwFeature* end);
	float sd_truncadd(float* weights, size_t mask, VwFeature* begin, VwFeature* end, float gravity);

	float sd_offset_add(float* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset);
	void sd_offset_update(float* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset, float update, float regularization);

	void quadratic(v_array<VwFeature> &f, const v_array<VwFeature> &first_part, 
		       const v_array<VwFeature> &second_part, size_t thread_mask);

	float one_of_quad_predict(v_array<VwFeature> &page_features, VwFeature& offer_feature, float* weights, size_t mask);

	float one_pf_quad_predict(float* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask);

	float one_pf_quad_predict_trunc(float* weights, VwFeature& f, v_array<VwFeature> &cross_features, size_t mask, float gravity);

	float offset_quad_predict(float* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask, size_t offset);

	float single_quad_weight(float* weights, VwFeature& page_feature, VwFeature* offer_feature, size_t mask);

}
#endif
