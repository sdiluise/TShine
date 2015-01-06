/*
 * TSChi2ConstrainCalc.cc
 *
 *  Created on: Oct 4, 2014
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 */

#include <cassert>


#include "MessageMgr.h"

#include "TSChi2ConstraintCalc.h"





TSChi2ConstraintCalc::TSChi2ConstraintCalc() {

	Init();
}

TSChi2ConstraintCalc::~TSChi2ConstraintCalc() {


}


void TSChi2ConstraintCalc::Init()
{

	Clear();

}


void TSChi2ConstraintCalc::Clear()
{

	fDimension = 0;


	for(int i=0; i<kDimension; ++i){

		fCacheParCenter[i] = 0;

		fParamArray[i] = 0;
	}


}

double  TSChi2ConstraintCalc::Compute() const
{

	return Eval();

}

void TSChi2ConstraintCalc::Configure(const TSNuiParamSet &pars, const TSCovMatrixMgr &cov)
{



	 int D = pars.GetSize();


	 if( D > kDimension ){

		 MSG::ERROR(__FILE__,"::",__FUNCTION__," Num. of Parameters exceeds max default dimension: ",kDimension);
		 MSG::ERROR(" Do change coded value first");
		 assert(0);

	 }

	 if( D != cov.GetDimension() ){

		 MSG::ERROR(__FILE__,"::",__FUNCTION__," Parameter Vector and Cov Matrix have different dimension: ",D,", ",cov.GetDimension());
		 assert(0);

	 }


	 /* This only for the method where a TMatrix is passed
	 if( !cov.IsSymmetric() ){
		 MSG::ERROR(__FILE__,"::",__FUNCTION__," Covariance Matrix is Not Symmetric");
	 }
	  */


	 /*
	  * Add this check
	  *
	 TDecompLU lu(a);

	 TMatrixD b;
	 if (!lu.Decompose()) {
		 cout << "Decomposition failed, matrix singular ?" << endl;
		 cout << "condition number = " << = a.GetCondition() << endl;
	 } else {
		 lu.Invert(b);
	 }
	 *
	 */


	 fDimension = D;


	 fParamSet.Clear();

	 fParamSet.Add(pars);


	 for(int i=0; i<D ; ++i){

    	TSNuiParam *p = (TSNuiParam*)pars.At(i);

    	fCacheParCenter[i] = p->GetPriorValue();

    	fCacheParValue[i] = 0;

    	fParamArray[i] = p;
    }



	 fMatCov.ResizeTo(D,D);
	 fMatW.ResizeTo(D,D);

	 for(int ix=0; ix<D ; ++ix){
		 for(int iy=0; iy<D ; ++iy){

			 fMatCov[ix][iy] = cov.GetElement(ix,iy);
			 fMatW[ix][iy] = cov.GetElement(ix,iy);

		 }
	 }



	 fMatW.Invert();

     //fMatW.Print();

	 TMatrixD U(fMatCov,TMatrixD::kMult,fMatW);

	 //This must be unitary.
	 U.Print();

	 /**
	  *
	       0  1  2     0,0   0,2
	       3  4  5  =
	       6  7  8     2,0   2,2

	       ilinear = ix*D + iy

	       (0 1 2) (3 4 5) (6 7 8)

	       (0,2) -> 0*3+2 = 2
	       (1,1) -> 1*3+1 = 4
	       (2,0) -> 2*3+1 = 6
	       (2,2) -> 2*3+2 = 8


	  */

	 int n=0;

	 for(int ix=0; ix<D ; ++ix){
		 for(int iy=0; iy<D ; ++iy){

			 int ilinear= ix*D + iy;

			 fCacheWeightsArray[ilinear] = fMatW[ix][iy];
			 fCacheWeightsArrayIdx[ilinear] = ix;
			 fCacheWeightsArrayIdy[ilinear] = iy;

			 //cout<<"ilinear "<<n<<" "<<ix<<" "<<iy<<" "<<ilinear<<"; "<<fCacheWeightsArrayIdx[ilinear]<<" "<<fCacheWeightsArrayIdy[ilinear]<<endl;
			 //n++;
		 }
	 }




}



double  TSChi2ConstraintCalc::Eval() const
{

	//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;


	/**
	 *  Three looping methods, they are checked to give the same
	 *  result.
	 *  The linearized one is the fastest:
	 *    x2 w.r.t. the array nested loop
	 *    x10 (??)  w.r.t. the nested loop with param calls from the TSNuiSetList (linked list)
	 *
	 */
	double fChi2 = 0;


	int n=0;
/*

	for(int ix=0; ix<fDimension ; ++ix){

		double d1= ( fCacheParCenter[ix] - fParamArray[ix]->GetValue() );

		for(int iy=0; iy<fDimension ; ++iy){

			fChi2 += d1 // ( fCacheParCenter[ix] - fParamArray[ix]->GetValue() )
					* ( fMatW[ix][iy]  )
					* ( fCacheParCenter[iy] - fParamArray[iy]->GetValue() );

			//if(n<5){
			//	cout<<n<<" "<<ix<<" "<<iy<<" "<<fCacheParCenter[ix]<<" "<<fMatW[ix][iy]<<" "<<fCacheParCenter[iy]<<endl;
			//}
			//n++;

		}
	}

	double chi21 = fChi2;

	//cout<<" ----- "<<endl;

*/


	int N = fDimension*fDimension;

	n=0;

	fChi2 = 0;

	for( int ilin=0; ilin<N; ++ilin ){

		int ix = fCacheWeightsArrayIdx[ilin];
		int iy = fCacheWeightsArrayIdy[ilin];

		//fChi2 += (fCacheParCenter[ix] - fParamArray[ix]->GetValue()) * fCacheWeightsArray[ilin] * (fCacheParCenter[iy]-fParamArray[iy]->GetValue());
		fChi2 += (fParamArray[ix]->GetPriorValue() - fParamArray[ix]->GetValue() )*(fCacheWeightsArray[ilin])*(fParamArray[iy]->GetPriorValue() - fParamArray[iy]->GetValue());

		//if(n<5){
		//  cout<<endl;
		//  cout<<" n "<<n<<"; "<<fParamArray[ix]->GetName()<<" "<<fParamArray[iy]->GetName()<<endl;
		//  cout<<ix<<" "<<iy<<" "<<fCacheParCenter[ix]<<" "<<fCacheWeightsArray[ilin]<<" "<<fCacheParCenter[iy]<<" ; "<<ilin<<endl;
		//  cout<<" "<<fParamArray[ix]->GetValue()<<" "<<fParamArray[iy]->GetValue()<<endl;
		//}
		//n++;

	}

	double chi22 = fChi2;



/*
	fChi2 = 0;
	for(int ix=0; ix<fDimension ; ++ix){

		TSNuiParam *px = (TSNuiParam*)fParamSet.At(ix);

		double d1 = ( fCacheParCenter[ix] - px->GetValue() );

		for(int iy=0; iy<fDimension ; ++iy){

			TSNuiParam *py = (TSNuiParam*)fParamSet.At(iy);

			fChi2 +=  d1
					* ( fMatW[ix][iy]  )
					* ( fCacheParCenter[iy] - py->GetValue() );

		}
	}


	//double chi23 = fChi2;

	//cout<<chi21<<" "<<chi22<<" "<<chi23<<endl;
*/


	return fChi2;
}


