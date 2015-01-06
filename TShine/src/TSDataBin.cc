/*
 * TSDataBin.cc
 *
 *  Created on: Jun 3, 2014
 *      Author: sdiluise
 */

#include <vector>


#include <TMath.h>
#include <TBranch.h>

#include "TH1FAuto.h"
#include "TH2FAuto.h"

#include "RootUtils.h"
#include "StringUtils.h"

#include "TSDataBin.h"



TSDataBin::TSDataBin() {
	// TODO Auto-generated constructor stub

	Init();
}

TSDataBin::TSDataBin(TString name, TString label, TString title)
:TSNamed(name,label,title)
{

	Init();

}

TSDataBin::TSDataBin(const TSDataBin& other)
:TSNamed( other.Name(), other.GetLabel(), other.Title() )
{

	//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	Init();


	SetUnbinned( other.IsUnbinned() );

	//
	// Import Phase Space Volume if Any
	//

	if( other.HasPhaseSpaceVolume() ){

		SetPhaseSpaceVolume( *other.GetPhaseSpaceVolume() );
	}


	//
	//Now Add Non Phase Space Variables
	//

	for(int ivar=1; ivar<=other.GetNumOfVars(); ++ivar){


		if( other.IsPhaseSpace(ivar) ) continue;

		int nbins, auto_type;
		double low,up;

		other.GetVarSettings(ivar,auto_type,nbins,low,up);


		if( !auto_type ){

			SetVariable(*other.GetVariable(ivar),nbins,low,up);

		}else if(auto_type==1){

			SetVariable(*other.GetVariable(ivar));

		}else if(auto_type==2){

			SetVariable(*other.GetVariable(ivar),low,up);

		}

	}


	//
	// Add the 2D Histos, 1D Histos are set when you add the variables
	//


	SetHistoTitleStyle( other.GetHistoTitleStyle() );

	TSHistogramFld *hFld= other.GetHistoFolder();

	//hFld->Print();

	GetHistoFolder()->SetSumw2( hFld->IsSumw2() );

	for(int ih=0; ih<hFld->GetSize(); ++ih){

		if( hFld->GetHistoDim(ih+1)==2 ){

			TH2F *h2 = new TH2F( *(TH2F*)hFld->At(ih) );

			//GetHistoFolder()->Add( hFld->At(ih)->Clone( hFld->At(ih)->GetName() ), hFld->GetHistoNameX(ih+1), hFld->GetHistoNameY(ih+1));

			GetHistoFolder()->Add( h2, hFld->GetHistoNameX(ih+1), hFld->GetHistoNameY(ih+1));

		}

	}


	//GetHistoFolder()->RenameAllWithBase("..");

	//cout<<__FILE__<<"::"<<__FUNCTION__<<" --- out --"<<endl;


}


TSDataBin::~TSDataBin() {


	delete fHistoFld; fHistoFld=NULL;

    delete fDataTree; fDataTree=NULL;

    //delete fPhaseSpaceVol;


    for(std::map<int,VarSettings*>::iterator it=fVarSetsList.begin(), end=fVarSetsList.end();
    		it!=end; it++){

    	delete it->second;
    }


}




int TSDataBin::Fill(const TSArgList &list){


	int NVLIST= list.GetSize();

	if(NVLIST==0) return 0;


	bool IsInside=true;

	if( HasPhaseSpaceVolume() ){

		IsInside = fPhaseSpaceVol->IsInside();

	}

	if( !IsInside ){
		return 0;
	}



	std::vector<TString> var_list;
	std::vector<double> values;


	if( fIsUnbinned ){
		for(int i=1; i<=fNumOfVars; ++i){
			fDataBufferWeight[i-1]=0;
		}
	}


	for(int ilist=0; ilist<NVLIST;++ilist){

		TSVariable *vsel=(TSVariable*)list.At(ilist);

		for(int i=1; i<=fNumOfVars; ++i){

			TSVariable *var = (TSVariable*)GetVariable(i);

			if( var==vsel ){
				var_list.push_back( vsel->Name() );
				values.push_back( vsel->GetValue() );


				if(fIsUnbinned){

					fDataBuffer[i-1]=var->GetValue();
					fDataBufferWeight[i-1]=1;
				}


			}


		}//all var loop

	}//loop list


	fHistoFld->Fill(var_list, values);


	if(fIsUnbinned){
		fDataTree->Fill();
	}



	return 1;

}





int TSDataBin::Fill(const TSVariable &v){

	bool IsInside=true;

	if( HasPhaseSpaceVolume() ){

		IsInside = fPhaseSpaceVol->IsInside();

	}

	if( !IsInside ){
		return 0;
	}


	std::vector<TString> var_list;
	std::vector<double> values;


	if( fIsUnbinned ){
		for(int i=1; i<=fNumOfVars; ++i){
			fDataBufferWeight[i-1]=0;
		}
	}

	for(int i=1; i<=fNumOfVars; ++i){

		TSVariable *var = (TSVariable*)GetVariable(i);

		if( var == &v){

			var_list.push_back( v.Name() );
			values.push_back( var->GetValue() );

			fHistoFld->Fill(v.Name(), var->GetValue());

			//fHistoFld->Fill(var_list, values);

			if(fIsUnbinned){

				fDataBuffer[i-1]=GetVariable(i)->GetValue();
				fDataBufferWeight[i-1]=1;

				fDataTree->Fill();

			}

			return 1;
		}//var


	}//loop


	return 1;
}


int TSDataBin::Fill(){




	bool IsInside=true;

	if( HasPhaseSpaceVolume() ){

		IsInside = fPhaseSpaceVol->IsInside();


	}

	if( !IsInside ){
		return 0;
	}



	std::vector<TString> var_list; var_list.reserve(fNumOfVars);
	std::vector<double> values; values.reserve(fNumOfVars);

	for(int i=1; i<=fNumOfVars; ++i){

		var_list.push_back( GetVariable(i)->Name() );
		values.push_back( GetVariable(i)->GetValue() );

		if(fIsUnbinned){
			fDataBuffer[i-1]=GetVariable(i)->GetValue();
			fDataBufferWeight[i-1]=1;
		}
	}


	fHistoFld->Fill(var_list, values);


	if( fIsUnbinned ){

		fDataTree->Fill();

	}


	/*
	static int count=0; count++;

	cout<<"ISINSIDE: "<<count<<" "<<fNumOfVars<<" "<<endl;
	fPhaseSpaceVol->Print();

	for(int i=0;i<var_list.size();++i){
		cout<<i<<" "<<var_list[i]<<" "<<values[i]<<endl;
	}
	cout<<" "<<endl;
	 */

	return 1;
}



const TSVariable* TSDataBin::GetVariable(int var) const{

	if(var<=0 || var>GetNumOfVars() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fVarList.count(var) !=1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," variable not set");
		return 0;
	}


	return ( fVarList.find(var) )->second;

}



int TSDataBin::GetVarSettings(int ivar, int &auto_type, int &nbins, double &min, double &max) const {

/**
 *
 *
 *  Return Settings Used to book Variable Histograms
 *
 *  auto_type = 0: Binning his specified
 *  auto_type = 1: Binning fully auto
 *  auto_type = 2: Binning Auto with a Range
 *  (See TH1FAuto/TH2FAuto documentation for
 *   details on the two last options)
 *
 *
 *
 */
	auto_type=0;
	nbins=0;
	max=0;
	min=0;


	if( !fVarSetsList.count(ivar) ){
		return 0;
	}

	VarSettings *vSet = fVarSetsList.find(ivar)->second;


	auto_type = vSet->IsBinningAuto;


	if( !auto_type ){

		nbins= vSet->NumOfBins;
		min = vSet->Min;
		max = vSet->Max;

	}else if( vSet->HasAutoRangeLimits){

		auto_type=2;
		nbins=-1;
		min = vSet->RangeLimLow;
		max = vSet->RangeLimUp;

	}

	return 1;

}



bool TSDataBin::IsPhaseSpace(int ivar) const{

	if( fVarSetsList.count(ivar)==1 ){

		return ((fVarSetsList.find(ivar))->second)->IsPhaseSpace;

	}else{

		return 0;

	}

}


void TSDataBin::Init() {


	fHasPhaseSpaceVol=false;
	fIsUnbinned=false;
	fNumOfVars=0;

	fHistoFld=0;
	fDataTree=0;
	fPhaseSpaceVol=0;

	SetHistoTitleStyle(0);

	fHistoNameBase0="h_";


}



int TSDataBin::SetBidimensional(TSVariable& var_x, TSVariable& var_y){

	int idx=0;
	int idy=0;

	for(int i=1; i<=fNumOfVars; ++i){

		if( GetVariable(i) == &var_x ) idx=i;
		if( GetVariable(i) == &var_y ) idy=i;
	}

	if(  !(idx!=0 && idy!=0 && idx!=idy) ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," illegal set of variables:");
		MSG::WARNING( var_x.Name(), " vs ",var_y.Name() );
		return 0;
	}


	fBuildHistogram2D(idx,idy);

	return 1;
}


int TSDataBin::SetHistoTitleStyle(int s){

	/**
	 *
	 *
	 * 0: Axis variables are listed (x vs y)
	 * and Phase Space Volume description is added if any
	 *
	 *
	 * 1: Either Axis variables or
	 *    Phase Space Volume description if any
	 *
	 * Separator is ",".
	 * this as to be changed both in
	 * fUpdataHistoTitles() anf fBuildHistoTitle(...)
	 *
	 */

	if(s==0 || s==1){
		fHistoTitleStyle=s;
	}else{
		SetHistoTitleStyle(0);
	}

}



void TSDataBin::SetName(TString name){

	TSNamed::SetName(name);


	GetHistoFolder()->RenameAllWithBase( fBuildHistoNameBase() );

}



int TSDataBin::SetPhaseSpaceVolume( TSPhaseSpaceVolume& vol ){


	if( HasPhaseSpaceVolume() ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," Phase Space Volume already set");
		return 0;
	}


	fPhaseSpaceVol = new TSPhaseSpaceVolume( vol );

	fPSHistoNBinsX=100; //-1 for Auto

	for(int dim=1; dim <= vol.GetDimension(); ++dim){

		const TSVariable* var= vol.GetVariable(dim);

		double vmin= vol.GetLowEdge(dim);
		double vmax= vol.GetUpEdge(dim);

		fSetVariable( *var, fPSHistoNBinsX, vmin,vmax, 1 );
	}

	fHasPhaseSpaceVol=true;

	//Rename Titles of Histos already booked
	fUpdateHistoTitles();


	return 1;
}



void TSDataBin::SetUnbinned(bool flag) {

	fIsUnbinned=flag;

	if(fIsUnbinned){
		fInitDataTree();
	}

}



int TSDataBin::SetVariable(const TSVariable& var) {


	return fSetVariable(var,-1,0,0,0);

}



int TSDataBin::SetVariable(const TSVariable& var, double range_min, double range_max) {


	return fSetVariable(var,-1,range_min,range_max,0);

}



int TSDataBin::SetVariable(const TSVariable& var, int n, double low, double up) {


	if(TMath::AreEqualRel(low,up,1e-5)){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," illegal Histogram range: ",low,",",up);
		return 0;
	}

	return fSetVariable(var,n,low,up,0);

}


int TSDataBin::fSetVariable(const TSVariable& var, int n, double low, double up, int type) {

	/**
	 *
	 *  type = 0 for PhaseSpaceVolume Variable
	 *
	 *
	 *
	 */



	for(int i=1; i<=fNumOfVars; ++i){

		if( fVarList[i] == &var ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__," variable: ",var.Name()," already present: not added");
			return 0;
		}
	}


	if( n<-1 || low>up ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," variable: ",var.Name()," illegal binning");
		return 0;
	}

	fNumOfVars++;
	fVarList[fNumOfVars]=&var;
	fVarSetsList[fNumOfVars]=new VarSettings();
	if(type==1)	fVarSetsList[fNumOfVars]->IsPhaseSpace=true;

	if(n==-1){
		fVarSetsList[fNumOfVars]->IsBinningAuto=true;

		if(low<up){
			fVarSetsList[fNumOfVars]->HasAutoRangeLimits=true;
			fVarSetsList[fNumOfVars]->RangeLimLow=low;
			fVarSetsList[fNumOfVars]->RangeLimUp=up;
		}

	}else{
		fVarSetsList[fNumOfVars]->NumOfBins=n;
		fVarSetsList[fNumOfVars]->Min=low;
		fVarSetsList[fNumOfVars]->Max=up;
	}



	fBuildHistogram1D(fNumOfVars,n,low,up);

	if( fIsUnbinned ){

		fAddTreeVariable(fNumOfVars);

	}

	return 1;
}


void TSDataBin::fUpdateHistoTitles(){

	if( !HasPhaseSpaceVolume() ) return;

	TString vol_description= fPhaseSpaceVol->GetDescription();

	if( GetHistoTitleStyle()==0 )
		fHistoFld->AppendToAllTitles(vol_description,", ");
	if( GetHistoTitleStyle()==1 )
		fHistoFld->AppendToAllTitles(vol_description);

}


void TSDataBin::fAddTreeVariable(int ivar){

	if( !fIsUnbinned || !fDataTree ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Data Tree not instantiated");
		return;
	}

	const TSVariable *var = GetVariable(ivar);

	if( !var ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," variable index out of range");
		return;
	}

	if(ivar == DATA_BUFF_SIZE){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," var. not added, reached data buffer size limit: ",DATA_BUFF_SIZE);
		return;
	}

	TString name,name2;

	name = var->Name();

	name2=name; name2+="/F";

	fDataTree->Branch(name,&fDataBuffer[ivar-1],name2);


	name = var->Name();

	name+="_w";

	name2=name; name2+="/I";


	fDataTree->Branch(name,&fDataBufferWeight[ivar-1],name2);


}



TString TSDataBin::fBuildEntriesTitle(float binw, TString unit){


	TString atit="Entries";


	if(binw>0){

		atit+="/";

		atit += STR_UTILS::ToString(binw);
	}

	if( !unit.IsWhitespace() && !unit.Contains("arb") ){

		atit+=" ";
		atit+=unit;
	}

	return atit;

}


TString TSDataBin::fBuildHistoName(int ivarx, int ivary) {

	/*
	TString hname="h_";

	TString name= ( GetName() );


	if( !name.IsWhitespace() ){
		hname+=name; hname+="_";
	}
	 */


	TString hbase = fBuildHistoNameBase();


	TString hname = hbase;


	TString namex= GetVariable(ivarx)->Name();

	hname+="_"; hname+=namex;


	if( ivary!=0 ){

		hname+="_vs_";

		TString namey= GetVariable(ivary)->Name();

		hname+=namey;
	}

	return hname;
}


TString TSDataBin::fBuildHistoNameBase() {

	TString base = fHistoNameBase0;

	base+=Name();

	return base;
}

TString TSDataBin::fBuildHistoTitle(int ivarx, int ivary) {


	TString htit="";

	TString htit_vol="";

	if( HasPhaseSpaceVolume() ){
		 htit_vol= fPhaseSpaceVol->GetDescription();
	}


	TString htit_vars="";

    TString titx=GetVariable(ivarx)->GetTitle();

    htit_vars+=titx;

    if( ivary!=0 ){

    	htit_vars+=" vs ";

    	TString tity= GetVariable(ivary)->GetTitle();

    	htit_vars+=tity;
    }

    int style= GetHistoTitleStyle();

    if(style==0){

    	htit=htit_vars;
    	if( HasPhaseSpaceVolume() ) htit+=", "; htit+=htit_vol;

    	return htit;
    }

    if(style==1){


    	if( HasPhaseSpaceVolume() ){
    		htit=htit_vol;
    	}else{
    		htit=htit_vars;
    	}

    	return htit;

    }






    return htit;
}



TString TSDataBin::fBuildAxisTitle(int ivar) {


	TString atit=GetVariable(ivar)->GetAxisTitle();

	return atit;

}



void TSDataBin::fBuildHistogram1D(int ivar, int n, double low, double up){


	if( !fHistoFld ){
		fInitHistoFld();
	}


	const TSVariable *var=GetVariable(ivar);

	TString var_name= var->Name();

	TString htit=  fBuildHistoTitle(ivar);

	TString htitx = fBuildAxisTitle(ivar);

	TString unit=""; if( var->HasUnit() ) unit=var->GetUnit();

	TString hname= fBuildHistoName(ivar);


	if( n>0 ){

		//Clone from temp to avoid "Potential Memory Leak WARNINGS"
		//TH1F htemp("ht",htit,n,low,up);
		//TH1F *h=(TH1F*)htemp.Clone(hname);

		TH1F *h=RootUtils::newTH1F(hname,htit,n,low,up);

        h->GetXaxis()->SetTitle(htitx);

        float binw=0; if( !unit.IsWhitespace() ) binw=h->GetXaxis()->GetBinWidth(1);

        TString htity=fBuildEntriesTitle(binw, unit);
        h->GetYaxis()->SetTitle(htity);

        fHistoFld->Add(h, var_name);


	}else if( n==-1 ){

		//Automatic Histo
		TH1FAuto *h;
		if(low<up) h=new TH1FAuto("ht",htit,low,up);
		else h=new TH1FAuto("ht",htit);

		h->SetName(hname);

		h->GetXaxis()->SetTitle(htitx);

		TString htity=fBuildEntriesTitle();
		h->GetYaxis()->SetTitle(htity);

		fHistoFld->Add(h, var_name);

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," wrong binning params for variable: ",var_name);
		return;
	}


}



void TSDataBin::fBuildHistogram2D(int ivarx, int ivary){

	if( !fHistoFld ){

		fInitHistoFld();

	}

	TString name_x = GetVariable(ivarx)->Name();
	TString name_y = GetVariable(ivary)->Name();


	TString hname=fBuildHistoName(ivarx,ivary);
	TString htit=fBuildHistoTitle(ivarx,ivary);
	TString htitx=fBuildAxisTitle(ivarx);
	TString htity=fBuildAxisTitle(ivary);

	int nx=fVarSetsList[ivarx]->NumOfBins;
	int ny=fVarSetsList[ivary]->NumOfBins;

	double xmin=fVarSetsList[ivarx]->Min;
	double xmax=fVarSetsList[ivarx]->Max;

	double ymin=fVarSetsList[ivary]->Min;
	double ymax=fVarSetsList[ivary]->Max;

	bool auto_x= fVarSetsList[ivarx]->IsBinningAuto;
	bool auto_y= fVarSetsList[ivary]->IsBinningAuto;

	bool is_range_lim_x=fVarSetsList[ivarx]->HasAutoRangeLimits;
	bool is_range_lim_y=fVarSetsList[ivary]->HasAutoRangeLimits;

	double range_x_low =fVarSetsList[ivarx]->RangeLimLow;
	double range_x_up=fVarSetsList[ivarx]->RangeLimUp;

	double range_y_low =fVarSetsList[ivary]->RangeLimLow;
	double range_y_up=fVarSetsList[ivary]->RangeLimUp;


	TH2FAuto *h2;

	//Use Dummy and then rename in order to
	//avoid the "Potential Memory Leak" WARNING
	TString hname_dummy="hdummy";

	if( auto_x && auto_y ){

		h2=new TH2FAuto(hname_dummy,htit);

		if(is_range_lim_x) h2->SetRangeLimitsX(range_x_low,range_x_up);
		if(is_range_lim_y) h2->SetRangeLimitsY(range_y_low,range_y_up);

	}else if( !auto_x && auto_y){

		h2=new TH2FAuto(hname_dummy,htit);
		h2->FixBinningX(nx,xmin,xmax);

		if(is_range_lim_y) h2->SetRangeLimitsY(range_y_low,range_y_up);

	}else if( auto_x && !auto_y ){

		h2=new TH2FAuto(hname_dummy,htit);
		h2->FixBinningY(ny,ymin,ymax);

		if(is_range_lim_x) h2->SetRangeLimitsX(range_x_low,range_x_up);

	}else{

		h2=new TH2FAuto(hname_dummy,htit,nx,xmin,xmax,ny,ymin,ymax);
	}

	h2->SetName(hname);
	h2->GetXaxis()->SetTitle(htitx);
	h2->GetYaxis()->SetTitle(htity);

	//cout<<__FILE__<<"::"<<__FUNCTION__<<" ----> "<<hname<<endl;
	fHistoFld->Add(h2,name_x,name_y);
	//cout<<"---"<<endl;


}


void TSDataBin::fInitDataTree(){

	TString name="DataTree_"; name+=Name();

	fDataTree= new TTree(name,name);

	cout<<fDataTree<<endl;

	fDataTree->Print();

	cout<<"---"<<endl;

}

void TSDataBin::fInitHistoFld(){

	TString name="HistoFld_"; name+=Name();
	fHistoFld = new TSHistogramFld(name);

}


