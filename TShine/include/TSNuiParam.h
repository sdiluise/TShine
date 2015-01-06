/*
 * TSNuiParam.h
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSNUIPARAM_H_
#define TSNUIPARAM_H_

#include <TRandom.h>
#include <TMath.h>
#include <TF1.h>

#include "TSVariable.h"

#include "TSParameter.h"

#include "TSCategory.h"
#include "TSCategoryCard.h"
#include "TSPhaseSpaceVolume.h"


/*
 *
 */

class TSNuiParam: public TSParameter {

public:

	TSNuiParam();
	TSNuiParam(TString name, TString label, TString title, TString unit="");
	virtual ~TSNuiParam();

	void SetCategoryCard(const TSCategoryCard &);
	void AddControlVar(TSVariable &, double min, double max);

	int CompareCategory(const TSCategoryCard &, int opt=0);

	const TSCategoryCard& GetCategoryCard() const {return fCard;}

	TSVariable* GetControlVariable(int) const;
	int GetControlVarIdx(const TSVariable &) const;

	int GetNumOfControlVars() const {return fNumOfControlVars;}
	TString GetControlVarsLabel() const;

	double GetPriorValue() const {return fPriorVal;}
	double GetPriorError() const {return fPriorError;}
	double GetPriorErrorLow() const {return fPriorErrLow;}
	double GetPriorErrorUp() const {return fPriorErrUp;}

	double GetRandom();

	bool HasCategoryCard() const {return fHasCategory;}
	bool HasControlVars() const {return fHasControlVars;}
	bool HasControlVariable(const TSVariable &) const;
	bool HasPrior() const {return fHasPrior;}
	bool HasErrorPrior() const {return fHasErrorPrior;}
	bool HasAsymErrorPrior() const {return fHasAsymErrPrior;}
	void Init();

	void Print();

	void Randomize();

	void Restore();
	void SetPriorValue(double);
	void SetPriorError(double);
	void SetPriorValueAndError(double v,double e) {SetPriorValue(v);SetPriorError(e);}
	void SetPriorAsymErrors(double,double);

private:

	bool   fHasPrior;
	bool fHasErrorPrior;
	bool fHasAsymErrPrior;
	double fPriorVal;
	double fPriorError;
	double fPriorErrLow;
	double fPriorErrUp;

	// temp solution, create a
	//structure for multi var and ranges
	TSVariable *fVar;
	TSVariable *fVarLoc;

	std::vector<TSVariable*> fVarList;
	std::vector<TSVariable*> fVarLocList;


	int fNumOfControlVars;
	bool fHasControlVars;

	double fVarMin;
	double fVarMax;

	TSCategoryCard fCard;

	bool fHasCategory;

	TRandom fRand;

	TF1 fFunc;

};

#endif /* TSNUIPARAM_H_ */
