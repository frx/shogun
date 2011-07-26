#ifndef _LOSSFUNCTION_H__
#define _LOSSFUNCTION_H__

#include <shogun/base/SGObject.h>
#include <shogun/lib/common.h>

namespace shogun
{

class CLossFunction: public CSGObject
{
public:
	/** 
	 * Destructor
	 */
	virtual ~CLossFunction() {};

	/** 
	 * Get loss for an example
	 * 
	 * @param prediction prediction
	 * @param label label
	 * 
	 * @return loss
	 */
	virtual float64_t loss(float64_t prediction, float64_t label) = 0;

	/** 
	 * Get the update for an example
	 * 
	 * @param prediction prediction
	 * @param label label
	 * @param eta_t learning rate
	 * @param norm norm
	 * 
	 * @return update
	 */
	virtual float64_t get_update(float64_t prediction, float64_t label,
				     float64_t eta_t, float64_t norm) = 0;

	/** 
	 * Get square of the gradient of the loss function
	 * 
	 * @param prediction prediction
	 * @param label label
	 * 
	 * @return square of gradient
	 */
	virtual float64_t square_grad(float64_t prediction, float64_t label) = 0;

	/** 
	 * Get first derivative of the loss function
	 * 
	 * @param prediction prediction
	 * @param label label
	 * 
	 * @return first derivative
	 */
	virtual float64_t first_derivative(float64_t prediction, float64_t label) = 0;

	/** 
	 * Get second derivative of the loss function
	 * 
	 * @param prediction prediction
	 * @param label label
	 * 
	 * @return second derivative
	 */
	virtual float64_t second_derivative(float64_t prediction, float64_t label) = 0;

	virtual const char* get_name() const { return "LossFunction"; }
};
}
#endif
