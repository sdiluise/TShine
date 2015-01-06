/*
 * TSMinimizationFunction.h
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 *
 */

#ifndef TSMINIMIZATIONFUNCTION_H_
#define TSMINIMIZATIONFUNCTION_H_


#include <iostream>


#include <TString.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraph2D.h>

#include "TSNamed.h"

#include "TSArgList.h"
#include "TSParamList.h"

#include "TSDataBin.h"
#include "TSFuncModel.h"


using std::cout;
using std::endl;


//#include "Minuit2/Minuit2Minimizer.h"
//#include "Math/Functor.h"

/*
 *
 */
class TSMinimizationFunction:  public ROOT::Math::IMultiGenFunction {



public:

	TSMinimizationFunction();
	TSMinimizationFunction(TString name, TString title);
	TSMinimizationFunction(const TSMinimizationFunction &);
	virtual ~TSMinimizationFunction();

	ROOT::Math::IMultiGenFunction * Clone() const {
	      return new TSMinimizationFunction();
	}

	//
	//To be implemented in the derived class
	//
	virtual double Compute() const;

	int GetDataDim() const {return fNDimData;}
	TObject* GetDataHistogram() const {return fDataHisto;}
	TTree* GetHistoryTree() const {return fHistory;}
	int GetNumOfCalls() const {return fNCalls;}
	const TSFuncModel* GetModel() const {return fModel;}

	const TSVariable* GetVariableX() const {return fVariableX;}
	const TSVariable* GetVariableY() const {return fVariableY;}

	bool HasModel() const {return fHasModel;}
	bool HasData() const {return fHasData;}

	void Init();

	virtual unsigned int NDim() const {return fNDim;}
	virtual unsigned int NPar() const {return fNPars;}

	void ResetNumOfCalls() const {fNCalls=0;}

	void SaveHistory(bool b);
	bool SaveHistoryIsSet() const {return fSaveHistory;}
	int SetBinnedData(const TSDataBin &data,const TSArgList& vars);
	int SetBinnedData(TObject &histo_data);
	int SetModel(const TSFuncModel &model);

	TGraph* ScanParameter(TSParameter&,int N, double min, double max);
	TGraph2D* ScanParameters(TSParameter&,TSParameter &,int Nx, double xmin, double xmax, int Ny, double ymin ,double ymax);
	void ScanParameters3D(const TSParamList &,int,double,double,int,double,double,int,double,double);
	void SetNormalizedScannedMin(double m=1);
	void ResetNormalizedScannedMin() {fIsScannedMinNorm=false;}

	void GetScannedMinima(double& func_min, double* pars);


	//== For now set implement locally methods of TSNamed to avoid conflicts with
	//== the inheritance from TObject
	void SetName(TString name) {fName=name;}
	void SetTitle(TString title) {fTitle=title;}
	TString Name() const {return fName;}
	TString Title() const {return fTitle;}
	const char* GetName() const {return fName.Data() ;}
	const char* GetTitle() const {return fTitle.Data() ;}
	//==
	int SetNumOfDim(int v) {fNDim=v;}


	double EvalFunctor(const double *x) const;

protected:

	void SetHasModel(bool b){ fHasModel=b;}
	void SetHasData(bool b){ fHasData=b;}

	//int SetNumOfDim(int v) {fNDim=v;}
	int SetNumOfDataDim(int v) {fNDimData=v;}
	int SetNumOfPar(int v) {fNPars=v;}

	void SetVariableX(const TSVariable &);
	void SetVariableY(const TSVariable &);


private:

	//
	// function called by the ROOT::Math:: base class, evaluates
	// the function to be minimized for the set of params x
	//
	inline double DoEval (const double *x) const;


	TObject *fDataHisto;
	const TSDataBin *fDataBin;
	const TSFuncModel*fModel;

	bool fGetModelVariables();
	bool fGetModelParams();

	void fBuildHistoryTree();
	void fInitHistoryTree();

	const TSVariable* fVariableX;
	const TSVariable* fVariableY;

	int fNDim;
	int fNDimData;
	int fNPars;
	bool fHasModel;
	bool fHasData;

	TString fName;
	TString fTitle;


	//
	// Cache values for performance improvement
	//
	static const int NMAX_MINIMIZER_PARAMS=100;
    TSParameter *fCacheParamList[NMAX_MINIMIZER_PARAMS];
	mutable double fCacheParamValue[NMAX_MINIMIZER_PARAMS];
	mutable double fCacheParamMin[NMAX_MINIMIZER_PARAMS];
	mutable double fCacheParamMax[NMAX_MINIMIZER_PARAMS];
	mutable double fCacheParamStep[NMAX_MINIMIZER_PARAMS];
	mutable bool fCacheParamIsFix[NMAX_MINIMIZER_PARAMS];

	mutable int fNCalls;
	mutable double fValue;

	bool fSaveHistory;

	TTree *fHistory;

	double fScannedFunctionMin;
	std::vector<double> fScannedMins;

	bool fIsScannedMinNorm;
	double fScannedMinNorm;

};

#endif /* TSMINIMIZATIONFUNCTION_H_ */
