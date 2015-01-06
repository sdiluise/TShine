/*
 * TSParamFunction.cc
 *
 *  Created on: Jun 13, 2014
 *      Author: sdiluise
 */

#include <cassert>
#include <cmath>

#include <TMath.h>
#include "StringUtils.h"

#include "TSParamFunction.h"

#define DEBUG 0
#if DEBUG
#define D(x) std::cout<<x<<std::endl;
#else   D(x)
#endif


TSParamFunction::TSParamFunction() {

	Init();

}

TSParamFunction::TSParamFunction(TString name, TString label, TString title, TString unit)
:TSParameter(name,label,title,unit)
{
	Init();



}

TSParamFunction::TSParamFunction(const TSParamFunction &other){


   Copy(other);


}


TSParamFunction::~TSParamFunction() {
	// TODO Auto-generated destructor stub

	fParamList.clear();

	delete fFunction;


}



void TSParamFunction::Copy(const TSParamFunction &other) {

	Init();

	TSParameter::Copy(other);

	SetFunction(other.GetFormula(),*other.GetPrimaryParamList());

}


TSParamFunction* TSParamFunction::Clone(TString name) const{

	TSParamFunction *p=new TSParamFunction();

	if( !name.IsWhitespace()) p->SetName(name);
	else p->SetName(GetName());


	p->Copy(*this);

	TSArgList *list = GetPrimaryParamList()->Clone();

	p->SetFunction(GetFormula(),*list);


	return p;

}

double TSParamFunction::Evaluate() const{


	//const_cast TF1 to be able to use
	//SetParameter
	TF1 *f = (TF1*)fFunction;

	fValue=0;

	for(int ip=1; ip<=GetNumOfPars(); ++ip){

		TSParameter *par=GetParam(ip);

		//fFunction->SetParameter(ip-1, par->GetValue() );
		//fFunction->SetParameter(ip-1, fCachedParams[ip-1] );
		f->SetParameter(ip-1, fCachedParams[ip-1] );

		//cout<<GetName()<<" calling: "<<__FUNCTION__<<" par: "<<ip<<" "<<par->GetName()<<" = "<<par->GetValue()<<" "<<fCachedParams[ip-1]<<endl;


	}

	//fValue = fFunction->Eval(0);
	fValue = f->Eval(0);

	if( TMath::IsNaN(fValue) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Evaluation Is NaN in ",GetName());
		f->Print();
		assert(0);
	}


	return fValue;

}



double TSParamFunction::GetError(double *cov) const{

	/**
	 *
	 *  Calculate error considering
	 *  errors and correlation between parameters
	 *  provided through the covariance matrix
	 *
	 *
	 */

	return 0;
}



double TSParamFunction::GetError() const{

	/**
	 *
	 * TF1 (fFunction) params should not be set as fixed
	 * otherwise the gradient wrt that param will be computed as 0.
	 *
	 *
	 */

	// would be necessary to const_cast the function?
	//
	// use TF1 *ftemp=(TF1*)fFunction;

	if( !fFunction ) return 0;


	double err=0;

	double x[2];
	x[0]=1;

	for(int ip=1; ip<=GetNumOfPars(); ++ip){

		double p_err = GetParam(ip)->GetError();

		double p_deriv=fFunction->GradientPar(ip-1,x);

		err += p_deriv*p_deriv*p_err*p_err;
	}

	err=sqrt(err);

	return err;
}



TSParameter* TSParamFunction::GetParam(int ip) const{


	if( fParamList.count(ip)==1){

		return fParamList.find(ip)->second;

	}else{

		MSG::WARNING(__FILE__,"::",__FUNCTION__, " param index out of range: ",ip);
		return 0;
	}

}



double TSParamFunction::GetValue() const{

	//cout<<"TSParamFunction::GetValue() "<<" from: "<<GetName()<<endl;


	for(int ip=1; ip<=GetNumOfPars(); ++ip){

		TSParameter *par=GetParam(ip);

		fCacheParam(ip);

	}


	return Evaluate();

}


double TSParamFunction::GetValue2() {

	/**
	 *
	 Function is re-evaluate at each call
	 even if  the values of the dependence
	 parameters have not changed meanwhile
	 This can effect performance.
	 Enable variable status tracking should
	 solve this


	 */

	if( !fFunction ) return 0;


	cout<<"BB "<<BroadcastActive()<<" any: "<<AnyServerChanged()<<" num: "<<GetNumOfServersChanged()<<" tot: "<<GetNumOfServers()<<endl;

	if( !BroadcastActive() ){

		fCacheParams();

		return Evaluate();

	}


	if( BroadcastActive() ){


		if( !AnyServerChanged() ){
			return fValue;
		}

		for(int ip=1; ip<=GetNumOfPars(); ++ip){

			TSParameter *par=GetParam(ip);

			if( ServerChanged(par) ){

				fCacheParam(ip);

				SetServerStatus(par,ServerClientMgr::kUnChanged);

			}else if( ServerRemoved(par) ){

				RemoveServer(par);
			}

		}//ip

		return Evaluate();

	}//



}



void TSParamFunction::Init(){


	fNumOfPars=0;

	fExpr="";
	fTF1expr="";

	fFunction=0;

    fValue=0;

    SetValue(0);
    SetError(0);
    SetAsymErrors(0,0);

    fBuildPrimaryParamList();
}



void TSParamFunction::Print() const{

	MSG::LOG("--- ",__FUNCTION__," ",GetName()," --- ");
	cout<<GetFormula()<<endl;
	cout<<GetTF1Formula()<<endl;
	cout<<" value: "<<GetValue()<<endl;

	if( fFunction ){

		for(int i=0;i<fFunction->GetNpar();++i){
			cout<<i<<" "<<fFunction->GetParName(i)<<" = "<<fFunction->GetParameter(i)<<" +/- "<<fFunction->GetParError(i)<<endl;
		}

	}else{

	}

	cout<<" "<<endl;
	cout<<"Primary Param List: "<<endl;
	if(GetPrimaryParamList()) GetPrimaryParamList()->PrintVariables();


	cout<<" --- "<<endl;
}



void TSParamFunction::SetAsymErrors(double low, double up){

	TSParameter::SetAsymErrors(low,up);

}


void TSParamFunction::SetError(double e){

	TSParameter::SetError(e);

}


bool TSParamFunction::SetFunction(TString expr){


	/**
	* param names must be ordered in decreasing length
	* so as to avoid that names containig a substring which mach
	* a shortest name of another param are spoiled
	* TF1 Par numbers are ordered the same way they appear in the param list
	*
	*/

	MSG::LOG(__FILE__,"::",__FUNCTION__," Name: ",GetName());

	int np = GetNumOfPars();

	if( np == 0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," no parameters are set ");
		return 0;
	}



	std::vector<TString> par_names;

	std::map<TString,int> name_2_parid;

	for(int ip=1; ip<=np; ++ip){

		par_names.push_back( GetParam(ip)->GetName() );

		name_2_parid[par_names[ip-1]]=ip-1;

		//cout<<ip<<" "<<par_names[ip-1]<<" "<<name_2_parid[par_names[ip-1]]<<endl;
	}

	std::sort(par_names.begin(),par_names.end(),STR_UTILS::CompLengthDecr);

	for(int ip=1; ip<=np; ++ip){
		//cout<<ip<<" "<<par_names[ip-1]<<" "<<name_2_parid[par_names[ip-1]]<<endl;
	}




	fExpr=expr;
	fTF1expr=expr;



	int func_np=0;

	for(int ip=1; ip<=np; ++ip){

		TString par_name= par_names[ip-1];

		int par_id = name_2_parid[par_name];

		if( fTF1expr.Contains( par_name) ){

			TString par_p="["; par_p+=par_id; par_p+="]";

			fTF1expr.ReplaceAll(par_name,par_p);

			func_np++;

			cout<<"TSParamFunction::SetFunction: "<<par_name<<" --> "<<par_p<<endl;

		}else{

			MSG::ERROR(__FILE__,"::",__FUNCTION__," param ",par_name, " not present in the expression ");
			MSG::ERROR(fTF1expr);
			return 0;
		}

	}

	cout<<" Param Function: "<<Name()<<" = "<<fExpr<<endl;

	fTF1expr+="+x";

	cout<<" TF1 Expression: "<<fTF1expr<<endl;

	if( func_np != np){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," not enough params in the expression ");
		return 0;
	}


	fBuildDescription();

	fBuildTF1();

	GetValue2();

	cout<<" --Function Set Exit-- "<<endl;
	return 1;
}



bool TSParamFunction::SetFunction(TString expr, const TSArgList &set){


	if( set.GetSize()==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," empty list");
		return false;
	}


	fClearParamList();


	for(int i=0; i<set.GetSize();++i){

		SetParameter( *(TSParameter*)set.At(i) );
	}

	return SetFunction(expr);

}



bool TSParamFunction::SetFunction(TString expr,TSParameter& p1){

	SetParameter(p1);

	return SetFunction(expr);


}



bool TSParamFunction::SetFunction(TString expr, TSParameter& p1, TSParameter& p2){

	SetParameter(p1);
	SetParameter(p2);

	return SetFunction(expr);
}



bool TSParamFunction::SetFunction(TString expr, TSParameter& p1, TSParameter& p2, TSParameter& p3){

	SetParameter(p1);
	SetParameter(p2);
	SetParameter(p3);

	return SetFunction(expr);
}



bool TSParamFunction::SetFunction(TString expr,TSParameter& p1,TSParameter& p2,TSParameter& p3, TSParameter& p4){

	SetParameter(p1);
	SetParameter(p2);
	SetParameter(p3);
	SetParameter(p4);

	return SetFunction(expr);
}



bool TSParamFunction::SetParameter(TSParameter& par){


	if(fNumOfPars == NMAX_CACHED_PARS){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," param ",par.Name(), " cannot be set, enlarge cache param cached sise");
		return 0;
	}

	for(int ip=1; ip<=GetNumOfPars(); ++ip){

		if( GetParam(ip) == &par){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," param ",par.Name(), " already set");
			return 0;
		}

	}


	par.AddClient(this);
	AddServer(&par);


	fNumOfPars++;

	fParamList[fNumOfPars] = &par;

	fUpdatePrimaryParamList(par);

	return 1;

}



void TSParamFunction::SetValue(double v) {

	cout<<"TSParamFunction::SetValue "<<GetName()<<" "<<v<<endl;
	TSParameter::SetValue(v);

}


void TSParamFunction::fBuildDescription(){


	TString fDes = fExpr;

	for(int ip=1; ip<=GetNumOfPars(); ++ip){

		TString par_name= GetParam(ip)->Name();

		if( fDes.Contains( par_name) ){

			TString par_label=GetParam(ip)->GetLabel();

			fDes.ReplaceAll(par_name,par_label);
		}

	}

	fDescription = fDes.Data();


}



void TSParamFunction::fBuildTF1(){


	if( fFunction ){
		delete fFunction;
		fFunction=0;
	}

	fFunction = new TF1(Name(),fTF1expr);


	for(int ip=1; ip<=GetNumOfPars();++ip){

		fFunction->SetParName(ip-1, GetParam(ip)->GetName() );
	}

	//fFunction->Print();


}


void TSParamFunction::fCacheParam(int ip) const{

	TSParameter *p=GetParam(ip);

	if( !p ) return;

	fCachedParams[ip-1]=p->GetValue();

}

void TSParamFunction::fCacheParams() const{

	for(int ip=1; ip<=GetNumOfPars();++ip){

		TSParameter *p=GetParam(ip);

		if( !p ) return;

		fCachedParams[ip-1]=p->GetValue();

	}

}


void TSParamFunction::fClearParamList(){


	fNumOfPars=0;

	fParamList.clear();

}


void TSParamFunction::fUpdatePrimaryParamList(const TSParameter &new_par) {


	//cout<<"from "<<GetName()<<" "<<__FILE__<<" "<<__FUNCTION__<<" "<<new_par.GetName()<<" "<<GetPrimaryParamList()<<" "<<GetNumOfPrimPars()<<endl;
	//GetPrimaryParamList()->Print();

	if( GetPrimaryParamList() ){

		TSArgList *list=new_par.GetPrimaryParamList();

		//cout<<"Add new pars: "<<list->GetSize()<<endl;
		GetPrimaryParamList()->AddOnce( *list );
	}



}



void TSParamFunction::fBuildPrimaryParamList() {

	//cout<<"--->1 "<<GetName()<<" "<<__FILE__<<"::"<<__FUNCTION__<<GetPrimaryParamList()<<" "<<GetPrimaryParamList()->GetSize()<<endl;

	fClearPrimaryParamList();

	//cout<<"--->2 "<<GetName()<<" "<<__FILE__<<"::"<<__FUNCTION__<<GetPrimaryParamList()<<" "<<GetPrimaryParamList()->GetSize()<<endl;

}
