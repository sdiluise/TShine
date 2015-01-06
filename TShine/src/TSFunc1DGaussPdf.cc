/*
 *
 *
 * TSFunc1DGaussPdf.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <cassert>

#include <TMath.h>

#include "MessageMgr.h"

#include "TSFunc1DGaussPdf.h"


TSFunc1DGaussPdf::TSFunc1DGaussPdf(TString name, TString title
					,const TSVariable &var_x, const TSParameter &mean, const TSParameter &sigma)

:TSFunction1D(name,title,var_x)
{

	fMean = &mean;
	fSigma = &sigma;


	AddParameter(mean);
	AddParameter(sigma);

}

TSFunc1DGaussPdf::~TSFunc1DGaussPdf() {


}


double TSFunc1DGaussPdf::Evaluate() const{

	if( !fMean || !fSigma){

		MSG::ERROR(__FILE__,"::",__FUNCTION__);
		return 0;
	}

	double x = fGetCachedX();

	//cout<<"Calling Evaluate form 1DGauss "<<this->Name()<<" cached_x "<<x<<endl;

	double mean = fMean->GetValue();
	double sigma = fSigma->GetValue();

	//cout<<"Calling Evaluate form 1DGauss "<<this->Name()<<" cached_x "<<x<<" mean "<<mean<<" sigma "<<sigma<<endl;

	fsigma_spread = 0;//GetSigmaSpread();

	double sigma2 = sigma*sigma + fsigma_spread*fsigma_spread;

	sigma2 = TMath::Sqrt(sigma2);


	//Return normalized Gaussian


	double val = TMath::Gaus(x,mean,sigma2,1);

	if( TMath::IsNaN(val)){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Eval Is NaN ", GetName());
		cout<<" Par set: "<<mean<<" "<<sigma2<<" "<<sigma<<" "<<fsigma_spread<<" at: "<<x<<endl;
		fSigma->PrintPrimaryParamList();
	}

	//cout<<" calculated val for Gauss: "<<val<<" at "<<x<<" mean "<<mean<<" sig "<<sigma<<endl;

	return val;

}
