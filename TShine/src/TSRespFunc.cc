/*
 * TSRespFunc.cc
 *
 *  Created on: Oct 2, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>
#include <iostream>

#include <TMath.h>

#include "MessageMgr.h"

#include "TSRespFunc.h"


using std::cout;
using std::endl;


double TSRespFunc::kToleranceToUnity = 1e-4;


TSRespFunc::TSRespFunc() {


	Init();

}

TSRespFunc::~TSRespFunc() {

	fClear();

}



TSRespFunc::TSRespFunc(const TSRespFunc &other) {


	Init();

	Copy(other);

}



TSRespFunc* TSRespFunc::Clone(TString name) const
{

	TSRespFunc *f = new TSRespFunc(*this);

	if( !name.IsWhitespace() ){

		f->SetName(name);
	}

	return f;
}


void TSRespFunc::Copy(const TSRespFunc &other)
{

	fClear();

	Set( other.GetN(), other.GetArrayX(), other.GetArrayY() );

	if( other.HasControlVars() ){

		SetControlVarsVolume( other.GetControlVarsVolume() );
	}

}


void TSRespFunc::AddControlVar(const TSVariable &v, double min, double max)
{

	fHasControlVars = true;

	fNumOfControlVars++;

	fVarsVolume.AddAxis(v, min, max);

}


void TSRespFunc::DisableControlVars()
{

	if( fNumOfControlVars>0 )
	{
		fHasControlVars = false;
	}

}


void TSRespFunc::EnableControlVars()
{

	if( fNumOfControlVars>0 )
	{
		fHasControlVars = true;
	}

}



double TSRespFunc::Eval(double x) const
{

	
	
	if( !fIsActive ){

		return x;
	}

	if( fHasControlVars ){

		if( !fVarsVolume.IsInside() ){
			return x;
		}

	}

	if( fIsUnit ){
		return 1;
	}


	if( fIsLinear ){

		return fParsPoly[0] + fParsPoly[1]*x;
	}


	if( fIsParab ){

		return fParsPoly[0] + fParsPoly[1]*x + fParsPoly[2]*x*x;

	}


	//
	//temporary
	//
	//return fGraph.Eval(x);

	if( fN==7 ){

		//double vv = fCalcSpline3Sigma(x);
		//cout<<"SPline: "<<vv<<" "<<fGraph.Eval(x)<<endl;

		return fCalcSpline3Sigma(x);

		
	}

	return fGraph.Eval(x);

}


const TSPhaseSpaceVolume& TSRespFunc::GetControlVarsVolume() const
{

	return fVarsVolume;

}



TGraph& TSRespFunc::GetGraph()
{

	return fGraph;
}

TGraph* TSRespFunc::GetGraph(TString name) const
{

	if( !name.IsWhitespace() ){

		return (TGraph*)fGraph.Clone(name);

	}else{

		return (TGraph*)fGraph.Clone();
	}

}



bool TSRespFunc::IsAnalytic() const{

	return IsUnit() || IsLinear() || IsParab();

}


void TSRespFunc::Print() const
{

	cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	cout<<" Status: "<<(IsActive() ? "Enabled" : "Disabled")<<endl;

	for(int i=0; i<fN; ++i){

		cout<< " "<<i<<": "<<fX[i]<<" "<<fY[i]<<endl;
	}

	if( IsUnit() ) cout<<" -Is Unit- "<<endl;

	if( IsLinear() ){
		cout<<" -Is Linear- intercept: "<<fParsPoly[0]<<" slope: "<<fParsPoly[1]<<endl;
	}
	if( IsParab() ){
		cout<<" -Is Parab- A: "<<fParsPoly[0]<<" B: "<<fParsPoly[1]<<" C: "<<fParsPoly[2]<<endl;
	}


	cout<<" Control Variables: "<<fNumOfControlVars<<endl;

	if( fHasControlVars ){

		fVarsVolume.Print();
	}

	cout<<" ------  "<<endl;

}




void TSRespFunc::SetControlVarsVolume(const TSPhaseSpaceVolume &vol)
{

	fVarsVolume.Copy(vol);

}



double TSRespFunc::Tweak(double central, double x) const
{

	return Eval(x - central);

}


void TSRespFunc::Init()
{


	fN=0;
	fX = 0;
	fY = 0;

	fClear();


}


void TSRespFunc::Set(int N, float* x, float *y, float shift_x, float shift_y)
{

	/**
	 *
	 * Should use templated version...
	 *
	 */

	if(N<=1){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid Num. of Points: ",N);
		return;
	}

	if(!x || !y){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null ptr to array");
		return;
	}


	double *xx = new double[N];
	double *yy = new double[N];

	for(int i=0; i<N;++i){
		xx[i] = x[i] + shift_x;
		yy[i] = y[i] + shift_y;
	}


	Set(N,xx,yy);

	delete [] xx;
	delete [] yy;

}



void TSRespFunc::Set(int N, double* x, double *y, double shift_x, double shift_y)
{

	//cout<<__FUNCTION__<<endl;

	if(N<=1){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Invalid Num. of Points: ",N);
		return;
	}

	if(!x || !y){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," Null ptr to array");
		return;
	}


	fClear();

	fN = N;
	fX = new double[N];
	fY = new double[N];


	int NUnits=0;

	fIsUnit = false;

	for(int i=0; i<N; ++i){

		fX[i]=x[i] + shift_x;
		fY[i]=y[i] + shift_y;

		if( TMath::AreEqualRel(fY[i], 1., kToleranceToUnity) ){
			NUnits++;
		}

		//cout<<" --- "<<i<<" "<<x[i]<<" "<<y[i]<<endl;
	}

	if(NUnits==N){
		fIsUnit = true;
	}

	SetActive(true);

	fCheckAnalytic();

	fFillGraph();

	//cout<<__FUNCTION__<<" out"<<endl;

}




void Set(int N, double *x, double *y, TSArgList& control_var_list, double shift_x, double shift_y)
{


}

void Set(int N, float *x, float *y, TSArgList& control_var_list)
{


}




double TSRespFunc::fCalcSpline3Sigma(double x, int opt) const
{


	   double InParam = x;

	  double Yspline = 999.;
	  double Y1 = 999.;
	  double Y2 = 999.;
	  double X1 = 999.;
	  double X2 = 999.;

	  double Xcentr = fX[3];
	  double Ycentr = fY[3];

	  double Xp1sigma = fX[4];
	  double Yp1sigma = fY[4];

	  double Xm1sigma = fX[2];
	  double Ym1sigma = fY[2];

	  double Xp2sigma = fX[5];
	  double Yp2sigma = fY[5];

	  double Xm2sigma = fX[1];
	  double Ym2sigma = fY[1];

	  double Xp3sigma = fX[6];
	  double Yp3sigma = fY[6];

	  double Xm3sigma = fX[0];
	  double Ym3sigma = fY[0];

	  // Select the parameter range and decide if to interpolate or extrapolate
	  // Since it is most probable to make a spline closer to the central value
	  // (the parameter is generated gaussian and the constraint is gaussian)
	  // start the if condition close to the central value and get farther from it

	  if(InParam>=Xm1sigma && InParam<Xcentr){   // (-1,0) --> Interpolate
	    X1 = Xm1sigma;
	    Y1 = Ym1sigma;
	    X2 = Xcentr;
	    Y2 = Ycentr;
	  }
	  else if(InParam>=Xcentr && InParam<Xp1sigma){     // (0,+1) --> Interpolate
	    X1 = Xcentr;
	    Y1 = Ycentr;
	    X2 = Xp1sigma;
	    Y2 = Yp1sigma;
	  }
	  else if(InParam>=Xm2sigma && InParam<Xm1sigma){    // (-2,-1) --> Interpolate
	    X1 = Xm2sigma;
	    Y1 = Ym2sigma;
	    X2 = Xm1sigma;
	    Y2 = Ym1sigma;
	  }
	  else if(InParam>=Xp1sigma && InParam<Xp2sigma){    // (1,2) --> Interpolate
	    X1 = Xp1sigma;
	    Y1 = Yp1sigma;
	    X2 = Xp2sigma;
	    Y2 = Yp2sigma;
	  }
	  else if(InParam>=Xm3sigma && InParam<Xm2sigma){  // (-3,-2) --> Interpolate
	    X1 = Xm3sigma;
	    Y1 = Ym3sigma;
	    X2 = Xm2sigma;
	    Y2 = Ym2sigma;
	  }
	  else if(InParam>=Xp2sigma && InParam<Xp3sigma){  // (2,3) --> Interpolate
	    X1 = Xp2sigma;
	    Y1 = Yp2sigma;
	    X2 = Xp3sigma;
	    Y2 = Yp3sigma;
	  }
	  else if(InParam<Xm3sigma){   // (-inf,-3) --> Extrapolate
	    X1 = Xm3sigma;
	    Y1 = Ym3sigma;
	    X2 = Xm2sigma;
	    Y2 = Ym2sigma;
	  }
	  else if(InParam>=Xp3sigma){   // (3,+inf) --> Extrapolate
	    X1 = Xp2sigma;
	    Y1 = Yp2sigma;
	    X2 = Xp3sigma;
	    Y2 = Yp3sigma;
	  }
	  else{

	    MSG::ERROR(__FILE__,"::",__FUNCTION__);
	    assert(0);

	  }

	  // Calculate the Spline

	  Yspline = (Y2-Y1)/(X2-X1)*(InParam-X1) + Y1;

	  //if(InParam>=Xm3sigma && InParam<Xm1sigma) cout << "Calc Output = " << Yspline << "  ";

	return Yspline;

}


void TSRespFunc::fCheckAnalytic()
{

	if( IsUnit() ) return;

	fCheckLinearity();

	if( IsLinear() ) return;

	fCheckParab();


}


void TSRespFunc::fCheckParab()
{

	//cout<<__FUNCTION__<<endl;

	if( fN<=2 ) return;

	double x1 = fX[0];
	double y1 = fY[0];

	double x2 = fX[fN/2];
	double y2 = fY[fN/2];

	double x3 = fX[fN-1];
	double y3 = fY[fN-1];

	// y = A + B*X + C*X*X

	double D1 = (x1-x2)*(x1-x3);
	double D2 = (x2-x1)*(x2-x3);
	double D3 = (x3-x1)*(x3-x2);

	if( D1==0 || D2==0 || D3==0 ){
		return;
	}

	double C = y1/D1 + y2/D2 + y3/D3;

	double B = -( (x2+x3)*y1/D1 + (x1+x3)*y2/D2 + (x1+x2)*y3/D3 );

	double A = x2*x3*y1/D1 + x1*x3*y2/D2 + x1*x2*y3/D3;

	if( fN == 3 ){

		fIsParab = true;
		fParsPoly[0]=A;
		fParsPoly[1]=B;
		fParsPoly[2]=C;

		return;
	}



	TGraph gp;

	int Naligned = 3;

	for(int i=0; i<fN; ++i){

		double x = fX[i];
		double y = fY[i];

		double t1 = y1*(x-x2)*(x-x3)/((x1-x2)*(x1-x3));

		double t2 = y2*(x-x1)*(x-x3)/((x2-x1)*(x2-x3));

		double t3 = y3*(x-x1)*(x-x2)/((x3-x1)*(x3-x2));

		//must be yfunc1 = yfunc2
		double yfunc1 = t1 + t2 + t3;

		double yfunc2 = A + B*x + C*x*x;

		//cout<<"xcheck "<<yfunc<<" "<<yy2<<endl;
		//if(!TMath::AreEqualAbs(yfunc,yy2,1e-6)){
		//	cout<<"not "<<endl;
		//		assert(0);
		//}

		gp.SetPoint(gp.GetN(),x,yfunc2);

		if( TMath::AreEqualRel(yfunc2, fY[i], 5*kToleranceToUnity) ){
			Naligned++;
		}

	}

	if(Naligned == fN){

		fIsParab = true;

		fParsPoly[0]=A;
		fParsPoly[1]=B;
		fParsPoly[2]=C;

		/*

		fFillGraph();

		static int np =0; np++;
		cout<<"=== Parab ==== "<<np<<endl;

		GetGraph(Form("parab%i",np))->Write();

		gp.SetName(Form("parabf%i",np));
		gp.Write();

		Print();

		*/

	}

	//cout<<__FUNCTION__<<" out "<<endl;

}

void TSRespFunc::fCheckLinearity()
{


	/**
	 *
	 * likely a chi2 test would be better
	 *
	 */

	//cout<<__FUNCTION__<<endl;



	if( fN <=1 ) return;

	double xfirst = fX[0];
	double yfirst = fY[0];


	double xlast = fX[fN-1];
	double ylast = fY[fN-1];

	if( xfirst == xlast ) return;

	// y = slope*x + b
	double slope = (ylast-yfirst)/(xlast-xfirst);
	double b = -xfirst*slope + yfirst;



	if( fN == 2 ){

		fIsLinear = true;
		fParsPoly[0] = b;
		fParsPoly[1] = slope;

		return;
	}




	int Naligned = 2;

	for(int i=1; i<fN-1; ++i){


		double interp_y = b + slope*fX[i];

		if( TMath::AreEqualRel(interp_y, fY[i], 5*kToleranceToUnity) ){
			Naligned++;
		}


	}

	if(Naligned == fN){

		fIsLinear = true;
		fParsPoly[0] = b;
		fParsPoly[1] = slope;

		/*
		static int nl =0; nl++;

        fFillGraph();

        cout<<"==line: == "<<nl<<endl;

        GetGraph(Form("linear%i",nl))->Write();

       Print();
       */
	}

   // cout<<"----"<<endl;

}

void TSRespFunc::fClear()
{

	fN=0;

	if( fX ) delete [] fX;
	if( fY ) delete [] fY;

	fX = 0;
	fY = 0;

	fShift[0]=0;
	fShift[1]=0;

	fXRange[0]=0;
	fXRange[1]=0;
	fYRange[0]=0;
	fYRange[1]=0;

	fParsPoly[0]=0;
	fParsPoly[1]=0;
	fParsPoly[2]=0;


	fIsActive = false;
	fIsLinear = false;
	fIsParab = false;
	fIsUnit = false;

	fHasControlVars = false;
	fNumOfControlVars = 0;

	fVarsVolume.Clear();

	fGraph.Clear();


}




void TSRespFunc::fFillGraph()
{

	fGraph.Clear();

	for(int i=0; i<fN; ++i){

		fGraph.SetPoint(fGraph.GetN(),fX[i],fY[i]);
	}



}

void TSRespFunc::ShiftAxis(double dx, double dy)
{

	for(int i=0; i<GetN(); ++i){

		fX[i] += dx;
		fY[i] += dy;
	}

	fCheckAnalytic();

	fFillGraph();

}


