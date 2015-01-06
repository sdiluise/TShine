/*
 * TSPhaseSpaceMap.h
 *
 *  Created on: May 28, 2014
 *      Author: sdiluise
 */
/**
 *
 *
 *  Author: Silvestro Di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *  TODO: Flag MapOK checking that all axis are binned
 *
 *
 *
 *
 *
 */

#ifndef TSPHASESPACEMAP_H_
#define TSPHASESPACEMAP_H_

#include <map>

#include <TString.h>

#include "TSNamed.h"
#include "TSVariable.h"
#include "TSPhaseSpaceVolume.h"
#include "TSHistogramFld.h"


class TSPhaseSpaceMap: public TSNamed {

public:

	TSPhaseSpaceMap();
	TSPhaseSpaceMap(TString name, TString title);
	TSPhaseSpaceMap(const TSPhaseSpaceMap &);
	virtual ~TSPhaseSpaceMap();

	int Add(const TSPhaseSpaceMap &);
	int AddVolume(TSPhaseSpaceVolume& );
	int BuildHistograms(int opt=0);
	TSPhaseSpaceMap* Clone(TString name, TString title);
	virtual int Fill();
	int FindVolume() const;
	int GetAxis(TString) const;
	int GetDimension() const {return fDimension;}
	TSHistogramFld* GetHistogramFld(TString opt="") const;
	int GetNumOfVolumes() const {return fNumOfVols;}
	const TSVariable* GetVariable(int) const;
	const TSVariable* GetVariable(TString) const;
	TSPhaseSpaceVolume* GetVolume(int) const;
	bool HasSelection() const {return fHasSelection;}
	virtual void Init();
	int IsBinned(int) const;
	int IsBinned(TString) const;
	int Multiply(const TSPhaseSpaceMap &);
	void Print() const;
	void RemoveSelection();
	void SelectRange(const TSVariable &, double min, double max, bool partial=false);
	int SetBinning(const TSVariable &,int n, double* bins);
	int SetBinning(const TSVariable &,int n, double low, double up);
	int SetBinning(TString var_list, TString file);
	int SetVariable(const TSVariable &);
	void SetVolumeSelected(int,bool);
	bool VolumeIsSelected(int) const;


private:



	std::map<int,const TSVariable*> fVarList;

    std::map<int,int> fVarIsBinned;

	std::map<int,TSPhaseSpaceVolume*> fVolumeList;
	std::map<int,bool> fVolIsSelected;

	int fMapOK;
	int fDimension;
	int fNumOfVols;
	int fHasSelection;

	TSHistogramFld fHistoFld;
};

#endif /* TSPHASESPACEMAP_H_ */
