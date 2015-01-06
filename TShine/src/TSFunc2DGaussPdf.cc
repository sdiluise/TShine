/*
 *
 *
 * TSFunc2DGaussPdf.cc
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

#include "TSFunc2DGaussPdf.h"


TSFunc2DGaussPdf::TSFunc2DGaussPdf(TString name, TString title,
		const TSVariable &var_x, const TSParameter &mean_x, const TSParameter &sigma_x,
		const TSVariable &var_y, const TSParameter &mean_y, const TSParameter &sigma_y)
:TSFunction2D(name,title,var_x,var_y)
{


	//save local copy of pointer to params , it may increase speed
	//check if this may hide changes of params
	//by other methods
	fMeanX = &mean_x;
	fSigmaX = &sigma_x;

	fMeanY = &mean_y;
	fSigmaY = &sigma_y;

	AddParameter(mean_x);
	AddParameter(sigma_x);
	AddParameter(mean_y);
	AddParameter(sigma_y);


}

TSFunc2DGaussPdf::~TSFunc2DGaussPdf() {


}


void TSFunc2DGaussPdf::Init(){



}


double TSFunc2DGaussPdf::Evaluate() const{

	if( !fMeanX || !fSigmaX || !fMeanY || !fSigmaY ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__, " null pointer to params. Function: ",GetName());
		assert(0);
	}

	double x = fGetCachedX();
	double y = fGetCachedY();

	//cout<<"Calling Evaluate form 2DGauss "<<this->Name()<<" cached_x: "<<x<<" cache_y: "<y<<<endl;

	double meanx = fMeanX->GetValue();
	double sigmax = fSigmaX->GetValue();

	double meany = fMeanY->GetValue();
	double sigmay = fSigmaY->GetValue();



	sigmax = sigmax*sigmax + fsigma_spreadX*fsigma_spreadX;
	sigmax = TMath::Sqrt(sigmax);

	sigmay = sigmay*sigmay + fsigma_spreadY*fsigma_spreadY;
	sigmay = TMath::Sqrt(sigmay);


	//Return normalized 2D Gaussian


	double valx = TMath::Gaus(x,meanx,sigmax,1);
	double valy = TMath::Gaus(y,meany,sigmay,1);

	return valx*valy;

}
