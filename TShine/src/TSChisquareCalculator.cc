/*
 * TSChisquareCalcuator.cc
 *
 *  Created on: Aug 19, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>

#include <TMath.h>
#include <TRandom.h>


#include "TSChisquareCalculator.h"


TRandom r;



TSChisquareCalculator::TSChisquareCalculator() {

	Init();

}

TSChisquareCalculator::TSChisquareCalculator(TString name, TString title)
:TSMinimizationFunction(name,title)
{


	Init();

}

TSChisquareCalculator::~TSChisquareCalculator() {


	fClearCache();

}




double TSChisquareCalculator::Compute() const {

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
		cout<<"TSChisquareCalculator::Compute() "<<COUNT<<endl;
	}

	bool WARN_EVAL=1;

	// return ComputeCached();//
	//TH1 ia also a TH2
	TH2F *histo = (TH2F*)( GetDataHistogram() );

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

	 double chi2=0;


	 if( NDim()==1  ){

		int NBINSX = histo->GetNbinsX();

		for(int ib=1; ib<=NBINSX; ++ib){

			double xx=histo->GetBinCenter(ib);

			double cont=histo->GetBinContent(ib);

			if(cont==0) continue;

			double err=histo->GetBinError(ib);

			if(err==0) continue;

			double func= GetModel()->Eval(xx);

			double  chi = (func-cont)/err;

			if(TMath::IsNaN(chi)){
				if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," chi2 is NaN ",TMath::IsNaN(func));

				chi=1e7;
				//continue;
			}

			chi2 += chi*chi;
		}


	 }//DIM=1



	 if( NDim()==2  ){

		 int NBINSX = histo->GetNbinsX();
		 int NBINSY = histo->GetNbinsY();

		 //cout<<"NBINS "<<NBINSX<<" "<<NBINSY<<endl;

		 for(int ibx=1; ibx<=NBINSX; ++ibx){
		   for(int iby=1; iby<=NBINSY; ++iby){



			 double xx=histo->GetXaxis()->GetBinCenter(ibx);
			 double yy=histo->GetYaxis()->GetBinCenter(iby);


			 double cont=histo->GetBinContent(ibx,iby);

			 //if(cont==0) continue;

			 double err=histo->GetBinError(ibx,iby);

/*
			 double xx=fCacheX[ibx-1];
			 double yy=fCacheY[iby-1];

			 double cont=data2D[ibx-1][iby-1];
			 double err=errors2D[ibx-1][iby-1];
*/

			 if(err==0) continue;

			 double func= GetModel()->Eval(xx,yy);

			 if(TMath::IsNaN(func)){
				 if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," func is NaN. x-y point ",xx," ",yy);
				 GetModel()->Print();
				 assert(0);
			 }

			 double  chi = (func-cont)/err;

			 if(TMath::IsNaN(chi)){
				 if(WARN_EVAL) MSG::WARNING(__FILE__,"::",__FUNCTION__," chi2 is NaN ", TMath::IsNaN(func));

				 chi=1e7;
				 //continue;
			 }

			 chi2 += chi*chi;

			 //cout<<ibx<<" "<<iby<<"; "<<xx<<" "<<yy<<"; "<<cont<<" "<<err<<"; "<<func<<endl;

			}//iby
		 }//ibx

	 }//DIM=2


     return chi2;

}


void TSChisquareCalculator::Init() {

	MSG::LOG(__FILE__,"::",__FUNCTION__);

	fCached=0;

	fCacheX=0;
	fCacheY=0;
	fCacheData1D=0;
	fCacheData2D=0;

}


void TSChisquareCalculator::fClearCache() {


	fCached=0;

	if(fCacheX) delete [] fCacheX;
	if(fCacheY) delete [] fCacheY;
	if(fCacheData1D) delete [] fCacheData1D;
	if(fCacheData2D) delete [] fCacheData2D;

}


void TSChisquareCalculator::fFillCache() {




}
