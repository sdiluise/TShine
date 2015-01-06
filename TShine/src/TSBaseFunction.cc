/*
 * TSBaseFunction.cc
 *
 *  Created on: Aug 3, 2014
 *      Author: Silvestro di Luise
 *      		Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <cassert>

#include <TMath.h>

#include "MessageMgr.h"

#include "TSBaseFunction.h"


double TSBaseFunction::GNorm=2.50662827463100024; //sqrt(2*Pi)=2.50662827463100024

TSBaseFunction::TSBaseFunction()
:TSNamed()
,fPrimaryParamList(0)
{

	Init();

}


TSBaseFunction::TSBaseFunction(TString name, TString title)
:TSNamed(name,name,title)
,fPrimaryParamList(0)
{

	Init();

}


TSBaseFunction::~TSBaseFunction() {


}



void TSBaseFunction::Init() {

	//MSG::LOG(__FILE__,"::",__FUNCTION__);

	TString name="ParSet_"; name+=GetName();

	fParamSet.SetName( name );

	fBuildPrimaryParamList();

	fNDim=0;
	SetNPar(0);


	//don't use setters first time as
	//Area is calculated as the product
	//at each settings of one of the two widths
	fBinWidthX=1;
	fBinWidthY=1;
	fBinArea=1;

	return;
}


int TSBaseFunction::AddParameter(const TSParameter &p){

	/*
	 *
	 *  To Implem
	 *
	 *  AddParameters(TSArgList)
	 *
	 *  AddPrimaryParameter, AddPrimaryParameters
	 *
	 *
	 */


	//
	//check duplicates
	//
	bool isnew=true;

	for(int i=0; i<fParamSet.GetSize(); ++i){

	   if( fParamSet.At(i) == (TObject*)(&p) ){
		   isnew=false;
		   break;
	   }

	}

	if( isnew ){

		TSParameter *obj_par = (TSParameter*)&p;

		fParamSet.Add( obj_par );

		if( fParamSet.GetSize()<=NMAX_FUNC_PARAMS ){
			fParamArray[fParamSet.GetSize()-1]=&p;
		}else{
			MSG::WARNING(__FILE__,"::",__FUNCTION__," param ", obj_par->GetName(), " not added to plain array, do increase size limit");
		}


		//
		//check dependencies
		//

		fUpdatePrimaryParamList(p);

		//if(  dynamic_cast<TSParamFunction*>(obj_par) ){
	    //
		//}

	}


	SetNPar( fParamSet.GetSize() );

	return 1;


}


const TSParameter* TSBaseFunction::GetParameter(int ipar) const{

	if(ipar<0 || ipar>NPar() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," param index outside range: ",ipar," in function: ",GetName());
		assert(0);
		//return 0;
	}


	return fParamArray[ipar];


}

const TSParameter* TSBaseFunction::GetParameter(TString name) const{


	TObject *o = (TObject*)fParamSet.FindObject(name);

	if( !o ) return 0;

	TSParameter *p = (TSParameter*)o;

	return p;

}


void TSBaseFunction::PrintParamList() const{

	MSG::LOG(GetName()," ",__FUNCTION__);

	for(int i=0; i<NPar();++i){

		cout<<" "<<i<<" "<<GetParameter(i)->GetName()<<" val: "<<GetParameter(i)->GetValue()<<endl;
	}


	fParamSet.Print();

	cout<<" ------- "<<endl;
}


void TSBaseFunction::PrintPrimaryParamList() const{


	MSG::LOG("-- ",__FUNCTION__," for function: ",GetName());

	if( fPrimaryParamList ){

		fPrimaryParamList->Print(); //print only name not values as it can store any obj

		cout<<"num of pars: "<<GetNumOfPrimPars()<<endl;

		for( int i=0; i<GetNumOfPrimPars(); ++i){

			TSParameter *par=dynamic_cast<TSParameter*>(fPrimaryParamList->At(i));
			if(par==0) continue;
			cout<<i<<" "<<par->GetName()<<" val: "<<par->GetValue()<<endl;
		}


	}else{
		MSG::WARNING(__FILE__"::",__FUNCTION__," list not initialized");
		return;
	}

	cout<<" --"<<endl;



}

void TSBaseFunction::ResetBinWidths() const{

	SetBinWidths(1.,1.);

}

void TSBaseFunction::SetBinWidths(double bx,double by) const{

	SetBinWidthX(bx);
	SetBinWidthY(by);
}


void TSBaseFunction::SetBinWidths(const TH1 &h) const{

	double bx=h.GetXaxis()->GetBinWidth(1);
	double by=h.GetYaxis()->GetBinWidth(1); //1 for TH1


	cout<<__FUNCTION__<<" "<<h.GetName()<<" "<<bx<<" "<<by<<endl;

	SetBinWidthX(bx);
	SetBinWidthY(by);
}


void TSBaseFunction::SetBinWidthX(double b) const{

	if( b<=0 || TMath::IsNaN(b) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," illegal bin width: ",b);
		return;
	}

	fBinWidthX=b;

	fBinArea=fBinWidthY*fBinWidthX;
}

void TSBaseFunction::SetBinWidthY(double b) const{

	if( b<=0 || TMath::IsNaN(b) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," illegal bin width: ",b);
		return;
	}

	fBinWidthY=b;

	fBinArea=fBinWidthY*fBinWidthX;
}


void TSBaseFunction::SetNDim(int n){

	if( !(n==1||n==2) ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__, " dim = ",n);
		assert(0);

	}

	fNDim=n;

}



void TSBaseFunction::SetNPar(int n){

	if( n<0 ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__, " par = ",n);
		assert(0);
	}

	fNPar=n;

}






void TSBaseFunction::fClearPrimaryParamList(){


	if(fPrimaryParamList) fPrimaryParamList->Clear();

}



void TSBaseFunction::fBuildPrimaryParamList(){

	fPrimaryParamList= new TSArgList();

}


void TSBaseFunction::fUpdatePrimaryParamList(const TSParameter &new_par){


	if( GetPrimaryParamList() ){

		TSArgList *list=new_par.GetPrimaryParamList();

		//cout<<"Add new pars: "<<list->GetSize()<<endl;
		GetPrimaryParamList()->AddOnce( *list );
	}


}


