/*
 * Copyright (c) 2009 Yahoo! Inc.  All rights reserved.  The copyrights
 * embodied in the content of this file are licensed under the BSD
 * (revised) open source license.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Adaptation of Vowpal Wabbit v5.1.
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society.
 */

#ifndef _VOWPALWABBIT_H__
#define _VOWPALWABBIT_H__

#include <shogun/classifier/vw/vw_common.h>
#include <shogun/classifier/vw/sparse_dense.h>
#include <shogun/classifier/vw/learners/VwAdaptiveLearner.h>
#include <shogun/classifier/vw/learners/VwNonAdaptiveLearner.h>
#include <shogun/classifier/vw/VwRegressor.h>
#include <shogun/features/StreamingVwFeatures.h>

namespace shogun
{

class CVowpalWabbit
{
public:
	/**
	 * Constructor, taking a features object
	 * as an optional argument
	 *
	 * @param feat StreamingVwFeatures object
	 */
	CVowpalWabbit(CStreamingVwFeatures* feat = NULL);

	/**
	 * Destructor
	 */
	~CVowpalWabbit();

	/**
	 * Set whether learning is adaptive or not
	 *
	 * @param adaptive_learning true if adaptive
	 */
	void set_adaptive(bool adaptive_learning);

	/**
	 * Set number of passes (only works for cached input)
	 *
	 * @param passes number of passes
	 */
	void set_num_passes(int32_t passes)
	{
		env->num_passes = passes;
	}

	/**
	 * Add a pair of namespaces whose features should
	 * be crossed for quadratic updates
	 *
	 * @param pair a string with the two namespace names concatenated
	 */
	void add_quadratic_pair(char* pair);

	/**
	 * Train on a StreamingVwFeatures object
	 *
	 * @param feat StreamingVwFeatures to train using
	 */
	virtual void train(CStreamingVwFeatures* feat = NULL);

	/**
	 * Predict for an example
	 *
	 * @param ex VwExample to predict for
	 *
	 * @return prediction
	 */
	virtual float predict(VwExample* ex);

	/**
	 * Get the environment
	 *
	 * @return environment as CVwEnvironment*
	 */
	virtual CVwEnvironment* get_env()
	{
		SG_REF(env);
		return env;
	}

	//virtual float64_t apply(SGSparseVector<float64_t> vec);
private:
	/**
	 * Initialize members
	 *
	 * @param feat Features object
	 */
	virtual void init(CStreamingVwFeatures* feat = NULL);

	/**
	 * Sets the train/update methods depending on parameters
	 * set, eg. adaptive or not
	 */
	virtual void set_learner();

	/**
	 * Predict with l1 regularization
	 *
	 * @param ex example
	 *
	 * @return prediction
	 */
	virtual float inline_l1_predict(VwExample* &ex);

	/**
	 * Predict with no regularization term
	 *
	 * @param ex example
	 *
	 * @return prediction
	 */
	virtual float inline_predict(VwExample* &ex);

	/**
	 * Reduce the prediction within limits
	 *
	 * @param ret prediction
	 *
	 * @return prediction within limits
	 */
	virtual float finalize_prediction(float ret);

	/**
	 * Print statistics
	 *
	 * @param ex example
	 */
	virtual void print_update(VwExample* &ex);

protected:
	/// Features
	CStreamingVwFeatures* features;

	/// Environment for VW, i.e., globals
	CVwEnvironment* env;

	/// Learner to use
	CVwLearner* learner;

	/// Regressor
	CVwRegressor* reg;
};

}
#endif // _VOWPALWABBIT_H__
