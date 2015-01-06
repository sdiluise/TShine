/*
 * TSDataSetMgr.h
 *
 *  Created on: Jun 7, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *  TSDataBin
 *  TSPhaseSpaceMap/Fld
 *
 *
 *
 *
 */

#ifndef TSDATASETMGR_H_
#define TSDATASETMGR_H_


#include <string>
#include <map>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>


#include "TSVariable.h"

#include "TSHistogramFld.h"
#include "TSPhaseSpaceMap.h"
#include "TSPhaseSpaceFld.h"
#include "TSDataBin.h"


#include "TSArgList.h"



class TSDataSetMgr: public TSNamed {


public:

	TSDataSetMgr(TString name, TString tag, TString title);
	virtual ~TSDataSetMgr();

	int Fill();
	int Fill(const TSVariable &);
	int Fill(const TSArgList &);
	int FillHistosFromFile(TFile *, TString opt="");
	TSDataBin* GetDataBin(int) const;
	TSPhaseSpaceFld* GetPhaseSpaceFld() const {return fPhaseSpaceFld;}
	int GetNumOfDataBins() const {return fNumOfDataBins;}
	int GetNumOfVars() const {return fNumOfVars;}
	TString GetTag() const {return fTag;}
	bool HasPhaseSpace() const {return fHasPhaseSpace;}
	bool HasTag() const {return fHasTag;}
	void Init();

	void Print(TString opt="") const;
	void PrintVariables(TString opt="") const;

	bool SetDataBin(TSDataBin&);
    bool SetPhaseSpace(TSPhaseSpaceMap&);
    void SetTag(TString);


private:

    bool fBuildDataBins();
    bool fSetVariable(const TSVariable &, int opt);
    bool fVarIsPhaseSpace(int) const;
    bool fVarIsData(int) const;


    TString fTag;

    int fNumOfDataBins;
    int fNumOfVars;

    bool fHasDataBin;
    bool fHasPhaseSpace;
    bool fHasTag;

    TSPhaseSpaceMap *fPhaseSpaceMap;
    TSPhaseSpaceFld *fPhaseSpaceFld;

    std::map<int,const TSVariable *> fVarList;

    std::map<int,TSDataBin*> fDataBinList;

    struct fVarSettings{
    	bool IsPhaseSpace;
    	bool IsData;
    	bool IsGlobalData;

    	fVarSettings():IsPhaseSpace(0),IsData(0),IsGlobalData(0)
    	{}
    };

    std::map<int,fVarSettings*> fVarSetsList;



};




#endif /* TSDATASETMGR_H_ */
