/*
 * TSDEdxGenerator.cc
 *
 *  Created on: Jun 14, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#include <cassert>
#include <cmath>
#include <vector>

#include <TRandom.h>
#include <TMath.h>

#include "TH1FAuto.h"

#include "StringUtils.h"

#include "TSDEdxGenerator.h"


#include "MessageMgr.h"




TSDEdxDistrib::TSDEdxDistrib(TString fname){

	Init();

	if(fname == "Landau" ){
		fIsLandau = true;
	}


}



int TSDEdxDistrib::GetNpar() const{

	int np=0;

	for(std::map<int,bool>::const_iterator it=par_used.begin(), end=par_used.end();
			it!=end;++it){

		if( it->second ) np++;

	}

	return np;

}



int TSDEdxDistrib::GetNpar(){

	int np=0;

	for(int i=0; i<par_used.size(); ++i){

		if( par_used[i] ) np++;
	}

	return np;

}


void TSDEdxDistrib::Init(){

	GNorm=2.50662827463100024; //sqrt(2*Pi)=2.50662827463100024
	landau_mpv_shift=0.22278;

	mean=0;
    sigma=0;
    sigma_tails=0;
    ratio_tails=0;
    sigma_scale_base=1;
	alpha=0;
	delta=0;
	spread=0;
	npoints = 1;

	ip_mean=0;
	ip_sigma=1;
	ip_sigma_tails=2;
	ip_ratio_tails=3;
	ip_sigma_scale_base=4;
	ip_sigma_scale_pow=5;
	ip_delta=6;
	ip_spread=7;
	ip_npoints=8;

	par_name[ip_mean]="mean";
	par_name[ip_sigma]="#sigma";
	par_name[ip_sigma_tails]="#sigma_{tails}";
	par_name[ip_ratio_tails]="tail ratio";
	par_name[ip_sigma_scale_base]="m";
	par_name[ip_sigma_scale_pow]="#alpha";
	par_name[ip_delta]="#delta";
	par_name[ip_spread]="width smear";
	par_name[ip_npoints]="n points";

	fMeanSet = false;
	fSigmaSet= false;
	fSigmaTailsSet= false;
	fRatioTailsSet= false;
	fSigmaScaleBaseSet= false;
	fSigmaScalingSet= false;
	fDeltaSet= false;
	fSpreadSet= false;
	fNPointsSet=false;

	fIsLandau = false;

	for(int i=0;i<par_name.size();++i){

		par_used[i]=false;
		par_num[par_name[i]]=i;
		par_val[i]=0;

	}


	SetMin(0);
	SetMax(0);

	HasPointDistr = false;

    mult.clear();
	npts.clear();

}

void TSDEdxDistrib::Print() {

	MSG::LOG("TSDEdxDistrib::Print()","------");

	int np=0;

	for(int i=0;i<par_name.size();++i){

		if( par_used[i] ){
		 cout<<np<<", "<<i<<" "<<par_name[i]<<" = "<<par_val[i]<<endl;
		 np++;
		}

	}

	MSG::LOG("TSDEdxDistrib::Print()"," ----- ");


}



void TSDEdxDistrib::InitFunctionParNames(TF1 &f){

	int np=0;

	for(int i=0; i<par_used.size(); ++i){

		if( par_used[i] ){

			f.SetParName(np, par_name[i]);

			np++;
		}

	}


}


void TSDEdxDistrib::SetFunctionPars(TF1 &f){

	int np=0;

	for(int i=0; i<par_used.size(); ++i){

		if( par_used[i] ){

			f.SetParameter(np,par_val[i]);

			np++;
		}

	}


}



double TSDEdxDistrib::PointAveragedDistrib(double *x, double *p){


	/**
	 *
	 *
	 *
	 *
	 */

	static int count=0; count++;

	//cout<<"0655301474 "<<count<<" "<<x[0]<<endl;

	int NT=mult.size();

	if(NT==0) return 0;


	double val=0;
	double nsum=0;

	for(int i=0; i<NT; ++i){

		float M = mult.at(i);
		float N = npts.at(i);

		nsum += M;

		SetNPoints(N);

		double p;

		val += M*Distrib(&x[0],&p);

	}

	//cout<<"0655301474 "<<x[0]<<" "<<val<<" "<<nsum<<" "<<0<<" "<<0<<" 2"<<endl;
	return val/nsum;


}


double TSDEdxDistrib::Distrib(double *x, double *p){

/**
 *
 *
 *
 *
 *
 *
 *
 */


	if( fIsLandau ){

		return TMath::Landau(x[0],mean+landau_mpv_shift,sigma,1);

	}



    double s = sigma*TMath::Power(sigma_scale_base,alpha);

    double s1 = s*(1-delta);
    s1 = TMath::Sqrt(s1*s1+spread*spread);
    if(npoints >1 ) s1 /= TMath::Sqrt(npoints);

    double s2= s*(1+delta);
    s2 = TMath::Sqrt(s2*s2+spread*spread);
    if(npoints >1 ) s2 /= TMath::Sqrt(npoints);

    double integ = GNorm*0.5*(s1+s2);

    double val = 0;

    if(x[0]<mean){
    	val = TMath::Gaus(x[0],mean,s1,0);
    }else{
    	val = TMath::Gaus(x[0],mean,s2,0);
    }

    val/=integ;

    double sigt = sigma_tails;
    //do not scale tails sigma with num of points
    //gives strange behavoiur in GetRandom()
    //if(npoints >1 ) sigt = sigma_tails/TMath::Sqrt(npoints);


    double val_tails=TMath::Gaus(x[0],mean,sigt,1);


    //cout<<"0655301474 "<<x[0]<<" "<<val<<" "<<val_tails<<" "<<ratio_tails<<" "<<npoints<<" 1"<<endl;

    val = (1-ratio_tails)*val + ratio_tails*val_tails;

    //cout<<"m: "<<mean<<" s: "<<sigma<<" d: "<<delta<<" N: "<<npoints<<" s1: "<<s1<<" s2: "<<s2<<" sig_t: "<<sigma_tails<<" r: "<<ratio_tails<<endl;
    //cout<<" s_c_b: "<<sigma_scale_base<<" alpha: "<<alpha<<" spread: "<<spread<<" s: "<<s<<endl;

    //cout<<par_val[0]<<" "<<par_val[1]<<" "<<par_val[2]<<endl;
    //cout<<x[0]<<" "<<val<<endl;
    //cout<<endl;



    if(x[0]>0.3 && x[0]<0.35){
    	//cout<<"a "<<x[0]<<" "<<val<<" "<<npoints<<" "<<sigma<<" "<<s1<<" "<<s2<<endl;
    }

    if(x[0]>0.4 && x[0]<0.5){
    	//cout<<"b "<<x[0]<<" "<<val<<" "<<npoints<<" "<<sigma<<" "<<s1<<" "<<s2<<endl;
    }

    return val;

}


void TSDEdxDistrib::SetParameters(double m, double s, double s2, double r, double scale, double alpha
		, double delta, double rms, double npoints){

	SetMean(m);
	SetSigma(s);
    SetSigmaTails(s2);
	SetRatioTails(r);
	SetSigmaScaleBase(scale);
	SetSigmaScaling(alpha);
	SetDelta(delta);
	SetSpread(rms);
	SetNPoints(npoints);

}


void TSDEdxDistrib::SetPointDistrib(TH1F &h){



	mult.clear();
	npts.clear();


	for(int in=1; in<= h.GetNbinsX(); ++in){

		if( h.GetBinContent(in)>0 ){

			mult.push_back( h.GetBinContent(in) );
			npts.push_back( h.GetBinCenter(in) );
		}

	}

	HasPointDistr = true;

}




void TSDEdxDistrib::SetMean(double val) {
	int ip=ip_mean;
	mean = val; par_val[ip]=val; par_used[ip]=true;
	fMeanSet = true;
}
void TSDEdxDistrib::SetSigma(double val) {
	int ip = ip_sigma;
	sigma = val; par_val[ip]=val; par_used[ip]=true;
	fSigmaSet = true;
}
void TSDEdxDistrib::SetSigmaTails(double val) {
	int ip = ip_sigma_tails;
	sigma_tails = val; par_val[ip]=val; par_used[ip]=true;
	fSigmaTailsSet = true;
}
void TSDEdxDistrib::SetRatioTails(double val) {
	int ip = ip_ratio_tails;
	ratio_tails = val; par_val[ip]=val; par_used[ip]=true;
	fRatioTailsSet = true;
}
void TSDEdxDistrib::SetSigmaScaleBase(double val) {
	int ip = ip_sigma_scale_base;
	sigma_scale_base = val; par_val[ip]=val; par_used[ip]=true;
	fSigmaScaleBaseSet = true;
}
void TSDEdxDistrib::SetSigmaScaling(double val) {
	int ip = ip_sigma_scale_pow;
	alpha = val; par_val[ip]=val; par_used[ip]=true;
	fSigmaScalingSet = true;
}
void TSDEdxDistrib::SetDelta(double val) {
	int ip = ip_delta;
	delta = val; par_val[ip]=val; par_used[ip]=true;
	fDeltaSet = true;
}
void TSDEdxDistrib::SetSpread(double val) {
	int ip=ip_spread;
	spread = val; par_val[ip]=val; par_used[ip]=true;
	fSpreadSet = true;
}
void TSDEdxDistrib::SetNPoints(double val) {
	int ip=ip_npoints;
	npoints = val; par_val[ip]=val; par_used[ip]=true;
	fNPointsSet = true;
}


//
//
//
//
//
//







namespace TSDedxGenerator{

    double GNorm=2.50662827463100024; //sqrt(2*Pi)=2.50662827463100024

    static double Spread=0;



    double Distribution(double *x, double *p){

    	/**
    	 *
    	 *
    	 *  p[0]: mean
    	 *  p[1]: sigma
    	 *  p[2]: scaling-alpha
    	 *  p[3]: asymmetry-delta
    	 *  p[4]: sigma spread
    	 *
    	 *
    	 *  sigma = s0*mean**alpha*(1-delta)
    	 *
    	 *
    	 *
    	 */


    	 double m  = p[0];
    	 double s0 = p[1];
    	 double alpha = p[2];
    	 double delta = p[3];
    	 double spread = p[4];


    	 double s = s0*TMath::Power(m,alpha);

    	 double s1 = s*(1-delta);
    	 s1 = TMath::Sqrt(s1*s1+spread*spread);

    	 double s2= s*(1+delta);
    	 s2 = TMath::Sqrt(s2*s2+spread*spread);


    	 double integ = GNorm*0.5*(s1+s2);

    	 double val = 0;

    	 if(x[0]<m){
    		 val = TMath::Gaus(x[0],m,s1,0);
    	 }else{
    		 val = TMath::Gaus(x[0],m,s2,0);
    	 }

    	return val/integ;


    }


	double DoubleTailedGaussian(double *x, double *p){

		/**
		 *
		 *  p[0] smaller sigma
		 *  p[1] larger sigma
		 *  p[2] amplitude ratio
		 *
		 */

		double s1= p[0];
		double s2= p[1];
		double frac = p[2];

		double val = (1-frac)*TMath::Gaus(x[0],0,s1,1) + frac*TMath::Gaus(x[0],0,s2,1);
		return val;

	}//


	double AsymmGaussian(double *x, double *p){

			/**
			 *
			 *  p[0] sigma
			 *  p[1] delta
			 *
			 *
			 *
			 */


			double s= p[0];
			double d= p[1];

			double val;

			if( x[0] >0 ) {

				s = s*(1+d);

				val = TMath::Gaus(x[0],0,s,0);

			}else{

				s = s*(1-d);

				val = TMath::Gaus(x[0],0,s,0);

			}


			return val/(s*GNorm);

		}//



	   std::vector<float> fN;
	   std::vector<float> fM;

	   double PointAveGaussian(double *x, double *p){

		   /**
		    *
		    *  sigma = p[0]
		    *
		    *
		    */


		   int NP = fN.size();

		   double s = p[0];

		   double msum = 0;

		   double val = 0;

		   for(int ip=0; ip<NP; ++ip){

			   float n = fN.at(ip);
			   float m = fM.at(ip);

			   msum += m;

			   s = s/TMath::Sqrt(n);

			   val += m*TMath::Gaus(x[0],0,s,1);
		   }


		   return val/msum;

	   }



	   double PointAveDistribution(double *x, double *p){

		   /**
		    *
		    * Function Distribution(double *x,double* p)
		    * averaged according to number of points
		    * distribution
		    *
		    *
		    * Refer to the implementation of
		    * the function Distribution for
		    * the params description
		    *
		    *
		    *  sigma = s0*m**alpha*(1-delta)/sqrt(n)
		    *
		    *
		    */


		   double mean  = p[0];
		   double s0 = p[1];
		   double alpha = p[2];
		   double delta = p[3];
		   double spread = p[4];

		   double xx = x[0];


		   int NP = fN.size();

		   double val = 0;

		   double nsum=0;

		   for(int ip=0; ip<NP; ++ip){
			   float n = fN.at(ip);
			   float mult = fM.at(ip);

			   nsum += mult;

			   double s = s0/TMath::Sqrt(n);

			   double params[5]={mean,s,alpha,delta,spread};

			   val += mult*Distribution(&x[0],params);

		   }

		   return val/nsum;

	   }




	   double Landau(double* x, double* p){

		   /**
		    *
		    * The Most Probable Value is located at -0.22278
		    * The mu parameter is chosen in order to shift
		    * the MPV to 0.
		    *
		    */

		   double s=p[0];

		   double mu = 0.22278;

		   return TMath::Landau(x[0],mu,s,1);

	   }



}




/******************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************/




TSDEdxGenerator::TSDEdxGenerator(TString name, TString title)
:TSNamed(name, "",title )
{


	Init();
}


TSDEdxGenerator::TSDEdxGenerator(TString name, TString title, const TSVariable &dedx_var)
:TSNamed(name, "",title )
{

	Init();

	SetDEdxVariable(dedx_var);

}


TSDEdxGenerator::~TSDEdxGenerator() {
	// TODO Auto-generated destructor stub


	delete fTrackDistrib;
	delete fDistrib;

}



void TSDEdxGenerator::ApplyCalibrationCorrection(bool b) {

	fApplyCalibCorr=b;
}

double TSDEdxGenerator::ComputeAverage(std::vector<double> &list, double trunc_low, double trunc_up, int algo, double param) const{

/**
 *   Compute Truncated average between trunc_low and trunc_up fractions
 *   of a list of charges.
 *
 *   Different options to compute the average
 *
 *   algo = 0: Standard
 *
 *
 *
 *
 *
 *
 *
 */


   if(trunc_low >= trunc_up){
	   MSG::ERROR(__FILE__,"::",__FUNCTION__);
	   return 0;
   }

   int nclu_tot = list.size();

   fNumOfUsedClusters=nclu_tot;


	if(nclu_tot==0){
		return 0;
	}


   if( algo == 0){

	   if(nclu_tot == 1) return list.at(0);

	   sort(list.begin(),list.end());

	   int idx1 = TMath::Nint(nclu_tot*trunc_low);
	   if(idx1>=1) idx1 -= 1;


	   int idx2 = TMath::Nint(nclu_tot*trunc_up);
	   if(idx2>=1) idx2 -= 1;

	   int nc = idx2-idx1+1;

	   fNumOfUsedClusters=nc;

	   if(nc==0){
		   MSG::ERROR(__FILE__,"::",__FUNCTION__," error in the truncation algo");
		   return 0;
	   }

	   if(nc == 1){
		 return list.at(idx1);
	   }

	   double ave=0;

	   for(int i=idx1; i<=idx2; ++i){
		   ave += list.at(i);
	   }

	   ave /= nc;

	   return ave;

   }


   return 0;

}



double TSDEdxGenerator::Generate() const{



	if( !fTrackDistrib ){
		return 0;
	}


	if( fTrackDistrib->InheritsFrom("TF1") ){

		return GetMean() + ( (TF1*)fTrackDistrib )->GetRandom();

	}else if( fTrackDistrib->InheritsFrom("TH1") ) {

		return GetMean() + ( (TH1*)fTrackDistrib )->GetRandom();

	}


}


double TSDEdxGenerator::Generate(double momentum, double mass, int num_clusters) const{

	if( fBetheBloch ){

		double dedx_mean = fBetheBloch->Eval(momentum,mass);


		SetMean(dedx_mean);

		if(num_clusters == 0){

			return Generate();

		}else{

			return Generate(num_clusters);
		}

	}


	MSG::ERROR(__FILE__,"::",__FUNCTION__," Bethe-Bloch function not set");

	return 0;
}



double TSDEdxGenerator::Generate(int num_clusters) const{


	if(num_clusters <= 0 ){
		return 0;
	}



	double calib_shift=0;

	if( fApplyCalibCorr ){
		calib_shift= fComputeCalibrationCorrection(num_clusters);
	}

	//cout<<calib_shift<<endl;


	std::vector<double> charge_list;

	for(int i=0; i<num_clusters; ++i){

		double charge = Generate();


		// remove the mean before or after computing the truncated average
		// is the equivalent if the normal mean is used, not if harmonic
		// or other alogs are used for that

		charge -= GetMean();

		charge_list.push_back(charge);

	}

	double ave = ComputeAverage(charge_list,fTruncLow,fTruncUp,fAverageAlgoType,fAverageAlgoParam);

	ave -= calib_shift;

	ave += GetMean();


	return ave;

}




TObject* TSDEdxGenerator::GetTrackDEdxDistrib() const{

	if( fTrackDistrib ){

		//return fTrackDistrib->Copy();

		if(fDistrib){
			fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));
		}

		return fTrackDistrib;
	}

	return 0;

}


TObject* TSDEdxGenerator::ImportTrackDEdxDistrib() const{

	if( fTrackDistrib ){

		//return fTrackDistrib->Copy();

		if(fDistrib){
			fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));
		}

		return fTrackDistrib->Clone();
	}

	return 0;

}



void TSDEdxGenerator::Init() {

	fTrackDedxDistrName = "TrackDedxDistr_";
	fClusterDedxDistrName = "ClusterDedxDistr_";


	fDistrib= 0; //new TSDEdxDistrib();

	fTrackDistrib = 0;

    fBetheBloch = 0;

	SetMean(0);
	fNumOfUsedClusters=0;
	SetTruncation(1,0);


	fClusterChargeDistrTitle="dE/dx cluster charge distribution";
	fTrackChargeDistrTitle="dE/dx track charge distribution";

	fVariableX.SetLabel("dE/dx");
	fVariableX.SetUnit("arb. units");
	fVariableX.SetTitle("dE/dx distribution");

	fIsClusterDistrib=false;
	fIsTrackDistrib=false;

	ApplyCalibrationCorrection(false);

	SetAverageAlgoType(0);
	SetAverageAlgoParam(0);

}


void TSDEdxGenerator::SetAverageAlgoType(int algo){

	fAverageAlgoType=algo;
}

void TSDEdxGenerator::SetAverageAlgoParam(double p){

	fAverageAlgoParam=p;
}


void TSDEdxGenerator::SetAsymmetry(double val){

    if( !fDistrib ) return;

	if( !fDistrib->DeltaSet() ){
		fDistrib->SetDelta(val);
		fInitTrackDistrib();
	}else{
		fDistrib->SetDelta(val);
		fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));

	}


}

void TSDEdxGenerator::SetBetheBloch( const TSBetheBlochFunc &f){

	fBetheBloch = &f;

}



void TSDEdxGenerator::SetClusterChargeDistrib(TF1 &f){


	SetTrackDEdxDistrib(f);


	fSetIsClusterDistrib();

	fSetDistribTitles();

}


void TSDEdxGenerator::SetClusterChargeDistrib(TH1F &h){


	SetTrackDEdxDistrib(h);

	fSetIsClusterDistrib();

	fSetDistribTitles();

}


void TSDEdxGenerator::SetClusterChargeDistrib(TString fname, double min, double max,
		double p0, double p1, double p2, double p3) {


	SetTrackDEdxDistrib(fname, min, max, p0, p1, p2, p3);

	fSetIsClusterDistrib();

	fSetDistribTitles();


}



void TSDEdxGenerator::SetDEdxVariable(const TSVariable &v){

	fVariableX.Copy(v);

}



void TSDEdxGenerator::SetMean(double val) const{

	fMean = val;

	//that's not what you want especially if mean=0
	//if( fDistrib && fDistrib->SigmaScaleBaseSet() ){
	//	SetSigmaScaling(fMean, fDistrib->GetSigmaScalePow() );
	//}


}


void TSDEdxGenerator::SetPointDistrib(TH1F &h) {

	if( !fDistrib ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," cannot apply ... no distribution is set yet... ");
		return;
	}

	fDistrib->SetPointDistrib(h);
	fDistrib->SetNPoints(1);

	fInitTrackDistrib();

}




void TSDEdxGenerator::SetSigmaScaling(double m, double alpha){

	if( !fDistrib ) return;



	if( !(fDistrib->SigmaScaleBaseSet()&&fDistrib->SigmaScalingSet()) ){
		fDistrib->SetSigmaScaleBase(m);
		fDistrib->SetSigmaScaling(alpha);
		fInitTrackDistrib();
	}else{
		fDistrib->SetSigmaScaleBase(m);
		fDistrib->SetSigmaScaling(alpha);
		fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));
	}


}



void TSDEdxGenerator::SetTrackDEdxDistrib(TH1F &h) {

	delete fTrackDistrib;

	fTrackDistrib = new TH1F(h);


	fSetIsTrackDistrib();

	fSetDistribTitles();


}


void TSDEdxGenerator::SetTrackDEdxDistrib(TF1 &f) {

	delete fTrackDistrib;

	fTrackDistrib = new TF1(f);


	fSetIsTrackDistrib();


	fSetDistribTitles();
}



void TSDEdxGenerator::SetTrackDEdxDistrib(TString fname, double min, double max,
		double p0, double p1, double p2, double p3) {


/**
 *  Configure built in functions via the functor class TSDEdxDistrib
 *
 *   "Gaus"
 *      p0 = mean
 *      p1 = sigma
 *
 *    "DoubleTailedGaus"
 *      p0 = mean
 *      p1 = sigma
 *      p2 = sigma_tail
 *      p3 = amplitude ratio
 *
 *
 *    "AsymGaus"
 *      p0 = mean
 *      p1 = sigma
 *      p2 = delta
 *
 *   "Landau"
 *     p0 = mpv;
 *     p1 = width
 *
 */


	if( fname == "Gaus" || "Gaussian" ){

		fDistrib = new TSDEdxDistrib(fname);
		fDistrib->SetMean(p0);
		fDistrib->SetSigma(p1);

	}else if(fname == "DoubleTailedGaus"){

		fDistrib = new TSDEdxDistrib(fname);
		fDistrib->SetMean(p0);
		fDistrib->SetSigma(p1);
		fDistrib->SetSigmaTails(p2);
		fDistrib->SetRatioTails(p3);


	}else if(fname == "AsymGaus"){

		fDistrib = new TSDEdxDistrib(fname);
		fDistrib->SetMean(p0);
		fDistrib->SetSigma(p1);
		fDistrib->SetDelta(p2);

	}else if(fname == "Landau" ){

		fDistrib = new TSDEdxDistrib(fname);

		fDistrib->SetMean(p0);
		fDistrib->SetSigma(p1);

	}else{

		return ;
	}


	fDistrib->SetMin(min);
	fDistrib->SetMax(max);


	//fDistrib->Print();

	fSetIsTrackDistrib();

	fInitTrackDistrib();

	fSetDistribTitles();

}



void TSDEdxGenerator::SetTruncation(double up, double low){

	fTruncUp = up;
	fTruncLow = low;
}



void TSDEdxGenerator::SetWidthSmearing(double val){

	if( !fDistrib ) return;

	if( !fDistrib->SpreadSet() ){
		fDistrib->SetSpread(val);
		fInitTrackDistrib();
	}else{
		fDistrib->SetSpread(val);
		fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));

	}


}



double TSDEdxGenerator::fComputeCalibrationCorrection(int nclus) const{

/**
 *
 *  Temporary solution:
 *  Redundant and very slow
 *
 *
 */

	int NSAMP=1000;



	double calib_corr=0;

	TH1FAuto hq;

	std::vector<double> q_list;


	for(int is=0; is<NSAMP;++is){

		q_list.clear();

		for(int i=0; i<nclus; ++i){

			double charge = Generate();

			charge -= GetMean();

			q_list.push_back(charge);

		}

		double ave = ComputeAverage(q_list,fTruncLow,fTruncUp,fAverageAlgoType,fAverageAlgoParam);

		hq.Fill(ave);

	}

	calib_corr = hq.GetMean();


	return calib_corr;

}



void TSDEdxGenerator::fSetDistribTitles() {


	if( !fTrackDistrib ){

		return;
	}



	TString name = fTrackDedxDistrName; //"TrackDedxDistr_";
	//if(fIsClusterDistrib) name=fClusterDedxDistrName;// skip for now

	name+= Name();

	((TNamed*)fTrackDistrib)->SetName(name);




	TString title,tity;

	if(fTrackDistrib->InheritsFrom("TF1")) tity="";
	if(fTrackDistrib->InheritsFrom("TH1F")) tity="entries";


	if( fIsClusterDistrib ) title=STR_UTILS::TitleXYZ(fClusterChargeDistrTitle,fVariableX.GetAxisTitle(),tity);
	else if( fIsTrackDistrib ) title=STR_UTILS::TitleXYZ(fTrackChargeDistrTitle,fVariableX.GetAxisTitle(),tity);


	if(fTrackDistrib->InheritsFrom("TF1")){

		((TF1*)fTrackDistrib)->SetTitle( title  );
	}

	if(fTrackDistrib->InheritsFrom("TH1F")){

		((TH1F*)fTrackDistrib)->SetTitle( title);
	}


}



void TSDEdxGenerator::fInitTrackDistrib() {


    if( fDistrib ) {

		delete fTrackDistrib;


		int NP = fDistrib->GetNpar();

		double min = fDistrib->GetMin();
		double max = fDistrib->GetMax();

		if( fDistrib->HasPointDistrib() ){

			fTrackDistrib = new TF1("f",fDistrib,&TSDEdxDistrib::PointAveragedDistrib,min,max,NP,"TSDEdxDistrib","PointAveragedDistrib");

		}else{

			fTrackDistrib = new TF1("f",fDistrib,&TSDEdxDistrib::Distrib,min,max,NP,"TSDEdxDistrib","Distrib");

		}


		fDistrib->InitFunctionParNames(*((TF1*)fTrackDistrib));

		fDistrib->SetFunctionPars(*((TF1*)fTrackDistrib));


    }



}


void TSDEdxGenerator::fSetIsClusterDistrib(){

	fIsClusterDistrib=true;
	fIsTrackDistrib=false;
}

void TSDEdxGenerator::fSetIsTrackDistrib(){

	fIsClusterDistrib=false;
	fIsTrackDistrib=true;
}




