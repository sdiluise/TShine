/*
 * TSFunc1DModel.h
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *	Make an intermediate TSFuncModel: public TSFunc2D
 *	Set NDIM etc...
 *
 */

#ifndef TSFUNC1DMODEL_H_
#define TSFUNC1DMODEL_H_

#include <TString.h>
#include <TFormula.h>
#include <TF1.h>

#include "TSVariable.h"
#include "TSParameter.h"
#include "TSFunction1D.h"
#include "TSFunction2D.h"


#include "TSArgList.h"

/*
 *
 */

class TSFuncModel: public TSFunction2D {

public:

	TSFuncModel(TString name,TString title);
	TSFuncModel(TString name,TString title,const TSArgList& set);
	TSFuncModel(TString name,TString title,const TSArgList& set, const TSArgSet &coefs);
	TSFuncModel(TString name,TString title,TString formula,const TSArgList& set);
	TSFuncModel(TString name,TString title,TString formula,const TSArgList& set, const TSArgSet &coefs);


	virtual ~TSFuncModel();

	virtual double Eval(double x, double y=0) const =0;
	virtual double Eval() const =0;
	virtual double Evaluate() const =0;

	double GetNormalization() const {return fNormalization;}
	int GetNumOfFuncs() const {return fNFuncs;}
	int GetNumOfCoefs() const {return fNCoefs;}
	bool HasCoefs() const {return fHasCoefs;}

	void Init();

	bool IsLinear() const {return fIsLinear;}
	bool IsTFormula() const {return fIsTFormula;}

	TString GetFormula() const {return fExpr;}
	TString GetTF1Formula() const {return fTF1Formula;}

	void Print() const;

	void ResetNormalization() const;
	void SetNormalization(double integral,double binw_width_x=1,double bin_width_y=1) const;
	void NormalizeToIntegral(const TH1 &) const;
	void NormalizeToHistogram(const TH1 &) const;

protected:

	int fAddCoefficients(const TSArgSet &);
	int fAddFunctions(const TSArgSet &);

	int fAnalyseModelFormula(TString expr,const TSArgSet &funcs, const TSArgSet &coeff);

	const TSFunction1D *fFunc1D;
	const TSParameter *fParams;

	TString fExpr;
	TString fTF1Formula;

	TFormula fFormula;

	int fNFuncs;
	int fNCoefs;
	bool fHasCoefs;
	bool fIsTFormula;
	bool fIsLinear;


	static const int NMAX_FORMULA_VARS=30;

	int func2ParamIndex[NMAX_FORMULA_VARS];
	int coef2ParamIndex[NMAX_FORMULA_VARS];

	TSParameter *fCoefsList[NMAX_FORMULA_VARS];
	TSFunction1D *fFunctionsList[NMAX_FORMULA_VARS];

private:

	mutable double fNormalization;

};

#endif /* TSFUNC1DMODEL_H_ */
