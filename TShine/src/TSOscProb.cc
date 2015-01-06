/*
 * TSOscProb.cc

 *
 *  Created on: Oct 20, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>

#include <TMath.h>

#include "StringUtils.h"
#include "RootUtils.h"

#include "MessageMgr.h"

#include "TSOscProb.h"


TSOscProb::TSOscProb()
:TSNamed()
,fVarEne("E","E_{#nu}","Neutrino Energy", "GeV")
,fVarLen("L","L_{#nu}","Neutrino Decay Length", "km")
{

	Init();
}


TSOscProb::TSOscProb(TString name, TString title)
:TSNamed(name,"",title)
,fVarEne("E","E_{#nu}","Neutrino Energy", "GeV")
,fVarLen("L","L_{#nu}","Neutrino Decay Length", "km")
{


	Init();
}


TSOscProb::~TSOscProb() {

	delete [] fParamCache;

}

void TSOscProb::BindToEnergyVariable(TSVariable &v)
{
	fBindVarEne = &v;

}

void TSOscProb::BindToLengthVariable(TSVariable &v)
{
	fBindVarLen = &v;

}

TGraph* TSOscProb::BuildGraph_ProbVsE(int npts, double min, double max){

	TGraph *g = new TGraph();


	double log_step = 0;
	if( fIsXbinningLog ){
		log_step = log10(max/min)/double(npts*1.);
	}

	for(int i=0; i<npts;++i){

		double ene = min + (max-min)*i/(npts-1);

		if( fIsXbinningLog ) {ene = min*pow( 10.0 , double(i)*log_step );}

		SetE(ene);

		double prob = Eval();


		g->SetPoint(g->GetN(), ene, prob);

	}


	g->SetMaximum(1.);
	g->SetMinimum(0.);

	g->SetTitle(GetTitle());
	g->GetXaxis()->SetTitle(fVarEne.GetAxisTitle());
	g->GetYaxis()->SetTitle(StringUtils::Name("P(",GetTransitionLabel(),")"));

	return g;
}


TGraph* TSOscProb::BuildGraph_ProbVsL(int npts, double min, double max){

	TGraph *g = new TGraph();


	double log_step = 0;
	if( fIsXbinningLog ){
		log_step = log10(max/min)/double(npts*1.);
	}

	for(int i=0; i<npts;++i){

		double len = min + (max-min)*i/(npts-1);

		if( fIsXbinningLog ) {len = min*pow( 10.0 , double(i)*log_step );}

		SetL(len);

		double prob = Eval();

		g->SetPoint(g->GetN(), len, prob);

	}


	g->SetMaximum(1.);
	g->SetMinimum(0.);

	g->SetTitle(GetTitle());
	g->GetXaxis()->SetTitle(fVarLen.GetAxisTitle());
	g->GetYaxis()->SetTitle(StringUtils::Name("P(",GetTransitionLabel(),")"));

	return g;
}


TGraph* TSOscProb::BuildGraph_ProbVsLoverE(int npts, double min, double max){


	TGraph *g = new TGraph();

	double log_step = 0;
	if( fIsXbinningLog ){
		log_step = log10(max/min)/double(npts*1.);
	}

	for(int i=0; i<npts;++i){

		double LE = min + (max-min)*i/(npts-1);
		if( fIsXbinningLog ) {LE = min*pow( 10.0 , double(i)*log_step );}

		double len = LE;
		double ene = 1;

		SetEandL(ene,len);

		double prob = Eval();

		g->SetPoint(g->GetN(), LE, prob);

	}

	g->SetMaximum(1.);
	g->SetMinimum(0.);
	g->SetTitle(GetTitle());
	g->GetXaxis()->SetTitle(StringUtils::Name("L/E [",fVarLen.GetUnit(),"/",fVarEne.GetUnit(),"]"));
	g->GetYaxis()->SetTitle(StringUtils::Name("P(",GetTransitionLabel(),")"));

	return g;
}


TGraph* TSOscProb::BuildGraph_ProbVsParam(TSParameter &par, int npts, double min, double max){


	TGraph *g = new TGraph();

	double val_save = par.GetValue();

	for(int i=0; i<npts;++i){

		double val;

		par.SetValue(val);

		FillParameters();

		double prob = Eval();

		g->SetPoint(g->GetN(), val, prob);

	}

	g->GetXaxis()->SetTitle(par.GetAxisTitle());

	par.SetValue(val_save);

	return g;

}



void TSOscProb::FillParameter(int i){


	if(i<0 || i>=fNumOfParams){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Index value: ",i," out of range");
		return ;
	}

	fParamCache[i] = fArrayOfPars[i]->GetValue();

}



void TSOscProb::FillParameters(){

	for(int i=0 ; i<fNumOfParams; ++i){

		fParamCache[i] = fArrayOfPars[i]->GetValue();
	}

}



void TSOscProb::Init(){


	mArgBase = 1.267; //GeV/km
	mArgConst = mArgBase;
	mArgEScale=1;
	mArgLScale=1;

	fNumOfParams = 0;

	mE=0;
	mL=0;

	SetXbinningLog(false);

	fTransitionId = 0;
	fTranstionName ="";
	fTransitionLabel ="";

	fParamCache=0;

	fBindVarEne = 0;
	fBindVarLen = 0;

	fNptBinAve=100;

}



void TSOscProb::FillHistogram(TH1F &h, TString opt){


	bool bin_ave = opt.Contains("bin-average");
	bool bin_minus_sigma = opt.Contains("minus-sigma");
	bool bin_plus_sigma = opt.Contains("plus-sigma");

	if(bin_minus_sigma || bin_plus_sigma) bin_ave = true;

	bool bin_low = opt.Contains("bin-low-edge");
	bool bin_up = opt.Contains("bin-up-edge");


	int Nx = h.GetNbinsX();

	double E;

	if(!bin_ave){

		for(int ibx=1; ibx<=Nx; ++ibx){

			if(bin_low) E =  h.GetXaxis()->GetBinLowEdge(ibx);
			else if (bin_up) E =  h.GetXaxis()->GetBinUpEdge(ibx);
			else E = h.GetXaxis()->GetBinCenter(ibx);

			if(E==0) E=1e-5;

			SetE(E);

			double prob = Eval();

			h.SetBinContent(ibx,prob);

		}//

		return;
	}

	if(bin_ave){

		int npts=fNptBinAve;

		std::vector<double> sum;

		double E,Emin,Emax, Pave;

		for(int ibx=1; ibx<=Nx; ++ibx){

			Emin = h.GetXaxis()->GetBinLowEdge(ibx);
			Emax = h.GetXaxis()->GetBinUpEdge(ibx);

			Pave=0;

			sum.clear();

			for(int ie=0;ie<npts;++ie){

				E = Emin + (Emax-Emin)*ie/(npts-1);

				if(E==0) E=1e-5;

				SetE(E);

				double prob = Eval();

				Pave+=prob;

				sum.push_back(prob);

			}//ie


			Pave/=npts;

			double mean,rms;
			RootUtils::MeanAndRMS(&sum,0,mean,rms);

			if(bin_minus_sigma){
				mean -= rms;
				rms = 0;
			}
			if(bin_plus_sigma){
				mean += rms;
				rms=0;
			}

			h.SetBinContent(ibx,mean);
			h.SetBinError(ibx,rms);

			//cout<<Pave<<" "<<mean<<endl;

		}//loop x

	}//bin ave

}


void TSOscProb::OscillateHistogram(TH1 &h, TH1 *hOut, TString opt){



	/***
	 *
	 *
	 *  Axis binnings of h and hOut are not checked, are assumed to be consistent
	 *
	 */


	int hin_dim = h.GetDimension();

	bool use_hout = hOut ? true : false;

	int hout_dim = use_hout ? hOut->GetDimension() : 0;

	int axis = 0;

	bool bin_ave = false;

	if( !opt.IsWhitespace() ){

		if(opt.Contains("axis-y")){

			if( hin_dim !=2 ){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram is not 2D, option ",opt," not valid");
				return;
			}
			axis = 1;
		}

		if(opt.Contains("bin-average")) bin_ave = true;

	}



	bool projection = false;

	if(use_hout && hin_dim==2 && hout_dim==1){
		projection = true;
	}



	cout<<endl<<" -- "<<__FILE__<<"::"<<__FUNCTION__<<" histo: "<<h.GetName()<<" --- "<<endl;
	if(use_hout) cout<<" Apply Other histogram: "<<hOut->GetName()<<endl; else cout<<" apply to same histo"<<endl;
	cout<<" Axis with Energy "<<(axis==0? "x":"y")<<endl;
	if(projection) cout<<" Projection on the destination histogram is needed"<<endl;


	int Nx = h.GetNbinsX();
	int Ny = h.GetNbinsY();

	//
	// keep options separated in order to improve performance
	//


	if(projection){

		//oscillate y then sum up on x in hout
		if( axis == 1){

			cout<<"  ---> Options. Oscillate-y project on x-axis"<<endl;

			for(int ibx=1; ibx<=Nx; ++ibx){

				double cont_x = 0;

				for(int iby=1; iby<=Ny; ++iby){

					double cont = h.GetBinContent(ibx,iby);
					if(cont==0) continue;

					double E = h.GetYaxis()->GetBinCenter(iby);

					SetE(E);

					double prob = Eval();

					cont_x += cont*prob;

				}//loop y

				hOut->SetBinContent(ibx,cont_x);
			}//loop x

			return;
		}//axis-y

		//first proj on hout then oscillate hout
		if( axis==0 ){

			cout<<" ---> Options. Oscillate-x project on x-axis"<<endl;

			for(int ibx=1; ibx<=Nx; ++ibx){

				double cont_x = 0;

				for(int iby=1; iby<=Ny; ++iby){
					double cont = h.GetBinContent(ibx,iby);

					cont_x += cont;
				}//loop y

				if(cont_x==0) continue;

				double E = h.GetXaxis()->GetBinCenter(ibx);
				SetE(E);
				double prob = Eval();

				hOut->SetBinContent(ibx,cont_x*prob);

			}//loop x

			return;
		}//axis-x


	}//projection



	//energy variable is on the axis-x
	if( axis == 0 ){

		if( !use_hout ){

			if(hin_dim==1){

				cout<<" ---> Options. 1D, Oscillate-x, same histo"<<endl;

				for(int ibx=1; ibx<=Nx; ++ibx){

					double cont = h.GetBinContent(ibx);
					if(cont==0) continue;

					double E = h.GetXaxis()->GetBinCenter(ibx);

					SetE(E);

					double prob = Eval();

					h.SetBinContent(ibx,cont*prob);

				}//loop x

				return;
			}//dim-1


			if(hin_dim==2){

				cout<<" ---> Options. 2D, Oscillate-x, same histo"<<endl;

				for(int ibx=1; ibx<=Nx; ++ibx){

					double E = h.GetXaxis()->GetBinCenter(ibx);

					SetE(E);

					double prob = Eval();

					for(int iby=1; iby<=Ny; ++iby){

						double cont = h.GetBinContent(ibx,iby);
						if(cont==0) continue;

						h.SetBinContent(ibx,iby,cont*prob);

					}//loop y

				}//loop x

				return;
			}//dim-2



		}//not to hout



		if( use_hout ){

			if(hin_dim==1){

				cout<<" ---> Options. 1D, Oscillate-x, other histo"<<endl;

				for(int ibx=1; ibx<=Nx; ++ibx){

					double cont = h.GetBinContent(ibx);
					if(cont==0) continue;

					double E = h.GetXaxis()->GetBinCenter(ibx);

					SetE(E);

					double prob = Eval();

					hOut->SetBinContent(ibx,cont*prob);

				}//loop x

				return;
			}//dim-1


			if(hin_dim==2){

				cout<<" ---> Options. 2D, Oscillate-x, other histo"<<endl;

				for(int ibx=1; ibx<=Nx; ++ibx){

					double E = h.GetXaxis()->GetBinCenter(ibx);

					SetE(E);

					double prob = Eval();

					for(int iby=1; iby<=Ny; ++iby){

						double cont = h.GetBinContent(ibx,iby);
						if(cont==0) continue;

						hOut->SetBinContent(ibx,iby,cont*prob);
					}//loop y

				}//loop x

				return;
			}//dim-1



		}//to hout


	}//axis-x




	//energy variable is on the axis-y
	if( axis == 1 ){


		if( !use_hout ){

			cout<<" ---> Options. 2D, Oscillate-y, same histo"<<endl;

			for(int iby=1; iby<=Ny; ++iby){

				double E = h.GetYaxis()->GetBinCenter(iby);

				SetE(E);

				double prob = Eval();

				for(int ibx=1; ibx<=Nx; ++ibx){

					double cont = h.GetBinContent(ibx,iby);

					if(cont==0) continue;
					h.SetBinContent(ibx,iby,cont*prob);

				}//loop x

			}//loop y

			return;
		}//not to hout


		if( use_hout ){

			cout<<" ---> Options. 2D, Oscillate-y, other histo"<<endl;

			for(int iby=1; iby<=Ny; ++iby){

				double E = h.GetYaxis()->GetBinCenter(iby);

				SetE(E);

				double prob = Eval();

				for(int ibx=1; ibx<=Nx; ++ibx){

					double cont = h.GetBinContent(ibx,iby);
					if(cont==0) continue;

					hOut->SetBinContent(ibx,iby,cont*prob);

				}//loop x
			}//loop y

			return;
		}//to hout

	}//axis-y





}//

void TSOscProb::SetEnergyUnit(TString unit){


	if( unit== "GeV" ) return;

	if( unit == "MeV"){

		mArgEScale = 1e3;
		mArgConst = mArgBase*mArgEScale/mArgLScale;

		fVarEne.SetUnit(unit);

		return;
	}

	if( unit == "eV" ){

		mArgEScale = 1e6;
		mArgConst = mArgBase*mArgEScale/mArgLScale;


		fVarEne.SetUnit(unit);

		return;
	}


	MSG::ERROR(__FILE__,"::",__FUNCTION__,"Illegal Energy Unit input");

	return;
}


void TSOscProb::SetLengthUnit(TString unit){

	if( unit== "km" ) return;

	if( unit == "m"){


		mArgLScale = 1e3;
		mArgConst = mArgBase*mArgEScale/mArgLScale;

		fVarLen.SetUnit(unit);

		return;
	}


	MSG::ERROR(__FILE__,"::",__FUNCTION__,"Illegal Length Unit input");

	return;
}


void TSOscProb::SetParameters(const TSParamList& list){


	int N = list.GetSize();

	delete [] fParamCache;
	fParamCache = new double[N];

	fArrayOfPars.clear();

	fParamList.Clear();


	for(int i=0; i<N;++i){

		TSParameter *p = (TSParameter*)list.At(i);

		fArrayOfPars.push_back(p);

		fParamList.Add(p);

		fParamCache[i] = p->GetValue();
	}

	fNumOfParams = N;

}
