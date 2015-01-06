/*
 * TSVariable.cc
 *
 *  Created on: May 26, 2014
 *      Author: sdiluise
 */

#include <TMath.h>

#include "StringUtils.h"

#include "TSVariable.h"

#include "MessageMgr.h"


using std::cout;
using std::endl;


TSVariable::TSVariable():ServerClientMgr("")
{
	// TODO Auto-generated constructor stub

	Clear();

}

TSVariable::TSVariable(TString name, TString label, TString title, TString unit):
		TSNamed(name,label,title)
		,ServerClientMgr(name)
{

  Clear();

  SetLabel(label);
  SetTitle(title);
  SetUnit(unit);


}


TSVariable::TSVariable(const TSVariable &other):
		TSNamed(other.Name(),other.GetLabel(),other.Title())
        ,ServerClientMgr(TString(other.Name()))

{


	Copy(other);

}


TSVariable::~TSVariable() {

}

void TSVariable::Clear() {

	fVal=0;
	fMin=0;
	fMax=0;
	fHasRange=false;
	fHasRangeForced=false;
	fHasValue=false;
	fHasUnit=false;

	fAxisUnit="";
    fAxisTitle="";
    fDefinition="";
	fDescription="";
	fLabelAndRange="";
	fRange="";
	fUnit="";

}


bool TSVariable::ContainsRange(double min, double max) const
{
	/**
	 *    |-------------|  this
	 *      |--------|     other
	 *
	 *
	 */

	if( !HasRange() ) return true;

	return (GetMin() <= min && GetMax() >=max) ? true: false;

}

bool TSVariable::ContainsRange(const TSVariable &var) const
{

	if( !var.HasRange() ) return true;

	return ContainsRange(var.GetMin(),var.GetMax());
}


void TSVariable::Copy(const TSVariable &other){


	//cout<<__FILE__<<"::"<<__FUNCTION__<<" "<<other.GetName()<<" from "<<GetName()<<endl;

	Clear();


	SetLabel(other.GetLabel());
	SetTitle(other.Title());

	if(other.HasUnit()){
		SetUnit(other.GetUnit());
	}

	if( other.HasRange() ){
		SetRange(other.GetMin(),other.GetMax());
	}

	if( other.HasValue() ){
		SetValue(other.GetValue());
	}




}


TSVariable *TSVariable::Clone(TString name) const{

	TSVariable *v=new TSVariable();

	if( !name.IsWhitespace()) v->SetName(name);
	else v->SetName(GetName());

	v->Copy(*this);


	return v;


}



bool TSVariable::IsInside(double x) const{

	if( !fHasRange ) return true;


	return (x>=fMin && x<fMax) ? true: false;

}



void TSVariable::ForceToRange(bool v){


	fHasRangeForced=v;

	//if(!v){
	//	MSG::WARNING(__FILE__,"::",__FUNCTION__," is Disabled: an assigned value which is outside the specified range will not be brought back into the range ");
	//}

	if(v){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," is Enabled: an assigned value which is outside the specified range will be brought back into the range ");
	}


}



TString TSVariable::GetLabelAndValue() const {

	TString str=STR_UTILS::Name(GetLabel(),": ",STR_UTILS::ToString(GetValue())," ",GetUnit());

	return str;


}

TString TSVariable::GetLabelAndValue(double val) const {

	TString str=STR_UTILS::Name(GetLabel(),": ",STR_UTILS::ToString(val)," ",GetUnit());

	return str;


}



TString TSVariable::GetTitleXY(TString tity) const{

	//if( tity.IsNull() ) tity="Entries";

	TString tit=STR_UTILS::TitleXYZ(GetTitle(),GetAxisTitle(),tity);

	return tit;

}

void TSVariable::Print() const {


	 cout<<GetName()<<": "<<GetDescription()<<" value: "<<GetValue();
	 cout<<endl;

}

bool TSVariable::RangesOverlap(double min,double max) const
{

	/**
	 *    |----------|      this range
	 *      |-----------|   other range (min,max)
	 *
	 *    |----------|
	 * |-----------|
	 *
	 *
	 *    |-------------|
	 *      |--------|
	 *
	 *      |---------|
	 *    |--------------|
	 *
	 */

	if( !HasRange() ) return true;

	// 1 & 2
	if( GetMax()<=min || GetMin()>=max) return false;

	// ??
	//return true;

	//3
	if( GetMin()<=min && GetMax()>=max) return true;

	//4
	if( GetMin()>=min && GetMax()<=max) return true;

	return false;

}

bool TSVariable::RangesOverlap(const TSVariable &var) const
{

	if( !var.HasRange() ) return true;

	return RangesOverlap(var);

}

bool TSVariable::RangeIsContained(double min,double max) const
{

	/**
		 *    |-------------|  this
		 *   |----------------|     other
		 *
		 *
		 */

	if( !HasRange() ) return true;


	return (GetMin() >=min && GetMax()<=max) ? true: false;

}

bool TSVariable::RangeIsContained(const TSVariable &var) const
{
	if( !var.HasRange() ) return true;

	return RangeIsContained(var);

}

void TSVariable::RemoveRange() {

	fHasRange=false;
	fMin=0;
	fMax=0;

	fBuildStrsWithRange();

}

bool TSVariable::SameRange(double min,double max) const
{

	if( !HasRange() ){
		return true;
	}

	if( TMath::AreEqualRel(GetMin(),min,1e-5) && TMath::AreEqualRel(GetMax(),max,1e-5) ){
		return true;
	}else{
		return false;
	}

	return false;

}

bool TSVariable::SameRange(const TSVariable &var) const
{
	if( !var.HasRange() ) return true;

	return SameRange(var.GetMin(),var.GetMax());

}

void TSVariable::SetLabel(TString label){

	TSNamed::SetLabel(label);

	fBuildStrsWithLabel();

}


int TSVariable::SetRange(double vmin, double vmax) {

	if(vmin >= vmax){

		MSG::WARNING("Range Not Set-Illegal range values ordering: ",vmin," ",vmax);
		return 0;
	}



	fMin=vmin;
	fMax=vmax;
	fHasRange=true;

	if( fHasValue && fHasRangeForced ){


		if( fVal < fMin ){

			MSG::WARNING(__FUNCTION__," ",Name()," current set value: ",fVal," is outside range. New value set to lower edge: ",fMin);
			fVal=fMin;


		}else if( fVal > fMax ){

			MSG::WARNING(__FUNCTION__," ",Name()," current set value: ",fVal," is outside range. New value set to upper edge: ",fMax);
			fVal=fMax;

		}

	}

	fBuildStrsWithRange();

	return 1;
}



void TSVariable::SetTitle(TString tit){

	TSNamed::SetTitle(tit);

	fBuildStrsWithTitle();

}


void TSVariable::SetUnit(TString unit) {


	if ( !unit.IsWhitespace() ){
		fUnit=unit;
		fHasUnit=true;

		fBuildStrsWithUnit();
	}



}


void TSVariable::SetValue(double v) {

	if( fHasRange && fHasRangeForced){

		if( v > fMax){
			fVal=fMax;
			MSG::WARNING(__FUNCTION__," for ",Name()," value: ", v," brought back into range: ",GetRange());

		}else
		if( v < fMin){
			fVal=fMin;
			MSG::WARNING(__FUNCTION__," for ",Name()," value: ",v," brought back into range: ",GetRange());

		}else{

			fVal=v;
		}


	}else{

		fVal=v;

	}

	fHasValue=true;


	if(ServerClientMgr::kBroadcastIsActive){
		ServerClientMgr::BroadcastToClients(ServerClientMgr::kChanged);
	}

}







void TSVariable::fBuildStrAxisTitle() {

	fAxisTitle=GetLabel();

	if( fHasUnit ) {
		fAxisTitle+=" "; fAxisTitle+=fAxisUnit;
	}

}

void TSVariable::fBuildStrAxisUnit() {

	if(fHasUnit){
		fAxisUnit="["; fAxisUnit+=fUnit; fAxisUnit+="]";
	}

}

void TSVariable::fBuildStrDefinition() {

	fDefinition=GetLabel();
	if( fHasRange ){
		fDefinition+=":"; fDefinition+=fRange;
	}
}

void TSVariable::fBuildStrDescription() {

	fDescription=Title();

	fDescription+=" "; fDescription+=GetLabel();
	if( fHasRange ){
		fDescription+=":"; fDescription+=fRange;
	}


}

void TSVariable::fBuildStrLabelAndRange() {


	fLabelAndRange=GetLabel();

	if( fHasRange ){
		fLabelAndRange+=":"; fLabelAndRange+=fRange;
	}
}

void TSVariable::fBuildStrRange() {

	if( fHasRange){

		fRange="[";
        fRange+=STR_UTILS::ToString(fMin);
        fRange+=",";
        fRange+=STR_UTILS::ToString(fMax);
		fRange+="]";

		if(fHasUnit){
			fRange+=" "; fRange+=fUnit;
		}

	}else{

		fRange="";
	}



}

void TSVariable::fBuildStrsWithLabel() {

   fBuildStrAxisTitle();
   fBuildStrLabelAndRange();
   fBuildStrDescription();
   fBuildStrDefinition();

}

void TSVariable::fBuildStrsWithRange() {


	fBuildStrRange();
	fBuildStrLabelAndRange();
	fBuildStrDescription();
	fBuildStrDefinition();
}

void TSVariable::fBuildStrsWithTitle() {

	fBuildStrDescription();
	fBuildStrDefinition();
}

void TSVariable::fBuildStrsWithUnit() {

	fBuildStrAxisUnit();
	fBuildStrAxisTitle();
	fBuildStrRange();
	fBuildStrLabelAndRange();
	fBuildStrDescription();
	fBuildStrDefinition();

}
