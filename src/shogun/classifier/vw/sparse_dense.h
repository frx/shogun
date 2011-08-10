#ifndef _VW_SPARSEDENSE_H__
#define _VW_SPARSEDENSE_H__

#include <math.h>
#include <shogun/classifier/vw/vw_common.h>

namespace shogun
{
	inline float32_t sign(float32_t w){ if (w < 0.) return -1.; else  return 1.;}

	inline float32_t real_weight(float32_t w,float32_t gravity)
	{
		float32_t wprime = 0.;
		if (gravity < fabsf(w))
			wprime = sign(w)*(fabsf(w) - gravity);
		return wprime;
	}

	float32_t sd_add(float32_t* weights, size_t mask, VwFeature* begin, VwFeature* end);
	float32_t sd_truncadd(float32_t* weights, size_t mask, VwFeature* begin, VwFeature* end, float32_t gravity);

	float32_t sd_offset_add(float32_t* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset);
	void sd_offset_update(float32_t* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset, float32_t update, float32_t regularization);

	void quadratic(v_array<VwFeature> &f, const v_array<VwFeature> &first_part, 
		       const v_array<VwFeature> &second_part, size_t thread_mask);

	float32_t one_of_quad_predict(v_array<VwFeature> &page_features, VwFeature& offer_feature, float32_t* weights, size_t mask);

	float32_t one_pf_quad_predict(float32_t* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask);

	float32_t one_pf_quad_predict_trunc(float32_t* weights, VwFeature& f, v_array<VwFeature> &cross_features, size_t mask, float32_t gravity);

	float32_t offset_quad_predict(float32_t* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask, size_t offset);

	float32_t single_quad_weight(float32_t* weights, VwFeature& page_feature, VwFeature* offer_feature, size_t mask);

}
#endif
