#include <shogun/losses/HingeLoss.h>

using namespace shogun;

float64_t CHingeLoss::loss(float64_t prediction, float64_t label)
{
	float64_t e = 1 - label * prediction;

	return (e > 0) ? e : 0;
}

float64_t CHingeLoss::get_update(float64_t prediction, float64_t label, float64_t eta_t, float64_t norm)
{
	if (label * prediction >= label * label)
		return 0;
	float64_t err = (label * label - label * prediction)/(label * label);
	float64_t normal = eta_t;

	return label * (normal < err ? normal : err)/norm;
}

float64_t CHingeLoss::square_grad(float64_t prediction, float64_t label)
{
	return first_derivative(prediction, label);
}

float64_t CHingeLoss::first_derivative(float64_t prediction, float64_t label)
{
	return (label * prediction >= label * label) ? 0 : -label;
}

float64_t CHingeLoss::second_derivative(float64_t prediction, float64_t label)
{
	return 0.;
}
