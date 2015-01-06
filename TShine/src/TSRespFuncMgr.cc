/*
 * TSRespFuncMgr.cc
 *
 *  Created on: Oct 2, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include "MessageMgr.h"

#include "TSRespFunc.h"

#include "TSRespFuncMgr.h"



TSRespFuncMgr::TSRespFuncMgr() {


}

TSRespFuncMgr::TSRespFuncMgr(const TSRespFuncMgr &other)
{

	Copy(other);

}


TSRespFuncMgr::~TSRespFuncMgr() {

	Clear();
}


void TSRespFuncMgr::Copy(const TSRespFuncMgr &other)
{

	Clear();


	std::map<TSNuiParam*,TSRespFunc*> aMap = other.GetMap();

	for( std::map<TSNuiParam*,TSRespFunc*>::iterator it=aMap.begin(), end=aMap.end()
				; it!= end; ++it)
	{

			Add(*it->first, *it->second);
	}



}


void TSRespFuncMgr::ImportMap(const TSRespFuncMgr &other)
{


}

void TSRespFuncMgr::Clear()
{

	for( std::map<TSNuiParam*,TSRespFunc*>::iterator it=fMap.begin(), end=fMap.end()
			; it!= end; ++it)
	{

		delete it->second;
	}

	fMap.clear();

	fSetOfParams.Clear();

}


int TSRespFuncMgr::Add(TSNuiParam& par, TSRespFunc &func)
{

	if( fMap.count(&par) ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," Resp. Func substituted for param: ",par.GetName());

		delete fMap.find(&par)->second;
	}

	fMap[&par] = func.Clone();

	fSetOfParams.Add(par);

	return 1;

}



void TSRespFuncMgr::Print() const
{

	cout<<" ---- TSRespFunc --- "<<endl;

	int n = 0;
	for( std::map<TSNuiParam*,TSRespFunc*>::const_iterator it=fMap.begin(), end=fMap.end()
				; it!= end; ++it)
		{

			TSNuiParam *par = it->first;
			TSRespFunc *rf = it->second;

			cout<<" -"<<n<<"- "<<par->GetName()<<" prior: "<<par->GetPriorValue()<<" actual: "<<par->GetValue()<<endl;

			rf->Print();

			n++;

		}


	cout<<" ----- "<<endl;

}



double TSRespFuncMgr::Twik(const TSNuiParam &param) const
{


	TSNuiParam *par = const_cast<TSNuiParam*>(&param);

	std::map<TSNuiParam*,TSRespFunc*>::const_iterator map_obj = fMap.find(par);

	if( map_obj == fMap.end() ){

		return param.GetValue();

	}else{

		TSRespFunc *rf =  map_obj->second;

		double central = param.GetPriorValue();
		double value = param.GetValue();

		double rfval = rf->Tweak(central,value);
	}





}
