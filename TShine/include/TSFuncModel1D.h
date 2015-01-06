/*
 * TSFuncModel1D.h
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNCMODEL1D_H_
#define TSFUNCMODEL1D_H_

#include "TSFuncModel.h"

/*
 *
 */
class TSFuncModel1D: public TSFuncModel {



public:

	TSFuncModel1D(TString name,TString title);
	TSFuncModel1D(TString name,TString title,const TSArgList& set, const TSArgSet &coefs);
	TSFuncModel1D(TString name,TString title,const TSArgList& set);
	TSFuncModel1D(TString name,TString title,TString formula,const TSArgList& set, const TSArgSet &coefs);
	TSFuncModel1D(TString name,TString title,TString formula,const TSArgList& set);
	virtual ~TSFuncModel1D();

	virtual double Eval(double x, double y=0) const;
	virtual double Eval() const;
	virtual double Evaluate() const;

	double operator()(const double *x) const { return 0;}
	double EvalFunctor(const double *x) const {return 0;}

private:

	int fCheckFunctions();

	double fEvaluateLinear() const;
	double fEvaluateTFormula() const;


};

#endif /* TSFUNCMODEL1D_H_ */
