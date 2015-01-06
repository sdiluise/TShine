/*
 * TSParameter.h
 *
 *  Created on: May 26, 2014
 *      Author: sdiluise
 */
/**
 *
 *
 *   Created on: May 25, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *   Class to implement a general Parameter.
 *   It inherits from a Variable.
 *   A Parameter is a Variable with an Error (or Asymmetric Errors)
 *   and a State: Fixed/Free;
 *   If a range is specified and Parameter is fixed to a value outside the range
 *   than the value is fixed to the closest range limit;
 *   Other settings useful for the fit are available: e.g. Precision
 *
 */



#ifndef TSPARAMETER_H_
#define TSPARAMETER_H_

#include <string>

#include <TString.h>

#include "TSArgList.h"

#include "TSVariable.h"


class TSParameter: public TSVariable {


public:

	TSParameter();
	TSParameter(TString name, TString label, TString title, TString unit="");
	TSParameter(const TSParameter &);
	virtual ~TSParameter();


	void Clear();
	void ClearError();
	void ClearAsymErrors();
	void Copy(const TSParameter &);
	virtual TSParameter* Clone(TString name,int) const;
	virtual TObject* Clone(const char* name="") const;

	void Fix();
	virtual double GetError() const {return fError;}
	virtual double GetErrorLow() const {return fErrLow;}
	virtual double GetErrorUp() const {return fErrUp;}
	virtual double GetErrorFrac() const;
    virtual double GetPrecision() const {return fPrec;}
    virtual double GetStep() const {return fStep;}

	bool HasAsymErrors() const {return fHasAsymErrors;}
	bool HasError() const {return fHasError;}
	bool IsFixed() const {return fIsFixed;}

	void Init();

	void Print() const;
	void Release();
	void SetAsymErrors(double err_low,double err_up);
	void SetError(double err);
	void SetFixed(double);
	void SetPrecision(double);
	void SetStep(double s);

	virtual bool SetFunction(TString expr,const TSArgList &) {}

	TSArgList* GetPrimaryParamList() const;
	void PrintPrimaryParamList() const;
	int GetNumOfPrimPars() const {return fPrimaryParamList->GetSize();}

private:

	double fError;
	double fErrLow;
	double fErrUp;
	double fPrec;
	double fStep;
	bool fHasAsymErrors;
	bool fHasError;
	bool fIsFixed;


	virtual void fBuildPrimaryParamList();

	TSArgList *fPrimaryParamList;

protected:
	virtual void fClearPrimaryParamList();

	//ClassDef(TSParameter,1)
};

#endif /* TSPARAMETER_H_ */
