/*
 * TH1FAuto.h
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 */

/**

   Class for a TH1F obj with Automatic Binning 
   A TTree (Buffer) is filled until fMaxBufferSize 
   (SetMaxBufferSize - defalut is 1000 entries),
   is reached FlushBuffer is called: the htemp global histo is created
   via TTree::Draw() and the htemp binning is used to
   built the current histogram.
   fBuffer is then flushed into the histogram;
   If fMaxBufferSize is set to zero, it is up to the user 
   to call FlushBuffer or BufferEmpty with option = 4
   if method is called by a base class.
   After the histo is built TH1F::Fill() will be called. 

   If RangeLimits are specified then outliers are not used to
   built the histogram, they will be filled in afterwards.  

   Different Histogram Built strategy can be implemented other
   than the one behind the TTree::Draw() method (Strategy 1).

   
 **/

#ifndef TH1FAUTO_H_
#define TH1FAUTO_H_


#include <TObject.h>
#include <TH1F.h>
#include <TTree.h>


class TH1FAuto: public TH1F {

public:

  TH1FAuto();
  TH1FAuto(const char* name, const char* title);
  TH1FAuto(const char* name, const char* title, double low, double up);
  TH1FAuto(const char* name, const char* title, int, double xmin, double xmax);
  TH1FAuto(const char* name, const char* title, int, double* bins);

  TH1FAuto(const TH1FAuto &);

  virtual ~TH1FAuto();

  int BufferEmpty(int);

  void Copy(const TH1FAuto &);
  TObject* Clone(const char* newname = "") const;

  void Draw(Option_t* opt);

  int  Fill(double);
  void FillBufferTo(TH1FAuto &);
  void FillBufferTo(double *);
  void FlushBuffer();
  int  GetBufferSize() const;
  double GetMean(int axis=1);
  double GetRMS(int axis=1);
  double GetLowRangeLimitX() const;
  double GetUpRangeLimitX() const;
  TH1F* GetTH1F() { return static_cast<TH1F*>(this); }
  bool HasLowRangeLimitX() const;
  bool HasUpRangeLimitX() const;
  bool IsAuto() const {return fIsAuto;}
  bool IsBuilt() const {return fIsBuilt;}
  void Reset();
  void ResetBuffer();
  void SetMaxBufferSize(int);
  void SetLowRangeLimitX(double);
  void SetRangeLimitsX(double, double);
  void SetStrategy(int);
  void SetUpRangeLimitX(double);

  int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);

private:

  void fClearRangeLimits();
  void fFillBuffer(double);
  void fInit();
  void fSetBuffer();

  TTree fBuffer;
  TTree fBufferOutLow;
  TTree fBufferOutUp;
  
  bool fIsAuto;
  bool fIsBuilt;
  int fMaxBufferSize;
  int fStrategy;
  bool   fRangeHasLimit[2];
  double fRangeLimit[2];
  double fX;

  //ClassDef(TH1FAuto,1);
};

#endif /* TH1FAUTO_H_ */
