/*
 * TSArgList.cc
 *
 *  Created on: Aug 5, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <iostream>

#include "MessageMgr.h"


#include "TSVariable.h"
#include "TSParameter.h"

#include "TSArgList.h"


using std::cout;
using std::endl;


TSArgList::TSArgList()
:TList()
{

}

TSArgList::TSArgList(TString name,TString title)
:TList()
{

	TList::SetName(name);

}


TSArgList::TSArgList(TObject &o1)
:TList()
{

	Add( &o1 );
}

TSArgList::TSArgList(TObject &o1, TObject &o2)
:TList()
{

	Add( &o1 );
	Add( &o2 );

}

TSArgList::TSArgList(TObject &o1, TObject &o2, TObject &o3)
:TList()
{

	Add( &o1 );
	Add( &o2 );
	Add( &o3 );

}

TSArgList::TSArgList(TObject &o1, TObject &o2, TObject &o3, TObject &o4)
:TList()
{

	Add( &o1 );
	Add( &o2 );
	Add( &o3 );
	Add( &o4 );

}


TSArgList::TSArgList(const TSArgList &other)
:TList()
{

	//copy constructor necessary to use
	//the constructor as an rvalue: as argument
	//to functions (in this case it must be declared as const argument)

	for(int i=0; i<other.GetSize();++i){
		this->Add(other.At(i));
	}

}

TSArgList::~TSArgList() {

}



void TSArgList::Add(TObject *o) {

	if( !o ) return ;

	TList::Add(o);

}


void TSArgList::AddOnce(TObject *o) {

	if( !o ) return;

	if( !TList::FindObject( o->GetName() ) ){

		Add(o);

	}else{
		return;
	}

}

/*
void TSArgList::Add(TSArgList *other) {



}
*/

void TSArgList::Add(const TSArgList &other) {

	//cout<<"Adding "<<other.GetSize()<<" "<<other.GetName()<<endl;

	for(int i=0; i<other.GetSize(); ++i){

		//cout<<other.At(i)<<endl;

		this->Add(other.At(i));
	}

}


void TSArgList::AddOnce(const TSArgList &other) {

	for(int i=0; i<other.GetSize();++i){

		this->AddOnce(other.At(i));
	}

}


void TSArgList::Copy(const TSArgList &other) {


	Clear();

	for(int i=0; i<other.GetSize();++i){
		this->Add(other.At(i));
	}


}


TSArgList* TSArgList::Clone(TString name) const{


	cout<<"TSArgList::Clone"<<endl;

	TSArgList *l=new TSArgList();

	if( !name.IsWhitespace()) l->SetName(name);
	else l->SetName(GetName());


	for(int i=0; i<GetSize();++i){

		cout<<"Cloning "<<At(i)->GetName()<<" "<<At(i)<<" "<<static_cast<TSParameter*>(At(i))<<endl;


		l->Add( At(i)->Clone() );
	}

	cout<<"cloned "<<endl;

	for(int i=0; i<GetSize();++i){

		cout<<"Cloned "<<l->At(i)->GetName()<<" "<<l->At(i)<<" "<<static_cast<TSParameter*>(l->At(i))<<endl;

	}

	return l;
}


void TSArgList::Print() const{

    for(int i=0; i<GetSize();++i){
    	At(i)->Print();
    }

}

void TSArgList::PrintParams() const{

    for(int i=0; i<GetSize();++i){
    	TSParameter* par=static_cast<TSParameter*>(At(i));
    	if(par) par->Print();
    }

}

void TSArgList::PrintVariables() const{

    for(int i=0; i<GetSize();++i){
    	TSVariable* var=static_cast<TSVariable*>(At(i));
    	if(var) var->Print();
    }

}

