/*
 * TSFunc1DExpPdf.h
 *
 *  Created on: Sep 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSFUNC1DEXPPDF_H_
#define TSFUNC1DEXPPDF_H_

#include "TSFunction1D.h"

/*
 *
 */


class TSFunc1DExpPdf: public TSFunction1D {

public:

	TSFunc1DExpPdf(TString name, TString title
			,const TSVariable &var_x, const TSParameter &mean, const TSParameter &slope);


	virtual ~TSFunc1DExpPdf();

	virtual double Evaluate() const;

	private:

	const TSParameter *fMean;
	const TSParameter *fSlope;

};

#endif /* TSFUNC1DEXPPDF_H_ */
