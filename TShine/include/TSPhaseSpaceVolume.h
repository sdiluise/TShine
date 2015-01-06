/*
 * TSPhaseSpaceVolume.h
 *
 *  Created on: May 29, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *   Implementation of a n-dimensional volume in
 *   real double valued variable space.
 *
 *
 */

#ifndef TSPHASESPACEVOLUME_H_
#define TSPHASESPACEVOLUME_H_


#include <map>

#include <TString.h>
#include "TSNamed.h"
#include "TSVariable.h"



class TSPhaseSpaceVolume: public TSNamed {


public:

	TSPhaseSpaceVolume();
	TSPhaseSpaceVolume(int dim, TString name, TString title);
	TSPhaseSpaceVolume(const TSPhaseSpaceVolume&);
	virtual ~TSPhaseSpaceVolume();

	int Add(const TSPhaseSpaceVolume& );
	int AddAxis(const TSVariable&, double low, double up);
	void Clear();
	void Copy(const TSPhaseSpaceVolume&);
	TSVariable* GetAxis(int) const;
	double GetCenter(int axis) const;
	int GetDimension() const {return fDimension;}
	TString GetDescription() const {return fDescription;}
	double GetLowEdge(int axis) const;
	double GetUpEdge(int axis) const;
	const TSVariable* GetVariable(int axis) const;
    void Init();
    int IsInside() const;

    void Print() const;
    void SetDimension(int);

    int SetAxis(int ax, const TSVariable&, double low, double up);


private:


    void fBuildDescription();

    TString fDescription;
	int fDimension;

	std::map<int,const TSVariable*> fVarList;
	std::map<int,TSVariable*> fAxisVariable;


};

#endif /* TSPHASESPACEVOLUME_H_ */
