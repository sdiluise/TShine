/*
 * TSParamList.h
 *
 *  Created on: Aug 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSPARAMLIST_H_
#define TSPARAMLIST_H_

#include <TString.h>


#include <TH1F.h>
#include <TH2F.h>
#include <TVectorD.h>
#include <TVectorT.h>

#include "TSParameter.h"
#include "TSArgList.h"


/*
 *
 */

class TSParamList: public TSArgList {

public:

	TSParamList();
	TSParamList(TString name, TString title="");
	TSParamList(TSParameter &o1);
	TSParamList(TSParameter &o1, TSParameter &o2);
	TSParamList(TSParameter &o1,TSParameter  &o2, TSParameter &o3);
	TSParamList(TSParameter &o1,TSParameter  &o2,TSParameter  &o3,TSParameter  &o4);
	TSParamList(const TSParamList &);

	virtual ~TSParamList();

	void Add(TSParameter *o);
	void Add(TSParameter *o1,TSParameter *o2,TSParameter *o3=0);
	void Add(TSParameter *o1,TSParameter *o2,TSParameter *o3,TSParameter *o4,TSParameter *o5=0);

	void Add(TSParameter& o1);
	void Add(TSParameter& o1,TSParameter& o2);
	void Add(TSParameter& o1,TSParameter& o2,TSParameter& o3);
	void Add(TSParameter& o1,TSParameter& o2,TSParameter& o3,TSParameter& o4);



	void AddOnce(TSParameter *o);

	void Add(const TSParamList &);

	void AddOnce(const TSParamList &);

	void AddToValues(const TVectorD &);

	TSParameter* At(int) const;

	double ComputeErrorLinear() const;
	double ComputeSum() const;

	void Copy(const TSArgList &);
	TSParamList* Clone(TString name="") const;

	TSParameter* Find(TString) const;

	void CopyValues(const TSArgList &);
	void CopyValuesAndErrors(const TSArgList &);


	void Fix();

	TH1F* GetHistoOfValues(TString name="") const;
	TH1F* GetHistoOfErrors(TString name="") const;
	TH1F* GetHistoOfUpErrors(TString name="") const;
	TH1F* GetHistoOfLowErrors(TString name="") const;
	TH1F* GetHistoOfFracErrors(TString name="") const;
	TH1F* GetHistoOfLowLimits(TString name="") const;
	TH1F* GetHistoOfUpLimits(TString name="") const;

	TH1F *GetHisto(TString name="") const;
	TH2F *GetHisto2D(TString name="") const;

	void Import(const TSArgList &);

	void Print() const;

	void Release();

	void SetValue(double);
	void SetFixed(double);
	void SetRange(double,double);
	void SetStep(double);
	void ForceToRange(bool);



};


#endif /* TSPARAMLIST_H_ */
