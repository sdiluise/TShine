/*
 * TSFunc2DProd.h
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNC2DPROD_H_
#define TSFUNC2DPROD_H_

#include "TSFunction2D.h"

/*
 *
 */
class TSFunc2DProd: public TSFunction2D {

public:

	TSFunc2DProd(TString name,TString title, TSFunction1D &funcX, TSFunction1D &funcY);

	virtual ~TSFunc2DProd();

	//make these purely virtual?
	//virtual double Eval(double *x, double *y=0) const;
	//virtual double Eval() const;
	virtual double Evaluate() const;

	void SetFunctionX();
	void SetFunctionY();

private:

	TSFunction1D *fFuncX;
	TSFunction1D *fFuncY;


};

#endif /* TSFUNC2DPROD_H_ */
