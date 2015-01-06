/*
 * TSCovMatrixMgr.cc
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include "StringUtils.h"

#include "MessageMgr.h"

#include "TSCovMatrixMgr.h"


TSCovMatrixMgr::TSCovMatrixMgr() {

	Init();

}



TSCovMatrixMgr::TSCovMatrixMgr(TString name, TString title)
:TSNamed(name,name,title)
{

	Init();

}



TSCovMatrixMgr::~TSCovMatrixMgr() {


}



void TSCovMatrixMgr::Add(const TMatrixT<double> &M, TString opt)
{

	if(M.GetNrows() != M.GetNcols() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrix to Add is not symmetric: ",M.GetNrows(),"x",M.GetNcols());
		return;
	}


	if(M.GetNrows() != fMatrix.GetNrows() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrices to Add have different dimensions: ",M.GetNrows()," vs ",fMatrix.GetNrows());
		return;
	}

	int DIM = M.GetNrows();

	bool dosq = false;
	if(opt=="Squared") dosq=true;

	for(int i=0; i<DIM; ++i){
		for(int j=0; j<DIM; ++j){

				if( dosq ) fMatrix[i][j] += M[i][j]*M[i][j];
				else fMatrix[i][j] += M[i][j];
		}
	}


}

void TSCovMatrixMgr::AggregateMatrix(const TMatrixT<double> &M){


	/**
	 *
	 *
	 * Align another matrix along the diagonal
	 *
	 */


	TMatrixTSym<double> mold(fMatrix);

	fMatrix.Clear();

	int DIM1=mold.GetNrows();
	int DIM2=M.GetNrows();

	int DIM = DIM1+DIM2;

	fMatrix.ResizeTo(DIM,DIM);


	for(int i=0; i<DIM1; ++i){
		for(int j=0; j<DIM1; ++j){

			fMatrix[i][j]=mold[i][j];
		}
	}


	for(int i=0; i<DIM2; ++i){
		for(int j=0; j<DIM2; ++j){

			fMatrix[DIM1+i][DIM1+j]=M[i][j];

		}
	}

	fMatHasChanged = true;

	fDimension = fMatrix.GetNrows();

	fMatrix.Print();


	Update();

	fBuildHisto();

}



void TSCovMatrixMgr::Decompose()
{


	fDecompChol.SetMatrix(fMatrix);

	fDecompChol.Decompose();

	fMatrixU.ResizeTo(fMatrix.GetNrows(),fMatrix.GetNrows());
	fMatrixU.Zero();
	fMatrixU.SetSub(0,0,fDecompChol.GetU());

	fMatrixL.ResizeTo(fMatrix.GetNrows(),fMatrix.GetNrows());

	fMatrixL.Transpose(fMatrixU);


	fMatrixL.Write("L");
	fMatrixU.Write("U");

}



double TSCovMatrixMgr::GetElement(int ix, int iy) const
{

	if( ix<0 || ix >= fMatrix.GetNrows() || iy<0 || iy>=fMatrix.GetNcols() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrix indices out of range: ",ix," ",iy);
		return 0;
	}

	return fMatrix[ix][iy];

}


void TSCovMatrixMgr::ImportSubMatrix(const TH2 &h2, std::vector<std::pair<int,int> > &ranges){


	if( h2.GetNbinsX() != h2.GetNbinsY() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram is not a symmetric Matrix ");
		MSG::ERROR(" Histogram: ",h2.GetName()," nx/ny: ",h2.GetNbinsX()," ",h2.GetNbinsY());
		return;
	}


	int DIM = h2.GetNbinsX();

	TMatrixTSym<double> M(DIM);


	for(int ix=1;ix<=DIM;++ix){
		for(int iy=1;iy<=DIM;++iy){

			M[ix-1][iy-1]=h2.GetBinContent(ix,iy);

		}//ix
	}//iy

	ImportSubMatrix(M,ranges);


}


void TSCovMatrixMgr::ImportSubMatrix(const TH2 &h2, int first_ele, int last_ele){


	if( h2.GetNbinsX() != h2.GetNbinsY() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram is not a symmetric Matrix ");
		MSG::ERROR(" Histogram: ",h2.GetName()," nx/ny: ",h2.GetNbinsX()," ",h2.GetNbinsY());
		return;
	}


	int DIM = h2.GetNbinsX();

	TMatrixTSym<double> M(DIM);


	for(int ix=1;ix<=DIM;++ix){
		for(int iy=1;iy<=DIM;++iy){

			M[ix-1][iy-1]=h2.GetBinContent(ix,iy);

		}//ix
	}//iy

	ImportSubMatrix(M,first_ele,last_ele);

}


void TSCovMatrixMgr::ImportSubMatrixDiag(const TH1 &h1, int first_ele, int last_ele){


	int DIM = h1.GetNbinsX();

	TMatrixTSym<double> M(DIM);


	for(int ix=1;ix<=DIM;++ix){
			M[ix-1][ix-1]=h1.GetBinContent(ix);
	}//iy

	ImportSubMatrix(M,first_ele,last_ele);

}


void TSCovMatrixMgr::ImportSubMatrix(const TMatrixT<double> &M, std::vector<std::pair<int,int> > &ranges)
{


	int DIM = M.GetNrows();

	if( DIM != M.GetNcols() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not a symmetric Matrix ");
		return;
	}


	int nranges = ranges.size();


	if( nranges==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Empty range list ");
		return;
	}


	int last_ele= (ranges.at(nranges-1)).second;

	if( last_ele > DIM-1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrix dimension does not match import settings");
		return;
	}


	std::vector<int> range_offs;

	int Nele = 0;

	for(int i=0; i<nranges;++i){

		int efirst = (ranges.at(i)).first;
		int elast = (ranges.at(i)).second;

		int len = elast-efirst+1;


		range_offs.push_back(Nele);

		Nele += len;

	}


	fMatrix.ResizeTo(Nele,Nele);


	for(int ir=0; ir<nranges;++ir){

		int efirst = (ranges.at(ir)).first;
		int elast = (ranges.at(ir)).second;

		//fill in-block elements
		for( int i=efirst; i<=elast; ++i) {
			for( int j=efirst; j<=elast; ++j) {

				int ix = range_offs[ir] + i - efirst;
				int iy = range_offs[ir] + j - efirst;


				fMatrix[ix][iy] = M[i][j];

			}
		}


		//fill block-to-block elements
		if(ir==0) continue;

		//previous block
		int efirst_pre = (ranges.at(ir-1)).first;
		int elast_pre = (ranges.at(ir-1)).second;

		for( int i=efirst_pre; i<=elast_pre; ++i) {
			for( int j=efirst; j<=elast; ++j) {

				int ix = range_offs[ir-1] + i - efirst_pre;
				int iy = range_offs[ir] + j - efirst;

				fMatrix[ix][iy] = M[i][j];

				fMatrix[iy][ix] = M[j][i];
			}
		}


	}//ranges


	fMatHasChanged = true;

	fDimension = fMatrix.GetNrows();

	fMatrix.Print();

	Update();


	fBuildHisto();

}



void TSCovMatrixMgr::ImportSubMatrix(const TMatrixT<double> &M, int first_ele, int last_ele)
{


	int DIM = M.GetNrows();

	if( DIM != M.GetNcols() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not a symmetric Matrix ");
		return;
	}



	if( fHasParList ){

		int Npar=fParList->GetSize();

		last_ele = first_ele + Npar -1 ;

	}else{

		if( !last_ele ){
			last_ele=DIM-1;
		}

	}



	if( last_ele > DIM-1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrix dimension does not match import settings");
		return;
	}


	int Nele = last_ele - first_ele + 1;


	fMatrix.ResizeTo(Nele,Nele);


	for( int i=first_ele; i<=last_ele; ++i) {
		for( int j=first_ele; j<=last_ele; ++j) {

			fMatrix[i-first_ele][j-first_ele] = M[i][j];

		}
	}

	fMatHasChanged = true;

	fDimension = fMatrix.GetNrows();

	Update();

	//fMatrix.Print();

	fBuildHisto();

}


void TSCovMatrixMgr::Init()
{


	fDimension = 0;

	fMatHasChanged = false;

	fParList=0;

	fHasParList = false;

	hMatHisto.SetName(StringUtils::Name("hMat","_",GetName()));

	fExtracted = 0;

}


void TSCovMatrixMgr::Print()
{

	cout<<__FILE__<<"::"<<__FUNCTION__<<": "<<GetName()<<" "<<GetTitle()<<endl;

	fMatrix.Print();

	cout<<" ------ "<<endl;

}



void TSCovMatrixMgr::Randomize()
{

	if ( fMatHasChanged ){

		Decompose();

		fExt.ResizeTo(fDimension);

		fMatHasChanged = false;
	}


	for(int i=0; i<fDimension; ++i){

		fExt.GetMatrixArray()[i] = fRandom.Gaus(0.,1.);

	}


	fExt = fMatrixL * fExt;

	//fExt.Print();


	if( fHasParList ){

		//fParList->Print();

		fParList->Restore();

		fParList->AddToValues(fExt);

		//fExt.Print();

		//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;
		//fParList->Print();
		//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	}




}



void TSCovMatrixMgr::SetElement(int ix, int iy, double val)
{

	if( ix<0 || ix >= fMatrix.GetNrows() || iy<0 || iy>=fMatrix.GetNcols() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Matrix indices out of range: ",ix," ",iy);
		return;
	}

	fMatrix[ix][iy] = val;

	fMatHasChanged = true;

}

void TSCovMatrixMgr::SetElements(TSNuiParamSet &list, TString opt){

	fParList = &list;

	fHasParList=true;


	int N = fParList->GetSize();


	fMatrix.ResizeTo(N,N);
	fMatrix.Zero();

	for(int i=0; i<N; ++i){

		TSNuiParam *p = (TSNuiParam*)fParList->At(i);

		double err= p->GetPriorError();

		SetElement(i,i,err*err);
	}

}

void TSCovMatrixMgr::SetParameters(TSNuiParamSet &list, TString opt){


	if( GetDimension()==0 ){

		SetElements(list);

		fMatrix.Zero();

	}


	int N = list.GetSize();

	if(N != GetDimension() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Number of Parameters and Matrix dimension do not match: ",N," vs ", GetDimension());
		return;
	}

	fParList = &list;

	fHasParList=true;

}


void TSCovMatrixMgr::Update(){


	fCorr.ResizeTo(fDimension,fDimension);

	double sig_i,sig_j;

	for(int i=0; i<fDimension;++i){

		sig_i= fMatrix[i][i];

		if(sig_i<=0){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Diagonal Element ",i,"<=0 : ",sig_i);
			return;
		}

		sig_i = sqrt(sig_i);

		for(int j=0; j<fDimension;++j){

			sig_j= fMatrix[j][j];

			if(sig_j<=0){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," Diagonal Element ",j," <=0 : ",sig_j);
				return;
			}

			sig_j = sqrt(sig_j);

			fCorr[i][j] = fMatrix[i][j]/(sig_i*sig_j);

		}//i
	}//j


	//fCorr.Write("corr");
}


void TSCovMatrixMgr::fBuildHisto(){


	hMatHisto.Reset();

	int N=fMatrix.GetNrows();

	hMatHisto.SetBins(N, 0.5,N+0.5, N, 0.5,N+0.5);

	for(int i=0;i<N;++i){
		for(int j=0;j<N;++j){

			hMatHisto.SetBinContent(i+1,j+1,fMatrix[i][j]);
			//if(fMatrix[i][j]==0) hMatHisto.SetBinContent(i+1,j+1,-1);
		}
	}


	hMatHisto.Write();
}
