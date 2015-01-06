/*
 * TSPhaseSpaceFld.cc
 *
 *  Created on: Jun 5, 2014
 *      Author: sdiluise
 */

#include "MessageMgr.h"


#include "TSPhaseSpaceFld.h"


/*
TSPhaseSpaceFld::TSPhaseSpaceFld() {
	// TODO Auto-generated constructor stub

}
*/

TSPhaseSpaceFld::TSPhaseSpaceFld(TString name, TString tag, TString title,
		const TSPhaseSpaceMap& map)
:TSNamed(name,tag,title)
{


	Init();

	SetTag(tag);


	fPhaseSpaceMap = &map;

	//Setup Histograms

	fHistoFld = map.GetHistogramFld();

	if( !fHistoFld ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," TSHistogramFld not set");
		return;
	}


	//cout<<fHistoFld<<" "<<fHistoFld->GetSize()<<endl;

	fHistoFld->ResetHistos();

	fInitCounters();

	fRenameHistos();


}

TSPhaseSpaceFld::~TSPhaseSpaceFld() {
	// TODO Auto-generated destructor stub


}

int TSPhaseSpaceFld::Fill() {

	if( !fHistoFld ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Histogram Fld not initialized");
		return 0;
	}

	fNumOfCalls++;


	//Check if is inside a bin;

	int volume = fPhaseSpaceMap->FindVolume();

	if( volume ){

		fEntriesIn++;

		fVolumeEntries[volume]++;

	}else{

		return 0;
	}



	int NVars= fPhaseSpaceMap->GetDimension();

	std::vector<TString> names;
	std::vector<double> values;


	for(int iv=1; iv<=NVars; ++iv){

		names.push_back( fPhaseSpaceMap->GetVariable(iv)->Name()  );
		values.push_back( fPhaseSpaceMap->GetVariable(iv)->GetValue()  );

		//cout<<names.at(iv-1)<<" "<<values.at(iv-1)<<endl;
	}



	fHistoFld->Fill(names,values);

	return 1;
}


float TSPhaseSpaceFld::GetVolumeEntries(int vol) const{

	if( !fPhaseSpaceMap ) return 0;

	if( fVolumeEntries.count(vol) ){

		return fVolumeEntries.find(vol)->second;
	}

	return 0;

}

void TSPhaseSpaceFld::Init() {

	SetTag("tag");
	fPhaseSpaceMap=0;
	fHistoFld=0;

	fNumOfCalls=0;
	fEntriesIn=0;

}



void TSPhaseSpaceFld::Print() const{



	cout<<endl<<"==== PhaseSpaceFolder "<<Name()<<" "<<GetTag()<<" "<<Title()<<" ======"<<endl;

	if( !fPhaseSpaceMap ) return;

	cout<<"PhaseSpaceMap: "<<fPhaseSpaceMap->GetName()<<" "<<fPhaseSpaceMap->GetTitle()<<endl;
	cout<<"Tag: "<<GetTag()<<endl;
	cout<<"Dimension "<<fPhaseSpaceMap->GetDimension()<<endl;
	cout<<"Axis Variables "<<endl;

	for(int i=1; i<=fPhaseSpaceMap->GetDimension();++i){
		cout<<i<<" "<<fPhaseSpaceMap->GetVariable(i)->Name()<<endl;
	}

	cout<<endl<<"Number of Volumes: "<<fPhaseSpaceMap->GetNumOfVolumes()<<endl;

	for(int i=1; i<=fPhaseSpaceMap->GetNumOfVolumes();++i){
		cout<<"vol: "<<i<<" "<<fPhaseSpaceMap->GetVolume(i)->GetDescription()<<", entries: "<<GetVolumeEntries(i)<<endl;
	}


	cout<<"Total Entries: "<<GetEntriesIn()<<endl;
	cout<<"Total Calls: "<<GetNumOfCalls()<<endl;


	cout<<"==== PhaseSpaceFolder: "<<Name()<<" ==== end ====="<<endl;



}

void TSPhaseSpaceFld::fInitCounters(){

	if( !fPhaseSpaceMap ) return;

	for(int i=1; i<=fPhaseSpaceMap->GetNumOfVolumes(); ++i){

		fVolumeEntries[i]=0;
	}


}



void TSPhaseSpaceFld::fRenameHistos(){


	if( !fHistoFld ) return;


	TString sep=",";
	if(GetTag().IsWhitespace() ) sep="";

	fHistoFld->PrependToAllTitles(GetTag(),sep);
	fHistoFld->PrependToAllNames(Name(),"_");



}


void TSPhaseSpaceFld::SetTag(TString tag) {

	fTag= tag;

	if( fTag.IsWhitespace() ){
		MSG::WARNING(__FILE__,"::",__FUNCTION__," Tag Is Empty");
	}


}

