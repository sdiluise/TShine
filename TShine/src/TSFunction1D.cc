/*
 * TSFunction1D.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <TAxis.h>

#include "TSFunction1D.h"

TSFunction1D::TSFunction1D(TString name, TString title)
:TSBaseFunction(name,title){

	InitFuncs();

	SetNDim(1);
    fX=0;

}

TSFunction1D::TSFunction1D(TString name, TString title, const TSVariable &var_x)
:TSBaseFunction(name,title)
{


	InitFuncs();

	SetNDim(1);
	SetVarX(var_x);
}



TSFunction1D::~TSFunction1D() {

	if(fTF1) fTF1->Delete();

}


void TSFunction1D::InitFuncs(){

	fTF1=0;

}


const TSVariable* TSFunction1D::GetVariableX() const{


	if( !fX ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," null pointer");
		return 0;
	}

	return fX;
}



void TSFunction1D::SetVarX(const TSVariable &vx) {

	fX= &vx;

}



double TSFunction1D::Eval() const{


	fCacheX( GetVariableX()->GetValue() );

	return Evaluate();
}


double TSFunction1D::Eval(double x, double y) const{


	fCacheX(x);

	return Evaluate();

}


TGraph* TSFunction1D::BuildGraphX(int npts, double y) const{


	/**
	 *
	 *
	 * Use range of the X Variable
	 *
	 *
	 *
	 */



	double xmin,xmax;

	if( GetVariableX()->HasRange() ){
		xmin= GetVariableX()->GetMin();
		xmax= GetVariableX()->GetMax();
	}else{
		MSG::WARNING(__FILE__,"::",__FUNCTION__," var x: ",GetVariableX()->Name()," has no range specified");
		xmin=0;
		xmax=1;
	}

	return BuildGraphX(xmin,xmax,npts);

}


TGraph* TSFunction1D::BuildGraphX(double xmin, double xmax, int npts) const{

	/**
	 *
	 *
	 *
	 *
	 *
	 *
	 */


	TGraph* g=new TGraph();

	TString name = Name() + "_x";
	g->SetName( name);


	double dx=xmax-xmin;


	double x;

	for(int i=0;i<npts;++i){

		// npts points in [xmin,xmax]
		x= xmin;
		if(npts>1) x += dx*i/(npts-1);

		double y=Eval(x,y);

		g->SetPoint(i,x,y);
	}


	//Set Axis Title

	TString tit= Title();
	tit+=";"; tit+=GetVariableX()->GetAxisTitle();

	g->SetTitle( tit );


	return g;


}


TF1* TSFunction1D::BuildTF1(TString name,int npx) const {



	double xmin = GetVariableX()->GetMin();
	double xmax = GetVariableX()->GetMax();

	TSArgList *par_list=GetPrimaryParamList();

	int npar = par_list->GetSize();

	TString fname;

	if( name.IsWhitespace() ){
		fname="TF1_";
		fname+=GetName();
	}else{
		fname=name;
	}


	TF1 *f1 = new TF1(fname,this,&TSFunction1D::EvalFunc
			  	  ,xmin,xmax,npar,"TSFunction1D","EvalFunc");

	f1->SetNpx(npx);

	TString title= GetAxisTitles();

	f1->SetTitle(title);

	for(int i=0; i<npar;++i){

		TSParameter *param= (TSParameter*)(par_list->At(i));

		f1->SetParameter(i,param->GetValue());
		f1->SetParName(i,param->GetLabel());
	}

	return f1;

}


double TSFunction1D::ComputeIntegral() const {


	return ComputeIntegral(GetVariableX()->GetMin(),GetVariableX()->GetMax());

}


double TSFunction1D::ComputeIntegral(double xmin, double xmax) const {

	//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	TF1 *f = BuildTF1();

	double integ= f->Integral(xmin,xmax);

	delete f;

	return integ;

}


TString TSFunction1D::GetAxisTitles() const {

	TString title=GetTitle();

	title +=";";
	title +=GetVariableX()->GetAxisTitle();

	return title;
}
