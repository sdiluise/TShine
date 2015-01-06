/*
 * HistoUtils.h
 *
 *  Created on: Mar 12, 2013
 *      Author: Silvestro di Luise
 */

#ifndef HISTOUTILS_H_
#define HISTOUTILS_H_

#include <TString.h>
#include <TGraph.h>
#include <TH1.h>
#include <TAxis.h>
#include <TObject.h>

//#include "NAStringUtil.h"


class HistoUtils: public TObject {

public:
	HistoUtils();
	virtual ~HistoUtils();



	TString BuildName(TString, TString);
	TString BuildName(int, int);
	TString BuildName(TString, int);
	TString BuildName(TString, int, int);
	TString BuildName(TString, TString, int);
	TString BuildName(TString, TString, int, int);
	TString BuildName(TString,TString,TString);

	TString BuildString(TString, TString);
	TString BuildString(int, int);
	TString BuildString(TString, int);
	TString BuildString(TString, int, int);
	TString BuildString(TString, TString, int);
	TString BuildString(TString, TString, int, int);
	TString BuildString(TString,TString,TString);

	TString BuildUnit(TString);
	TString BuildAtit(TString,TString);
	TString BuildTitle(TString,TString);
	TString BuildTitle(TString,TString,TString);
	TString BuildRange(TString,float,float,TString);
	TString BuildRange(float,float,TString);
	TString BuildRange(TString,float,float);
	TString BuildRange(float,float);

	bool CheckAxisLimits(TAxis*,TAxis*);
	bool CheckBinLimits(TAxis*,TAxis*);
	bool CheckBinAlignment(TH1*,TH1*);
	bool CheckConsistency(TH1*,TH1*);


	bool PtrCheck(TObject*,TObject*);

	void SetSeparator(TString);



private:

	TString mBuildString(TString, TString);
	TString mBuildString(int, int);
	TString mBuildString(TString, int);
	TString mBuildString(TString, int, int);
	TString mBuildString(TString, TString, int);
	TString mBuildString(TString, TString, int, int);
	TString mBuildString(TString,TString,TString);

	TString m_separ;
	TString m_separ_name;
	TString m_separ_tit;
	TString m_separ_null;
	TString m_unitL;
	TString m_unitR;

};

#endif /* HISTOUTILS_H_ */
