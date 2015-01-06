/*
 * TSFunc2DGaussPdf.h
 *
 *  Created on: Aug 3, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#ifndef TSFUNC2DGAUSSPDF_H_
#define TSFUNC2DGAUSSPDF_H_

#include <TString.h>

#include <TMath.h>

#include "TSVariable.h"
#include "TSParameter.h"

#include "TSBaseFunction.h"
#include "TSFunction2D.h"


class TSFunc2DGaussPdf: public TSFunction2D {


public:

	TSFunc2DGaussPdf();
	TSFunc2DGaussPdf(TString name,TString title);
	TSFunc2DGaussPdf(TString name, TString title,
			const TSVariable &var_x, const TSParameter &mean_x, const TSParameter &sigma_x,
			const TSVariable &var_y, const TSParameter &mean_y, const TSParameter &sigma_y);
	virtual ~TSFunc2DGaussPdf();



	virtual double Evaluate() const;

	virtual void Init();

	void SetSigmaSpreadX(double v) const {fsigma_spreadX=v;}
	virtual double GetSigmaSpreadX() const {/*cout<<"gatspr:"<<this<<endl;*/ return fsigma_spreadX;}

	void SetSigmaSpreadY(double v) const {fsigma_spreadY=v;}
	virtual double GetSigmaSpreadY() const {/*cout<<"gatspr:"<<this<<endl;*/return fsigma_spreadY;}

	//for direct access
	double GetMeanX() const { return fMeanX ? fMeanX->GetValue(): 0; }
	double GetSigmaX() const {
		return fSigmaX ? TMath::Sqrt( TMath::Power(fSigmaX->GetValue(),2)+TMath::Power(fsigma_spreadX,2) ): 0;
	}
	double GetMeanY() const { return fMeanY ? fMeanY->GetValue(): 0; }
	double GetSigmaY() const {
		return fSigmaY ? TMath::Sqrt( TMath::Power(fSigmaY->GetValue(),2)+TMath::Power(fsigma_spreadY,2) ): 0;
	}

	const TSParameter *GetParamMeanX() const {return fMeanX;}
	const TSParameter *GetParamMeanY() const {return fMeanY;}
	const TSParameter *GetParamSigmaX() const {return fSigmaX;}
	const TSParameter *GetParamSigmaY() const {return fSigmaY;}


private:


	//const TSVariable *fX;
	const TSParameter *fMeanX;
	const TSParameter *fSigmaX;

	//const TSVariable *fY;
	const TSParameter *fMeanY;
	const TSParameter *fSigmaY;

	mutable double fsigma_spreadX;
	mutable double fsigma_spreadY;


	double fParams[4];
};

#endif /* TSFUNC1DGAUSSPDF_H_ */
