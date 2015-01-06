/*
 * TSBaseFunction.h
 *
 *  Created on: Aug 3, 2014
 *      Author: Silvestro di Luise
 *      		Silvestro.Di.Luise@cern.ch
 *
 *  ABC
 *
 *
 *  BaseFunction +
 *    DIM=1
 *    fX variable and cache
 *    Eval method ?
 *    TGraph (1D) builder
 *
 *
 */

#ifndef TSBASEFUNCTION_H_
#define TSBASEFUNCTION_H_

#include <TObject.h>
#include <TString.h>
#include <TF1.h>

#include <TH2.h>
#include <TH1.h>


#include "Math/IFunction.h"
#include "Math/IParamFunction.h"

#include "TSNamed.h"
#include "TSParameter.h"
#include "TSParamFunction.h"

#include "TSVariable.h"

#include "TSArgList.h"



//
//
//
//
class TSBaseFunction: public TSNamed {


public:

	TSBaseFunction();
	TSBaseFunction(TString name, TString title);
	virtual ~TSBaseFunction();


	const TSParameter* GetParameter(int) const;
	const TSParameter* GetParameter(TString name) const;

	void PrintParamList() const;

	virtual double DoEvalPar(const double *x, const double *p) const {return 0;};
	virtual const double* Parameters() const {return 0;}
	virtual void SetParameters(const double *p) {}


	virtual unsigned int NDim() const {return fNDim;}
	virtual unsigned int NPar() const {return fNPar;}

	virtual void SetNDim(int);
	void SetNPar(int);

    virtual double Evaluate() const =0;
	virtual double Eval(double x, double y=0) const =0;
	virtual double Eval() const =0;

	double GetBinArea() const {return fBinArea;}
	double GetBinWidthX() const {return fBinWidthX;}
	double GetBinWidthY() const {return fBinWidthY;}

	void Init();

	//TString Name() const {return fName;}
	//TString Title() const {return fTitle;}
	// Overrides virtual TObject method to make the derived object
	// searchable in the root containers.
	//const char* GetName() const {return fName.Data();}
	//const char* GetTitle() const {return fTitle.Data();}


	TSArgList* GetPrimaryParamList() const { return fPrimaryParamList; }
	void PrintPrimaryParamList() const;
	int GetNumOfPrimPars() const {return fPrimaryParamList->GetSize();}

	void ResetBinWidths() const;
	void SetBinWidths(double,double) const;
	void SetBinWidths(const TH1 &) const;
	void SetBinWidthX(double) const;
	void SetBinWidthY(double) const;


	static double GNorm;//=2.50662827463100024; //sqrt(2*Pi)=2.50662827463100024

protected:

	int AddParameter(const TSParameter& p);

	virtual void fClearPrimaryParamList();

private:

	//TString fName,fTitle;

	int fNDim;
	int fNPar;

	mutable double fBinWidthX;
	mutable double fBinWidthY;
	mutable double fBinArea;

	TSArgList fParamSet;

	TSArgList *fPrimaryParamList;

	virtual void fBuildPrimaryParamList();
	virtual void fUpdatePrimaryParamList(const TSParameter &);

	//plain array for O(1) access to params..if needed
	//can be accessed with the method which retrieve the
	//param via param number
	static const int NMAX_FUNC_PARAMS=100;
	const TSParameter *fParamArray[NMAX_FUNC_PARAMS];
	const TSParameter *fPrimaryParamArray[NMAX_FUNC_PARAMS];


};

#endif /* TSBASEFUNCTION_H_ */
