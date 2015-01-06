/*
 * TSCategoryDef.cc

 *
 *  Created on: Sep 24, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>
#include <iostream>
#include <algorithm>

#include "StringUtils.h"

#include "TSCategoryDef.h"


using std::cout;
using std::endl;


TSCategoryDef::TSCategoryDef()
{


	Init();
}

TSCategoryDef::TSCategoryDef(TString name, TString label, TString title)
:TSNamed(name,label,title)
{

	Init();

	//needed to generate name-hash
	SetName(name);
}



TSCategoryDef::~TSCategoryDef() {


}



void TSCategoryDef::Clear(){

	fMember.clear();
	fMemLabel.clear();
	fMemHash.clear();

	fNumOfMems=0;

}



void TSCategoryDef::Copy(const TSCategoryDef &other){

	/**
	 *
	 * Copy Members, clear old members
	 *
	 */


	Clear();



	for(int i=first_ele_idx; i<other.GetNumOfMembers()+first_ele_idx; ++i){

		AddMember(other.GetMemName(i),other.GetMemLabel(i));

	}

	//fHasAllMems=true;


}



void TSCategoryDef::AddMember(TString name, TString label)
{


	if( name.IsWhitespace() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName(),", Member name is whitespace, not added");
		return;
	}

	for(int i=0;i<fMember.size();++i){


		if( fMember.at(i)==name  ){

			MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName()," Member name already used, not added, name: ",name );
			return;
		}

	}

	if( label.IsWhitespace() ) label=name;


	fMember.push_back(name);
	fMemLabel.push_back(label);
	fMemBinLabel.push_back(StringUtils::Name(GetLabel(),":",label));
	fMemHash.push_back( name.Hash() );
	fNumOfMems = fMember.size();

	fCheckCollisions();

	fBuildHash();

	fBuildHisto();

}



void TSCategoryDef::AddMembers(TString name_list, TString label_list)
{

	std::vector<TString> names =StringUtils::Tokenize(name_list,token_sep);

	std::vector<TString> labels = StringUtils::Tokenize(label_list,token_sep);

	if(names.size() != labels.size() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName(),", Member and Label list have different sizes");

		//set missing labels equal to the member name
		if(names.size() > labels.size() ){

			int n=labels.size();

			for(int i=n;i<names.size();++i){
				labels.push_back( names.at(i) );
			}

		}

	}


	for(int i=0; i<names.size(); ++i){

		AddMember(names.at(i),labels.at(i) );

	}


}



void TSCategoryDef::AddMembers(const TSCategoryDef &cat, TString name_list)
{

	if( name_list.IsWhitespace() ){

		Clear();

		Copy(cat);

		fHasAllMems = true;

		return;
	}


	std::vector<TString> names = StringUtils::Tokenize(name_list,token_sep);


	std::vector<int> idx;

	for(int i=first_ele_idx; i<cat.GetNumOfMembers()+first_ele_idx; ++i){

		TString mem = cat.GetMemName(i);

		for(int j=0;j<names.size();++j){

			if( mem==names.at(j) ){
				idx.push_back( i );
			}
		}

	}

	if( idx.size() == 0){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid member list");
		return;
	}


	Clear();


	std::sort(idx.begin(),idx.end());

	//cout<<name_list<<" "<<names.size()<<" "<<idx.size()<<endl;

	for(int i=0;i<idx.size();++i){

		AddMember( cat.GetMemName(idx.at(i)), cat.GetMemLabel( idx.at(i) ) );
	}

	if( GetHash() == cat.GetHash() ){
		fHasAllMems=true;
	}


}



int  TSCategoryDef::GetCategoryNameHash() const
{

	return fNameHash;

}


TString  TSCategoryDef::GetMemName(int i) const
{

	TString name="";

	// if i>=1 && i<=N: i<1 && i>N
	// if i>=0 && i<N:  i<0 && i>N-1

	if( i<first_ele_idx || i>GetNumOfMembers()-1+first_ele_idx ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName()," Member id out of range: ",i);
		return name;
	}

	return fMember[i-first_ele_idx];

}


TString  TSCategoryDef::GetMemLabel(int i) const
{

	if( i<first_ele_idx || i>GetNumOfMembers()-1+first_ele_idx ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName()," Member id out of range: ",i);
		return "";
	}

	return fMemLabel[i-first_ele_idx];

}

TString  TSCategoryDef::GetMemBinLabel(int i) const
{

	if( i<first_ele_idx || i>GetNumOfMembers()-1+first_ele_idx ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName()," Member id out of range: ",i);
		return "";
	}

	return fMemBinLabel[i-first_ele_idx];

}


int  TSCategoryDef::GetMemHash(int i) const
{

	if( i<first_ele_idx || i>GetNumOfMembers()-1+first_ele_idx ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Category: ",GetName()," Member id out of range: ",i);
		return 0;
	}

	return fMemHash[i-first_ele_idx];

}


bool TSCategoryDef::HasMember(TString name) const
{

	for(int i=0; i<fMember.size();++i){

		if( fMember[i]==name ) return true;
	}

	return false;

}

void  TSCategoryDef::Init()
{

	fHasAllMems=false;

	token_sep=":";

	//for Getters, start to count
	//elements from 0 or 1
	//do not touch this (init value=0)
	first_ele_idx = 0;
}

void TSCategoryDef::Print() const
{

	cout<<"-Category-: "<<GetName()<<" "<<GetLabel()<<" "<<GetTitle()<<endl;

	cout<<"   Num. of Members: "<<fMember.size()<<endl;

	for(int i=0; i<fMember.size(); ++i){
		cout<<"  "<<i<<" "<<fMember.at(i)<<" "<<fMemLabel.at(i)<<endl;
	}


}



void TSCategoryDef::PrintHashTable() const
{

	cout<<" "<<endl;

	cout<<" Category: "<<GetName()<<" "<<GetLabel()<<" "<<GetTitle()<<endl;

	cout<<" Name: "<<GetName()<<" "<<fNameHash<<endl;

	cout<<"  Num. of Members: "<<fMember.size()<<endl;

	for(int i=0; i<fMember.size(); ++i){
		cout<<"  "<<i+1<<" "<<fMember.at(i)<<" "<<fMemHash.at(i)<<endl;
	}

}



void TSCategoryDef::SetName(TString name)
{

	TSNamed::SetName(name);

	fNameHash = name.Hash();

}







void TSCategoryDef::fBuildHash()
{

	/**
	 *
	 * Uses only member names
	 *
	 */

	TString sum="";

	for(int i=0;i<fMember.size();++i){
		sum += fMember.at(i);
	}

	fHash=sum.Hash();


}

void TSCategoryDef::fBuildHisto()
{


	fHisto.SetName(StringUtils::Name("hCatCard_",GetName()));
	fHisto.SetTitle(GetTitle());

	int N = GetNumOfMembers();

	fHisto.SetBins(N,0.5,N+0.5);

	for(int i=1; i<=N; ++i){

		fHisto.GetXaxis()->SetBinLabel(i,GetMemBinLabel(i-1));
	}


}

bool TSCategoryDef::fCheckCollisions()
{

	/**
	 *
	 *
	 *
	 */

	for(int i=0;i<fMember.size()-1;++i){
		for(int j=i+1;j<fMember.size();++j){

			if(fMemHash[i] == fMemHash[j]){

				MSG::ERROR(" Two Members Have the Same Hash value:");
				MSG::ERROR(" ",fMember[i]," ",fMember[j]);
				MSG::ERROR(" ",fMemHash[i]," ",fMemHash[j]);

				assert(0);

			}

			//return false;
		}
	}

	return true;


}


