/*
 * TSRespFunc.h
 *
 *  Created on: Oct 2, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSRESPFUNC_H_
#define TSRESPFUNC_H_

#include <TGraph.h>
#include <TMultiGraph.h>

#include "TSNamed.h"


#include "TSVariable.h"
#include "TSPhaseSpaceVolume.h"

#include "TSArgList.h"

/*
 *
 */

class TSRespFunc: public TSNamed {

public:

	TSRespFunc();
	TSRespFunc(const TSRespFunc&);
	virtual ~TSRespFunc();



	void AddControlVar(const TSVariable &, double min, double max);

	TSRespFunc* Clone(TString ="") const;

	void Copy(const TSRespFunc &);

	void Disable() { SetActive(false); }
	void Enable() { SetActive(true); }

	void EnableControlVars();
	void DisableControlVars();

	double Eval(double x) const;

	double* GetArrayX() const { return fX; }
	double* GetArrayY() const { return fY; }

	TGraph& GetGraph();
	TGraph* GetGraph(TString) const;

	double GetAxisShiftX() const {return fShift[0];}
	double GetAxisShiftY() const {return fShift[1];}


	int GetN() const {return fN;}
	bool HasControlVars() const {return fHasControlVars;}
	void Init();
	bool IsActive() const {return fIsActive;}
	bool IsEnabled() const {return IsActive();}
	bool IsParab() const {return fIsParab;}
	bool IsLinear() const {return fIsLinear;}
	bool IsUnit() const {return fIsUnit;}
	bool IsAnalytic() const;

	void Print() const;

	void SetActive(bool b) { fIsActive=b; }

	void Set(int N, double* x, double* y, double dx=0, double dy=0);
	void Set(int N, float* x, float* y, float dx=0, float dy=0);

	void Set(int N, double *x, double *y, TSArgList& control_var_list, double dx=0, double dy=0);
	void Set(int N, float *x, float *y, TSArgList& control_var_list, float dx=0, float dy=0);

	void ShiftAxis(double dx, double dy=0);

	void SetControlVarsVolume(const TSPhaseSpaceVolume& );
	const TSPhaseSpaceVolume& GetControlVarsVolume() const;

	double Tweak(double central, double x) const;

private:

	void fCheckAnalytic();
	void fCheckLinearity();
	void fCheckParab();
	void fClear();
	void fFillGraph();
	void fShitAxis();
	double fCalcSpline3Sigma(double, int opt=0) const ;

	int fN;
	double *fX;
	double *fY;
	double fShift[2];
	double fXRange[2];
	double fYRange[2];
	double fParsPoly[3];
	bool fIsActive;
	bool fIsParab;
	bool fIsLinear;
	bool fIsUnit;


	static double kToleranceToUnity;

	TGraph fGraph;

	bool fHasControlVars;
	int fNumOfControlVars;

	TSPhaseSpaceVolume fVarsVolume;

};

#endif /* TSRESPFUNC_H_ */
