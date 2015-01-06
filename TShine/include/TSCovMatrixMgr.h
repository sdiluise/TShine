/*
 * TSCovMatrixMgr.h
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSCOVMATRIXMGR_H_
#define TSCOVMATRIXMGR_H_

#include <vector>
#include <utility>

#include <TObjArray.h>
#include <TH2.h>
#include <TH2F.h>


#include <TVectorD.h>
#include <TMatrixTSym.h>
#include <TMatrixT.h>
#include <TDecompChol.h>
#include <TRandom.h>

#include <RooMultiVarGaussian.h>

#include "TSNamed.h"

#include "TSParamList.h"

#include "TSNuiParam.h"
#include "TSNuiParamSet.h"

/*
 *
 */

class TSCovMatrixMgr: public TSNamed {

public:

	TSCovMatrixMgr();
	TSCovMatrixMgr(TString name, TString title);
	virtual ~TSCovMatrixMgr();

	void Add(const TMatrixT<double> &, TString opt="");

	void AggregateMatrix(const TMatrixT<double> &);

	void Decompose();


	void ImportSubMatrix(const TMatrixT<double> &, int first_ele=0, int last_ele=0);
	void ImportSubMatrix(const TMatrixT<double> &, std::vector<std::pair<int,int> > &);

	void ImportSubMatrix(const TH2 &, int first_ele=0, int last_ele=0);
	void ImportSubMatrix(const TH2 &, std::vector<std::pair<int,int> > &);

	void ImportSubMatrixDiag(const TH1 &, int first_ele=0, int last_ele=0);



	int GetDimension() const {return fDimension;}
	double GetElement(int,int) const;

	const TMatrixTSym<double>& GetTMatrixTSym() const {return fMatrix;}

	void Init();

	void Print();

	void Randomize();

	void SetElement(int,int,double);
	void SetElements(TSNuiParamSet &,TString opt="");
	void SetParameters(TSNuiParamSet &,TString opt="");

	void Update();

private:

	int fDimension;

	bool fMatHasChanged;

	TMatrixTSym<double> fMatrix;
	TMatrixTSym<double> fCorr;


	TDecompChol fDecompChol;

	TMatrixTSym<double> fMatrixU;
	TMatrixTSym<double> fMatrixL;

	TRandom fRandom;
	//
	double *fExtracted;
	TVectorD fExt;

	//
	TSNuiParamSet *fParList;

	TObjArray fParArray;

	bool fHasParList;

	TH2F hMatHisto;

	void fBuildHisto();
};

#endif /* TSCOVMATRIXMGR_H_ */
