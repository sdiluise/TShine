/*
 * TSFunction2D.cc
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <iostream>

#include "MessageMgr.h"

#include "TSFunction2D.h"


using std::cout;
using std::endl;

TSFunction2D::TSFunction2D(TString name, TString title)
:TSFunction1D(name,title)
{


	InitFuncs();

}

TSFunction2D::TSFunction2D(TString name, TString title,
		const TSVariable &var_x, const TSVariable &var_y)
:TSFunction1D(name,title,var_x)
{

	InitFuncs();

	SetNDim(2);
	SetVarY(var_y);

}


TSFunction2D::~TSFunction2D() {


	if(fTF2) fTF2->Delete();
}


void TSFunction2D::InitFuncs() {


	fTF2=0;

}


const TSVariable* TSFunction2D::GetVariableY() const{


	if( !fY ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," null pointer");
		return 0;
	}

	return fY;
}



void TSFunction2D::SetVarY(const TSVariable &vy) {

	fY= &vy;

}



double TSFunction2D::Eval() const{


	fCacheX( GetVariableX()->GetValue() );
	fCacheY( GetVariableY()->GetValue() );

	return Evaluate();

}


double TSFunction2D::Eval(double x, double y) const{

	fCacheX(x);
	fCacheY(y);

	return Evaluate();

}


TGraph2D* TSFunction2D::BuildGraph2D(int nptx, int npty) const{

	/**
	 *
	 *  Use range of the X,Y Variables
	 *
	 *
	 */

	double xmin,xmax;

	if( GetVariableX()->HasRange() ){
		xmin= GetVariableX()->GetMin();
		xmax= GetVariableX()->GetMax();
	}else{
		MSG::WARNING(__FILE__,"::",__FUNCTION__," var x: ",GetVariableY()->Name()," has no range specified");
		xmin=0;
		xmax=1;
	}

	double ymin,ymax;

	if( GetVariableY()->HasRange() ){
		ymin= GetVariableY()->GetMin();
		ymax= GetVariableY()->GetMax();
	}else{
		MSG::WARNING(__FILE__,"::",__FUNCTION__," var y: ",GetVariableY()->Name()," has no range specified");
		ymin=0;
		ymax=1;
	}


	return BuildGraph2D(xmin,  xmax, ymin,  ymax,nptx, npty);

}



TGraph2D* TSFunction2D::BuildGraph2D(double xmin, double xmax, double ymin, double ymax,int nptx, int npty) const
{
	/**
	 *
	 *
	 *
	 *
	 *
	 *
	 */


	TGraph2D* g=new TGraph2D();

	g->SetName( Name() );
	g->SetTitle( Title() );



	double dx = xmax-xmin;
	double dy = ymax-ymin;

	double x,y;

	int np=0;

	for(int ix=0;ix<nptx;++ix){

		// nptx points in [xmin,xmax]
		x= xmin;
		if(nptx>1) x += dx*ix/(nptx-1);

		for(int iy=0;iy<npty;++iy){

			// npty points in [ymin,ymax]
			y = ymin;
			if(npty>1) y += dy*iy/(npty-1);


			double z=Eval(x,y);

			g->SetPoint(np,x,y,z);
			np++;

		}//iy

	}//ix


	//Set Axis Titles

	TString tit = Title();

	g->Draw();

	g->SetTitle(tit);

	//
	//
	//
	g->GetHistogram("empty")->GetXaxis()->SetTitle( GetVariableX()->GetAxisTitle() );
	g->GetHistogram("empty")->GetYaxis()->SetTitle( GetVariableY()->GetAxisTitle() );


	//g->Draw();

	return g;


}






TF2* TSFunction2D::BuildTF2(TString name,int npx, int npy) const {



	double xmin = GetVariableX()->GetMin();
	double xmax = GetVariableX()->GetMax();
	double ymin = GetVariableY()->GetMin();
	double ymax = GetVariableY()->GetMax();

	TSArgList *par_list=GetPrimaryParamList();

	int npar = par_list->GetSize();


	TString fname;

	if( name.IsWhitespace() ){
	  fname="TF2_";
	  fname+=GetName();
	}else{
	  fname=name;
	}

	TF2 *f2 = new TF2(fname,this,&TSFunction2D::EvalFunc
			  	  ,xmin,xmax,ymin,ymax,npar,"TSFunction2D","EvalFunc");

	f2->SetNpx(npx);
	f2->SetNpy(npy);

	TString title= GetAxisTitles();

	f2->SetTitle(title);

	f2->Draw();

	f2->GetXaxis()->SetTitleOffset( 1.6*f2->GetXaxis()->GetTitleOffset() );
	f2->GetYaxis()->SetTitleOffset( 1.6*f2->GetYaxis()->GetTitleOffset() );

	f2->Draw();



	for(int i=0; i<npar;++i){

		TSParameter *param= (TSParameter*)(par_list->At(i));

		f2->SetParameter(i,param->GetValue());
		f2->SetParName(i,param->GetLabel());
	}

	return f2;

}


double TSFunction2D::ComputeIntegral() const {

	return ComputeIntegral(GetVariableX()->GetMin(),GetVariableX()->GetMax(),GetVariableY()->GetMin(),GetVariableY()->GetMax());
}

double TSFunction2D::ComputeIntegral(double xmin, double xmax, double ymin, double ymax) const {

	//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	TF2 *f = BuildTF2();

	double integ= f->Integral(xmin,xmax,ymin,ymax);

	delete f;

	return integ;

}


TString TSFunction2D::GetAxisTitles() const {

	TString title=GetTitle();

	title +=";";
	title +=GetVariableX()->GetAxisTitle();
	title +=";";
	title +=GetVariableY()->GetAxisTitle();

	return title;
}
