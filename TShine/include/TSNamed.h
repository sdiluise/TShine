/*
 * TSNamed.h
 *
 *  Created on: May 26, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 */

/**

Basic Class to implement Named Object.
Attributes are Name, Label and a Title.

Description = Title + Label


*/

#ifndef TSNAMED_H_
#define TSNAMED_H_

#include <string>

#include <TString.h>
#include <TObject.h>

#include "StringUtils.h"


#include "MessageMgr.h"


using std::string;


class TSNamed: public TObject {


public:

	TSNamed();
	TSNamed(TString name, TString label, TString title);
	virtual ~TSNamed();

	void Clear();

	virtual TString GetDescription() const {return fDescription;}
	TString GetLabel() const {return fLabel;}
	TString Label() const {return fLabel;}
	TString Name() const {return fName;}
	TString Title() const {return fTitle;}

	//Override TObject::GetName in order to make
	//objects Inherithing from TSName searchable in
	//ROOT containers
	const char* GetName() const { return fName.Data(); }
	const char* GetTitle() const { return fTitle.Data(); }

	virtual void SetLabel(TString);
	virtual void SetName(TString);
	virtual void SetTitle(TString);


private:


	void fBuildStrDescription();


	TString fName;
	TString fLabel;
	TString fTitle;

	TString fDescription;

};

#endif /* TSNAMED_H_ */
