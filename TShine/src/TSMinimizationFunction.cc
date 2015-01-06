/*
 * TSMinimizationFunction.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#include <cassert>

#include <TMath.h>

#include "StringUtils.h"

#include "TSHistogramFld.h"

#include "TSParamList.h"

#include "TSMinimizationFunction.h"


TSMinimizationFunction::TSMinimizationFunction()
//:TSNamed("TSMinimFunc","TSMinimFunc")
:ROOT::Math::IMultiGenFunction()
{

	SetName("TSMinimFunc");
	SetTitle("TSMinimFunc");


	Init();
}

TSMinimizationFunction::TSMinimizationFunction(TString name,TString title)
//:TSNamed(name,title)
:ROOT::Math::IMultiGenFunction()
{

	SetName(name);
	SetTitle(title);

	Init();

}

TSMinimizationFunction::TSMinimizationFunction(const TSMinimizationFunction &) {


}


TSMinimizationFunction::~TSMinimizationFunction() {

	if(fHistory) delete fHistory;

}


void TSMinimizationFunction::Init(){



	MSG::LOG(__FILE__,"::",__FUNCTION__);

	fDataBin=0;
	fModel=0;
	fDataHisto=0;

	SetHasModel(false);
	SetHasData(false);

	SetNumOfDim(0);
	SetNumOfPar(0);

	fVariableX = 0;
	fVariableY = 0;

	ResetNumOfCalls();
	SaveHistory(false);

	fHistory=0;
	fInitHistoryTree();

	fIsScannedMinNorm=false;
	fScannedMinNorm=1;

}



void TSMinimizationFunction::SaveHistory(bool save){

	if( !save ){

		fSaveHistory=save;
		return;
	}


	fBuildHistoryTree();

	fSaveHistory=save;


}


int TSMinimizationFunction::SetBinnedData(TObject &histo_data){

	MSG::LOG("---- "," TSMinimizationFunction ",GetName()," ",GetTitle() );
	MSG::LOG(" ----- SetBinnedData: Histogram -----");

	TSHistogramFld hMgr;

	int hDim=0;

	if(hMgr.IsTH1(histo_data) ) hDim=1;
	else if(hMgr.IsTH2(histo_data)) hDim=2;

	if( !hDim ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid Histogram Type: not a TH1 or TH2");
		assert(0);
	}

	if( HasModel() ){


		if( hDim != NDim() ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Model Dimension and number of data variable are different");
			assert(0);
		}

	}

	bool data_set=true;

	fDataHisto = &histo_data;

	cout<<" Selected Data Histogram: "<<fDataHisto->GetName()<<endl;


	SetHasData(data_set);

	SetNumOfDataDim(hDim);



	return 1;
}


int TSMinimizationFunction::SetBinnedData(const TSDataBin &data_bin, const TSArgList &var_list){

	MSG::LOG("---- "," TSMinimizationFunction ",GetName()," ",GetTitle() );
	MSG::LOG(" ----- SetData -----");

	cout<<" ---> "<<data_bin.GetName()<<" "<<data_bin.GetTitle()<<endl;


	if( !(var_list.GetSize()==1 || var_list.GetSize()==2) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid dimension for Var List set");
	    assert(0);
	}


	if( HasModel() ){

		if( var_list.GetSize() != NDim() ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," Model Dimension and number of data variable are different");
		    assert(0);
		}

	}

	var_list.Print();

	bool data_set=false;

	fDataBin = &data_bin;

	if( !fDataBin ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not Valid Data Bin obj");
		assert(0);
	}


    if( !fDataBin->GetHistoFolder() ){
    	MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram Folder not found");
    	assert(0);
    }

    int dim= var_list.GetSize();

    TSHistogramFld *hFld= fDataBin->GetHistoFolder();

    TString varx_name= var_list.At(0)->GetName();

    TString vary_name="";
    if(dim==2) vary_name= var_list.At(1)->GetName();


    TObject *obj = hFld->GetTagHisto(varx_name,vary_name);

    if( !obj ){
    	if(dim==1)
    		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not found 1D Histogram for variable: ",varx_name);
    	else
    		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not found 2D Histogram for variables: ",varx_name,", ",vary_name);

    	assert(0);
    }

    if(dim==1 && ! hFld->IsTH1(*obj) ){
    	MSG::ERROR(__FILE__,"::",__FUNCTION__," object: ",obj->GetName()," Is Not a TH1");
    	assert(0);
    }

    if(dim==2 && ! hFld->IsTH2(*obj) ){
    	MSG::ERROR(__FILE__,"::",__FUNCTION__," object: ",obj->GetName()," Is Not a TH2");
    	assert(0);
    }


    fDataHisto=obj;

    cout<<" Selected Data Histogram: "<<fDataHisto->GetName()<<endl;


	SetHasData(data_set);

	SetNumOfDataDim(dim);

	return data_set;
}



int TSMinimizationFunction::SetModel(const TSFuncModel &model){

	MSG::LOG("---- "," TSMinimizationFunction ",GetName()," ",GetTitle() );
	MSG::LOG(" ----- SetModel -----");

	cout<<" ---> "<<model.GetName()<<" "<<model.GetTitle()<<endl;

	//model.Print();


	fModel = &model;


	bool model_set=false;

	model_set = fGetModelVariables();

	model_set = fGetModelParams();


	if( HasData() ){

		if( GetDataDim() != NDim() ){

			MSG::ERROR(" Dimension of Data and Model do not match ");
			assert(0);
		}
	}


	SetHasModel(model_set);

	MSG::LOG("---- "," TSMinimizationFunction "," SetModel: ",HasModel()," -----");

	return model_set;
}



void TSMinimizationFunction::fInitHistoryTree() {

	if( fHistory ) delete fHistory;


	TString name="TTree_"; name+=GetName();

	fHistory = new TTree(name,name);

}

void TSMinimizationFunction::fBuildHistoryTree() {


	if( !fHistory ){
		fInitHistoryTree();
	}

	if( fHistory->GetNbranches() !=0 ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," History Tree already build, deleting the old one");

		fInitHistoryTree();
	}

	fHistory->Branch("NCall",&fNCalls,"NCall/I");
	fHistory->Branch("Value",&fValue,"fValue/D");



	if( !HasModel() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Model Not set, History Tree cannot be filled with Model Parameters");
		return;
	}

	TSArgList *list = GetModel()->GetPrimaryParamList();

	if( !list ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," primary param list not found ");
		//assert(0);
	}

	int np=list->GetSize();

	if(np > NMAX_MINIMIZER_PARAMS){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," num of pars: ",np, " need to enlarge max cache size: ",NMAX_MINIMIZER_PARAMS);
		    assert( 0);
	}

	for(int ipar=0; ipar<list->GetSize(); ++ipar){

		TString name= list->At(ipar)->GetName();
		TString nameT = name+"/D";

		fHistory->Branch(name,&fCacheParamValue[ipar],nameT);

		name +="_IsFix";
		nameT = name+"/I";
		fHistory->Branch(name,&fCacheParamIsFix[ipar],nameT);

	}

}

bool TSMinimizationFunction::fGetModelParams() {


	if( !GetModel() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," model not set ");
		assert( GetModel() );
	}

	TSArgList *list = GetModel()->GetPrimaryParamList();

	if(! list ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," primary param list not found ");
	    assert( 0);
	}

	int np=list->GetSize();
	int npp=GetModel()->GetNumOfPrimPars();

	cout<<" Num of primary params: "<<np<<" "<<npp<<endl; //they must be equal

	if(np > NMAX_MINIMIZER_PARAMS){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," num of pars: ",np, " need to enlarge max cache size: ",NMAX_MINIMIZER_PARAMS);
	    assert( 0);
	}

	//
	// Params pointers and values list is cached for performance reason
	//
	SetNumOfPar(np);

	for(int i=0; i<np; ++i){

        fCacheParamList[i]  = dynamic_cast<TSParameter*>(list->At(i));
		fCacheParamValue[i] = fCacheParamList[i]->GetValue();
		fCacheParamMin[i] = fCacheParamList[i]->GetMin();
		fCacheParamMax[i] = fCacheParamList[i]->GetMax();
		fCacheParamStep[i] = fCacheParamList[i]->GetStep();
		fCacheParamIsFix[i] = fCacheParamList[i]->IsFixed();

	}


    return 1;

}



bool TSMinimizationFunction::fGetModelVariables() {


	if( !GetModel() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," model not set ");
		assert( GetModel() );
	}


	int nd= GetModel()->NDim();

	if( !(nd==1||nd==2) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," invalid num of dimensions: ",nd);
		assert( 0 );
	}


	SetNumOfDim(nd);


	if( GetModel()->GetVariableX() ){
		SetVariableX( *GetModel()->GetVariableX() );
	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," variable X not found");
		assert( 0 );
	}

	if( NDim()==2 ){

		if(GetModel()->GetVariableY() ){
			SetVariableY( *GetModel()->GetVariableY() );
		}else{
			MSG::ERROR(__FILE__,"::",__FUNCTION__," variable Y not found");
			assert( 0 );
		}
	}

	return 1;
}


void TSMinimizationFunction::SetVariableX(const TSVariable &v){

	fVariableX = &v;

}

void TSMinimizationFunction::SetVariableY(const TSVariable &v){

	fVariableY= &v;
}



double TSMinimizationFunction::EvalFunctor(const double *x) const {


	return DoEval(x);

}



inline double TSMinimizationFunction::DoEval (const double *x) const {

	fNCalls++;

	//
	// Cache params values
	//

	int NPAR = NPar();


	for(int i=0; i<NPAR; ++i){

		if( fCacheParamList[i]->HasRange() && !fCacheParamList[i]->IsInside(x[i]) ){

			//cout<<"##### "<<fCacheParamList[i]->GetName()<<" outrange "<<x[i]<<" "<<fCacheParamList[i]->GetMin()<<" "<<fCacheParamList[i]->GetMax()<<endl;

			//return 1e6;
		}


		if( TMath::IsNaN(x[i]) ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," attempt to use a NaN parameter value. Par. Num : ",i);
			MSG::ERROR(" Param Name: ",fCacheParamList[i]->GetName()," current value: ",fCacheParamValue[i]);
			continue;
			assert(0);

		}

		fCacheParamValue[i] = x[i];

		fCacheParamList[i]->SetValue(  x[i] );
	}


	//cout<<"Calling DoEval "<<fNCalls<<" "<<x[0]<<" "<<x[NPAR-1]<<endl;

	//
	// Evaluate Minimization Function with this set of params
	//

	fValue = Compute();

	//
	// save the history
	//
	if( SaveHistoryIsSet() ){

		if( GetHistoryTree() ){

			GetHistoryTree()->Fill();

		}else{

			MSG::WARNING(__FILE__,"::",__FUNCTION__," Attempt to fill a void History Tree");
		}
	}


	//cout<<"value "<<fValue<<endl;

	return fValue;

}


TGraph* TSMinimizationFunction::ScanParameter(TSParameter& par,int N, double min, double max){

	TGraph *g=new TGraph();

	if(N<2 || min>=max){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
		return g;
	}

	if( !GetModel() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," No Model found");
		return g;
	}


	TSParamList *plist=new TSParamList();

	TSArgList *alist= GetModel()->GetPrimaryParamList();

	plist->Copy(*alist);

	if(!plist){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
		return g;
	}


	cout<<" Scanning Param "<<par.GetName()<<endl;
	cout<<"Param List "<<endl;
	plist->PrintParams();

	int NPARS=plist->GetSize();


	bool par_range_set=par.HasRangeForced();
	double par_orig_value=par.GetValue();


	par.ForceToRange(false);



	double *x=new double[NPARS];

	double fmin=0;
	double par_min=0;


	for(int istep=0; istep<N; ++istep){

		double pval=min+(max-min)*istep/(N-1);

		par.SetValue(pval);

		for(int ip=0;ip<NPARS;++ip){

			x[ip]= plist->At(ip)->GetValue();

		}

		double func=DoEval(x);

		g->SetPoint(istep,pval,func);

		if(istep==0){
			fmin=func;
			par_min=pval;
		}else{
			if(func<fmin) {
				fmin=func;
				par_min=pval;
			}
		}

	}//scan


	if( fIsScannedMinNorm ){

		double x,y;

		for(int istep=0; istep<N; ++istep){
			g->GetPoint(istep,x,y);

			double ynew=y-fmin+fScannedMinNorm;

			g->SetPoint(istep,x,ynew);
		}


		fmin=fScannedMinNorm;
	}


	fScannedFunctionMin=fmin;
	fScannedMins.clear();
	fScannedMins.push_back(par_min);


	par.ForceToRange(par_range_set);
	par.SetValue(par_orig_value);

	delete plist;

	delete [] x;


	/*
	TString gname="gParScan_"; gname+=GetName(); gname+="_"; gname+=par.GetName();

	TString gtit="param scan: "; gtit+=par.GetLabel();
	gtit+=";"; gtit+=par.GetAxisTitle(); gtit+=";"; gtit+="minimization function";
	 */

	TString gname=STR_UTILS::Name("gParScan","_",GetName(),"_",par.GetName());
	TString gtitle=STR_UTILS::TitleXYZ(STR_UTILS::Title("param scan:"," ",par.GetLabel()),par.GetAxisTitle(),"minimization function");

	g->SetName(gname);
	g->SetTitle(gtitle);


	cout<<" -- scan exit. graph: "<<g->GetName()<<endl;

	cout<<" Minimum: "<<fmin<<" for param  "<<par.GetName()<<": "<<par_min<<"; range: "<<min<<" "<<max<<" npoints: "<<N<<endl;

	return g;
}




TGraph2D* TSMinimizationFunction::ScanParameters(TSParameter& parx,TSParameter &pary,int Nx, double xmin, double xmax, int Ny, double ymin ,double ymax) {


	TGraph2D *g=new TGraph2D();

		if(Nx<2 || xmin>=xmax || Ny<2 ||ymin>=ymax){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
			return g;
		}

		if( !GetModel() ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," No Model found");
			return g;
		}


		TSParamList *plist=new TSParamList();

		TSArgList *alist= GetModel()->GetPrimaryParamList();

		plist->Copy(*alist);

		if(!plist){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
			return g;
		}


		cout<<" Scanning Params: "<<parx.GetName()<<" "<<pary.GetName()<<endl;
		cout<<"Param List "<<endl;
		plist->PrintParams();

		int NPARS=plist->GetSize();


		bool parx_range_set=parx.HasRangeForced();
		double parx_orig_value=parx.GetValue();

		bool pary_range_set=pary.HasRangeForced();
		double pary_orig_value=pary.GetValue();

		parx.ForceToRange(false);
		pary.ForceToRange(false);



		double *x=new double[NPARS];

		double fmin=0;
		double parx_min=0;
		double pary_min=0;


		std::vector<double> vfvalues;

		int np=0;

		for(int istepx=0; istepx<Nx; ++istepx){
			for(int istepy=0; istepy<Ny; ++istepy){


			double pvalx=xmin+(xmax-xmin)*istepx/(Nx-1);
			double pvaly=ymin+(ymax-ymin)*istepy/(Ny-1);


			parx.SetValue(pvalx);
			pary.SetValue(pvaly);

			for(int ip=0;ip<NPARS;++ip){

				x[ip]= plist->At(ip)->GetValue();

			}

			double func=DoEval(x);

			g->SetPoint(g->GetN(),pvalx,pvaly,func);

			if( fIsScannedMinNorm ){
				vfvalues.push_back(func);
			}

			if(istepx==0 && istepy==0){
				fmin=func;
				parx_min=pvalx;
				pary_min=pvaly;
			}else{
				if(func<fmin) {
					fmin=func;
					parx_min=pvalx;
					pary_min=pvaly;
				}
			}

			}//scanx
		}//scany


		if( fIsScannedMinNorm ){

			double znew;

			int n=0;
			for(int istepx=0; istepx<Nx; ++istepx){
				for(int istepy=0; istepy<Ny; ++istepy){

					//try this to avoid filling the vector vfvalues
					//double xmin = g->GetHistogram()->GetXaxis()->GetBinCenter(ix);
					//double ymin = g->GetHistogram()->GetYaxis()->GetBinCenter(iy);

					double pvalx=xmin+(xmax-xmin)*istepx/(Nx-1);
					double pvaly=ymin+(ymax-ymin)*istepy/(Ny-1);

					double znew=vfvalues[n]-fmin+fScannedMinNorm;

					g->SetPoint(n,pvalx,pvaly,znew);
					n++;
				}
			}

			fmin=fScannedMinNorm;
		}


		fScannedFunctionMin=fmin;
		fScannedMins.clear();
		fScannedMins.push_back(parx_min);
		fScannedMins.push_back(pary_min);


		parx.ForceToRange(parx_range_set);
		parx.SetValue(parx_orig_value);
		pary.ForceToRange(pary_range_set);
		pary.SetValue(pary_orig_value);


		delete plist;

		delete [] x;

		/*
		TString gname="gParScan_"; gname+=GetName();
		gname+="_"; gname+=parx.GetName(); gname+="_"; gname+=pary.GetName();

		TString gtit="param scan: "; gtit+=parx.GetLabel(); gtit+=" vs "; gtit+=pary.GetLabel();
		gtit+=";"; gtit+=parx.GetAxisTitle();
		gtit+=";"; gtit+=pary.GetAxisTitle();
		gtit+=";"; gtit+="minimization function";
		 */

		TString gname=STR_UTILS::Name("gParScan","_",GetName(),"_",parx.GetName(),"_",pary.GetName());
		TString gtitle=STR_UTILS::TitleXYZ(STR_UTILS::Title(parx.GetLabel()," vs ",pary.GetLabel()),parx.GetAxisTitle(),pary.GetAxisTitle(),"minimization function");
		g->SetName(gname);
		g->SetTitle(gtitle);

		g->GetXaxis()->SetTitleOffset( 1.6*g->GetXaxis()->GetTitleOffset() );
		g->GetYaxis()->SetTitleOffset( 1.6*g->GetYaxis()->GetTitleOffset() );
		g->GetZaxis()->SetTitleOffset( 1.6*g->GetZaxis()->GetTitleOffset() );




		cout<<" -- scan exit. graph: "<<g->GetName()<<endl;

		cout<<" Minimum: "<<fmin
				<<" for params  "<<parx.GetName()<<": "<<parx_min<<" vs "<<pary.GetName()<<": "<<pary_min<<endl;
		cout<<" x-range: "<<xmin<<" "<<xmax<<" npoints: "<<Nx<<endl;
	    cout<<" y-range: "<<ymin<<" "<<ymax<<" npoints: "<<Ny<<endl;

		return g;

}


void TSMinimizationFunction::ScanParameters3D(const TSParamList& list,
		int Nx, double xmin, double xmax, int Ny, double ymin ,double ymax,int Nz, double zmin ,double zmax
		) {


	    if( list.GetSize()!=3){
	    	MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
	    	return;
	    }

		if(Nx<2 || xmin>=xmax || Ny<2 ||ymin>=ymax || Nz<2 || zmin>=zmax){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
			return;
		}

		if( !GetModel() ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," No Model found");
			return;
		}


		TSParamList *plist=new TSParamList();

		TSArgList *alist= GetModel()->GetPrimaryParamList();

		plist->Copy(*alist);

		if(!plist){
			return;
		}


		TSParameter *parx=list.At(0);
		TSParameter *pary=list.At(1);
		TSParameter *parz=list.At(2);

		cout<<" Scanning Params: "<<parx->GetName()<<" "<<pary->GetName()<<" "<<parz->GetName()<<endl;
		cout<<"Param List "<<endl;
		plist->PrintParams();

		int NPARS=plist->GetSize();


		bool parx_range_set=parx->HasRangeForced();
		double parx_orig_value=parx->GetValue();

		bool pary_range_set=pary->HasRangeForced();
		double pary_orig_value=pary->GetValue();

		bool parz_range_set=parz->HasRangeForced();
		double parz_orig_value=parz->GetValue();

		parx->ForceToRange(false);
		pary->ForceToRange(false);
		parz->ForceToRange(false);



		double *x=new double[NPARS];

		double fmin=0;
		double parx_min=0;
		double pary_min=0;
		double parz_min=0;

		int np=0;

		for(int istepx=0; istepx<Nx; ++istepx){
			for(int istepy=0; istepy<Ny; ++istepy){
				for(int istepz=0; istepz<Nz; ++istepz){


			double pvalx=xmin+(xmax-xmin)*istepx/(Nx-1);
			double pvaly=ymin+(ymax-ymin)*istepy/(Ny-1);
			double pvalz=zmin+(zmax-zmin)*istepz/(Nz-1);

			parx->SetValue(pvalx);
			pary->SetValue(pvaly);
			parz->SetValue(pvalz);


			for(int ip=0;ip<NPARS;++ip){

				x[ip]= plist->At(ip)->GetValue();

			}

			double func=DoEval(x);

			if(np==0){
				fmin=func;
				parx_min=pvalx;
				pary_min=pvaly;
				parz_min=pvalz;
			}else{
				if(func<fmin) {
					fmin=func;
					parx_min=pvalx;
					pary_min=pvaly;
					parz_min=pvalz;
					//cout<<"New Min: scan3d "<<np<<"; "<<istepx<<" "<<istepy<<" "<<istepz
						//	<<"; "<<pvalx<<" "<<pvaly<<" "<<pvalz<<"; "<<parx_min<<" "<<pary_min<<" "<<parz_min<<endl;
				}
			}

			np++;
			if(np%10000==0) cout<<"scan3d "<<np<<"; "<<istepx<<" "<<istepy<<" "<<istepz
					<<"; "<<pvalx<<" "<<pvaly<<" "<<pvalz<<"; "<<parx_min<<" "<<pary_min<<" "<<parz_min<<endl;

				}//scanz
			}//scany
		}//scanz

		fScannedFunctionMin=fmin;
		fScannedMins.clear();
		fScannedMins.push_back(parx_min);
		fScannedMins.push_back(pary_min);
		fScannedMins.push_back(parz_min);


		parx->ForceToRange(parx_range_set);
		parx->SetValue(parx_orig_value);

		pary->ForceToRange(pary_range_set);
		pary->SetValue(pary_orig_value);

		parz->ForceToRange(parz_range_set);
		parz->SetValue(parz_orig_value);


		delete plist;

		delete [] x;



		cout<<" Minimum: "<<fmin
				<<" for params  "<<parx->GetName()<<": "<<parx_min<<" vs "<<pary->GetName()<<": "<<pary_min
				<<" vs "<<parz->GetName()<<": "<<parz_min<<endl;
		cout<<" x-range: "<<xmin<<" "<<xmax<<" npoints: "<<Nx<<endl;
	    cout<<" y-range: "<<ymin<<" "<<ymax<<" npoints: "<<Ny<<endl;
	    cout<<" z-range: "<<zmin<<" "<<zmax<<" npoints: "<<Nz<<endl;

}

void TSMinimizationFunction::GetScannedMinima(double &fval, double *pars) {

	fval= fScannedFunctionMin;
	for(int i=0; i<fScannedMins.size(); ++i){
		pars[i]=fScannedMins[i];
	}

}


void TSMinimizationFunction::SetNormalizedScannedMin(double min){

	fIsScannedMinNorm=true;
	fScannedMinNorm=min;

}


inline double TSMinimizationFunction::Compute () const {


	//
	// Pass param. values to the model
	//

	// fModel.SetParameters();


	//
	// Loop over data
	//






	return 0;

}
