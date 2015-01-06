/*
 * TSFunc1DGaussAsymPdf.h
 *
 *
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 */

#ifndef TSFUNC1DGAUSSASYMPDF_H_
#define TSFUNC1DGAUSSASYMPDF_H_

#include "TSFunc1DGaussPdf.h"



class TSFunc1DGaussAsymPdf: public TSFunc1DGaussPdf {


public:
	TSFunc1DGaussAsymPdf();
	TSFunc1DGaussAsymPdf(TString name, TString title
					,const TSVariable &var_x
					,const TSParameter &mean, const TSParameter &sigma, const TSParameter &delta);
	virtual ~TSFunc1DGaussAsymPdf();

	/*
	 * Methods that need to be implemented from the mother class
	 * ROOT::TMath::...
	 *
	 */

	virtual double DoEvalPar(const double *x, const double *p) const {return 0;};
	virtual const double* Parameters() const {return 0;}
	virtual void SetParameters(const double *p) {}
	//ROOT::Math::IParametricFunctionMultiDim* Clone() const
	//   {
	//	      return new TSBaseFunction();
	//	   }

	virtual unsigned int NDim() const {return fNDim;}
	virtual unsigned int NPar() const {return fNPar;}
	/** */

	virtual void Init();

	virtual double Eval() const;

private:

	const TSVariable *fX;
	const TSParameter *fMean;
	const TSParameter *fSigma;
	const TSParameter *fDelta;

	int fNDim;
	int fNPar;

	mutable double chache_x;

};

#endif /* TSFUNC1DGAUSSASYMPDF_H_ */
