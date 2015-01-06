/*
 * TSHistoNuiParamMgr.h
 *
 *  Created on: Oct 10, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSHISTONUIPARAMMGR_H_
#define TSHISTONUIPARAMMGR_H_

#include <cassert>

#include <vector>

#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>

#include "TSNamed.h"

#include "TSVariable.h"
#include "TSNuiParam.h"
#include "TSNuiParamSet.h"
#include "TSRespFunc.h"

/*
 *
 *
 *
 */

class TSHistoNuiParamMgr: public TSNamed {


public:

	TSHistoNuiParamMgr();
	virtual ~TSHistoNuiParamMgr();

	int AddNuiParam(TSNuiParam &);
	int AddNuiParamWithRespFunc(TSNuiParam &, TSRespFunc &);

	bool HasHistogram() const {return bool(fHisto);}

	void Init();

	void Print() const;
	void PrintBinInfo(int ix, int iy=1) const;

	void ReweightHistogram(int opt=0);

	void SetHistogram(TH1*, const TSVariable *vx=0);
	void SetHistogram(TH1*, const TSVariable *vx, const TSVariable *vy);

private:

	void fDeleteNuiParamArray();
	int fGetLinearIdx(int,int=1) const;

	TH1 *fHisto;
	TSVariable *fAxisVariable[2];

	int fHistoDim;
	int fHistoNbins[2];

	std::vector<TSNuiParamSet*> fVecOfNuiParamSet;

};

#endif /* TSHISTONUIPARAMMGR_H_ */
