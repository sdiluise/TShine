/*
 * TSCategoryDef.h
 *
 *  Created on: Sep 24, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *		Define a Category:
 *
 *		     Category
 *		        Member 1
 *		        Member 2
 *			    .....
 *
 *
 */

#ifndef TSCATEGORYDEF_H_
#define TSCATEGORYDEF_H_

#include <TString.h>
#include <TH1F.h>

#include "MessageMgr.h"

#include "TSNamed.h"

/*
 *
 *
 *
 * TODO: Move HasAll to TSCategory ?
 *       Transfer Hash values via Setters?
 *
 *
 */


class TSCategoryDef: public TSNamed {


public:

	TSCategoryDef();
	TSCategoryDef(TString name, TString label, TString title);
	virtual ~TSCategoryDef();

	void AddMember(TString name, TString label="");
	void AddMembers(TString list, TString labels);

	void AddMembers(const TSCategoryDef &, TString list="");

	void Clear();
	void Copy(const TSCategoryDef &);
	int GetCategoryNameHash() const;
	int GetHash() const {return fHash;}
	TH1F& GetHistogram() {return fHisto;}
	int GetNumOfMembers() const {return fNumOfMems;}
	TString GetMemName(int) const;
	TString GetMemLabel(int) const;
	TString GetMemBinLabel(int) const;
	int GetMemHash(int) const;
	bool HasAll() const {return fHasAllMems;}
	bool HasMember(TString name) const;
	void Init();
	void Print() const;
	void PrintHashTable() const;

	void SetName(TString);


protected:
	bool fHasAllMems;

private:

	void fBuildHash();
	void fBuildHisto();
	bool fCheckCollisions();

	int fHash;
	int fNameHash;
	int fNumOfMems;

	//to be Structured
	std::vector<TString> fMember;
	std::vector<TString> fMemLabel;
	std::vector<TString> fMemBinLabel;
	std::vector<int> fMemHash;

	int first_ele_idx;
	TString token_sep;

	TH1F fHisto;
};

#endif /* TSCATEGORYDEF_H_ */
