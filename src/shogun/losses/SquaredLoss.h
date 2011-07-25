#ifndef _SQUAREDLOSS_H__
#define _SQUAREDLOSS_H__

#include <shogun/lib/losses/LossFunction.h>

namespace shogun
{

class CSquaredLoss: public CLossFunction
{
public:
	/**
	 * Constructor
	 */
	CSquaredLoss() {};

	/**
	 * Destructor
	 */
	~CSquaredLoss() {};

	float64_t loss(float64_t prediction, float64_t label);

	float64_t get_update(float64_t prediction, float64_t label, float64_t eta_t, float64_t norm);

	float64_t square_grad(float64_t prediction, float64_t label);

	float64_t first_derivative(float64_t prediction, float64_t label);

	float64_t second_derivative(float64_t prediction, float64_t label);
};

}
