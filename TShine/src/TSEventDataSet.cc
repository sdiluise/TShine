/*
 * TSEventDataSet.cc
 *
 *  Created on: Oct 26, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <unordered_map>

#include "RootUtils.h"

#include "TSEventDataSet.h"

TSEventDataSet::TSEventDataSet() {

	Init();

}

TSEventDataSet::TSEventDataSet(TString name, TString title)
:TSNamed(name,name,title)
{

	Init();

}


TSEventDataSet::~TSEventDataSet() {


}



void TSEventDataSet::Add(TSEventDataSet &set)
{

	for(int i=0; i<set.GetEntries(); ++i){

		AddEvent( set.GetEvent(i) );
	}


}

void TSEventDataSet::AddEvent(TSEvent *eve)
{

	if( !eve ) return;

	fEventArray.Add(eve);

	fNumOfEvents++;

	if( eve->HasWeightParams() ){

		AddNuiParams( eve->GetNuiParamSet() );

	}


}


void TSEventDataSet::AddNuiParams(TSNuiParamSet &set){


	for(int i=set.LoopStart(); i<set.LoopNext(); ++i){

		TSNuiParam *p = (TSNuiParam*)set.LoopAt();

		if( !fNuiParam_map.count(p) ){

			fNuiParam_map[p]=fNuiParam_map.size()+1;

			fNuiParam_Set.Add(p);

			fHasWeightParams = true;

			//add to the param list
			TSParameter *par = (TSParameter*)set.LoopAt();
			fParamList.Add(par);
			fParamList_Nui.Add(par);

			fHasListOfParams = true;
		}

	}


}




void TSEventDataSet::BuildCategoryCardHisto()
{

	if(fCategCardHisto.GetNbinsX()==1 && TString(fCategCardHisto.GetXaxis()->GetBinLabel(1)).IsWhitespace() ){

		fCategCardHisto.SetName(StringUtils::Name("hEventDataSet_CategCard_",GetName()));

		fCategCardHisto.SetTitle(GetTitle());

		fCategCardHisto.GetYaxis()->SetTitle("Events");

		fCategCardHisto.SetStats(0);

	}


	double offset_x = 0.;

	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		if( !event->HasCategoryCard() ) continue;

		TSCategoryCard card = event->GetCategoryCard();

		for(int i=0; i<card.GetNumOfCategories();++i){

			TSCategory *categ = card.GetCategory(i);
			TString descr= categ->GetDescription();
			TSCategory mother = categ->GetMotherCategory();

			TH1F *h = &mother.GetHistogram();

			int Nold = fCategCardHisto.GetNbinsX();

			int Ncateg = h->GetNbinsX();


			bool is_new = true;
			bool is_first = false;

			for(int ib=1; ib<=Nold ;++ib){

				TString lab_start = fCategCardHisto.GetXaxis()->GetBinLabel(ib);

				TString lab_end = fCategCardHisto.GetXaxis()->GetBinLabel(ib+Ncateg-1);

				if(lab_start.IsWhitespace() && Nold==1){is_first=true; break;}

			    TString labnew_start = h->GetXaxis()->GetBinLabel(1);
			    TString labnew_end = h->GetXaxis()->GetBinLabel(Ncateg);


				if(lab_start == labnew_start ){
					if(lab_end == labnew_end)
					{
						is_new = false; break;
					}
				}

			}//ib

			if(is_new){

				if(!is_first){

					int Nnew = Nold +1 +Ncateg;

					std::vector<float> cont;
					std::vector<TString> labels;

					for(int ib=1; ib<=Nold; ++ib){
						cont.push_back(fCategCardHisto.GetBinContent(ib));
						labels.push_back(fCategCardHisto.GetXaxis()->GetBinLabel(ib));
					}


					fCategCardHisto.SetBins(Nnew,offset_x,Nnew+offset_x);

					for(int ib=1;ib<=Ncateg;++ib){
						fCategCardHisto.GetXaxis()->SetBinLabel(Nold+1+ib,h->GetXaxis()->GetBinLabel(ib));
					}

				}else{

					Nold = 0;
					int Nnew = Ncateg;

					fCategCardHisto.SetBins(Nnew,offset_x,Nnew+offset_x);

					for(int ib=1;ib<=Ncateg;++ib){
						fCategCardHisto.GetXaxis()->SetBinLabel(ib,h->GetXaxis()->GetBinLabel(ib));
					}

				}//

			}//add new set of bins


			//fill histo

			for(int im=0; im<categ->GetNumOfMembers();++im){

				fCategCardHisto.Fill( categ->GetMemBinLabel(im), 1. );

			}

		}//categ loop


	}



	fCategCardHisto.LabelsOption("v","x");

	fCategCardHisto.GetXaxis()->SetLabelOffset( 2*fCategCardHisto.GetXaxis()->GetLabelOffset() );


}

void TSEventDataSet::BuildCategoryCardMatrix()
{

	cout<<__FUNCTION__<<endl;

	fCategCardMatrix.SetName(StringUtils::Name("hEventDataSet_CategCardMatrix_",GetName()));

	fCategCardMatrix.SetTitle(GetTitle());

	fCategCardMatrix.SetStats(0);


	//
	// Build 1D Table first
	//
	BuildCategoryCardHisto();


	int N = fCategCardHisto.GetNbinsX();

	fCategCardMatrix.SetBins(N,0.,N,N,0.,N);



	for(int i=1;i<=N;++i){

		fCategCardMatrix.GetXaxis()->SetBinLabel(i,fCategCardHisto.GetXaxis()->GetBinLabel(i));
		fCategCardMatrix.GetYaxis()->SetBinLabel(i,fCategCardHisto.GetXaxis()->GetBinLabel(i));

	}


	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = GetEvent(iev);

		if( !event->HasCategoryCard() ) continue;

		TSCategoryCard card = event->GetCategoryCard();

		for(int cat1=0; cat1<card.GetNumOfCategories();++cat1){
			for(int cat2=0; cat2<card.GetNumOfCategories();++cat2){

				//if(cat1==cat2) continue;

				TSCategory *categ1 = card.GetCategory(cat1);

				TSCategory *categ2 = card.GetCategory(cat2);

				TSCategory mother1 = categ1->GetMotherCategory();
				TSCategory mother2 = categ2->GetMotherCategory();

				for(int im1=0; im1<categ1->GetNumOfMembers();++im1){
				for(int im2=0; im2<categ2->GetNumOfMembers();++im2){

					fCategCardMatrix.Fill( categ1->GetMemBinLabel(im1), categ2->GetMemBinLabel(im2), 1. );

				}
				}

			}
		}

	}



	fCategCardMatrix.LabelsOption("v","x");

	fCategCardMatrix.GetXaxis()->SetLabelOffset( 2*fCategCardMatrix.GetXaxis()->GetLabelOffset() );
	fCategCardMatrix.GetYaxis()->SetLabelOffset( 2*fCategCardMatrix.GetYaxis()->GetLabelOffset() );



}


void TSEventDataSet::Clear()
{

	fEventArray.Clear();

	fNumOfEvents = 0;
	fEffNumOfEvts = 0;

	fHasWeightParams = false;
	fHasListOfPoI = false;
	fHasListOfParams = false;
	fSavedWeightPosition = -1;

	fNuiParam_map.clear();

	fNuiParam_Set.Clear();

	fParamList.Clear();
	fParamList_Nui.Clear();
	fParamList_PoI.Clear();

	fSelectedHistogram = 0;
	fProcComputeWeight = true;
	fProcOscillate = true;

	ClearCategoryCardHisto();

}

void TSEventDataSet::ClearCategoryCardHisto()
{
	fCategCardHisto.Reset();

	for(int i=1; i<=fCategCardHisto.GetNbinsX(); ++i){
		fCategCardHisto.GetXaxis()->SetBinLabel(i,"");
	}

}


double TSEventDataSet::ComputeEffectiveNumOfEvts()
{
	fEffNumOfEvts = 0;

	for(int i=0;i<fNumOfEvents;++i){

		TSEvent *eve = GetEvent(i);

		fEffNumOfEvts += eve->GetWeight();
	}


	return fEffNumOfEvts;

}


void TSEventDataSet::ComputeWeight()
{

	for(int i=0; i<fNumOfEvents; ++i){

		GetEvent(i)->ComputeWeight();
	}

}


void TSEventDataSet::FillHistogram(TH1 &h ,int iv){

	double sum = 0;
	double sumw2 = 0;

	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		h.Fill( event->GetVarValue(iv), event->GetWeight() );
	}



}

void TSEventDataSet::FillHistogram(TH1 &h ,const TSVariable &v){


	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		h.Fill( event->GetVarValue(v), event->GetWeight() );

	}



}

void TSEventDataSet::FillHistogram(TH2 &h ,const TSVariable &vx, const TSVariable &vy){

	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		h.Fill( event->GetVarValue(vx), event->GetVarValue(vy), event->GetWeight() );
	}


}

void TSEventDataSet::FillHistogramWithSelection(TH1 &h ,const TSVariable &v, const TSCategoryCard &card_sel){



	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		TSCategoryCard card_eve = event->GetCategoryCard();

		if( !card_eve.Compare(card_sel) ) continue;

		h.Fill( event->GetVarValue(v), event->GetWeight() );



	}

}

void TSEventDataSet::FillHistograms()
{



	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));

		//cout<<__FUNCTION__<<" "<<event->GetId()<<endl;
		//event->FillValues();

		//std::unordered_map<const TSVariable*,double> mm = event->FillValues();



		fSample.Fill( event->FillValues(), event->GetWeight() );


		//cout<<"WW "<<event->GetWeightNative()<<" "<<event->GetWeight()<<endl;
	}


}


void TSEventDataSet::FillHistogramsNoWeight()
{
	//
	// keep as separated method for performances reason
	//
	for(int iev=0; iev<fNumOfEvents; ++iev){

		TSEvent *event = static_cast<TSEvent*>(fEventArray.UncheckedAt(iev));


		fSample.Fill( event->FillValues(), 1.);

	}


}


TSEvent* TSEventDataSet::FindEventId(int id)
{

	for(int i=0; i<fNumOfEvents;++i){

		TSEvent *eve = static_cast<TSEvent*>(fEventArray.UncheckedAt(i));

		if(eve->GetId() == id) return eve;
	}

	return 0;

}



TSEvent* TSEventDataSet::GetEvent(int i)
{

	//if(i<0 || i>fNumOfEvents-1){
	//	MSG::ERROR(__FILE__,"::",__FUNCTION__," index of out bounds ",i,". Max entries: ",fNumOfEvents);
	//	return 0;
	//}

	//return (TSEvent*)fEventArray.UncheckedAt(i);

	return static_cast<TSEvent*>(fEventArray.UncheckedAt(i));

}


TH1F& TSEventDataSet::GetHistogram(const TSVariable &vx)
{

	return *fSample.GetHistogram(vx);
}

TH2F& TSEventDataSet::GetHistogram2D(const TSVariable &vx, const TSVariable &vy)
{

	return *fSample.GetHistogram2D(vx,vy);
}



TH1* TSEventDataSet::GetHistogram()
{


	return fSelectedHistogram;

}


int TSEventDataSet::GetVarIdx(TSVariable &var)
{

	return fSample.GetVarIdx(var);

}


void TSEventDataSet::Init()
{

	Clear();


	fNuiParam_Set.SetName(StringUtils::Name("nuiParamSet_evDataSet_",GetName()));

	fParamList.SetName(StringUtils::Name("paramList_evDataSet_",GetName()));
	fParamList_PoI.SetName(StringUtils::Name("paramListPoI_evDataSet_",GetName()));
	fParamList_Nui.SetName(StringUtils::Name("paramListNui_evDataSet_",GetName()));


	fSample.SetName(StringUtils::Name("sample",GetName()));

}


void TSEventDataSet::Oscillate()
{
	for(int i=0; i<fNumOfEvents; ++i){

		GetEvent(i)->Oscillate();
	}

}

void TSEventDataSet::Process()
{

	/**
	 * to be optimized...
	 *
	 * create a loop for each "if" cobination
	 *  ....
	 */

	if(fSelectedHistogram) fSelectedHistogram->Reset();


	for(int i=0; i<GetNumOfEvents(); ++i){

		TSEvent *eve = GetEvent(i);

		eve->RestoreWeightNative();

		if(fProcComputeWeight){
			//if(i==0) cout<<"ok "<<i<<endl;
			eve->ComputeWeight();
		}

		if(fProcOscillate) eve->Oscillate();

		double val_x = eve->GetVarValue(0);//*fSelectedHistoVar[0]);

		fSelectedHistogram->Fill(val_x,eve->GetWeight());

		/*
		if(fSelectedHistoDim==1) {

			fSelectedHistogram->Fill(val_x,eve->GetWeight());
		}else{

			double val_y = eve->GetVarValue(*fSelectedHistoVar[1]);

			((TH2*)fSelectedHistogram)->Fill(val_x,val_y,eve->GetWeight());
		}
		*/

	}

}

void TSEventDataSet::ProcessEventsWith(double (TSEvent::*method)(int) ){

	for(int i=0; i<GetNumOfEvents(); ++i){

		TSEvent *eve = GetEvent(i);

		(eve->*method)(1);
	}

}


void TSEventDataSet::ResetHistograms()
{

	fSample.Reset();

}



void TSEventDataSet::ReWeightWithSavedPosition(int weight_id){

	int id = weight_id;

	if(id == -1){

		if(fSavedWeightPosition==-1){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," No weight position saved for reweighting");
			return;
		}else{
			id = fSavedWeightPosition;
		}
	}

	for(int i=0; i<fNumOfEvents; ++i){

		GetEvent(i)->ReWeightWithSavedPosition(id);
	}


}



void TSEventDataSet::SelectHistogram(const TSVariable &vx)
{

	if( !fSample.GetHistogram(vx) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram for variable ",vx.GetName()," not found");
		exit(0);
	}

	fSelectedHistoDim = 1;

	fSelectedHistogram = fSample.GetHistogram(vx);

	fSelectedHistoVar[0] = 0;
	fSelectedHistoVar[1] = 0;

	fSelectedHistoVar[0] = (TSVariable*)&vx;

}


void TSEventDataSet::SelectHistogram(const TSVariable &vx,const TSVariable &vy)
{

	if( !fSample.GetHistogram2D(vx,vy) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram not found ");
		MSG::ERROR(" variable names, x:",vx.GetName()," y:", vy.GetName());
		exit(0);
	}


	fSelectedHistoDim = 2;

	fSelectedHistogram = fSample.GetHistogram2D(vx,vy);

	fSelectedHistoVar[0] = 0;
	fSelectedHistoVar[1] = 0;

	fSelectedHistoVar[0] = (TSVariable*)&vx;
	fSelectedHistoVar[1] = (TSVariable*)&vy;

}



void TSEventDataSet::SetHistogram(const TSVariable &v, int n, double min, double max)
{

	fSample.AddVariable(v,n,min,max);

}
void TSEventDataSet::SetHistogram(const TSVariable &v, int n, double *bin)
{

	fSample.AddVariable(v,n,bin);

	//fVarMatch[]

}
void TSEventDataSet::SetHistogram(const TSVariable &vx, const TSVariable &vy, int nx, double *binx, int ny, double *biny)
{

	fSample.AddVariable(vx,nx,binx);
	fSample.AddVariable(vy,ny,biny);
	fSample.SetBidimensional(vx,vy);

}

void TSEventDataSet::SetHistogram(const TSVariable &vx, const TSVariable &vy, int nx, double xmin, double xmax, int ny, double ymin, double ymax)
{

	fSample.AddVariable(vx,nx,xmin,xmax);
	fSample.AddVariable(vy,ny,ymin,ymax);
	fSample.SetBidimensional(vx,vy);


}

void TSEventDataSet::SetHistogram(const TSVariable &vx, const TSVariable &vy, int nx, double xmin, double xmax, int ny, double *biny)
{

	fSample.AddVariable(vx,nx,xmin,xmax);
	fSample.AddVariable(vy,ny,biny);
	fSample.SetBidimensional(vx,vy);

}


void TSEventDataSet::SetHistogram(const TSVariable &vx, const TSVariable &vy, int nx, double *binx, int ny, double ymin, double ymax)
{

	fSample.AddVariable(vx,nx,binx);
	fSample.AddVariable(vy,ny,ymin,ymax);
	fSample.SetBidimensional(vx,vy);

}

void TSEventDataSet::SetListOfPoI(const TSParamList &list)
{

	fParamList_PoI.Clear();

	fParamList_PoI.Add(list);

	fParamList.AddOnce(list);

	if(fParamList_PoI.GetSize()>0){
		fHasListOfPoI = true;
		fHasListOfParams = true;
	}

}

void TSEventDataSet::SetWeightPosition(int weight_id){

	fSavedWeightPosition = weight_id;
}


void TSEventDataSet::UpdateParamList()
{


	for(int i=0; i<fNumOfEvents; ++i){

		TSEvent *eve=GetEvent(i);

		if( eve->HasWeightParams() ){

			AddNuiParams( eve->GetNuiParamSet() );

		}

	}


}
