/*
 * TSCategoryCard.cc
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <vector>

#include "MessageMgr.h"

#include "TSCategoryCard.h"


#define VERBOSE false
#if VERBOSE
#define V(x) std::cout<<x<<std::endl
#else
#define V(x)
#endif




TSCategoryCard::TSCategoryCard()
{


}


TSCategoryCard::TSCategoryCard(TString name, TString title)
:TSNamed(name,name,title)
{


}

TSCategoryCard::TSCategoryCard(const TSCategoryCard &other)
:TSNamed(other.GetName(),other.GetName(),other.GetTitle())
{

	Copy(other);
}


TSCategoryCard::~TSCategoryCard() {


	Clear();

}



void TSCategoryCard::Clear()
{

	for(std::vector<TSCategory*>::iterator it=fCatList.begin(), end=fCatList.end(); it!=end; ++it){

		delete *it;

	}

	fCatList.clear();

}


int TSCategoryCard::Compare(const TSCategoryCard &other, int opt_card, int opt_categ)
{

	/**
	 *
	 *  ---- For opt_card -----
	 *  opt == 0 (default) Exclusive Comaprison of Categories:
	 *            at least one Categ in common
	 *
	 *  opt ==1 Inclusive Comparison of Categories:
	 *          same set of Categories (any order)
	 *
	 *  ---opt_categ---
	 *   Same meaning.
	 *   Check description in TSCategory::Compare
	 *
	 */

	//cout<<__FUNCTION__<<endl;

	int opt = opt_card;

	int N1 = GetNumOfCategories();

	int N2 = other.GetNumOfCategories();

	V("---   Comparing cards: "<<GetName()<<" vs "<<other.GetName());
	V((opt_card==1 ? "Inclusive Card Comparison":" Exclusive Card Comparison"));
	V((opt_categ==1 ? "  Inclusive Category Comparison":" Exclusive Category Comparison"));
	V("      Num. of Categs:   "<<N1<<" vs "<<N2);

	if( VERBOSE ){

		cout<<endl<<" ---This Card:---- "<<endl;
		Print();
		cout<<" ------------- "<<endl;


		cout<<endl<<" ---Compared to other Card: ---- "<<endl;
		other.Print();

		cout<<"--------------  "<<endl;

	}

	int flag = 0;

	if( opt==1 && N1 != N2 ){

		flag = 0;
		V(" Different number of Categs between the two Cards, return: "<<flag);

		return flag;
	}



	std::vector<int> match1;
	std::vector<int> match2;

	for(int i=0; i<N1; ++i){

		TSCategory *cat1 = GetCategory(i);

		for(int j=0; j<N2; ++j){
			if( cat1->SameType( *other.GetCategory(j) ) ){
				match1.push_back(i);
				match2.push_back(j);

			}
		}
	}


	if( opt==1 && match1.size() != N1 ){

		flag = 0;

		V(" Different Sets of Categories between the two Cards, return: "<<flag);

		return flag;
	}




	for(int i=0; i<match1.size(); ++i){

		TSCategory *cat1 = GetCategory(match1[i]);
		TSCategory *cat2 = other.GetCategory(match2[i]);

		int flag_cat = cat1->Compare(*cat2,opt_categ);

		V(" ----> comparing: "<<i<<" "<<cat1->GetName()<<" "<<cat2->GetName()<<" result flag: "<<flag_cat);

		if( flag_cat ){
			V(" Categories comparison OK: "<<flag_cat);
		}else{
			V(" Categories comparison Not Passed: return "<<flag_cat);
			return flag_cat;
		}

	}

	V(" All Categories comparison OK: return 1");

	return 1;

}



void TSCategoryCard::Copy(const TSCategoryCard &other)
{



	Clear();


	for(int i=0; i<other.GetNumOfCategories(); ++i){

		TSCategory *card = const_cast<TSCategory*>(other.GetCategory(i));

	    AddCategory( *card );
	}


}


void TSCategoryCard::AddCategory(const TSCategory &cat)
{

	for(int i=0; i<fCatList.size();++i){

		if( fCatList[i]->GetName() == cat.GetName() ){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," Category ",cat.GetName()," already added");
			return;
		}
	}


	TSCategory *cnew= new TSCategory(cat);


	fCatList.push_back(cnew);

}


TH1F& TSCategoryCard::GetHistogram()
{

	fHisto.SetName(StringUtils::Name("hCatCard_",GetName()));
	fHisto.SetTitle(GetTitle());


	std::vector<TString> label;
	std::vector<bool> is_present;

	for(int i=0; i<GetNumOfCategories();++i){

		TSCategory *cat = GetCategory(i);

		TSCategory mother = GetCategory(i)->GetMotherCategory();

		for(int im=0; im<mother.GetNumOfMembers(); ++im){
			label.push_back( mother.GetMemBinLabel(im) );

			bool isin=0;

			for(int im2=0;im2<cat->GetNumOfMembers();++im2){
				if(cat->GetMemBinLabel(im2)==mother.GetMemBinLabel(im) ) isin=true;
			}

			if(isin) is_present.push_back(1);
			else is_present.push_back(0);
		}



		if(i+1!=GetNumOfCategories()){
			label.push_back("");
			is_present.push_back(0);
		}

	}

	int N=label.size();

	fHisto.SetBins(N,0.,N);

	for(int i=1;i<=N;++i){
		fHisto.GetXaxis()->SetBinLabel(i,label[i]);
		fHisto.SetBinContent(i,is_present[i]);
	}


	cout<<__FUNCTION__<<" "<<fHisto.GetXaxis()<<endl;

	fHisto.SetStats(0);
	fHisto.LabelsOption("v","x");
	fHisto.GetXaxis()->SetLabelOffset( 2*fHisto.GetXaxis()->GetLabelOffset() );

	cout<<__FUNCTION__<<endl;

	return fHisto;
}



TSCategory* TSCategoryCard::GetCategory(int i) const
{


	if( i<0 || i > GetNumOfCategories()-1 ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," invalid element index ",i);
		return 0;
	}


	return fCatList.at(i);


}


TSCategory* TSCategoryCard::GetCategory(const TSCategory &c) const
{


	for(int i=0; i<fCatList.size();++i){

		TSCategory *cat = fCatList.at(i);

		if( cat->SameType(c) ){
			return cat;
		}

	}

	return 0;

}



void TSCategoryCard::Print() const
{

	cout<<" --- TSCategoryCard: "<<GetName()<<" "<<GetTitle()<<" ---- "<<endl;

	cout<<"  Num of Categories: "<<fCatList.size()<<endl;

	for(int i=0; i<fCatList.size();++i){

		cout<<" -"<<i<<"-"<<endl;
		fCatList[i]->Print();
	}


	cout<<" -------------------- "<<endl;
}
