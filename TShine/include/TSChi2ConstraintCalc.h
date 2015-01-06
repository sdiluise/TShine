/*
 * TSChi2ConstraintCalc.h
 *
 *  Created on: Oct 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSCHI2CONSTRAINTCALC_H_
#define TSCHI2CONSTRAINTCALC_H_


#include <TMath.h>
#include <TMatrixTSym.h>
#include <TMatrixT.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TDecompChol.h>
#include <TDecompLU.h>

#include "TSNamed.h"

#include "TSNuiParam.h"
#include "TSNuiParamSet.h"
#include "TSCovMatrixMgr.h"

/*
 *
 */
class TSChi2ConstraintCalc: public TSNamed {

public:

	TSChi2ConstraintCalc();
	virtual ~TSChi2ConstraintCalc();

	void Clear();

	double Eval() const;

	virtual double Compute() const;

	int GetDimension() const {return fDimension;}

	void Init();


	void Configure(const TSNuiParamSet &, const TSCovMatrixMgr &);

private:


	int fDimension;


	TSNuiParamSet fParamSet;


	static const int kDimension = 500;

	TSNuiParam *fParamArray[kDimension];

	double fCacheParCenter[kDimension];
	double fCacheParValue[kDimension];

	double fCacheWeightsArray[kDimension*kDimension];
	int fCacheWeightsArrayIdx[kDimension*kDimension];
	int fCacheWeightsArrayIdy[kDimension*kDimension];
	double fCacheWeightsMatix[kDimension][kDimension];

	TMatrixTSym<double> fMatCov;
	TMatrixTSym<double> fMatW;


};

#endif /* TSCHI2CONSTRAINTCALC_H_ */
