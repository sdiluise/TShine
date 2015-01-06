/*
 * TSParticleFld.h
 *
 *  Created on: Aug 27, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *	Mass default is [GeV]
 *
 *	Lengths in [m]
 *
 *  Times in [s]
 *
 *  The option SetUnitMeV will change only the
 *  mass values and labels, not the computation of
 *  the decay len etc where it is assumed that
 *  momentum and mass are in GeV
 *
 */

#ifndef TSPARTICLEFLD_H_
#define TSPARTICLEFLD_H_


#include <TString.h>
#include <TRandom.h>

#include "StringUtils.h"
#include "TSArgList.h"

#include "TSNamed.h"

/*
 *
 */

class TSParticle: public TSNamed{

	public:
	 TSParticle(TString name,TString label,double mass, double charge, double tau=0);

	 void Init();

	 int Q() const {return fQ;}

	 double M() const {return fM;}//default unit
	 double M2() const {return fM2;}//default unit

	 double MMeV() {return fMMeV;}
	 double M2MeV() {return fM2MeV;}

	 double MGeV() const {return fMGeV;}
	 double M2GeV() const {return fM2GeV;}

	 TString LabelQ() const {return lQ;}//q=-2
	 TString LabelQSign() const {return lQsign;}//q<0

	 TString LabelMassUnits() const { return unitMass;}
	 TString LabelMass2Units() const { return unitMass2;}

	 TString LabelMass() const {return lM;}//m=... default unit
	 TString LabelMass2() const {return lM2;}//m=... default unit

	 TString LabelMassMeV() const {return lMGeV;}//m= ... GeV/c2
	 TString LabelMassGeV() const {return lMMeV;}//m= ... MeV/c2

	 TString LabelMass2MeV() const {return lM2GeV;}//m = ... GeV^2/c4
	 TString LabelMass2GeV() const {return lM2MeV;}//m^2 = ... MeV^2/c4

	 bool IsUnitMeV() const {return unitIsMeV;}
	 bool IsUnitGeV() const {return unitIsGeV;}

	 void Print() const;

	 double ScaleToGeV() const {return gev;}
	 double ScaleToMeV() const {return mev;}

	 void SetCharge(int);
	 void SetMass(double);
	 void SetTau(double);//s
	 double GetBeta(double P) const;
	 double GetGamma(double P) const;
	 double GetBetaGamma(double) const;
	 double GetE(double P) const; //GeV
	 double GetTau() const;//s
	 double GetCTau() const;//m
	 double GetDecayTime(double P) const; //s
	 double GetDecayLength(double P) const;//P in GeV, Len in m
	 double GetToF(double P) const;//in s
	 double GenerateDecayLength(double P);//GeV, m

	 void SetUnitMeV();
	 void SetUnitGeV();

	 static TString GetReactionForm(const TSArgList &initial,TString,const TSArgList &final);

	 static double MassP;
	 static double MassN;
	 static double MassDe;
	 static double MassPiPlus;
	 static double MassPiMinus;
	 static double MassPi0;
	 static double MassKPlus;
	 static double MassKMinus;
	 static double MassK0;
	 static double MassK0S;
	 static double MassK0L;
	 static double MassEle;
	 static double MassPosit;
	 static double MassMuPlus;
	 static double MassMuMinus;


	 static double TauP;
	 static double TauN;
	 static double TauDe;
	 static double TauPiPlus;
	 static double TauPiMinus;
	 static double TauPi0;
	 static double TauKPlus;
	 static double TauKMinus;
	 static double TauK0;
	 static double TauK0S;
	 static double TauK0L;
	 static double TauEle;
	 static double TauPosit;
	 static double TauMuPlus;
	 static double TauMuMinus;



	 static int PDGP;
	 static int PDGN;
	 static int PDGDe;
	 static int PDGPiPlus;
	 static int PDGPiMinus;
	 static int PDGPi0;
	 static int PDGKPlus;
	 static int PDGKMinus;
	 static int PDGK0;
	 static int PDGK0S;
	 static int PDGK0L;
	 static int PDGEle;
	 static int PDGPosit;
	 static int PDGMuPlus;
	 static int PDGMuMinus;

	 static int IdP;
	 static int IdN;
	 static int IdDe;
	 static int IdPiPlus;
	 static int IdPiMinus;
	 static int IdPi0;
	 static int IdKPlus;
	 static int IdKMinus;
	 static int IdK0;
	 static int IdK0S;
	 static int IdK0L;
	 static int IdEle;
	 static int IdPosit;
	 static int IdMuPlus;
	 static int IdMuMinus;

	private:

	 //
	 //values
	 //
	 int fQ;
	 double fM;
	 double fM2;
	 double fMMeV;
	 double fM2MeV;
	 double fMGeV;
	 double fM2GeV;

	 double fTau;


	 //
	 // Utility Labels String
	 //
	 TString lM;    //  default
	 TString lM2;   // default
	 TString lMGeV; // m= ... GeV/c2
	 TString lMMeV; // m= ... MeV/c2
	 TString lM2GeV; // m = ... GeV^2/c4
	 TString lM2MeV; // m = ... MeV^2/c4

	 TString lQ; //q=-2
	 TString lQsign; //q<0


	 //

	 double mev;
	 double gev;

	 bool unitIsMeV;
	 bool unitIsGeV;

	 TString lMname;
	 TString lM2name;
	 TString unitM;
	 TString unitM2;
	 TString unitMass;
	 TString unitMass2;
	 TString unitMeV;
	 TString unitGeV;
	 TString unitMeV2;
	 TString unitGeV2;
	 TString unitMassMeV;
	 TString unitMass2MeV;
	 TString unitMassGeV;
	 TString unitMass2GeV;


	 TString lQname;
	 TString lQpos;
	 TString lQneg;
	 TString lQzero;

	 TRandom rand;


};




class TSParticleFld {


public:
	TSParticleFld();
	virtual ~TSParticleFld();
};

#endif /* TSPARTICLEFLD_H_ */
