/*
 * TSEventDataSet.h
 *
 *  Created on: Oct 26, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSEVENTDATASET_H_
#define TSEVENTDATASET_H_

#include <map>
#include <unordered_map>

#include <TObjArray.h>

#include "TSNamed.h"

#include "TSVariable.h"
#include "TSSample.h"
#include "TSEvent.h"

#include "TSParamList.h"
#include "TSNuiParamSet.h"

#include "MessageMgr.h"

/*
 *
 */
class TSEventDataSet: public TSNamed {

public:
	TSEventDataSet();
	TSEventDataSet(TString name, TString title);

	virtual ~TSEventDataSet();

	void Add(TSEventDataSet &);
	void AddEvent(TSEvent *);

	void AddNuiParams(TSNuiParamSet &);

	void BuildCategoryCardHisto();
	void BuildCategoryCardMatrix();

	void Clear();
	void ClearCategoryCardHisto();

	double ComputeEffectiveNumOfEvts();

	void ComputeWeight();

	void FillHistogram(TH1& ,int);
	void FillHistogram(TH1& ,const TSVariable &);
	void FillHistogram(TH2& ,const TSVariable &, const TSVariable &);

	void FillHistogramWithSelection(TH1& ,const TSVariable &, const TSCategoryCard &);

	void FillHistograms();
	void FillHistogramsNoWeight();

	TSEvent* FindEventId(int);
	TH1F& GetCategoryCardHisto() {return fCategCardHisto;}
	TH2F& GetCategoryCardMatrix() {return fCategCardMatrix;}

	double GetEffectiveNumOfEvts() const {return fEffNumOfEvts;}

	int GetEntries() const {return fNumOfEvents;}
	TSEvent* GetEvent(int);
	TH1F& GetHistogram(const TSVariable &);
	TH2F& GetHistogram2D(const TSVariable &, const TSVariable &);

	virtual TH1* GetHistogram();

	TSNuiParamSet& GetNuiParamSet() {return fNuiParam_Set;}
	TSParamList& GetParamSet() {return fParamList;}
	TSParamList& GetParamSetPoI() {return fParamList_PoI;}
	TSParamList& GetParamSetNui() {return fParamList_Nui;}


	int GetNumOfEvents() const {return fNumOfEvents;}

	TSSample& GetSample() {return fSample;}

	int GetVarIdx(TSVariable &);

	bool HasProcessComputeWeight() const {return fProcComputeWeight;}
	bool HasProcessOscillate() const {return fProcOscillate;}

	bool HasListOfPoI() const {return fHasListOfPoI;}
	bool HasListOfParams() const {return fHasListOfParams;}
	bool HasWeightParams() const {return fHasWeightParams;}


	void Init();

	virtual void Process();

	void ProcessEventsWith(double (TSEvent::*method)(int) );

	void Oscillate();

	void ResetHistograms();

	void ReWeightWithSavedPosition(int weight_id=-1);

	void SelectHistogram(const TSVariable &);
	void SelectHistogram(const TSVariable &,const TSVariable &);

	void SetHistogram(const TSVariable &, int, double, double);
	void SetHistogram(const TSVariable &, int, double *);
	void SetHistogram(const TSVariable &, const TSVariable &, int, double *, int, double *);
	void SetHistogram(const TSVariable &, const TSVariable &, int, double, double, int, double, double);
	void SetHistogram(const TSVariable &, const TSVariable &, int, double , double, int, double *);
	void SetHistogram(const TSVariable &, const TSVariable &, int, double *, int, double, double);

	void SetListOfPoI(const TSParamList&);

	void SetProcessComputeWeight(bool b) {fProcComputeWeight=b;}
	void SetProcessOscillate(bool b) {fProcOscillate=b;}

	void SetWeightPosition(int weight_id);

	void UpdateParamList();

private:

	int fNumOfEvents;
	double fEffNumOfEvts;

	TObjArray fEventArray;

	TSSample fSample;

	std::map<int,int> fVarMatch;
	std::map<const TSVariable*,const TSVariable*> fVarMatch2D;


	TH1F fCategCardHisto;
	TH2F fCategCardMatrix;

	bool fHasWeightParams;
	std::unordered_map<TSNuiParam*,int> fNuiParam_map;

	bool fHasListOfPoI;
	bool fHasListOfParams;

	TSNuiParamSet fNuiParam_Set;

	TSParamList fParamList;
	TSParamList fParamList_Nui;
	TSParamList fParamList_PoI;

	TH1* fSelectedHistogram;
	int fSelectedHistoDim;
	const TSVariable *fSelectedHistoVar[2];

	int fSavedWeightPosition;
	bool fProcComputeWeight;
	bool fProcOscillate;

};

#endif /* TSEVENTDATASET_H_ */
