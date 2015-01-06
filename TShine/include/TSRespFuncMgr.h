/*
 * TSRespFuncMgr.h
 *
 *  Created on: Oct 2, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSRESPFUNCMGR_H_
#define TSRESPFUNCMGR_H_

#include <cassert>
#include <iostream>
#include <map>
#include <vector>

#include <TString.h>

#include "TSNamed.h"

#include "TSNuiParam.h"
#include "TSNuiParamSet.h"

#include "TSRespFunc.h"


/*
 *
 */

class TSRespFuncMgr: public TSNamed {

public:

	TSRespFuncMgr();
	TSRespFuncMgr(const TSRespFuncMgr &);
	virtual ~TSRespFuncMgr();

	void Clear();
	void Copy(const TSRespFuncMgr &);

	int Add(TSNuiParam &, TSRespFunc &);

	void ImportMap(const TSRespFuncMgr &);

	const std::map<TSNuiParam*,TSRespFunc*>& GetMap() const {return fMap;}

	void Print() const;

	double Twik(const TSNuiParam& ) const;

private:

	std::map<TSNuiParam*, TSRespFunc*> fMap;

	TSNuiParamSet fSetOfParams;

};

#endif /* TSRESPFUNCMGR_H_ */
