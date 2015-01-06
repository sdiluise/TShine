/*
 * TH1FAuto.cc
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <iostream>

#include <TROOT.h>
#include <TH1F.h>

#include "TH1FAuto.h"

#include "MessageMgr.h"

//ClassImp(TH1FAuto);


using std::cout;
using std::endl;

TH1FAuto::TH1FAuto()
:TH1F("","",1,0.,1.)
{

	fInit();
}

TH1FAuto::TH1FAuto(const char* name, const char *title)
: TH1F(name,title,1,0.,1.)
{

  fInit();

}



TH1FAuto::TH1FAuto(const char* name, const char *title, double low, double up)
: TH1F(name,title,1,0.,1.)
{


  fInit();

  SetRangeLimitsX(low,up);

}



TH1FAuto::TH1FAuto(const char* name, const char *title, int n, double xmin, double xmax)
: TH1F(name,title,n,xmin,xmax)
{


  fInit();

  fIsBuilt=true;

  fIsAuto=false;
}




TH1FAuto::TH1FAuto(const char* name, const char *title, int n, double* bins)
: TH1F(name,title,n,bins)
{


  fInit();

  fIsBuilt=true;

}


TH1FAuto::TH1FAuto(const TH1FAuto& other)
{

	TH1FAuto::Copy(other);


}


void TH1FAuto::Copy(const TH1FAuto& other)
{

	if(other.IsBuilt() ){

		  cout<<__FILE__<<"::"<<__FUNCTION__<<":"<<GetName()<<endl;

		  TH1F::Copy( *(TH1F*)&other );

	  }else{


		  fInit();

		  if( other.HasLowRangeLimitX() ){
			  SetLowRangeLimitX( other.GetLowRangeLimitX() );
		  }
		  if( other.HasUpRangeLimitX() ){
			  SetUpRangeLimitX( other.GetUpRangeLimitX() );
		  }


	  }

}


TObject* TH1FAuto::Clone(const char* newname) const{


	cout<<"OK CLONING: "<<__FUNCTION__<<" "<<GetName()<<endl;

	TH1FAuto *h=new TH1FAuto(*this);

	h->SetName(newname);

	return h;


}


TH1FAuto::~TH1FAuto() {


}



int TH1FAuto::BufferEmpty(int action){



	if(action==4){
		//cout<<"calling BufferEmpty: "<<action<<endl;
		FlushBuffer();
		return 1;
	}

	//cout<<"calling BufferEmpty: "<<action<<endl;
	return TH1F::BufferEmpty();
}



void TH1FAuto::Draw(Option_t* opt){

     FlushBuffer();

     TH1F::Draw(opt);
}


int TH1FAuto::Fill(double x) {

  //cout<<"AutoFill "<<endl;

  if( !fIsBuilt ){

    fFillBuffer(x);

    return GetBufferSize();

  }else{

    return TH1F::Fill(x);
  }


}


void TH1FAuto::FillBufferTo(double *array){

	int N;
	double *x;

	int n=0;

	N = fBuffer.GetEntriesFast();

	fBuffer.Draw("x","","goff");
	x= fBuffer.GetV1();

	for(int i=0;i<N;++i){
	  array[n]=x[i];
	  n++;
	}


	N = fBufferOutLow.GetEntriesFast();

	fBufferOutLow.Draw("x","","goff");
	x= fBufferOutLow.GetV1();
	for(int i=0;i<N;++i){
		array[n]=x[i];
		n++;
	}


	N = fBufferOutUp.GetEntriesFast();

	fBufferOutUp.Draw("x","","goff");
	x= fBufferOutUp.GetV1();
	for(int i=0;i<N;++i){
		array[n]= x[i];
		n++;
	}


}

void TH1FAuto::FillBufferTo(TH1FAuto &h){

	int N;
	double *x;

	N = fBuffer.GetEntriesFast();

	fBuffer.Draw("x","","goff");
	x= fBuffer.GetV1();
	for(int i=0;i<N;++i){
		h.Fill( x[i] );
	}


	N = fBufferOutLow.GetEntriesFast();

	fBufferOutLow.Draw("x","","goff");
	x= fBufferOutLow.GetV1();
	for(int i=0;i<N;++i){
		h.Fill( x[i] );
	}


	N = fBufferOutUp.GetEntriesFast();

	fBufferOutUp.Draw("x","","goff");
	x= fBufferOutUp.GetV1();
	for(int i=0;i<N;++i){
		h.Fill( x[i] );
	}


}

void TH1FAuto::FlushBuffer() {

	//cout<<"FlushBuffer "<<endl;

  if(fIsBuilt) return;


  if(fStrategy==1){

    fBuffer.Draw("x","","goff");
    TH1F *htemp = (TH1F*)gROOT->FindObject("htemp");

    if( !htemp ){
      //Error
      return;
    }
    //htemp->Write();

    TH1F::SetBins(htemp->GetNbinsX(),htemp->GetXaxis()->GetXmin(),htemp->GetXaxis()->GetXmax());

    int N=fBuffer.GetEntriesFast();

    double *x=fBuffer.GetV1();

    for(int i=0;i<N;++i){
      TH1F::Fill(x[i]);
    }

    if(fBufferOutLow.GetEntriesFast() > 0){

      fBufferOutLow.Draw("x","","goff");

      N=fBufferOutLow.GetEntriesFast();

      x=fBufferOutLow.GetV1();

      for(int i=0;i<N;++i){
    	  TH1F::Fill(x[i]);
      }

    }


    if(fBufferOutUp.GetEntriesFast() > 0){

      fBufferOutUp.Draw("x","","goff");

       N=fBufferOutUp.GetEntriesFast();

       x=fBufferOutUp.GetV1();

      for(int i=0;i<N;++i){
    	  TH1F::Fill(x[i]);
      }

    }


  }else{

	  if( !fIsBuilt ){
		  cout<<" !!! ERROR !!!: "<<__FILE__<<"::"<<__FUNCTION__<<" No strategy set to build the Histogram"<<endl;
	  }
	  return;
  }


  ResetBuffer();

  fIsBuilt=true;

  SetStrategy(0);

}




int TH1FAuto::GetBufferSize() const{

  return fBuffer.GetEntriesFast()
    +fBufferOutLow.GetEntriesFast()
    +fBufferOutUp.GetEntriesFast();

}

double TH1FAuto::GetLowRangeLimitX() const{

	return fRangeLimit[0];

}

 double TH1FAuto::GetUpRangeLimitX() const {

	 return fRangeLimit[1];

}

bool TH1FAuto::HasLowRangeLimitX() const{

	return fRangeHasLimit[0];

 }

 bool TH1FAuto::HasUpRangeLimitX() const {

	 return fRangeHasLimit[1];

 }


double TH1FAuto::GetMean(int axis){


	FlushBuffer();

	return TH1F::GetMean(axis);

}

double TH1FAuto::GetRMS(int axis){

	FlushBuffer();

	return TH1F::GetRMS(axis);

}

void TH1FAuto::Reset() {


	if(fIsBuilt){

		TH1F::Reset();

	}else{
		ResetBuffer();
	}

}


void TH1FAuto::ResetBuffer() {

	fBuffer.Clear();
	fBufferOutLow.Clear();
	fBufferOutUp.Clear();

}

void TH1FAuto::SetMaxBufferSize(int val) {

  fMaxBufferSize=val;

}



void TH1FAuto::SetLowRangeLimitX(double val){

  fRangeHasLimit[0]=true;
  fRangeLimit[0]=val;

  if( fRangeHasLimit[1] && fRangeLimit[0]>=fRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH1FAuto::SetRangeLimitsX(double low, double up){

  if(low>=up){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitX(low);
  SetUpRangeLimitX(up);

}



void TH1FAuto::SetStrategy(int val) {

  fStrategy=val;

}



void TH1FAuto::SetUpRangeLimitX(double val){

  fRangeHasLimit[1]=true;
  fRangeLimit[1]=val;

  if( fRangeHasLimit[0] && fRangeLimit[0]>=fRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH1FAuto::fClearRangeLimits() {

  fRangeHasLimit[0]=false;
  fRangeHasLimit[1]=false;

  fRangeLimit[0]=0;
  fRangeLimit[1]=0;


  return;

}



void TH1FAuto::fFillBuffer(double x) {

  if(fIsBuilt) return;


  fX=x;


  if( fRangeHasLimit[0] && x<fRangeLimit[0] ){
    fBufferOutLow.Fill();
  }else if( fRangeHasLimit[1] && x>fRangeLimit[1] ){
    fBufferOutUp.Fill();
  }else{
    fBuffer.Fill();
  }


  if( fMaxBufferSize>0 && GetBufferSize()==fMaxBufferSize ){

    FlushBuffer();

  }


}



void TH1FAuto::fInit(){

  fIsBuilt=false;
  fIsAuto=true;

  SetStrategy(1);
  SetMaxBufferSize(1000);
  fSetBuffer();

  fClearRangeLimits();
}



void TH1FAuto::fSetBuffer() {


  fBuffer.Branch("x",&fX);
  fBufferOutLow.Branch("x",&fX);
  fBufferOutUp.Branch("x",&fX);

}

int TH1FAuto::Write(const char* name, Int_t option, Int_t bufsize) {

	/**
	 *
	 *  Override the TObject method.
	 *  The const version is not yet implemented
	 *  considering that FlushBuffer is not const
	 *
	 *  N.B.:you must provide a name
	 *
	 *
	 */

	FlushBuffer();

	return TH1F::Write(name,option,bufsize);

}

