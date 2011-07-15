/*
   SVM with stochastic gradient
   Copyright (C) 2007- Leon Bottou
   
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA
   $Id: svmsgd.cpp,v 1.13 2007/10/02 20:40:06 cvs Exp $

   Shogun adjustments (w) 2008-2009 Soeren Sonnenburg
*/

#include "OnlineSVMSGD.h"
#include "base/Parameter.h"
#include "lib/Signal.h"

using namespace shogun;

// Available losses
#define HINGELOSS 1
#define SMOOTHHINGELOSS 2
#define SQUAREDHINGELOSS 3
#define LOGLOSS 10
#define LOGLOSSMARGIN 11

// Select loss
#define LOSS HINGELOSS

// One when bias is regularized
#define REGULARIZEBIAS 0

inline
float64_t loss(float64_t z)
{
#if LOSS == LOGLOSS
	if (z >= 0)
		return log(1+exp(-z));
	else
		return -z + log(1+exp(z));
#elif LOSS == LOGLOSSMARGIN
	if (z >= 1)
		return log(1+exp(1-z));
	else
		return 1-z + log(1+exp(z-1));
#elif LOSS == SMOOTHHINGELOSS
	if (z < 0)
		return 0.5 - z;
	if (z < 1)
		return 0.5 * (1-z) * (1-z);
	return 0;
#elif LOSS == SQUAREDHINGELOSS
	if (z < 1)
		return 0.5 * (1 - z) * (1 - z);
	return 0;
#elif LOSS == HINGELOSS
	if (z < 1)
		return 1 - z;
	return 0;
#else
# error "Undefined loss"
#endif
}

inline
float64_t dloss(float64_t z)
{
#if LOSS == LOGLOSS
	if (z < 0)
		return 1 / (exp(z) + 1);
	float64_t ez = exp(-z);
	return ez / (ez + 1);
#elif LOSS == LOGLOSSMARGIN
	if (z < 1)
		return 1 / (exp(z-1) + 1);
	float64_t ez = exp(1-z);
	return ez / (ez + 1);
#elif LOSS == SMOOTHHINGELOSS
	if (z < 0)
		return 1;
	if (z < 1)
		return 1-z;
	return 0;
#elif LOSS == SQUAREDHINGELOSS
	if (z < 1)
		return (1 - z);
	return 0;
#else
	if (z < 1)
		return 1;
	return 0;
#endif
}


COnlineSVMSGD::COnlineSVMSGD()
: COnlineLinearMachine()
{
	init();
}

COnlineSVMSGD::COnlineSVMSGD(float64_t C)
: COnlineLinearMachine()
{
	init();

	C1=C;
	C2=C;
}

COnlineSVMSGD::COnlineSVMSGD(float64_t C, CStreamingDotFeatures* traindat)
: COnlineLinearMachine()
{
	init();
	C1=C;
	C2=C;

	set_features(traindat);
}

COnlineSVMSGD::~COnlineSVMSGD()
{
}

/* Bc.combine_and_clip((t-skip)/(t+skip),B,2*skip/(t+skip),
   1/(100*lambda),100/lambda); */

void combine_and_clip(SGVector<float64_t> &Bc,
		      float64_t c1,
		      const SGVector<float64_t> &B,
		      float64_t c2,
		      float64_t vmin,
		      float64_t vmax)
{
	// Expand beforehand
	// s == v2
	int32_t dims = B.vlen;
	for (int32_t i=0; i<dims; i++)
		if (B.vector[i])
		{
			Bc.vector[i] = Bc.vector[i] * c1 + B.vector[i] * c2;
			Bc.vector[i] = CMath::min(CMath::max(Bc.vector[i], vmin),
						  vmax);
		}
}

// B.compute_ratio(x, lambda, w_1, w, y*diffloss);

void compute_ratio(SGVector<float64_t> &B,
		   const SGVector<float64_t> &x,
		   const SGVector<float64_t> &w_old,
		   const SGVector<float64_t> &w_new,
		   float64_t lambda,
		   float64_t loss)
{
	// Perform expansion beforehand?
	// d = (float*) vec, i.e., B
	// s = (float*) wp, i.e., old w
	// sp = (float*) wpp, i.e., new w
	int32_t dims = w_new.vlen;
	for (int32_t i=0; i<dims; i++)
	{
		// If feature is zero, skip
		if (!x.vector[i])
			continue;

		float64_t diffw = w_new.vector[i] - w_old.vector[i];
		if (diffw)
			B.vector[i] += diffw/(lambda * diffw + loss * x.vector[i]);
		else
			B.vector[i] += 1/lambda;
	}
}

bool COnlineSVMSGD::train(CFeatures* data)
{
	if (data)
	{
		if (!data->has_property(FP_STREAMING_DOT))
			SG_ERROR("Specified features are not of type CStreamingDotFeatures\n");
		set_features((CStreamingDotFeatures*) data);
	}

	features->start_parser();
	
	// allocate memory for w and initialize everyting w and bias with 0
	ASSERT(features);
	ASSERT(features->get_has_labels());
	if (w)
		delete[] w;
	w_dim=1;
	w=new float64_t;
	bias=0;

	count = skip;
	bool updateB = false;
	
	// Shift t in order to have a 
	// reasonable initial learning rate.
	// This assumes |x| \approx 1.
	float64_t maxw = 1.0 / sqrt(lambda);
	float64_t typw = sqrt(maxw);
	float64_t eta0 = typw / CMath::max(1.0,dloss(-typw));
	t = 1 / (eta0 * lambda);

	SG_INFO("lambda=%f, epochs=%d, eta0=%f\n", lambda, epochs, eta0);
	
	//do the sgd
	calibrate();
	if (features->is_seekable())
		features->reset_stream();

	CSignal::clear_cancel();
	
	int32_t vec_count;
	for(int32_t e=0; e<epochs && (!CSignal::cancel_computations()); e++)
	{
		vec_count=0;
		count = skip;
		while (features->get_next_example())
		{
			vec_count++;
			// Expand w vector if more features are seen in this example
			features->expand_if_required(w, w_dim);
			features->expand_if_required(Bc.vector, Bc.vlen);
			features->expand_if_required(B.vector, B.vlen);

			// Get the actual vector in dense form
			SGVector<float64_t> x;
			features->expand_if_required(x.vector, x.vlen);
			features->add_to_dense_vec(1, x.vector, w_dim);

			float64_t y = features->get_label();
			float64_t z = y * features->dense_dot(w, w_dim);
			float64_t eta = 1.0 / t;

			if (updateB == true)
			{

#if LOSS < LOGLOSS
			if (z < 1)
#endif
			{
				SGVector<float64_t> w_1;
				w_1.vector = CMath::clone_vector(w, w_dim);

				float64_t loss_1 = dloss(z);

				// w.add(x, eta*loss_1*y, Bc);
				// Multiply dense vectors x and Bc
				CMath::vector_multiply(x_mult_Bc, x.vector, Bc.vector, w_dim);

				// w_new = 1 * (w) + k * (x_mult_Bc)
				CMath::add(w, 1, w, eta * loss_1 * y, x_mult_Bc, w_dim);

				// Calculate z using new w
				float64_t z2 = y * features->dense_dot(w, w_dim);
				float64_t diffloss = dloss(z2) - loss_1;

				if (diffloss)
				{
					compute_ratio(B, x, w_1, w, lambda, y*diffloss);

					if (t > skip)
						combine_and_clip(Bc,
								 (t-skip)/(t+skip),
								 B,
								 2*skip/(t+skip),
								 1/(100*lambda),
								 100/lambda);
					else
						combine_and_clip(Bc,
								 t/(t+skip),
								 B,
								 skip/(t+skip),
								 1/(100*lambda),
								 100/lambda);

					// B.clear()
					memset(B.vector, 0, ((sizeof) float64_t) * B.vlen);
				}
			}
			updateB = false;
			}
			else
			{
				if (--count <= 0)
				{
					//w.add(x, eta*dloss(z)*y, Bc);
					CMath::vector_multiply(x_mult_Bc, x.vector, Bc.vector, w_dim);
					CMath::add(w, 1, w, eta * dloss(z) * y, x_mult_Bc, w_dim);
					count = skip;
					updateB = true;
				}
#if LOSS < LOGLOSS
				if (z < 1)
#endif
				{
					//w.add(x, eta*dloss(z)*y, Bc);
					CMath::vector_multiply(x_mult_Bc, x.vector, Bc.vector, w_dim);
					CMath::add(w, 1, w, eta * dloss(z) * y, x_mult_Bc, w_dim);
				}
			}
			
			t += 1;
			features->release_example();
		}
		
		// If the stream is seekable, reset the stream to the first
		// example (for epochs > 1)
		if (features->is_seekable() && e < epochs-1)
			features->reset_stream();
		else
			break;
	}
	features->end_parser();
	
	float64_t wnorm =  CMath::dot(w, w, w_dim);
	SG_INFO("Norm: %.6f, Bias: %.6f\n", wnorm, bias);

	return true;
}

void COnlineSVMSGD::calibrate(int32_t max_vec_num)
{ 
	int32_t c_dim=1;
	float64_t* c=new float64_t;
	
	// compute average gradient size
	int32_t n = 0;
	float64_t m = 0;
	float64_t r = 0;

	while (features->get_next_example() && m<=1000)
	{
		//Expand c if more features are seen in this example
		features->expand_if_required(c, c_dim);
			
		r += features->get_nnz_features_for_vector();
		features->add_to_dense_vec(1, c, c_dim, true);

		//waste cpu cycles for readability
		//(only changed dims need checking)
		m=CMath::max(c, c_dim);
		n++;

		features->release_example();
		if (n>=max_vec_num)
			break;
	}

	SG_PRINT("Online SGD calibrated using %d vectors.\n", n);

	// bias update scaling
	bscale = m/n;

	// compute weight decay skip
	skip = (int32_t) ((16 * n * c_dim) / r);

	SG_INFO("using %d examples. skip=%d  bscale=%.6f\n", n, skip, bscale);

	delete[] c;
}

void COnlineSVMSGD::init()
{
	t=1;
	C1=1;
	C2=1;
	lambda=1e-4;
	wscale=1;
	bscale=1;
	epochs=1;
	skip=1000;
	count=1000;
	use_bias=true;

	use_regularized_bias=false;

	m_parameters->add(&C1, "C1",  "Cost constant 1.");
	m_parameters->add(&C2, "C2",  "Cost constant 2.");
	m_parameters->add(&lambda, "lambda", "Regularization parameter.");
	m_parameters->add(&wscale, "wscale",  "W scale");
	m_parameters->add(&bscale, "bscale",  "b scale");
	m_parameters->add(&epochs, "epochs",  "epochs");
	m_parameters->add(&skip, "skip",  "skip");
	m_parameters->add(&count, "count",  "count");
	m_parameters->add(&use_bias, "use_bias",  "Indicates if bias is used.");
	m_parameters->add(&use_regularized_bias, "use_regularized_bias",  "Indicates if bias is regularized.");
}
