/*
 * TSNuiParamSet.h
 *
 *  Created on: Sep 29, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSNUIPARAMSET_H_
#define TSNUIPARAMSET_H_

#include <vector>
#include <map>

#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>

#include "TSNuiParam.h"

#include "TSParamList.h"
#include "TSArgList.h"

#include "TSRespFunc.h"

/*
 *
 */

class TSNuiParamSet;

typedef TSNuiParamSet TSNuiParamList;


class TSNuiParamSet: public TSParamList {

public:

	TSNuiParamSet();
	virtual ~TSNuiParamSet();

	void AddResponseFunctionToPar(const TSRespFunc &, const TSNuiParam &);
	void AddResponseFunctionToPar(const TSRespFunc &, int);

	void BuildCategoryCardHisto(int opt=0);

	double ComputeWeight(int opt=1);
	double ComputeWeightFromPrior();

	bool HasParameter(const TSNuiParam &) const;

	bool HasResponseFunctions() const {return fHasRespFuncs;}

	TH2F& GetCategoryCardHisto() {return fCategCardHisto;}
	TH1F& GetHistoOfParams(TString opt="");
	TH1F& GetHistoOfPriors(TString opt="");

	TSRespFunc* GetParamRespFunc(const TSNuiParam &) const;
	TSRespFunc* GetParamRespFunc(int) const;

	int GetParamIdx(const TSNuiParam &) const;

	double GetWeight() const {return fWeight;}

	void Init();

//moved in TSArgList
//	int LoopStart();
//	int LoopNext();
//	TObject* LoopAt();

	void Print() const;
	void PrintDifferences(TSNuiParamSet &);
	void PrintLong() const;
	void PrintNameSeq() const;
	void Randomize();
	void Restore();

	void SetWeight(double w) {fWeight=w;}

private:

	bool fHasRespFuncs;
	mutable double fWeight;

	std::map<int,TSRespFunc*> fRespFuncList;
	std::map<int,bool> fParHasRespFunc;//this not really needed

	TH1F fHistoParams;
	TH1F fHistoPriors;

	TH2F fCategCardHisto;

	TObjLink *objLink;
	bool fLoopStart;

};





#endif /* TSNUIPARAMSET_H_ */
