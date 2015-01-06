/*
 * TSEvent.cc
 *
 *  Created on: Sep 27, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>

#include "MessageMgr.h"

#include "TSArgList.h"

#include "TSEvent.h"


using std::cout;
using std::endl;


TSEvent::TSEvent()
:TSNamed("","","")
{

	Init();

}


TSEvent::TSEvent(TString name, TString tag, TString title)
:TSNamed(name,tag,title)
{

	Init();

}

TSEvent::TSEvent(const TSEvent &other) {

	Init();

	SetName( other.GetName() );
	SetLabel( other.GetLabel() );
	SetTitle( other.GetTitle() );
	SetTag( other.GetTag() );

	TSArgList list;

	for(int i=0; i<other.GetNumOfVars(); ++i){

		list.Add( (TSVariable*)other.GetVarLink(i) );

	}

	SetVariables(list);

	for(int i=0; i<GetNumOfVars();++i){

		SetVarValue(i, other.GetVarValue(i));
	}

	SetWeightNative( other.GetWeightNative() );
	SetWeight( other.GetWeight() );


	SetId( other.GetId() );

}



TSEvent::~TSEvent() {

	//Remove allocated vars

	for(std::map<int,TSVariable*>::iterator it=fVarList.begin(), end=fVarList.end(); it!=end; ++it){

		delete it->second;

	}


}



void TSEvent::AddResponseFunctionToParam(const TSRespFunc &rf, const TSNuiParam &par)
{

	if( !HasWeightParams() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Weight Parameters not set for this TSEvent ");
		return;
	}


	if( !fNuiParSet.HasParameter(par) ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Parameter ",par.GetName()," not a Weight Param  for this TSEvent ");
		return;
	}


	fNuiParSet.AddResponseFunctionToPar(rf,par);

}


double TSEvent::ComputeWeight()
{

	double w_nui = 1;

	if( HasWeightParams() ){
		w_nui = fNuiParSet.ComputeWeight();
	}else{
		return w_nui;
	}


	ReWeightWith(w_nui);

	return fWeight;

}


void TSEvent::Fill()
{

	for(int i=0; i<fNumOfVars;++i){

		fVarList[i]->SetValue( fVarPtr[i]->GetValue() );

	}

}

std::map<const TSVariable*,double>& TSEvent::FillValues()
{

	int id=0;
	for(std::map<int,const TSVariable*>::const_iterator it=fVarPtr.begin(), end=fVarPtr.end();
			it!=end;++it){

		double val = GetVarValue(id);

		//const TSVariable *v = it->second;
		fMapOfValues[it->second]= val;

		id++;
	}

	return fMapOfValues;
}

TSCategoryCard& TSEvent::GetCategoryCard()
{

	return fCard;

}


const TSVariable* TSEvent::GetVariable(int i) const
{
	if( fVarPtr.count(i)==1 ){

		//ptr to local
		return fVarList.find(i)->second;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index out of range: ",i);
		return NULL;
	}

}

const TSVariable* TSEvent::GetVarLink(int i) const
{
	if( fVarPtr.count(i)==1 ){

		//ptr to local
		return fVarPtr.find(i)->second;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index out of range: ",i);
		return NULL;
	}

}




double TSEvent::GetVarValue(int i) const
{



	std::map<int,TSVariable *>::const_iterator fItr = fVarList.find(i);

	if( fItr!=fVarList.end() ){

		return fItr->second->GetValue();

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index out of range: ",i);
		return 0;
	}


	//alt..
	if( fVarList.count(i)==1 ){

		TSVariable *v = fVarList.find(i)->second;

		return v->GetValue();

	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index out of range: ",i);
		return 0;
	}
}


double TSEvent::GetVarValue(const TSVariable &v)
{

	FillValues();


	//UNORD
	std::map<const TSVariable*,double>::const_iterator fItr = fMapOfValues.find(&v);

	//cout<<"** "<<v.GetName()<<" "<<fMapOfValues.count(&v)<<endl;

	//cout<<fItr<<" "<<fMapOfValues.end()<<endl;

	if( fItr != fMapOfValues.end() ){

		//cout<<"** "<<fItr->second<<endl;

		return fItr->second;

	}else{

		return 0;
	}



	//alt...
	if( fMapOfValues.count(&v) ){

		return fMapOfValues.find(&v)->second;

	}else{

		return 0;
	}


}


double TSEvent::GetWeightPosition(int pos)
{

	if( pos>=fWeightSaved.size() ) return 1;

	return fWeightSaved[pos];

}


bool TSEvent::HasVariable(const TSVariable &v) const
{

	for(std::map<int,const TSVariable*>::const_iterator it=fVarPtr.begin(), end=fVarPtr.end(); it!=end; ++it){

		if( it->second == &v) return true;

	}

	return false;

}


void TSEvent::Init()
{

	SetId(-1);

	SetWeightNative(1);

	fWeightSaved.reserve(10);

	fPOTMgr.SetNativePOT(1.,20.);

	fHasCategory = false;
	fHasProbFunc = false;
	fHasWeightParams = false;

	fNumOfVars = 0;

	fProbFunction = 0;

	fIdxEne = -1;
	fIdxLen = -1;

	fOscillProb = 1;
}



double TSEvent::ComputeOscillProb()
{

	/*
	 * Keep this in a separate method for performances reason
	 *
	 */
	double fOscillProb = 1;

	if( fProbFunction ){

		fProbFunction->FillParameters();

		if(fIdxEne>=0) fProbFunction->SetE(GetVarValue(fIdxEne));
		if(fIdxLen>=0) fProbFunction->SetL(GetVarValue(fIdxLen));

		fOscillProb = fProbFunction->Eval();

		return fOscillProb;

	}else{

		return fOscillProb;
	}

}


double TSEvent::Oscillate(int opt)
{

	fOscillProb = 1;

	//cout<<__FUNCTION__<<" "<<fProbFunction<<" HasProbFunc: "<<fHasProbFunc<<" fIdxEne: "<<fIdxEne<<" fIdxLen: "<<fIdxLen<<endl;

	if( fProbFunction ){

		fProbFunction->FillParameters();

		if(fIdxEne>=0) fProbFunction->SetE(GetVarValue(fIdxEne));
		if(fIdxLen>=0) fProbFunction->SetL(GetVarValue(fIdxLen));

		fOscillProb = fProbFunction->Eval();

	}else{

		return fOscillProb;
	}

	ReWeightWith(fOscillProb);

	return fOscillProb;

}



void TSEvent::Process()
{

	RestoreWeightNative();

	ComputeWeight();

	Oscillate();


}


void TSEvent::Print() const
{

	cout<<"-- TSEvent: "<<GetName()<<" "<<GetTag()<<" id: "<<GetId()<<endl;
	cout<<"Weight: "<<GetWeight()<<endl;

	for(int i=0; i<fNumOfVars; ++i){
		cout<<" "<<i<<" "<<GetVariable(i)->GetName()<<": "<<GetVarValue(i)<<endl;
	}

	if( HasCategoryCard() ){

		fCard.Print();
	}

	if( HasWeightParams() ){
		cout<<" --Weight Parameters:-- "<<endl;

		fNuiParSet.PrintNameSeq();

		cout<<" "<<endl;
		fNuiParSet.Print();
	}

	if( HasOscillProbFunction() ){

		cout<<" Probability Function: "<<endl;

		cout<<" Energy Variable: "<<(fIdxEne>=0 ? GetVariable(fIdxEne)->GetName(): "NN")<<endl;
		if(fIdxLen>=0) cout<<" Length Variable: "<<GetVariable(fIdxLen)->GetName()<<endl;


	}


	cout<<"--"<<endl<<endl;

}



double TSEvent::RestoreWeightNative()
{

	SetWeightNative( GetWeightNative() );

	return fWeightNative;
}



double TSEvent::ReWeightWith(double w){


	fWeight *= w;

	return fWeight;

}

double TSEvent::ReWeightWithSavedPosition(int pos){


	double w = GetWeightPosition(pos);


	//cout<<__FUNCTION__<<" "<<GetWeightPosition(pos)<<" "<<fWeightNative<<endl;

	ReWeightNativeWith(w/fWeightNative);

	return w;

}



double TSEvent::ReWeightNativeWith(double w){


	fWeight = fWeightNative*w;

	//cout<<__FUNCTION__<<" "<<w<<" "<<fWeightNative<<" "<<fWeight<<endl;

	return fWeight;

}


double TSEvent::ReWeightWithNuiParams(int opt){

	double w=1;

	w = fNuiParSet.ComputeWeight();

	return ReWeightWith(w);

}



double TSEvent::SaveWeightPosition(int pos)
{

	if( pos >= fWeightSaved.size() ){
		fWeightSaved.resize(pos+1,1);
	}

	fWeightSaved[pos] = fWeight;

	return fWeight;
}



void TSEvent::SetCategoryCard(const TSCategoryCard &card)
{


	fCard.Copy(card);

	fHasCategory = true;

}


void TSEvent::SetNativePOT(float POT, float Power)
{
	fPOTMgr.SetNativePOT(POT,Power);

}

void TSEvent::ScaleToPOT(float POT, float Power)
{

	/**
	 *
	 * With this function also the native wieght is scaled
	 * accordingly to the ratio between the the native pot
	 * and the new input values
	 * (i.e.: native weights and native pot are not aligned
	 * and have in general different meaning: native weight is what is multiplied
	 * for when a ReWeight method is invoked)
	 *
	 */

	//current scaling wrt to native POT
	double scale = fPOTMgr.GetPOTScaling();

	fPOTMgr.SetPOT(POT,Power);

	//new scaling wrt to native POT
	double scale_new = fPOTMgr.GetPOTScaling();

	//cout<<__FUNCTION__<<" "<<scale_new<<" "<<scale<<endl;

	//scale back and scale wrt to input values
	ReWeightWith(scale_new/scale);

	//scale also native weight
	fWeightNative *= scale_new/scale;

}



void TSEvent::ScaleToNativePOT()
{

	//cout<<__FUNCTION__<<" scale weight with: "<<1./fPOTMgr.GetPOTScaling()<endl;

	//get scaling wrt native POT

	double scale = fPOTMgr.GetPOTScaling();

	ReWeightWith( 1./scale );

	fWeightNative *= (1/scale) ;

	fPOTMgr.Restore();

}



int TSEvent::SetOscillProbFunction(TSOscProb *prob)
{

	if( !prob ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null ptr to Probability function");
		return 0;
	}

	fIdxEne = -1;
	fIdxLen = -1;

	for(int i=0; i<GetNumOfVars();++i){

		if(prob->GetEnergyVariableBound() == GetVarLink(i) ){
			fIdxEne = i;
		}

		if(prob->GetLengthVariableBound() == GetVarLink(i) ){
			fIdxLen = i;
		}
	}

	(fIdxEne == -1 && fIdxLen ==-1) ? fHasProbFunc = false:  fHasProbFunc = true;

	fHasProbFunc ? fProbFunction = prob : fProbFunction = 0;

	cout<<__FUNCTION__<<" "<<fProbFunction<<" "<<fHasProbFunc<<" "<<fIdxEne<<" "<<fIdxLen<<endl;

}


void TSEvent::SetVariables(const TSArgList &vars)
{

	int N = vars.GetSize();

	if( N==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," List is Empty");
		assert(0);
	}


	for(int i=0; i<N; ++i){

		const TSVariable *v = (TSVariable*)vars.At(i);

		if ( !v ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Null Pointer to Variable: ",i);
			assert(0);
		}

		fVarPtr[i] = v;

		fMapOfValues[v]=0;

		fVarList[i] = v->Clone();

	}

	fNumOfVars = N;

}



void TSEvent::SetVarValue(int i, double val)
{

	//NB: GetVar return the pointer to the server variable
	//not the ptr to the local copy

	if( fVarList.count(i)==1 ){

		TSVariable *v = fVarList.find(i)->second;

		return v->SetValue(val);

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index out of range: ",i);

	}

}



void TSEvent::SetWeight(double w){

	fWeight = w;

}



void TSEvent::SetWeightNative(double w){

	fWeightNative = w;

	fWeight = w;

}



int TSEvent::SetWeightParams(const TSNuiParamSet &list)
{

	/**
	 *
	 *
	 *
	 *
	 *
	 */


	for(int ip=0; ip<list.GetSize(); ++ip){

		TSNuiParam *par = (TSNuiParam*)list.At(ip);

		bool add_par = true;

		// If Param Has Control Vars check that the ALL the Variables
		// that are in common are compatible, i.e.: variable
		// in the event is in the range of the corresponding Parameter Control Var
		if( par->HasControlVars() ){

			for(int iv=0; iv<GetNumOfVars();++iv){

				int var_idx = par->GetControlVarIdx(*GetVarLink(iv));
				if( var_idx>=0 ){
					if( !par->GetControlVariable(var_idx)->IsInside(GetVarValue(iv)) ){
						add_par = false;
					}else{
						//cout<<"OK CONTROL VAR: "<<GetVariable(iv)->GetName()<<" param "<<ip<<" "<<par->GetName()<<endl;
						//GetVariable(iv)->Print();
					}
				}//

			}//vars

		}//if control variables

		if( !add_par ){
			continue;
		}

		//cout<<"Add Param "<<ip<<" "<<par->GetName()<<" "<<par->GetPriorValue()<<endl;
		//par->Print();

		//
		// If both event and Par have CategoryCard
		// add Param to the list if comparison is passed
		//
		if( !HasCategoryCard() ){

			add_par = true;

		}else{

			if( !par->HasCategoryCard() ){
				add_par = true;
			}else{

				add_par = fCard.Compare( par->GetCategoryCard() );

			}

		}//category card/control var check

		if( !add_par ){
			continue;
		}


		fNuiParSet.Add(par);

		//Add response function if present
		TSRespFunc *rf = list.GetParamRespFunc(*par);

		if(rf) fNuiParSet.AddResponseFunctionToPar(*rf,*par);

	}//ipar


    if( fNuiParSet.GetSize() > 0 ){
    	fHasWeightParams = true;
    }

	return 1;
}

//
//  -- static functions ---
//

int TSEvent::fProcChainN=0;

void TSEvent::ClearProcessChain(){

	fProcChainN=0;
}

