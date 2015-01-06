/*
 * TSHistoNuiParamMgr.cc
 *
 *  Created on: Oct 10, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "MessageMgr.h"
#include "RootUtils.h"

#include "TSHistoNuiParamMgr.h"


#define VERBOSE true;
#ifdef VERBOSE
#define V(x) cout<<x<<endl;
#else
#define V(x)
#endif



TSHistoNuiParamMgr::TSHistoNuiParamMgr() {

	Init();
}

TSHistoNuiParamMgr::~TSHistoNuiParamMgr() {


	fDeleteNuiParamArray();

}

int TSHistoNuiParamMgr::AddNuiParam(TSNuiParam &nuiPar)
{




	return 1;
}


int TSHistoNuiParamMgr::AddNuiParamWithRespFunc(TSNuiParam &nuiParam, TSRespFunc &respFunc)
{

	bool DEBUG = VERBOSE;

	if( !fHisto ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Cannot add Param: No Histogram is assigned to the Mgr");
		return 0;
	}



	V(__FILE__<<"::"<<__FUNCTION__<<" Adding Param: "<<nuiParam.GetName()<<" and Resp Function: "<<respFunc.GetName());


	if( !nuiParam.HasControlVars() ){

		V("nuiPar has no control variables: Add it to all the bins");

		for(int ix=1; ix<=fHistoNbins[0]; ++ix){
			for(int iy=1; iy<=fHistoNbins[1]; ++iy){

				int idx = fGetLinearIdx(ix,iy);

				//create a ParamSet if not present yet for this bin
				if ( !fVecOfNuiParamSet[idx] ){
					fVecOfNuiParamSet[idx] = new TSNuiParamSet();
				}

				//add this param to the list of params acting on this bin
				fVecOfNuiParamSet[idx]->Add(&nuiParam);

				//add resp function

			}//iy
		}//ix


		return 1;

	}//param has no control vars


	//
	// Check if this params is to be added only to a particular set of bins
	//

	V("Histogram Axis variables: ");
	V(" X: "<<fAxisVariable[0]->GetName() );
	if( fHistoDim==2 ) V(" Y: "<<fAxisVariable[1]->GetName() );


	V("  Parameter: control vars ");

     if(DEBUG){
		for(int iv=0; iv<nuiParam.GetNumOfControlVars();++iv){
			cout<<iv<<" "<<nuiParam.GetControlVariable(iv)->GetName()
			  <<" range: "<<nuiParam.GetControlVariable(iv)->GetRange()<<endl;
		}
     }

	int parVarIdx[2]={-1,-1};

	int axisFlag[2]={0,0};


	if( nuiParam.HasControlVars() ){

		for(int iv=0; iv<nuiParam.GetNumOfControlVars(); ++iv){


			if( TString(fAxisVariable[0]->GetName()) == TString(nuiParam.GetControlVariable(iv)->GetName()) ){
			//if( nuiParam.HasControlVariable(fAxisVariable[0]) ){	 //should use this now...check first
				V(" Control Variable Match on axis X: "<<fAxisVariable[0]->GetName());
				parVarIdx[0]=iv;
			}

			if( TString(fAxisVariable[1]->GetName()) == TString(nuiParam.GetControlVariable(iv)->GetName()) ){
			//if( nuiParam.HasControlVariable(fAxisVariable[1]) ){	 //should use this now.. check first
				V(" Control Variable Match on axis Y: "<<fAxisVariable[1]->GetName());
				parVarIdx[1]=iv;
			}

		}//iv

	}//

	if(fHistoDim >= 1){

		if(parVarIdx[0]==-1){
			V("X axis var is not a control var for param: add param to all X bins");
			axisFlag[0]=1;
		}


		if(parVarIdx[0]>=0 ){
	        if(nuiParam.GetControlVariable(parVarIdx[0])->HasRange()){
	        	V("X axis var is  a control var for param: add param to X bins comprised in the energy range of the param control var");
	        	axisFlag[0]=2;
	        }else{
	        	V("X axis var is  a control var for param w/o a specified range: add param to all X bins");
	        	axisFlag[0]=1;
	        }
		}

	}

	if(fHistoDim == 2){

		if(parVarIdx[1]==-1){
			V("Y axis var is not a control var for param: add param to all Y bins");
			axisFlag[1]=1;
		}

		if(parVarIdx[1]>=0 ){
			if(nuiParam.GetControlVariable(parVarIdx[1])->HasRange()){
				V("Y axis var is  a control var for param: add param to Y bins comprised in the energy range of the param control var");
				axisFlag[1]=2;
			}else{
				V("Y axis var is  a control var for param w/o a specified range: add param to all Y bins");
				axisFlag[1]=1;
			}
		}

	}


	//axisFlag: 0 do not attach param
	//          1 attach to all the bins
	//          2        to bins in the right range


	if( fHistoDim==1 ){

		for(int ix=1; ix<=fHistoNbins[0]; ++ix){


			bool skip_bin=true;


			if(axisFlag[0]==2){
				double xmin = fHisto->GetXaxis()->GetBinLowEdge(ix);
				double xmax = fHisto->GetXaxis()->GetBinUpEdge(ix);
				double xcenter = fHisto->GetBinCenter(ix);

				//NB this will return true if the control variable of the parameter has no range
				//if default will change check explicitly:
				// if(!nuiParam.GetControlVariable(parVarIdx[0])->HasRange()) skip_bin = false;
				//
				if(nuiParam.GetControlVariable(parVarIdx[0])->ContainsRange(xmin,xmax)){

					V(" Selected X bin: "<<ix<<" range: ["<<xmin<<","<<xmax<<"]");
					skip_bin = false;
				}

			}else if(axisFlag[0]==1){
					skip_bin=false;
			}

			if(skip_bin) continue;

			int idx = ix;

			//create a ParamSet if not present yet for this bin
			if ( !fVecOfNuiParamSet[idx] ){
				fVecOfNuiParamSet[idx] = new TSNuiParamSet();
			}

			//add this param to the list of params acting on this bin
			fVecOfNuiParamSet[idx]->Add(&nuiParam);

			//add resp function


		}//ix


		return 1;
	}//DIM =1


	if( fHistoDim == 2 ) {

		for(int ix=1; ix<=fHistoNbins[0]; ++ix){

			bool skip_bin_x = true;

			double xmin = fHisto->GetXaxis()->GetBinLowEdge(ix);
			double xmax = fHisto->GetXaxis()->GetBinUpEdge(ix);
			double xcenter = fHisto->GetXaxis()->GetBinCenter(ix);

			if(axisFlag[0]==2){
				if(nuiParam.GetControlVariable(parVarIdx[0])->ContainsRange(xmin,xmax)){
					//V(" Selected X bin: "<<ix<<" range: ["<<xmin<<","<<xmax<<"]");
					skip_bin_x = false;
				}

			}else if(axisFlag[0]==1){
				skip_bin_x=false;
			}


			if(skip_bin_x) continue;

			V(" Selected X bin: "<<ix<<" range: ["<<xmin<<","<<xmax<<"]");

			for(int iy=1; iy<=fHistoNbins[1]; ++iy){

				bool skip_bin_y = true;

				double ymin = fHisto->GetYaxis()->GetBinLowEdge(iy);
				double ymax = fHisto->GetYaxis()->GetBinUpEdge(iy);
				double ycenter = fHisto->GetYaxis()->GetBinCenter(iy);

				if( axisFlag[1]==2){
					if(nuiParam.GetControlVariable(parVarIdx[1])->ContainsRange(ymin,ymax)){
						//V(" Selected Y bin: "<<iy<<" range: ["<<ymin<<","<<ymax<<"]");
						skip_bin_y = false;
					}

				}else if(axisFlag[1]==1){
					skip_bin_y = false;
				}

				if(skip_bin_y) continue;

				V(" Selected Y bin: "<<iy<<" range: ["<<ymin<<","<<ymax<<"]");

				//attach par and rf to this bin

				int idx = fGetLinearIdx(ix,iy);

				//create a ParamSet if not present yet for this bin
				if ( !fVecOfNuiParamSet[idx] ){
					V(" Create list for bin "<<ix<<" "<<iy<<" idx: "<<idx);
					fVecOfNuiParamSet[idx] = new TSNuiParamSet();
				}

				//add this param to the list of params acting on this bin
				V(" Add Param to  bin "<<ix<<" "<<iy<<" idx: "<<idx);

				fVecOfNuiParamSet[idx]->Add(&nuiParam);

				//add resp function

			}//iy
		}//ix


		return 1;

	}// DIM = 2


	MSG::ERROR(__FILE__,"::",__FUNCTION__," Unable to Add Parameter ",nuiParam.GetName()," to histo ",fHisto->GetName());
	return 0;

}


void TSHistoNuiParamMgr::Init() {


		fHisto = 0;
		fHistoDim=0;
		fHistoNbins[0]=0;
		fHistoNbins[1]=0;

		fAxisVariable[0]=0;
		fAxisVariable[1]=0;


}




void TSHistoNuiParamMgr::PrintBinInfo(int ix, int iy) const
{

	if(!fHisto){
		cout<<__FUNCTION__<<" No Histogram Set"<<endl;
		return;
	}




	int idx = fGetLinearIdx(ix,iy);

	if( !fVecOfNuiParamSet[idx] ){
		//cout<<__FUNCTION__<<" No NuiParamSet for bin: "<<ix<<" "<<iy<<endl;
		return;
	}

	int NNui = 0;
	if(fVecOfNuiParamSet[idx]) NNui = fVecOfNuiParamSet[idx]->GetSize();



	TString var_name[2];

	for(int i=0; i<2;++i){
		fAxisVariable[i] ? var_name[i] = fAxisVariable[i]->GetName(): var_name[i]="";
	}

	cout<<" "<<endl;
	cout<<" Bin X: "<<ix<<" "<<var_name[0]<<" "<<RootUtils::GetBinRangeLabel(*fHisto,ix,"x");

	if(fHistoDim==2){
		cout<<" Bin Y: "<<iy<<" "<<var_name[1]<<" "<<RootUtils::GetBinRangeLabel(*fHisto,iy,"y");
	}
	cout<<endl;


	cout<<" Number of Nuisance Parameters: "<<NNui<<endl;

	if(!fVecOfNuiParamSet[idx]) return;

	fVecOfNuiParamSet[idx]->PrintNameSeq();

	fVecOfNuiParamSet[idx]->Print();


}


void TSHistoNuiParamMgr::Print() const
{

	cout<<" ======== "<<__FILE__<<"::"<<__FUNCTION__<<" - "<<GetName()<<endl;

	if( !fHisto ){
		cout<<" No Histogram Set "<<endl;
		return;
	}

	cout<<" Histogram: "<< fHisto->GetName()<<" Dimension: "<<fHistoDim<<" Nbins X: "<<fHistoNbins[0];
    if(fHistoDim==2) cout<<" Y: "<<fHistoNbins[1];
    cout<<endl;


	TString var_name[2];

	for(int i=0; i<2;++i){
		fAxisVariable[i] ? var_name[i] = fAxisVariable[i]->GetName(): var_name[i]="";
	}

	cout<<"Axis Variable X: "<<var_name[0];if(fHistoDim==2) cout<<" Y: "<<var_name[1];cout<<endl;


	int N=0;
	for(int i=0; i<fVecOfNuiParamSet.size();++i){
		if(fVecOfNuiParamSet[i] !=0) N++;
	}

	if( N==0 ){

		cout<<" No NuiParamSet present to Histogram bins"<<endl;

	}else{

		cout<<" -List of bins-: "<<fHistoNbins[0]<<" x "<<fHistoNbins[1]<<endl;

		for(int ix=1; ix<=fHistoNbins[0]; ++ix){

			for(int iy=1; iy<=fHistoNbins[1]; ++iy){


				PrintBinInfo(ix,iy);


			}//iy

		}//ix

	}//at least one bins as a set of params attached

	cout<<" ======== "<<__FILE__<<"::"<<__FUNCTION__<<" - "<<GetName()<<" ==== Out ===="<<endl;

}




void TSHistoNuiParamMgr::ReweightHistogram(int opt)
{



	for(int ix=1; ix<=fHistoNbins[0]; ++ix){
		for(int iy=1; iy<=fHistoNbins[1]; ++iy){

			int idx = fGetLinearIdx(ix,iy);

			float cont = fHisto->GetBinContent(ix,iy);

			double w = fVecOfNuiParamSet[idx]->ComputeWeight();

			fHisto->SetBinContent(ix,iy,cont*w);

		}//iy
	}//ix



}




void TSHistoNuiParamMgr::SetHistogram(TH1* h, const TSVariable *vx) {


	SetHistogram(h,vx,0);


}


void TSHistoNuiParamMgr::SetHistogram(TH1* h, const TSVariable *vx, const TSVariable *vy) {

	if( h ){

		fHisto = h;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null Ptr to Hist");
		assert(0);
	}



	if( RootUtils::IsTH1(*h) ){

		fHistoDim = 1;

		if( !vx ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," No variable specified for axis X");
			exit(0);
		}


		fAxisVariable[0] = (TSVariable*)vx;

	}else if( RootUtils::IsTH2(*h) ){

		fHistoDim = 2;


		if( !vx ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," No variable specified for axis X");
			exit(0);
		}

		if( !vy ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," No variable specified for axis Y");
			exit(0);
		}

		fAxisVariable[0] = (TSVariable*)vx;
		fAxisVariable[1] = (TSVariable*)vy;


	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Not a TH1/TH2 obj");
		assert(0);
	}


	fHistoNbins[0] = fHisto->GetNbinsX();
	fHistoNbins[1] = fHisto->GetNbinsY();


	fDeleteNuiParamArray();

	int Dim = fHistoNbins[0]*fHistoNbins[1];

	fVecOfNuiParamSet.reserve(Dim);

	for(int i=0; i<Dim; ++i){
		fVecOfNuiParamSet.push_back(0);
	}


}




void TSHistoNuiParamMgr::fDeleteNuiParamArray(){


	for(int i=0; i<fVecOfNuiParamSet.size(); ++i){

		delete fVecOfNuiParamSet[i];

	}

	fVecOfNuiParamSet.clear();

}



int TSHistoNuiParamMgr::fGetLinearIdx(int ix, int iy) const
{

	/*
	 *   iy
	 *
	 *    4
	 *    3
	 *    2
	 *    1 2 3 4 5 6  ix
	 *
	 *
	 *    ix=NX iy=NY
	 *
	 *    id = NX+(NY-1)*NX = NX*NY
	 *
	 *    ix=1 iy=NY
	 *
	 *    id = 1+(NY-1)*NX = NX*NY-(NX-1)
	 *
	 */

	if(ix<1 || ix>fHistoNbins[0]) return -1;
	if(iy<1 || iy>fHistoNbins[1]) return -1;

	 int id = ix + (iy-1)*fHistoNbins[0];

	 id -= 1; //start from zero in the linear array

	 return id;

}

