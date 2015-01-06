/*
 * TSFunction2D.h
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNCTION2D_H_
#define TSFUNCTION2D_H_


#include <TF2.h>
#include <TGraph.h>
#include <TGraph2D.h>

/*
 *
 */



#include "TSFunction1D.h"


class TSFunction2D: public TSFunction1D {

public:

	TSFunction2D(TString name, TString title);
	TSFunction2D(TString name, TString title, const TSVariable &var_x, const TSVariable &var_y);

	virtual ~TSFunction2D();

	//
	virtual double Eval(double x, double y=0) const;
	virtual double Eval() const;
	virtual double EvalFunc(double *x, double *p) const {return Eval(x[0],x[1]);}

	//TGraph* BuildGraph(int ntpx=100, double y=0) const {return 0;}
	//TGraph* BuildGraph(int ntps=100) const {return 0;}


	TGraph2D* BuildGraph2D(double xmin, double xmax, double ymin, double ymax,int nptx=100, int npty=100) const;
	TGraph2D* BuildGraph2D(int nptx=100, int npty=100) const;
	TF2* BuildTF2(TString name="",int npx=100, int npy=100) const;
	virtual double ComputeIntegral() const;
	virtual double ComputeIntegral(double xmin, double xmax, double ymin, double ymax) const;

	TString GetAxisTitles() const;

	void InitFuncs();

	virtual void SetVarY(const TSVariable &);
	const TSVariable* GetVariableY() const;


private:

	const TSVariable *fY;

protected:
	void fCacheY(double val) const {cached_y=val;}
	double fGetCachedY() const {return cached_y;}

	mutable TF2* fTF2;

protected:
	mutable double cached_y;

};

#endif /* TSFUNCTION2D_H_ */
