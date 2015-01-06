/*
 * TSBetheBlochMgr.h
 *
 *  Created on: Jul 14, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */

#ifndef TSBETHEBLOCHMGR_H_
#define TSBETHEBLOCHMGR_H_

#include <map>

#include "TSBetheBlochFunc.h"

#include "TSPhaseSpaceMap.h"


class TSBetheBlochMgr: public TSBetheBlochFunc {



public:

	TSBetheBlochMgr(TString name, TString title, double (*fcn)(double*, double*), double xmin, double xmax, int npar);
    TSBetheBlochMgr(TString name, TString title, TString table_file);

	virtual ~TSBetheBlochMgr();

	Double_t Eval(Double_t momentum, Double_t mass = 0, Double_t z = 0, Double_t t = 0) const;
	TSBetheBlochFunc* GetVolumeBetheBlochFunc(int) const;
	bool HasPhaseSpaceMap() const {return fHasPSMap;}

	void Init();

	void SetPhaseSpaceMap(const TSPhaseSpaceMap &);

	void SetFunctionToSelection(TSBetheBlochFunc &);

	void UseDefaultBetheBloch(bool b) {fUseDefaultBB = b;}


private:

	bool fHasPSMap;
	bool fUseDefaultBB;

	int fGetVolumeBBId(int) const;


	TSPhaseSpaceMap *fPSMap;
    std::map<int,int> fBBMap;
    std::map<int,TSBetheBlochFunc*> fBBList;

};

#endif /* TSBETHEBLOCHMGR_H_ */
