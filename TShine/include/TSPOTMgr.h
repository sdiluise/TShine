/*
 * TSPOTMgr.h
 *
 *  Created on: Nov 8, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSPOTMGR_H_
#define TSPOTMGR_H_

#include <TMath.h>

#include "StringUtils.h"

#include "MessageMgr.h"

#include "TSNamed.h"


/*
 *
 */

class TSPOTMgr: public TSNamed {

public:
	TSPOTMgr() { Init(); }
	virtual ~TSPOTMgr(){};

	double GetNativePOTD() const;
	double GetPOTD() const;
	float GetPOT() const {return fPOT;}
	float GetNativePOT() const {return fPOTnative;}
	float GetNativePOTPower() const {return fPOTnative_pow;}
	float GetPOTPower() const {return fPOTpower;}
	float GetPOTScaling() const {return fPOTscaling;}
	TString GetPOTLabel() const {return fPOTlabel;}
	TString GetNativePOTLabel() const {return fPOTNativeLabel;}

	void Init();

	void SetNativePOT(float POT, float Power=20);
	void SetPOT(float POT, float Power=20);
	void Restore();

private:

	float fPOT;
	float fPOTnative;
	float fPOTpower;
	float fPOTnative_pow;
	float fPOTscaling;

	TString fPOTlabel;
	TString fPOTNativeLabel;

	inline void fBuildPOTNames();
};



inline double TSPOTMgr::GetPOTD() const
{

	double P = fPOT*TMath::Power(10,fPOTpower);

	return P;
}


inline double TSPOTMgr::GetNativePOTD() const
{

	double P = fPOTnative*TMath::Power(10,fPOTnative_pow);

	return P;
}

inline void TSPOTMgr::Init()
{


	SetNativePOT(1,20);//1e20

	fBuildPOTNames();
}

inline void TSPOTMgr::SetNativePOT(float POT, float Power)
{
	/**
	 *
	 *
	 */


	if(POT <= 0 || Power<=0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs ");
		MSG::ERROR("POT: ",POT,"e^",Power);
		return;
	}

	fPOTscaling = 1;

    fPOTnative = POT;
	fPOT = POT;

	fPOTpower = Power;
	fPOTnative_pow = fPOTpower;

	fBuildPOTNames();

}



inline void TSPOTMgr::Restore(){

	SetNativePOT( GetNativePOT(), GetNativePOTPower() );

}


inline void TSPOTMgr::SetPOT(float POT, float Power)
{


	if(POT <= 0 || Power <= 0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid inputs");
		return;
	}

	//scaling factor w.r.t. current normalization

	fPOTscaling = (POT/fPOT)*TMath::Power(10,Power-fPOTpower);

	fPOT = POT;
	fPOTpower = Power;

	fBuildPOTNames();

}



inline void TSPOTMgr::fBuildPOTNames()
{


	fPOTNativeLabel = "Native POT: ";
	fPOTNativeLabel += StringUtils::ToString(fPOTnative);
	fPOTNativeLabel +="x10^{"; fPOTNativeLabel+=fPOTpower; fPOTNativeLabel+="}";


	fPOTlabel = "POT: ";
	fPOTlabel += StringUtils::ToString(fPOT);
	fPOTlabel +="x10^{"; fPOTlabel+=fPOTpower; fPOTlabel+="}";


}

#endif /* TSPOTMGR_H_ */
