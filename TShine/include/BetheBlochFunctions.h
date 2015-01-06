/*
 * BetheBlochFunctions.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *     Name-spaced Folder of Bethe-Bloch parametrizations
 *
 *     Functions must be declared static to avoid
 *     "Duplicate Symbol" error at compile time in the case
 *     the file is included multiple times
 *
 *
 *
 */

#ifndef BETHEBLOCHFUNCTIONS_H_
#define BETHEBLOCHFUNCTIONS_H_


#include <cmath>



namespace BETHE_BLOCH_FUNC{

    //
    // some params for initialization
	//
	static const double PAR_INPUT[5]={0.0663945,9.04358,2.49409,0.164471,0.216649};


   	//==================================================


	static double BetheBloch(double *x, double *par){

	  /**
	   *  5-Parameters B-B Function Implementation
	   *
	   *
	   */

		const double bg = x[0];
		//const double bg = pow(10,x[0]);
		const double beta2 = bg*bg / (1.0 + bg*bg);
		const double lnbg = log(bg);
		const double X = log10(bg);
		//const double X = x[0];

		double E0 = par[0];
		double C  = par[1];
		double XA = par[2];
		double a  = par[3];

		//if(a==0) a=0.001;

		/*
		E0=0.0693708;
		C =11.7495;
		XA=2.15453;
		a =0.389196;
		//if(a==0) a=0.001;
		 */

		 /*
		E0=0.0693708;
		C =11.7495;
		XA=2.15453;
		a =0.389196;
		*/

		  // density effect correction as parametrised in Sternhaimer PhysRevB 1971
		const double common = sqrt(2.0*log(10)/(a*3.0)) /  3.0;
		const double X0 = XA - common;          // from continuity conditions
		const double X1 = XA + 2.0*common;    // from continuity conditions
		double delta = 0; // the correction
		if (X > X0) {
			delta += 2.0 * log(10) * (X - XA);
			if (X < X1)
				delta += a * (X1-X)*(X1-X)*(X1-X);
		}

		return E0/beta2 * (C + 2.0*lnbg - beta2 - delta) + par[4];

	}

	//==================================================



}




#endif /* BETHEBLOCHFUNCTIONS_H_ */
