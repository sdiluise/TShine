/*
 * TSCategoryCard.h
 *
 *  Created on: Sep 25, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef TSCATEGORYCARD_H_
#define TSCATEGORYCARD_H_

#include <vector>

#include <TH1F.h>

#include "TSNamed.h"

#include "TSCategory.h"

/*
 *
 *
 *
 */

class TSCategoryCard: public TSNamed {

public:

	TSCategoryCard();
	TSCategoryCard(TString name, TString title);
	TSCategoryCard(const TSCategoryCard &);

	virtual ~TSCategoryCard();

	void Clear();

	int Compare(const TSCategoryCard &, int opt_card=0, int opt_categ=0);

	void Copy(const TSCategoryCard &);

	void AddCategory(const TSCategory &);


	TSCategory* GetCategory(int) const;
	TSCategory* GetCategory(const TSCategory &) const;

	TH1F& GetHistogram();

	int GetNumOfCategories() const {return fCatList.size();}

	void Print() const;

private:

	std::vector<TSCategory*> fCatList;

	TH1F fHisto;
};

#endif /* TSCATEGORYCARD_H_ */
