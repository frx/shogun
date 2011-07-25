#include <shogun/lib/losses/SquaredLoss.h>

using namespace shogun;

float64_t CSquaredLoss::loss(float64_t prediction, float64_t label)
{
	float64_t example_loss = (prediction - label) * (prediction - label);

	return example_loss;
}

float64_t CSquaredLoss::get_update(float64_t prediction, float64_t label, float64_t eta_t, float64_t norm)
{
	/* When exp(-eta_t) ~= 1, replace 1-exp(-eta_t)
	 * with its first order Taylor expansion around 0
	 * to avoid catastrophic cancellation.
	 */
	if (eta_t < 1e-6)
		return (label - prediction)*eta_t/norm;
	return (label - prediction)*(1-exp(-eta_t))/norm;
}

float64_t CSquaredLoss::square_grad(float64_t prediction, float64_t label)
{
	return (prediction - label) * (prediction - label);
}

float64_t CSquaredLoss::first_derivative(float64_t prediction, float64_t label)
{
	return 2. * (prediction - label);
}

float64_t CSquaredLoss::second_derivative(float64_t prediction, float64_t label)
{
	return 2;
}
