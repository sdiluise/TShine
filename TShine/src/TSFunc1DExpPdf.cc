/*
 * TSFunc1DExpPdf.cc
 *
 *  Created on: Sep 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>

#include <TMath.h>

#include "RootUtils.h"

#include "MessageMgr.h"

#include "TSFunc1DExpPdf.h"


TSFunc1DExpPdf::TSFunc1DExpPdf(TString name, TString title
		,const TSVariable &var_x, const TSParameter &mean, const TSParameter &slope)

:TSFunction1D(name,title,var_x)
{

	fMean = &mean;
	fSlope = &slope;


	AddParameter(mean);
	AddParameter(slope);
}

TSFunc1DExpPdf::~TSFunc1DExpPdf() {

}


double TSFunc1DExpPdf::Evaluate() const{

	if( !fMean || !fSlope){

		MSG::ERROR(__FILE__,"::",__FUNCTION__);
		return 0;
	}

	double x = fGetCachedX();

	//cout<<"Calling Evaluate form 1DExp "<<this->Name()<<" cached_x "<<x<<endl;

	double mean = fMean->GetValue();
	double slope = fSlope->GetValue();

	if(RootUtils::IsZero(slope)){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," slope is zero");
		return 0;
	}

	//cout<<"Calling Evaluate form 1DExp "<<this->Name()<<" cached_x "<<x<<" mean "<<mean<<" slope "<<slope<<endl;


	//Return normalized Gaussian


	double arg = x-mean;

	arg *= slope;

	double val = TMath::Exp(-arg);

	//normalized
	double xmin=GetVariableX()->GetMin();
	double xmax=GetVariableX()->GetMax();

	double integ=-1*(1./slope)*(TMath::Exp(-slope*(xmax-mean))-TMath::Exp(-slope*(xmin-mean)));

	val /= integ;

	if( TMath::IsNaN(val)){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Eval Is NaN ", GetName());
		cout<<" Par set: "<<mean<<" "<<slope<<" at: "<<x<<endl;
	}

	//cout<<" calculated val for Exp: "<<val<<" at "<<x<<" mean "<<mean<<" slope "<<slope<<endl;

	return val;

}
