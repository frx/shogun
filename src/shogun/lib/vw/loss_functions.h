#ifndef _VW_LOSSFUNCTIONS_H__
#define _VW_LOSSFUNCTIONS_H__

#include <string>

class loss_function
{
public:
	/** 
	 * Evaluate the example's loss.
	 * 
	 * @param prediction prediction
	 * @param label label
	 * 
	 * @return loss
	 */
	virtual float getLoss(float prediction, float label) = 0;
	
	/** 
	 * Evaluate the update scalar
	 * 
	 * @param prediction prediction
	 * @param label label
	 * @param eta_t learning rate
	 * @param norm norm
	 * 
	 * @return update
	 */
	virtual float getUpdate(float prediction, float label, float eta_t, float norm) = 0;

	virtual float getRevertingWeight(float prediction, float eta_t) = 0;

	virtual float getSquareGrad(float prediction, float label) = 0;

	virtual float first_derivative(float prediction, float label) = 0;

	virtual float second_derivative(float prediction, float label) = 0;

	virtual ~loss_function() {};
};

loss_function* getLossFunction(string funcName, double loss_parameter = 0);

#endif
