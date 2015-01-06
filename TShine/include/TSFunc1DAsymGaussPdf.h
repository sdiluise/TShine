/*
 * TSFunc1DAsymGaussPdf.h
 *
 *  Created on: Aug 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNC1DASYMGAUSSPDF_H_
#define TSFUNC1DASYMGAUSSPDF_H_


#include "TSFunc1DGaussPdf.h"

/*
 *
 */


class TSFunc1DAsymGaussPdf: public TSFunc1DGaussPdf {


public:

	TSFunc1DAsymGaussPdf(TString name, TString title
						,const TSVariable &var_x, const TSParameter &mean, const TSParameter &sigma
						,const TSParameter &delta );


	virtual ~TSFunc1DAsymGaussPdf();

	virtual double Evaluate() const;

	double GetDelta() const { return fDelta ? fDelta->GetValue(): 0; }

	const TSParameter *GetParamDelta() const {return fDelta;}

private:

	const TSParameter *fDelta;

};

#endif /* TSFUNC1DASYMGAUSSPDF_H_ */
