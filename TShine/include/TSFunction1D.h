/*
 * TSFunction1D.h
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *	  BaseFunction +
 *    DIM=1
 *    fX variable and cache
 *    Eval method ?
 *    TGraph (1D) builder
 *
 *
 *
 */

#ifndef TSFUNCTION1D_H_
#define TSFUNCTION1D_H_

#include <iostream>
#include <vector>

#include <TF1.h>
#include <TGraph.h>

#include "TSVariable.h"
#include "TSParameter.h"


#include "TSBaseFunction.h"


using std::cout;
using std::endl;

/*
 *
 */

class TSFunction1D: public TSBaseFunction {


public:
	TSFunction1D(TString name,TString title);
	TSFunction1D(TString name, TString title, const TSVariable &var_x);
	virtual ~TSFunction1D();


	//
	virtual double Eval(double x, double y=0) const;
	virtual double Eval() const;
	virtual double Evaluate() const {return 0;};   //-------///
	virtual double EvalFunc(double *x, double *p) const {return Eval(x[0]);}

	TGraph* BuildGraphX(double xmin, double xmax, int npts=300) const;
	TGraph* BuildGraphX(int npts=300, double y=0) const;
    TF1* BuildTF1(TString name="",int npx=300) const;

    virtual double ComputeIntegral() const;
    virtual double ComputeIntegral(double xmin, double xmax) const;

    TString GetAxisTitles() const;

    void InitFuncs();

	virtual void SetVarX(const TSVariable &);
	const TSVariable* GetVariableX() const;

	virtual double GetSigmaSpread() const {/*cout<<"sig spr: "<<this<<endl;*/ return 0;}


private:
	const TSVariable *fX;

protected:
	void fCacheX(double val) const {/*cout<<"caching x for: "<<this->Name()<<" "<<val<<endl;*/cached_x=val;}
	double fGetCachedX() const {return cached_x;}

protected:
	mutable double cached_x;

	mutable TF1* fTF1;
};

#endif /* TS1DFUNCTION_H_ */
