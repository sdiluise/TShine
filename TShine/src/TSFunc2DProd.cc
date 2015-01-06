/*
 * TSFunc2DProd.cc
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <cassert>

#include <TMath.h>

#include "MessageMgr.h"

#include "TSFunc2DProd.h"


TSFunc2DProd::TSFunc2DProd(TString name,TString title, TSFunction1D &funcX, TSFunction1D &funcY)
:TSFunction2D(name,title,*funcX.GetVariableX(),*funcY.GetVariableX())
{
	fFuncX= &funcX;
	fFuncY= &funcY;

	int NP=funcX.NPar() + funcY.NPar();

	SetNPar(NP);

	for(int i=0; i< funcX.NPar(); i++){
		AddParameter(*funcX.GetParameter(i));
	}

	for(int i=0; i< funcY.NPar(); i++){

		AddParameter(*funcY.GetParameter(i));
	}

}



TSFunc2DProd::~TSFunc2DProd() {


}




double TSFunc2DProd::Evaluate() const{

	if( !fFuncX || !fFuncY ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__);
		assert(0);
	}

	double xx=fGetCachedX();
	double yy=fGetCachedY();

	double val_x = fFuncX->Eval(xx);

	double val_y = fFuncY->Eval(yy);

	if( TMath::IsNaN(val_x) && TMath::IsNaN(val_y)){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Eval Is NaN ", GetName());
		cout<<" values: "<<val_x<<"  "<<val_y<<" at: "<<xx<<" "<<yy<<endl;
	}

	return val_x*val_y;


}



