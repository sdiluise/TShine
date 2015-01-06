/*
 * TSOscProb.h
 *
 *  Created on: Oct 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSOSCPROB_H_
#define TSOSCPROB_H_

#include <vector>

#include <TGraph.h>

#include "TSNamed.h"

#include "TSParameter.h"
#include "TSParamList.h"
#include "TSVariable.h"

/*
 *
 */

class TSOscProb: public TSNamed {


public:

	TSOscProb();
	TSOscProb(TString name, TString title);
	virtual ~TSOscProb();

	void BindToEnergyVariable(TSVariable &);
	void BindToLengthVariable(TSVariable &);


	TGraph* BuildGraph_ProbVsE(int npts, double min, double max);
	TGraph* BuildGraph_ProbVsL(int npts, double min, double max);
	TGraph* BuildGraph_ProbVsLoverE(int npts, double min, double max);
	TGraph* BuildGraph_ProbVsParam(TSParameter &, int npts, double min, double max);


	virtual double Eval() =0;

	void FillHistogram(TH1F&,TString opt="");

	virtual void FillParameter(int);
	virtual void FillParameters();

	virtual TString GetTransitionLabel() {return "";}

	TSVariable *GetEnergyVariableBound() {return fBindVarEne;}
	TSVariable *GetLengthVariableBound() {return fBindVarLen;}

	void Init();

	bool IsXbinningLog() const {return fIsXbinningLog;}

	void OscillateHistogram(TH1 &, TH1 *, TString opt="");


	void Print();

	void SetE(double E) const {mE = E;}
	void SetL(double L) const {mL = L;}
	void SetEandL(double E, double L) const {mE=E;mL=L;}

	void SetEnergyUnit(TString ="MeV");
	void SetLengthUnit(TString ="km");

	void SetBinAveNumOfPoints(int n) {fNptBinAve=n;}

	virtual void SetParameters(const TSParamList& );

	void SetXbinningLog(bool b) {fIsXbinningLog = b;}

protected:

	TSVariable fVarEne;
	TSVariable fVarLen;

	TSVariable *fBindVarEne;
	TSVariable *fBindVarLen;


	double mArgBase;
	double mArgConst;
	double mArgEScale;
	double mArgLScale;

	mutable double mE;
	mutable double mL;

	bool fIsXbinningLog;

	int fTransitionId;
	TString fTranstionName;
	TString fTransitionLabel;

	int fNumOfParams;

	TSParamList fParamList;
	std::vector<TSParameter*> fArrayOfPars;
	double *fParamCache;

	int fNptBinAve;
};

#endif /* TSOSCPROB_H_ */
