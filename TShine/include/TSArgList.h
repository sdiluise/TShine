/*
 * TSArgList.h
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *  Wrap root TList
 *  Used as container for any object which inherits from TObject
 *
 *
 */

#ifndef TSARGLIST_H_
#define TSARGLIST_H_

#include <TObject.h>
#include <TList.h>

/*
 *
 */


class TSArgList;

typedef TSArgList TSArgSet;

class TSArgList: public TList {

public:

	TSArgList();
	TSArgList(TString name, TString title="");
	TSArgList(TObject &o1);
	TSArgList(TObject &o1, TObject &o2);
	TSArgList(TObject &o1, TObject &o2, TObject &o3);
	TSArgList(TObject &o1, TObject &o2, TObject &o3, TObject &o4);

	TSArgList(const TSArgList &);

	virtual ~TSArgList();

	void Add(TObject *o);
	void AddOnce(TObject *o);
	void Add(const TSArgList &);
	void AddOnce(const TSArgList &);

	void Copy(const TSArgList &);
	TSArgList* Clone(TString name="") const;

	void Init();

	long int LoopCount() const {return fCount;}
	int LoopStart() const;
	int LoopNext() const;
	TObject* LoopAt() const;

	void Print() const;
	void PrintParams() const;
	void PrintVariables() const;

private:

	mutable TObjLink *objLink;
	mutable bool fLoopStart;

	mutable int fSize;
	mutable long int fCount;
};


//
//  ---------- inline functions ----------
//



inline int TSArgList::LoopStart() const
{

	objLink = FirstLink();

	fLoopStart = true;

	fSize = GetSize();

	fCount = 0;

	return 0;
}



inline int TSArgList::LoopNext() const
{

	if(!fLoopStart) objLink = objLink->Next();
	else fLoopStart = false;

	fCount++;

	return !objLink ? 0 : fSize;
}



inline TObject* TSArgList::LoopAt() const
{

	return !objLink ? 0 : objLink->GetObject();

}


#endif /* TSARGLIST_H_ */
