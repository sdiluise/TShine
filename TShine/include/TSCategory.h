/*
 * TSCategory.h
 *
 *  Created on: Sep 24, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *	  Create a Category Sample, i.e.: a subset of the category
 *
 *	  	     Category
 *	  	           Members
 *
 *
 *
 */

#ifndef TSCATEGORY_H_
#define TSCATEGORY_H_

#include <TString.h>


#include "TSCategoryDef.h"

/*
 *
 */
class TSCategory: public TSCategoryDef {

public:

	TSCategory();
	TSCategory(const TSCategoryDef &, TString members="");
	TSCategory(const TSCategory &);
	virtual ~TSCategory();

	int Compare(const TSCategory &, int opt=0) const;


	void Copy(const TSCategory &);

	const TSCategoryDef& GetMotherCategory() const {return fMotherCateg;}

	bool SameMember(int,const TSCategory&, int) const;
	bool SameType(const TSCategory &) const;
	void Set(const TSCategoryDef &, TString list="");

private:

	TSCategoryDef fMotherCateg;

};

#endif /* TSCATEGORY_H_ */
