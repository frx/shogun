#ifndef _ONLINESGDQN_H___
#define _ONLINESGDQN_H___
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

   Shogun adjustments (w) 2008 Soeren Sonnenburg
*/

#include "lib/common.h"
#include "features/Labels.h"
#include "machine/OnlineLinearMachine.h"
#include "features/StreamingDotFeatures.h"

namespace shogun
{
/** @brief class OnlineSGDQN */
class COnlineSGDQN : public COnlineLinearMachine
{
	public:
		/** default constructor  */
		COnlineSGDQN();

		/** constructor
		 *
		 * @param C constant C
		 */
		COnlineSGDQN(float64_t C);

		/** constructor
		 *
		 * @param C constant C
		 * @param traindat training features
		 * @param trainlab labels for training features
		 */
		COnlineSGDQN(float64_t C, CStreamingDotFeatures* traindat);

		virtual ~COnlineSGDQN();

		/** get classifier type
		 *
		 * @return classifier type OnlineSGDQN
		 */
		virtual inline EClassifierType get_classifier_type() { return CT_SGDQN; }

		/** train classifier
		 *
		 * @param data training data (parameter can be avoided if distance or
		 * kernel-based classifiers are used and distance/kernels are
		 * initialized with train data)
		 *
		 * @return whether training was successful
		 */
		virtual bool train(CFeatures* data=NULL);

		/** calibrate
		 *
		 * @param max_vec_num Maximum number of vectors to calibrate using
		 * (optional) if set to -1, tries to calibrate using all vectors
		 * */
		void calibrate(int32_t max_vec_num=1000);

		/**
		 * Determine optimal value of t0
		 *
		 * @param max_vec_num Maximum number of vectors to use
		 * @param t0_epochs Number of epochs
		 *
		 * @return Optimum t0 value
		 */
		float64_t determine_t0(int32_t max_vec_num, int32_t t0_epochs);

		/**
		 * Test the sgd on max_vec_num vectors.
		 *
		 * @param max_vec_num Number of vectors to test upon
		 * @param cost Cost, returned by reference
		 */
		void test(int32_t max_vec_num, float64_t& cost);

		/**
		 * Set t0
		 *
		 * @param t0 t0
		 */
		inline void set_t0(float64_t t0) { t=t0; }

		/**
		 * Get t0
		 *
		 * @return t0
		 */
		inline float64_t get_t0() { return t; }

		/** set C
		 *
		 * @param c_neg new C constant for negatively labeled examples
		 * @param c_pos new C constant for positively labeled examples
		 *
		 */
		inline void set_C(float64_t c_neg, float64_t c_pos) { C1=c_neg; C2=c_pos; }

		/** get C1
		 *
		 * @return C1
		 */
		inline float64_t get_C1() { return C1; }

		/** get C2
		 *
		 * @return C2
		 */
		inline float64_t get_C2() { return C2; }

		/** set epochs
		 *
		 * @param e new number of training epochs
		 */
		inline void set_epochs(int32_t e) { epochs=e; }

		/** get epochs
		 *
		 * @return the number of training epochs
		 */
		inline int32_t get_epochs() { return epochs; }

		/** set lambda
		 *
		 * @param l value of regularization parameter lambda
		 */
		inline void set_lambda(float64_t l) { lambda=l; }

		/** get lambda
		 *
		 * @return the regularization parameter lambda
		 */
		inline float64_t get_lambda() { return lambda; }

		/** @return object name */
		inline virtual const char* get_name() const { return "OnlineSGDQN"; }

	private:
		void init();

	private:
		float64_t t;
		float64_t lambda;
		int32_t epochs;
		int32_t skip;
		int32_t count;

};
}
#endif
