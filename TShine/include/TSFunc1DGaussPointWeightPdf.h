/*
 * TSFunc1DGaussPointWeightPdf.h
 *
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *      		Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#ifndef TSFUNC1DGAUSSPOINTWEIGHTPDF_H_
#define TSFUNC1DGAUSSPOINTWEIGHTPDF_H_

#include <TH1F.h>

#include "TSVariable.h"
#include "TSParameter.h"

#include "TSFunc1DGaussPdf.h"


class TSFunc1DGaussPointWeightPdf: public TSFunc1DGaussPdf {

public:
	TSFunc1DGaussPointWeightPdf();
	TSFunc1DGaussPointWeightPdf(TString name, TString title
				, const TSVariable &var_x, const TSParameter &mean, const TSParameter &sigma);
	virtual ~TSFunc1DGaussPointWeightPdf();


    virtual double Eval() const;

    virtual void Init();

    int GetNumOfWeights() const {return fNWeights;}
    int SetPointDistribution(const TH1F &h);

private:

    int fNWeights;
	float *fNPoints;
	float *fPointMult;
	float fWeightsSum;

	const TSVariable *fX;
	const TSParameter *fMean;
	const TSParameter *fSigma;

};

#endif /* TSFUNC1DGAUSSPOINTWEIGHTPDF_H_ */
