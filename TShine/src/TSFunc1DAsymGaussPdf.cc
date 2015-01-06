/*
 * TSFunc1DAsymGaussPdf.cc
 *
 *  Created on: Aug 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "TSFunc1DAsymGaussPdf.h"

TSFunc1DAsymGaussPdf::TSFunc1DAsymGaussPdf(TString name, TString title
		,const TSVariable &var_x, const TSParameter &mean, const TSParameter &sigma
		,const TSParameter &delta)
:TSFunc1DGaussPdf(name,title,var_x,mean,sigma)
{

		fDelta = &delta;

		AddParameter(delta);
}

TSFunc1DAsymGaussPdf::~TSFunc1DAsymGaussPdf() {


}

double TSFunc1DAsymGaussPdf::Evaluate() const{

	if( !fMean || !fSigma || !fDelta ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__);
		assert(0);
	}

	double x = fGetCachedX();

	//cout<<"Calling Evaluate form 1DAsymGauss "<<this->Name()<<" cached_x "<<x<<endl;

	double mean = fMean->GetValue();
	double sigma = fSigma->GetValue();

	double delta=fDelta->GetValue();

	//!!!!!
	delta= abs(delta);

	fsigma_spread = 0;//GetSigmaSpread();

	double sleft= sigma*(1-delta);
	double sright= sigma*(1+delta);

	sleft = sleft*sleft + fsigma_spread*fsigma_spread;
	sright = sright*sright + fsigma_spread*fsigma_spread;

	sleft = TMath::Sqrt(sleft);
	sright = TMath::Sqrt(sright);


	double integ= GNorm*0.5*(sleft+sright);

	//Return normalized Gaussian


	double val = 0;
	if( x<=mean ) val=TMath::Gaus(x,mean,sleft,0);
	else val=TMath::Gaus(x,mean,sright,0);

	val /= integ;

	//cout<<" calculated val for AsymGauss: "<<val<<" at: "<<x<<"; mean: "<<mean<<" sig: "<<sigma<<" delta: "<<delta<<endl;

	return val;

}
