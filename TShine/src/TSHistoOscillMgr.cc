/*
 * TSHistoOscillMgr.cc
 *
 *  Created on: Oct 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */
#include <cassert>

#include "RootUtils.h"

#include "TSHistoOscillMgr.h"

TSHistoOscillMgr::TSHistoOscillMgr() {


	Init();

}

TSHistoOscillMgr::~TSHistoOscillMgr() {

}




void TSHistoOscillMgr::Init()
{

	fHisto = 0;
	fProbFunc = 0;

	fHistoDim=0;
	fHistoNbins[0]=0;
	fHistoNbins[1]=0;

	fAxisVariable[0]=0;
	fAxisVariable[1]=0;

	fApplyToX = 0;
	fApplyToY = 0;

}


void TSHistoOscillMgr::Oscillate()
{


	if( !fHisto ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram not se");
		return;
	}

	if( !fProbFunc ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Probability Function not set");
		return;
	}


	if(fApplyToX) {
		fOscillateX();
	}


	if(fApplyToY){
		fOscillateY();
	}

	//MSG::ERROR(__FILE__,"::",__FUNCTION__," Axis not set ");

}



void TSHistoOscillMgr::fOscillateX()
{

	for(int ix=1; ix<=fHistoNbins[0]; ++ix){

		double x = fHisto->GetXaxis()->GetBinCenter(ix);

		fProbFunc->SetE(x);

		double prob = fProbFunc->Eval();


		for(int iy=1; iy<=fHistoNbins[1]; ++iy){

			double y = fHisto->GetYaxis()->GetBinCenter(iy);

			float cont = fHisto->GetBinContent(ix,iy);

			//if(ix==10) cout<<ix<<" "<<iy<<" "<<x<<" "<<y<<"; "<<cont<<"; "<<prob<<endl;

			fHisto->SetBinContent(ix,iy,cont*prob);

			if(ix==10) cout<<ix<<" "<<iy<<" "<<x<<" "<<y<<"; "<<cont<<"; "<<prob<<"; "<<fHisto->GetBinContent(ix,iy)<<endl;


		}//ix

	}//iy

}



void TSHistoOscillMgr::fOscillateY()
{


	for(int iy=1; iy<=fHistoNbins[1]; ++iy){

		double y = fHisto->GetYaxis()->GetBinCenter(iy);

		fProbFunc->SetE(y);

		double prob = fProbFunc->Eval();


		for(int ix=1; ix<=fHistoNbins[0]; ++ix){

			float cont = fHisto->GetBinContent(ix,iy);

			fHisto->SetBinContent(ix,iy,cont*prob);

		}//iy

	}//ix

}



void TSHistoOscillMgr::SetAxis(TString a) {

	a.ToLower();

	if(a == "x" ) fApplyToX = true;
	else if(a == "y" ) fApplyToY = true;
	else
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid axis name");
}




void TSHistoOscillMgr::SetHistogram(TH1* h, const TSVariable *vx) {


	SetHistogram(h,vx,0);


}


void TSHistoOscillMgr::SetHistogram(TH1* h, const TSVariable *vx, const TSVariable *vy) {

	if( h ){

		fHisto = h;

	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null Ptr to Histogram");
		assert(0);
	}



	if( RootUtils::IsTH1(*h) ){

		fHistoDim = 1;

		if( vx ){
			fAxisVariable[0] = (TSVariable*)vx;
		}else{

		}


	}else if( RootUtils::IsTH2(*h) ){

		fHistoDim = 2;


		if( vx ){
			fAxisVariable[0] = (TSVariable*)vx;
		}else{

		}

		if( vy ){
			fAxisVariable[1] = (TSVariable*)vy;
		}else{

		}


	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not a TH1/TH2 obj");
		assert(0);
	}


	fHistoNbins[0] = fHisto->GetNbinsX();
	fHistoNbins[1] = fHisto->GetNbinsY();


	fSetAxis();

}

void TSHistoOscillMgr::SetOscillProb(TSOscProb *prob) {

	if(!prob){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null ptr to argument");
		return;
	}


   fProbFunc = prob;

   fSetAxis();

}




void TSHistoOscillMgr::fSetAxis() {

	if( !fHisto || !fProbFunc ){
		return;
	}


	if( fAxisVariable[0] && fAxisVariable[0] == fProbFunc->GetEnergyVariableBound() ){
		fApplyToX = true;
	}

	if( fAxisVariable[1] && fAxisVariable[1] == fProbFunc->GetEnergyVariableBound() ){
		fApplyToY = true;
	}

}
