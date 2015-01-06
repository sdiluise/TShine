/*
 * TSMinimizer.cc
 *
 *  Created on: Aug 12, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <TObject.h>
#include <TObjArray.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>
#include <TMultiGraph.h>


#include "MessageMgr.h"

#include "TSParameter.h"
#include "TSMinimizer.h"

TSMinimizer::TSMinimizer(TString minimizer_type, TString algo_type):
fMinFunction(0)
{

	Init();

	fMinimizer = ROOT::Math::Factory::CreateMinimizer(minimizer_type.Data(),algo_type.Data());

}



TSMinimizer::~TSMinimizer() {

	ClearContourGraphs();

}


TGraph* TSMinimizer::BuildContourGraph(TString name_x, TString name_y, unsigned int npts) {


	if( !GetMinimizer() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No Minimizer Set" );
		return new TGraph();
	}

	if( !GetMinimizationFunction() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No Minimization Function Set" );
		return new TGraph();
	}

	TSArgList *plist = GetMinimizationFunction()->GetModel()->GetPrimaryParamList();

	if( !plist ) {
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Model Primary Param List Not Found" );
		return new TGraph();
	}

	TSParameter *parx= (TSParameter*)plist->FindObject(name_x);
	TSParameter *pary= (TSParameter*)plist->FindObject(name_y);

	if( !parx || !pary ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," not a valid parameter ", (!parx ? name_x:name_y) );
		return new TGraph();
	}

	int i= GetMinimizer()->VariableIndex( string(name_x.Data()) ); //parx->GetName()
	int j= GetMinimizer()->VariableIndex( string(name_y.Data()) ); //pary->GetName()

	if(i==-1 || j==-1){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," not a valid parameter ", (i==-1? name_x:name_y) );
		return new TGraph();
	}



	TString gname="gContour_";
	gname+=name_x; gname+="_"; gname+=name_y;

	TString gtit="contour "; gtit+=parx->GetLabel(); gtit+=" vs "; gtit+=pary->GetLabel();

	TString titx=parx->GetAxisTitle();
	TString tity=pary->GetAxisTitle();

	gtit+=";";	gtit += titx; gtit+=";"; gtit+=tity;

	if(GetMinimizer()->IsFixedVariable(i) || GetMinimizer()->IsFixedVariable(j) ) {

		TGraph *g=new TGraph();
		g->SetName(gname);
		g->SetTitle(gtit);

		// Set Point to the best fit values or the input values?
		g->SetPoint(0,1,1);
		return g;
	}


	double *xco=new double[npts];
	double *yco=new double[npts];

	GetMinimizer()->Contour(i,j,npts,xco,yco);

	TGraph* g=new TGraph(npts,xco,yco);

	g->SetName(gname);
	g->SetTitle(gtit);


	delete [] xco;
	delete [] yco;

	return g;
}



void TSMinimizer::ClearContourGraphs(){

	for(std::map< std::pair<TString,TString>,TGraph* >::iterator it=fContourGraphList.begin(), end=fContourGraphList.end();
			it!=end;
			++it){

		cout<<"contour: "<<(it->first).first<<" - "<<(it->first).second<<" graph: "<<(it->second)->GetName()<<endl;

		delete it->second;

	}

	fContourGraphList.clear();

}


void TSMinimizer::ClearContourParamList(){

	fContourParamPairs.clear();
}

void TSMinimizer::ComputeContours() {


	if( fContourParamPairs.size() == 0 ){
		return;
	}


	MSG::LOG("--- ",__FUNCTION__," --- ");

	ClearContourGraphs();

	int NCS = fContourParamPairs.size();

	unsigned int npts = fNptsContour;

	for(int ip=0; ip<NCS; ++ip){

		TString namex=fContourParamPairs.at(ip).first;
		TString namey=fContourParamPairs.at(ip).second;

		TGraph *g = BuildContourGraph(namex,namey,npts);

		fContourGraphList[std::make_pair(namex,namey)]=g;

		g->Write(g->GetName(),TObject::kOverwrite);

	}


	for(std::map< std::pair<TString,TString>,TGraph* >::iterator it=fContourGraphList.begin(), end=fContourGraphList.end();
			it!=end;
			++it){

		cout<<"created contour: "<<(it->first).first<<" - "<<(it->first).second<<" graph: "<<(it->second)->GetName()<<endl;

	}

	cout<<" ----- "<<endl;

}


void TSMinimizer::Configure() {

	/**
	 *
	 *  Updates parameters values and constraints
	 *
	 *
	 */


	if( !GetMinimizationFunction() ){

        MSG::ERROR(__FILE__,"::",__FUNCTION__," Minimization Function Not Set - ");
        assert(0);
	}


	TSArgList *par_list =  GetMinimizationFunction()->GetModel()->GetPrimaryParamList();

	int NP = par_list->GetSize();


	cout<<" --- Parameters: ---- "<<endl;
	for(int ip=0; ip<NP; ++ip){

		TSParameter *par= dynamic_cast<TSParameter*>(par_list->At(ip));

		if( par->HasRange() ){
			GetMinimizer()->SetLimitedVariable(ip,par->GetName(),par->GetValue(),par->GetStep(),par->GetMin(),par->GetMax());
			//GetMinimizer()->SetVariable(ip,par->GetName(),par->GetValue(),par->GetStep());
			GetMinimizer()->SetVariableValue(ip,par->GetValue());
		}else{
			GetMinimizer()->SetVariable(ip,par->GetName(),par->GetValue(),par->GetStep());
		}
		//GetMinimizer()->SetFixedVariable(ip,par->GetName(),par->GetValue());

		if( par->IsFixed() ){
			GetMinimizer()->FixVariable(ip);
		}

		//Add Parameter Ranges
		//if(par->HasRange() ){
		//	GetMinimizer()->SetVariableLimits(ip,par->GetMin(),par->GetMax());
		//}


		cout<<" "<<ip<<" "<<par->GetName()<<"  --> initial value: "<<par->GetValue()<<" step: "<<par->GetStep()<<" fix: "
				<<GetMinimizer()->IsFixedVariable(ip)
				<<" idx: "<<GetMinimizer()->VariableIndex(par->GetName())
				<<" range: "<<par->GetMin()<<" "<<par->GetMax()
				<<endl;


	}

	cout<<"NDim() "<<GetMinimizer()->NDim()<<" NFree() "<<GetMinimizer()->NFree()<<endl;


	cout<<" ------ "<<endl;


}



void TSMinimizer::FillCovarianceMatrix() {


	if( !GetMinimizer() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Minimizer Not Set");
		return;
	}


	int NDIM=GetMinimizer()->NDim();

	double *cov=new double[NDIM*NDIM];
	double *covh=new double[NDIM*NDIM];

	fCovMatrix.Clear();
	fCovMatrix.ResizeTo(NDIM,NDIM);

	fCorrMatrix.Clear();
	fCorrMatrix.ResizeTo(NDIM,NDIM);

	fHesseMatrix.Clear();
	fHesseMatrix.ResizeTo(NDIM,NDIM);

	TH2F *hCovMatrix=new TH2F("hCovMatrix","Covariance Matrix",NDIM,0.,NDIM+0.,NDIM,0.,NDIM+0.);
	TH2F *hCorrMatrix=new TH2F("hCorrMatrix","Correlation Matrix",NDIM,0.,NDIM+0.,NDIM,0.,NDIM+0.);

	TH1F *hGCC = new TH1F("hGCC","Global Correlation;;correlation ",NDIM,0.,NDIM+0.);

	TSArgList *plist = GetMinimizationFunction()->GetModel()->GetPrimaryParamList();

	if(plist){

		for(int i=0;i<plist->GetSize();++i){
			TSParameter* par= (TSParameter*)plist->At(i);

			hCovMatrix->GetXaxis()->SetBinLabel(i+1,par->GetLabel());
			hCovMatrix->GetYaxis()->SetBinLabel(i+1,par->GetLabel());

			hCorrMatrix->GetXaxis()->SetBinLabel(i+1,par->GetLabel());
			hCorrMatrix->GetYaxis()->SetBinLabel(i+1,par->GetLabel());

			hGCC->GetXaxis()->SetBinLabel(i+1,par->GetLabel());

		}

	}


	GetMinimizer()->GetCovMatrix(cov);
	GetMinimizer()->GetHessianMatrix(covh);

	for(int ix=0;ix<NDIM;++ix){
		//cov[i *ndim + j]
		//cout<<"Diag "<<ix<<" "<<TMath::Sqrt( cov[ix*NDIM+ix] )<<endl;

		hGCC->SetBinContent(ix+1,GetMinimizer()->GlobalCC(ix));
	}


	for(int ix=0;ix<NDIM;++ix){
		for(int iy=0;iy<NDIM;++iy){

			string namex=GetMinimizer()->VariableName(ix);
			string namey=GetMinimizer()->VariableName(iy);

			//if(iy>ix) continue;
			//cov[i *ndim + j]
			//cout<<"cov "<<ix<<" "<<iy<<"; "<<namex<<" "<<namey<<" "<<( cov[ix*NDIM+iy] )<<" "<<GetMinimizer()->CovMatrix(ix,iy)
						//		<<" "<<GetMinimizer()->Correlation(ix,iy)<<endl;

			fCovMatrix[ix][iy]=cov[ix*NDIM+iy];
			fCorrMatrix[ix][iy]=GetMinimizer()->Correlation(ix,iy);
			fHesseMatrix[ix][iy]=covh[ix*NDIM+iy];

			hCovMatrix->SetBinContent(ix+1,iy+1,cov[ix*NDIM+iy]);
			hCorrMatrix->SetBinContent(ix+1,iy+1,GetMinimizer()->Correlation(ix,iy));

		}
	}


	fCovMatrix.Print();
	fCorrMatrix.Print();
	fHesseMatrix.Print();

	fCovMatrix.Write("CovMat");
	fCorrMatrix.Write("CorrMat");
	fHesseMatrix.Write("HesseMat");


	hCovMatrix->Write();
	hCorrMatrix->Write();
	hGCC->Write();

	TMatrixD Multi;
	Multi.ResizeTo(NDIM,NDIM);
	Multi.Mult(fCovMatrix,fHesseMatrix);
	Multi.Print();
	Multi.Write("Multi");


	delete [] cov;
	delete [] covh;


	double *xscan=new double[100];
	double *yscan=new double[100];

	for(unsigned int i=0;i<plist->GetSize();++i){

		TSParameter* par= (TSParameter*)plist->At(i);

		//this will make the scan between +/- 2 sigmas
		double xmin=0;
		double xmax=0;

		unsigned int n=100;
		GetMinimizer()->Scan(i,n, xscan, yscan,xmin,xmax);

		TGraph *g=new TGraph(100,xscan,yscan);

		TString gname="gScan_";gname+=par->GetName();

		TString gtit="scan "; gtit += par->GetTitle(); gtit+=";";
		gtit+=par->GetAxisTitle();
		gtit+="; Minimization Function";

		g->SetName(gname);
		g->SetTitle(gtit);

		g->Write();
	}


}



void TSMinimizer::FitScan(TSParamList &par_list){


	int NPars = par_list.GetSize();

	if(NPars>2){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Max supported num of scan parameters is 2");
		return;
	}




}



void TSMinimizer::GetContourGraphs(TList &list) const{

	for(std::map< std::pair<TString,TString>,TGraph* >::const_iterator it=fContourGraphList.begin(), end=fContourGraphList.end();
				it!=end;
				++it){

		list.Add( (TGraph*)it->second );

	}

}


void TSMinimizer::GetCovMatrix(TMatrixD &Mat) const{


    if(fCovMatrix.GetNoElements()==0) return;

    Mat.Clear();

    Mat.ResizeTo(fCovMatrix.GetNrows(),fCovMatrix.GetNcols());


    Mat+=fCovMatrix;

}

void TSMinimizer::GetCorrMatrix(TMatrixD &Mat) const{


    if(fCorrMatrix.GetNoElements()==0) return;

    Mat.Clear();

    Mat.ResizeTo(fCorrMatrix.GetNrows(),fCorrMatrix.GetNcols());

    Mat+=fCorrMatrix;



}

void TSMinimizer::GetScanGraphs(TList &list) const{

	for(std::map<TString,TGraph* >::const_iterator it=fScanGraphList.begin(), end=fScanGraphList.end();
				it!=end;
				++it){

		list.Add( (TGraph*)it->second );

	}

}

void TSMinimizer::GetHistoryTree(TTree *Tree) const{


	if( !GetMinimizationFunction() ) return;

	TTree *t = GetMinimizationFunction()->GetHistoryTree();

	if(!t) return;

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	//t->Print();
	//t->Show(0);

	//delete Tree;

    Tree = t->CloneTree();

	cout<<"ok"<<endl;

	cout<<Tree<<endl;

	Tree->Print();




}


void TSMinimizer::Init(){


	fBestFitParamList = new TSParamList("BestFitParamList");

	SetNumOfContourPoints(100);
}


int TSMinimizer::Minimize() {

	/**
	 *
	 *
	 *
	 *
	 *
	 *
	 */


	cout<<endl<<" ================ TSMinimizer::Minimize =============== "<<endl;

	 Configure();

	 int NDIM = GetMinimizer()->NDim();

	 fMinFunction->GetModel()->NormalizeToIntegral( *( (TH2F*)fMinFunction->GetDataHistogram() ) );

	 GetMinimizer()->Minimize();

	 //fMinFunction->SaveHistory(false);

	 //GetMinimizer()->Hesse();

	 const double MinVal=GetMinimizer()->MinValue();

	 const double *parlist= GetMinimizer()->X();
	 const double *errlist=GetMinimizer()->Errors();


	 cout<<endl<<" =minimization done="<<endl;

	 GetMinimizer()->PrintResults();

	 cout<<"    ====    "<<endl;





	 //
	 // Covariance Matrix
	 //

	 //FillCovarianceMatrix();


	 //
	 // Contours
	 //

	 ComputeContours();



	 //
	 // restore params at best fit values
	 //

	 TSArgList *list = fMinFunction->GetModel()->GetPrimaryParamList();


	 cout<<" === GetMinosErrors === "<<endl;

	 double val=0,err=0,err_low=0,err_up=0,gcc=0;

	 for(int i=0; i<GetMinimizer()->NDim();++i){

		 val=parlist[i];
		 err=errlist[i];
		 //gcc=GetMinimizer()->GlobalCC(i);
		 //GetMinimizer()->GetMinosError(i,err_low,err_up);

		 //cout<<"-->BestFit: "<<i<<" "<<GetMinimizer()->VariableName(i)<<" "<<val<<" "<<err<<" "<<err_low<<" "<<err_up<<" "<<gcc<<endl;

		 TSParameter *par= (TSParameter*)list->At(i);

		 par->SetError(err);
		 par->SetAsymErrors(err_low,err_up);

	 }

	 cout<<" === GetMinosErrors end === "<<endl;

     cout<<" == Print Results after Minos "<<endl;

     GetMinimizer()->PrintResults();

     cout<<" == "<<endl;

	 for(int i=0; i<GetMinimizer()->NDim();++i){

	 		 val=parlist[i];
	 		 err=errlist[i];
	 		 //cout<<"-->BestFit: "<<i<<" "<<GetMinimizer()->VariableName(i)<<" "<<val<<" "<<err<<" "<<err_low<<" "<<err_up<<" "<<gcc<<endl;

	 		 TSParameter *par= (TSParameter*)list->At(i);

	 		 par->SetValue(val);
	 		 par->SetError(err);

	 	cout<<"-->BestFit: "<<i<<" "<<par->GetName()<<" "<<par->GetValue()<<"; "<<par->GetError()<<"; "<<par->GetErrorLow()<<" "<<par->GetErrorUp()<<endl;

	 }


	 fBestFitParamList->Copy(*list);

	 fBestFitParamList->Print();

	 for(int i=0; i<fBestFitParamList->GetSize();++i){
		 ((TSParameter*)fBestFitParamList->At(i))->Print();
	 }

	 cout<<endl<<" =============== Minimize End ============== "<<endl;


	 return 1;

}


void TSMinimizer::SaveContour(const TSParameter &p1, const TSParameter &p2){


	//first check if already present

	for(int i=0;i<fContourParamPairs.size();++i){

		if(        fContourParamPairs.at(i).first==p1.GetName()
				&& fContourParamPairs.at(i).second==p2.GetName())
		{
			MSG::WARNING(__FILE__,"::",__FUNCTION__," contour already sey for: ",p1.GetName()," - ",p2.GetName());
			return;
		}
	}

	fContourParamPairs.push_back( std::make_pair(p1.GetName(),p2.GetName()) );

}


void TSMinimizer::SaveContours(const TSParamList &list){



	for(int i=0; i<list.GetSize()-1; ++i){

		for(int j=i+1; j<list.GetSize(); ++j){

				SaveContour(  *list.At(i), *list.At(j) );

			}
	}


}

void TSMinimizer::SetFunction(TSMinimizationFunction &minim_func){


	MSG::LOG(" ===== TSMinimizer::SetFunction ===== ");

	//minim_func.SetNumOfDim( minim_func.NPar() );

	GetMinimizer()->Clear();

    fMinFunction = &minim_func;

    int NPARS = minim_func.NPar();

    //ROOT::Math::Functor functor1(minim_func,NPARS);
    //ROOT::Math::Functor functor1(&minim_func,&TSChisquareCalculator::EvalFunctor,NPARS);
    ROOT::Math::Functor *functor1 = new ROOT::Math::Functor(&minim_func,&TSMinimizationFunction::EvalFunctor,NPARS);



	GetMinimizer()->SetFunction(*functor1);




	//
	//set parameters
	//

	Configure();


	cout<<"=========================================="<<endl;


}
