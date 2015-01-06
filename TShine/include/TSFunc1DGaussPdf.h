/*
 * TSFunc1DGaussPdf.h
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */

#ifndef TSFUNC1DGAUSSPDF_H_
#define TSFUNC1DGAUSSPDF_H_

#include <iostream>

#include <TMath.h>


#include "TSFunction1D.h"



using std::cout;
using std::endl;
/*
 *
 */


class TSFunc1DGaussPdf: public TSFunction1D {

public:
	TSFunc1DGaussPdf(TString name, TString title
					,const TSVariable &var_x, const TSParameter &mean, const TSParameter &sigma);


	virtual ~TSFunc1DGaussPdf();

	virtual double Evaluate() const;


	void SetSigmaSpread(double v) const {fsigma_spread=v;}
	virtual double GetSigmaSpread() const {cout<<"gatspr:"<<this<<endl;return fsigma_spread;}

	//for direct access
	double GetMean() const {return GetMeanX(); }
	double GetMeanX() const { return fMean ? fMean->GetValue(): 0; }
	double GetSigma() const { return GetSigmaX(); }
	double GetSigmaX() const {
		return fSigma ? TMath::Sqrt( TMath::Power(fSigma->GetValue(),2)+TMath::Power(fsigma_spread,2) ): 0;
	}

	const TSParameter *GetParamMean() const {return fMean;}
	const TSParameter *GetParamMeanX() const {return fMean;}
	const TSParameter *GetParamSigma() const {return fSigma;}
	const TSParameter *GetParamSigmaX() const {return fSigma;}

protected:

	const TSParameter *fMean;
	const TSParameter *fSigma;


	mutable double fsigma_spread;

};

#endif /* TSFUNC1DGAUSSPDF_H_ */
