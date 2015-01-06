/*
 * TSToFGenerator.cc
 *
 *  Created on: Jun 16, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */


#include <TRandom.h>
#include <TAxis.h>

#include "StringUtils.h"

#include "TSToFGenerator.h"


#include "MessageMgr.h"



TRandom RRAN(0);



namespace TSToF_Generator{


	TRandom RRAN2(0);


    double Gaussian(double *x, double *p){

    	/**
    	 *  Normalized Gaussian
    	 *  p[0] mean
    	 *  p[1] sigma
    	 *
    	 */

    	return TMath::Gaus(x[0],p[0],p[1],1);
    }


	double DoubleTailedGaussian(double *x, double *p){

		/**
		 *
		 *
		 *
		 *  Normalized
		 *
		 *  p[0] mean
		 *  p[1] smaller sigma
		 *  p[2] larger sigma
		 *  p[3] amplitude fraction of the larger gaussian
		 *
		 *
		 *
		 */

		double m = p[0];
		double s1 = p[1];
		double s2 = p[2];
		double frac = p[3];


		double val = (1-frac)*TMath::Gaus(x[0],m,s1,1) + frac*TMath::Gaus(x[0],m,s2,1);

		return val;

	}//


}

//=========================================================================
//
//
//
//
//
//
//=========================================================================



TSToF_Part::TSToF_Part(TString name, TString label, float mass)
: Name(name), Label(label), Mass(mass), fMass2IsGauss(0)
{


  SetMassScale(0);
  SetWidthPar(-1);
  SetUseMassScale(0);

  fNpx=300;

  fMass2Distrib = 0;

}


TSToF_Part::~TSToF_Part(){


	for(std::map<int,TF1*>::iterator it=fParamFunc.begin(), end=fParamFunc.end();
			it!=end;
			++it){

		delete it->second;
	}


}


TF1* TSToF_Part::GetMass2Distrib(double mom) const{


	double fNSigRange=7;

	if( !fMass2Distrib ) return 0;


	if( Mass2IsGaussian() ){

		//
		// For a gaussian is clear which is the mean and width-parameter to use to set the
		// range.
		//
		double m = GetParamValue(0,mom);
		double s = GetParamValue(1,mom);

		//cout<<" CentralMoment: "<<GetName()<<" mom "<<mom<<"; m "<<m<<" w "<<s<<"; "<<fMass2Distrib->GetXmin()<<" "<<fMass2Distrib->GetXmax()<<endl;

		fMass2Distrib->SetParameters(m,s);
		fMass2Distrib->SetRange(m-fNSigRange*s,m+fNSigRange*s);
		fMass2Distrib->SetNpx(fNpx);



		return fMass2Distrib;



	}else{


		int NP = GetNpar();

		for(int ipar=0; ipar<NP; ++ipar){

			double val = GetParamValue(ipar,mom);

			fMass2Distrib->SetParameter(ipar,val);

		}

		if( GetWidthParam()>=0 && GetWidthParam()<NP ){

			//
			// If not a Gaussian but the inofrmation on which one is
			// the width-parameter the the range is set with the same
			// procedure.
			// N.B. that the mean parameter is assumed to be
			// always the first one
			//

			double w=fMass2Distrib->GetParameter( GetWidthParam() );

			double m = fMass2Distrib->GetParameter(0);

			fMass2Distrib->SetRange(m-fNSigRange*w,m+fNSigRange*w);




		}else{

			// If not a gaussian and no informantion on the
		    // distribution width parameter then the width of the
		    // distribution is calculated numerically via the central
		    // moment of order two
			// in this case the computation itself depends on the range
			// then the original range from the tof-variable fVariableY
			// is taken as input (fVariableY must be set with a range)

			double m = fMass2Distrib->GetParameter(0);

			double xmin=1; double xmax=1;

			if(!fVariableY.HasRange()){
				MSG::ERROR(__FILE__,"::",__FUNCTION__," ToF variable not set cannot set initial range to compute the CentralMoment");
			}else{

				xmin=fVariableY.GetMin();
				xmax=fVariableY.GetMax();
			}

			double rms = fMass2Distrib->CentralMoment(2,xmin,xmax);

			//cout<<"central mom "<<GetName()<<" "<<xmin<<" "<<xmax<<" "<<mom<<endl;

			if( rms==TMath::Infinity() || TMath::IsNaN(rms) || rms<0 ){
				MSG::WARNING(__FILE__,"::",__FUNCTION__," Failed to calculated Central moment for par ",fMass2Distrib->GetName(), " at momentum ",mom);

				rms=(fMass2Distrib->GetXmax()-fMass2Distrib->GetXmin())/TMath::Sqrt(12.);
			}else{

				rms=TMath::Sqrt(rms);
			}

			fMass2Distrib->SetRange(m-fNSigRange*rms,m+fNSigRange*rms);


		}

		fMass2Distrib->SetNpx(fNpx);
		return fMass2Distrib;

	}


}


TF1* TSToF_Part::GetParamFunction(int ip) const{

	if( fParamFunc.count(ip)==1 ){

		return fParamFunc.find(ip)->second;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," not present a param number ", ip);
		return new TF1();
	}

}



double TSToF_Part::GetParamValue(int ipar, double mom) const{



	if( GetParamFunction(ipar) ){

		double val = GetParamFunction(ipar)->Eval(mom);

		double scale = 1;

		if ( UseMassScale() ){
			scale = GetMassScaling(ipar,mom);
		}

		return val*scale;
	}

	return 0;


}



double TSToF_Part::GetMassScaling(int par, double P) const {


	/**
	 *
	 * Parameter of type Width is scaled according to the
	 * formula which depends on the momentum P the mass
	 * of the particle and the mass set for the scaling
	 *
	 *
	 *
	 */


	if(P<=0) return 1;

	double scale=1;

	if( par==GetWidthParam() ){
		scale = ( 1 - 0.5*(Mass*Mass/(P*P))  )/( 1 - 0.5*(fMassScale*fMassScale/(P*P) ) );
		cout<<"   scaling for "<<GetName()<<" on param: "<<par<<"; scale= "<<scale<<endl;
	}



	return scale;
}



void TSToF_Part::SetMass2Distrib(TString name, double xmin, double xmax, int width_par){


	if( name == "gaus" ){

		delete fMass2Distrib;

		TString name=STR_UTILS::Name("ToF_Mass2","_","gaus","_",GetName());

		fMass2Distrib = new TF1(name,TSToF_Generator::Gaussian,xmin,xmax,2);
		fMass2Distrib->SetParameters(0.5*(xmin+xmax),0.5*(xmin-xmax));//dummy
		fMass2IsGauss = true;
		fMass2DistribName="gaus";


		TString title=STR_UTILS::Name(GetLabel()," - ",fVariableY.GetLabel()," distribution ","");
		TString titlexy=STR_UTILS::TitleXYZ(title,fVariableY.GetAxisTitle());

		fMass2Distrib->SetTitle(titlexy);
		//fMass2Distrib->GetXaxis();//->SetTitle(fVariableY.GetAxisTitle());

		fMass2Distrib->SetParNames("mean","#sigma");

		SetWidthPar(1);


	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," function not known: ",name);
		return;

	}


}



void TSToF_Part::SetMass2Distrib(TF1 &f, int width_par){


	if(fMass2Distrib) delete fMass2Distrib;


	fMass2Distrib = new TF1(f); //(TF1*)f.Clone("ToF_Mass2_Distr");

	TString name=STR_UTILS::Name("ToF_Mass2","_",f.GetName(),"_",GetName());

	fMass2Distrib->SetName(name);
	fMass2Distrib->SetRange(f.GetXmin(),f.GetXmax());

	TString title=STR_UTILS::Name(fVariableY.GetLabel()," distribution ",GetLabel());
	TString titlexy=STR_UTILS::TitleXYZ(title,fVariableY.GetAxisTitle());

	fMass2Distrib->SetTitle(titlexy);

	for(int i=0;i<f.GetNpar();++i){
		fMass2Distrib->SetParName(i,f.GetParName(i));
	}

	if( width_par>=0 && width_par<f.GetNpar() ){
		SetWidthPar(width_par);
	}

	cout<<" adding width par "<<name<<" "<<width_par<<" "<<f.GetNpar()<<" " <<GetWidthParam()<<endl;

	fMass2IsGauss = false;
	fMass2DistribName = "";

}


void TSToF_Part::SetMomentumVariable(const TSVariable &Mom){

	fVariableX.Copy(Mom);

}


void TSToF_Part::SetParamFunc(int ip, TF1 &f){

	/**
	 *  Paramterization (as a function of the momentum)
	 *  of the paramter ip of the Mass2Disribution function
	 *
	 */


	if( fParamFunc.count(ip) ){

		delete fParamFunc[ip];
	}

	TString fname = "ToF_Mass2_ParFunc_"; fname+=GetName();
	fname+="_"; fname+=ip;

	fParamFunc[ip]=(TF1*)f.Clone(fname);


	if( Mass2IsGaussian() && ip==1){

		double pmax = f.GetXmax();
		double sig_max = f.Eval(pmax);

		if(fMass2Distrib){
			fMass2Distrib->SetRange(-7*sig_max,7*sig_max);
		}

	}

	TString par_name=fMass2Distrib->GetParName(ip);
	TString tit=STR_UTILS::Name(GetLabel()," - ",fVariableY.GetLabel()," distribution: parameterization of ",par_name);
	TString tity=par_name;

	TString title=STR_UTILS::TitleXYZ(tit,fVariableX.GetAxisTitle(),tity);

	fParamFunc[ip]->SetTitle(title);

}


void TSToF_Part::SetToFVariable(const TSVariable &Mass2){

	fVariableY.Copy(Mass2);

}






//===================================================
//
//
//
//
//
//====================================================


TSToFGenerator::TSToFGenerator(TString name):
TSNamed(name,"","")
{
	// TODO Auto-generated constructor stub

	Init();


}

TSToFGenerator::TSToFGenerator(TString name,const TSVariable &vx, const TSVariable &vy):
TSNamed(name,"","")
{
	// TODO Auto-generated constructor stub

	Init();

    SetMomentumVariable(vx);
    SetToFVariable(vy);


}


TSToFGenerator::~TSToFGenerator() {
	// TODO Auto-generated destructor stub



	for(std::map<TString,TSToF_Part*>::iterator it=fPartList.begin(), end=fPartList.end();
			it!=end;
			++it){

		delete it->second;
	}

	delete fToFResolDistr;
    delete fMomResol;


}



double TSToFGenerator::ComputeDm2(double tof, double mom, double len) const{

	double sigmaT = GetSigmaToF();
	double sigmaP = GetSigmaMom(mom);
	double sigmaL = GetSigmaPathLen();

	double dm2dp = ComputeDm2dp(tof,mom,len);
	double dm2dt = ComputeDm2dt(tof,mom,len);
	double dm2dL = ComputeDm2dL(tof,mom,len);

	double dm2 = sigmaT*sigmaT*dm2dt*dm2dt
			+ sigmaP*sigmaP*dm2dp*dm2dp
			+ sigmaL*sigmaL*dm2dL*dm2dL;

	return TMath::Sqrt(dm2);

}


double TSToFGenerator::ComputeDm2dp(double tof, double mom, double len) const{

	if( len ==0 ) return 0;

	return 2*mom*( (CL*CL*tof*tof)/(len*len)-1 );
}



double TSToFGenerator::ComputeDm2dt(double tof, double mom, double len) const{

	if( len == 0) return 0;

	return (mom*mom*CL*CL*2*tof)/(len*len);
}



double TSToFGenerator::ComputeDm2dL(double tof, double mom, double len) const{

	if(len==0 ) return 0;

	return mom*mom*tof*tof*CL*CL*2/(len*len*len);
}



double TSToFGenerator::ComputeMass(double tof, double mom, double len) const{

	/**
	 *  tof: ns
	 *  mom: GeV/c
	 *  len: m
	 *
	 */


	double m = mom*TMath::Sqrt( (CL*CL*tof*tof)/(len*len)-1 );

	return m;

}



double TSToFGenerator::ComputeToF(double mass, double mom, double len) const{

/**
 *  mass: GeV/c**2
 *  mom: GeV/c
 *  len: m
 *
 */


	double beta = mom/TMath::Sqrt(mass*mass+mom*mom);


	double tof = len/(beta*CL);


    double m = ComputeMass(tof,mom,len);


	return tof;

}




double TSToFGenerator::GenerateMass2(double mass, double mom, double len) const{

	double tof = ComputeToF(mass, mom, len);

	double dT = GetToFResolDistr()->GetRandom();

    tof += dT;

    //
    // this should be moved inside ComputeToF?
    //
    tof += fToFT0;



    //
    double sigP= GetSigmaMom(mom);

    double dP = RRAN.Gaus(0,sigP);

    mom += dP;

    //
    double dL = RRAN.Gaus(0,GetSigmaPathLen());

    len += dL;
    //

    double M = ComputeMass(tof,mom,len);



	return M*M;


}



double TSToFGenerator::GenerateMass2(TString name, double mom) const{



	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " not set");
		return 0;
	}


	TSToF_Part *part = fGetParticle(name);

	TF1 *fMass2 = part->GetMass2Distrib();



	if( !fMass2 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " Mass2 distribution not found");
		return 0;
	}


	if( part->GetNumOfParFuncs() != fMass2->GetNpar() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " missing parametrization for some params");
		return 0;
	}



	// faster
	if( part->Mass2IsGaussian() ){

		double m=part->GetParamValue(0,mom);
		double s=part->GetParamValue(1,mom);

		return RRAN.Gaus(m,s);
	}



	TF1 *fMass2Mom = part->GetMass2Distrib(mom);


	double mass2 = fMass2Mom->GetRandom();

	return mass2;



}




TF1* TSToFGenerator::GetMass2Distrib(TString name,  double mom) const{

	/*
	 *
	 * Setup the mass2 distrib function with the value of the
	 * parameters calculated in Momentum = mom
	 *
	 *
	 */

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return 0;
	}

	TSToF_Part *part = fGetParticle(name);

	return part->GetMass2Distrib(mom);



}

int TSToFGenerator::GetMass2DistribNPars(TString name) const{

	TSToF_Part *part = fGetParticle(name);

	if(part) return part->GetNpar();
	else return 0;

}

TF1* TSToFGenerator::ImportMass2Distrib(TString name,  double mom) const{

/*
 *  Function from GetMass2Distrib is cloned and the title is update with
 *  the value of the momentum
 *
 *
 */

	TF1 *f = GetMass2Distrib(name,mom);

	if(!f) return 0;

	//TF1 *ff= new TF1();
	//ff->Copy(*f);

	TF1 *ff=(TF1*)f->Clone();

	TString mom_set=fVariableX.GetLabelAndValue(mom);

	TString tit0=f->GetTitle();

	TString titnew=STR_UTILS::AppendToTitleXYZ(tit0," ",mom_set);

	ff->SetTitle(titnew);

	return ff;
}




TF1* TSToFGenerator::GetParametrization(TString name, int ipar) const{

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return 0;
	}


	return fGetParticle(name)->GetParamFunction(ipar);

}


double TSToFGenerator::GetParamValue(TString name, int ipar, double mom) const{

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return 0;
	}


	return fGetParticle(name)->GetParamValue(ipar,mom);

}



double TSToFGenerator::GetSigmaMom(double mom) const{

	if( GetMomResolFunc() ){

		GetMomResolFunc()->Eval(mom);

	}else{

		return 0;
	}


}



void TSToFGenerator::Init(){

	fToFResolDistr=0;
	fMomResol=0;

	SetMomentumResolution(0);
	SetPathLengthResolution(0);
	SetToFResolution(0);
	SetToFOffset(0);

	fVariableX.SetName("ToFGen_x");
	fVariableY.SetName("ToFGen_y");


	// m/ns
	CL = TMath::C()*1e-9;

}


void TSToFGenerator::SetMass2DistributionFunction(TString name, TF1& f, int width_par){


	/**
	 *
	 *   Set the Mass2 Distribution Function for a Particle
	 *
	 *   One has to specify which param number can be representative
	 *   of the distribition width, this is necessary to se properly
	 *   the x-axis range of the distribution every time all the
	 *   parameters are calculated from their own parametrization function
	 *   for a given momentum  value
	 *
	 *
	 */

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}


	TSToF_Part *part = fGetParticle(name);

	int wpar=-1;
	if( width_par>=0 && width_par< f.GetNpar() ){
		wpar=width_par;
	}

	part->SetMass2Distrib(f,wpar);


	for(int i=0;i<f.GetNpar();++i){
		part->GetMass2Distrib()->SetParName(i,f.GetParName(i));
	}





}



void TSToFGenerator::SetMass2DistributionFunction(TString name, TString fname, int width_par){


	/**
	 *
	 *   Set the Mass2 Distribution Function for a Particle
	 *   see comments for the parent method
	 *   SetMass2DistributionFunction(TString fname,TF1 &f, int width_par)
	 *
	 *
	 *
	 *
	 */

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}

	TSToF_Part *part = fGetParticle(name);

	double xmin=fVariableY.GetMin();
	double xmax=fVariableY.GetMax();

	if( fname == "gaus"){

		part->SetMass2Distrib(fname,xmin,xmax);

	}else if(fname == "double_tailed_gaus"){

		TF1 *f = new TF1("f",TSToF_Generator::DoubleTailedGaussian,xmin,xmax,4);

		f->SetName("TailedGaus");

		//dummy
		f->SetParameters(0,1,2,0.4);

		f->SetParNames("mean","#sigma","#sigma_{tail}","tail ratio");

		int wpar=-1;
		if( width_par>=0 && width_par< f->GetNpar() ){
			wpar=width_par;
		}

		part->SetMass2Distrib(*f,wpar);


		delete f;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " function name not valid");
		return;
	}




}


void TSToFGenerator::SetMass2MeanFunction(TString name, TF1& f){

	/**
	 *
	 * Set Parametrization Function of the Mass2-Mean
	 * as a function of the Momentum
	 *
	 *  Assume that Mass2 Distrib is Gaussian
	 *
	 */




	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}


	TSToF_Part *part = fGetParticle(name);

	if( part->GetMass2Distrib()==0 ){

		//if not yet set book a gaussian and set the range from SetToFVariable variable if present
		if(fVariableY.HasRange()){
			part->SetMass2Distrib("gaus",fVariableY.GetMin(),fVariableY.GetMax());
		}else{
			part->SetMass2Distrib("gaus",-1,1);
		}


	}else{

		if( !part->Mass2IsGaussian() ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," mass distrib for ",name," is not a gaussian, cannot use this method");
			return;
		}

	}

	part->SetParamFunc(0,f);


}



void TSToFGenerator::SetMass2MeanFunction(TString name, TString poly_name, double p0, double p1, double p2){

	/**
	 *
	 * Set Parametrization Function of the Mass2-Mean
	 * as a function of the Momentum
	 *
	 *  Assume that Mass2 Distrib is Gaussian
	 *
	 */




	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}

	double PARS[3];

	if( poly_name == "const" || poly_name== "pol0" ){

		poly_name = "pol0";

		PARS[0]=p0;

	}else if(poly_name == "linear" || poly_name== "pol1"){

		poly_name = "pol1";

		PARS[0]=p0;
		PARS[1]=p1;

	}else if(poly_name == "parab" || poly_name== "pol2"){

		poly_name = "pol2";

		PARS[0]=p0;
		PARS[1]=p1;
		PARS[2]=p2;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," function name ",poly_name, "not valid");
		return;

	}


	double xmin=fVariableX.GetMin();
	double xmax=fVariableX.GetMax();


	TSToF_Part *part = fGetParticle(name);

	if( part->GetMass2Distrib()==0 ){

		//if not yet set book a gaussian and set the range from SetToFVariable variable if present
		if(fVariableY.HasRange()){
			part->SetMass2Distrib("gaus",fVariableY.GetMin(),fVariableY.GetMax());
		}else{
			part->SetMass2Distrib("gaus",-1,1);
		}

	}else{

		if( !part->Mass2IsGaussian() ){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," mass distrib for ",name," is not a gaussian, cannot use this method");
			return;
		}



	}




	TF1* f= new TF1("f",poly_name,xmin,xmax);
	f->SetParameters(PARS);

	part->SetParamFunc(0,*f);

	delete f;


	return;


}



void TSToFGenerator::SetMass2WidthFunction(TString name, TF1& f){

	/**
	 *
	 *  Set Parametrization Function of the Mass2-Width
	 *  as a function of the Momentum
	 *
	 *  Assume that Mass2 Distrib is gaussian
	 *
	 */

	if( !fGetParticle(name) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}


	TSToF_Part *part = fGetParticle(name);

	if( part->GetMass2Distrib()==0 ){


		//book a gaussian and set range from the maximum width in the
		//momentum range
		double fmax= f.GetMaximum();

		part->SetMass2Distrib("gaus",-7*fmax,7*fmax);

	}else{

		if( !part->Mass2IsGaussian()){

			MSG::ERROR(__FILE__,"::",__FUNCTION__," mass distrib for ",name," is not a gaussian");
			return;
		}

		// if a gaussian already booked set range from the maximum width in the
		// momentum range
		double fmax= f.GetMaximum();
		part->GetMass2Distrib()->SetRange(-7*fmax,7*fmax);

	}


	part->SetParamFunc(1,f);



}


void TSToFGenerator::SetMass2WidthFunction(TString name, TString poly_name, double p0, double p1, double p2){

	/**
	 *
	 *  Set Parametrization Function of the Mass2-Width
	 *  as a function of the Momentum
	 *
	 *  Assume that Mass2 Distrib is Gaussian
	 *
	 */




	if( fGetParticle(name)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}

	double PARS[3];

	if( poly_name == "const" || poly_name== "pol0" ){

		poly_name = "pol0";

		PARS[0]=p0;

	}else if(poly_name == "linear" || poly_name== "pol1"){

		poly_name = "pol1";

		PARS[0]=p0;
		PARS[1]=p1;

	}else if(poly_name == "parab" || poly_name== "pol2"){

		poly_name = "pol2";

		PARS[0]=p0;
		PARS[1]=p1;
		PARS[2]=p2;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," function name ",poly_name, "not valid");
		return;

	}


	double xmin=fVariableX.GetMin();
	double xmax=fVariableX.GetMax();

	TSToF_Part *part = fGetParticle(name);

	TF1 f("f",poly_name,xmin,xmax);
	f.SetParameters(PARS);


	if( part->GetMass2Distrib()==0 ){

		//book a gaussian and set range from the maximum width in the
		//momentum range
			double fmax= f.GetMaximum();

			part->SetMass2Distrib("gaus",-7*fmax,7*fmax);

		}else{

			if( !part->Mass2IsGaussian()){

				MSG::ERROR(__FILE__,"::",__FUNCTION__," mass distrib for ",name," is not a gaussian");
				return;

			}

			//if a gaussian already booked  and set range from the maximum width in the
			//momentum range
			double fmax= f.GetMaximum();

			part->GetMass2Distrib()->SetRange(-7*fmax,7*fmax);



		}


	part->SetParamFunc(1,f);


	return;



}



void TSToFGenerator::SetMass2WidthScaling(TString name1, TString name2){

	/*
	 *
	 *
	 * Scale Width of Mass2 distrib for Particle 1 wrt to Particle 2
	 * Check the method TSToF_Part::GetMassScaling(int, double)
	 * N.B.: a previous parameterization of the width parameter
	 * of name1 is overridden
	 *
	 *
	 */


	if( !fGetParticle(name1) || !fGetParticle(name2) ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," missing particle ");
		return;
	}

	if( fGetParticle(name1)->GetMass()==0 || fGetParticle(name1)->GetMass()==0){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particles with zero mass ");
		return;
	}

	/**
	 *
	 */

	float mass1 = fGetParticle(name1)->GetMass();
	float mass2 = fGetParticle(name2)->GetMass();


	//
	// the scaling is applied to the parameter tagged to be the width of the distribution
	// (default is par number 1 for gaussian)
	//


	int width_par1 = fGetParticle(name1)->GetWidthParam();
	int width_par2 = fGetParticle(name2)->GetWidthParam();

	TF1 *f2 = (TF1*)fGetParticle(name2)->GetParamFunction(width_par2);

    TF1 *f1 = new TF1(*f2); //(TF1*)f2->Clone();

    fGetParticle(name1)->SetParamFunc(width_par1,*f1);

    fGetParticle(name1)->SetMassScale(mass2);

    delete f1;

}


void TSToFGenerator::SetMomentumRange(double min, double max) {

	fVariableX.SetRange(min,max);

}



void TSToFGenerator::SetMomentumVariable(const TSVariable &Mom) {

	fVariableX.Copy(Mom);

	//MSG::LOG(__FILE__,"::",__FUNCTION__," ",fVariableX.GetLabel());

}


void TSToFGenerator::SetParamFunction(TString name, int ip, TF1 &f){

	/**
	 *
	 *  Set Parametrization Function of the parameter ip
	 *  for the particle name
	 *  as a function of the Momentum
	 *
	 */


	if( fGetParticle(name)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " not set");
		return;
	}


	TSToF_Part *part = fGetParticle(name);

	if( ip<0 || ip > part->GetNpar()-1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " param index out of range: ",ip);
		return;
	}


	part->SetParamFunc(ip,f);

}



void TSToFGenerator::SetParamFunction(TString name, int ipar, TString poly_name, double p0, double p1, double p2){

	/**
	 *
	 *  Set Parametrization Function of the Mass2 distribution
	 *  parameter ipar of the particle name
	 *  as a function of the Momentum
	 *
	 *  Assume that Mass2 Distrib is Gaussian
	 *
	 */


	if( fGetParticle(name)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, "not set");
		return;
	}


	TSToF_Part *part = fGetParticle(name);

	if( ipar<0 || ipar > part->GetNpar()-1 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," particle ",name, " param index out of range: ",ipar);
		return;
	}


	double PARS[3];

	if( poly_name == "const" || poly_name== "pol0" ){

		poly_name = "pol0";

		PARS[0]=p0;

	}else if(poly_name == "linear" || poly_name== "pol1"){

		poly_name = "pol1";

		PARS[0]=p0;
		PARS[1]=p1;

	}else if(poly_name == "parab" || poly_name== "pol2"){

		poly_name = "pol2";

		PARS[0]=p0;
		PARS[1]=p1;
		PARS[2]=p2;

	}else{

		MSG::ERROR(__FILE__,"::",__FUNCTION__," function name ",poly_name, "not valid");
		return;

	}


	double xmin=fVariableX.GetMin();
	double xmax=fVariableX.GetMax();


	TF1* f= new TF1("f",poly_name,xmin,xmax);
	f->SetParameters(PARS);


	part->SetParamFunc(ipar,*f);

	delete f;


	return;

}



void TSToFGenerator::SetParticle(TString name, TString label, float mass){


	if( fGetParticle(name) ){

		MSG::WARNING(__FILE__,"::",__FUNCTION__," replacing particle: ",name);

		delete fPartList[name];
	}


	fPartList[name] = new TSToF_Part(name,label,mass);
	fPartList[name]->SetMomentumVariable(fVariableX);
	fPartList[name]->SetToFVariable(fVariableY);



}






void TSToFGenerator::SetMomentumResolution(double p0, double p1, double p2) {


	if( !fMomResol ){
		fMomResol = new TF1("MomResol","pol2");
	}

	fMomResol->SetParameters(p0,p1,p2);

}

void TSToFGenerator::SetPathLengthResolution(double dL) {

	fSigmaLen = dL;

}


void TSToFGenerator::SetToFRange(double min, double max) {

	fVariableY.SetRange(min,max);

}


void TSToFGenerator::SetToFResolution(double dT) {


	if( !fToFResolDistr ){
		fToFResolDistr = new TF1("TofResol","gaus",-10*dT,10*dT);
		fToFResolDistr->SetParameters(1,0,dT);
		fSigmaToF = dT;
	}else{
		fToFResolDistr->SetParameter(1,dT);
		fToFResolDistr->SetRange(-10*dT,10*dT);
		fSigmaToF=dT;
	}


}



void TSToFGenerator::SetToFResolution(TF1& f) {


	delete fToFResolDistr;

    fToFResolDistr = new TF1(f); //(TF1*)f.Clone("ToFResol");
    fToFResolDistr->SetName("ToFResol");

    fSigmaToF = fToFResolDistr->CentralMoment(2,fToFResolDistr->GetXmin(),fToFResolDistr->GetXmax());

}

void TSToFGenerator::SetToFVariable(const TSVariable &Mass2) {

	fVariableY.Copy(Mass2);

}

void TSToFGenerator::SetToFOffset(double T0) {

	fToFT0 = T0;
}






TSToF_Part* TSToFGenerator::fGetParticle(TString name) const{


	if( fPartList.count(name)==1 ){

		return fPartList.find(name)->second;

	}else{

		return 0;
	}



}
