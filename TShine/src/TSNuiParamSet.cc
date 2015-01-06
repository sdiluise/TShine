/*
 *
 *
 * TSNuiParamSet.cc
 *
 *  Created on: Sep 29, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <map>


#include "MessageMgr.h"

#include "TSNuiParamSet.h"




TSNuiParamSet::TSNuiParamSet() {

	Init();

}

TSNuiParamSet::~TSNuiParamSet() {


	for(std::map<int,TSRespFunc*>::const_iterator it=fRespFuncList.begin(), end=fRespFuncList.end()
		;it!=end; ++it)
		{

			delete it->second;
		}


}



void TSNuiParamSet::AddResponseFunctionToPar(const TSRespFunc &rf, const TSNuiParam &par)
{

	int idx = GetParamIdx(par);
	if(idx == -1){
		//MSG::WARNING(__FILE__,"::",__FUNCTION__," Param ",par.GetName(), " Not found ");
		return;
	}

	AddResponseFunctionToPar(rf,idx);

}



void TSNuiParamSet::AddResponseFunctionToPar(const TSRespFunc &rf, int ipar)
{

	fRespFuncList[ipar] = rf.Clone();

}



void TSNuiParamSet::BuildCategoryCardHisto(int opt)
{
	/**
	 *
	 *
		If a Category present in the histogram is not present
	 	among the Categories of the param then all the
	    member of that Category are checked-in for the param
		this is coherent with the exclusive Comparison algo used
		by default to compare two CategoryCards (opt==0)
		Select opt==1 to follow the inclusive comparison algo:
		if a Category is not added to the CategoryCard of the
		param it is assumed that the param will not be
		associate to cards where that category is present

	 */

	fCategCardHisto.SetName(StringUtils::Name("hCategCard_",GetName()));
	fCategCardHisto.SetTitle(GetTitle());

	fCategCardHisto.SetStats(0);

	TH1F hMotherCatName;

	bool is_first = true;

	std::vector<TString> label_y;



	int Ny = GetSize();

	for(int i=0; i<Ny;++i){

		TSNuiParam *p = (TSNuiParam*)At(i);

		label_y.push_back(p->GetLabel());
	}


	double offset_x = 0;
	double offset_y = 0;

	for(int i=0; i<GetSize();++i){

		TSNuiParam *p = (TSNuiParam*)At(i);

		if( !p->HasCategoryCard() ) continue;

		TSCategoryCard card = p->GetCategoryCard();


		for(int ic=0; ic<card.GetNumOfCategories();++ic){

			TSCategory *categ = card.GetCategory(ic);
			TSCategory mcateg = categ->GetMotherCategory();

			int Nm = mcateg.GetNumOfMembers();

			//mcateg.Print();


			bool is_new = true;

			if(is_first){

				fCategCardHisto.SetBins(Nm,offset_x,Nm+offset_x,Ny,offset_y,Ny+offset_y);
				for(int ib=0;ib<Nm;++ib) fCategCardHisto.GetXaxis()->SetBinLabel(ib+1,mcateg.GetMemBinLabel(ib));
				for(int ib=0;ib<Ny;++ib) fCategCardHisto.GetYaxis()->SetBinLabel(ib+1,label_y[ib]);

				hMotherCatName.SetBins(Nm,offset_x,Nm+offset_x);
				for(int ib=0;ib<Nm;++ib) hMotherCatName.GetXaxis()->SetBinLabel(ib+1,mcateg.GetName());



			}else{

				int Nx = fCategCardHisto.GetNbinsX();


				for(int ib=1;ib<=Nx;++ib){

					TString lab_start = fCategCardHisto.GetXaxis()->GetBinLabel(ib);
					TString lab_end = fCategCardHisto.GetXaxis()->GetBinLabel(ib+Nm-1);

					TString labnew_start = mcateg.GetMemBinLabel(0);
					TString labnew_end = mcateg.GetMemBinLabel(Nm-1);

					if(lab_start == labnew_start && lab_end == labnew_end){
						is_new = false;
						break;
					}

				}//ib

			}//

			if(is_new){

				std::vector<TString> label_x;
				std::vector<TString> label_cat_x;


				if( !is_first){

					int Nx = fCategCardHisto.GetNbinsX();

					for(int ib=1;ib<=Nx;++ib){
						label_x.push_back(fCategCardHisto.GetXaxis()->GetBinLabel(ib));
						label_cat_x.push_back(hMotherCatName.GetXaxis()->GetBinLabel(ib));
					}

					label_x.push_back("");
					label_cat_x.push_back("");


					for(int ib=0;ib<Nm;++ib){
						label_x.push_back(mcateg.GetMemBinLabel(ib));
						label_cat_x.push_back(mcateg.GetName());

					}

					for(int ib=0;ib<label_x.size();++ib){
					}

				}else{

					for(int ib=0;ib<Nm;++ib){
						label_x.push_back(mcateg.GetMemBinLabel(ib));
						label_cat_x.push_back(mcateg.GetName());

					}

					is_first = false;

				}

				int N= label_x.size();

				fCategCardHisto.SetBins(N,offset_x,N+offset_x,Ny,offset_y,Ny+offset_y);
				for(int ib=0;ib<N;++ib) fCategCardHisto.GetXaxis()->SetBinLabel(ib+1,label_x[ib]);
				for(int ib=0;ib<Ny;++ib) fCategCardHisto.GetYaxis()->SetBinLabel(ib+1,label_y[ib]);

				hMotherCatName.SetBins(N,offset_x,N+offset_x);
				for(int ib=0;ib<N;++ib) hMotherCatName.GetXaxis()->SetBinLabel(ib+1,label_cat_x[ib]);

			}//is_new


		}//categs

	}//params


	//hMotherCatName.Write("hMotherCatName");
	//fill

	float bin_cont = -1;

	int Nx =  fCategCardHisto.GetNbinsX();

	for(int ip=0; ip<GetSize();++ip){

		TSNuiParam *p = (TSNuiParam*)At(ip);

		if( !p->HasCategoryCard() ) continue;

		int iby = ip+1;

		TSCategoryCard card = p->GetCategoryCard();

		//cout<<" -"<<ip<<"- "<<endl;
		//p->Print();
		//card.Print();

		//cout<<"___ par__: "<<ip<<" "<<p->GetName()<<endl;

		//
		// If a Category present in the histogram is not present
		// among the Categories of the param then all the
		// member of that Category are checked-in for the param
		// this is coherent with the exclusive Comparison algo used
		// by default to compare two CategoryCards (opt==0)
		// Select opt==1 to follow the inclusive comparison algo
		//
		if(opt==0){
		for(int ibx=1; ibx<=Nx;++ibx){
			TString cat_bin_name = hMotherCatName.GetXaxis()->GetBinLabel(ibx);

			if(cat_bin_name.IsWhitespace()) continue;

			bool hasCat = false;
			for(int ic=0; ic<card.GetNumOfCategories();++ic){

				TString cat_par_name = ((TSCategory*)card.GetCategory(ic))->GetName();

				if(cat_par_name == cat_bin_name) hasCat = true;
			}


			if(!hasCat){
				//cout<<" Not Present "<<cat_bin_name<<endl;

				for(int ibx=1; ibx<=Nx;++ibx){
					TString name = hMotherCatName.GetXaxis()->GetBinLabel(ibx);
					if(name == cat_bin_name) fCategCardHisto.SetBinContent(ibx,iby,bin_cont);
				}
			}

		}//histo bin loop
		}//opt
		//


		for(int ic=0; ic<card.GetNumOfCategories();++ic){

			TSCategory *categ = card.GetCategory(ic);

			int Nm = categ->GetNumOfMembers();

			//
			for(int ibx=1; ibx<=Nx;++ibx){
				TString lab = fCategCardHisto.GetXaxis()->GetBinLabel(ibx);

				if( lab.IsWhitespace() ) continue;

				for(int im=0; im<Nm;++im){
					if(lab==categ->GetMemBinLabel(im)){
						//cout<<"IsEqual: "<<lab<<" - "<<categ->GetMemBinLabel(im)<<endl;
						fCategCardHisto.SetBinContent(ibx,iby,bin_cont);
					}
				}

			}

		}


	}//params

	for(int ix=1;ix<=fCategCardHisto.GetNbinsX();++ix){
		for(int iy=1;iy<=fCategCardHisto.GetNbinsY();++iy){
			if( fCategCardHisto.GetBinContent(ix,iy)==0){
				fCategCardHisto.SetBinContent(ix,iy,-bin_cont);
			}
		}
	}


	fCategCardHisto.LabelsOption("v","x");

	fCategCardHisto.GetXaxis()->SetLabelOffset( 2*fCategCardHisto.GetXaxis()->GetLabelOffset() );

	if(fCategCardHisto.GetNbinsY()>25){
		fCategCardHisto.GetYaxis()->SetLabelSize(0.75*fCategCardHisto.GetYaxis()->GetLabelSize());
	}

}





double TSNuiParamSet::ComputeWeight(int opt) 
{

	/**
	 *
	 * if opt==0 use prior value
	 * current value otherwise
	 *
	 */

	double w=1;

	int N = GetSize();


	if(N == 0) return w;


	for(int i=LoopStart(); i<LoopNext(); ++i){
		
		TSNuiParam *p = (TSNuiParam*)LoopAt();

		double param_val;

		if( opt==0 ) param_val = p->GetPriorValue();
		else 
		   param_val = p->GetValue();

		if( fRespFuncList.count(i) ){

			param_val = fRespFuncList.find(i)->second->Eval(param_val);
		}

		
		w *= param_val;		
	}

/*
	for(int i=0; i<N; ++i){

		double param_val;

		if( opt==0 ) param_val = ((TSNuiParam*)At(i))->GetPriorValue();
		else 
		   param_val = ((TSNuiParam*)At(i))->GetValue();

		if( fRespFuncList.count(i) ){

			//param_val = fRespFuncList.find(i)->second->Eval(param_val);
		}

		//cout<<"PROC: "<<i<<" "<<__FUNCTION__<<" "<<((TSNuiParam*)At(i))->GetName()<<" val: "<<param_val<<" w: "<<w<<endl;

		w *= param_val;
	}
*/

	fWeight = w;

	return w;
}



double TSNuiParamSet::ComputeWeightFromPrior() 
{

	return ComputeWeight(0);

}


TSRespFunc* TSNuiParamSet::GetParamRespFunc(const TSNuiParam &p) const
{

	if( !fHasRespFuncs ) return 0;

	int idx = GetParamIdx(p);

	if( idx == -1) return 0;

	if( fParHasRespFunc.count(idx) ){
		return fRespFuncList.find(idx)->second;
	}

	return 0;
}


TH1F& TSNuiParamSet::GetHistoOfParams(TString opt){



	opt.ToLower();

	bool use_prior = opt.Contains("prior");
	bool use_cvars = opt.Contains("control-vars");
	bool use_splitline = opt.Contains("split-line");


	TH1F* fHisto = use_prior ? &fHistoPriors: &fHistoParams;


	int Nx = GetSize();


	TString h_name =  use_prior ? "hHistoPriors_" : "hHistoParams_";

	fHisto->SetName(StringUtils::Name(h_name,GetName()));

	fHisto->SetBins(Nx,0.,Nx);

	fHisto->SetTitle( GetTitle() );

	fHisto->GetYaxis()->SetTitle("Value");

	fHisto->SetStats(0);



	for(int i=1; i<=Nx; ++i){

		TSNuiParam *par= ((TSNuiParam*)At(i-1));

		TString par_label = par->GetLabel();

		TString contr_var_label = par->GetControlVarsLabel();

		TString label = par_label;

		if( use_cvars )
			label = StringUtils::Name(par_label," ",contr_var_label);

		if( use_splitline )
			label = StringUtils::SplitLineLabel(par_label,contr_var_label);


		double val = use_prior ? par->GetPriorValue() : par->GetValue();

		double err = par->GetPriorError();

		fHisto->GetXaxis()->SetBinLabel(i,label);

		fHisto->SetBinContent(i,val);

		if(use_prior)
				fHisto->SetBinError(i,err);


	}//

	fHisto->LabelsOption("v","x");

	return *fHisto;



}

TH1F& TSNuiParamSet::GetHistoOfPriors(TString opt)
{


	return GetHistoOfParams(StringUtils::Name("prior ",opt));

}


TSRespFunc* TSNuiParamSet::GetParamRespFunc(int id) const
{

	if( !fHasRespFuncs ) return 0;

	if( fParHasRespFunc.count(id) ){
		return fRespFuncList.find(id)->second;
	}

	return 0;
}



int TSNuiParamSet::GetParamIdx(const TSNuiParam &p) const
{

	for(int ip=LoopStart(); ip<LoopNext(); ++ip){

		TSNuiParam *par = (TSNuiParam*)LoopAt();

		if( par == &p ){
			return ip;
		}

	}


	return -1;

}

bool TSNuiParamSet::HasParameter(const TSNuiParam &p) const
{

	for(int ip=LoopStart(); ip<LoopNext(); ++ip){

		TSNuiParam *par = (TSNuiParam*)LoopAt();

		if( par == &p ){
			return true;
		}

	}


	return false;

}


void TSNuiParamSet::Init(){

	fHasRespFuncs = false;

	fWeight = 1;

	objLink = 0;


}


/** Moved in TSArgList
int TSNuiParamSet::LoopStart()
{


	objLink = FirstLink();

	fLoopStart = true;

	return 0;

}



int TSNuiParamSet::LoopNext()
{

	if(!fLoopStart) objLink = objLink->Next();
	else fLoopStart = false;

	return !objLink ? 0 : GetSize();

}



TObject* TSNuiParamSet::LoopAt()
{

	return !objLink ? 0 : objLink->GetObject();

}
*/


void TSNuiParamSet::PrintLong() const
{

	cout<<"-- TSNuiParamSet::Print(): "<<GetName()<<" size: "<<GetSize()<<" --- "<<endl;

	for(int i=0; i<GetSize();++i){
		TSNuiParam *p=(TSNuiParam*)this->At(i);

		cout<<" "<<i<<"- ";
		p->Print();

	}

	cout<<" ------ "<<endl;
}


void TSNuiParamSet::PrintDifferences(TSNuiParamSet &other)
{

	cout<<__FUNCTION__<<" "<<GetName()<<" "<<other.GetName()<<endl;
	cout<<" entries: "<<GetSize()<<" vs "<<other.GetSize()<<endl;

	for(int i=LoopStart(); i<LoopNext(); ++i){

		TSNuiParam *p = (TSNuiParam*)LoopAt();

		if( !other.HasParameter(*p) ){

			cout<<p->GetName()<<" is not in the second list"<<endl;
		}

	}

	cout<<" "<<endl;
	for(int i=other.LoopStart(); i<other.LoopNext(); ++i){

		TSNuiParam *p = (TSNuiParam*)other.LoopAt();

		if( !HasParameter(*p) ){

			cout<<p->GetName()<<" is not in the caller list"<<endl;
		}

	}

	cout<<" --- "<<endl;

}

void TSNuiParamSet::Print() const
{

	cout<<"-- TSNuiParamSet::Print(): "<<GetName()<<" size: "<<GetSize()<<" --- "<<endl;

		for(int i=0; i<GetSize();++i){
			TSNuiParam *p=(TSNuiParam*)this->At(i);

			cout<<" "<<i<<"- ";

			cout<<p->GetName()<<": "<<p->GetDescription()<<" value: "<<p->GetValue();
			if( p->HasError() ) cout<<" +/-"<<p->GetError();
			if( p->HasAsymErrors() ) cout<<" asymm: "<<p->GetErrorLow()<<" "<<p->GetErrorUp();
			if( p->IsFixed() ) cout<<" (fixed) ";
			if( p->HasControlVars() ){
				cout<<" --Contr. Vars:";
				for(int iv=0;iv<p->GetNumOfControlVars();++iv){
					cout<<p->GetControlVariable(iv)->GetName()<<" "<<p->GetControlVariable(iv)->GetRange();
					if(iv<p->GetNumOfControlVars()-1) cout<<",";
				}
				cout<<endl;
			}

			if( fRespFuncList.find(i) != fRespFuncList.end() ){
				cout<<" Has Response Function. Tweaked value: "<<fRespFuncList.find(i)->second->Eval(p->GetValue())<<endl;
			}

			cout<<endl;
		}//

		cout<<" ------ "<<endl;
}


void TSNuiParamSet::PrintNameSeq() const
{

	cout<<endl;
	for(int i=0;i<GetSize();++i)
	{
		cout<< ((TSNuiParam*)At(i))->GetName();
		if(i<GetSize()-1) cout<<", ";
	}
	cout<<endl;

}

void TSNuiParamSet::Randomize()
{

	for(int i=LoopStart(); i<LoopNext(); ++i){
		( (TSNuiParam*)LoopAt() )->Randomize();
	}



	/*
	for( int i=0; i<GetSize(); ++i){

		( (TSNuiParam*)At(i) )->Randomize();

	}
	 */


}

void TSNuiParamSet::Restore()
{


	for(int i=LoopStart(); i<LoopNext(); ++i){
		( (TSNuiParam*)LoopAt() )->Restore();
	}

	/*
	for( int i=0; i<GetSize(); ++i){

		( (TSNuiParam*)At(i) )->Restore();

	}
	 */

}


