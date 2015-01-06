/*
 * TSPhaseSpaceVolume.cc
 *
 *  Created on: May 29, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include "TSPhaseSpaceVolume.h"

#include "MessageMgr.h"


TSPhaseSpaceVolume::TSPhaseSpaceVolume()
:TSNamed("","","")
{

	Init();

}

TSPhaseSpaceVolume::TSPhaseSpaceVolume(int dim, TString name, TString title)
:TSNamed(name,"",title)
{


	Init();

	SetDimension(dim);

}


TSPhaseSpaceVolume::TSPhaseSpaceVolume(const TSPhaseSpaceVolume& other)
:TSNamed(other.Name(),other.GetLabel(),other.Title())
{

   Init();

   SetDimension(other.GetDimension());

   for(int i=1; i<=GetDimension();++i){
	   SetAxis(i, *other.GetVariable(i), other.GetLowEdge(i), other.GetUpEdge(i));
   }

}


TSPhaseSpaceVolume::~TSPhaseSpaceVolume() {


	//Remove allocated vars in axis
	Clear();



}


void TSPhaseSpaceVolume::Copy(const TSPhaseSpaceVolume& other) {

	Clear();

	SetDimension(other.GetDimension());

	for(int i=1; i<=GetDimension();++i){
		SetAxis(i, *other.GetVariable(i), other.GetLowEdge(i), other.GetUpEdge(i));
	 }

}

int TSPhaseSpaceVolume::Add(const TSPhaseSpaceVolume& second) {

	/**
	 *
	 *  Add a second volume to this volume
	 *
	 *  Dimension is sum of the two dimensions
	 *
	 *  Axis vars and ranges are added to the existing axis.
	 *
	 *
	 *
	 */


 int DIM2=second.GetDimension();

 int DIM1=GetDimension();

 SetDimension(DIM1+DIM2);

 for(int axis=1; axis<=DIM2; ++axis){
	 SetAxis(DIM1+axis, *(second.GetVariable(axis)), second.GetLowEdge(axis), second.GetUpEdge(axis));
 }

  return 1;

}

void TSPhaseSpaceVolume::Clear() {

  fDimension=0;

  //Clear fAxisVariables

  for(std::map<int,TSVariable*>::iterator it=fAxisVariable.begin(), end=fAxisVariable.end(); it!=end; ++it){

	  delete it->second;

  }

}



TSVariable* TSPhaseSpaceVolume::GetAxis(int axis) const {

	if(axis<=0 || axis>GetDimension()){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fAxisVariable.count(axis) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis variable not set");
		return 0;
	}

	return (fAxisVariable.find(axis)->second);


}

double TSPhaseSpaceVolume::GetCenter(int axis) const{


	if(axis<=0 || axis>GetDimension()){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fAxisVariable.count(axis) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis variable not set");
		return 0;
	}


	return (fAxisVariable.find(axis)->second)->GetValue();


}


double TSPhaseSpaceVolume::GetLowEdge(int axis) const {

	if(axis<=0 || axis>GetDimension()){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fAxisVariable.count(axis) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis variable not set");
		return 0;
	}

	return (fAxisVariable.find(axis)->second)->GetMin();


}

double TSPhaseSpaceVolume::GetUpEdge(int axis) const {


	if(axis<=0 || axis>GetDimension()){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fAxisVariable.count(axis) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis variable not set");
		return 0;
	}

	return (fAxisVariable.find(axis)->second)->GetMax();

}

const TSVariable* TSPhaseSpaceVolume::GetVariable(int axis) const{

	if(axis<=0 || axis>GetDimension()){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fAxisVariable.count(axis) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis variable not set");
		return 0;
	}


	return (fVarList.find(axis))->second;
}


void TSPhaseSpaceVolume::Init() {

	fDimension=0;
}



int TSPhaseSpaceVolume::IsInside() const{



	for(int i=1; i<=fDimension; ++i){

		double val=GetVariable(i)->GetValue();

		//double range_min=fAxisVariable.find(i)->second->GetMin();
		//double range_max=fAxisVariable.find(i)->second->GetMax();
		//bool is_in = (val>= range_min && val<range_max);

		bool is_in = fAxisVariable.find(i)->second->IsInside(val);

		if( !is_in ){
			return 0;
		}

	}


	return 1;
}



void TSPhaseSpaceVolume::Print() const {

	cout<<"Volume: "<<Name()<<" "<<Title()<<": "<<GetDescription()<<endl;

}


int TSPhaseSpaceVolume::AddAxis(const TSVariable& var, double low,	double up)
{

	fDimension++;

	return SetAxis(fDimension,var,low,up);


}


int TSPhaseSpaceVolume::SetAxis(int ax, const TSVariable& var, double low,	double up) {



	if(ax<=0 || ax>fDimension ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," axis number outside volume dimensions");
		return 0;
	}


	fVarList[ax]=&var;


	TSVariable *vaxis= new TSVariable(var);

	vaxis->RemoveRange();


	vaxis->SetValue( 0.5*(low+up) );

	vaxis->SetRange(low,up);

	if( !vaxis->SetRange(low,up) ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," range not valid");
		return 0;
	}



	//delete existing
	if( fAxisVariable.count(ax) ){
		delete (fAxisVariable.find(ax))->second;
	}


    fAxisVariable[ax]=vaxis;

    fBuildDescription();


    return 1;
}



void TSPhaseSpaceVolume::SetDimension(int dim) {

	if(dim<=0){
      MSG::ERROR(__FILE__,"::",__FUNCTION__," dimension <= 0");
	  return;
	}

	fDimension=dim;

}


void TSPhaseSpaceVolume::fBuildDescription() {

   fDescription="";


   for(int i=1; i<=fDimension; ++i){

	   if( fAxisVariable.count(i)>0 ){
		   TString str= fAxisVariable[i]->GetLabelAndRange();

		   if(i>1) fDescription+=" ";
		   fDescription += str;

	   }

   }

}
