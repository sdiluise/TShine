/*
 * TSFunc1DModel.cc
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */
#include <cassert>

#include <limits>
#include <vector>

#include <TList.h>
#include <TObject.h>

#include <TF1.h>
#include <TF2.h>
#include <TMath.h>

#include "RootUtils.h"

#include "StringUtils.h"
#include "TSFunc1DGaussPdf.h"
#include "TSFuncModel.h"


TSFuncModel::TSFuncModel(TString name,TString title)
:TSFunction2D(name,title)
,fIsLinear(0),fIsTFormula(0)
{

	Init();

}


TSFuncModel::TSFuncModel(TString name,TString title, TString formula, const TSArgList& funcs, const TSArgSet &coefs)
:TSFunction2D(name,title)
,fIsLinear(0),fIsTFormula(0)
{
	Init();

	funcs.Print();
	coefs.Print();

	fIsTFormula = fAnalyseModelFormula(formula,funcs,coefs);


	fIsTFormula = fAddFunctions(funcs);

	fIsTFormula = fAddCoefficients(coefs);


	//last check
	if( fIsTFormula ){

		//cout<<fFormula.GetNpar()<<" "<<fNFuncs<<" "<<fNCoefs<<endl;

		if(fFormula.GetNpar() != fNFuncs+fNCoefs){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," tot number of functions and coefficient different from TFormula params");
			MSG::ERROR("TFormula pars: ",fFormula.GetNpar()," Num of funcs: ",fNFuncs," Num of coefs: ",fNCoefs);
			assert(0);
		}

	}


}



TSFuncModel::TSFuncModel(TString name,TString title, TString formula, const TSArgList& funcs)
:TSFunction2D(name,title)
,fIsLinear(0),fIsTFormula(0)
{

	Init();


	funcs.Print();

	fIsTFormula = fAnalyseModelFormula(formula,funcs,TSArgList());

	fIsTFormula = fAddFunctions(funcs);


	//last check
	if( fIsTFormula ){

		//cout<<fFormula.GetNpar()<<" "<<fNFuncs<<" "<<fNCoefs<<endl;

		if(fFormula.GetNpar() != fNFuncs+fNCoefs){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," tot number of functions and coefficient different from TFormula params ");
			MSG::ERROR("TFormula pars: ",fFormula.GetNpar()," Num of funcs: ",fNFuncs," Num of coefs: ",fNCoefs);
			assert(0);
		}

	}

}


TSFuncModel::TSFuncModel(TString name,TString title, const TSArgList& funcs, const TSArgSet &coefs)
:TSFunction2D(name,title)
,fIsTFormula(0),fIsLinear(0)
{

	/**
	 *   MODEL:
	 *   a Linear combination of Functions and Coefficient is assumed
	 *   in the same order they appear in the ArgSet lists
	 *
	 *   C1*F1 + C2*F2 + .....
	 *
	 */

	Init();

	funcs.Print();
	coefs.Print();

	if( funcs.GetSize() == coefs.GetSize() ){
		fIsLinear = true;
	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__,
				" different number of functions and coefficient in Linear Model constructor");
		assert(0);
	}

	fIsLinear = fAddFunctions(funcs);

	fIsLinear = fAddCoefficients(coefs);

}



TSFuncModel::TSFuncModel(TString name,TString title, const TSArgList& funcs)
:TSFunction2D(name,title)
,fIsTFormula(0),fIsLinear(0)
{

	/**
	 *
	 * As above without coefficients
	 *
	 */

	Init();

	funcs.Print();


	fIsLinear = fAddFunctions(funcs);



}


TSFuncModel::~TSFuncModel() {


}



void TSFuncModel::Init(){

	//MSG::LOG(__FILE__,"::",__FUNCTION__);

	fNFuncs=0;
	fNCoefs=0;
	fHasCoefs=false;

	SetNormalization(1,1,1);

}


void TSFuncModel::Print() const {


	MSG::LOG("====== ",__FUNCTION__," ====== ");

	cout<<" Model: "<<GetName()<<" "<<GetTitle()<<endl;
	cout<<"  Dimension: "<<NDim()<<endl;
	cout<<"  Normalization: "<<GetNormalization()<<" bin widths: "<<GetBinWidthX()<<" "<<GetBinWidthY()<<endl;
	cout<<"  Variables: "<<GetVariableX()->GetName();
	if( NDim()==2 ) cout<<" "<<GetVariableY()->GetName();
	cout<<endl;
	cout<<"   Num. of Functions: "<<GetNumOfFuncs()<<" Num. of Coefs: "<<GetNumOfCoefs()<<endl;

	cout<<"   Formula: ";
	if( IsTFormula() ) cout<<GetFormula()<<"; TF1 expr:  "<<GetTF1Formula()<<endl;
	else cout<<" linear "<<endl;

	for(int i=0; i<GetNumOfFuncs();++i){
		cout<<"  Func: "<<i<<" "<<fFunctionsList[i]->GetName()<<endl;
	}
	for(int i=0; i<GetNumOfCoefs();++i){
		cout<<"  Coef: "<<i<<" "<<fCoefsList[i]->GetName()<<": "<<fCoefsList[i]->GetValue()<<endl;
	}


	TSArgList *plist = GetPrimaryParamList();
	if(plist){
		cout<<" Num of Primary Params: "<<plist->GetSize()<<endl;
		for(int i=0;i<plist->GetSize();++i){
			TSParameter* par= (TSParameter*)plist->At(i);
			par->Print();
		}

	}else{
		MSG::WARNING("No Primary Param List found");
	}
	if( IsTFormula() ){
		cout<<"  TFormula: "<<endl;
		fFormula.Print();
	}

	cout<<" =============================== "<<endl;



}


void TSFuncModel::NormalizeToIntegral(const TH1 &h) const{

	/**
	 *
	 *  Scale Model by the bin_width area, binx or binx*biny,
	 *  of the 1D/2D histogram h.
	 *
	 *  If the TSFuncModel is fit to the histogram h
	 *  then the coefficient of the different Pdf,
	 *  at least in the linear case, correspond to
	 *  the yields of the individual Pdf and the
	 *  sum of yields to the total integral of h
	 *
	 */


	SetBinWidths(h);

	SetNormalization(1,GetBinWidthX(),GetBinWidthY());

}


void TSFuncModel::NormalizeToHistogram(const TH1 &h) const{

	/**
	 *
	 *
	 */

	double h_integ=0;


	if(h.GetDimension()==1 ){
		h_integ= ((TH1F*)&h)->Integral(1,h.GetNbinsX());
	}

	if(h.GetDimension()==2 ){
		h_integ= ((TH2F*)&h)->Integral(1,h.GetNbinsX(),1,h.GetNbinsY());
	}

	//


	if( RootUtils::IsZero(h_integ) ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," histogram has null integral: ",h.GetName());
		return;
	}

	double f_integ=0;

	if(NDim()==1) {
		f_integ = TSFunction1D::ComputeIntegral(h.GetXaxis()->GetXmin(),h.GetXaxis()->GetXmax());
	}
	else if(NDim()==2) {
		f_integ = TSFunction2D::ComputeIntegral(h.GetXaxis()->GetXmin(),h.GetXaxis()->GetXmax()
											,h.GetYaxis()->GetXmin(),h.GetYaxis()->GetXmax());
	}


	if( RootUtils::IsZero(f_integ) ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," function has null integral: ",GetName());
		return;
	}

	double scale=h_integ/f_integ;

	SetNormalization(1,1,1);

	SetBinWidths(h);


	//cout<<h_integ<<" "<<f_integ<<" "<<GetBinWidthX()<<" "<<GetBinWidthY()<<endl;

	SetNormalization(scale,GetBinWidthX(),GetBinWidthY());

	//cout<<"-----"<<endl;


}


void TSFuncModel::ResetNormalization() const{

	SetNormalization(1,1,1);

}

void TSFuncModel::SetNormalization(double integral, double bin_width_x, double bin_width_y) const{

	fNormalization = integral*bin_width_x*bin_width_y;

	SetBinWidths(bin_width_x,bin_width_y);


}


int TSFuncModel::fAddCoefficients(const TSArgSet &set){
/**
 *
 *
 *  Add list of coefficients to the Model
 *  Check consistency
 *
 *
 *
 */

	if(set.GetSize() == 0){
		MSG::ERROR(__FILE__"::",__FUNCTION__," list is empty");
	}

	if(set.GetSize() > NMAX_FORMULA_VARS){
			MSG::ERROR(__FILE__"::",__FUNCTION__
			," too many variables in the expression. Do increase max allowed size: ",NMAX_FORMULA_VARS);
	}


	for(int i=0; i<set.GetSize();++i){

		TObject *obj = (TObject*)set.At(i);

		TSParameter *param = dynamic_cast<TSParameter*>(obj);

		if( !param ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," ",obj->GetName()," not a TSParameter object");
			assert(0);
		}


		fCoefsList[i]=param;


		AddParameter( *param );

	}

	fNCoefs = set.GetSize();

	fHasCoefs=true;

	return 1;

}



int TSFuncModel::fAddFunctions(const TSArgSet &set){
/**
 *
 *
 *  Add list of functions to the Model
 *  Check consistency
 *
 *
 *
 */

	if(set.GetSize() ==0 ){
		MSG::ERROR(__FILE__"::",__FUNCTION__, " list is empty");
		assert(0);
	}

	if(set.GetSize() > NMAX_FORMULA_VARS){
		MSG::ERROR(__FILE__"::",__FUNCTION__
				," too many variables in the expression. Do increase max allowed size: ",NMAX_FORMULA_VARS);
	}


	TSVariable *vx=0;


	for(int i=0; i<set.GetSize();++i){

		TObject *obj = (TObject*)set.At(i);

		TSFunction1D *f=dynamic_cast<TSFunction1D*>(obj);

		if( !f ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," ",obj->GetName()," not a TSFunction object");
			assert(0);
		}


		/*
		if( f->NDim() !=DIM ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," ",obj->GetName()," not a 1-DIM function");
			assert(0);
		}


		if(i==0)
			vx = (TSVariable*)f->GetVariableX();

			if (!vx ){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," function ",obj->GetName()," has null X variable ptr");
				assert(0);
			}

		else{

			if( f->GetVariableX() != vx){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," functions have different X variable");
				assert(0);
			}

		}

		 */
		cout<<" Adding Function: "<<f->GetName()<<" x variable: "<<f->GetVariableX()->Name()<<endl;

		fFunctionsList[i] = f;



		//primary param list

		TSArgList *list=f->GetPrimaryParamList();
		if(list){

			for(int i=0;i<list->GetSize();++i){
				AddParameter(  *(TSParameter*)list->At(i) );
			}
		}
	}//


	fNFuncs = set.GetSize();

	//SetVarX(*vx);



	//cout<<this->GetVariableX()->Name()<<endl;

	return 1;
}






int TSFuncModel::fAnalyseModelFormula(TString expr,const TSArgSet &funcs, const TSArgSet &coefs){


	/**
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 */

	MSG::LOG(__FILE__"::",__FUNCTION__);

	fExpr = expr;

	int NFuncs = funcs.GetSize();
	int NCoefs = coefs.GetSize();

	if( (NFuncs || NCoefs)>NMAX_FORMULA_VARS ){
		MSG::ERROR(__FILE__"::",__FUNCTION__
					," too many variables in the expression. Do increase max allowed size: ",NMAX_FORMULA_VARS);
	}

	if(NFuncs == 0){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," empty function list");
		assert(0);
	}


	// Order functions and coefficient name in order
	// to replace first the one which is longer
	// In such a way it should be avoided that
	// a variable name which contains another variable
	// name it is partially substituted with the param
	// number which is relative to of the  var with the
	// short
	// i.e.: to avoid this
	// var1+var1_2 --> [0]+[0]_2



	std::vector<TString> list_func_names;

	std::vector<TString> list_coef_names;

	for(int i=0; i<NFuncs; ++i){

		TObject *o= (TObject*)funcs.At(i);

		TString func_name= TString(o->GetName() );

		list_func_names.push_back(func_name);

	}

	std::sort(list_func_names.begin(),list_func_names.end(),STR_UTILS::CompLengthDecr);

	for(int i=0; i<NCoefs; ++i){

		TObject *o= (TObject*)coefs.At(i);

		TString coef_name= TString(o->GetName() );

		list_coef_names.push_back(coef_name);

	}


	std::sort(list_coef_names.begin(),list_coef_names.end(),STR_UTILS::CompLengthDecr );


	std::vector<TString> list_names;

	for(int i=0; i<list_func_names.size();++i){
		//cout<<"function: "<<i<<" "<<list_func_names.at(i)<<endl;
		list_names.push_back(list_func_names.at(i));
	}

	for(int i=0; i<list_coef_names.size();++i){
		//cout<<"coeff: "<<i<<" "<<list_coef_names.at(i)<<endl;
		list_names.push_back(list_coef_names.at(i));
	}


	std::sort(list_names.begin(),list_names.end(), STR_UTILS::CompLengthDecr );

	//check double occourencies
	for(int i=0; i<list_names.size();++i){

		if( list_names[i]==list_names[i+1] ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Two Element with the same name: ",list_names[i]);
			assert(0);
		}

	}


	//-----


	std::map<TString,bool> isFunc;
	std::map<TString,bool> isCoef;

	std::map<TString,int> parNumber;

	for(int i=0; i<list_func_names.size(); ++i){
		isFunc[ list_func_names[i] ] = true;
	}

	for(int i=0; i<list_coef_names.size(); ++i){
		isCoef[ list_coef_names[i] ] = true;
	}

	for(int i=0; i<list_names.size(); ++i){
		parNumber[ list_names[i] ] = -1;
	}

	//--

	//formula to be used by TFormula
	TString math_formula = expr;

	int NPAR=0;

	for(int i=0; i<list_names.size(); ++i){

		TString vname= list_names[i];

		if( expr.Contains(vname) ){

			//cout<<expr<<" contains: "<<vname<<endl;

			//translate var names in [0], [1] ... as it is the
			//TFormula format
			TString par_expr="["; par_expr+=NPAR; par_expr+="]";

			math_formula.ReplaceAll(vname,par_expr);

			if( isFunc.count(vname) ){

			}
			if( isCoef.count(vname) ){

			}

			parNumber[vname]=NPAR;

			NPAR++;

		}else{

			MSG::ERROR(__FILE__,"::",__FUNCTION__," ",expr," does not contain: ",vname);
			assert(0);

		}

	}



	cout<<" Model Expression: "<<expr<<endl;
	cout<<" TFormula format: "<<math_formula<<endl;

	//
	//add x to make compilable by TFormula
	//
	math_formula +="+x";

	cout<<"Model TFormula: "<<math_formula<<endl;

	fTF1Formula = math_formula;

	//
	// Static Method to set max num of params etc in the formula
	// def is 1000. Reducing it may improve performance
	//
	//TFormula::SetMaxima(NPAR*3,NPAR,NPAR*3);


	int flag = fFormula.Compile(math_formula);

    if(flag==0){

    	fIsTFormula = true;

    	for(int i=0; i<funcs.GetSize();++i){

    		TString name = funcs.At(i)->GetName();

    		int ipar = parNumber[name];

    		func2ParamIndex[i]=ipar;

    		cout<<"Function: "<<i<<" "<<name<<" at param: " <<ipar<<endl;

    		fFormula.SetParName(ipar,name);
    	}

    	for(int i=0; i<coefs.GetSize();++i){

    		TString name = coefs.At(i)->GetName();

    		int ipar = parNumber[name];

    		coef2ParamIndex[i]=ipar;

    		cout<<"Coefficient: "<<i<<" "<<name<<" at param: " <<ipar<<endl;

    		fFormula.SetParName(ipar,name);
    	}

    }else{

    	fIsTFormula = false;
    	MSG::ERROR(__FILE__,"::",__FUNCTION__," compilation failed for: ", math_formula," from ",expr);
    	MSG::ERROR("   TFormula::Compile error: ",flag);

    	assert(0);
    }

    fFormula.Print();


	return fIsTFormula;
}


