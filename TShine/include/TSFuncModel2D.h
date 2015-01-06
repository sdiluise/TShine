/*
 * TSFuncModel2D.h
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNCMODEL2D_H_
#define TSFUNCMODEL2D_H_

#include "TFormula.h"
#include "TF2.h"


#include "TSFuncModel.h"

/*
 *
 */


class TSFuncModel2D: public TSFuncModel {

public:

	TSFuncModel2D(TString name,TString title);
	TSFuncModel2D(TString name,TString title,const TSArgList& set, const TSArgSet &coefs);
	TSFuncModel2D(TString name,TString title,const TSArgList& set);
	TSFuncModel2D(TString name,TString title,TString formula,const TSArgList& set, const TSArgSet &coefs);
	TSFuncModel2D(TString name,TString title,TString formula,const TSArgList& set);
	virtual ~TSFuncModel2D();

	virtual double Eval(double x, double y=0) const;
	virtual double Eval() const;
	virtual double Evaluate() const;
	virtual double EvalFunc(double *x, double *p) const {return Eval(x[0],x[1]);}

	void Init();

private:

	int fCheckFunctions();

	double fEvaluateLinear() const;
	double fEvaluateTFormula() const;

	mutable TFormula *f2Cache;

};

#endif /* TSFUNCMODEL2D_H_ */
