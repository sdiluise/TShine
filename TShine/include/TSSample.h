/*
 * TSSample.h
 *
 *  Created on: Sep 26, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *  Generic Data Container
 *
 *  Normalization
 *
 *  CategoryCard
 *
 *  Data Variable
 *
 *  DataStructure:
 *     Mulit-dim
 *     Binned and/or Unbinned
 *
 */

#ifndef TSSAMPLE_H_
#define TSSAMPLE_H_

#include <utility>
#include <vector>
#include <map>
#include <unordered_map>

#include <TRandom.h>
#include <TString.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THStack.h>
#include <TTree.h>
#include <TAxis.h>
#include <TList.h>

#include "TSNamed.h"
#include "TSPOTMgr.h"

#include "TSArgList.h"
#include "TSParamList.h"

#include "TSVariable.h"


#include "TSCategory.h"
#include "TSCategoryCard.h"

#include "TSNuiParam.h"
#include "TSNuiParamSet.h"
#include "TSRespFunc.h"
#include "TSHistoNuiParamMgr.h"
#include "TSHistoOscillMgr.h"

/*
 *
 */

class TSSample: public TSNamed {

public:

	TSSample();
	TSSample(TString name, TString label, TString title);
	virtual ~TSSample();

	void AddVariable(const TSVariable &, int n, double min, double max, TString opt="");
	void AddVariable(const TSVariable &, int n, double *bins, TString opt="");

	void AddVariables(const TSArgList &);//not active

	int CompareCategoryCard(const TSCategoryCard &, int opt=1);

	void Disable1D(bool b) {fDisable1D=b;}
	void Disable2D(bool b) {fDisable2D=b;}


	void Fill(double w=1);

	void Fill(std::map<const TSVariable*,double>&, double w=1);

	void Fill(TTree &, TCut &, TString vars, TString weight="");

	void Fill(int, double x, double w);
	void Fill(int, double x, double y, double w);


	const TSCategoryCard& GetCategoryCard() const {return fCard;}

	TTree& GetDataTree() {return fDataTree;}
	TList& GetHistoList() {return hList;}

	TH1F* GetHistogram(const TSVariable &);
	TH1F* GetToyHistogram(const TSVariable &);
	TH2F* GetHistogram2D(const TSVariable &, const TSVariable &);
	TH2F* GetToyHistogram2D(const TSVariable &, const TSVariable &);

	int GetVarIdx(const TSVariable &) const;

	//const TSHistoNuiParamMgr& GetHistoNuiParamMgr() const {return fNuiParamMgr;}

	float GetPOT() const {return fPOT;}
	float GetNativePOT() const {return fPOTnative;}
	float GetNativePOTPower() const {return fPOTnative_pow;}
	float GetPOTPower() const {return fPOTpower;}
	float GetPOTScaling() const {return fPOTscaling;}
	TString GetPOTLabel() const {return fPOTlabel;}
	TString GetNativePOTLabel() const {return fPOTNativeLabel;}
	int GetToyId() const {return fToyId;}

	bool HasCategoryCard() const {return fHasCategory;}
	bool IsUnbinned() const {return fIsUnbinned;}

	void Init();

	void Print();

	void Randomize(int =0);
	THStack* RandomizedStack(const TSVariable &var, int ntoy=1000);
	void Restore();

	void Reset();
	void ResetToys();

	void SetCategoryCard(const TSCategoryCard &);

	void SetNativePOT(float POT, float Power=20);
	void ScaleToPOT(float POT, float Power=20);
	void ScaleToNativePOT();


	void SetBidimensional(const TSVariable &var_x, const TSVariable &var_y);

	int SetOscillProbFunction(TSOscProb *);

	void SetUnbinned(bool b){ fIsUnbinned = b; }

	int SetWeightParams(const TSNuiParamSet &);



private:

	TRandom fRand;

	TTree fDataTree;
	bool fIsUnbinned;
	void fInitDataTree();
	void fUpdateDataTreeBranches();

	void fBuildPOTNames();

	void fNormalizeAll(int opt=0);

	float fPOT;
	float fPOTnative;
	float fPOTpower;
	float fPOTnative_pow;
	float fPOTscaling;

	TString fPOTlabel;
	TString fPOTNativeLabel;

	//
	bool fHasCategory;
	TSCategoryCard fCard;

	//

	std::map<const TSVariable*, int> fVarIdx;

	//
	//  Try also with plain C array
	//  for performance optimization
	//
	int fNumOfVars;
	int fNumOfAuxVars;

	std::vector<const TSVariable*> fVarListAll;
	std::vector<const TSVariable*> fVarList;
	std::vector<const TSVariable*> fAuxVarList;

	std::vector<TAxis*> fVarAxis;
	std::vector<TAxis*> fAuxVarAxis;

	std::vector<int> fVarType;

	std::vector<TH1F*> fVarHisto;
	std::vector<TH2F*> fVarHisto2D;

	std::vector<TH1F*> fVarHistoToy;
	std::vector<TH2F*> fVarHisto2DToy;

	std::vector<TH1F*> fVarHistoAux;
	std::vector<TH2F*> fVarHisto2DAux;

	std::vector<std::pair<int,int> > fHisto2DIdx;
	std::vector<std::pair<int,int> > fHisto2DAuxIdx;

	std::vector<TSHistoNuiParamMgr*> fHistoNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHistoOscillProbMgr;
	std::vector<TSHistoNuiParamMgr*> fHistoToyNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHistoToyOscillProbMgr;

	std::vector<TSHistoNuiParamMgr*> fHisto2DNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHisto2DOscillProbMgr;
	std::vector<TSHistoNuiParamMgr*> fHisto2DToyNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHisto2DToyOscillProbMgr;

	bool fDisable1D;
	bool fDisable2D;


	TList hList;
	TList hListToy;
	TList hListAux;
	TList hListAll;

	TSVariable *fVars;
	double **fVarBins;

	struct fVarSettings{

		int nbins;
		double *bins;
		TAxis *axis;
		bool IsAux;

		fVarSettings(): bins(0),axis(0),IsAux(0) {};

	};

	//----

	double *fVarCache;
	std::vector<TH1F*> fHistoList;
	std::vector<TH2F*> fHistoList2D;
	double fEventWeight;
	int fToyId;

	//
	//-----
	//


	void fFillFromCache();

};

#endif /* TSSAMPLE_H_ */
