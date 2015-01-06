/*
 * StatUtils.h
 *
 *  Created on: Nov 9, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#ifndef STATUTILS_H_
#define STATUTILS_H_


#include <iostream>
#include <limits>
#include <cmath>

#include <string>
#include <TString.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

#include <TObject.h>
#include <TMath.h>
#include <TRandom.h>

class TH2;
class TH1;
class TH2F;
class TH1F;
class TH2Poly;

using namespace std;

using std::cout;
using std::endl;

/*
 *
 */

namespace StatUtils {


	double Chisquare(double *x, double *p){

		//p[0] Normaliation, p[1] degree of freedom
		//const needed to avoid too large value of the normalization param p[0]
		//i.e.: for p[1]=dof=50  Gamma(50/2)=6e23;

		double half_mu = 0.5*p[1];

		double D = pow(2,half_mu) * TMath::Gamma(half_mu);

		return (1/D)*p[0]*pow(x[0],(half_mu-1))*exp( -0.5*x[0] );
	}

	double GetChi2(double x, double n){

		double half_nu = 0.5*n;

		double D = pow(2,half_mu) * TMath::Gamma(half_mu);

		return (1/D)*pow(x,(half_mu-1))*exp( -0.5*x );

	}

};

#endif /* STATUTILS_H_ */
