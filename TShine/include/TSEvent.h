/*
 * TSEvent.h
 *
 *  Created on: Sep 27, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSEVENT_H_
#define TSEVENT_H_

#include <vector>
#include <map>
#include <unordered_map>

#include <TString.h>
#include <TH1F.h>
#include <TH2F.h>

#include "TSNamed.h"

#include "TSPOTMgr.h"

#include "TSVariable.h"
#include "TSCategory.h"
#include "TSCategoryCard.h"
#include "TSNuiParam.h"
#include "TSNuiParamSet.h"

#include "TSOscProb.h"
#include "TSOscProb3Plus1.h"

/*
 *
 *
 *
 */

class TSEvent;


class TSEventCommands{

public:



};

class TSEvent: public TSNamed {


public:

	TSEvent();
	TSEvent(TString name, TString tag, TString title);
	TSEvent(const TSEvent &);
	virtual ~TSEvent();

	void AddResponseFunctionToParam(const TSRespFunc &, const TSNuiParam &);


	static void ClearProcessChain();

	double ComputeOscillProb();

	double ComputeWeight();


	void Fill();

	std::map<const TSVariable*,double>& FillValues();

	TSCategoryCard& GetCategoryCard();



	int GetId() const {return fId;}

	int GetNumOfVars() const {return fNumOfVars;}

	TSNuiParamSet& GetNuiParamSet() {return fNuiParSet;}

	double GetOscillProb() const {return fOscillProb;}

	TSOscProb* GetOscillProbFunction() {return fProbFunction;}

	TSPOTMgr& GetPOTMgr() {return fPOTMgr;}

	TString GetTag() const {return GetLabel();}

	const TSVariable* GetVariable(int) const;
	const TSVariable* GetVarLink(int) const;

	double GetVarValue(int) const;
	double GetVarValue(const TSVariable &);

	double GetWeight() const {return fWeight;}
	double GetWeightNative() const {return fWeightNative;}

	double GetWeightPosition(int position);
	std::vector<double>& GetSavedWeights() {return fWeightSaved;}

	bool HasVariable(const TSVariable &) const;

	bool HasCategoryCard() const {return fHasCategory;}
	bool HasOscillProbFunction() const {return fHasProbFunc;}
	bool HasWeightParams() const {return fHasWeightParams;}


	void Init();

	double Oscillate(int opt=1);

	virtual void Process();
	void ProcessWith(double (TSEvent::*p)() ) { (this->*p)(); }

	void Print() const;

	double RestoreWeightNative();

	double ReWeightWith(double);
	double ReWeightWithSavedPosition(int postion);
	double ReWeightNativeWith(double);

	double ReWeightWithNuiParams(int opt=0);


	double SaveWeightPosition(int position);

	void SetCategoryCard(const TSCategoryCard &);

	void SetId(int i) {fId=i;}

	void SetNativePOT(float POT, float Power=20);
	void ScaleToPOT(float POT, float Power=20);
	void ScaleToNativePOT();


	int SetOscillProbFunction(TSOscProb *);

	void SetTag(TString tag) { SetLabel(tag); }

	void SetVariables(const TSArgList &);

	void SetVarValue(int, double);

	void SetWeightNative(double w);
	void SetWeight(double w);

	int SetWeightParams(const TSNuiParamSet &);


private:

	bool fHasCategory;
	bool fHasProbFunc;
    bool fHasWeightParams;

	TSCategoryCard fCard;

	int fId;
	double fWeight;
	double fWeightNative;

	std::vector<double> fWeightSaved;

    TSPOTMgr fPOTMgr;

	int fNumOfVars;

	//std::map<TString,const TSVariable*> fVarPtr;
	//std::map<TString,TSVariable *> fVarList;

	std::map<int,const TSVariable*> fVarPtr;
	std::map<int,TSVariable *> fVarList;

	//std::map<int,TSVariable *>::const_iterator fVarListItr;

	std::map<const TSVariable*,double> fMapOfValues;

	TSNuiParamSet fNuiParSet;

	TSOscProb *fProbFunction;
	int fIdxEne;
	int fIdxLen;
	double fOscillProb;


	//static members
	static int fProcChainN;
};

#endif /* TSEVENT_H_ */
