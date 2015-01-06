/*
 * TSFuncModel2D.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>
#include <TMath.h>

#include "TSFuncModel2D.h"


TSFuncModel2D::TSFuncModel2D(TString name,TString title)
:TSFuncModel(name,title)
{

	Init();

	SetNDim(2);


}

TSFuncModel2D::TSFuncModel2D(TString name,TString title,const TSArgList& set, const TSArgSet &coefs)
:TSFuncModel(name,title,set,coefs)
{

	Init();

	SetNDim(2);
	fCheckFunctions();

}

TSFuncModel2D::TSFuncModel2D(TString name,TString title,const TSArgList& set)
:TSFuncModel(name,title,set)
{

	Init();

	SetNDim(2);
	fCheckFunctions();

}

TSFuncModel2D::TSFuncModel2D(TString name,TString title,TString formula,const TSArgList& set, const TSArgSet &coefs)
:TSFuncModel(name,title,formula,set,coefs)
{

	Init();

	SetNDim(2);
	fCheckFunctions();

}


TSFuncModel2D::TSFuncModel2D(TString name,TString title,TString formula,const TSArgList& set)
:TSFuncModel(name,title,formula,set)
{

	Init();

	SetNDim(2);
	fCheckFunctions();

}

TSFuncModel2D::~TSFuncModel2D() {

}



void TSFuncModel2D::Init() {

	f2Cache=0;

}


int TSFuncModel2D::fCheckFunctions(){


	int NFS=GetNumOfFuncs();


	int DIM=NDim();


	int NPAR=0;

	TSVariable *vx=0;
	TSVariable *vy=0;


	for( int i=0; i<NFS; ++i ){

		TSFunction2D *f = dynamic_cast<TSFunction2D*>(fFunctionsList[i]);


		if(!f){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," func number ",i," is not 2D");
			assert(0);
		}


		if(f->NDim() !=DIM ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," func ",f->GetName()," is not 1D");
			assert(0);
		}


		if(i==0){
			vx = (TSVariable*)f->GetVariableX();
			vy = (TSVariable*)f->GetVariableY();
		}

		if (!vx ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," function ",f->GetName()," has null X variable ptr");
			assert(0);
		}

		else{

			if( f->GetVariableX() != vx){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," functions have different X variable");
				assert(0);
			}

		}



		if (!vy ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," function ",f->GetName()," has null Y variable ptr");
			assert(0);
		}

		else{

			if( f->GetVariableY() != vy){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," functions have different y variable");
				assert(0);
			}

		}



		NPAR+=f->NPar();
	}


	SetNPar(NPAR);


	if( vx == vy){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," X and Y variables are the equal: ", vx->Name());
		assert(0);
	}


	SetVarX(*vx);
	SetVarY(*vy);

	cout<<"fCheckFunctions: "<<this->GetName()<<" var x: "<<this->GetVariableX()->Name()
											<<" var y: "<<this->GetVariableY()->Name()
											<<" tot num of funcs par: "<<NPar()<<endl;

	NPAR+=GetNumOfCoefs();
	SetNPar(NPAR);
	cout<<"Tot model pars: "<<NPar()<<endl;


	return 1;

}


double TSFuncModel2D::Eval() const{

	fCacheX( GetVariableX()->GetValue() );
	fCacheY( GetVariableY()->GetValue() );


	return Evaluate();
}



double TSFuncModel2D::Eval(double x, double y) const{


	fCacheX(x);
	fCacheY(y);

	return Evaluate();
}


double TSFuncModel2D::Evaluate() const{


	double val=0;

	if( IsLinear() ){

		val= fEvaluateLinear();
	}
	else if( IsTFormula() ){

		val= fEvaluateTFormula();

	}else{


		MSG::ERROR(__FILE__,"::",__FUNCTION__);
		return 0;
	}

	double norm=GetNormalization();

	return norm*val;

}


double TSFuncModel2D::fEvaluateTFormula() const{


	double xx=fGetCachedX();
	double yy=fGetCachedY();


	int NFS=GetNumOfFuncs();

	int NCS=GetNumOfCoefs();

	// SetParameter is non-const so the obj
	// needs a re-cast
	// now fFormula is copied to a local
	// TFormula object
	// todo: define fFormula as ptr
	// then recast it as TFormula

	if( !f2Cache ){
		f2Cache=new TFormula(fFormula);
	}

	TFormula *TFr = f2Cache;

	TSFunction2D *f=0;

	for( int i=0; i<NFS; ++i ){

		f= dynamic_cast<TSFunction2D*>(fFunctionsList[i]);

		double fval= f->Eval(xx,yy);

		int ipar=func2ParamIndex[i];

		//fFormula.SetParameter(ipar,f);
		TFr->SetParameter(ipar,fval);
	}


	for( int i=0; i<NCS; ++i ){

		double c=fCoefsList[i]->GetValue();

		int ipar=coef2ParamIndex[i];

		//fFormula.SetParameter(ipar,c);
		TFr->SetParameter(ipar,c);

	}


	//fFormula.Print();
	//TFr->Print();

	double val = TFr->Eval(0);

	//cout<<"Val x "<<xx<<" val "<<val<<endl;


	return val;

}

double TSFuncModel2D::fEvaluateLinear() const{


	double val = 0;

	double xx=fGetCachedX();
	double yy=fGetCachedY();

	TSFunction2D *f=0;

	int NFS=GetNumOfFuncs();

	for( int i=0; i<NFS; ++i ){

		f= dynamic_cast<TSFunction2D*>(fFunctionsList[i]);

		double c=1;
		if( HasCoefs() ) c=fCoefsList[i]->GetValue();

		if( TMath::IsNaN(c) ){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," Coefficient Eval IsNaN: ",xx," ",yy);
			MSG::WARNING(" coeffiecient: ",fCoefsList[i]->GetName());
		}

		double fval=f->Eval(xx,yy);

		if( TMath::IsNaN(fval) ){
			MSG::WARNING(__FILE__,"::",__FUNCTION__," Func Eval IsNaN: ",xx," ",yy);
			MSG::WARNING(" function: ",f->GetName());
		}

		val += c*fval;

	}


	return val;


}
