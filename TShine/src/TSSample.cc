/*
 * TSSample.cc
 *
 *  Created on: Sep 26, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>

#include <TObject.h>
#include <TMath.h>
#include <TRandom.h>
#include <TCut.h>

#include "RootUtils.h"
#include "StringUtils.h"

#include "MessageMgr.h"

#include "TSSample.h"



#define VERBOSE true
#ifdef VERBOSE
#define V(x) cout<<x<<endl;
#else
#define V(x)
#endif



TSSample::TSSample() {


	Init();
}


TSSample::TSSample(TString name, TString label, TString title)
:TSNamed(name,label,title)
{

	Init();
}

TSSample::~TSSample() {


	//delete histos
	for(int i=0; i<hListAll.GetSize(); ++i){

		TObject *o = hListAll.At(i);

		delete o;
	}



	/*
	std::vector<TSHistoNuiParamMgr*> fHistoNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHistoOscillProbMgr;
	std::vector<TSHistoNuiParamMgr*> fHistoToyNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHistoToyOscillProbMgr;

	std::vector<TSHistoNuiParamMgr*> fHisto2DNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHisto2DOscillProbMgr;
	std::vector<TSHistoNuiParamMgr*> fHisto2DToyNuiParMgr;
	std::vector<TSHistoOscillMgr*> fHisto2DToyOscillProbMgr;
	 */

	for(int i=0;i<fHistoNuiParMgr.size();++i){

		delete fHistoNuiParMgr[i];
		delete fHistoOscillProbMgr[i];
		delete fHistoToyNuiParMgr[i];
		delete fHistoToyOscillProbMgr[i];
	}


	for(int i=0; i<fHisto2DNuiParMgr.size();++i){

		delete fHisto2DNuiParMgr[i];
		delete fHisto2DOscillProbMgr[i];
		delete fHisto2DToyNuiParMgr[i];
		delete fHisto2DToyOscillProbMgr[i];
	}

}


void TSSample::AddVariable(const TSVariable &var, int nbins, double min, double max, TString opt)
{


	AddVariable(var,nbins,RootUtils::CreateBinsArray(nbins,min,max),opt);

}


void TSSample::AddVariable(const TSVariable &var, int nbins, double *bins, TString opt)
{


	for(int i=0; i<fVarList.size();++i){

		if( fVarList[i] == &var ){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," Variable: ",var.GetName()," already added");
			return;
		}

	}




	TAxis *a = new TAxis(nbins,bins);

	TString hname=StringUtils::Name("h","_",GetName(),"_",var.GetName());

	TString htit=GetTitle(); //Add CategoryCard label

	TH1F *h = new TH1F(hname,htit,nbins,bins);

	h->GetXaxis()->SetTitle( var.GetAxisTitle() );
	h->GetYaxis()->SetTitle("Entries");

	fVarListAll.push_back( &var );

	if(opt=="Aux"){

		fVarHistoAux.push_back(h);
		hListAux.Add(h);

		fNumOfAuxVars++;

		hListAll.Add(h);

		fVarType.push_back(2);

		fAuxVarList.push_back( &var );

		fAuxVarAxis.push_back(a);



	}else{

		fVarList.push_back( &var );

		TH1F *htoy = new TH1F(*h);
		htoy->SetName(hname+"_toy");


		fVarHisto.push_back(h);
		fVarHistoToy.push_back(htoy);

		hList.Add(h);
		hListToy.Add(htoy);

		hListAll.Add(h);
		hListAll.Add(htoy);

		fVarType.push_back(1);

		fVarAxis.push_back(a);


		fHistoNuiParMgr.push_back(0);
		fHistoOscillProbMgr.push_back(0);
		fHistoToyNuiParMgr.push_back(0);
		fHistoToyOscillProbMgr.push_back(0);
	}


	fNumOfVars = fVarList.size();


	delete [] fVarCache;
	fVarCache = new double[fNumOfVars];

	for(int i=0; i<fNumOfVars;++i) fVarCache[i]=-2;

	if( IsUnbinned() ){
		fUpdateDataTreeBranches();
	}



}


void TSSample::AddVariables(const TSArgList &var_list)
{


	int N = var_list.GetSize();

	if( N==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Empty list");
		return;
	}

	for(int i=0; i<N; ++i){

		TSVariable *var=(TSVariable*)var_list.At(i);

		if( !var ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Null ptr to Var num: ",i);
			continue;
		}

		AddVariable(*var,1,0,1);

	}


	if( IsUnbinned() ){
		fUpdateDataTreeBranches();
	}


}


int TSSample::CompareCategoryCard(const TSCategoryCard &other, int opt)
{

	int status = fCard.Compare(other);



	return status;
}


void TSSample::Fill(std::map<const TSVariable*,double>& map, double w)
{

	fEventWeight = w;

    //cout<<"fill "<<map.size()<<" "<<fNumOfVars<<endl;

	//int n=0;
	//for(std::unordered_map<const TSVariable*,double>::iterator it=map.begin(), end=map.end();it!=end;++it){

		//cout<<it->first<<" "<<it->second<<endl;
	//	n++;
	//}

	//UNORD
	std::map<const TSVariable*,double>::iterator it;

	for(int i=0; i<fNumOfVars;++i){

		it = map.find(fVarList[i]);

		//cout<<" find "<<i<<" "<<fVarList[i]<<" "<<fVarList[i]->GetName()<<endl;

		if( it != map.end() ){

			//if(fDisable2D && !IsUnbinned() ) fVarHisto[i]->Fill( it->second, w );
			//else fVarCache[i] = it->second;

			fVarCache[i] = it->second;

			//cout<<" "<<fVarList[i]->GetName()<<" "<<it->second<<" w: "<<fEventWeight<<endl;
		}

	}

	fFillFromCache();
}


void TSSample::Fill(double w)
{

	/**
	 *
	 *
	 *  Write variables values in the cache
	 *  then fill histograms
	 *
	 *
	 */

	fEventWeight = w;


    for(int i=0; i<fNumOfVars;++i){

    	fVarCache[i] = fVarList[i]->GetValue();

    }

    fFillFromCache();

}


void TSSample::fFillFromCache()
{


	double WW = fEventWeight;

	for(int i=0; i<fNumOfVars;++i){

    	 fVarHisto[i]->Fill( fVarCache[i], WW );

	     //RootUtils::FillHistoFast(*fVarHisto[i],fVarCache[i],WW);

  	}


    int NH2= fVarHisto2D.size();


    for(int i=0; i< NH2; ++i){

    	int idx1= fHisto2DIdx[i].first;
    	int idx2= fHisto2DIdx[i].second;

    	fVarHisto2D[i]->Fill( fVarCache[idx1], fVarCache[idx2], WW );

	    //RootUtils::FillHistoFast(*fVarHisto2D[i],fVarCache[idx1],fVarCache[idx2],WW);
    }


	if( IsUnbinned() ){

		fDataTree.Fill();

		//cout<<"IsUn "<<GetName()<<" "<<fDataTree.GetEntries()<<" "<<fVarHisto[0]->GetEntries()<<endl;

		if(fDataTree.GetEntries() != fVarHisto[0]->GetEntries() ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," ",GetName()," Mismatch entry between histogram and tree");
			assert(0);
		}

	}


}


void TSSample::Fill(int ih, double x, double w){

	fVarHisto[ih]->Fill(x, w);
}

void TSSample::Fill(int ih, double x, double y, double w){

	fVarHisto2D[ih]->Fill(x, y, w);
}


void TSSample::Fill(TTree &t, TCut &selection, TString vars, TString weight)
{

	std::vector<TString> varList=StringUtils::Tokenize(vars,":");


	int nVar = varList.size();

	//Depends on GetVx, currently max is GetV4
	if( nVar>4 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Too many vars: ", vars," Max supported by this method is 4 ");
		return;
	}

	if( fVarList.size() == 0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No Variables set form the Sample");

		return;
	}

	if( nVar>fVarList.size() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Branch Variables list and Sample variables number do not match ");
		MSG::ERROR("Sample: ",GetName()," vars: ", fVarList.size());
		return;
	}




	for(int i=0; i<nVar;++i){

		if( !t.GetBranch(varList[i]) ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Branch ", varList[i]," does not exist");
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Sample: ",GetName());
			return;
		}
	}


	bool hasWeight = !weight.IsWhitespace();

	if( hasWeight && !t.GetBranch(weight) ){
		hasWeight=false;
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Branch ", weight," does not exist");
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Sample: ",GetName());
		assert(0);
	}



	int nEveTot =  t.GetEntries();
	t.SetEstimate(nEveTot);

	int nEve=0;

	t.Draw(vars,selection,"goff");

	nEve=t.GetSelectedRows();

	double *x1 = t.GetV1();
	double *x2 = t.GetV2();
	double *x3 = t.GetV3();
	double *x4 = t.GetV4();


	//cout<<__FUNCTION__<<" "<<GetName()<<" NSel: "<<nEve<<endl;
	//cout<<" vars: "<<vars<<" "<<selection<<endl;

	//save V1 first otherwise xw will "overwrite" it
	std::vector<double> xx1; //= new double[nEve];
	for(int i=0; i<nEve; ++i){
		xx1.push_back(x1[i]);
	}

	double *xw = 0;

	if( hasWeight ){

		t.Draw(weight,selection,"goff");

		nEve=t.GetSelectedRows();

		xw = t.GetV1();

	}


	//fill

	for(int iev=0; iev<nEve; ++iev ){

		fVarCache[0] = xx1[iev];
		if(nVar>=2) fVarCache[1] = x2[iev];
		if(nVar>=3) fVarCache[2] = x3[iev];
		if(nVar>=4) fVarCache[3] = x4[iev];

		fEventWeight = 1;

		if(hasWeight){
			fEventWeight = xw[iev];
		}


		fFillFromCache();

	}



}


TH1F* TSSample::GetHistogram(const TSVariable &var)
{

	int idx = GetVarIdx(var);

	if( idx==-1 ) return 0;

	return fVarHisto[idx];
}

TH1F* TSSample::GetToyHistogram(const TSVariable &var)
{

	int idx = GetVarIdx(var);

	if( idx==-1 ) return 0;

	return fVarHistoToy[idx];


}

TH2F* TSSample::GetHistogram2D(const TSVariable &varx, const TSVariable &vary)
{

	int idx1 = GetVarIdx(varx);
	int idx2 = GetVarIdx(vary);

	if( idx1==-1 || idx2==-1 ) return 0;


	int NH2= fVarHisto2D.size();


	for(int i=0; i< NH2; ++i){

		int h_idx1= fHisto2DIdx[i].first;
		int h_idx2= fHisto2DIdx[i].second;

		if(h_idx1==idx1 && h_idx2 == idx2 ){
			return fVarHisto2D[i];
		}

	}

	return 0;
}


TH2F* TSSample::GetToyHistogram2D(const TSVariable &varx, const TSVariable &vary)
{

	int idx1 = GetVarIdx(varx);
	int idx2 = GetVarIdx(vary);

	if( idx1==-1 || idx2==-1 ) return 0;


	int NH2= fVarHisto2DToy.size();


	for(int i=0; i< NH2; ++i){

		int h_idx1= fHisto2DIdx[i].first;
		int h_idx2= fHisto2DIdx[i].second;

		if(h_idx1==idx1 && h_idx2 == idx2 ){
			return fVarHisto2DToy[i];
		}

	}

	return 0;

}


int TSSample::GetVarIdx(const TSVariable &var) const
{


	for(int i=0; i<fVarList.size();++i){

		if( fVarList[i]==&var ) return i;
	}

	return -1;
}




void TSSample::Init()
{

	fNumOfVars=0;
	fNumOfAuxVars=0;

	fIsUnbinned = false;

	fInitDataTree();

	fHasCategory = false;

	//
	// POT = POT^Power
	//
	fPOTpower = 20;

	fPOTnative=1;
	fPOTnative_pow = fPOTpower;
	fPOT=1;
	fPOTscaling=1;


	fVarCache = new double[1];

	fEventWeight=1;

	fToyId = 0;

	fDisable1D = false;
	fDisable2D = false;


}


THStack* TSSample::RandomizedStack(const TSVariable &v, int ntoy){


	THStack *hs = new THStack();


	for(int ih=0; ih<fNumOfVars; ++ih){

		if( fVarList[ih] == &v){


			for(int itoy =0; itoy<ntoy; ++itoy){

				Randomize();

				TH1F *htoy = new TH1F( *fVarHistoToy[ih] );

				hs->Add(htoy);

			}//toys

		}


	}


	return hs;

}



void TSSample::Randomize(int opt)
{


	for(int ih=0; ih<fNumOfVars; ++ih){

		TH1F *h = fVarHisto[ih];

		int NX = h->GetNbinsX();



		for(int ib=1; ib<=NX; ++ib){

			float cont = h->GetBinContent(ib);

			if(cont == 0.) continue;

			cont = (float)fRand.PoissonD(cont);
			fVarHistoToy[ih]->SetBinContent(ib,cont);
		}

	}



	int NH2= fVarHisto2D.size();


	for(int ih=0; ih< NH2; ++ih){



		int NX = fVarHisto2D[ih]->GetNbinsX();
		int NY = fVarHisto2D[ih]->GetNbinsY();


		for(int ix=1; ix<=NX; ++ix){
			for(int iy=1; iy<=NY; ++iy){

				float cont = fVarHisto2D[ih]->GetBinContent(ix,iy);

				if(cont == 0.) continue;

				cont = (float)fRand.PoissonD(cont);
				fVarHisto2DToy[ih]->SetBinContent(ix,iy,cont);

			}
		}


	}//



}


void TSSample::Reset()
{

	for(int ih=0; ih<fNumOfVars; ++ih){

		fVarHisto[ih]->Reset();
	}



	int NH2= fVarHisto2D.size();


	for(int ih=0; ih< NH2; ++ih){
		fVarHisto2D[ih]->Reset();
	}


}


void TSSample::ResetToys()
{

	for(int ih=0; ih<fNumOfVars; ++ih){

		fVarHistoToy[ih]->Reset();
	}



	int NH2= fVarHisto2D.size();


	for(int ih=0; ih< NH2; ++ih){
		fVarHisto2DToy[ih]->Reset();
	}


}



void TSSample::Restore()
{

	/**
	 *
	 *  Restore all the toy to the corresponding histo
	 *
	 *
	 */


	for(int ih=0; ih<fNumOfVars; ++ih){

		TH1F *h = fVarHisto[ih];

		int NX = h->GetNbinsX();


		for(int ib=1; ib<=NX; ++ib){

			float cont = h->GetBinContent(ib);

			fVarHistoToy[ih]->SetBinContent(ib,cont);
		}

	}//ih


	int NH2= fVarHisto2D.size();


	for(int ih=0; ih< NH2; ++ih){


		int NX = fVarHisto2D[ih]->GetNbinsX();
		int NY = fVarHisto2D[ih]->GetNbinsY();


		for(int ix=1; ix<=NX; ++ix){
			for(int iy=1; iy<=NY; ++iy){

				float cont = fVarHisto2D[ih]->GetBinContent(ix,iy);

				fVarHisto2DToy[ih]->SetBinContent(ix,iy,cont);

			}
		}


	}//


}





int TSSample::SetWeightParams(const TSNuiParamSet &setOfNuiParams)
{


	bool DEBUG = VERBOSE;

	V(endl<<"==== "<<__FILE__<<"::"<<__FUNCTION__<<" Sample: "<<GetName()<<" ===== ");


	V(" Has Category Cards: "<<HasCategoryCard() );

	if(DEBUG){
		if( HasCategoryCard() ){
			fCard.Print();
		}
	}


	int NNuis = setOfNuiParams.GetSize();


	V(" Nuisance Parameter Set: "<<setOfNuiParams.GetName());

	V(" Number of Nuisance Parameters: "<<NNuis);


	if( NNuis == 0 ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," Sample: ",GetName(), " Param List is empty");
		return 0;

	}



    double finalWeight = 1;


    for(int ipar=0; ipar<NNuis; ++ipar){


    	TSNuiParam *nuiPar = (TSNuiParam*)setOfNuiParams.At(ipar);

    	V(" ----Param--- "<<ipar<<" "<<nuiPar->GetName());

    	nuiPar->Print();

    	bool hasCategoryCard = nuiPar->HasCategoryCard();

    	bool hasControlVars = nuiPar->HasControlVars();

    	V(endl<<" -Cards Check- "<<" Has Card "<<hasCategoryCard);

    	int cards_ok = false;

    	if( !hasCategoryCard && !this->HasCategoryCard()  ){

    		V(" None has cards: ok  ");

    		cards_ok = true;

    	}else if( hasCategoryCard &&  !this->HasCategoryCard() ){


    		V(" Only the param has card: skipped ");

    		 cards_ok = false;

    	}else if (!hasCategoryCard && this->HasCategoryCard() ){

    		V(" Only the sample has card: ok ");

    		cards_ok = true;


    	}else{

    		V(" Both have cards: Compare Cards ");

    		cards_ok = CompareCategoryCard( nuiPar->GetCategoryCard() );

    		V(" Compare Cards: flag "<<cards_ok);

    		if( !cards_ok ){
    			V(" Cards not compatible ");
    		}

    	}//cards check


    	if( !cards_ok ) continue;


        V(" ---- Cards Check OK --- ");
        V(" Add Param to the Histogram for Control Vars Check ");

        V(endl<<"  Loop over all the histograms 1D, Tot: "<<fVarHisto.size());

        for(int ih=0; ih<fVarHisto.size(); ++ih){

        	V(ih<<"- histogram: "<<fVarHisto[ih]->GetName()<<" Variable: "<<fVarList[ih]->GetName());

        	// create a TSHistoNuiParamMgr if not present
        	if( !fHistoNuiParMgr[ih] ){
        		fHistoNuiParMgr[ih] = new TSHistoNuiParamMgr();
        	}
        	TSHistoNuiParamMgr *mgr = fHistoNuiParMgr[ih];

        	//link the histogram if not done yet
        	if(!mgr->HasHistogram())
        		mgr->SetHistogram(fVarHisto[ih],fVarList[ih]);
        	//ad this NuiParam
        	mgr->AddNuiParam(*nuiPar);
        	//

        }

        V(endl<<"  Loop over all the Toy histograms 1D, Tot "<<fVarHistoToy.size());

        for(int ih=0; ih<fVarHistoToy.size(); ++ih){
        	V(ih<<"- histogram: "<<fVarHistoToy[ih]->GetName()<<" Variable: "<<fVarList[ih]->GetName());

        	if( !fHistoToyNuiParMgr[ih] ){
        		fHistoToyNuiParMgr[ih] = new TSHistoNuiParamMgr();
        	}
        	TSHistoNuiParamMgr *mgr = fHistoToyNuiParMgr[ih];

        	if(!mgr->HasHistogram())
        		mgr->SetHistogram(fVarHistoToy[ih],fVarList[ih]);
        	//ad this NuiParam
        	mgr->AddNuiParam(*nuiPar);

        	mgr->Print();
        }

        V(endl<<"  Loop over all the histograms 2D, Tot: "<<fVarHisto2D.size());

        for(int ih=0; ih<fVarHisto2D.size(); ++ih){
        	int idx = fHisto2DIdx[ih].first;
        	int idy = fHisto2DIdx[ih].second;
        	V(ih<<"- histogram: "<<fVarHisto2D[ih]->GetName()<<" Variable x: "<<fVarList[idx]->GetName()<<" y: "<<fVarList[idy]->GetName());

        	if( !fHisto2DNuiParMgr[ih] ){
        		fHisto2DNuiParMgr[ih] = new TSHistoNuiParamMgr();
        	}
        	TSHistoNuiParamMgr *mgr = fHisto2DNuiParMgr[ih];


        	if(!mgr->HasHistogram())
        		mgr->SetHistogram(fVarHisto2D[ih],fVarList[idx],fVarList[idy]);
        	//ad this NuiParam
        	mgr->AddNuiParam(*nuiPar);
        }

        V(endl<<"  Loop over all the Toy histograms 2D, Tot: "<<fVarHisto2DToy.size());

        for(int ih=0; ih<fVarHisto2DToy.size(); ++ih){
        	int idx = fHisto2DIdx[ih].first;
        	int idy = fHisto2DIdx[ih].second;
        	V(ih<<"- histogram: "<<fVarHisto2DToy[ih]->GetName()<<" Variable x: "<<fVarList[idx]->GetName()<<" y: "<<fVarList[idy]->GetName());

        	if( !fHisto2DToyNuiParMgr[ih] ){
        		fHisto2DToyNuiParMgr[ih] = new TSHistoNuiParamMgr();
        	}
        	TSHistoNuiParamMgr *mgr = fHisto2DToyNuiParMgr[ih];

        	if(!mgr->HasHistogram())
        		mgr->SetHistogram(fVarHisto2DToy[ih],fVarList[idx],fVarList[idy]);
        	//ad this NuiParam
        	mgr->AddNuiParam(*nuiPar);

        }




        //V(" Add Histogram "<<fVarHisto2D[0]);

        /*
        cout<<fVarHisto2D[0]->GetName()<<endl;

        TSRespFunc RF;

        int iv1 = fHisto2DIdx[0].first;
        int iv2 = fHisto2DIdx[0].second;

        cout<<iv1<<" "<<iv2<<endl;
        cout<<fVarList[iv1]->GetName()<<" "<<fVarList[iv2]->GetName()<<endl;

        if(!fNuiParamMgr.HasHistogram()) fNuiParamMgr.SetHistogram(fVarHisto2D[0],fVarList[iv1],fVarList[iv2]);
        //if(!fNuiParamMgr.HasHistogram()) fNuiParamMgr.SetHistogram(fVarHisto[0],fVarList[0]);


        fNuiParamMgr.AddNuiParamWithRespFunc(*nuiPar,RF);
         */

    }//nui par loop



    cout<<" === "<<__FUNCTION__<<" ==== Final Weight: "<<finalWeight<<endl;

	return 1;

}



void TSSample::Print()
{

	cout<<" "<<endl;
	cout<<"--- Sample: "<<GetName()<<" "<<GetLabel()<<" "<<GetTitle()<<" --- "<<endl;

	cout<<"Native Normalization POT: "<<GetNativePOT()<<"x10**"<<GetNativePOTPower()<<endl;
	cout<<"Normalization POT: "<<GetPOT()<<"x10**"<<GetPOTPower()<<" ("<<GetPOTLabel()<<")"<<endl;
	cout<<"Last POT scaling: "<<GetPOTScaling()<<endl;

	cout<<"-----"<<endl;

}



void TSSample::SetBidimensional(const TSVariable &var_x, const TSVariable &var_y)
{


	TSVariable *vx=0;
	TSVariable *vy=0;
	TAxis *ax=0;
	TAxis *ay=0;

	int idx[2]={-1,-1};
	int itype[2]={0,0};

	for(int i=0;i<fVarListAll.size();++i){

		if( fVarListAll[i] == &var_x){
			vx = (TSVariable*)fVarListAll[i];
			idx[0]=i;
			itype[0]=fVarType[i];
		}

		if( fVarListAll[i] == &var_y){
			vy = (TSVariable*)fVarListAll[i];
			//ay = fVarAxis[i];
			idx[1]=i;
			itype[1]=fVarType[i];
		}

	}


	if( !vx || !vy ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," One Variable not found");
		return;
	}

	if( vx == vy ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Same variable on both axis");
		return;
	}


	if( fVarType[idx[0]] != fVarType[idx[1]] ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," 2D Histo with variables of different type, e.g.: one is an Aux variable. Histo not built.");
		return;
	}


	ax = fVarAxis[idx[0]];
	ay = fVarAxis[idx[1]];


	TString hname = StringUtils::Name("h","_",GetName(),"_",vx->GetName(),"_",vy->GetName());
	TString htit = GetTitle();


    TH2F *h = new TH2F(hname,htit,ax->GetNbins(),ax->GetXbins()->GetArray(),ay->GetNbins(),ay->GetXbins()->GetArray());

    h->GetXaxis()->SetTitle(vx->GetAxisTitle());
    h->GetYaxis()->SetTitle(vy->GetAxisTitle());



	if( itype[0]==1 && itype[1]==1){
		fHisto2DIdx.push_back(  std::make_pair(idx[0],idx[1]) );

		TH2F *htoy = new TH2F(*h);
		htoy->SetName(hname+"_toy");

		fVarHisto2D.push_back(h);
		fVarHisto2DToy.push_back(htoy);

	    hList.Add(h);
	    hListToy.Add(htoy);

	    hListAll.Add(h);
	    hListAll.Add(htoy);


	    fHisto2DNuiParMgr.push_back(0);
	    fHisto2DOscillProbMgr.push_back(0);
	    fHisto2DToyNuiParMgr.push_back(0);
	    fHisto2DToyOscillProbMgr.push_back(0);

	}else
	if( itype[0]==2 && itype[1]==2){
		fHisto2DAuxIdx.push_back(  std::make_pair(idx[0],idx[1]) );

		fVarHisto2DAux.push_back(h);

		hListAux.Add(h);
		hListAll.Add(h);
	}




}



int TSSample::SetOscillProbFunction(TSOscProb *p)
{


	TSVariable *pEne = p->GetEnergyVariableBound();

	if( !pEne ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Energy Variable not Set");
		return 0;
	}


	for(int i=0; i<fVarList.size(); ++i){

		TSHistoOscillMgr* oMgr = new TSHistoOscillMgr();

		oMgr->SetHistogram( fVarHisto[i], fVarList[i]);
		oMgr->SetOscillProb(p);

	}


	for(int i=0; i<fVarHisto2D.size();++i){

		int idx = fHisto2DIdx[i].first;
		int idy = fHisto2DIdx[i].second;

		TSHistoOscillMgr* oMgr = new TSHistoOscillMgr();

		oMgr->SetHistogram( fVarHisto[i], fVarList[idx], fVarList[idy] );
		oMgr->SetOscillProb(p);

	}



	return 1;
}


void TSSample::SetCategoryCard(const TSCategoryCard &card)
{

     fCard.Copy(card);

     fCard.SetName(card.GetName() );

     fHasCategory = true;

}


void TSSample::SetNativePOT(float POT, float Power)
{
	/**
	 *
	 *
	 */


	if(POT <= 0 || Power<=0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
		return;
	}

	fPOTscaling = 1;

    fPOTnative = POT;
	fPOT = POT;

	fPOTpower = Power;
	fPOTnative_pow = fPOTpower;

	fBuildPOTNames();

}



void TSSample::ScaleToNativePOT(){

	ScaleToPOT( GetNativePOT(), GetNativePOTPower() );

}



void TSSample::ScaleToPOT(float POT, float Power)
{


	if(POT <= 0 || Power <= 0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
		return;
	}

	//scaling factor w.r.t. current normalization

	fPOTscaling = (POT/fPOT)*TMath::Power(10,Power-fPOTpower);

	fPOT = POT;
	fPOTpower = Power;

	fBuildPOTNames();

	fNormalizeAll();

}




//
//
//
//=======================================
//
//
//
//



void TSSample::fBuildPOTNames()
{


	fPOTNativeLabel = "Native POT: ";
	fPOTNativeLabel += StringUtils::ToString(fPOTnative);
	fPOTNativeLabel +="x10^{"; fPOTNativeLabel+=fPOTpower; fPOTNativeLabel+="}";


	fPOTlabel = "POT: ";
	fPOTlabel += StringUtils::ToString(fPOT);
	fPOTlabel +="x10^{"; fPOTlabel+=fPOTpower; fPOTlabel+="}";


}


void TSSample::fInitDataTree()
{


	fDataTree.SetName( StringUtils::Name("tree","_",GetName()) );
	fDataTree.SetTitle( GetTitle() );

}


void TSSample::fUpdateDataTreeBranches()
{

	TString br_name;

	for(int iv=0; iv<fVarList.size(); ++iv){

		br_name = fVarList[iv]->GetName();

		if( !fDataTree.GetBranch(br_name) ){
			fDataTree.Branch(br_name, (fVarCache+iv), StringUtils::Name(br_name,"/D"));
		}

	}


	br_name="EventWeight";

	if( !fDataTree.GetBranch(br_name))
		fDataTree.Branch(br_name, &fEventWeight, StringUtils::Name(br_name,"/D"));


	br_name="ToyId";

	if( !fDataTree.GetBranch(br_name))
		fDataTree.Branch(br_name, &fToyId, StringUtils::Name(br_name,"/I"));


	//Add Nuisance....



}




void TSSample::fNormalizeAll(int opt)
{


	for(int i=0; i<fVarList.size(); ++i){

		fVarHisto[i]->Scale(fPOTscaling);
		fVarHistoToy[i]->Scale(fPOTscaling);

		cout<<"ScaleToPOT: "<<fVarHisto[i]->GetName()<<"; "<<fPOTlabel<<"; "<<fPOTNativeLabel
					<<" scale factor: "<<fPOTscaling<<endl;

	}



	int NH2= fVarHisto2D.size();


	for(int i=0; i< NH2; ++i){

		int idx1= fHisto2DIdx[i].first;
		int idx2= fHisto2DIdx[i].second;

		fVarHisto2D[i]->Scale( fPOTscaling );
		fVarHisto2DToy[i]->Scale( fPOTscaling );

	}


}






