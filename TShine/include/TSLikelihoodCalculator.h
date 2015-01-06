/*
 * TSLikelihoodCalculator.h
 *
 *  Created on: Aug 22, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSLIKELIHOODCALCULATOR_H_
#define TSLIKELIHOODCALCULATOR_H_

#include "MessageMgr.h"

#include "TSMinimizationFunction.h"
/*
 *
 */
class TSLikelihoodCalculator: public TSMinimizationFunction {

public:
	TSLikelihoodCalculator();
	TSLikelihoodCalculator(TString name, TString title);

	virtual ~TSLikelihoodCalculator();


	void Init();

    virtual double Compute() const;

private:

    void fClearCache();
    void fFillCache();
    mutable bool fCached;
    mutable double *fCacheX;
    mutable double *fCacheY;
    mutable double *fCacheData1D;
    mutable double *fCacheData2D;

    mutable double data2D[200][200];
    mutable double errors2D[200][200];
};

#endif /* TSLIKELIHOODCALCULATOR_H_ */
