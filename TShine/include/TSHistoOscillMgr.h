/*
 * TSHistoOscillMgr.h
 *
 *  Created on: Oct 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSHISTOOSCILLMGR_H_
#define TSHISTOOSCILLMGR_H_


#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>


#include "TSNamed.h"

#include "TSVariable.h"

#include "TSOscProb.h"

/*
 *
 */
class TSHistoOscillMgr: public TSNamed {

public:
	TSHistoOscillMgr();
	virtual ~TSHistoOscillMgr();


	void Init();

	void Oscillate();

	void SetAxis(TString);


	void SetHistogram(TH1*, const TSVariable *vx=0);
	void SetHistogram(TH1*, const TSVariable *vx, const TSVariable *vy);

	void SetOscillProb(TSOscProb *);


private:

	void fOscillateX();
	void fOscillateY();

	void fSetAxis();
	bool fApplyToX;
	bool fApplyToY;

	TH1 *fHisto;
	TSOscProb *fProbFunc;


	int fHistoDim;
	int fHistoNbins[2];
	TSVariable *fAxisVariable[2];
};

#endif /* TSHISTOOSCILLMGR_H_ */
