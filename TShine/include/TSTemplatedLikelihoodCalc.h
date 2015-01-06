/*
 * TSTemplatedLikelihoodCalc.h
 *
 *  Created on: Nov 3, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSTEMPLATEDLIKELIHOODCALC_H_
#define TSTEMPLATEDLIKELIHOODCALC_H_


#include <TObjArray.h>
#include <TH1.h>

#include "TSNuiParamSet.h"
#include "TSMinimizationFunction.h"
#include "TSChi2ConstraintCalc.h"


#include "TSEventDataSet.h"

/*
 *
 */

class TSTemplatedLikelihoodCalc: public TSMinimizationFunction {

public:
	TSTemplatedLikelihoodCalc();
	virtual ~TSTemplatedLikelihoodCalc();


	void AddObservedVsExpected(TH1 *Obs, TSEventDataSet *Exp);

	void AddParamChi2Constraint(TSChi2ConstraintCalc &);

	virtual double Compute() const;

	virtual double ComputeChi2Constraint() const;

	virtual double GetValue() const {return fValue;}
	virtual double GetLikeValue() const {return fLikeValue;}

	virtual double GetChi2ConstraintValue() const {return fConstraintValue;}


	double GetLikeValuePart(int);

	bool HasParamSet() const {return fHasParamSet;}

	void Init();

	//void RandomizeDataObs();

protected:


private:

	mutable double fValue;
	mutable std::vector<double> fLikeValuePart;

	mutable double fLikeValue;
	mutable double fConstraintValue;

	int fNHistoPairs;
	int fNConstraints;

	TObjArray aObsData;
	TObjArray aObsDataRef;
	TObjArray aExpData;
	TObjArray aExpDataMgr;
	TObjArray aConstraint;


	TSNuiParamSet fNuiParam_Set;
	bool fHasParamSet;

};

#endif /* TSTEMPLATEDLIKELIHOODCALC_H_ */
