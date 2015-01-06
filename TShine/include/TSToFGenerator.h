/*
 * TSToFGenerator.h
 *
 *  Created on: Jun 16, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 *  Momentum is in GeV/c, Mass is in GeV/c^2
 *  Lengths are in m
 *  Times in ns
 *
 *  Time resolution is the Pdf of the smearing
 *  to be added to the calculated ToF
 *
 *  ToF offset is a time bias to be added to the ToF
 *  calculated from the kinematics (mass,mom,path len)
 *
 *  Momentum Resolution is the expression
 *  of dP/P as a function of the P (second order poly)
 *
 *  Path Length Resolution is the sigma
 *  to be used for a Guassian smearing
 *
 *
 *
 *  Typical Parametrizations for parameters of Gauss shaped
 *  resolution at a given momentum:
 *
 * N.B.: If a user def function of the mass2 distribution is provided
 * the first param must be the mean of the distrib.
 * An option exists to provide the number of the parameter that
 * can represents the width of the distribution.
 * This will be used to auto set the limits in m2 over
 * a wide range of momenta
 * (and also for the width scaling as a function of the mass)
 * if no param number is specified the RMS of the function
 * will be calculated numerically using the CentralMoment
 * method of TF1
 *
 *
 *
 *
 * proton: 0
 * kaon: 1
 * pion: 2
 * electron: 3
 *
  double  m2_m_p0[4];
  double  m2_m_p1[4];

  double  m2_s_p0[4];
  double  m2_s_p1[4];
  double  m2_s_p2[4];


  Mean-Parameter linear parametrization:

  m2_m_p0[0]= 0.8876;
  m2_m_p1[0]= 0;

  m2_m_p0[1]= mass_k*mass_k;
  m2_m_p1[1]= 0;

  m2_m_p0[2]= 0.0167222;
  m2_m_p1[2]= 0;

  m2_m_p0[3]= 0.;
  m2_m_p1[3]= 0.;


 Width-Parameter parabolic parametrization

  //p
  m2_s_p0[0]= 0.0295321;
  m2_s_p1[0]= -0.00630375;
  m2_s_p2[0]= 0.00799005;

  //k
  m2_s_p0[1]= 0.00557491;
  m2_s_p1[1]= -0.00219247;
  m2_s_p2[1]= 0.00820526;

  //pi
  m2_s_p0[2]= 0.00557491;
  m2_s_p1[2]= -0.00219247;
  m2_s_p2[2]= 0.00820526;

  //e
  m2_s_p0[3]= 0.00557491;
  m2_s_p1[3]= -0.00219247;
  m2_s_p2[3]= 0.00820526;

 *
 *
 *
 * TODO:
 * TODO: ToFGenerator
 * TODO: copy ctor for bothe the classes
 * TODO:
 *
 *
 *   //example on how to set a Landau shaped pdf for the m2 distrib
 *   and set the individual param parametrization as function of the momentum
 *   TF1 *fLan=new TF1("fLan","landau",-1,100);
 *   fLan->SetParNames("a","m","w");
 *
 *   tofGen.SetMass2DistributionFunction("part",*fLan,2);
 *
 *   tofGen.SetParamFunction("part", 0 , "linear", m2_m_p0[0],m2_m_p1[0]);
 *   tofGen.SetParamFunction("part", 1 , "parab", m2_s_p0[0],m2_s_p1[0],m2_s_p2[0]);
 *   tofGen.SetParamFunction("part", 2 , "parab", m2_s_p0[2],m2_s_p1[2],3*m2_s_p2[2]);
 *
 */

#ifndef TSTOFGENERATOR_H_
#define TSTOFGENERATOR_H_

#include <string>
#include <vector>
#include <map>

#include <TString.h>
#include <TF1.h>
#include <TMath.h>

#include "TSNamed.h"

#include "TSVariable.h"


class TSToF_Part;



class TSToFGenerator: public TSNamed {


public:

	TSToFGenerator(TString);
	TSToFGenerator(TString, const TSVariable &vx, const TSVariable &vy);

	virtual ~TSToFGenerator();

	double ComputeDm2(double tof, double mom, double len) const;
	double ComputeDm2dp(double tof, double mom, double len) const;
	double ComputeDm2dt(double tof, double mom, double len) const;
	double ComputeDm2dL(double tof, double mom, double len) const;
	double ComputeMass(double tof, double mom, double len) const;
	double ComputeToF(double mass, double mom, double len) const;
	double GenerateMass2(TString part, double mom) const;
	double GenerateMass2(double mass, double mom, double len) const;
	TF1*  GetMass2Distrib(TString part, double mom) const;
	int   GetMass2DistribNPars(TString part) const;
	TF1* GetMomResolFunc() const {return fMomResol;}
	TF1* GetParametrization(TString name, int par) const;
	double GetParamValue(TString name, int par, double p) const;
	double GetSigmaMom(double) const;
	double GetSigmaPathLen() const {return fSigmaLen;}
	double GetSigmaToF() const {return fSigmaToF;}
	double GetToFOffset() const {return fToFT0;}
	TF1* GetToFResolDistr() const {return fToFResolDistr;}

	void Init();
	TF1*  ImportMass2Distrib(TString part,double mom) const;

	void SetParticle(TString name, TString label, float mass=0);

	void SetMass2DistributionFunction(TString name, TF1&, int width_par=-1);
	void SetMass2DistributionFunction(TString name, TString fname,int width_par=-1);
	void SetMass2MeanFunction(TString name, TF1&);
	void SetMass2MeanFunction(TString name, TString poly_name, double, double=0, double=0);
	void SetMass2WidthFunction(TString name, TF1&);
	void SetMass2WidthFunction(TString name, TString poly_name, double, double=0, double=0);
	void SetMass2WidthScaling(TString part1, TString part2);
	void SetParamFunction(TString, int par, TF1&);
	void SetParamFunction(TString name, int par, TString poly_name, double, double=0, double=0);

	void SetMomentumVariable(const TSVariable &);
	void SetMomentumResolution(double,double =0,double =0); // dP/P
	void SetPathLengthResolution(double); //m
	void SetMomentumRange(double min, double max);
	void SetToFResolution(double); // ps
	void SetToFResolution(TF1&);
	void SetToFVariable(const TSVariable &);
	void SetToFRange(double, double);
	void SetToFOffset(double); //ps applied only to mass2 generation not to compute ToF


private:

	TSToF_Part* fGetParticle(TString) const;

	std::map<TString,TSToF_Part*> fPartList;

    TF1* fToFResolDistr;
    TF1* fMomResol;

	double fToFT0;
	double fSigmaToF;
	double fSigmaLen;

	TSVariable fVariableX;
	TSVariable fVariableY;

	double CL; // c light in m/ns

};










class TSToF_Part{



public:
	TSToF_Part(TString name, TString label, float mass=0);
	~TSToF_Part();

	TString GetLabel() const {return Label;}
	float GetMass() const {return Mass;}
	TF1* GetMass2Distrib() const {return fMass2Distrib;}
	TF1* GetMass2Distrib(double mom) const;
	TString GetMass2DistribName() const {return fMass2DistribName;}
	TString GetName() const {return Name;}
	TF1* GetParamFunction(int) const;
	double GetParamValue(int par, double p) const;
    int GetNpar() const {return fMass2Distrib==0 ? 0 : fMass2Distrib->GetNpar();}
    int GetNumOfParFuncs() const {return fParamFunc.size();}
    double GetMassScale() const {return fMassScale;}
    double GetMassScaling(int, double) const;
    int GetWidthParam() const {return fWidthPar;}
    bool Mass2IsGaussian() const {return fMass2IsGauss;}
	void SetMass(float m) {Mass=m;}
	void SetMassScale(float m) {fMassScale=m; SetUseMassScale(true);}
	void SetMass2Distrib(TString fname, double xmin, double xmax, int width_par=-1);
	void SetMass2Distrib(TF1&, int width_par=-1);

	void SetMomentumVariable(const TSVariable &);
	void SetToFVariable(const TSVariable &);
	void SetParamFunc(int, TF1 &);
	void SetUseMassScale(bool b) {fUseMassScale=b;}
	void SetWidthPar(int p) {fWidthPar=p;}
	bool UseMassScale() const {return fUseMassScale;}

private:

	TString Name;
	TString Label;
	float Mass;

	bool fUseMassScale;
	float fMassScale;

	TF1* fMass2Distrib;
	std::map<int,TF1*> fParamFunc;

	TString fMass2DistribName;
	bool fMass2IsGauss;

	TSVariable fVariableX;
	TSVariable fVariableY;

	int fWidthPar;
	int fNpx;

};


#endif /* TSTOFGENERATOR_H_ */
