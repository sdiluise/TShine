/*
 * TSDataBin.h
 *
 *  Created on: Jun 3, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 *
 */

#ifndef TSDATABIN_H_
#define TSDATABIN_H_

#include <map>

#include <TString.h>
#include <TTree.h>

#include "TSVariable.h"
#include "TSPhaseSpaceVolume.h"
#include "TSHistogramFld.h"

#include "TSArgList.h"

#include "MessageMgr.h"


class VarSettings;


class TSDataBin: public TSNamed {

public:

	TSDataBin();
	TSDataBin(TString name, TString label, TString title);
	TSDataBin(const TSDataBin&);
	virtual ~TSDataBin();

	int Fill();
	int Fill(const TSVariable &);
	int Fill(const TSArgList &);
	TTree* GetDataTree() {return fDataTree;}
	TSHistogramFld* GetHistoFolder() const {return fHistoFld;}
	int GetHistoTitleStyle() const {return fHistoTitleStyle;}
	int GetNumOfVars() const {return fNumOfVars;}
	TSPhaseSpaceVolume* GetPhaseSpaceVolume() const {return fPhaseSpaceVol;}
	const TSVariable* GetVariable(int) const;
	int GetVarSettings(int var, int &auto_type, int &nbins, double &min, double &max) const;
	bool HasPhaseSpaceVolume() const {return fHasPhaseSpaceVol;}
	void Init();
	bool IsPhaseSpace(int) const;
	bool IsUnbinned() const {return fIsUnbinned;}
	int SetBidimensional(TSVariable&, TSVariable&);
	int SetHistoTitleStyle(int);
	void SetName(TString);
	int SetPhaseSpaceVolume(TSPhaseSpaceVolume&);
    void SetUnbinned(bool);
	int SetVariable(const TSVariable&);
	int SetVariable(const TSVariable&, double range_min, double range_max);
	int SetVariable(const TSVariable&, int n, double low, double up);


private:

	TString fBuildEntriesTitle(float =0, TString="");
	TString fBuildHistoName(int,int=0);
	TString fBuildHistoNameBase();
	TString fBuildHistoTitle(int,int=0);
	TString fBuildAxisTitle(int);

	void fAddTreeVariable(int);
	void fBuildHistogram1D(int ivar, int n=-1, double low=0, double up=0);
	void fBuildHistogram2D(int varx, int vary);
	void fInitDataTree();
	void fInitHistoFld();
	void fUpdateHistoTitles();
	int  fSetVariable(const TSVariable&, int n, double low, double up, int type);


	TSHistogramFld *fHistoFld;
	TSPhaseSpaceVolume *fPhaseSpaceVol;
	TTree *fDataTree;


	class VarSettings{
	public:
			int NumOfBins;
			double Min;
			double Max;
			bool IsBinningAuto;
			bool HasAutoRangeLimits;
			double RangeLimLow;
			double RangeLimUp;
			bool IsPhaseSpace;

			VarSettings():NumOfBins(0),IsBinningAuto(0),HasAutoRangeLimits(0)
						,IsPhaseSpace(0)
						,Min(0),Max(0),RangeLimLow(0),RangeLimUp(0)
						{}
	};

	std::map<int,const TSVariable*> fVarList;
	std::map<int,VarSettings*> fVarSetsList;


	static const int DATA_BUFF_SIZE=50;
	float fDataBuffer[DATA_BUFF_SIZE];
	int fDataBufferWeight[DATA_BUFF_SIZE];
	bool fHasPhaseSpaceVol;
	bool fIsUnbinned;
	int fNumOfVars;

	int fHistoTitleStyle;
	TString fHistoNameBase0;


	int fPSHistoNBinsX;

};

#endif /* TSDATABIN_H_ */
