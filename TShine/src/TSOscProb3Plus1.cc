/*
 * TSOscProb3Plus1.cc
 *
 *  Created on: Oct 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cmath>

#include <TMath.h>

#include "TSOscProb3Plus1.h"


TSOscProb3Plus1::TSOscProb3Plus1() {

	Init();

}



TSOscProb3Plus1::TSOscProb3Plus1(TString name, TString title)
:TSOscProb(name,title)
{

	Init();
}



TSOscProb3Plus1::~TSOscProb3Plus1() {


}


double TSOscProb3Plus1::ComputeSurvProbNuE() const
{

	double arg = mArgConst*mDM2_14*mL/mE;

	double prob = 1 - mSin2_2Theta_ee*sin(arg)*sin(arg);

	//cout<<mSin2_2Theta_ee<<" "<<mDM2_14<<" "<<mL<<" "<<mE<<" "<<prob<<endl;

	return prob;

	//mSin2_Theta_ee = fParamCache[0];
	//mSin2_Theta_mumu = fParamCache[1];
	//mDM2_14 = fParamCache[2];

}


double TSOscProb3Plus1::ComputeNuSApp() const
{

	double arg = mArgConst*mDM2_14*mL/mE;

	double prob = mSin2_2Theta_ee*sin(arg)*sin(arg);

	return prob;

	//mSin2_Theta_ee = fParamCache[0];
	//mSin2_Theta_mumu = fParamCache[1];
	//mDM2_14 = fParamCache[2];


}

double TSOscProb3Plus1::ComputeSurvProbNuMu() const
{

	double arg = mArgConst*mDM2_14*mL/mE;

	double prob = 1 - mSin2_2Theta_mumu*sin(arg)*sin(arg);

	return prob;

	//mSin2_Theta_ee = fParamCache[0];
	//mSin2_Theta_mumu = fParamCache[1];
	//mDM2_14 = fParamCache[2];


}

double TSOscProb3Plus1::ComputeNuMuToNuE() const
{

	double arg = mArgConst*mDM2_14*mL/mE;

	double prob = 0.25*mSin2_2Theta_mumu*mSin2_2Theta_ee*sin(arg)*sin(arg);

	return prob;

	//mSin2_Theta_ee = fParamCache[0];
	//mSin2_Theta_mumu = fParamCache[1];
	//mDM2_14 = fParamCache[2];

}

void TSOscProb3Plus1::DefineTransition(int id, TString name, TString label)
{

	for( int i=0; i<vTrsId.size(); ++i){

		if(vTrsId[i] == id){
			vTrsName[i] = name;
			vTrsLabel[i] = label;
			return;
		}
	}

	vTrsId.push_back(id);
	vTrsName.push_back(name);
	vTrsLabel.push_back(label);

}



double TSOscProb3Plus1::Eval() {

/**
 *
 * temporary if switch implementation
 *
 */

	if( fTransitionId == fSurvProbNuE){

		return ComputeSurvProbNuE();

	}else
	if( fTransitionId == fNuSApp){

		return ComputeNuSApp();

	}else
	if( fTransitionId == fSurvProbNuMu){

		return ComputeSurvProbNuMu();

	}else
	if( fTransitionId == fNuMuToNuE){

		return ComputeNuMuToNuE();

	}


	MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid Transition Index ", fTransitionId);


	return 0;

}

void TSOscProb3Plus1::FillParameter(int i){

	TSOscProb::FillParameter(i);

	//mSin2_2Theta_ee = fParamCache[0];
	//mSin2_2Theta_mumu = fParamCache[1];
	//mDM2_14 = fParamCache[2];

	if(i==0) mSin2_2Theta_ee = fParamCache[i];
	else if (i==1) mSin2_2Theta_mumu = fParamCache[i];
	else if (i==2) mDM2_14 = fParamCache[i];

}

void TSOscProb3Plus1::FillParameters(){


	TSOscProb::FillParameters();

	mSin2_2Theta_ee = fParamCache[0];
	mSin2_2Theta_mumu = fParamCache[1];
	mDM2_14 = fParamCache[2];


}


TString TSOscProb3Plus1::GetTransitionLabel()
{

	for( int i=0; i<vTrsId.size(); ++i){

		if(vTrsId[i] == fTransitionId){
				return vTrsLabel[i];
		}
	}


	return "";

}



void TSOscProb3Plus1::Init() {


	TSOscProb::Init();

	mDM2_14 = 0;
	mSin2_2Theta_ee = 0;
	mSin2_2Theta_mumu = 0;


	fSurvProbNuE=1;
	fNuSApp = 2;
	fSurvProbNuMu =3;
	fNuMuToNuE =4;

	DefineTransition(fSurvProbNuE,"SurvProbNuE","#nu_{e} #rightarrow #nu_{e}");
	DefineTransition(fNuSApp,"NuSApp","#nu_{e} #rightarrow #nu_{s}");
	DefineTransition(fSurvProbNuMu,"SurvProbNuMu","#nu_{#mu} #rightarrow #nu_{#mu}");
	DefineTransition(fNuMuToNuE,"NuMuToNuE","#nu_{#mu} #rightarrow #nu_{#e}");




}


void TSOscProb3Plus1::SetParameters(const TSParamList &list)
{

	TSOscProb::SetParameters(list);


}

void TSOscProb3Plus1::SetParameters(TSParameter &sin2_2theta_ee, TSParameter &sin2_2theta_mumu, TSParameter &dm2_14)
{

	TSOscProb::SetParameters( TSParamList(sin2_2theta_ee,sin2_2theta_mumu,dm2_14) );


}


void TSOscProb3Plus1::SetTransition(TString name)
{

	cout<<__FUNCTION__<<" "<<name<<endl;

	for( int i=0; i<vTrsId.size(); ++i){

		if(vTrsName[i] == name){
			fTransitionId = vTrsId[i];
			cout<<__FUNCTION__<<" "<<name<<" "<<i<<endl;
			return;
		}
	}

	MSG::ERROR(__FILE__,"::",__FUNCTION__," Transition: ",name, " Not found ");
	return;

}

void TSOscProb3Plus1::SetTransition(int id)
{

	fTransitionId = id;

}



