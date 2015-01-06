/*
 * TSPhaseSpaceFld.h
 *
 *  Created on: Jun 5, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#ifndef TSPHASESPACEFLD_H_
#define TSPHASESPACEFLD_H_

#include <string>
#include <map>

#include <TString.h>

#include "TSHistogramFld.h"
#include "TSPhaseSpaceMap.h"


class TSPhaseSpaceFld: public TSNamed {

	// to begin with
	//  Relate to Map by pointer
	//: public TSPhaseSpaceMap {


public:

	TSPhaseSpaceFld(TString name, TString tag, TString title, const TSPhaseSpaceMap &);
	virtual ~TSPhaseSpaceFld();


	int Fill();
	int GetEntriesIn() const {return fEntriesIn;}
	TSHistogramFld* GetHistogramFld() const {return fHistoFld;}
	int GetNumOfCalls() const {return fNumOfCalls;}
	TString GetTag() const {return fTag;}
	float GetVolumeEntries(int) const;
	void Init();
	void Print() const;

	void SetTag(TString);

private:


	int fEntriesIn;
	long int fNumOfCalls;

	void fRenameHistos();
	void fInitCounters();

	TString fTag;

	const TSPhaseSpaceMap *fPhaseSpaceMap;

	std::map<int,float> fVolumeEntries;

	TSHistogramFld *fHistoFld;

};



#endif /* TSPHASESPACEFLD_H_ */
