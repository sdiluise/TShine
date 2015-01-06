/*
 * TSChisquareCalcuator.h
 *
 *  Created on: Aug 19, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 * Specialization of the Class TSMinimizationFunction
 *
 *
 *
 */

#ifndef TSCHISQUARECALCUATOR_H_
#define TSCHISQUARECALCUATOR_H_

#include "MessageMgr.h"

#include "TSMinimizationFunction.h"


/*
 *
 */

class TSChisquareCalculator: public TSMinimizationFunction {




public:

	TSChisquareCalculator();
	TSChisquareCalculator(TString name, TString title);

	virtual ~TSChisquareCalculator();

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

#endif /* TSCHISQUARECALCUATOR_H_ */
