/*
 * TSTemplatedLikelihoodCalc.cc
 *
 *  Created on: Nov 3, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cmath>

#include <TMath.h>

#include "RootUtils.h"

#include "MessageMgr.h"

#include "TSTemplatedLikelihoodCalc.h"


#define VERBOSE true
#if VERBOSE
#define V(x) cout<<x<<endl;
#else
#define V(x)
#endif


TSTemplatedLikelihoodCalc::TSTemplatedLikelihoodCalc() {

	Init();

}

TSTemplatedLikelihoodCalc::~TSTemplatedLikelihoodCalc() {


}

void TSTemplatedLikelihoodCalc::AddObservedVsExpected(TH1 *Obs, TSEventDataSet *Exp){

	V(endl<<__FILE__<<"::"<<__FUNCTION__);

	if( !Obs  || !Exp){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null Pointer to Observed/Expectation object");
		exit(0);
	}

	TH1* hExp = Exp->GetHistogram();

	if( !hExp ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null Pointer to Expectation Histo");
		MSG::ERROR("DataSet: ",Exp->GetName());
		exit(0);
	}


	if( Obs->GetDimension() != hExp->GetDimension() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histograms with different dimensions");
		exit(0);
	}


	if( !RootUtils::SameBinning(*Obs,*hExp)  ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histograms with different axis binning");
		exit(0);
	}


	Exp->UpdateParamList();

	if( Exp->HasWeightParams() ){

		fHasParamSet = true;

		fNuiParam_Set.Add(Exp->GetNuiParamSet());

	}


	aObsData.Add(Obs);
	aExpData.Add(hExp);
	aExpDataMgr.Add(Exp);

	fNHistoPairs++;
}


void TSTemplatedLikelihoodCalc::AddParamChi2Constraint(TSChi2ConstraintCalc &Constr)
{


	aConstraint[fNConstraints] = &Constr;

	fNConstraints++;
}


double TSTemplatedLikelihoodCalc::Compute() const
{



	fValue = 0;
	fLikeValue = 0;

	fLikeValuePart.clear();

	double LogL_Tot = 0;

	double n_obs = 0;
	double n_exp = 0;

	int bin = 0;
	//
	//use TH1::GetBin(i,j) and TH1::GetArray() to speed up
	// content retrieval
	//

	//cout<<__FUNCTION__<<" "<<fNHistoPairs<<endl;


	for(int pair=0; pair<fNHistoPairs; ++pair){


		double LogL = 0;

		dynamic_cast<TSEventDataSet*>(aExpDataMgr[pair])->Process();


		TH1 *hobs = (TH1*)aObsData[pair];
		TH1 *hexp = (TH1*)aExpData[pair];

		float *a_obs = 0;
		float *a_exp = 0;

		if( RootUtils::IsTH1F(*hobs) ){
			a_obs = static_cast<TH1F*>(hobs)->GetArray();
			a_exp = static_cast<TH1F*>(hobs)->GetArray();
		}
		else if( RootUtils::IsTH2F(*hobs) ){
			a_obs = static_cast<TH2F*>(hobs)->GetArray();
			a_exp = static_cast<TH2F*>(hobs)->GetArray();
		}

		int Nx = hobs->GetNbinsX();
		int Ny = hexp->GetNbinsY();

		//cout<<" "<<hobs->GetName()<<" "<<hexp->GetName()<<" Nx/Ny: "<<Nx<<" "<<Ny<<endl;

		for(int ibx=1; ibx<=Nx; ibx++){

			//bin = hobs->GetBin(ibx,1);
			bin = RootUtils::GetBin(Nx,ibx);

			n_obs = hobs->GetBinContent(ibx,1);
			n_exp = hexp->GetBinContent(ibx,1);

			//n_obs = a_obs[bin];
			//n_exp = a_exp[bin];


			if(n_obs>0 && n_exp>0){
				LogL += n_exp-n_obs + n_obs*log(n_obs/n_exp);
			}else{
				LogL += n_exp - n_obs;
			}

			//cout<<bin<<"; "<<ibx<<" 1; obs: "<<n_obs<<" exp: "<<n_exp<<" LogL: "<<LogL<<" "<<a_obs[bin]<<" "<<endl;

			for(int iby=2; iby<=Ny; iby++){

				//bin = hobs->GetBin(ibx,iby);
				bin = RootUtils::GetBin(Nx,ibx,Ny,iby);

				n_obs = hobs->GetBinContent(ibx,iby);
				n_exp = hexp->GetBinContent(ibx,iby);

				//n_obs = a_obs[bin];
				//n_exp = a_exp[bin];

				//cout<<ibx<<" "<<iby<<"; "<<n_obs<<" "<<n_exp<<endl;

				if(n_obs>0 && n_exp>0){
					LogL += n_exp-n_obs + n_obs*log(n_obs/n_exp);
				}else{
					LogL += n_exp - n_obs;
				}

			}//iby
		}//ibx

		fLikeValuePart.push_back(2*LogL);

		LogL_Tot += LogL;

	}//pair


	double Chi2 = ComputeChi2Constraint();

	fLikeValue = 2*LogL_Tot;

	double L = 2*LogL_Tot + Chi2;

	fValue = L;

	return L;
}



double TSTemplatedLikelihoodCalc::ComputeChi2Constraint() const
{


	fConstraintValue = 0;

	if(fNConstraints == 0){
		return fConstraintValue;
	}


	for(int i=0; i<fNConstraints;++i){

		fConstraintValue += dynamic_cast<TSChi2ConstraintCalc*>(aConstraint[i])->Compute();
	}


	return fConstraintValue;

}



double TSTemplatedLikelihoodCalc::GetLikeValuePart(int i)
{

	if(fLikeValuePart.size()>=i+1){
		return fLikeValuePart.at(i);
	}

	return 0;
}

void TSTemplatedLikelihoodCalc::Init(){

	fValue = 0;
	fLikeValue = 0;
	fConstraintValue = 0;


	fNHistoPairs = 0;

	fNConstraints = 0;


	fHasParamSet = false;

}
