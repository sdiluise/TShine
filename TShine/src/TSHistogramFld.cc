/*
 * TSHistogramFdr.cc
 *
 *  Created on: Jun 2, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 */


#include <TString.h>

#include <TH1F.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TH2Poly.h>


#include "RootUtils.h"

#include "TSHistogramFld.h"

#include "MessageMgr.h"


TSHistogramFld::TSHistogramFld() {

	Init();

}

TSHistogramFld::TSHistogramFld(TString name)
{

	Init();

	TList::SetName( name );

}

TSHistogramFld::~TSHistogramFld() {

	Clear();
}


void TSHistogramFld::Add(TObject *h, TString var_x, TString var_y){


	if( !h ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__, " adding NULL pointer");
		return;
	}

	TString name= h->GetName();


	int DIM=1;

	if( name.IsWhitespace() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__, " object name is empty ");
		return;
	}

	if( var_x.IsWhitespace() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__, " variable x name is empty ");
		return;
	}


	if(  (h->InheritsFrom("TH2") || h->InheritsFrom("TH2Poly"))  ){

		if( var_y.IsWhitespace() ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__, " variable-y name is empty in 2D Histo");
			return;
		}

		DIM=2;
	}


	if(  h->InheritsFrom("TH2")  ){

		ApplyTH2Style( (TH2*)h );

	}


	//if( fObjPos.count(name)==1 ){
	if( FindObject(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__, " object with name ",name," already present");
		return;
	}



	TList::Add( (TObject*)h );

	if( fSumw2 ){
		((TH1*)h)->Sumw2();
	}

	int idx=GetSize();

	fObjPos[name]=idx;
	fObjName[idx]=name;

	fObjNameX[idx]=var_x;
	fObjNameY[idx]=var_y;


	fHistoSettings *fHisto = new fHistoSettings;

	fHisto->Position=idx;
	fHisto->Name=name;
	fHisto->Dim=DIM;

	fHisto->NameX=var_x;
	fHisto->NameY=var_y;

	fHistoSetsList[idx]=fHisto;

	//cout<<__FILE__<<"::"<<__FUNCTION__<<" ===> "<<h->GetName()<<endl;

}


void TSHistogramFld::ApplyTH2Style(TH2 *h2) {

	if(!h2) return;

	h2->GetXaxis()->SetTitleOffset( 1.5*h2->GetXaxis()->GetTitleOffset()  );
	h2->GetYaxis()->SetTitleOffset( 1.5*h2->GetYaxis()->GetTitleOffset()  );

}


void TSHistogramFld::AppendToAllNames(TString s, TString sep) {


  for(int i=0; i<GetSize(); ++i){

	  TString name= ((TNamed*)At(i))->GetName();
      name+=sep;
	  name+=s;

	  ((TNamed*)At(i))->SetName(name);

  }


}


void TSHistogramFld::AppendToAllTitles(TString s, TString sep) {

	for(int i=0; i<GetSize(); ++i){

		  TString title= ((TH1*)At(i))->GetTitle();

		  if( !title.IsWhitespace() ) title+=sep;
		  title+=s;

		  ((TH1*)At(i))->SetTitle(title);

	  }



}

TSHistogramFld* TSHistogramFld::Clone(TString name) const {


	TSHistogramFld *fld=new TSHistogramFld(name);

	int N=GetSize();

	for(int i=0; i<N; ++i){

		fld->Add( At(i)->Clone(), GetHistoNameX(i+1), GetHistoNameY(i+1) );

	}


	return fld;
}



void TSHistogramFld::Clear(Option_t* opt){


	TList::Clear(opt);

	fObjPos.clear();
	fObjName.clear();
	fObjNameX.clear();
	fObjNameY.clear();

	for( std::map<int,fHistoSettings*>::iterator it=fHistoSetsList.begin(), end=fHistoSetsList.end();
			it!=end;
			++it
	){

		delete it->second;
	}


}



void TSHistogramFld::Copy(TSHistogramFld& other) {

	SetName( other.GetName() );

	for(int i=0; i<other.GetSize();++i){

		Add( (other.At(i))->Clone(), other.GetHistoNameX(i+1), GetHistoNameY(i+1) );

	}


}



int TSHistogramFld::Fill(std::vector<TString>& var_list, std::vector<double>& values){


	int N=var_list.size();

	if(N<=0) return 0;

	int NH=GetSize();

	//Print();

	for(int ih=1; ih<=NH; ++ih){

		TObject *obj=GetObject(ih);

		//cout<<"List of Histos: "<<NH<<"; "<<ih<<" "<<obj->GetName()<<endl;

		bool isTH1= IsTH1(*obj);//
		bool isTH2= IsTH2(*obj);//
		bool isTH2Poly=IsTH2Poly(*obj);//

		//bool isTH1= obj->InheritsFrom("TH1") && !obj->InheritsFrom("TH2") && !obj->InheritsFrom("TH2Poly");
		//bool isTH2=obj->InheritsFrom("TH2") && !obj->InheritsFrom("TH2Poly");
		//bool isTH2Poly=obj->InheritsFrom("TH2Poly");

		if( isTH1 ){

			for(int ivar=0; ivar<N; ++ivar){
				if(var_list[ivar]==GetHistoNameX(ih)){
					( (TH1*)obj )->Fill( values[ivar] );
					break;
				}
			}


		}else if( isTH2 || isTH2Poly ){

			for(int ivarx=0, is_found=false; ivarx<N && !is_found; ++ivarx){


				for(int ivary=ivarx+1; ivary<N && !is_found; ++ivary){

					double vx, vy;

					if(var_list[ivarx]==GetHistoNameX(ih) && var_list[ivary]==GetHistoNameY(ih)){
						vx=values[ivarx];
						vy=values[ivary];
						is_found=true;
					}else if(var_list[ivarx]==GetHistoNameY(ih) && var_list[ivary]==GetHistoNameX(ih)){
						vx=values[ivary];
						vy=values[ivarx];
						is_found=true;
					}

					if(is_found){

						if( isTH2 ) ((TH2*)obj)->Fill(vx,vy);
						else if( isTH2Poly ) FillPoly((TH2Poly*)obj,vx,vy);

 					}

				}//iy
			}//ix


		}


	}//NH


	return 0;

}



int TSHistogramFld::Fill(int N, TString* var_list, double *values){

    ///To be Implemented

	return 1;

}



int TSHistogramFld::Fill(TString vx, TString vy, double x, double y, double w, int opt) {


	///To be implemented

	return 1;
}



int TSHistogramFld::Fill(TString vx, double x, double w, int opt) {


	///To be Implemented

	std::vector<TString> name;
	std::vector<double> val;

	name.push_back(vx);
	val.push_back(x);


	return Fill(name,val);

}




int TSHistogramFld::AddPolyBinContent(TH2Poly& h2, int ibin, double w){


	/*
	int N=h2.GetNumberOfBins();


	TList *bin_list=h2.GetBins();

	if( !bin_list ) return 0;

	TH2PolyBin *bin= (TH2PolyBin*)bin_list->At(ibin-1);
	 */

	h2.AddBinContent(ibin-1,w);

}


int TSHistogramFld::FillPoly(TH2Poly* h2, double x, double y, double w, int opt){

/**
 *
 *
 *  Default option is opt==0 or opt==1:
 *  a loop is executed over all the bins,
 *  if the point (x,y) is inside the bin the
 *  bin content is incremented by w (default=1).
 *  In this way it is assured that two (or more)
 *  overlapping bins are both incremented if they
 *  both contains the point (x,y).
 *
 *  Option opt==0 used a modified version of IsInside that
 *  matched the same logic used in TSvariable to check if
 *  a value x is inside the range: [xmin,xmax : xmin <= x <xmax
 *  In TH2Poly in fact the point x is not considred inside is it
 *  is on the left margin of the intervals i.e. on the left sides of
 *  the bin rectangular. This couses discrepancies in the number of entries
 *  between the map bins represented via TH2Poly and the bins
 *  represented via TH1F/TH2F etc..
 *
 *  If opt==2 default TH2Poly::Fill is called which used the same IsInside method
 *  of option 1 and do not fill overlapping bins (only the first one)
 *
 *
 *
 */

	//cout<<"FillPoly "<<h2->GetName()<<" "<<x<<" "<<y<<" "<<opt<<endl;

	if(opt==2){
		return h2->Fill(x,y,w);
	}

	int N=h2->GetNumberOfBins();


	TList *bin_list=h2->GetBins();

	if( !bin_list ) return 0;

	for(int i=0; i<N; ++i){

		TH2PolyBin *bin= (TH2PolyBin*)bin_list->At(i);

		if( opt == 0){

			TGraph *bin_poly=(TGraph*)bin->GetPolygon();

			bool is_in = RootUtils::IsInsideRectangular(bin_poly,x,y);

			if( is_in ){

				//cout<<"ISIN "<<GetName()<<" "<<x<<" "<<y<<" bin num: "<<i<<endl;
				h2->AddBinContent(i,w);
			}


		}
		else if( opt==1 ){

			if( bin && bin->IsInside(x,y) ){

				bin->Fill(w);
			}
		}//opt

	}//bin loop


	return 1;
}


int TSHistogramFld::FillHistosFromFile(TFile *file, TString opt) {


	if( !file ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," null ptr to file");
		return 0;
	}


	for(int i=0; i<GetSize(); ++i){

		TObject *obj = At(i);

		TString hname = obj->GetName();

		TObject *obj_2 = file->Get(hname);

		if( !obj_2 ){
			MSG::WARNING("\t histo: ",hname," not found in file: ",file->GetName());
			continue;
		}

		//cout<<" Filling "<<hname<<endl;


		if( IsTH1(*obj) ){

			static_cast<TH1*>(obj)->Add( static_cast<TH1*>(obj_2) );

		}else if( IsTH2(*obj) ){

			static_cast<TH2*>(obj)->Add( static_cast<TH2*>(obj_2) );

		}else if( IsTH2Poly(*obj) ){

			MSG::WARNING(__FILE__,"::",__FUNCTION__," function not implemented yet for TH2Poly. Not filling: ",obj->GetName());

		}

	}

	return 1;

}


int TSHistogramFld::GetHistoDim(int pos) const {

	if(  fObjName.count(pos)!=1 ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," index out of list range");
		return 0;
	}

	TObject *obj = GetObject(pos);

	if( IsTH1(*obj) ) return 1;
	else if( IsTH2(*obj) || IsTH2Poly(*obj) ) return 2;
	else return 0;

}


TString TSHistogramFld::GetHistoName(int pos) const {

	TString dummy="";

	if(  fObjName.count(pos)!=1 ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," index out of list range");
		return dummy;
	}

    return fObjName.find(pos)->second;


}



TString TSHistogramFld::GetHistoNameX(int pos) const {

	TString dummy="";

	if(  fObjNameX.count(pos)!=1 ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," index out of list range");
		return dummy;
	}

    return fObjNameX.find(pos)->second;


}



TString TSHistogramFld::GetHistoNameY(int pos) const {

	TString dummy="";

	if(  fObjNameY.count(pos)!=1 ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," index out of list range");
		return dummy;
	}

	 return fObjNameY.find(pos)->second;


}



TList* TSHistogramFld::GetListOf(TString type) const {


	TList *list=new TList();

	TString name=GetName();

	if(name.IsWhitespace() ) name="List";

	name+="_"; name+=type;

	list->SetName(name);

	for(int i=0; i<GetSize();++i){

		TObject *obj = At(i);

		if( obj->InheritsFrom(type) ){
			list->Add(obj);
		}

	}

	return list;

}



TObject* TSHistogramFld::GetObject(int pos) const {

	if(pos<=0 || pos>GetSize() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," index out of list range");
		return 0;
	}

	return At(pos-1);


}

float TSHistogramFld::GetOutliersFrac(TString var_x, TString var_y) const {

	TObject *obj = GetTagHisto(var_x,var_y);

	if( !obj ) return 0;

	if( obj->InheritsFrom("TH2Poly") ){
		return 0;
	}

	TH2 *hh= (TH2*)obj;

	float tot_ent=hh->GetEntries();

	if(tot_ent==0) return 0;

	float eff_ent = hh->GetEffectiveEntries();

	float out_ent= tot_ent-eff_ent ;

	float frac = out_ent/tot_ent;

	//cout<<__FUNCTION__<<" "<<obj->GetName()<<" "<<eff_ent<<" "<<tot_ent<<endl;

	return frac;

}


TObject* TSHistogramFld::GetTagHisto(TString var_x, TString var_y) const {

	int N=GetSize();

	bool is2D=true;
	if(var_y.IsWhitespace()) is2D=false;

	for(int i=1; i<=N; ++i){

		if( !is2D && GetHistoNameX(i)==var_x ){

			return GetObject(i);

		}else {

			if(GetHistoNameX(i)==var_x && GetHistoNameY(i)==var_y){
				return GetObject(i);
			}

		}


	}

	return 0;

}




int TSHistogramFld::Import(TSHistogramFld& other) {


	for(int i=0; i<other.GetSize(); ++i){
		Add( other.GetObject(i+1)->Clone(), other.GetHistoNameX(i+1), other.GetHistoNameY(i+1) );
	}

	return 1;

}



void TSHistogramFld::Init(){

	SetOwner();

	SetSumw2(false);


}

bool TSHistogramFld::IsTH1(const TObject& obj) const {

	return obj.InheritsFrom("TH1") && !IsTH2(obj) && !IsTH2Poly(obj);

}

bool TSHistogramFld::IsTH2(const TObject& obj) const {

	return obj.InheritsFrom("TH2") && !IsTH2Poly(obj);

}


bool TSHistogramFld::IsTH2Poly(const TObject& obj) const {

	 return obj.InheritsFrom("TH2Poly");

}

void TSHistogramFld::PrependToAllNames(TString s, TString sep) {


	for(int i=0; i<GetSize(); ++i){

		  TString name=s; name+=sep;
		  name+=((TNamed*)At(i))->GetName();

		  ((TNamed*)At(i))->SetName(name);

	  }

}

void TSHistogramFld::PrependToAllTitles(TString s, TString sep) {


	for(int i=0; i<GetSize(); ++i){

		TString title=s;

		TString tit0=((TH1*)At(i))->GetTitle();

		if( !tit0.IsWhitespace() ){
			title+=sep;
			title+=tit0;
		}


		((TH1*)At(i))->SetTitle(title);

	}


}

void TSHistogramFld::ReplaceToAllTitles(TString s) {

	for(int i=0; i<GetSize(); ++i){

		((TH1*)At(i))->SetTitle(s);

	}

}


void TSHistogramFld::RenameAllWithBase(TString base, TString sep) {

	for(int i=0; i<GetSize(); ++i){

		TString name=base;
		name+=sep;

		TString name_x=fHistoSetsList[i+1]->NameX;

		name+=name_x;

		if( fHistoSetsList[i+1]->Dim==2 ){
			TString name_y=fHistoSetsList[i+1]->NameY;
			name+="_";
			name+=name_y;
		}

		((TH1*)At(i))->SetName(name);

	}

}


void TSHistogramFld::ResetHistos(){


	for(int i=0; i<GetSize();++i){

		TObject *obj = At(i);

		if( IsTH1(*obj) ){
			((TH1*)obj)->Reset();
		}
		else if( IsTH2(*obj) ){
			((TH2*)obj)->Reset();
		}
		else if( IsTH2Poly(*obj) ){
			((TH2Poly*)obj)->TH2::Reset();
		}

	}


}

void TSHistogramFld::SetSumw2(bool opt){

	fSumw2=opt;

	//update existing histos
	if( fSumw2 ){

		for(int i=0; i<GetSize(); ++i){
			((TH1*)At(i))->Sumw2();
		}

	}


}

