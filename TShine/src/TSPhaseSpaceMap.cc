/*
 * TSPhaseSpaceMap.cc
 *
 *  Created on: May 28, 2014
 *      Author: sdiluise
 */
/**
 *
 *
 *
 *
 *
 *
 *
 *
 */
#include <fstream>
#include <string>

#include <TMath.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH2Poly.h>

#include "TSPhaseSpaceMap.h"

#include "StringUtils.h"
#include "RootUtils.h"
#include "MessageMgr.h"


using std::cout;
using std::endl;


TSPhaseSpaceMap::TSPhaseSpaceMap() {

	Init();
}

TSPhaseSpaceMap::TSPhaseSpaceMap(TString name, TString title)
:TSNamed(name,"",title)
{

	Init();
}

TSPhaseSpaceMap::TSPhaseSpaceMap(const TSPhaseSpaceMap& other)
{

	Init();

	SetName(other.Name());
	SetTitle(other.Title());

	for(int i=1; i<=other.GetDimension(); ++i){

		SetVariable( *other.GetVariable(i) );

	}


	Add( other );

}


TSPhaseSpaceMap::~TSPhaseSpaceMap() {
	// TODO Auto-generated destructor stub


  // Delete all volumes
  for(std::map<int,TSPhaseSpaceVolume*>::iterator it=fVolumeList.begin(), end=fVolumeList.end(); it!=end; ++it){

	  delete it->second;
  }

  //Delete Histograms
  //TODO:


}


int TSPhaseSpaceMap::Add(const TSPhaseSpaceMap& second) {


	/**
	 *
	 *  Add volumes of a second map to this map.
	 *
	 *  Axis variables and ordering must be the same between
	 *  the two maps.
	 *
	 *
	 */

	if( GetDimension() != second.GetDimension() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," different dimensions");
		MSG::ERROR(" Maps: ",Name()," ",second.Name());
		MSG::ERROR(" Dims: ",GetDimension()," ",second.GetDimension());
		return 0;
	}


	for( int i=1; i<= GetDimension(); ++i){

		if( GetVariable(i) != second.GetVariable(i) ){
			MSG::ERROR(__FILE__,"::",__FUNCTION__,": different set of variables");
			MSG::ERROR(" Map1 ",Name()," axis: ",i," var: ", GetVariable(i)->Name());
			MSG::ERROR(" Map2 ",second.Name()," axis: ",i," var: ", second.GetVariable(i)->Name());
			return 0;
		}
	}



	for( int i=1; i<=second.GetNumOfVolumes();++i){

		TSPhaseSpaceVolume *v= new TSPhaseSpaceVolume( *(second.GetVolume(i)) );
		AddVolume( *v );

	}



	return 1;

}


int TSPhaseSpaceMap::AddVolume(TSPhaseSpaceVolume& vol) {



	if( GetDimension() != vol.GetDimension() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__,": Volume and Map have different dimensions ");
		MSG::LOG(" ... map dim: ",GetDimension()," volume dim: ",vol.GetDimension());
		return 0;
	}


    for(int i=1; i<=GetDimension(); ++i){

    	if( GetVariable(i) != vol.GetVariable(i) ){
    		MSG::ERROR(__FILE__,"::",__FUNCTION__,": Volume and Map have different axis variables or ordering");
    		return 0;
    	}


    	if( !IsBinned(i) ){
    		//cout<<"First Binning for: "<<GetVariable(i)->GetName()<<" in map: "<<GetName()<<endl;

    		fVarIsBinned[i]=true;
    	}

    }


	fNumOfVols++;

	fVolumeList[fNumOfVols]=&vol;
	fVolIsSelected[fNumOfVols]=false;

	BuildHistograms();


	return 1;

}



int TSPhaseSpaceMap::BuildHistograms(int opt) {


	int DIM=GetDimension();

	int NVOLS=GetNumOfVolumes();

	//cout<<"DIM: "<<DIM<<" NOLVS "<<NVOLS<<endl;


	//for(int vol=1; vol<=NVOLS; ++vol){
	// GetVolume(vol)->Print();
	//}

	int NH=0;

    fHistoFld.Clear();

	for(int ax1=1; ax1<=DIM; ++ax1){

		for(int ax2=ax1+1; ax2<=DIM; ++ax2){

			NH++;
			TString vname1=GetVariable(ax1)->Name();
			TString vname2=GetVariable(ax2)->Name();

			TString h2name=vname1; h2name+="_"; h2name+=vname2;
			//cout<<NH<<" x: "<<ax1<<" y: "<<ax2<<" var x : "<<vname1<<" var y:"<<vname2<<endl;


			double ranges[4];

			std::vector<TGraph*> glist;


			//loop over volumes

			for(int vol=1; vol<=NVOLS; ++vol){


				if( !(GetVolume(vol)->GetVariable(ax1) && GetVolume(vol)->GetVariable(ax2)) ){

					MSG::WARNING(__FILE__,"::",__FUNCTION__," volume ",vol," not totally built:");
					MSG::WARNING(" axis: ",vname1, " or axis ", vname2, " is missing");

					continue;
				}

				double xmin=GetVolume(vol)->GetLowEdge(ax1);
				double xmax=GetVolume(vol)->GetUpEdge(ax1);

				double ymin=GetVolume(vol)->GetLowEdge(ax2);
				double ymax=GetVolume(vol)->GetUpEdge(ax2);

                //cout<<"Map "<<GetName()<<" volume: "<<vol<<" ["<<xmin<<", "<<xmax<<"]"<<" "<<"["<<ymin<<", "<<ymax<<"]"<<endl;


                if(glist.size()==0){
                	ranges[0]=xmin;
                	ranges[1]=xmax;
                	ranges[2]=ymin;
                	ranges[3]=xmax;
                }else{
                	ranges[0]=TMath::Min(ranges[0],xmin);
                	ranges[1]=TMath::Max(ranges[1],xmax);
                	ranges[2]=TMath::Min(ranges[2],ymin);
                	ranges[3]=TMath::Max(ranges[3],ymax);
                }

                //Build Rectangular TGraph Bin
                TGraph *g = RootUtils::BuildGraph(Form("bin_%d",vol),Form("bin_%d",vol),xmin,xmax,ymin,ymax);

                //Skip if already present
                //or overlapping with existing bins

                bool is_new = true;

                for(int ig=0; ig<glist.size(); ++ig){

                	if( RootUtils::CompareGraphs(*glist.at(ig),*g) ){
                		is_new=false;
                		delete g;
                		break;
                	}

                }

                if( is_new ){

                	bool is_ovl=false;

                	for(int ig=0; ig<glist.size(); ++ig){
                		if( RootUtils::OverlappingRectangulars(*glist.at(ig),*g) ){
                			is_ovl=true;
                			MSG::WARNING(__FILE__,"::",__FUNCTION__," Overlapping Bins");
                			cout<<" Variables; x: "<<vname1<<" y: "<<vname2<<endl;
                			cout<<" Bin "<<vol<<" x: ["<<xmin<<", "<<xmax<<"] y: ["<<ymin<<", "<<ymax<<"]"<<endl;
                			cout<<" ->> overlaps with bin: "<<ig+1<<endl;
                			for(int ip=0; ip<glist.at(ig)->GetN();++ip){
                				cout<<" x: "<<(glist.at(ig)->GetX())[ip]<<" y: "<<(glist.at(ig)->GetY())[ip]<<endl;
                			}
                		}
                	}

                	glist.push_back(g);
                }




			}//vols

			//increase histo range by 10% each side
			ranges[0] -= 0.1*(ranges[1]-ranges[0]);
			ranges[1] += 0.1*(ranges[1]-ranges[0]);
			ranges[2] -= 0.1*(ranges[3]-ranges[2]);
			ranges[3] += 0.1*(ranges[3]-ranges[2]);


			h2name= STR_UTILS::Name(GetName(),"_",h2name);

			TH2Poly *poly = new TH2Poly(h2name,"",ranges[0],ranges[1],ranges[2],ranges[3]);

			for(int ig=0; ig<glist.size();++ig){
				poly->AddBin(glist.at(ig));
			}

			poly->Draw();
			poly->SetTitle(GetTitle());
			poly->GetXaxis()->SetTitleOffset(0.5);
			poly->GetYaxis()->SetTitleOffset(0.5);
			poly->GetXaxis()->SetTitle( GetVariable(ax1)->GetAxisTitle() );
			poly->GetYaxis()->SetTitle( GetVariable(ax2)->GetAxisTitle() );
			poly->Draw();


			//poly->Write();

			fHistoFld.Add(poly, vname1, vname2);

		}//axis2
	}//axis1



}


TSPhaseSpaceMap* TSPhaseSpaceMap::Clone(TString name, TString title){


	TSPhaseSpaceMap* map= new TSPhaseSpaceMap( *this );

	map->SetName(name);
	map->SetTitle(title);

	return map;

}



int TSPhaseSpaceMap::Fill(){

	//to be overridden in derived class
	return 0;

}


int TSPhaseSpaceMap::FindVolume() const{


	for(int idim=1; idim<=fDimension; ++idim){
		if( ! IsBinned(idim) ){
			TString name=GetVariable(idim)->Name();
			MSG::WARNING(__FILE__,"::",__FUNCTION__," axis of variable: ",name," is not binned");
			return 0;
		}
	}


	for(int ivol=1; ivol<=fNumOfVols; ++ivol){


		int is_in = GetVolume(ivol)->IsInside();

		if( is_in ) return ivol;

	}

	return 0;
}



int TSPhaseSpaceMap::GetAxis(TString name) const{


	for(int i=1; i<=fVarList.size(); ++i){

		if( ( fVarList.find(i)->second )->Name()==name  ){

			return i;
		}

	}

	return 0;
}



TSHistogramFld* TSPhaseSpaceMap::GetHistogramFld(TString opt) const {


	return fHistoFld.Clone( opt );

}




const TSVariable* TSPhaseSpaceMap::GetVariable(int axis) const{

	if(axis <=0 || axis> GetDimension() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," out of dimension");
		return 0;
	}

	if( fVarList.count(axis)==1){

		return (fVarList.find(axis))->second;

	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," variable not set");
		return 0;
	}

}


const TSVariable* TSPhaseSpaceMap::GetVariable(TString name) const{


	for(int i=1; i<=fVarList.size(); ++i){


		if( ( fVarList.find(i)->second )->Name()==name  ){

			return GetVariable(i);

		}


	}

	return 0;
}

TSPhaseSpaceVolume* TSPhaseSpaceMap::GetVolume(int num) const{


	if( fVolumeList.count(num)==1){

		return (fVolumeList.find(num))->second;

	}else{
		MSG::ERROR(__FILE__,"::",__FUNCTION__," num: ",num," is not present");
		return 0;
	}


}

void TSPhaseSpaceMap::Init(){

	fMapOK=0;
	fDimension=0;
	fNumOfVols=0;
	fHasSelection=false;

	TString fld_name="HistFld_"; fld_name+=Name();
    fHistoFld.SetName(fld_name);

}


int TSPhaseSpaceMap::IsBinned(int axis) const{

	if(fVarIsBinned.count(axis)==1){

		return fVarIsBinned.find(axis)->second;

	}else{

		return 0;
	}

}



int TSPhaseSpaceMap::IsBinned(TString name) const{

	int axis=GetAxis(name);

	if( axis ){

		return fVarIsBinned.find(axis)->second;

	}else{

		return 0;
	}

}


int TSPhaseSpaceMap::Multiply(const TSPhaseSpaceMap& second) {

	/**
	 *
	 *  Multiply this map by a second Map
	 *
	 *  New Map Dimensions is the sum of the two dimensions
	 *  New Map Num. of Volumes is the product of the two Num. of Volumes
	 *
	 *  DIM2: Dimension of second map
	 *  NVOL2: NumOfVolumes of second map
	 *
	 *  For each existing volume of this map, NVOL2 new volumes are created
	 *  by adding to the this volume the NVOL2 volumes of the second map
	 *
	 *
	 */




 if(GetNumOfVolumes()==0 || second.GetNumOfVolumes()==0){
	 MSG::ERROR(__FILE__," ",__FUNCTION__," a map is empty ");
	 return 0;
 }


 int DIM1 = GetDimension();
 int DIM2 = second.GetDimension();

 // check no duplicate axis

 for(int axis1=1; axis1<=DIM1; ++axis1){
	 for(int axis2=1; axis2<=DIM2; ++axis2){

		 if( IsBinned(axis1) && GetVariable(axis1) == second.GetVariable(axis2) ){
			 MSG::ERROR(__FILE__,"::",__FUNCTION__,": two binned axis with same variable: ", GetVariable(axis1)->Name());
			 return 0;
		 }

	 }
 }

 //cout<<"multiply: "<<GetName()<<" X "<<second.GetName()<<endl;

 for(int axis1=1; axis1<=DIM1; ++axis1){

	 if( IsBinned(axis1) ) continue;

	 cout<<"not Binned in this map: axis "<<axis1<<" "<<GetVariable(axis1)->Name()<<endl;

 }

 //for(int axis2=1; axis2<=DIM2; ++axis2){
 //	cout<<"second map axis: "<<axis2<<" "<<second.GetVariable(axis2)->GetName()<<" IsBinned: "<<second.IsBinned(axis2)<<endl;
 //}




 int DIM=DIM1+DIM2;

 int NVOL1 = GetNumOfVolumes();
 int NVOL2 = second.GetNumOfVolumes();


 std::vector<TSPhaseSpaceVolume*> temp_vol_list;


 for(int vol1=1; vol1<=NVOL1; ++vol1){


	 TSPhaseSpaceVolume *v1= GetVolume(vol1);


	 //Temp. copy to create new volumes
	 TSPhaseSpaceVolume *v0=new TSPhaseSpaceVolume(*v1);
	 temp_vol_list.push_back(v0);

	 TSPhaseSpaceVolume *v2= second.GetVolume(1);

	 v1->Add(*v2);

 }//


 for(int vol1=1; vol1<=NVOL1; ++vol1){

	 TSPhaseSpaceVolume *v0=temp_vol_list.at(vol1-1);

	 for(int vol2=2; vol2<=NVOL2; ++vol2){


    	 TSPhaseSpaceVolume *vnew= new TSPhaseSpaceVolume(*v0);


    	 TSPhaseSpaceVolume *v2= second.GetVolume(vol2);


    	 vnew->Add(*v2);


    	 AddVolume(*vnew);


     }//vol2


 }//vol1


 for(int i=0; i<NVOL1; ++i){

	 delete temp_vol_list.at(i);

 }



 return 1;
}


void TSPhaseSpaceMap::Print() const{


	cout<<endl<<"==== PhaseSpaceMap "<<Name()<<" "<<Title()<<" ======"<<endl;

	cout<<"Dimension "<<GetDimension()<<endl;
	cout<<"Axis Variables "<<endl;

	for(int i=1; i<=GetDimension();++i){
		cout<<i<<" "<<GetVariable(i)->Name()<<endl;
	}

	cout<<endl<<"Number of Volumes: "<<GetNumOfVolumes()<<endl;

	for(int i=1; i<=GetNumOfVolumes();++i){
		cout<<"vol: "<<i<<" "<<GetVolume(i)->GetDescription()<<endl;
	}

	cout<<"==== PhaseSpaceMap: "<<Name()<<" ==== end ====="<<endl;


}



void TSPhaseSpaceMap::RemoveSelection(){

	fHasSelection=false;

	for(int ivol=1; ivol<=GetNumOfVolumes(); ++ivol){

		SetVolumeSelected(ivol,false);
	}


}



void TSPhaseSpaceMap::SelectRange(const TSVariable &var, double range_min, double range_max, bool partial){

 /**
 *
 *   Select all volumes for which the bin range of the variable var
 *   is completely or partially (option partial==true)
 *   in the interval [var_min,var_max[;
 *   default is partial=false;
 *
 *         |---|                  var axis range  completely inside
 *
 *                       |---|    var axis range partially inside
 *   |---------------------|
 *  range_min               range_max
 *
 *
 */

	int axis=GetAxis( var.Name() );

	if( !axis ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," variable ",var.Name(), " not part of the map");
		return;
	}


	if( range_min > range_max){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," variable ",var.Name(), " illegal range limits");
		return;
	}


	fHasSelection=true;

	for(int ivol=1; ivol<=GetNumOfVolumes(); ++ivol){

		//double axis_center=GetVolume(ivol)->GetCenter(axis);

		double axis_low =GetVolume(ivol)->GetLowEdge(axis);
		double axis_up =GetVolume(ivol)->GetUpEdge(axis);

        int low_in = GetVolume(ivol)->GetAxis(axis)->IsInside(range_min);
        int up_in = GetVolume(ivol)->GetAxis(axis)->IsInside(range_max);


        int inside= axis_low>=range_min && axis_up<range_max;

        bool is_sel;

        if(partial) is_sel= (low_in || up_in || inside);
        else is_sel=inside;

        if( is_sel ){

        	SetVolumeSelected(ivol,true);

        }

	}



	/*
	//Build Labels and Descriptions for the selection on the variable

	TSVariable var_sel(var);
	std::string name=var_sel.GetName(); name+="_sel";
	var_sel.SetName( name );
	var_sel.SetRange(range_min,range_max);

	cout<<var_sel.GetDescription()<<endl;
	*/


}


void TSPhaseSpaceMap::SetVolumeSelected(int ivol, bool active){

	/**
	 *
	 * Activate or Deactivate a specific volume number
	 *
	 *
	 */


	if(active) fHasSelection=true;

	if( fVolIsSelected.count(ivol)==1 ){

		fVolIsSelected[ivol]=active;
	}

}


bool TSPhaseSpaceMap::VolumeIsSelected(int ivol) const{

	if( !HasSelection() ){

		return true;
	}

	if( fVolIsSelected.count(ivol)==1 ){

		return fVolIsSelected.find(ivol)->second;

	}else{

		return false;
	}


}



int TSPhaseSpaceMap::SetBinning(TString var_names, TString file_name) {

	/**
	 *
	 *
	 *
	 *
	 *
	 *  Read Binning From text file.
	 *
	 *  a block where all the rows begin with a "#" will be skipped
	 *
	 *
	 *
	 *
	 */


	std::vector<TString> names = STR_UTILS::Tokenize(var_names,":");


	int N=names.size();


    std::map<int,const TSVariable*> var_list;


    for(int i=0;i<N;++i){

    	if( GetVariable(names[i]) ){

    		var_list[i+1]=GetVariable(names[i]);

    		cout<<names[i]<<" "<<var_list[i+1]->Name()<<endl;

    	}else{

    		MSG::ERROR(__FILE__,"::",__FUNCTION__," variable: ",names[i]," not set in map");
    		return 0;
    	}

    }


	fstream fin(file_name);

	if( !fin.good() ){
		MSG::ERROR(__FILE__," ",__FUNCTION__," File ",file_name," Not Valid");
		return 0;
	}



	TString var_name,off_tag,mult_tag;

	int nbins;
	double offset,bin_width;


	bool stop=false;

	int block=0;

	TSPhaseSpaceMap blocks_sum_map;
	for(int i=1;i<=N;++i){
		blocks_sum_map.SetVariable( *GetVariable( names[i-1] ) );
	}


	while(1){

	    //TSPhaseSpaceMap axis_map[N];
	    TSPhaseSpaceMap *axis_map[N];
	    for(int i=0;i<N;++i) axis_map[i]=new TSPhaseSpaceMap();

	    TSPhaseSpaceMap block_map;


		for(int axis=1; axis<=N; ++axis){

			fin>>var_name;

			if( !fin.good() ) { stop=true; break;}
			if( fin.eof() ) {stop=true; break;}

			fin>>off_tag>>offset>>nbins>>mult_tag>>bin_width;

			if(var_name.BeginsWith("#")){
				continue;
			}

			cout<<var_name<<" "<<off_tag<<" "<<offset<<" "<<nbins<<" "<<mult_tag<<" "<<bin_width<<endl;

			double xlow,xup;

			axis_map[axis-1]->SetVariable( *GetVariable(names[axis-1]) );

			block_map.SetVariable(*GetVariable(names[axis-1]));

			for(int bin=0; bin<nbins; ++bin){

				xlow=offset+bin_width*bin;
				xup=xlow+bin_width;

				cout<<bin+1<<": ["<<xlow<<", "<<xup<<"]"<<endl;

				TSPhaseSpaceVolume *vol=new TSPhaseSpaceVolume(1,"","");
				vol->SetAxis(1,*GetVariable(names[axis-1]),xlow,xup);

				axis_map[axis-1]->AddVolume(*vol);
			}

			//axis_map[axis-1]->Print();

			if(axis==1) block_map.Add(*axis_map[axis-1]);
			else block_map.Multiply(*axis_map[axis-1]);

		}


		//delete [] axis_map; //cling compiler
		for(int i=0;i<N;++i) delete axis_map[i];

		if(stop) break;


        block_map.Print();

        blocks_sum_map.Add(block_map);


		block++;


		if( !fin.good() ) break;
		if( fin.eof() ) break;
	}//


	//blocks_sum_map.Print();

	//blocks_sum_map.BuildHistograms();

	//Now add it to the existing map
	Add(blocks_sum_map);


	return 1;
}


int TSPhaseSpaceMap::SetBinning(const TSVariable& var, int n, double* bins) {

	fMapOK=0;

	 if(n<=0){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," for var ",var.Name()," num of bins <=0");
	    return 0;
	 }



	 TAxis ax(n,bins);

	 TSPhaseSpaceMap new_map(var.Name(),"");

	 new_map.SetVariable(var);


	 for(int i=1; i<=n; ++i){

		 TSPhaseSpaceVolume *vol=new TSPhaseSpaceVolume(1,"","");

		 vol->SetAxis(1, var, ax.GetBinLowEdge(i), ax.GetBinUpEdge(i));


		 new_map.AddVolume(*vol);
	 }


	 if(GetNumOfVolumes()==0){

		 this->Add(new_map);

	 }else{

		 this->Multiply(new_map);
	 }

	 fMapOK=1;

	 return fMapOK;

}



int TSPhaseSpaceMap::SetBinning(const TSVariable& var, int n, double low, double up) {

  fMapOK=0;

  if(n<=0){
	  MSG::ERROR(__FILE__,"::",__FUNCTION__," for var ",var.Name()," num of bins <=0");
	  return 0;
  }

  if(low>=up){
	  MSG::ERROR(__FILE__,"::",__FUNCTION__," for var ",var.Name()," illegal range");
	  return 0;
  }


  TAxis ax(n,low,up);

  double *xbins=new double[n+1];

  for(int i=0; i<=n; ++i){
	  xbins[i]=ax.GetBinLowEdge(i+1);
  }

  int fMapOK=SetBinning(var,n,xbins);


  delete [] xbins;


  return fMapOK;

}


int TSPhaseSpaceMap::SetVariable(const TSVariable& var) {

	for(int i=1;i<=fDimension;++i){

		if( fVarList[i]==&var ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," variable: ",var.Name()," already present: not added");
			return 0;
		}
	}

	fDimension++;
	fVarList[fDimension]=&var;
    fVarIsBinned[fDimension]=false;

	return 1;
}





