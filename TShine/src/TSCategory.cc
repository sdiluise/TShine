/*
 * TSCategory.cc
 *
 *  Created on: Sep 24, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "TSCategory.h"



using std::cout;
using std::endl;


#define VERBOSE false
#if VERBOSE
#define V(x) std::cout<<x<<std::endl
#else
#define V(x)
#endif


TSCategory::TSCategory()
{

}

TSCategory::TSCategory(const TSCategoryDef &cat, TString members)
:TSCategoryDef(cat.GetName(),cat.GetLabel(),cat.GetTitle())
{

	TSCategory::Init();

	fMotherCateg.Copy(cat);
	fMotherCateg.SetName(cat.GetName());
	fMotherCateg.SetLabel(cat.GetLabel());


	SetName(cat.GetName());

	AddMembers(cat,members);


}


TSCategory::~TSCategory() {


}


TSCategory::TSCategory(const TSCategory &other)
:TSCategoryDef(other.GetName(),other.GetLabel(),other.GetTitle())
{


	Copy(other);

}




void TSCategory::Set(const TSCategoryDef &cat, TString list)
{

	Clear();

	AddMembers(cat,list);

	fMotherCateg.Copy(cat);
	fMotherCateg.SetName(cat.GetName());
	fMotherCateg.SetLabel(cat.GetLabel());


}


void TSCategory::Copy(const TSCategory &other)
{


	TSCategoryDef::Copy(other);

	fMotherCateg.Copy(other.GetMotherCategory());
	fMotherCateg.SetName(other.GetMotherCategory().GetName());
	fMotherCateg.SetLabel(other.GetMotherCategory().GetLabel());


}



bool TSCategory::SameMember(int i_this, const TSCategory &other, int i_other) const{

	// add your comparison algo...Hash value, name string ...
	return GetMemHash(i_this) == other.GetMemHash(i_other) ? true : false;

}

bool TSCategory::SameType(const TSCategory &other) const{

	// add your comparison algo...Hash value, name string...
	return GetCategoryNameHash() == other.GetCategoryNameHash() ? true : false;

}


int TSCategory::Compare(const TSCategory &other, int opt) const
{

	/**
	 *
	 *
	 *  Compare two Sample of the Same Category Type: i.e. Two Category with same name
	 *
	 *  opt==0 (default) Non-Inclusive Comparison
	 *  	two categories have at least one member in common, e.g.:
	 *
	 *  	Selection:CCQE   vs  Selection::CCInc,CCQE
	 *
	 *  opt==1 Inclusive Comparison
	 *  	two categories must have the same set of memebers (any order)
	 *
	 *  	Selection:CCQE,CCInc   vs  Selection::CCInc,CCQE
	 *
	 *
	 */

	V(endl<<__FILE__<<"::"<<__FUNCTION__<<" "<<GetName()<<" "<<other.GetName()<<" opt: "<<opt);

	int flag = 0;

	// it's the same
	if( this == &other ){

		V(" same object: return 1");
		return 1;
	}

	//it's not the same Category
	if( !SameType(other) ){

		V(" Categories are different: return 0");
		return  0;
	}


	//both are a complete sample of the Mother Category
	if( HasAll() && other.HasAll() ){

		V(" Both have All the members of the Mother Category: return 1 ");
		return 1;
	}


	int N1 = GetNumOfMembers();
	int N2 = other.GetNumOfMembers();

	V("Number of Members: "<<N1<<" vs "<<N2);

	if( opt==1 && N1 != N2 ){

		V(" Different Num. of Members on Inclusive Comparison: return 0");
		return 0;
	}


	//cout<<" Same Category, Same Num Of Members "<<endl;
	//cout<<" Check for members in common "<<endl;

	int nmatch = 0;

	for(int i=0; i<N1;++i){
		for(int j=0; j<N2;++j){

			if( SameMember(i,other,j) ){

				V("Member is in common: "<<GetMemName(i));
				nmatch++;
			}

		}//j
	}//i

	V(" Number of Member Matches: "<<nmatch);

	// --
	if(nmatch==0) V(" No Matches return 0");
	else{

		if(opt==0){

		}

		if(opt==1){
			if(nmatch!=N1) V(" Inclusive Coparison, Not all members matched return 0");
		}
	}

	//--


	if( opt == 0){

		return nmatch>0 ? true: false;

	}else if( opt ==1 ){

		return nmatch == N1 ? true: false;

	}


	MSG::ERROR(__FILE__,"::",__FUNCTION__," algorithm error");

	return 0;
}


