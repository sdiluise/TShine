/*
 * TSHistogramFld.h
 *
 *  Created on: Jun 2, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *      Class to create a folder of Histograms (TH1,TH2, TH2Poly...)
 *
 *      Each histogram-axis has associated the name of the corresponding TSVariable.
 *      Those names are to be provided when the histogram is added to the folder
 *      via the Add method.
 *
 *      Several Fill methods are provided to fill all the histograms
 *      at the same time from a given set of TSVariable values.
 *
 *
 *      Folder is the Owner of the Added Objects
 *
 *
 *
 */

#ifndef TSHISTOGRAMFLD_H_
#define TSHISTOGRAMFLD_H_

#include <string>
#include <map>

#include <TString.h>

#include <TFile.h>
#include <TH2Poly.h>
#include <TList.h>



class TSHistogramFld: public TList {

public:
	TSHistogramFld();
	TSHistogramFld(TString name);
	virtual ~TSHistogramFld();

	void Add(TObject*, TString, TString="");
	int AddPolyBinContent(TH2Poly &,int bin, double w=1);
	TSHistogramFld* Clone(TString name="") const;
	void AppendToAllNames(TString, TString sep="_");
	void AppendToAllTitles(TString, TString sep=" ");
	void Clear(Option_t* opt="");
	void Copy(TSHistogramFld&);
	int Fill(int N, TString* list, double* values);
	int Fill(std::vector<TString>&, std::vector<double>&);
	int Fill(TString vx, TString vy, double x, double y, double w=1, int opt=0);
	int Fill(TString vx, double x, double w=1, int opt=0);
	int FillPoly(TH2Poly *h, double x, double y, double w=1, int opt=1);

	int FillHistosFromFile(TFile *,TString opt="");

	int GetHistoDim(int) const;
	TString GetHistoName(int) const;
	TString GetHistoNameX(int) const;
	TString GetHistoNameY(int) const;
	TList *GetListOf(TString) const;
	TObject* GetObject(int) const;
	float GetOutliersFrac(TString, TString ="") const;
	TObject* GetTagHisto(TString, TString ="") const;
	int Import(TSHistogramFld&);
	bool IsSumw2() const {return fSumw2;}

	void Init();

	bool IsTH1(const TObject& ) const;
	bool IsTH2(const TObject& ) const;
	bool IsTH2Poly(const TObject& ) const;

	void PrependToAllNames(TString,TString sep="_");
	void PrependToAllTitles(TString,TString sep=" ");
	void ReplaceToAllTitles(TString);
	void RenameAllWithBase(TString,TString sep="_");
	void ResetHistos();

	void SetSumw2(bool =false);

private:


	bool fSumw2;

	void ApplyTH2Style(TH2 *);

	struct fHistoSettings{
		int Position;
		TString Name;
		int Dim;
		TString NameX;
		TString NameY;
	};

	std::map<int,fHistoSettings*> fHistoSetsList;

	std::map<TString,int> fObjPos;
	std::map<int,TString> fObjName;

	std::map<int,TString> fObjNameX;
	std::map<int,TString> fObjNameY;



};

#endif /* TSHISTOGRAMFLD_H_ */
