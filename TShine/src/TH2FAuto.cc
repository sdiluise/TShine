/*
 * TH1FAuto.cc
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */

#include <iostream>

#include <TROOT.h>
#include <TH2F.h>

#include "RootUtils.h"

#include "TH2FAuto.h"


using std::cout;
using std::endl;

TH2FAuto::TH2FAuto()
  : TH2F("","",1,0.,1.,1,0.,1.)
{

	fInit();
}

TH2FAuto::TH2FAuto(const char* name, const char *title)
  : TH2F(name,title,1,0.,1.,1,0.,1.)
{

  fInit();


}



TH2FAuto::TH2FAuto(const char* name, const char *title, double xlow, double xup, double ylow, double yup)
  : TH2F(name,title,1,0.,1.,1,0.,1.)
{


  fInit();

  SetRangeLimits(xlow,xup,ylow,yup);

}


TH2FAuto::TH2FAuto(const char* name, const char *title, int nx, double xmin, double xmax, int ny, double ymin, double ymax)
  : TH2F(name,title,nx,xmin,xmax,ny,ymin,ymax)
{


  fInit();

  fIsBuilt=true;
  fIsAuto=false;

}


TH2FAuto::TH2FAuto(const char* name, const char *title, int nx, double* binsx, int ny, double* binsy)
  : TH2F(name,title,nx,binsx,ny,binsy)
{


  fInit();

  fIsBuilt=true;
  fIsAuto=false;
}



TH2FAuto::TH2FAuto(const TH2FAuto &other) {


	TH2FAuto::Copy(other);

}


void TH2FAuto::Copy( const TH2FAuto &other ){

	if(other.IsBuilt() ){

		TH2F::Copy( *(TH2F*)&other );

	}else{


		fInit();

		if(other.IsBinningXFix() ){

			FixBinningX(other.GetFixedNbinsX(),other.GetFixedBinsArrayX());

		}else{


			if( other.HasLowRangeLimitX() ) {
					SetLowRangeLimitX( other.GetLowRangeLimitX() );
			}

			if( other.HasUpRangeLimitX() ) {
				SetUpRangeLimitX( other.GetUpRangeLimitX() );
			}

		}

		if(other.IsBinningYFix() ){

			FixBinningY(other.GetFixedNbinsY(),other.GetFixedBinsArrayY());


		}else{

			if( other.HasLowRangeLimitY() ) {
				SetLowRangeLimitY( other.GetLowRangeLimitY() );
			}

			if( other.HasUpRangeLimitY() ) {
				SetUpRangeLimitY( other.GetUpRangeLimitY() );
			}

		}


		SetTitle(other.GetTitle());
		GetXaxis()->SetTitle(  other.GetXaxis()->GetTitle() );
		GetYaxis()->SetTitle(  other.GetYaxis()->GetTitle() );


	}//IsNotBuilt






}


TObject* TH2FAuto::Clone(const char* newname) const{

	//cout<<"OK CLONING2: "<<__FUNCTION__<<" "<<GetName()<<endl;

	TH2FAuto *h=new TH2FAuto(*this);

	h->SetName(newname);

	return h;


}


TH2FAuto::~TH2FAuto() {



}


int TH2FAuto::BufferEmpty(int action){



	if(action==4){
		//cout<<"calling BufferEmpty: "<<action<<endl;
		FlushBuffer();
		return 1;
	}

	//cout<<"calling BufferEmpty: "<<action<<endl;
	return TH2F::BufferEmpty();
}



void TH2FAuto::Draw(Option_t* opt){

     FlushBuffer();

     TH2F::Draw(opt);
}


int TH2FAuto::Fill(double x, double y) {


  if( !fIsBuilt ){

    fFillBuffer(x,y);

    return GetBufferSize();

  }else{

    return TH2F::Fill(x,y);
  }


}

int TH2FAuto::FillBufferTo(TH2FAuto &h){


	int N;
	double *x,*y;

	N = fBuffer.GetEntriesFast();

	fBuffer.Draw("x:y","","goff");
	x= fBuffer.GetV1();
	y= fBuffer.GetV2();

	for(int i=0;i<N;++i){
		h.Fill( x[i], y[i] );
	}


	N = fBufferOut.GetEntriesFast();

	fBufferOut.Draw("x:y","","goff");
	x= fBufferOut.GetV1();
	y= fBufferOut.GetV2();

	for(int i=0;i<N;++i){
		h.Fill( x[i],y[i] );
	}


	return 1;
}



void TH2FAuto::FixBinningX(int n, double low, double up){

	fFixBinningX=true;

	fFixNumOfBins[0]=n;
	fFixLimitsX[0]=low;
	fFixLimitsX[1]=up;

	//fFixedAxisX.Set(n,low,up);

	double *xbins=RootUtils::CreateBinsArray(n,low,up);

	fFixedAxisX.Set(n,xbins);

	delete [] xbins;

	if(fFixBinningY){
		FlushBuffer();
	}

}


void TH2FAuto::FixBinningX(int n, const double* x){

	fFixBinningX=true;

	fFixNumOfBins[0]=n;
	fFixLimitsX[0]=x[0];
	fFixLimitsX[1]=x[n];

	fFixedAxisX.Set(n,&x[0]);

	fFixAxisVariableBins[1]=true;

	if(fFixBinningY){
		FlushBuffer();
	}

}



void TH2FAuto::FixBinningY(int n, double low, double up){

	fFixBinningY=true;

	fFixNumOfBins[1]=n;
	fFixLimitsY[0]=low;
	fFixLimitsY[1]=up;

	fFixedAxisY.Set(n,low,up);

	double *xbins=RootUtils::CreateBinsArray(n,low,up);

	fFixedAxisY.Set(n,xbins);

	delete [] xbins;


	if(fFixBinningX){
		FlushBuffer();
	}

}


void TH2FAuto::FixBinningY(int n, const double* x){

	fFixBinningY=true;

	fFixNumOfBins[1]=n;
	fFixLimitsY[0]=x[0];
	fFixLimitsY[1]=x[n];

	fFixedAxisY.Set(n,&x[0]);

	fFixAxisVariableBins[1]=true;


	if(fFixBinningX){
		FlushBuffer();
	}

}


void TH2FAuto::FlushBuffer() {

  if(fIsBuilt) return;

  if( fFixBinningX && fFixBinningY ){


	  //TH2F::SetBins(fFixNumOfBins[0],fFixLimitsX[0],fFixLimitsX[1]
	  //             ,fFixNumOfBins[1],fFixLimitsY[0],fFixLimitsY[1]);
	  TH2F::SetBins(fFixNumOfBins[0],GetFixedBinsArrayX()
	  	                ,fFixNumOfBins[1],GetFixedBinsArrayY());


  } else
  if(fStrategy==1){

    fBuffer.Draw("y:x","","goff"); //N.B.:the first variable is plotted on the y axis
    TH2F *htemp = (TH2F*)gROOT->FindObject("htemp");

    if( !htemp ){
      //Error
      return;
    }

    //cout<<endl<<"htemp "<<endl;
    //cout<<htemp->GetNbinsX()<<" "<<htemp->GetXaxis()->GetXmin()<<" "<<htemp->GetXaxis()->GetXmax()<<endl;
    //cout<<htemp->GetNbinsY()<<" "<<htemp->GetYaxis()->GetXmin()<<" "<<htemp->GetYaxis()->GetXmax()<<endl;

    if( fFixBinningX ){

    	//TH2F::SetBins(fFixNumOfBins[0],fFixLimitsX[0],fFixLimitsX[1]
    	//    			,htemp->GetNbinsY(),htemp->GetYaxis()->GetXmin(),htemp->GetYaxis()->GetXmax());

    	TH2F::SetBins(fFixNumOfBins[0],GetFixedBinsArrayX()
    	     			,htemp->GetNbinsY()
    	     			,RootUtils::CreateBinsArray(htemp->GetNbinsY(),htemp->GetYaxis()->GetXmin(),htemp->GetYaxis()->GetXmax()) );


    }else if(fFixBinningY){

    	//TH2F::SetBins(htemp->GetNbinsX(),htemp->GetXaxis()->GetXmin(),htemp->GetXaxis()->GetXmax()
    	 //   			,fFixNumOfBins[1],fFixLimitsY[0],fFixLimitsY[1]);

    	TH2F::SetBins(htemp->GetNbinsX(),RootUtils::CreateBinsArray(htemp->GetNbinsX(),htemp->GetXaxis()->GetXmin(),htemp->GetXaxis()->GetXmax())
    	    	    			,fFixNumOfBins[1],GetFixedBinsArrayY());

    	//cout<<GetName()<<endl;
    	//cout<<"fulsh fix Y: "<<fFixNumOfBins[1]<<" "<<fFixLimitsY[0]<<" "<<fFixLimitsY[1]<<endl;
    	//cout<<"auto x: "<<htemp->GetNbinsX()<<" "<<htemp->GetXaxis()->GetXmin()<<" "<<htemp->GetXaxis()->GetXmax()<<endl;

    }else{

    	TH2F::SetBins(htemp->GetNbinsX(),htemp->GetXaxis()->GetXmin(),htemp->GetXaxis()->GetXmax()
    			,htemp->GetNbinsY(),htemp->GetYaxis()->GetXmin(),htemp->GetYaxis()->GetXmax());
    }

  }else{

	  if( !fIsBuilt ){
		  cout<<" !!! ERROR !!!: "<<__FILE__<<"::"<<__FUNCTION__<<" No strategy set to build the Histogram: "<<GetName()<<endl;
	  }
	  return;
  }

  //
  // Now fill the histo with data from buffers
  //
  int N=fBuffer.GetEntriesFast();

  //N.B.: the first variable is plotted on the y axis
  double *y=fBuffer.GetV1();
  double *x=fBuffer.GetV2();

  for(int i=0;i<N;++i){
	  TH2F::Fill(x[i],y[i]);
  }


  if(fBufferOut.GetEntriesFast() > 0){

	  fBufferOut.Draw("y:x","","goff");//N.B.: the first variable is plotted on the y axis

      N=fBufferOut.GetEntriesFast();

      //N.B.: the first variable is plotted on the y axis
      y=fBufferOut.GetV1();
      x=fBufferOut.GetV2();

      for(int i=0;i<N;++i){
    	  TH2F::Fill(x[i],y[i]);
      }

  }

    
   ResetBuffer();

  fIsBuilt=true;

  SetStrategy(0);

}



int TH2FAuto::GetBufferSize() {

  return fBuffer.GetEntriesFast()+fBufferOut.GetEntriesFast();

}



int TH2FAuto::GetFixedNbinsX() const{

	return fFixedAxisX.GetNbins();
}

const double* TH2FAuto::GetFixedBinsArrayX() const{

	if(fFixedAxisX.GetXbins())
		return fFixedAxisX.GetXbins()->GetArray();
}


int TH2FAuto::GetFixedNbinsY() const{

	return fFixedAxisY.GetNbins();


}


const double* TH2FAuto::GetFixedBinsArrayY() const{

	if(fFixedAxisY.GetXbins())
				return fFixedAxisY.GetXbins()->GetArray();
}




double TH2FAuto::GetLowRangeLimitX() const{
	return fXRangeLimit[0];
}
double TH2FAuto::GetLowRangeLimitY() const{
	return fYRangeLimit[0];
}
double TH2FAuto::GetUpRangeLimitX() const{
	return fXRangeLimit[1];
}
double TH2FAuto::GetUpRangeLimitY() const{
	return fYRangeLimit[1];
}
bool TH2FAuto::HasLowRangeLimitX() const{
	return fXRangeHasLimit[0];
}
bool TH2FAuto::HasLowRangeLimitY() const{
	return fYRangeHasLimit[0];
}
bool TH2FAuto::HasUpRangeLimitX() const{
	return fXRangeHasLimit[1];
}
bool TH2FAuto::HasUpRangeLimitY() const{
	return fYRangeHasLimit[1];
}

double TH2FAuto::GetMean(int axis) {

	FlushBuffer();

	return TH2F::GetMean(axis);
}

double TH2FAuto::GetRMS(int axis) {


	FlushBuffer();

	return TH2F::GetRMS(axis);

}


void TH2FAuto::Reset() {

	if(fIsBuilt){

		TH2F::Reset();

	}else{

		ResetBuffer();
	}

}


void TH2FAuto::ResetBuffer() {

	  fBuffer.Clear();
	  fBufferOut.Clear();

}



void TH2FAuto::SetMaxBufferSize(int val) {

  fMaxBufferSize=val;

}



void TH2FAuto::SetLowRangeLimitX(double val){
  
  fXRangeHasLimit[0]=true;
  fXRangeLimit[0]=val;
  
  if( fXRangeHasLimit[1] && fXRangeLimit[0]>=fXRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::SetLowRangeLimitY(double val){
  
  fYRangeHasLimit[0]=true;
  fYRangeLimit[0]=val;
  
  if( fYRangeHasLimit[1] && fYRangeLimit[0]>=fYRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}


void TH2FAuto::SetRangeLimits(double xlow, double xup, double ylow, double yup){
  
  if(xlow>=xup || ylow>=yup){
    fClearRangeLimits();
    return;
  }

  SetRangeLimitsX(xlow,xup);
  SetRangeLimitsY(ylow,yup);
  
}



void TH2FAuto::SetRangeLimitsX(double xlow, double xup){
  
  if(xlow>=xup){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitX(xlow);
  SetUpRangeLimitX(xup);
  
}



void TH2FAuto::SetRangeLimitsY(double ylow, double yup){
  
  if(ylow>=yup){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitY(ylow);
  SetUpRangeLimitY(yup);
  
}



void TH2FAuto::SetStrategy(int val) {

  fStrategy=val;

}



void TH2FAuto::SetUpRangeLimitX(double val){
  
  fXRangeHasLimit[1]=true;
  fXRangeLimit[1]=val;

  if( fXRangeHasLimit[0] && fXRangeLimit[0]>=fXRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::SetUpRangeLimitY(double val){
  
  fYRangeHasLimit[1]=true;
  fYRangeLimit[1]=val;

  if( fYRangeHasLimit[0] && fYRangeLimit[0]>=fYRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::fClearRangeLimits() {
  
  fXRangeHasLimit[0]=false;
  fXRangeHasLimit[1]=false;
  fYRangeHasLimit[0]=false;
  fYRangeHasLimit[1]=false;

  fXRangeLimit[0]=0;
  fXRangeLimit[1]=0;
  fYRangeLimit[0]=0;
  fYRangeLimit[1]=0;

    
  return;

}



void TH2FAuto::fFillBuffer(double x, double y) {

  if(fIsBuilt) return;


  fX=x;
  fY=y;


  if(    (fXRangeHasLimit[0] && x<fXRangeLimit[0]) 
      || (fXRangeHasLimit[1] && x>fXRangeLimit[1])
      || (fYRangeHasLimit[0] && y<fYRangeLimit[0])
      || (fYRangeHasLimit[1] && y>fYRangeLimit[1])

      ){

    fBufferOut.Fill();

  }else{

    fBuffer.Fill();
  }


  if( fMaxBufferSize>0 && GetBufferSize()==fMaxBufferSize ){

    FlushBuffer();

  }


}



void TH2FAuto::fInit(){

  fIsBuilt=false;
  fIsAuto=true;

  fFixBinningX=false;
  fFixBinningY=false;
  fFixAxisVariableBins[0]=false;
  fFixAxisVariableBins[1]=false;

  SetStrategy(1);
  SetMaxBufferSize(1000);
  fSetBuffer();

  fClearRangeLimits();

  fX=0;
  fY=0;

}



void TH2FAuto::fSetBuffer() {


  fBuffer.Branch("x",&fX);
  fBuffer.Branch("y",&fY);

  fBufferOut.Branch("x",&fX);
  fBufferOut.Branch("y",&fY);


}



int TH2FAuto::Write(const char* name, Int_t option, Int_t bufsize){

	/**
		 *
		 *  Override the TObject method.
		 *  The const version is not yet implemented
		 *  considering that FlushBuffer is not const
		 *
		 *
		 */

		FlushBuffer();

		return TH2F::Write(name,option,bufsize);

}

