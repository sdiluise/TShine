/*
 * TH2FAuto.h
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *      		Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

/**

   Class for a TH2F obj with Automatic Binning 
   A TTree (Buffer) is filled until fMaxBufferSize 
   (SetMaxBufferSize - defalut is 1000 entries),
   is reached FlushBuffer is called: the htemp global histo is created
   via TTree::Draw() and the htemp binning is used to
   built the current histogram.
   fBuffer is then flushed into the histogram;
   If fMaxBufferSize is set to zero, it is up to the user 
   to call FlushBuffer.
   After the histo is built TH2F::Fill() will be called. 

   If RangeLimits are specified then outliers are not used to
   built the histogram, they will be filled in afterwards.  

   Different Histogram Built strategy can be implemented other
   then the one behind the TTree::Draw() method (Strategy 1).

   
 **/

#ifndef TH2FAUTO_H_
#define TH2FAUTO_H_


#include <TH2F.h>
#include <TTree.h>


class TH2FAuto: public TH2F {

public:

  TH2FAuto();
  TH2FAuto(const char* name, const char* title);
  TH2FAuto(const char* name, const char* title, double xlow, double xup, double ylow, double yup);
  TH2FAuto(const char* name, const char* title, int,double, double, int, double, double);
  TH2FAuto(const char* name, const char* title, int,double*, int, double*);

  TH2FAuto(const TH2FAuto &);

  virtual ~TH2FAuto();

  int BufferEmpty(int);

  void Copy(const TH2FAuto &);
  TObject* Clone(const char* newname = "") const;

  void Draw(Option_t* opt);

  int  Fill(double,double);
  int FillBufferTo(TH2FAuto &);
  void FixBinningX(int n, double low, double up);
  void FixBinningY(int n, double low, double up);
  void FixBinningX(int n, const double* bins);
  void FixBinningY(int n, const double* bins);
  void FlushBuffer();
  int  GetBufferSize();
  double GetMean(int axis=1);
  double GetRMS(int axis=1);

  int GetFixedNbinsX() const;
  int GetFixedNbinsY() const;
  const double* GetFixedBinsArrayX() const;
  const double* GetFixedBinsArrayY() const;

  double GetLowRangeLimitX() const;
  double GetLowRangeLimitY() const;
  double GetUpRangeLimitX() const;
  double GetUpRangeLimitY() const;

  bool HasLowRangeLimitX() const;
  bool HasLowRangeLimitY() const;
  bool HasUpRangeLimitX() const;
  bool HasUpRangeLimitY() const;


  bool IsAuto() const {return fIsAuto;}
  bool IsBuilt() const {return fIsBuilt;}
  bool IsBinningXFix() const {return fFixBinningX;}
  bool IsBinningYFix() const {return fFixBinningY;}
  void Reset();
  void ResetBuffer();
  void SetMaxBufferSize(int);
  void SetLowRangeLimitX(double);
  void SetLowRangeLimitY(double);
  void SetRangeLimits(double xlow, double xup, double ylow, double yup);
  void SetRangeLimitsX(double low, double up);
  void SetRangeLimitsY(double low, double up);
  void SetStrategy(int);
  void SetUpRangeLimitX(double);
  void SetUpRangeLimitY(double);


  int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);

private:

  void fClearRangeLimits();
  void fFillBuffer(double,double);
  void fInit();
  void fSetBuffer();

  TTree fBuffer;
  TTree fBufferOut;

  bool fIsAuto;
  bool fIsBuilt;
  int fMaxBufferSize;
  int fStrategy;
  bool fFixBinningX;
  bool fFixBinningY;
  bool fFixAxisVariableBins[2];
  int fFixNumOfBins[2];
  double fFixLimitsX[2];
  double fFixLimitsY[2];

  TAxis  fFixedAxisX;
  TAxis  fFixedAxisY;

  bool   fXRangeHasLimit[2];
  bool   fYRangeHasLimit[2];
  double fXRangeLimit[2];
  double fYRangeLimit[2];


  double fX;
  double fY;

};

#endif /* TH2FAUTO_H_ */
