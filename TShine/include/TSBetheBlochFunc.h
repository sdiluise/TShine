/*
 *
 *
 * TSBetheBlochFunc.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Silvestro Di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#ifndef TSBETHEBLOCHFUNC_H_
#define TSBETHEBLOCHFUNC_H_

#include <vector>

#include <TMath.h>
#include <TString.h>
#include <TF1.h>
#include <TGraph.h>

#include "BetheBlochFunctions.h"



class TSBetheBlochFunc: public TF1 {


public:
	TSBetheBlochFunc();
	TSBetheBlochFunc(TString name, TString title, double (*fcn)(double*, double*), double xmin, double xmax, int npar);
	TSBetheBlochFunc(TString name, TString title, TString table_file);
	virtual ~TSBetheBlochFunc();


	TGraph* BuildGraphFromFunction(int npt=300,double xmin=0,double xmax=0) const;
    void BuildTableFromFunction(int npts, int opt=0);

	void Clear();

	TGraph *CloneTable(TString ="") const;

	void ComputeMoments(std::vector<double> &ptot_data,double mass, double &mean,double &rms) const;
	void ComputeMoments(TH1F &h_ptot,double mass,double &mean,double &rms) const;

	Double_t Eval(Double_t momentum, Double_t mass = 0, Double_t z = 0, Double_t t = 0) const;


	bool HasFunction() const {return fHasFunc;}
	bool HasTable() const {return fHasTable;}

	TString GetAxisTitleX() const {return fAxisTitleX;}
	TString GetAxisTitleY() const {return fAxisTitleY;}

	int     GetInterpType() const {return fInterpType;}
	TString GetInterpOpt() const {return fInterpOpt;}
	int GetNumOfPoints() const {return fTable.GetN();}
	double* GetPointsArrayX() const {return fTable.GetX();}
	double* GetPointsArrayY() const {return fTable.GetY();}


	bool ImportGraph(const TGraph &);
	bool ImportTable(const TGraph& g) {return ImportGraph(g);}
	bool ImportTable(TString file_name);
	void Init();

	void SetInterpolation(int);
	void SetParNames(TString, TString ="",TString ="",TString ="",TString ="", TString ="");
	void UseFunction(bool);
	void UseTable(bool);
	bool UsingFunction() const {return fUseFunc;}
	bool UsingTable() const {return fUseTable;}

private:

	double fEvalFunction(double, int opt=0) const;
	double fEvalTabular(double, int opt=0) const;
	void fSetTitles();

	bool fHasFunc;
	bool fHasTable;
	int  fInterpType;
	TString fInterpOpt;
	bool fUseFunc;
    bool fUseTable;

	TGraph fTable;

	TString fAxisTitleX;
	TString fAxisTitleY;

};

#endif /* TSBETHEBLOCHFUNC_H_ */
