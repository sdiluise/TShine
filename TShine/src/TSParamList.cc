/*
 * TSParamList.cc
 *
 *  Created on: Aug 21, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <TMath.h>

#include "RootUtils.h"
#include "TSParamList.h"

TSParamList::TSParamList()
:TSArgList()
{

}

TSParamList::TSParamList(TString name, TString title)
:TSArgList(name,title)
{


}

TSParamList::TSParamList(TSParameter &o1)
:TSArgList(o1)
{

}

TSParamList::TSParamList(TSParameter &o1, TSParameter &o2)
:TSArgList(o1,o2)
{


}
TSParamList::TSParamList(TSParameter &o1,TSParameter  &o2, TSParameter &o3)
:TSArgList(o1,o2,o3)
{

}
TSParamList::TSParamList(TSParameter &o1,TSParameter  &o2,TSParameter  &o3,TSParameter  &o4)
:TSArgList(o1,o2,o3,o4)
{

}

TSParamList::TSParamList(const TSParamList &other)
{
	//copy constructor necessary to use
	//the constructor as an rvalue: as argument
	//to functions (in this case it must be declared as const argument)


	for(int i=0; i<other.GetSize();++i){

		TSArgList::Add( other.At(i) );

	}

}


TSParamList::~TSParamList() {


}


void TSParamList::Add(TSParameter *o){

	TSArgList::Add(o);
}

void TSParamList::Add(TSParameter *o1,TSParameter *o2,TSParameter *o3){

	TSArgList::Add(o1);
	TSArgList::Add(o2);
	if(o3) TSArgList::Add(o3);

}


void TSParamList::Add(TSParameter *o1,TSParameter *o2,TSParameter *o3,TSParameter *o4,TSParameter *o5){

	TSArgList::Add(o1);
	TSArgList::Add(o2);
	TSArgList::Add(o3);
	TSArgList::Add(o4);
	if(o5) TSArgList::Add(o5);


}

void TSParamList::Add(TSParameter &o1){

	TSArgList::Add(&o1);

}

void TSParamList::Add(TSParameter &o1,TSParameter &o2){

	TSArgList::Add(&o1);
	TSArgList::Add(&o2);

}

void TSParamList::Add(TSParameter& o1,TSParameter & o2,TSParameter& o3){


	Add(&o1,&o2);
	TSArgList::Add(&o3);

}

void TSParamList::Add(TSParameter& o1,TSParameter & o2,TSParameter& o3, TSParameter& o4){

	TSArgList::Add(&o1);
	TSArgList::Add(&o2);
	TSArgList::Add(&o3);
	TSArgList::Add(&o4);

}



void TSParamList::AddOnce(TSParameter *o){

	TSArgList::AddOnce(o);

}


void TSParamList::Add(const TSParamList &other){

	TSArgList::Add(other);

}

void TSParamList::AddOnce(const TSParamList &other){

	TSArgList::AddOnce(other);

}

void TSParamList::AddToValues(const TVectorD &v)
{

	if( v.GetNoElements() != GetSize() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Vector and List dimension do not match: ",v.GetNoElements()," vs ",GetSize());
		return;
	}


	for(int i=LoopStart();i<LoopNext();++i){

		TSParameter *p = (TSParameter*)LoopAt();

		double val = p->GetValue();

		double dv = v.GetMatrixArray()[i];

		p->SetValue(val+dv);
	}

}

TSParameter* TSParamList::At(int i) const{

	return static_cast<TSParameter*>( TSArgList::At(i) );

}


TSParameter* TSParamList::Find(TString name) const{

	if( TSArgList::FindObject(name) ){
		return static_cast<TSParameter*>( TSArgList::FindObject(name) );
	}
	else return 0;

}


double TSParamList::ComputeErrorLinear() const{

	double err=0;

	for(int i=LoopStart();i<LoopNext();++i){

		double e = ((TSParameter*)LoopAt())->GetError();

		err+=e*e;
	}

	if( err>0 ) err=TMath::Sqrt(err);
	else err=0;

	return err;
}

double TSParamList::ComputeSum() const{

	double sum=0;

	for(int i=0;i<GetSize();++i){
		sum += At(i)->GetValue();
	}

	return sum;
}

void TSParamList::Copy(const TSArgList &other){


	for(int i=0;i<other.GetSize();++i){

		if( !static_cast<TSParameter*>( other.TSArgList::At(i) ) ) {
			MSG::ERROR(__FILE__,"::",__FUNCTION__," cannot copy TSArgList: ",other.GetName()," not a TSParameterList");
			cout<<other.TSArgList::At(i)->GetName()<<endl;
			return;
		}
	}


	TSArgList::Copy(other);

}



void TSParamList::CopyValues(const TSArgList &other){


	for(int i=0; i<other.GetSize();++i){

		TSParameter* po=(TSParameter*)other.At(i);
		if(!po) return;

		TSParameter *p=Find(po->GetName());

		if( p ){
			cout<<__FILE__<<"::"<<__FUNCTION__<<" Replacing value of "<<p->GetName()<<": "<<p->GetValue()<<" with: "<<po->GetValue()<<endl;
			p->SetValue(po->GetValue());
		}

	}

}


void TSParamList::CopyValuesAndErrors(const TSArgList &other){


	for(int i=0; i<other.GetSize();++i){

		TSParameter* po=(TSParameter*)other.At(i);
		if(!po) return;

		TSParameter *p=Find(po->GetName());

		if( p ){
			//cout<<__FILE__<<"::"<<__FUNCTION__<<" Replacing value of "<<p->GetName()<<": "<<p->GetValue()<<" with: "<<po->GetValue()<<endl;
			p->SetValue(po->GetValue());
			p->SetError(po->GetError());
			if(po->HasAsymErrors()) p->SetAsymErrors(po->GetErrorLow(),po->GetErrorUp());
		}

	}

}



TSParamList* TSParamList::Clone(TString name) const{

	//cout<<__FILE__<<"::"<<__FUNCTION__<<" "<<name<<endl;

	TSParamList *l=new TSParamList();

	if( !name.IsWhitespace()) l->SetName(name);
	else l->SetName(GetName());


	for(int i=0; i<GetSize();++i){

		//cout<<"Cloning ";this->At(i)->Print();

		TSParameter* p=this->At(i)->Clone("",0);

		l->Add( p );
	}


	return l;

}



void TSParamList::Fix() {

	for(int i=0;i<GetSize();++i){
		At(i)->Fix();
	}
}


void TSParamList::Import(const TSArgList &other){

	for(int i=0;i<other.GetSize();++i){
		if( !static_cast<TSParameter*>( other.TSArgList::At(i) ) ) {
			MSG::ERROR(__FILE__,"::",__FUNCTION__," cannot copy TSArgList: ",other.GetName()," not a TSParameterList");
			cout<<other.TSArgList::At(i)->GetName()<<endl;
			return;
		}
	}


	for(int i=0;i<other.GetSize();++i){
		TSParameter *par=static_cast<TSParameter*>( other.TSArgList::At(i) );

		Add( *par->Clone("",0) );
	}


}


TH1F* TSParamList::GetHistoOfValues(TString name) const{

	TH1F *h = GetHisto("Values");

	h->SetTitle(" parameter values");
	h->GetYaxis()->SetTitle("value");


	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetValue());
		h->SetBinError(i+1,p->GetError());
	}

	return h;
}

TH1F* TSParamList::GetHistoOfErrors(TString name) const {

	TH1F *h = GetHisto("Errors");

	h->SetTitle(" parameter errors");
	h->GetYaxis()->SetTitle("error");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetError());
		h->SetBinError(i+1,0);
	}

	return h;
}

TH1F* TSParamList::GetHistoOfUpErrors(TString name) const{

	TH1F *h = GetHisto("UpErrors");

	h->SetTitle(" parameter upper errors");
	h->GetYaxis()->SetTitle("upper error");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetErrorUp());
		h->SetBinError(i+1,0);
	}

	return h;
}

TH1F* TSParamList::GetHistoOfLowErrors(TString name) const{

	TH1F *h = GetHisto("LowErrors");

	h->SetTitle(" parameter lower errors");
	h->GetYaxis()->SetTitle(" lower error ");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetErrorLow());
		h->SetBinError(i+1,0);
	}

	return h;
}


TH1F* TSParamList::GetHistoOfFracErrors(TString name) const{

	TH1F *h = GetHisto("FracErrors");

	h->SetTitle(" parameter fractional errors");
	h->GetYaxis()->SetTitle(" fractional error ");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		//double val=p->GetValue();
		//double err=p->GetError();

		double r=p->GetErrorFrac();



		h->SetBinContent(i+1,r);
		h->SetBinError(i+1,0);
	}

	return h;
}


TH1F* TSParamList::GetHistoOfLowLimits(TString name) const{

	TH1F *h = GetHisto("LowLimimits");

	h->SetTitle(" parameter lower limit");
	h->GetYaxis()->SetTitle(" lower limit ");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetMin());
		h->SetBinError(i+1,0);
	}

	return h;
}


TH1F* TSParamList::GetHistoOfUpLimits(TString name) const{

	TH1F *h = GetHisto("UpLimits");

	h->SetTitle(" parameter upper limit");
	h->GetYaxis()->SetTitle(" upper limit ");

	for(int i=0;i<GetSize();++i){

		TSParameter *p= (TSParameter*)At(i);

		h->SetBinContent(i+1,p->GetMax());
		h->SetBinError(i+1,0);
	}

	return h;
}


TH1F *TSParamList::GetHisto(TString name) const{

	int N=GetSize();

	TString hname = "h_";//ParList_";

	hname+=GetName();

	hname+="_"; hname+=name;

	TString title="list of params";

	//Use this contructor to avoid the warning
	//Warning in <TROOT::Append>: Replacing existing TH1: h1 (Potential memory leak).
	TH1F *h=new TH1F();
	h->SetName(hname);
	h->SetTitle(title);
	h->SetBins(N,0.,N);

	for(int i=0;i<N;++i){

		TSParameter *p= (TSParameter*)At(i);

		h->GetXaxis()->SetBinLabel(i+1, p->GetLabel());
	}

	return h;
}


TH2F *TSParamList::GetHisto2D(TString name) const{

	int N=GetSize();

	TString hname = "h2D_";//ParList_";

	hname+=GetName();

	hname+="_"; hname+=name;

	TString title=" param matrix ";

	//Use this contructor to avoid the warning
	//Warning in <TROOT::Append>: Replacing existing TH1: h1 (Potential memory leak).
	TH2F *h=new TH2F();
	h->SetName(hname);
	h->SetTitle(title);
	h->SetBins(N,0.,N,N,0.,N);

	for(int i=0;i<N;++i){

		TSParameter *p= (TSParameter*)At(i);

		h->GetXaxis()->SetBinLabel(i+1, p->GetLabel());
		h->GetYaxis()->SetBinLabel(i+1, p->GetLabel());

	}

	return h;
}



void TSParamList::Print() const {

	cout<<"-- TSParamList::Print(): "<<GetName()<<" size: "<<GetSize()<<" --- "<<endl;

	for(int i=0; i<GetSize();++i){
		cout<<" "<<i<<"- ";
		this->At(i)->Print();
	}

	cout<<" ------ "<<endl;



}


void TSParamList::Release() {

	for(int i=0;i<GetSize();++i){
		At(i)->Release();
	}
}


void TSParamList::SetFixed(double val) {

	for(int i=0;i<GetSize();++i){
		At(i)->SetFixed(val);
	}
}

void TSParamList::SetRange(double min, double max) {

	for(int i=0;i<GetSize();++i){
		At(i)->SetRange(min,max);
	}
}

void TSParamList::SetStep(double s) {

	for(int i=0;i<GetSize();++i){
		At(i)->SetStep(s);
	}
}

void TSParamList::ForceToRange(bool b) {

	for(int i=0;i<GetSize();++i){
		At(i)->ForceToRange(b);
	}
}

void TSParamList::SetValue(double val) {

	for(int i=0;i<GetSize();++i){
		At(i)->SetValue(val);
	}
}
