/*
 * TSBetheBlochFunc.cc
 *
 *  Created on: Jun 13, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include "StringUtils.h"

#include "RootUtils.h"
#include "TSBetheBlochFunc.h"
#include "TH1FAuto.h"
#include "TH2FAuto.h"

#include "MessageMgr.h"


TSBetheBlochFunc::TSBetheBlochFunc(){

	Init();
}

TSBetheBlochFunc::TSBetheBlochFunc(TString name, TString title,double (*fcn)(double*,double*), double xmin, double xmax, int npar)
:TF1(name,fcn,xmin,xmax,npar)
{

	SetTitle(title);
	Init();

}


TSBetheBlochFunc::TSBetheBlochFunc(TString name, TString title, TString file_name)
:TF1()
,fTable(file_name)
{

	// Build Has Tabular (TGraph)

	SetName(name);
	SetTitle(title);

    Init();

    fHasTable = true;
    fUseTable = true;
    fHasFunc = false;
    fUseFunc = false;

}



TSBetheBlochFunc::~TSBetheBlochFunc() {
	// TODO Auto-generated destructor stub

}



TGraph* TSBetheBlochFunc::BuildGraphFromFunction(int N,double xmin,double xmax) const{


	TGraph *g=new TGraph();

	TString gname=STR_UTILS::Name("g",GetName());
	TString gtitle=STR_UTILS::TitleXYZ(GetTitle(),GetAxisTitleX(),GetAxisTitleY());

	g->SetName(gname);
	g->SetTitle(gtitle);

	if(N<=2){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Num of points: ",N);
		return g;
	}

	if( !HasFunction() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No function found for: ",GetName());
		return g;
	}

	double min_x,max_x;

	if( xmin==0 && xmin==xmax){
		min_x=GetXmin();
		max_x=GetXmax();
	}else{
		min_x=xmin;
		max_x=xmax;
	}

	//cout<<min_x<<" "<<max_x<<endl;

	for(int ip=0; ip<N;++ip){

		double x=min_x+(max_x-min_x)*ip/(N-1);

		double bb=Eval(x);

		g->SetPoint(ip,x,bb);
	}


	return g;



}


void TSBetheBlochFunc::BuildTableFromFunction(int npts, int opt){

	/**
	 *
	 *
	 *  Create a TGraph of npts points calculated from
	 *  this function
	 *  and within the function range.
	 *
	 *
	 */


	if( npts < 2){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," illegal num of points. BBFunc: ",GetName());
		return;
	}

	if( !HasFunction() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No Function is Set for ",GetName());
		return;
	}

	TGraph g;

	double xmin = GetXmin();
	double xmax = GetXmax();

	for(int ip=0; ip<npts; ++ip){


		double x= xmin + (xmax-xmin)*ip/(npts-1);

		double y = TF1::Eval(x);

		g.SetPoint(ip,x,y);

	}


	ImportTable(g);

}



void TSBetheBlochFunc::Clear() {

}



TGraph* TSBetheBlochFunc::CloneTable(TString name) const{

	if( !HasTable() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Table not set");
	}

	return (TGraph*)fTable.Clone(name);

}


void TSBetheBlochFunc::ComputeMoments(std::vector<double> &ptot_data, double mass,double &mean,double &rms) const{


	std::vector<double> vec_val;

	for(int i=0;i<ptot_data.size();++i){

		vec_val.push_back( Eval( ptot_data.at(i), mass) );

		//if(i==0) cout<<"PD "<<Eval( ptot_data.at(i), mass)<<endl;

	}

	RootUtils::MeanAndRMS(&vec_val,0,mean,rms);



}



void TSBetheBlochFunc::ComputeMoments(TH1F &h_ptot, double mass,double &mean,double &rms) const{


	mean=0;
	rms=0;


	std::vector<double> vec_val;
	std::vector<double> vec_w;


	for(int i=1;i<=h_ptot.GetNbinsX();++i){

		double w=h_ptot.GetBinContent(i);

		if(w==0) continue;


		double bb=Eval(h_ptot.GetBinCenter(i),mass);

		//if(w>0) cout<<"PD "<<i<<" "<<w<<" "<<bb<<endl;

		vec_val.push_back(bb);
		vec_w.push_back(w);

	}



	RootUtils::MeanAndRMS(&vec_val,&vec_w,mean,rms);

}



Double_t TSBetheBlochFunc::Eval(Double_t momentum, Double_t mass, Double_t z, Double_t t) const{

/**
 *
 *  Overrides TF1 Method
 *
 *
 *
 */

 double beta_gamma = momentum;

 if( mass > 0){

	  beta_gamma = momentum/mass;
  }


 if( fUseFunc ){

	   return fEvalFunction(beta_gamma);

 }else if( fUseTable ){

		return fEvalTabular(beta_gamma);
 }


 return 0;

}


bool TSBetheBlochFunc::ImportGraph(const TGraph& g){


	//Clear existing
	if( fTable.GetN() != 0 ){
		fTable.Clear();
	}


	double x,y;

	for(int ip=0; ip<g.GetN(); ++ip){

		g.GetPoint(ip,x,y);

		fTable.SetPoint(ip,x,y);

	}


	fHasTable = true;

	return 1;
}



bool TSBetheBlochFunc::ImportTable(TString file_name){


	TString file = file_name;

	//Use TGraph constructor;

	TGraph g(file);

	if( g.IsZombie() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Failed to load table");
		return 0;
	}


	return ImportGraph(g);

}


void TSBetheBlochFunc::Init() {

	fHasFunc=true;
    fHasTable=false;

    fUseFunc=true;
    fUseTable=false;

    fInterpType=2;
    fInterpOpt="S";

    fAxisTitleX = "#beta#gamma";
    fAxisTitleY = "Bethe-Bloch ";


    TString fTableName = "g";
    fTableName+=GetName();

    fTable.SetName( fTableName );

    fSetTitles();

}



void TSBetheBlochFunc::SetInterpolation(int opt){


	if(opt==0){

		fInterpType = opt;
		fInterpOpt = "";

	}else if( opt==1 ){

		fInterpType = opt;
		fInterpOpt = "S";

	}else{

		SetInterpolation(0);
	}



}



void TSBetheBlochFunc::SetParNames(TString p0, TString p1,TString p2,TString p3,TString p4, TString p5){

	SetParName(0,p0);

	if( !p1.IsWhitespace() ) SetParName(1,p1);
	if( !p2.IsWhitespace() ) SetParName(2,p2);
	if( !p3.IsWhitespace() ) SetParName(3,p3);
	if( !p4.IsWhitespace() ) SetParName(4,p4);
	if( !p5.IsWhitespace() ) SetParName(5,p5);
}



void TSBetheBlochFunc::UseFunction(bool use){


	if( !use ){

		//Diable function - switch to table
		if( !HasTable() ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," Cannot switch to table: Tabular form not provided");

		}else{

			fUseFunc = false;
			fUseTable = true;
		}

	}else{

		fUseFunc = true;
		fUseTable = false;

	}



}


void TSBetheBlochFunc::UseTable(bool use){


	if( use ){

		if( HasTable() ){

			fUseTable = true;
			fUseFunc = false;

		}else{

			MSG::ERROR(__FILE__,"::",__FUNCTION__," Table Not Set");

			fUseTable = false;

		}

	}else{

		// disable table - switch to function

		if( !HasFunction() ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," Disable Table. Cannot Switch to Function: Func. not set");

			fUseFunc = false;
			fUseTable = false;

		}else{

			fUseFunc = true;
			fUseTable = false;

		}
	}


}


double TSBetheBlochFunc::fEvalFunction(double beta_gamma, int opt) const {

	return TF1::Eval(beta_gamma);

}


double TSBetheBlochFunc::fEvalTabular(double beta_gamma, int opt) const {


	return fTable.Eval(beta_gamma, 0, fInterpOpt);

}


void TSBetheBlochFunc::fSetTitles(){


/**
 *  Set Title and Axis Titles using
 *  SetTitle ensure the setting is kept
 *  even if the Object has not been Drawn yet
 *  (i.e.: no axis has been created yet)
 *
 */


	TString tit = STR_UTILS::TitleXYZ(GetTitle(),GetAxisTitleX(),GetAxisTitleY());

	// Function
	TF1::SetTitle(tit);

	for(int ip=0; ip<GetNpar(); ++ip){

		TString par_name=Form("BB_{%i}",ip);

		TF1::SetParName(ip,par_name);

	}

	// Table
	fTable.SetTitle(tit);


	//TF1::GetXaxis()->SetTitle( fAxisTitleX );
	//TF1::GetYaxis()->SetTitle( fAxisTitleY );

	//fTable.GetXaxis()->SetTitle( fAxisTitleX );
	//fTable.GetYaxis()->SetTitle( fAxisTitleY );


}
