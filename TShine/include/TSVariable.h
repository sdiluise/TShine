/*
 * TSVariable.h
 *
 *  Created on: May 26, 2014
 *      Author: sdiluise
 */


/**
 *
 * Class Implementing a Generic Real Variable
 *
 *
 *  Attributes:
 *    Value and a Range.
 *
 *
 * Literals defining Variable are:
 *  Name, Label, Title and Unit (opt).
 *
 *  Available Literal Attributes:
 *
 *  AxisUnit= [Unit]
 *  AxisTitle=Label+AxisUnit
 *  Range=[min,max]+Unit
 *  LabelAndRange= Label+Range
 *  Description= Tile+Label+Range
 *  Definition=LabelAndRange (temporary)
 *
 *
 *
 */


#ifndef TSVARIABLE_H_
#define TSVARIABLE_H_

#include <iostream>
#include <string>
#include <TString.h>

#include "MessageMgr.h"

#include "TSNamed.h"

#include "ServerClientMgr.h"


using std::cout;
using std::endl;




class TSVariable: public TSNamed, public ServerClientMgr {


public:

	TSVariable();
	TSVariable(TString name, TString label, TString title, TString unit="");
	TSVariable(const TSVariable&);
	virtual ~TSVariable();

	void Clear();
	bool ContainsRange(double min, double max) const;
	bool ContainsRange(const TSVariable &) const;
	void Copy(const TSVariable &);
	TSVariable* Clone(TString name="") const;
	void ForceToRange(bool);
	double GetMin() const {return fMin;}
	double GetMax() const {return fMax;}
	TString GetAxisTitle() const {return fAxisTitle;}
	TString GetAxisUnit() const {return fAxisUnit;}
	TString GetDefinition() const {return fDefinition;}
	TString GetDescription() const {return fDescription;}
	TString GetRange() const {return fRange;}
	double GetRangeCenter() const {return 0.5*(fMin+fMax);}
	TString GetLabelAndRange() const {return fLabelAndRange;}
	TString GetLabelAndValue() const;
	TString GetLabelAndValue(double ) const;
	TString GetTitleXY(TString tity="entries") const;//def tity is "entries"
	TString GetUnit() const {return fUnit;}
	virtual double GetValue() const {return fVal;}
    bool HasRange() const {return fHasRange;}
    bool HasRangeForced() const {return fHasRangeForced;}
    bool HasUnit() const {return fHasUnit;}
    bool HasValue() const {return fHasValue;}

    bool IsInside(double) const;
    void Print() const;
    bool RangeIsContained(double min,double max) const;
    bool RangeIsContained(const TSVariable &) const;
    bool RangesOverlap(double min,double max) const;
    bool RangesOverlap(const TSVariable &) const;
	void RemoveRange();
	bool SameRange(double min,double max) const;
	bool SameRange(const TSVariable &) const;
	void SetLabel(TString);
	int SetRange(double, double);
	void SetTitle(TString);
	void SetUnit(TString);
	void SetValue(double);


private:

	void fBuildStrAxisTitle();
	void fBuildStrAxisUnit();
	void fBuildStrDefinition();
	void fBuildStrDescription();
	void fBuildStrLabelAndRange();
	void fBuildStrRange();

	void fBuildStrsWithLabel();
    void fBuildStrsWithRange();
    void fBuildStrsWithTitle();
    void fBuildStrsWithUnit();


	double fVal;
	double fMin;
	double fMax;

	bool fHasRange;
	bool fHasRangeForced;
	bool fHasUnit;
	bool fHasValue;


	TString fAxisUnit;
	TString fAxisTitle;
	TString fDefinition;
	TString fDescription;
	TString fLabelAndRange;
	TString fRange;
	TString fUnit;


};

#endif /* TSVARIABLE_H_ */
