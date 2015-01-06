/*
 * TSParameter.cc
 *
 *  Created on: May 26, 2014
 *      Author: sdiluise
 */

#include <cmath>

#include <TObject.h>
#include <TMath.h>

#include "RootUtils.h"
#include "TSParameter.h"
#include "TSParamFunction.h"


#include "MessageMgr.h"




TSParameter::TSParameter()
:fPrimaryParamList(0)
{

	Clear();

	Init();
}


TSParameter::TSParameter(TString name, TString  label, TString title, TString unit)
: TSVariable(name,label,title,unit)
,fPrimaryParamList(0)
{

	Clear();

	Init();

}


TSParameter::TSParameter(const TSParameter &other)
:TSVariable()
{

	Clear();

	Init();

	SetName(other.GetName());

	Copy(other);
}


TSParameter::~TSParameter() {

	//cout<<GetName()<<" "<<__FUNCTION__<<" "<<fPrimaryParamList<<endl;
	if(fPrimaryParamList)
		delete fPrimaryParamList;

}


void TSParameter::Clear() {

	//TVariable::Clear();

	fIsFixed=false;
	ClearError();
	ClearAsymErrors();
	SetStep(0.01);
	SetPrecision(1);


	fClearPrimaryParamList();

}


void TSParameter::Copy(const TSParameter &other)
{

	//cout<<__FILE__<<"::"<<__FUNCTION__<<" "<<other.GetName()<<" from "<<GetName()<<endl;

	//cout<<"value "<<other.GetValue()<<" HasValue "<<other.HasValue()<<endl;

	Clear();

	Init();

	TSVariable::Copy(other);

	if( other.HasError() ){
		SetError(other.GetError());
	}

	if( other.HasAsymErrors() ){
		SetAsymErrors(other.GetErrorLow(),other.GetErrorUp());
	}

	SetStep(other.GetStep());
	SetPrecision(other.GetPrecision());

	if( other.IsFixed() ) Fix();

}


TSParameter* TSParameter::Clone(TString name,int a) const{


	cout<<"inside clone method of: "<<GetName()<<endl;

	TSParameter *p=new TSParameter();

	if( !name.IsWhitespace() ) p->SetName(name);
	else p->SetName(GetName());


	p->Copy(*this);


	return p;
}


TObject* TSParameter::Clone(const char* name) const{

	//cout<<"inside TObject clone method of: "<<GetName()<<endl;

	TSParameter *p= this->Clone("name",0);

	return (TObject*)p;

}


void TSParameter::Init() {

	SetStep(0.01);
	SetPrecision(1);

	fBuildPrimaryParamList();

}


void TSParameter::ClearAsymErrors() {

	fErrLow=0;
	fErrUp=0;

    fHasAsymErrors=false;
}



void TSParameter::ClearError() {

	fError=0;
	fHasError=false;

}


void TSParameter::Fix() {

	fIsFixed=true;
}


double TSParameter::GetErrorFrac() const {

	double r=0;

	if( !RootUtils::IsZero(GetValue()) && !RootUtils::IsZero(GetError())){
		r=GetError()/GetValue();
		if(TMath::IsNaN(r)) r=0;
	}

	return r;
}


TSArgList* TSParameter::GetPrimaryParamList() const {

	return fPrimaryParamList;

}



void TSParameter::Print() const {

  cout<<GetName()<<": "<<GetDescription()<<" value: "<<GetValue();
  if( HasError() ) cout<<" +/-"<<GetError();
  if( HasAsymErrors() ) cout<<" asymm: "<<GetErrorLow()<<" "<<GetErrorUp();
  if( IsFixed() ) cout<<" (fixed) ";
  cout<<endl;

}

void TSParameter::PrintPrimaryParamList() const {

	MSG::LOG("--- ",__FUNCTION__," for param: ",GetName()," ---");

	TObject *this_obj = (TObject*)(this);
	//TSParameter *this_par=(TSParameter*)(this);
	//TSParamFunction *this_parf=(TSParamFunction*)(this);


	//TObject *this_obj = static_cast<TObject*>(this);
	TSParameter *this_par=static_cast<TSParameter*>(this_obj);
	TSParamFunction *this_parf=static_cast<TSParamFunction*>(this_obj);


	//cout<<this_obj<<" "<<this_par<<" "<<this_parf<<endl;


	if( fPrimaryParamList ){

		fPrimaryParamList->Print(); //print only name not values as it can store any obj

		cout<<"num of pars: "<<GetNumOfPrimPars()<<" value of the function: "<<GetValue()<<endl;

		for( int i=0; i<GetNumOfPrimPars(); ++i){

			TObject *obj = fPrimaryParamList->At(i);

			TSParameter *par=static_cast<TSParameter*>(fPrimaryParamList->At(i));
			TSParamFunction *parf=static_cast<TSParamFunction*>(fPrimaryParamList->At(i));

			if(par==0) continue;
			cout<<i<<" "<<par->GetName()<<" val: "<<par->GetValue()<<"; "<<obj<<" "<<par<<" "<<parf<<endl;
		}


	}else{
		MSG::WARNING(__FILE__"::",__FUNCTION__," list not initialized");
		return;
	}

	cout<<" ----- "<<endl;

}

void TSParameter::Release() {

	fIsFixed=false;

}



void TSParameter::SetAsymErrors(double err_low, double err_up) {

    /**
     *
     * Errors must be: lower negative and upper positive OR equals and positive
     *  otherwise they are set to zero
     *
     * */


	if( !(
		    (err_low<0 && err_up>0)
			          ||
		   ((err_low==err_up)&&(err_low>=0))
          )
	    ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," Illegal Asymmetric Errors: ",err_low," ",err_up);
		MSG::WARNING(" Param: ",GetName());

		fErrLow=0;
		fErrUp=0;
		fHasAsymErrors=false;

        return;
	}

	fErrLow=err_low;
	fErrUp=err_up;

    fHasAsymErrors=true;

}



void TSParameter::SetError(double e) {

	fError=abs(e);

	fHasError=true;

	//SetAsymErrors(fError,fError);
}



void TSParameter::SetFixed(double v) {


	SetValue(v);

	Fix();

}



void TSParameter::SetPrecision(double p) {

	fPrec = p;

}

void TSParameter::SetStep(double s) {

	fStep = s;

}


void TSParameter::fBuildPrimaryParamList() {


	if( fPrimaryParamList ) delete fPrimaryParamList;

	TObject *o = (TObject*)(this);

	//if I am a primary param add myself to the list for my own primary params
	fPrimaryParamList= new TSArgList( *o );

	//cout<<"====> "<<GetName()<<" "<<__FILE__<<" "<<__FUNCTION__<<" "<<fPrimaryParamList<<" "<<fPrimaryParamList->GetSize()<<endl;

}


void TSParameter::fClearPrimaryParamList() {

	if( fPrimaryParamList ) fPrimaryParamList->Clear();

}
