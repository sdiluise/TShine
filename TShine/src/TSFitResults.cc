/*
 * TSFitResults.cc
 *
 *  Created on: Aug 22, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "RootUtils.h"
#include <TMath.h>

#include <TList.h>

#include "TSFitResults.h"

TSFitResults::TSFitResults()
{

	Init();
}

TSFitResults::TSFitResults(TString name, TString title,TString tag)
:TSNamed(name,name,title)
{

	Init();

	SetTag(tag);
}


TSFitResults::~TSFitResults(){

	ClearContourGraphs();

}

void TSFitResults::ClearContourGraphs() {


	for(std::map< std::pair<TString,TString>,TGraph* >::iterator it=fContourGraphList.begin(), end=fContourGraphList.end();
				it!=end;
				++it){

			cout<<"contour: "<<(it->first).first<<" - "<<(it->first).second<<" graph: "<<(it->second)->GetName()<<endl;

			delete it->second;

	}

	fContourGraphList.clear();


	for(int i=0; fScanGraphList.size();++i){
		delete fScanGraphList.at(i);
	}

	fScanGraphList.clear();
}




TObject* TSFitResults::GetHisto(TString tag) const{

	TString name=hBaseName; name+=tag;

	TObject *h= hList.FindObject(name);

	if(!h){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," histog tagged ",tag, " not found");
		return 0;
	}

	return h;

}


void TSFitResults::ImportMinimizationResults(const TSMinimizer &Minimizer) {


	MSG::LOG(__FILE__,"::",__FUNCTION__," ",GetName());


	if( !Minimizer.GetMinimizer() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Minimizer Not Found ");
		return;
	}

	if( !Minimizer.GetMinimizationFunction() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Minimization Function Not Found ");
		return;
	}

	fNumOfCalls = Minimizer.GetMinimizer()->NCalls();
	fNumOfPars = Minimizer.GetMinimizer()->NDim();
	fNumOfFreePars = Minimizer.GetMinimizer()->NFree();

	fMinValue = Minimizer.GetMinimizer()->MinValue();
	fStatusCode = Minimizer.GetMinimizer()->Status();


	if( fNumOfPars ==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Minimizer has Zero Params ");
		return;
	}

	//
	//Write Best Fit Param List
	//
	TSParamList *list=Minimizer.GetBestFitParamList();

	SetListOfFitParams(*list);

	int Npars=list->GetSize();

	//
	cout<<" Get Errors "<<endl;
	//

	Minimizer.GetCovMatrix(fCovMatrix);
	Minimizer.GetCorrMatrix(fCorrMatrix);


	//fCovMatrix.Print();
	//fCorrMatrix.Print();

	bool fill_cov=false;
	bool fill_corr=false;

	if(fCovMatrix.GetNrows()==fNumOfPars){
		fill_cov=true;
	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Cov Matrix Dimension not matching no. pars ",fCovMatrix.GetNrows()," ",fNumOfPars);
	}
	if(fCovMatrix.GetNrows()==fNumOfPars){
		fill_corr=true;
	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Corr Matrix Dimension not matching no. pars ",fCorrMatrix.GetNrows()," ",fNumOfPars);
	}


	TH2F* hCV2=list->GetHisto2D();
	TH2F* hCR2=list->GetHisto2D();

	for(int ix=0;ix<hCV2->GetNbinsY();++ix){
		for(int iy=0;iy<hCV2->GetNbinsY();++iy){

			if(fill_cov) hCV2->SetBinContent(ix+1,iy+1,fCovMatrix[ix][iy]);
			if(fill_corr) hCR2->SetBinContent(ix+1,iy+1,fCorrMatrix[ix][iy]);
		}
	}

	fImportHisto2D(&hCovMatrix,hCV2);
	fImportHisto2D(&hCorrMatrix,hCR2);

	hCovMatrix.SetTitle("Covariance Matrix");
	hCorrMatrix.SetTitle("Correlation Matrix");

	delete hCV2;
	delete hCR2;


	//
	fGCC.Clear();
	fGCC.ResizeTo(fNumOfPars);

	TH1F *hgcc=list->GetHisto();
	hgcc->Reset();
	hgcc->SetTitle("global correlation");
	hgcc->GetYaxis()->SetTitle("global correlation coeff");
	for(int i=0;i<hgcc->GetNbinsX();++i){
		hgcc->SetBinContent(i+1,Minimizer.GetMinimizer()->GlobalCC(i));
		hgcc->SetBinError(i+1,0);

		fGCC[i]=Minimizer.GetMinimizer()->GlobalCC(i);
	}

	fImportHisto1D(&hGCC,hgcc);
	hGCC.SetMinimum(0.);



	delete hgcc;


	//
	cout<<" Get Contours and Scan Graphs "<<endl;
	//

   TList gCont;
   TList gScan;

   Minimizer.GetContourGraphs(gCont);
   Minimizer.GetScanGraphs(gScan);



   //
   cout<<" History Tree "<<endl;
   //

   //TTree *T=
   //Minimizer.GetMinimizationFunction()->GetHistoryTree()->Print();//Minimizer.GetHistoryTree(fHistoryTree);
   //Minimizer.GetMinimizationFunction()->GetHistoryTree()->Write();
   //Minimizer.GetMinimizationFunction()->GetHistoryTree()->CloneTree();

   //cout<<"out "<<T<<endl;
   //T->Print();
   //fHistoryTree->Print();

	if(Minimizer.GetMinimizationFunction()->GetHistoryTree()){
		fHistoryTree = Minimizer.GetMinimizationFunction()->GetHistoryTree()->CloneTree();
		//fHistoryTree->CopyEntries(Minimizer.GetMinimizationFunction()->GetHistoryTree());

		fHistoryTree->SetName("FitResults_HistoryTree");
		fHistoryTree->Write();
	}



   MSG::LOG(__FILE__,"::",__FUNCTION__," ",GetName(), " --end ");

}




void TSFitResults::Init(){

	fNumOfCalls = 0;
	fNumOfPars = 0;
	fNumOfFreePars = 0;
	fMinValue = 0;
	fStatusCode=0;

	SetCPUTime(0);
	SetRealTime(0);
	SetIteration(1);
	SetTag("");

	fBuildHistos();

}



void TSFitResults::SetListOfTrueParams(const TSParamList &list){

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	list.PrintParams();

	fTrueParamList.Import(list);

	fTrueValues.Clear();
	fTrueValues.ResizeTo(fTrueParamList.GetSize());
	for(int i=0;i<fTrueParamList.GetSize();++i){
		fTrueValues[i]=fTrueParamList.At(i)->GetValue();
	}

	TH1F *h=list.GetHistoOfValues();

	fImportHisto1D(&hTrueVal,h);


	hTrueVal.SetTitle("true param values");

	fFillPulls();

	MSG::LOG(__FILE__,"::",__FUNCTION__," -- end --");
}



void TSFitResults::SetListOfInputParams(const TSParamList &list){

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	list.PrintParams();

	fInputParamList.Import(list);



	TH1F *h=list.GetHistoOfValues();

	fImportHisto1D(&hInputVal,h);

	delete h;

	hInputVal.SetTitle("input param values");


}

void TSFitResults::SetListOfFitParams(const TSParamList &list){

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	list.PrintParams();

	fFitParamList.Clear();
	fFitParamList.Import(list);

	fFitValues.Clear();
	fFitValues.ResizeTo(fFitParamList.GetSize());

	fErrors.Clear();
	fErrors.ResizeTo(fFitParamList.GetSize());

	fErrLow.Clear();
	fErrLow.ResizeTo(fFitParamList.GetSize());

	fErrUp.Clear();
	fErrUp.ResizeTo(fFitParamList.GetSize());

	fErrorFrac.Clear();
	fErrorFrac.ResizeTo(fFitParamList.GetSize());


	for(int i=0;i<fFitParamList.GetSize();++i){

		fFitValues[i]=fFitParamList.At(i)->GetValue();
		fErrors[i]=fFitParamList.At(i)->GetError();
		fErrLow[i]=fFitParamList.At(i)->GetErrorLow();
		fErrUp[i]=fFitParamList.At(i)->GetErrorUp();
		fErrorFrac[i]=fFitParamList.At(i)->GetErrorFrac();

	}

	//
	TH1F *hv=list.GetHistoOfValues();

	fImportHisto1D(&hFitVal,hv);

	hFitVal.SetTitle("fit param values");

	delete hv;

	//
	TH1F* he=list.GetHistoOfErrors();

	fImportHisto1D(&hError,he);

	hError.SetTitle("fit param error ");

	delete he;

	//
	TH1F *hl= list.GetHistoOfLowErrors();

	fImportHisto1D(&hErrLow,hl);

	hErrLow.SetTitle("fit param lower error");

	delete hl;

	//
	TH1F *hu= list.GetHistoOfUpErrors();

	fImportHisto1D(&hErrUp,hu);

	hErrUp.SetTitle("fit param upper error");

	delete hu;

	//
	TH1F *hf= list.GetHistoOfFracErrors();

	fImportHisto1D(&hErrorFrac,hf);

	hErrorFrac.SetTitle("fit param fractional error");

	delete hf;

	//
	TH1F *hll= list.GetHistoOfLowLimits();

	fImportHisto1D(&hLimitLow,hll);

	hLimitLow.SetTitle("fit param lower limit");

	delete hll;

	//
	TH1F *hlu= list.GetHistoOfUpLimits();

	fImportHisto1D(&hLimitUp,hlu);

	hLimitUp.SetTitle("fit param upper limit");

	delete hlu;


	//


	fFillPulls();




}


void TSFitResults::SetTag(TString tag){

	fTag=tag;

	if(!tag.IsWhitespace()) fHasTag=true;
	else fHasTag=false;

}

void TSFitResults::WriteHistos() {

	/*
	hGCC.Write();
	hErrors.Write();
	hErrLow.Write();
	hErrUp.Write();
	hErrorFrac.Write();

	hTrueVal.Write();
	hInputVal.Write();
	hFitVal.Write();
	hResid.Write();
	hPull.Write();

	hCovMatrix.Write();
	hCorrMatrix.Write();
	//hHesseMatrix.Write();
*/
	hList.Write();

}


void TSFitResults::fFillPulls() {

	MSG::LOG(__FILE__,"::",__FUNCTION__);


	int NP=fFitParamList.GetSize();

	int NT= fTrueParamList.GetSize();


	TH1F *hp=0;

	if(NT>0) hp=fTrueParamList.GetHistoOfValues();
	else if(NP>0) hp=fFitParamList.GetHistoOfValues();

	if(hp==0) return;


	fImportHisto1D(&hPull,hp);

	hPull.SetTitle("fit pulls");
	hPull.GetYaxis()->SetTitle("pull");

	hPull.Reset();


	fImportHisto1D(&hResid,hp);

	hResid.SetTitle("fit residual");
	hResid.GetYaxis()->SetTitle("fit residual");

	hResid.Reset();


	if( !(NT>0 && NT==NP) ){
		return;
	}

	fPull.Clear();
	fPull.ResizeTo(NP);
	fResid.Clear();
	fResid.ResizeTo(NP);


	for(int i=0;i< NP ;++i){

		TSParameter *ptrue= (TSParameter*)fTrueParamList.At(i);
		TSParameter *pfit= (TSParameter*)fFitParamList.At(i);

		double val=ptrue->GetValue();
		double fit=pfit->GetValue();
		double err=pfit->GetError();

		double resid=fit-val;

		double pull=0;

		if(!RootUtils::IsZero(err)) {
			pull=resid/err;
		}

		hPull.SetBinContent(i+1,pull);

		hResid.SetBinContent(i+1,resid);
		hResid.SetBinError(i+1,err);

		fPull[i]=pull;
		fResid[i]=resid;
	}


}


void TSFitResults::fImportHisto1D(TH1F *h, TH1F *h2){

	if( !h || !h2 ){
		return;
	}

	int N=h2->GetNbinsX();

	h->SetStats(0);
	h->SetBins(N,0,N);
	h->Reset();

	h->SetTitle( h2->GetTitle() );
	h->GetXaxis()->SetTitle( h2->GetXaxis()->GetTitle() );
	h->GetYaxis()->SetTitle( h2->GetYaxis()->GetTitle() );

	for(int i=1;i<=N;++i){
		h->SetBinContent(i,h2->GetBinContent(i));
		h->SetBinError(i,h2->GetBinError(i));
		h->GetXaxis()->SetBinLabel(i, h2->GetXaxis()->GetBinLabel(i) );
	}


}

void TSFitResults::fBuildHistos() {


	// to be fixed;

	kCovMatrix="CovMatrix";
	kCorrMatrix="CorrMatrix";
	kHesseMatrix="HesseMatrix";

	kGCC="GCC";
	kError="Error";
	kErrLow="ErrorLow";
	kErrUp="ErrUp";
	kErrorFrac="ErrorFrac";
	kLimitLow="LimitLow";
	kLimitUp="LimitUp";

	kTrueValue="TrueVal";
	kInputValue="InputVal";
	kFitValue="FitVal";
	kResid="Resid";
	kPull="Pull";



	hGCC.SetName(kGCC);
	hError.SetName(kError);
	hErrLow.SetName(kErrLow);
	hErrUp.SetName(kErrUp);
	hErrorFrac.SetName(kErrorFrac);

	hTrueVal.SetName(kTrueValue);
	hInputVal.SetName(kInputValue);
	hFitVal.SetName(kFitValue);
	hResid.SetName(kResid);
	hPull.SetName(kPull);

	hCovMatrix.SetName(kCovMatrix);
	hCorrMatrix.SetName(kCorrMatrix);
	hHesseMatrix.SetName(kHesseMatrix);


	hList.Add(&hGCC);
	hList.Add(&hError);
	hList.Add(&hErrLow);
	hList.Add(&hErrUp);
	hList.Add(&hErrorFrac);

	hList.Add(&hTrueVal);
	hList.Add(&hInputVal);
	hList.Add(&hFitVal);
	hList.Add(&hResid);
	hList.Add(&hPull);
	hList.Add(&hCovMatrix);
	hList.Add(&hCorrMatrix);
	hList.Add(&hHesseMatrix);


	hBaseName="h"; hBaseName+=GetName(); hBaseName+="_";

	for(int i=0;i<hList.GetSize();++i){

		TString name=hBaseName; name+=hList.At(i)->GetName();
		((TNamed*)hList.At(i))->SetName(name);

		((TH2F*)hList.At(i))->SetStats(0);
	}


}

void TSFitResults::fImportHisto2D(TH2F *h, TH2F *h2){

	if( !h || !h2 ){
		return;
	}

	int NX=h2->GetNbinsX();
	int NY=h2->GetNbinsY();

	h->SetStats(0);
	h->SetBins(NX,0,NX,NY,0,NY);
	h->Reset();

	h->SetTitle( h2->GetTitle() );
	h->GetXaxis()->SetTitle( h2->GetXaxis()->GetTitle() );
	h->GetYaxis()->SetTitle( h2->GetYaxis()->GetTitle() );

	for(int ix=1;ix<=NX;++ix){
	   for(int iy=1;iy<=NY;++iy){
		h->SetBinContent(ix,iy,h2->GetBinContent(ix,iy));
		h->SetBinError(ix,iy,h2->GetBinError(ix,iy));
	  }
	}

	for(int ix=1;ix<=NX;++ix){
		h->GetXaxis()->SetBinLabel(ix, h2->GetXaxis()->GetBinLabel(ix) );
	}

	for(int iy=1;iy<=NY;++iy){
		h->GetYaxis()->SetBinLabel(iy, h2->GetYaxis()->GetBinLabel(iy) );
	}

}
