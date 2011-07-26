#include <shogun/lib/vw/sparse_dense.h>
#include <math.h>

namespace shogun
{
	float sd_add(float* weights, size_t mask, VwFeature* begin, VwFeature* end)
	{
		float ret = 0.;
		for (VwFeature* f = begin; f!= end; f++)
			ret += weights[f->weight_index & mask] * f->x;
		return ret;
	}

	float sd_truncadd(float* weights, size_t mask, VwFeature* begin, VwFeature* end, float gravity)
	{
		float ret = 0.;
		for (VwFeature* f = begin; f!= end; f++)
		{
			float w = weights[f->weight_index & mask];
			float wprime = real_weight(w,gravity);
			ret += wprime*f->x;
		}
		return ret;
	}

	float sd_offset_add(float* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset)
	{
		float ret = 0.;
		for (VwFeature* f = begin; f!= end; f++)
			ret += weights[(f->weight_index + offset) & mask] * f->x;
		return ret;
	}

	float sd_offset_truncadd(float* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset, float gravity)
	{
		float ret = 0.;
		for (VwFeature* f = begin; f!= end; f++)
		{
			float w = weights[(f->weight_index+offset) & mask];
			float wprime = real_weight(w,gravity);
			ret += wprime*f->x;
		}
		return ret;
	}

	void sd_offset_update(float* weights, size_t mask, VwFeature* begin, VwFeature* end, size_t offset, float update, float regularization)
	{
		for (VwFeature* f = begin; f!= end; f++) 
			weights[(f->weight_index + offset) & mask] += update * f->x - regularization * weights[(f->weight_index + offset) & mask];
	} 

	void quadratic(v_array<VwFeature> &f, const v_array<VwFeature> &first_part, 
		       const v_array<VwFeature> &second_part, size_t thread_mask)
	{
		for (VwFeature* i = first_part.begin; i != first_part.end; i++)
		{
			size_t halfhash = quadratic_constant * i->weight_index;
			float i_value = i->x;
			for (VwFeature* ele = second_part.begin; ele != second_part.end; ele++) {
				size_t quad_index = (halfhash+ele->weight_index) & thread_mask;
				VwFeature temp = {i_value * ele->x, quad_index};
				f.push(temp);
			}
		}
	}

	float one_of_quad_predict(v_array<VwFeature> &page_features, VwFeature& offer_feature, float* weights, size_t mask)
	{
		float prediction = 0.0;
		for (VwFeature* i = page_features.begin; i != page_features.end; i++)
		{
			size_t halfhash = quadratic_constant * i->weight_index;
			prediction += weights[(halfhash + offer_feature.weight_index) & mask] * i->x;
		}
		return prediction * offer_feature.x;
	}

	float one_pf_quad_predict(float* weights, VwFeature& f, v_array<VwFeature> &cross_features, size_t mask)
	{
		size_t halfhash = quadratic_constant * f.weight_index;
  
		return f.x * 
			sd_offset_add(weights, mask, cross_features.begin, cross_features.end, halfhash);
	}

	float one_pf_quad_predict_trunc(float* weights, VwFeature& f, v_array<VwFeature> &cross_features, size_t mask, float gravity)
	{
		size_t halfhash = quadratic_constant * f.weight_index;
  
		return f.x * 
			sd_offset_truncadd(weights, mask, cross_features.begin, cross_features.end, halfhash, gravity);
	}

	float offset_quad_predict(float* weights, VwFeature& page_feature, v_array<VwFeature> &offer_features, size_t mask, size_t offset)
	{
		float prediction = 0.0;
		size_t halfhash = quadratic_constant * page_feature.weight_index + offset;

		for (VwFeature* ele = offer_features.begin; ele != offer_features.end; ele++)
			prediction += weights[(halfhash + ele->weight_index) & mask] * ele->x;

		return (prediction*page_feature.x);
	}

	float single_quad_weight(float* weights, VwFeature& page_feature, VwFeature* offer_feature, size_t mask)
	{
		size_t halfhash = quadratic_constant * page_feature.weight_index;
		float quad_weight = weights[(halfhash + offer_feature->weight_index) & mask] * offer_feature->x;
		return (quad_weight*page_feature.x);
	}
}
