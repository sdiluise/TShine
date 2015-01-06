/*
 *
 *
 * TSParamFunction.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#ifndef TSPARAMFUNCTION_H_
#define TSPARAMFUNCTION_H_

#include <map>

#include <TString.h>
#include <TF1.h>


#include "TSArgList.h"

#include "TSParameter.h"


class TSParamFunction: public TSParameter {

public:

	TSParamFunction();
	TSParamFunction(TString name, TString label, TString title, TString unit="");
	TSParamFunction(const TSParamFunction &);
	virtual ~TSParamFunction();

	void Copy(const TSParamFunction &);
	virtual TSParamFunction* Clone(TString name="") const;

	virtual double Evaluate() const;
	TString GetDescription() const {return fDescription;}
	double GetError() const;
	double GetError(double *cov) const;
	TF1* GetFunction() const {return fFunction;}
	TString GetFormula() const {return fExpr;}
	TString GetTF1Formula() const {return fTF1expr;}
    int GetNumOfPars() const {return fNumOfPars;}
    TSParameter* GetParam(int) const;
    virtual double GetValue() const;
    double GetValue2();
    void Init();
    void Print() const;
    bool SetFunction(TString expr);
	bool SetFunction(TString expr,const TSArgList &);
	bool SetFunction(TString expr,TSParameter&);
	bool SetFunction(TString expr,TSParameter&, TSParameter&);
	bool SetFunction(TString expr,TSParameter&, TSParameter&,TSParameter&);
	bool SetFunction(TString expr,TSParameter&, TSParameter&,TSParameter&, TSParameter&);

	bool SetParameter(TSParameter&);


private:
  //-- Disable external access to methods Not Allowed on ParamFunction --
  // Setter still need for internal used ex. when cloning the object
  void SetValue(double);
  void SetError(double);
  void SetAsymErrors(double err_low,double err_up);
  //

private:

  void fBuildDescription();

  void fBuildTF1();

  void fCacheParam(int) const;
  void fCacheParams() const;

  int fNumOfPars;
  mutable double fValue;

  TF1 *fFunction;
  TString fExpr;
  TString fTF1expr;

  void fClearParamList();
  std::map<int,TSParameter*> fParamList;

  void fUpdatePrimaryParamList(const TSParameter &);
  virtual void fBuildPrimaryParamList();



  TString fDescription;

  static const int NMAX_CACHED_PARS=10;
  mutable double fCachedParams[NMAX_CACHED_PARS];

};

#endif /* TSPARAMFUNCTION_H_ */
