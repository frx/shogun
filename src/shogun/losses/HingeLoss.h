#ifndef _HINGELOSS_H__
#define _HINGELOSS_H__

#include <shogun/losses/LossFunction.h>

namespace shogun
{

class CHingeLoss: public CLossFunction
{
public:
	/**
	 * Constructor
	 */
	CHingeLoss() {};

	/**
	 * Destructor
	 */
	~CHingeLoss() {};

	float64_t loss(float64_t prediction, float64_t label);

	float64_t get_update(float64_t prediction, float64_t label, float64_t eta_t, float64_t norm);

	float64_t square_grad(float64_t prediction, float64_t label);

	float64_t first_derivative(float64_t prediction, float64_t label);

	float64_t second_derivative(float64_t prediction, float64_t label);

	virtual const char* get_name() const { return "HingeLoss"; }
};

}

#endif
