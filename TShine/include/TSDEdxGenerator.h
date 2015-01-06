/*
 * TSDEdxGenerator.h
 *
 *  Created on: Jun 14, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *  Generate Monte Carlo <dEdx> for a charged particle track
 *  af a given momentum.
 *
 *  The mean-<dEdx> must be calculated form a
 *  Bethe-Bloch function.
 *
 *  The spread around the mean-<dEdx> is calculated according to
 *  the selected option.
 *
 *  Simulation Options:
 *
 *	 -  Generate According to given distribution function, independently
 *	   on the Number of Points.
 *
 *	    Distrib. Function already implemented.
 *
 *	      Gaussian
 *	      Asymmetric Gaussian
 *	      Double Tailed Gaussian
 *	      Superposition of Gaussians weighted According to a Number of Points Distr.
 *
 *
 *
 *	    Mean Values of the Distrib. Function are set to zero
 *	    as the central <dEdx> is to be calculated from the provided Bethe-Bloch function.
 *
 *	    Sigma Scaling Option:
 *
 *	      alpha
 *
 *
 *       Use Defined Distribution Function
 *
 *
 *   -  Generate <dEdx> as truncated average of the clusters
 *      charges
 *
 *
 *
 *
 *
 *
 *
 */



#ifndef TSDEDXGENERATOR_H_
#define TSDEDXGENERATOR_H_

#include <map>
#include <vector>


#include <TString.h>

#include <TF1.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TObject.h>

#include "TH1FAuto.h"

#include "TSNamed.h"

#include "TSVariable.h"

#include "TSBetheBlochFunc.h"



class TSDEdxDistrib;


class TSDEdxGenerator: public TSNamed {


public:
	TSDEdxGenerator(TString name, TString title);
	TSDEdxGenerator(TString name, TString title, const TSVariable &dedx_var);


	virtual ~TSDEdxGenerator();

	void ApplyCalibrationCorrection(bool);

	double ComputeAverage(std::vector<double> &clusters, double trunc_low, double trunc_up, int algo, double param=0) const;

	double Generate() const;
	double Generate(int num_clusters) const;
	double Generate(double momentum, double mass, int num_clusters=0) const;

	double GetMean() const {return fMean;}
	int GetNumOfUsedClusters() const {return fNumOfUsedClusters;}
	TObject* GetTrackDEdxDistrib() const;
    TObject* ImportTrackDEdxDistrib() const;


	void Init();

	void SetAverageAlgoType(int);
	void SetAverageAlgoParam(double);

	void SetAsymmetry(double);

	void SetBetheBloch(const TSBetheBlochFunc &);

	void SetClusterChargeDistrib(TF1 &);
	void SetClusterChargeDistrib(TH1F &);
	void SetClusterChargeDistrib(TString fname, double min, double max, double p0, double p1=0, double p2=0, double p3=0);

	void SetDEdxVariable(const TSVariable &v);

    //this is an averall mean shift of the distrib you use to generate dE/dx
	void SetMean(double) const;
	void SetPointDistrib(TH1F &);
	void SetSigmaScaling(double m, double alpha);

	void SetTrackDEdxDistrib(TF1 &);
	void SetTrackDEdxDistrib(TH1F &);
	void SetTrackDEdxDistrib(TString fname, double min, double max, double p0, double p1=0, double p2=0, double p3=0);
    void SetTruncation(double up, double low=0);
	void SetWidthSmearing(double);


private:

	double fComputeCalibrationCorrection(int nclusters) const;

	void fInitTrackDistrib();
	void fSetDistribTitles();
	void fSetIsClusterDistrib();
	void fSetIsTrackDistrib();

	int fAverageAlgoType;
	double fAverageAlgoParam;

	TObject *fTrackDistrib;

    TSDEdxDistrib *fDistrib;

    mutable int fNumOfUsedClusters;

    double fTruncUp;
    double fTruncLow;

    mutable double fMean;

    const TSBetheBlochFunc *fBetheBloch;

    bool fApplyCalibCorr;

    TString fTrackDedxDistrName;
    TString fClusterDedxDistrName;

    bool fIsClusterDistrib;
    bool fIsTrackDistrib;
    TString fClusterChargeDistrTitle;
    TString fTrackChargeDistrTitle;

    TSVariable fVariableX;

    TH1FAuto autoTH1F;

};







class TSDEdxDistrib {

public:

	TSDEdxDistrib(TString fname="");
	double Distrib(double *x, double *p);
	double PointAveragedDistrib(double *x, double *p);

	double GetMin() const {return xmin;}
	double GetMax() const {return xmax;}

	int GetNpar();
	int GetNpar() const;
	bool HasPointDistrib() const {return HasPointDistr;}
	void Init();
	void InitFunctionParNames(TF1 &);
	bool IsLandau() const {return fIsLandau;}
	void SetFunctionPars(TF1 &);
	void SetPointDistrib(TH1F &);


	double GetMean() const {return mean;}
	double GetSigma() const {return sigma;}
	double GetSigmaTails() const {return sigma_tails;}
	double GetRatioTails() const {return ratio_tails;}
	double GetSigmaScaleBase() const {return sigma_scale_base;}
	double GetSigmaScalePow() const {return alpha;}
	double GetDelta() const {return delta;}
	double GetSpread() const {return spread;}


	void SetMean(double);
	void SetSigma(double);
	void SetSigmaTails(double);
	void SetRatioTails(double);
	void SetSigmaScaleBase(double);
	void SetSigmaScaling(double);
	void SetDelta(double);
	void SetSpread(double);
	void SetNPoints(double);

	bool MeanSet() const {return fMeanSet;}
	void Print();
	bool SigmaSet() const {return fSigmaSet;}
	bool SigmaTailsSet() const {return fSigmaTailsSet;}
	bool RatioTailsSet() const {return fRatioTailsSet;}
	bool SigmaScaleBaseSet() const {return fSigmaScaleBaseSet;}
	bool SigmaScalingSet() const {return fSigmaScalingSet;}
	bool DeltaSet() const {return fDeltaSet;}
	bool SpreadSet() const {return fSpreadSet;}
	bool NPointsSet() const {return fNPointsSet;}

	void SetParameters(double m, double s, double s2=0, double r=0, double scale=1, double alpha=0, double delta=0, double rms=0, double npts=1);

	void SetMin(double v) {xmin=v;}
	void SetMax(double v) {xmax=v;}



private:

	double GNorm;

	bool fIsLandau;

	double mean;
	double sigma;
	double sigma_tails;
	double ratio_tails;
	double sigma_scale_base;
	double alpha;
	double delta;
	double spread;
	double npoints;

	double landau_mpv_shift;

	double xmin,xmax;

	bool HasPointDistr;

	std::map<int,bool> par_used;
	std::map<int,TString> par_name;
	std::map<TString,int> par_num;
	std::map<int,double> par_val;

	std::vector<float> mult;
	std::vector<float> npts;

	int ip_mean,ip_sigma,ip_sigma_tails
		,ip_ratio_tails,ip_sigma_scale_base,ip_sigma_scale_pow
		,ip_delta,ip_spread,ip_npoints;

	bool fMeanSet,fSigmaSet,fSigmaTailsSet,fRatioTailsSet
		,fSigmaScaleBaseSet,fSigmaScalingSet,fDeltaSet,fSpreadSet,fNPointsSet;


};


#endif /* TSDEDXGENERATOR_H_ */
