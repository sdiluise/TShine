/*
 * TSParticleFld.cc
 *
 *  Created on: Aug 27, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */


#include <TMath.h>

#include <TRandom.h>

#include "TSParticleFld.h"

//PDG code
int TSParticle::PDGP = 2212;
int TSParticle::PDGN = 2112;
int TSParticle::PDGDe = 1011;
int TSParticle::PDGPiPlus = 211;
int TSParticle::PDGPiMinus = -211;
int TSParticle::PDGPi0 = 111;
int TSParticle::PDGKPlus = 321;
int TSParticle::PDGKMinus = -321;
int TSParticle::PDGK0 = 311;
int TSParticle::PDGK0S = 310;
int TSParticle::PDGK0L = 130;
int TSParticle::PDGEle = 11;
int TSParticle::PDGPosit = -11;
int TSParticle::PDGMuPlus = 13;
int TSParticle::PDGMuMinus = -13;

//G4 code
int TSParticle::IdP = 14;
int TSParticle::IdN = 13;
int TSParticle::IdDe = 45;
int TSParticle::IdPiPlus = 8;
int TSParticle::IdPiMinus = 9;
int TSParticle::IdPi0 = 7;
int TSParticle::IdKPlus = 11;
int TSParticle::IdKMinus = 12;
int TSParticle::IdK0 = 10;
int TSParticle::IdK0S = 16;
int TSParticle::IdK0L = 10;
int TSParticle::IdEle = 3;
int TSParticle::IdPosit = 2;
int TSParticle::IdMuPlus = 5;
int TSParticle::IdMuMinus = 6;

// [GeV]
double TSParticle::MassP=0.938272;
double TSParticle::MassN=0.939565;
double TSParticle::MassDe=1.8756;
double TSParticle::MassPiPlus=0.139570;
double TSParticle::MassPiMinus=0.139570;
double TSParticle::MassPi0=0.134976;
double TSParticle::MassKPlus=0.493667;
double TSParticle::MassKMinus=0.493667;
double TSParticle::MassK0=0.497648;
double TSParticle::MassK0S=0.497648;
double TSParticle::MassK0L=0.497648;
double TSParticle::MassEle=0.510998e-3;
double TSParticle::MassPosit=0.510998e-3;
double TSParticle::MassMuPlus=0.105658;
double TSParticle::MassMuMinus=0.105658;

// [s]
double TSParticle::TauP=TMath::Infinity();
double TSParticle::TauN=881.5;
double TSParticle::TauDe=TMath::Infinity();
double TSParticle::TauPiPlus=2.6033*1e-8;
double TSParticle::TauPiMinus=2.6033*1e-8;
double TSParticle::TauPi0=0;//8.4*1e-17;
double TSParticle::TauKPlus=1.2380*1e-8;
double TSParticle::TauKMinus=1.2380*1e-8;
double TSParticle::TauK0=5.116*1e-8;
double TSParticle::TauK0S=0.8953*1e-10;
double TSParticle::TauK0L=5.116*1e-8;
double TSParticle::TauEle=TMath::Infinity();
double TSParticle::TauPosit=TMath::Infinity();
double TSParticle::TauMuPlus=2.197*1e-6;
double TSParticle::TauMuMinus=2.197*1e-6;




TSParticle::TSParticle(TString name,TString label,double mass, double charge, double tau)
:TSNamed(name,label,label)
{

	Init();

	SetCharge(charge);
	SetMass(mass);
	SetTau(tau);

}


void TSParticle::Init() {


	SetMass(0);
	SetCharge(0);
	SetTau(0);

	mev=1;
	gev=1e-3;


	lMname="m";
	lM2name="m^{2}";

	unitMeV="MeV";
	unitGeV="GeV";

	unitMeV2="MeV^{2}";
	unitGeV2="GeV^{2}";

	unitMassMeV="MeV/c^{2}";
	unitMass2MeV="MeV^{2}/c^{4}";
	unitMassGeV="GeV/c^{2}";
	unitMass2GeV="GeV^{2}/c^{4}";

	lQname="q";

	lQpos=lQname+">0";
	lQneg=lQname+"<0";
	lQzero=lQname+"=0";

	SetUnitGeV();


}

void TSParticle::Print() const{

	std::cout<<Name()<<" "<<Label()<<" "<<LabelQ()<<" "<<LabelQSign()<<" "<<LabelMass()<<" "<<LabelMass2()<<endl;
}





void TSParticle::SetCharge(int q) {

	fQ=q;

	lQ=lQname; lQ+=":";
	if(q>0) lQ+="+";
	lQ+=q;

	if(q==0) lQsign=lQzero;
	else if(q<0) lQsign=lQneg;
	else lQsign=lQpos;

}


void TSParticle::SetMass(double m) {

	fM=m;
	fM2=m*m;

	fMMeV=m*mev;
	fM2MeV=m*mev*mev;

	fMGeV=m*gev;
	fM2GeV=m*gev*gev;

	/*
	TString lM;    //  default
		 TString lM2;   // default
		 TString lMGeV; // m= ... GeV/c2
		 TString lMMeV; // m= ... MeV/c2
		 TString lM2GeV; // m = ... GeV^2/c4
		 TString lM2MeV; // m = ... MeV^2/c4
*/


	lM=STR_UTILS::Name(lMname,"=",STR_UTILS::ToString(fM)," ",unitMass);
	lM2=STR_UTILS::Name(lM2name,"=",STR_UTILS::ToString(fM2)," ",unitMass2);

	lMMeV=STR_UTILS::Name(lMname,"=",STR_UTILS::ToString(fM*mev)," ",unitMassMeV);
	lM2MeV=STR_UTILS::Name(lM2name,"=",STR_UTILS::ToString(fM2*mev*mev)," ",unitMass2MeV);

	lMGeV=STR_UTILS::Name(lMname,"=",STR_UTILS::ToString(fM*gev)," ",unitMassGeV);
	lM2GeV=STR_UTILS::Name(lM2name,"=",STR_UTILS::ToString(fM2*gev*gev)," ",unitMass2GeV);

}



double TSParticle::GenerateDecayLength(double P)  {

	/**
	 * Not const function: TRandom functions are not const
	 *
	 */

	/**
	 * Tlab = gamma*tau
	 * Llab = beta*c*Tlab= beta*c*gamma*tau
	 *
	 * probability to survive for a time t
	 * P(t)=Exp(-t/gamma*tau)= Exp(-t/Tlab) //
	 * Normalized is   (1/gamma*tau)*P(t) = N*P(t)
	 *
	 * L=beta*c*t
	 * t=L/beta*c
	 * P(L)= N*P(t(L))*dt/dL = N*Exp(-L/(beta*c*gamma*tau))*(1/beta*c)
	 * 						 = (1/beta*c*gamma*tau)*Exp(-L/beta*c*gamma*tau);
	 *                       = (1/Llab)*Exp(-L/Llab) //
	 *
	 */


	  double Llab= GetDecayLength(P);

	  double Lrand= rand.Exp(Llab); //TRandom::Exp(tau) = exp( -t/tau );

	  //cout<<Llab<<" "<<Lrand<<endl;

	  return Lrand;

}

double TSParticle::GetBetaGamma(double P) const {

	return P/MGeV();

}


double TSParticle::GetDecayLength(double P) const{


	// L(P)=beta*gamma*c*tau;

	//cout<<P<<" "<<GetBetaGamma(P)<<" "<<GetCTau()<<endl;

	return GetBetaGamma(P)*GetCTau();

}


double TSParticle::GetE(double P) const {

	return TMath::Sqrt(MGeV()*MGeV() + P*P);

}

double TSParticle::GetGamma(double P) const {

	if( MGeV()==0 ) return TMath::Infinity();

	return GetE(P)/MGeV();
}

double TSParticle::GetBeta(double P) const {

	if( GetE(P)==0 ) return TMath::Infinity();

	return P/GetE(P);
}


double TSParticle::GetDecayTime(double P) const {

	return GetGamma(P)*GetTau();
}



double TSParticle::GetTau() const {

	return fTau;

}

double TSParticle::GetCTau() const {

	return TMath::C()*GetTau();

}


void TSParticle::SetTau(double t) {

	fTau=t;
}


void TSParticle::SetUnitMeV() {

	mev=1;
	gev=1e-3;

	unitM=unitMeV;
	unitM2=unitMeV2;
	unitMass=unitMassMeV;
	unitMass2=unitMass2MeV;

	if(IsUnitGeV()) SetMass(M()/gev);

	unitIsMeV=true;
}

void TSParticle::SetUnitGeV() {

	mev=1e3;
	gev=1;


	unitM=unitGeV;
	unitM2=unitGeV2;
	unitMass=unitMassGeV;
	unitMass2=unitMass2GeV;

	if(IsUnitMeV()) SetMass(M()/mev);

	unitIsGeV=true;
}




//
// static function
//


TString TSParticle::GetReactionForm(const TSArgList &initial,TString str_to,const TSArgList &final){

	TString reac="";

	for(int i=0; i<initial.GetSize();++i){

		const TSParticle *p= (TSParticle*)initial.At(i);

		//cout<<"i="<<i<<endl;
		//p->Print();

		if(i!=0) reac+="+";
		reac+=p->Label();

	}

	reac+=str_to;

	for(int i=0; i<final.GetSize();++i){

		TSParticle *p= (TSParticle*)final.At(i);

		if(i!=0) reac+="+";
		reac+=p->Label();

	}



	return reac;

}



//
//
//
//
//
//



TSParticleFld::TSParticleFld() {


}

TSParticleFld::~TSParticleFld() {

}

