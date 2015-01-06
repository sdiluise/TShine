/*
 * TSOscProb3Plus1.h
 *
 *  Created on: Oct 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSOSCPROB3PLUS1_H_
#define TSOSCPROB3PLUS1_H_

#include "TSParamList.h"

#include "TSOscProb.h"

/*
 *
 *
 *
 */

class TSOscProb3Plus1: public TSOscProb {


public:

	TSOscProb3Plus1();
	TSOscProb3Plus1(TString name, TString title);

	virtual ~TSOscProb3Plus1();

	double ComputeSurvProbNuE() const;
	double ComputeNuSApp() const;
	double ComputeSurvProbNuMu() const;
	double ComputeNuMuToNuE() const;


	void DefineTransition(int,TString,TString);
	double Eval();

	void FillParameter(int);
	void FillParameters();

	TString GetTransitionLabel();

	void Init();

	void SetParameters(const TSParamList &);
	void SetParameters(TSParameter &sin2_2theta_ee, TSParameter &sin2_2theta_mumu, TSParameter &dm2_14);

	void SetTransition(TString);
	void SetTransition(int);


private:

	double mDM2_14;
	double mSin2_2Theta_ee;
	double mSin2_2Theta_mumu;

	int fSurvProbNuE,fNuSApp,fSurvProbNuMu,fNuMuToNuE;

	std::vector<int> vTrsId;
	std::vector<TString> vTrsName;
	std::vector<TString> vTrsLabel;

};

#endif /* TSOSCPROB3PLUS1_H_ */
