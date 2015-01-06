/*
 * TSFitResults.h
 *
 *  Created on: Aug 22, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFITRESULTS_H_
#define TSFITRESULTS_H_

#include <vector>
#include <utility>
#include <map>

#include <TString.h>
#include <TList.h>

#include <TGraph.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>

#include <TVectorD.h>
#include <TMatrixD.h>

#include "TSArgList.h"
#include "TSParamList.h"


#include <TString.h>

#include "TSNamed.h"
#include "MessageMgr.h"

#include "TSMinimizer.h"



/*
 *
 */

class TSFitResults: public TSNamed {


public:

	TSFitResults();
	TSFitResults(TString name, TString title, TString tag="");
	virtual ~TSFitResults();

	void ClearContourGraphs();

	int GetIteration() const {return fIteration;}
	int GetNumOfCalls() const {return fNumOfCalls;}
	int GetNumOfPars() const {return fNumOfPars;}
	int GetNumOfFreePars() const {return fNumOfFreePars;}
	double GetMinValue() const {return fMinValue;}
	int GetStatusCode() const {return fStatusCode;}
	TString GetTag() const {return fTag;}
	float GetCPUTime() const {return fCPUTime;}
	float GetRealTime() const {return fRealTime;}

	TList* GetListOfHistos() {return &hList;}
	TObject* GetHisto(TString tag) const;

	bool HasTag() const {return fHasTag;}

	void ImportMinimizationResults(const TSMinimizer &);

	void Init();

	void SetCPUTime(float t) {fCPUTime=t;}
	void SetIteration(int i) {fIteration=i;}
	void SetListOfTrueParams(const TSParamList &);
	void SetListOfInputParams(const TSParamList &);
	void SetListOfFitParams(const TSParamList &);

	void SetRealTime(float t) {fRealTime=t;}
	void SetTag(TString);

	void WriteHistos();

private:

	TString fTag;
	bool fHasTag;
	int fNumOfCalls;
	int	fNumOfPars;
	int fNumOfFreePars;
	double	fMinValue;
	int	fStatusCode;
	int fIteration;
	float fCPUTime;
	float fRealTime;

	TSParamList fInputParamList;
	TSParamList fTrueParamList;
	TSParamList fFitParamList;


	//
	// Vectors/Matrices
	//
	TMatrixD fCovMatrix;
	TMatrixD fCorrMatrix;
	TMatrixD fHesseMatrix;

	TVectorD fGCC;
	TVectorD fTrueValues;
	TVectorD fFitValues;
	TVectorD fErrors;
	TVectorD fErrorFrac;
	TVectorD fResid;
	TVectorD fPull;
	TVectorD fErrLow;
	TVectorD fErrUp;

	//
	// Histograms
	//

	TList hList;

	TH2F hCovMatrix;
	TH2F hCorrMatrix;
	TH2F hHesseMatrix;

	TH1F hGCC;
	TH1F hError;
	TH1F hErrLow;
	TH1F hErrUp;
	TH1F hErrorFrac;
	TH1F hLimitLow;
	TH1F hLimitUp;

	TH1F hTrueVal;
	TH1F hInputVal;
	TH1F hFitVal;
	TH1F hResid;
	TH1F hPull;

	TString hBaseName;
	TString kCovMatrix;
	TString kCorrMatrix;
	TString kHesseMatrix;

	TString kGCC;
	TString kError;
	TString kErrLow;
	TString kErrUp;
	TString kErrorFrac;
	TString kLimitLow;
	TString kLimitUp;

	TString kTrueValue;
	TString kInputValue;
	TString kFitValue;
	TString kResid;
	TString kPull;

	//
	//

	std::map<std::pair<TString,TString>, TGraph* > fContourGraphList;
	std::vector< std::pair<TString,TString> > fContourParamPairs;

	std::map<TString, TGraph* > fScanGraphList;

	void fBuildHistos();
	void fFillPulls();
	void fImportHisto1D(TH1F* h, TH1F* hnew);
	void fImportHisto2D(TH2F* h, TH2F* hnew);


	TTree *fHistoryTree;
};

#endif /* TSFITRESULTS_H_ */
