/*
 * TSFuncModel1D.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "TSFuncModel1D.h"


TSFuncModel1D::TSFuncModel1D(TString name,TString title)
:TSFuncModel(name,title)
{

	SetNDim(1);

}

TSFuncModel1D::TSFuncModel1D(TString name,TString title,const TSArgList& set, const TSArgSet &coefs)
:TSFuncModel(name,title,set,coefs)
{

	SetNDim(1);
	fCheckFunctions();

}

TSFuncModel1D::TSFuncModel1D(TString name,TString title,const TSArgList& set)
:TSFuncModel(name,title,set)
{

	SetNDim(1);
	fCheckFunctions();

}

TSFuncModel1D::TSFuncModel1D(TString name,TString title,TString formula,const TSArgList& set, const TSArgSet &coefs)
:TSFuncModel(name,title,formula,set,coefs)
{

	SetNDim(1);
	fCheckFunctions();

}

TSFuncModel1D::TSFuncModel1D(TString name,TString title,TString formula,const TSArgList& set)
:TSFuncModel(name,title,formula,set)
{

	SetNDim(1);
	fCheckFunctions();

}


TSFuncModel1D::~TSFuncModel1D() {

}



int TSFuncModel1D::fCheckFunctions(){


	int NFS=GetNumOfFuncs();


	int DIM=NDim();

	int NPAR=0;

	TSVariable *vx=0;

	for( int i=0; i<NFS; ++i ){

		TSFunction1D *f = dynamic_cast<TSFunction1D*>(fFunctionsList[i]);

		if(f->NDim() !=DIM ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," func ",f->GetName()," is not 1D");
			assert(0);
		}


		if(i==0)
			vx = (TSVariable*)f->GetVariableX();

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


		NPAR+=f->NPar();
	}



	SetNPar(NPAR);

	SetVarX(*vx);

	//cout<<"fCheckFunctions "<<this->GetName()<<" var x: "<<this->GetVariableX()->Name()<<" tot func par: "<<NPar()<<endl;


	NPAR+=GetNumOfCoefs();
	SetNPar(NPAR);
	//cout<<"Tot model pars: "<<NPar()<<endl;


	return 1;

}


double TSFuncModel1D::Eval() const{

	fCacheX( GetVariableX()->GetValue() );

	return Evaluate();
}



double TSFuncModel1D::Eval(double x, double y) const{


	fCacheX(x);

	return Evaluate();
}


double TSFuncModel1D::Evaluate() const{


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

	double norm = GetNormalization();

	return norm*val;


}


double TSFuncModel1D::fEvaluateTFormula() const{


	double xx=fGetCachedX();

	int NFS=GetNumOfFuncs();

	int NCS=GetNumOfCoefs();

	// SetParameter id non-const so the obj
	// needs a re-cast
	//just for testing, define fFormula as ptr
	// then recst it as TFormula
	// remove delete at the end
	TFormula *TFr = new TFormula(fFormula);


	for( int i=0; i<NFS; ++i ){

		double f=fFunctionsList[i]->Eval(xx);

		int ipar=func2ParamIndex[i];

		//fFormula.SetParameter(ipar,f);
		TFr->SetParameter(ipar,f);
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

	delete TFr;//remove this

	return val;

}


double TSFuncModel1D::fEvaluateLinear() const{


	double val = 0;

	double xx=fGetCachedX();

	int NFS=GetNumOfFuncs();

	for( int i=0; i<NFS; ++i ){

		double c=1;
		if( HasCoefs() ) c=fCoefsList[i]->GetValue();

		double f=fFunctionsList[i]->Eval(xx);

		val += c*f;

	}


	return val;


}
