/*
 * TSLikelihoodCalculator.cc
 *
 *  Created on: Aug 22, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <TObject.h>
#include <TMath.h>

#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>

#include <TRandom.h>

#include "TSLikelihoodCalculator.h"

TSLikelihoodCalculator::TSLikelihoodCalculator()
{
	Init();
}

TSLikelihoodCalculator::TSLikelihoodCalculator(TString name, TString title)
:TSMinimizationFunction(name,title)
{
	Init();
}


TSLikelihoodCalculator::~TSLikelihoodCalculator() {

	fClearCache();
}



double TSLikelihoodCalculator::Compute() const {

	/**
	 *
	 *
	 *
	 *
	 *
	 *
	 */


	static int COUNT=0; COUNT++;
	if(COUNT%1000==0) {
		cout<<"TSLikelihoodCalculator::Compute() "<<COUNT<<endl;
	}


	bool WARN_EVAL=1;

	// return ComputeCached();//

	 //A TH1 is also a TH2
	 TH2F *histo =  (TH2F*)( GetDataHistogram() );

	 if( !histo){
		 MSG::ERROR(__FILE__,"::",__FUNCTION__," Null pointer to data histogram");
		 return 0;
	 }




	 if( !fCached ){

		 int NBINSX = histo->GetNbinsX();
		 int NBINSY = histo->GetNbinsY();

		 fCacheX = new double[NBINSX];
		 fCacheY = new double[NBINSY];

		 for(int ibx=1; ibx<=NBINSX; ++ibx){
			 for(int iby=1; iby<=NBINSY; ++iby){

				 fCacheX[ibx-1]=histo->GetXaxis()->GetBinCenter(ibx);
				 fCacheY[iby-1]=histo->GetYaxis()->GetBinCenter(iby);


				 data2D[ibx-1][iby-1]=histo->GetBinContent(ibx,iby);

				 errors2D[ibx-1][iby-1]=histo->GetBinError(ibx,iby);
			 }
		 }

		 fCached=true;
	 }

	 double LnL=0;
	 double LnL_Norm=0;

	 double Nexp_Tot=0; //Sum Of Model Yield
	 double Nobs_Tot=0;




	 if( NDim()==1  ){

		int NBINSX = histo->GetNbinsX();

		for(int ib=1; ib<=NBINSX; ++ib){

			double LL=0;

			double xx=histo->GetBinCenter(ib);

			double Nobs=histo->GetBinContent(ib);

			Nobs_Tot+=Nobs;

			double Nexp= GetModel()->Eval(xx);

			if( TMath::IsNaN(Nexp) ){
				if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," Function Evaluation is NaN. x point: ",xx);
				continue;
			}

			if( Nexp<0 ) continue;

			if(Nobs==0) LL = -Nexp;
			else LL = -Nexp + Nobs*TMath::Log(Nexp);

			if( TMath::IsNaN(LL) ){
				if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," LogL is NaN: ", Nexp," ", Nobs);
				continue;
			}

			LnL+=LL;
		}


	 }//DIM=1



	 if( NDim()==2  ){

		 int NBINSX = histo->GetNbinsX();
		 int NBINSY = histo->GetNbinsY();

		 for(int ibx=1; ibx<=NBINSX; ++ibx){
		   for(int iby=1; iby<=NBINSY; ++iby){


			   double LL=0;

			   double xx=histo->GetXaxis()->GetBinCenter(ibx);
			   double yy=histo->GetYaxis()->GetBinCenter(iby);

			   double Nobs=histo->GetBinContent(ibx,iby);

			   //double xx=fCacheX[ibx-1];
			   //double yy=fCacheY[iby-1];
			   //double Nobs=data2D[ibx-1][iby-1];
			   //double err=errors2D[ibx-1][iby-1];


			 Nobs_Tot+=Nobs;


			 double Nexp= GetModel()->Eval(xx,yy);

			 if( TMath::IsNaN(Nexp) ){
				 if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," Function Evaluation is NaN. x-y point: ",xx," ",yy);
				 continue;
			 }

			 if( Nexp<0 ) continue;

			 if(Nobs==0) LL = -Nexp;
			 else LL = -Nexp + Nobs*TMath::Log(Nexp);

			 if( TMath::IsNaN(LL) ){
				 if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," LogL is NaN: ",Nexp," ",Nobs);
				 continue;
			 }

			 LnL+=LL;

			}//iby
		 }//ibx

	 }//DIM=2


     return -2*LnL;

}



void TSLikelihoodCalculator::Init() {

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	fCached=0;

	fCacheX=0;
	fCacheY=0;
	fCacheData1D=0;
	fCacheData2D=0;

}


void TSLikelihoodCalculator::fClearCache() {


	fCached=0;

	if(fCacheX) delete [] fCacheX;
	if(fCacheY) delete [] fCacheY;
	if(fCacheData1D) delete [] fCacheData1D;
	if(fCacheData2D) delete [] fCacheData2D;

}


void TSLikelihoodCalculator::fFillCache() {




}

