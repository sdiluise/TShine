/*
 * TSBetheBlochMgr.cc
 *
 *  Created on: Jul 14, 2014
 *
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */


#include "TSBetheBlochMgr.h"



TSBetheBlochMgr::TSBetheBlochMgr(TString name, TString title, double (*fcn)(double*, double*), double xmin, double xmax, int npar)
:TSBetheBlochFunc(name,title,fcn,xmin,xmax,npar)
{

 Init();

}

TSBetheBlochMgr::TSBetheBlochMgr(TString name, TString title, TString table_file)
:TSBetheBlochFunc(name,title,table_file)
{

 Init();

}


TSBetheBlochMgr::~TSBetheBlochMgr() {
    // TODO Auto-generated destructor stub


}


Double_t TSBetheBlochMgr::Eval(Double_t momentum, Double_t mass, Double_t z, Double_t t) const{


	if( !HasPhaseSpaceMap() ){

		if( fUseDefaultBB ){
			return TSBetheBlochFunc::Eval(momentum,mass,z,t);
		}else{
			return 0;
		}

	}else{

		int volume = fPSMap->FindVolume();

		if( volume == 0 ){

			if( fUseDefaultBB ){
				return TSBetheBlochFunc::Eval(momentum,mass,z,t);
			}else{
				return 0;
			}

		}else{

			//int bb_id = fGetVolumeBBId(volume);

			TSBetheBlochFunc *fbb = GetVolumeBetheBlochFunc(volume);

			if( fbb ){

				return fbb->Eval(momentum,mass,z,t);

			}else{

				if( fUseDefaultBB ){
					return TSBetheBlochFunc::Eval(momentum,mass,z,t);
				}else{
					MSG::ERROR(__FILE__,"::",__FUNCTION__," BB function not found for volume ",volume);
				}

			}

		}

	}





    return 0;

}


TSBetheBlochFunc* TSBetheBlochMgr::GetVolumeBetheBlochFunc(int vol) const{

	int bb_id = fGetVolumeBBId(vol);

	if( bb_id && fBBList.count(bb_id) ){

		return fBBList.find(bb_id)->second;

	}

	return 0;
}


int TSBetheBlochMgr::fGetVolumeBBId(int vol) const {

	if( fBBMap.count(vol) ){

		return fBBMap.find(vol)->second;
	}

	return 0;
}


void TSBetheBlochMgr::Init() {

	fPSMap = 0;

	UseDefaultBetheBloch(true);

}


void TSBetheBlochMgr::SetFunctionToSelection(TSBetheBlochFunc &fbb) {

	if( !HasPhaseSpaceMap() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," no phase space set");
		return;
	}


	for(int ivol=1; ivol<= fPSMap->GetNumOfVolumes(); ++ivol){

		if( fPSMap->VolumeIsSelected(ivol) ){

			int NFBB = fBBList.size();

			fBBList[NFBB+1] = &fbb;

			fBBMap[ivol]=NFBB+1;
		}

	}



}


void TSBetheBlochMgr::SetPhaseSpaceMap(const TSPhaseSpaceMap &map){

	delete fPSMap;

	fPSMap = new TSPhaseSpaceMap(map);

	fHasPSMap = true;
}


