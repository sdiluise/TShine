/*
 * TSNuiParam.cc
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <cmath>

#include "MessageMgr.h"

#include "TSNuiParam.h"

TSNuiParam::TSNuiParam() {


}

TSNuiParam::TSNuiParam(TString name, TString label, TString title, TString unit)
:TSParameter(name,label,title,unit)
{

	Init();
}

TSNuiParam::~TSNuiParam()
{

	//this loop will free also fVarLoc ptr
	for(std::vector<TSVariable*>::iterator it=fVarLocList.begin(),end=fVarLocList.end()
			;it!=end; ++it){

		delete *it;
	}



}


void TSNuiParam::SetCategoryCard(const TSCategoryCard &card)
{

	fCard.Copy(card);

	fHasCategory = true;

}


void TSNuiParam::AddControlVar(TSVariable &var, double min, double max)
{

	// temp solution
	// create a var struct and an array
	// to store multiple control vars
	fVar = &var;

	//subst
	if(fVarLoc) delete fVarLoc;

	fVarLoc = new TSVariable(var);

	fVarLoc->SetRange(min,max);

	fVarMin=min;
	fVarMax=max;

	fHasControlVars = true;

	bool is_new = true;
	for(int i=0; i<fVarList.size();++i){

		if(fVarList.at(i) == &var){
			is_new = false;

			fVarLocList[i] = fVarLoc;
		}
	}

	if(is_new){

		fVarList.push_back( &var );
		fVarLocList.push_back(fVarLoc);

		fNumOfControlVars = fVarLocList.size();
	}


}


int TSNuiParam::CompareCategory(const TSCategoryCard &other, int opt)
{

	if( !HasCategoryCard() ){

		return true;
	}

	int flag = fCard.Compare(other,opt);

	return flag;
}





TSVariable* TSNuiParam::GetControlVariable(int i) const
{

	if( !fHasControlVars ){
		return 0;
	}

	if(i<0 || i>=fNumOfControlVars){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Var. index out of range: ",i," Num Of Vars: ",fNumOfControlVars);
		MSG::ERROR(" -->Param: ",GetName());

		return NULL;
	}


	return fVarLocList[i];


}



int TSNuiParam::GetControlVarIdx(const TSVariable &v) const
{

	for(int i=0; i<fVarList.size();++i){

		if( fVarList[i] == &v ) return i;
	}

	return -1;
}


TString TSNuiParam::GetControlVarsLabel() const
{

	TString label = "";

	for(int i=0; i<fNumOfControlVars;++i){

		TString l = fVarLocList[i]->GetLabelAndRange();

		if(i==0) label = l;
		else label = StringUtils::Name(label,",",l);
	}

	return label;
}


double TSNuiParam::GetRandom()
{

	if( HasAsymErrorPrior() ){

		//temporary, use asym gauss
		return fRand.Gaus(GetPriorValue(), 0.5*(abs(GetPriorErrorLow())+GetPriorErrorUp()) );

	}else{

		return fRand.Gaus(GetPriorValue(),GetPriorError());
	}

	MSG::WARNING(__FILE__,"::",__FUNCTION__," No Prior Set for NuiParam: ",GetName());

	return 0;

}



bool TSNuiParam::HasControlVariable(const TSVariable &v) const
{

	for(int i=0; i<fVarList.size();++i){

		if( fVarList[i] == &v ) return true;
	}

	return false;
}



void TSNuiParam::Init(){

	fHasPrior=false;
	fHasErrorPrior=false;
	fHasAsymErrPrior=false;
	fPriorVal=0;
	fPriorError=0;
	fPriorErrLow=0;
	fPriorErrUp=0;

	fNumOfControlVars=0;
	fHasControlVars = false;
	fVar=0;
	fVarLoc=0;
	fVarMin=0;
	fVarMax=0;


	fHasCategory = false;


	fRand.SetSeed(0);
}


void TSNuiParam::Print(){

	cout<<endl;
	cout<<" ---- TSNuiParam ---  "<<GetName()<<endl;

	TSParameter::Print();

	if( HasControlVars() ){
		cout<<" -Control Variables-: "<<GetNumOfControlVars()<<endl;

		for(int i=0;i<GetNumOfControlVars();++i){
			fVarLocList[i]->Print();
		}

	}


	cout<<" -Has Category Card-: "<<HasCategoryCard()<<endl;

	if( HasCategoryCard() ){

		fCard.Print();

	}

	cout<<" --------- "<<endl;
}


void TSNuiParam::Randomize(){

	SetValue( GetRandom() );

}

void TSNuiParam::Restore(){

	if(fHasPrior){
		SetValue(fPriorVal);
	}

}


void TSNuiParam::SetPriorValue(double v)
{
	//add some check also...
	fPriorVal = v;

	SetValue( fPriorVal );

	fHasPrior = true;
}

void TSNuiParam::SetPriorError(double e)
{

	//add some check also
	fPriorError = abs(e);

	fHasErrorPrior = true;

	SetError( fPriorError);
}

void TSNuiParam::SetPriorAsymErrors(double err_low, double err_up)
{

	/*
	 *
	 * all asym errs overwritten
	 *
	 */

	if( !(
			    (err_low<0 && err_up>0)
				          ||
			   ((err_low==err_up)&&(err_low>=0))
	          )
		    ){

			MSG::WARNING(__FILE__,"::",__FUNCTION__," Illegal Asymmetric Errors: ",err_low," ",err_up);
			MSG::WARNING(" NuiParam: ",GetName());

			fPriorErrLow=0;
			fPriorErrUp=0;

			SetAsymErrors(fPriorErrLow,fPriorErrUp);

			fHasAsymErrPrior=false;

	        return;
		}

		fPriorErrLow=err_low;
		fPriorErrUp=err_up;

		SetAsymErrors(fPriorErrLow,fPriorErrUp);

	    fHasAsymErrPrior=true;


}
