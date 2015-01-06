/*
 * TSNamed.cc
 *
 *  Created on: May 26, 2014
 *      Author: sdiluise
 */

#include "TSNamed.h"

TSNamed::TSNamed() {


	Clear();

}

TSNamed::TSNamed(TString name, TString label, TString title) {


  SetName(name);
  SetLabel(label);
  SetTitle(title);


}

TSNamed::~TSNamed() {


}

void TSNamed::Clear() {

   fName="";
   fTitle="";
   fLabel="";
   fDescription="";

}


void TSNamed::SetLabel(TString s) {

	fLabel=s;

	fBuildStrDescription();
}

void TSNamed::SetName(TString s) {

	fName=s;
}

void TSNamed::SetTitle(TString s) {

	fTitle=s;

	fBuildStrDescription();
}



void TSNamed::fBuildStrDescription() {


 fDescription=fTitle; fDescription+=" "; fDescription+=fLabel;


}
