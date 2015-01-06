/*
 * TSMinimizer.h
 *
 *  Created on: Aug 12, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *   Minimize a function of the type TSMinimizationFunction
 *   over its parameters
 *
 *
 *
 */

#ifndef TSMINIMIZER_H_
#define TSMINIMIZER_H_

#include <vector>
#include <utility>
#include <map>

#include <TString.h>

#include <TVectorD.h>
#include <TMatrixD.h>
#include <TTree.h>

#include <TStopwatch.h>

#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"
#include "Math/Factory.h"

#include "TVirtualFitter.h"

#include "Math/IFunction.h"
#include "Math/Util.h"

#include "TSArgList.h"
#include "TSParamList.h"

#include "TSMinimizationFunction.h"
#include "TSChisquareCalculator.h"

/*
 * TODO: add name and title to the minimizer class?
 *
 *
 */

/*
 *
 */

class TSMinimizer {


public:

	TSMinimizer(TString minimizer_type, TString algo_type);

	TGraph* BuildContourGraph(TString name_x, TString name_y, unsigned int npts=100);


	void ClearContourGraphs();
	void ClearContourParamList();
	void ComputeContours();
	void Configure();

	void FillCovarianceMatrix();

	ROOT::Math::Minimizer* GetMinimizer() const {return fMinimizer;}
	TSMinimizationFunction* GetMinimizationFunction() const {return fMinFunction;}

	void FitScan(TSParamList &par_list);

	TSParamList* GetBestFitParamList() const {return fBestFitParamList;}

	void GetContourGraphs(TList &) const;
	void GetCovMatrix(TMatrixD &) const;
	void GetCorrMatrix(TMatrixD &) const;
	void GetScanGraphs(TList &) const;
	void GetHistoryTree(TTree *) const;


	void Init();

	int Minimize();



	void SaveContour(const TSParameter &, const TSParameter &);
	void SaveContours(const TSParamList &);

	void SetFunction(TSMinimizationFunction &minim_func);

	int SetNumOfContourPoints(int n) {fNptsContour=n;}

	virtual ~TSMinimizer();

private:

	TSMinimizationFunction *fMinFunction;

	ROOT::Math::Minimizer *fMinimizer;

	TSParamList *fBestFitParamList;

	std::map<std::pair<TString,TString>, TGraph* > fContourGraphList;
	std::vector< std::pair<TString,TString> > fContourParamPairs;

	std::map<TString,TGraph*> fScanGraphList;

	int fNptsContour;

	TMatrixD fCovMatrix;
	TMatrixD fCorrMatrix;
	TMatrixD fHesseMatrix;

	TStopwatch fWatch;
};

#endif /* TSMINIMIZER_H_ */
