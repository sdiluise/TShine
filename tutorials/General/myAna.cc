/**myAna.cc
 *
 * Created on: 2014
 *      Author: Silvestro di Luise
 *		Silvestro.Di.Luise@cern.ch
 *
 *
 *  
 *
 *
 */

//
//
//
#include <iostream>
#include <bitset>
#include <cstring>
#include <cmath>
#include <vector>
#include <utility>
#include <functional>

//
// ROOT
//
#include <TROOT.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreePlayer.h>
#include <TTreeFormula.h>
#include <TLeaf.h>
#include <TEventList.h>
#include <TNtuple.h>

#include <TMath.h>

#include <TObject.h>
#include <TH2Poly.h>
#include <TObjArray.h>

#include <TRandom.h>

#include <TH1F.h>
#include <TH2F.h>
#include <THStack.h>

#include <TCut.h>
#include <TF1.h>
#include <TF2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMultiGraph.h>
#include <TGraph2D.h>
#include <TList.h>


#include "Math/Minimizer.h"
#include "Minuit2/Minuit2Minimizer.h"
//#include "Math/GSLMinimizer.h"
#include "Math/Functor.h"
#include "Math/Factory.h"
#include "TVirtualFitter.h"
#include "Math/IFunction.h"
#include "Math/Util.h"



//
// TShine 
//


#include "RootUtils.h"
#include "MessageMgr.h"
#include "StringUtils.h"

#include "TSArgList.h"
#include "TSParamList.h"

#include "TH1FAuto.h"
#include "TH2FAuto.h"


#include "TSParticleFld.h"


#include "TSVariable.h"
#include "TSParameter.h"
#include "TSParamFunction.h"

#include "TSPhaseSpaceMap.h"
#include "TSPhaseSpaceFld.h"

#include "TSDataBin.h"
#include "TSDataSetMgr.h"

#include "BetheBlochFunctions.h"
#include "TSBetheBlochFunc.h"


#include "TSToFGenerator.h"
#include "TSDEdxGenerator.h"


#include "TSFunc1DGaussPdf.h"
#include "TSFunc1DAsymGaussPdf.h"
#include "TSFunc1DExpPdf.h"
#include "TSFunc2DProd.h"
#include "TSFuncModel.h"
#include "TSFuncModel1D.h"
#include "TSFuncModel2D.h"


#include "TSMinimizationFunction.h"
#include "TSMinimizer.h"
#include "TSChisquareCalculator.h"
#include "TSLikelihoodCalculator.h"


#include "TSFitResults.h"




// COMMENTS  
#define C(x) std::cout<<x<<std::endl;

//DEBUG logs  
#define DEBUG true
#if DEBUG
#define D(x) std::cout<<x<<std::endl;
#else
#define D(x)
#endif



using std::cout;
using std::endl;



int run_ana(){
  
        //
        TRandom rand;
        
  
        //  Retrieve a generic 2D dEdx-ToF histogram from 2009 thin target analysis
        //  It will be used later on 
	TFile *_file0 = TFile::Open("../Data/trkstpRST_K.root");

	TH2F* hRST_Data=(TH2F*)_file0->Get("h2003");
       
	
	C(" ---- Open ouput file ----- ");

	TFile fout("output.root","recreate");

	//save real data histo
	hRST_Data->SetName("hRST_Data");
	hRST_Data->Write();
	

	C("--- Particle List ----");


	//
	// (Re)Define yourself a TSParticle obj using pre-defined static particle properties
        //  and user defined labelling
	//
	TSParticle P("p","p",TSParticle::MassP,1);
	TSParticle N("n","n",TSParticle::MassN,0);
	TSParticle PiPlu("PiPlu","#pi^{+}",TSParticle::MassPiPlus,1,TSParticle::TauPiPlus);
	TSParticle PiMin("PiMin","#pi^{-}",TSParticle::MassPiMinus,-1,TSParticle::TauPiPlus);
	TSParticle Pi0("Pi0","#pi^{0}",TSParticle::MassPi0,0,TSParticle::TauPi0);
	TSParticle KPlu("KPlu","K^{+}",TSParticle::MassKPlus,1,TSParticle::TauKPlus);
	TSParticle KMin("KMin","K^{-}",TSParticle::MassKMinus,-1,TSParticle::TauKMinus);
	TSParticle K0("K0","K^{0}",TSParticle::MassK0,0,TSParticle::TauK0);
	TSParticle Posit("Posit","e^{+}",TSParticle::MassPosit,1,TSParticle::TauEle);
	TSParticle Ele("Ele","e^{-}",TSParticle::MassPosit,-1,TSParticle::TauEle);
	TSParticle MuPlus("MuPlus","#mu^{+}",TSParticle::MassMuPlus,1,TSParticle::TauMuPlus);
	TSParticle MuMinus("MuMinus","#mu^{-}",TSParticle::MassMuMinus,-1,TSParticle::TauMuMinus);
	TSParticle X("X","X",0,0,0);

        // create a string to be used in Legend
	TString decay_form= TSParticle::GetReactionForm(TSArgList(PiPlu),"->",TSArgList(MuPlus,X));

	D("Decay: "<<decay_form);

        // Dump Info
	P.Print();
	KPlu.Print();
	KMin.Print();
	K0.Print();



	C(" ============ Analysis =========== \n");
	//


	C("\t -- Kinematics and Observable variables --- ")

	
        //
	// Define variables needed to characterize the data
	// Choose labelling to be used in the data presentation
	//
	TSVariable Theta("Theta","#theta","polar angle","mrad");
	TSVariable Mom("Mom","p_{tot}","momentum","GeV/c");
	TSVariable ZTarg("ZTarg","z_{targ}","z-target","cm");

	TSVariable dEdx("dEdx","dE/dx","dE/dx","arb. units");
	TSVariable ToF("ToF","m^{2}","ToF m^{2}","GeV^{2}/c^{4}");
	TSVariable NPoints("NPoints","Num. of Points","Num. of Points");

        //Set generic (maximal) range for the time being...
	Theta.SetRange(0.,400.);
	Mom.SetRange(0.1,31.);
	ZTarg.SetRange(0.,90.);

	dEdx.SetRange(0.8,1.8);
	ToF.SetRange(-0.5,2);
	NPoints.SetRange(1.,300.);

        //Put kinetmatic-type variable in a convenient list
	TSArgList KinVarList(Theta,Mom,ZTarg);

        //Show list info
	KinVarList.PrintVariables();

	// dummy: chek if 200. is in the specfied interval
	bool is_in = Theta.IsInside(200.);


	C("\t ----- Phase Space -----");
	//
        //  ...Check class definition for all the 
        //     object creation options
	//

	//
	//  Create a 2D Phase Space Map: Mom vs Theta
	//

	TSPhaseSpaceMap mapKin2D("mapKin2D","map Mom-Theta");

        // Momentum represented on the first (x) axis
        // Theta on the second (y)
	mapKin2D.SetVariable(Mom);
	mapKin2D.SetVariable(Theta);

        // X-Y bins are correlated ==> read binning in tabular form from
        // a text file (check the format)
        // The first string specifies colon-separated variable names
        // in the same order they must be read from the input file
	mapKin2D.SetBinning("Mom:Theta","mymap.mom_theta.K.txt");

        //Get the folder with all the histograms (TH1F, TH2Poly) describing
        // the Map
	mapKin2D.GetHistogramFld()->Write();


	//
	//
	// Create a 3D PhaseSpace Map: Theta vs Mom vs target-z
	//
	//

	TSPhaseSpaceMap mapKin3D("mapKin3D","map Theta-Mom-Z");
	
	//X-axis: Theta
	//Y-axis: Mom
	mapKin3D.SetVariable(Theta);
	mapKin3D.SetVariable(Mom);

	//read map...
	mapKin3D.SetBinning("Theta:Mom","mymap.theta_mom.txt");

        //it's a 3D map...Object is warning you to specify the binning
        //along the 3rd axis...
	//Add Z axis: ZTarg
	mapKin3D.SetVariable(ZTarg);

	// Simple binning, not correlated with Theta and Mom
	// Here 3 uniformly spaced bins are created 
        // (possible also variable width bins)
        // A 3D-bin-map is created using the built 2D (Theta-Mom) and 1D (Z) bins; 
	mapKin3D.SetBinning(ZTarg,3,ZTarg.GetMin(),ZTarg.GetMax());

	//Show binning
	mapKin3D.Print();

	//Save map histos
	mapKin3D.GetHistogramFld()->Write();

	//
	// Create two folders with the same map, one for positive one for negative tracks
	// Folders differs from map as they are "fillable" containers
        // Can be used to plot multiplicties
	TSPhaseSpaceFld mapKin3D_Pos("map_PosTracks","q>0",mapKin3D.Title(),mapKin3D);
	TSPhaseSpaceFld mapKin3D_Neg("map_NegTracks","q<0",mapKin3D.Title(),mapKin3D);

	// Disable range checks, if true values set outside
        // the range will be brought back into the limits
	ZTarg.ForceToRange(false);
	Theta.ForceToRange(false);
	Mom.ForceToRange(false);


       C("\t  ----- ToF ------");

       // 
       // Prepare parameterization to generate ToF mass2 
       //

       //
       // values are taken from fits to data
       //
       double  m2_m_p0[4],m2_m_p1[4],m2_s_p0[4],m2_s_p1[4],m2_s_p2[4];

       //Mean-Parameter linear parametrization: mean(ptot) = a +ptot*b
       //p
       m2_m_p0[0]= P.M2();   m2_m_p1[0]= 0.;
       //k
       m2_m_p0[1]= KPlu.M2();  m2_m_p1[1]= 0;
       //pi
       m2_m_p0[2]= PiPlu.M2();   m2_m_p1[2]= 0;
       //e
       m2_m_p0[3]= Ele.M2();   m2_m_p1[3]= 0.;


       //Width-Parameter parabolic parametrization: sigma(ptot) = a + ptot*b + ptot*ptot*c
       //p
       m2_s_p0[0]= 0.0295321;   m2_s_p1[0]= -0.00630375;   m2_s_p2[0]= 0.00799005;
       //k
       m2_s_p0[1]= 0.00557491;   m2_s_p1[1]= -0.00219247;   m2_s_p2[1]= 0.00820526;
       //pi
       m2_s_p0[2]= 0.00557491;   m2_s_p1[2]= -0.00219247;   m2_s_p2[2]= 0.00820526;
       //e
       m2_s_p0[3]= 0.00557491;   m2_s_p1[3]= -0.00219247;   m2_s_p2[3]= 0.00820526;


       //
       // Create ToFGenerator obj
       //
       TSToFGenerator tofGen("tofGen",Mom,ToF);

       //define particles and their mass
       tofGen.SetParticle("proton","p",TSParticle::MassP);
       tofGen.SetParticle("kaon","K",TSParticle::MassKPlus);
       tofGen.SetParticle("pion","#pi",TSParticle::MassPiPlus);

       //generic particle
       tofGen.SetParticle("part","part",TSParticle::MassKPlus);

       //parametrization of the mean and width vs momentum
       // If not otherwise specified the m2 distribution for a given value of the mometum
       // is assumed to be a gaussian G(mean(ptot),sigma(ptot));
       tofGen.SetMass2MeanFunction("proton","linear",m2_m_p0[0],m2_m_p1[0]);
       tofGen.SetMass2MeanFunction("kaon","linear",m2_m_p0[1],m2_m_p1[1]);
       tofGen.SetMass2MeanFunction("pion","linear",m2_m_p0[2],m2_m_p1[2]);

       tofGen.SetMass2WidthFunction("proton","parab",m2_s_p0[0],m2_s_p1[0],m2_s_p2[0]);
       tofGen.SetMass2WidthFunction("kaon","parab",m2_s_p0[1],m2_s_p1[1],m2_s_p2[1]);
       tofGen.SetMass2WidthFunction("pion","parab",m2_s_p0[2],m2_s_p1[2],m2_s_p2[2]);


       //Specify a different m2 distribution, e.g.: a double tailed gaussian
       //4 parameters: G([0],[1])+[3]*G([0],[2]);  
       // for each parameters specify the dependence as as funnction of the momentum
       tofGen.SetMass2DistributionFunction("part","double_tailed_gaus");
       tofGen.SetParamFunction("part", 0 , "linear", m2_m_p0[0],m2_m_p1[0]);
       tofGen.SetParamFunction("part", 1 , "parab", m2_s_p0[0],m2_s_p1[0],m2_s_p2[0]);
       tofGen.SetParamFunction("part", 2 , "parab", m2_s_p0[2],m2_s_p1[2],3*m2_s_p2[2]);
       tofGen.SetParamFunction("part", 3 , "const", 0.5);

       //optional
       //tofGen.SetMass2WidthScaling("part","pion");


       // Get parameterization function of each parameter
       // of the mass2 distribution function
       TSArgList listOfToFParamFuncs;

       for(int i=0; i<tofGen.GetMass2DistribNPars("proton");++i){
	 
	 listOfToFParamFuncs.Add(tofGen.GetParametrization("proton",i));
       }

       for(int i=0; i<tofGen.GetMass2DistribNPars("pion");++i){

	 listOfToFParamFuncs.Add(tofGen.GetParametrization("pion",i));
       }

       for(int i=0; i<tofGen.GetMass2DistribNPars("kaon");++i){

	 listOfToFParamFuncs.Add(tofGen.GetParametrization("kaon",i));
       }

       for(int i=0; i<tofGen.GetMass2DistribNPars("part");++i){

	 listOfToFParamFuncs.Add(tofGen.GetParametrization("part",i));
       }
       
       //write to output file
       listOfToFParamFuncs.Write();


       //
       // Get Distribution for a given ptot value
       //
       TF1 *fM2_P_Mom=tofGen.ImportMass2Distrib("proton",8);
       TF1 *fM2_Pi_Mom=tofGen.ImportMass2Distrib("pion",8);
       TF1 *fM2_K_Mom=tofGen.ImportMass2Distrib("kaon",8);
       TF1 *fM2_Part_Mom=tofGen.ImportMass2Distrib("part",8);

       fM2_P_Mom->Write();

       fM2_Pi_Mom->Write();
       fM2_K_Mom->Write();
       fM2_Part_Mom->Write();



       C("\t --- ToF form path legth and time resolution -----");
       
       //Another way to compute m2 for a given particle type and momentum

       TSToFGenerator tofResponse("tofResponse");
       
       tofResponse.SetToFResolution(100);//ps
    
       double PathLen=7;// m




       C("\t ------- Bethe Bloch --------");

       //
       // Create Bethe Bloch function handler
       // it can hold both an analytic function or a tabular-form function
       // Here two obj are create, one using a built in 5 param function
       // and another one using a table function read from a txt file
       // You can also add both description to the same obj and then switch
       // between the two via UseTable(bool);  
       // The input to the function is the gamma variable: gamma = p/M
       //

       TSBetheBlochFunc bbFunc("bbFunc","Bethe-Bloch function",BETHE_BLOCH_FUNC::BetheBloch,0.1,300,5);
       bbFunc.SetParameters(BETHE_BLOCH_FUNC::PAR_INPUT);

       TSBetheBlochFunc bbFuncTable("bbFuncTable","Bethe-Bloch tabular","bethe_bloch_table.txt");
       

       C("\t  ----- dE/dx generation -----");

       //
       // Class to generate dEdx quantity from a given Bethe Bloch function
       // and a defined distribution function
       //

       TSDEdxGenerator dedxGenTrack("dedxGenGlobal","dE/dx generation at track level");

       //pdf of the dEdx around the mean, here mean is 0 and width 4%;
       dedxGenTrack.SetTrackDEdxDistrib("Gaussian",-1,1,0,0.04);

       //Use another built in pdf...
       //dedxGenTrack.SetTrackDEdxDistrib("DoubleTailedGaus",-1,1,0,0.1,0.5,0.5);

       // change the mean with this method. N.B.: this will be an additional
       // shift with respect to the mean calculate from the bethe bloch function
       dedxGenTrack.SetMean(0.);

       // set the bethe bloch function to be used to calculate the mean (<dEdx>) for a given 
       // value of the momentum (and mass)
       dedxGenTrack.SetBetheBloch(bbFunc);


       //Get the dEdx dstribution function aad save it 
       TF1 *TF1_dEdx_Gen = (TF1*)dedxGenTrack.ImportTrackDEdxDistrib();

       TF1_dEdx_Gen->Write();


       //
       // Here dEdx is calculated from a truncated mean over single cluster dEdx
       //

       TSDEdxGenerator dedxGenCluster("dedxGenCluster","dE/dx generation as truncated mean of cluster charges");

       //pdf of the single cluster
       dedxGenCluster.SetClusterChargeDistrib("Gaussian",-1,1,0.,0.4);
       dedxGenCluster.SetMean(0.);
       //perform truncated mean at 60%
       dedxGenCluster.SetTruncation(0.6);
       //for bias studies
       dedxGenCluster.ApplyCalibrationCorrection(false);


       //Generate a distribution for a Pion at 3 GeV/c;
       //Use the Automatic Binning Histogram so you don't have to guess the axis range
       TH1FAuto hTestTruncMean("hTestTruncMean","");
       for(int i=0;i<1000;++i){
      	 hTestTruncMean.Fill(dedxGenTrack.Generate(3,PiPlu.M()));
       }
       hTestTruncMean.Write();
     


       

       C("\t -------- Data Set Set Up -----------");

       C("\t \t  -- Create a Data Bin ----");
       //
       // Create a data bin : data container prototype for each one of the phase space volumes
       //

       TSDataBin dataBin("dataBin","dataBin","data bin");
       
       //Option to fill data unbinned as well
       dataBin.SetUnbinned(false);

       //dataBin.SetPhaseSpaceVolume(*mapKin3D.GetVolume(1));

       //
       // create variable for individual particles, so that individual histos 
       // will be added into the data bin
       // Those histos will be useful to check dEdx,m2,.. distribution separated
       // by particle type...if such info is available form the MC

       //individual particle dEdx
       TSVariable dEdx_P("dEdx_P","dE/dx","dE/dx for p's","arb. units");
       TSVariable dEdx_K("dEdx_K","dE/dx","dE/dx for K's","arb. units");
       TSVariable dEdx_Pi("dEdx_Pi","dE/dx","dE/dx for #pi's","arb. units");

       //BB central values
       TSVariable dEdx_BB_P("dEdx_BB_P","dE/dx","<dE/dx> for p's","arb. units");
       TSVariable dEdx_BB_K("dEdx_BB_K","dE/dx","<dE/dx> for K's","arb. units");
       TSVariable dEdx_BB_Pi("dEdx_BB_Pi","dE/dx","<dE/dx> for #pi's","arb. units");

       //m2
       TSVariable ToF_P("ToF_P","m^{2}","m^{2} for p's","GeV^{2}/c^{4}");
       TSVariable ToF_K("ToF_K","m^{2}","m^{2} for K's","GeV^{2}/c^{4}");
       TSVariable ToF_Pi("ToF_Pi","m^{2}","m^{2} for #pi's","GeV^{2}/c^{4}");

       //In order to check the spread of the expected sigma over a bin momentum range, to be included in
       //the data bin maybe later, it can be checked directly from the functional form
       //TSVariable ToF_Sig_P("ToF_Sig_P","#sigma-m^{2}","#sigma-m^{2} for p's","GeV^{2}/c^{4}");
       //TSVariable ToF_Sig_K("ToF_Sig_K","#sigma-m^{2}","#sigma-m^{2} for K's","GeV^{2}/c^{4}");
       //TSVariable ToF_Sig_Pi("ToF_Sig_Pi","#sigma-m^{2}","#sigma-m^{2} for #pi's","GeV^{2}/c^{4}");

       //
       // Define completely the binning
       // options are available to make this automatic
       // here a 1D histo will be created with 100 bins in the specified range
       // for the specified TVariable object
       dataBin.SetVariable(dEdx,100,dEdx.GetMin(),dEdx.GetMax());
       dataBin.SetVariable(dEdx_P,100,dEdx.GetMin(),dEdx.GetMax());
       dataBin.SetVariable(dEdx_Pi,100,dEdx.GetMin(),dEdx.GetMax());
       dataBin.SetVariable(dEdx_K,100,dEdx.GetMin(),dEdx.GetMax());


       dataBin.SetVariable(dEdx_BB_P,100,dEdx.GetMin(),dEdx.GetMax());
       dataBin.SetVariable(dEdx_BB_Pi,100,dEdx.GetMin(),dEdx.GetMax());
       dataBin.SetVariable(dEdx_BB_K,100,dEdx.GetMin(),dEdx.GetMax());


       //define only the max range
       //for m2 distrib the binning will be automatic as  
       // typical widht of m2 varies largely with the momentum
       dataBin.SetVariable(ToF,ToF.GetMin(),ToF.GetMax());
       dataBin.SetVariable(ToF_P,ToF.GetMin(),ToF.GetMax());
       dataBin.SetVariable(ToF_K,ToF.GetMin(),ToF.GetMax());
       dataBin.SetVariable(ToF_Pi,ToF.GetMin(),ToF.GetMax());


       //Number of points on track
       dataBin.SetVariable(NPoints,NPoints.GetMax()-NPoints.GetMin()+1,NPoints.GetMin()+0.5,NPoints.GetMax()+0.5);

       //					
       // Create Heare corresponding 2D histos for the
       // desired combination of variables
       //
       dataBin.SetBidimensional(dEdx,ToF);
       dataBin.SetBidimensional(dEdx_P,ToF_P);
       dataBin.SetBidimensional(dEdx_K,ToF_K);
       dataBin.SetBidimensional(dEdx_Pi,ToF_Pi);

       
       C("\t \t  -- Create a Data Set ----");
 
       //
       //
       //   Create a data set: 
       //                    a collection of data bins over a phase space
       //
       //

       TSDataSetMgr dataSetMgr("dataSetMgr","Glob","data set");

       //set the DataBin created above
       dataSetMgr.SetDataBin( dataBin );

       // Set the phase space map created above
       // A TSDataBin dataBin will be created for each
       // mapKin3D bin
       dataSetMgr.SetPhaseSpace(mapKin3D);

       //Show infos...
       dataSetMgr.Print();

       //Same as dataSetMgr but for the 2D map
       TSDataSetMgr dataSetMgr2D("dataSetMgr2D","Glob","data set 2D");
    
       dataSetMgr2D.SetDataBin( dataBin );

       dataSetMgr2D.SetPhaseSpace(mapKin2D);

       //Show infos
       dataSetMgr2D.Print();



       C("\t  --- Generation ----");

       //
       // First example on how to put together all the object created so far
       //
       // It's Simple example on how to generated dEdx, m2 quantities 
       // and fill the container
       //

       //save the generated particle ptot distribtion 
       TH1F hGenPtot("hGenPtot",Mom.GetTitleXY(),100,Mom.GetMin(),Mom.GetMax());

       TH1FAuto hGenBB("hGenBB","Bethe-Bloch;#beta#gamma");
       
       TH2FAuto hGenPtotVsMass2("hGenPtotVsMass2","",100,Mom.GetMin(),10.,200,-0.5,2);

       //generate 10k events
       for(int i=0; i<10000; ++i){

	 //just uniform
	 double z_rand=rand.Exp(ZTarg.GetMax()-ZTarg.GetMin());
	
	 //dummy pdf: exponential
	 double ptot_rand=Mom.GetMin()+rand.Exp(4);

	 //...
	 double theta_rand=rand.Exp(200);

	 //...
	 int npts_rand=rand.Gaus(100,50);

	 //set TSvariable object to current values
	 ZTarg.SetValue(z_rand);
	 Theta.SetValue(theta_rand);
	 Mom.SetValue(ptot_rand);
	 NPoints.SetValue(npts_rand);

	 //Compute the <dEdx> from the bethe-bloch function
	 double bb_p = bbFunc.Eval(ptot_rand,TSParticle::MassP);
	 double bb_k = bbFunc.Eval(ptot_rand,TSParticle::MassKPlus);
	 double bb_pi = bbFunc.Eval(ptot_rand,TSParticle::MassPiPlus);

       
	 //Generate a m2 value
	 double m2_p=tofGen.GenerateMass2("proton",ptot_rand);
	 double m2_k=tofGen.GenerateMass2("kaon",ptot_rand);
	 double m2_pi=tofGen.GenerateMass2("pion",ptot_rand);

	 //Generate a dEdx value
	 double dedx_p=dedxGenTrack.Generate(ptot_rand,TSParticle::MassP);
	 double dedx_k=dedxGenTrack.Generate(ptot_rand,TSParticle::MassKPlus);
	 double dedx_pi=dedxGenTrack.Generate(ptot_rand,TSParticle::MassPiPlus);
	 
	
	 //give values to variable
	 dEdx_BB_P.SetValue(bb_p);
	 dEdx_BB_K.SetValue(bb_k);
	 dEdx_BB_Pi.SetValue(bb_pi);

	 dEdx.SetValue(dedx_pi);
	 ToF.SetValue(m2_pi);

	 dEdx_Pi.SetValue(dedx_pi);
	 ToF_Pi.SetValue(m2_pi);

	 dEdx_P.SetValue(dedx_p);
	 ToF_P.SetValue(m2_p);

	 dEdx_K.SetValue(dedx_k);
	 ToF_K.SetValue(m2_k);

	 //
	 // -- Filling one-entry-per-event histo --
	 //
	 // on Fill() all the histogram will be filled with 
         // the current values of the associated variables
	 //

	 // here you just fill a single bin irregardless of
         // the phase space bin you are in 
	 
	 dataBin.Fill();

	 // Here you fill the data bin relative
         // to the ptot-theta-z bin for this event
	 // 3D map
	 dataSetMgr.Fill();

	 // 2D map
	 dataSetMgr2D.Fill();


	 //
	 // -- Filling multiple-entry-event histo --
         // dEdx, ToF are associated to cumulative distributions
	 // Pi already filled, now set vars for P and K and
         // Fill only relative histos (do not call just Fill() otherwise
	 // histos for individual particles variables, dEdx_Pi..., will be filled multiple times)
	 //
	 dEdx.SetValue(dedx_p);
	 ToF.SetValue(m2_p);

	 dataBin.Fill( TSArgList(dEdx,ToF) );
	 dataSetMgr.Fill( TSArgList(dEdx,ToF) );
	 dataSetMgr2D.Fill( TSArgList(dEdx,ToF) );


	 dEdx.SetValue(dedx_k);
	 ToF.SetValue(m2_k);

	 dataBin.Fill( TSArgList(dEdx,ToF) );
	 dataSetMgr.Fill( TSArgList(dEdx,ToF) );

	 dataSetMgr2D.Fill( TSArgList(dEdx,ToF) );


	 //Overall distrib
	 hGenPtot.Fill(ptot_rand);

	 hGenBB.Fill( bb_pi );
	 
	 hGenPtotVsMass2.Fill(ptot_rand,m2_p);
	 hGenPtotVsMass2.Fill(ptot_rand,m2_k);

	 hGenPtotVsMass2.Fill(ptot_rand,m2_pi);
	 

	 //this is the MapFolder created at the beginning
	 //It can be used standalone to have an accupancy map
	 //There is a MapFolder object also in each DataSetMgr obj which is
         // filled automatically via the Fill(...) method 
	 // this can be retrieved as shown below
	 mapKin3D_Pos.Fill();
	 
       }

       //
       //  -- Generic infos
       //
       cout<<"Tot Calls/EntriesIn Pos: "<<mapKin3D_Pos.GetNumOfCalls()<<" "<<mapKin3D_Pos.GetEntriesIn()<<endl;
       cout<<"Tot Calls/EntriesIn Neg: "<<mapKin3D_Neg.GetNumOfCalls()<<" "<<mapKin3D_Neg.GetEntriesIn()<<endl;


       mapKin3D_Pos.GetHistogramFld()->Write();
       mapKin3D_Neg.GetHistogramFld()->Write();

       mapKin3D_Pos.Print();
	

       hGenPtot.Write();
       hGenBB.Write();
       hGenPtotVsMass2.Write();

       dataBin.GetHistoFolder()->Write();
       
       cout<<"Outliers: "<<dataBin.GetHistoFolder()->GetOutliersFrac(dEdx.Name(),ToF.Name())<<endl;
	
       //In case dataBin is set to be unbinned
       if( dataBin.GetDataTree() )
	 dataBin.GetDataTree()->Write();

	

       //Get a DataBin for a specific phase space bin
       //Get all the associated histos and save them
       dataSetMgr.GetDataBin(35)->GetHistoFolder()->Write();

       dataSetMgr2D.GetDataBin(1)->GetHistoFolder()->Write();

       //
       //There is a MapFolder object also in each DataSetMgr obj which is
       // filled automatically via the Fill(...) method 
       // 

       dataSetMgr2D.GetPhaseSpaceFld()->Print();
       
       dataSetMgr2D.GetPhaseSpaceFld()->Write();

	//get a single histos...
	//dataSetMgr2D.GetPhaseSpaceFld()->GetHistogramFld()->At(0)->Write();
	//dataSetMgr2D.GetPhaseSpaceFld()->GetHistogramFld()->At(1)->Write("DBM1");
	//dataSetMgr2D.GetPhaseSpaceFld()->GetHistogramFld()->At(2)->Write("DBM2");



	C("\t ---- Define Parameters for Fit Models -----");

	//
	// Define all the parameters will be used to build the
        // final fitting Model
	//

	//
	// -- Function variables
	//
	TSVariable dEdx_x("dEdx_x","dE/dx","dE/dx","arb. units");
	TSVariable m2_x("m2_x","m^{2}","ToF m^{2}","GeV^{2}/c^{4}");

	//
	// -- Functions Parameters --
	//

	//dEdx related
	TSParameter dEdx_sig_0("dEdx_sig_0","#sigma_{dE/dx}","dE/dx width","arb. units");
	TSParameter dEdx_sig_alpha("dEdx_sig_alpha","#alpha","#alpha scaling factor");

	TSParameter dEdx_sig_delta("dEdx_sig_delta","#delta","asymmetry parameter");

	TSParameter dEdx_Pi_mean("dEdx_Pi_mean","m_{dE/dx}-#pi","#pi dE/dx mean","arb. units");

	TSParameter dEdx_mean_PtoPi("dEdx_mean_PtoPi","m_{dE/dx}-P#pi","dE/dx mean","arb. units");
	TSParameter dEdx_mean_KtoPi("dEdx_mean_KtoPi","m_{dE/dx}-K#pi","dE/dx mean","arb. units");

        // Definition of relative mean:
	// p_mean=pi_mean+p2pi_mean
	// => p2pi_mean=p_mean-pi_mean

	//
	//define a ParamFunction to handle parameter function of other parameters...
	//
	TSParamFunction dEdx_P_mean("dEdx_P_mean","m_{dE/dx}-P","P dE/dx mean","arb. units");
	dEdx_P_mean.SetFunction("dEdx_Pi_mean+dEdx_mean_PtoPi",TSArgList(dEdx_Pi_mean,dEdx_mean_PtoPi));

	TSParamFunction dEdx_K_mean("dEdx_K_mean","m_{dE/dx}-K","K dE/dx mean","arb. units");
	dEdx_K_mean.SetFunction("dEdx_Pi_mean+dEdx_mean_KtoPi",TSArgList(dEdx_Pi_mean,dEdx_mean_KtoPi));

	TSParamFunction dEdx_Pi_sig("dEdx_Pi_sig","#sigma_{dE/dx}-#pi","dE/dx width","arb. units");
	dEdx_Pi_sig.SetFunction("dEdx_sig_0*TMath::Power(dEdx_Pi_mean/dEdx_Pi_mean,dEdx_sig_alpha)",TSArgList(dEdx_sig_0,dEdx_Pi_mean,dEdx_sig_alpha));

	TSParamFunction dEdx_P_sig("dEdx_P_sig","#sigma_{dE/dx}-P","dE/dx width","arb. units");
	dEdx_P_sig.SetFunction("dEdx_sig_0*TMath::Power(dEdx_P_mean/dEdx_Pi_mean,dEdx_sig_alpha)",TSArgList(dEdx_sig_0,dEdx_Pi_mean,dEdx_P_mean,dEdx_sig_alpha));

	TSParamFunction dEdx_K_sig("dEdx_K_sig","#sigma_{dE/dx}-K","dE/dx width","arb. units");
	dEdx_K_sig.SetFunction("dEdx_sig_0*TMath::Power(dEdx_K_mean/dEdx_Pi_mean,dEdx_sig_alpha)"
								,TSArgList(dEdx_sig_0,dEdx_Pi_mean,dEdx_K_mean,dEdx_sig_alpha));

	TSParameter dEdx_E_mean("dEdx_E_mean","m_{dE/dx}-e","e dE/dx mean","arb. units");
	TSParameter dEdx_E_sig("dEdx_E_sig","m_{dE/dx}-e","e dE/dx width","arb. units");


	//m2 related

	TSParameter m2_Pi_mean("m2_Pi_mean","m^{2}","#pi ToF m^{2} mean","GeV^{2}/c^{4}");
	TSParameter m2_Pi_sig("m2_Pi_sig","#sigma_{m^{2}}","#pi ToF m^{2} width","GeV^{2}/c^{4}");

	TSParameter m2_P_mean("m2_P_mean","m^{2}","P ToF m^{2} mean","GeV^{2}/c^{4}");
	TSParameter m2_P_sig("m2_P_sig","#sigma_{m^{2}}","P ToF m^{2} width","GeV^{2}/c^{4}");

	TSParameter m2_K_mean("m2_K_mean","m^{2}","K ToF m^{2} mean","GeV^{2}/c^{4}");
	TSParameter m2_K_sig("m2_K_sig","#sigma_{m^{2}}","K ToF m^{2} width","GeV^{2}/c^{4}");

	TSParameter m2_E_mean("m2_E_mean","m^{2}","e ToF m^{2} mean","GeV^{2}/c^{4}");
	TSParameter m2_E_sig("m2_E_sig","#sigma_{m^{2}}","e ToF m^{2} width","GeV^{2}/c^{4}");



	//
	// Test how dEdx_K_sig, a ParamFunction is calculated
        // from the base parameters 
        //
	dEdx_sig_0.SetValue(0.04);
	dEdx_Pi_mean.SetValue(1.13);
	dEdx_mean_KtoPi.SetValue(-0.1);
	dEdx_sig_alpha.SetValue(0.65);

	dEdx_K_sig.Print();

	
	//
	// Init params with some Values
	//

	dEdx_x.SetRange(0.7,1.4);
	m2_x.SetRange(-0.5,1.5);

	dEdx_sig_0.SetValue(0.04);
	dEdx_sig_alpha.SetValue(0.65);

	dEdx_sig_delta.SetRange(0.,0.99);
	double truth_delta=0.1;
	dEdx_sig_delta.SetValue(truth_delta);

	//some reasonable bethe bloch values
	double BB_P=1.;
	double BB_K=1.17;
	double BB_Pi=1.2;

	dEdx_Pi_mean.SetValue(BB_Pi);
	dEdx_mean_PtoPi.SetValue(BB_P-BB_Pi);
	dEdx_mean_KtoPi.SetValue(BB_K-BB_Pi);

	dEdx_E_mean.SetValue(1.5);

	m2_Pi_mean.SetValue(PiPlu.M2());
	m2_P_mean.SetValue(P.M2());
	m2_K_mean.SetValue(KPlu.M2());

	m2_E_mean.SetValue(0.);

	m2_Pi_sig.SetValue(0.1);
	m2_P_sig.SetValue(0.1);
	m2_K_sig.SetValue(0.1);

	m2_E_sig.SetValue(0.1);


	//
	// -- Yields --
	// define parameters to be used as Yields int he fits
	//

	TSParameter NPi("NPi","N_{#pi}","#pi yield");
	TSParameter NP("NP","N_{P}","P yield");
	TSParameter NK("NK","N_{K}","K yield");
	TSParameter NE("NE","N_{e}","e yield");

        //whatever...: K 10% of Pions
	NP.SetValue(9);
	NK.SetValue(1);
	NPi.SetValue(10);



	C("\t  ---- Distribution Functions --------- ");

	//
        // Building block Functions 
        //

	//TSFunc1DGaussPdf Pi_dEdx_Pdf("Pi_dEdx_Pdf","#pi dE/dx pdf",dEdx_x,dEdx_Pi_mean,dEdx_Pi_sig);


	// Build 1D dEdx Single Particles Pdf

	//TSFunc1DGaussPdf Pi_dEdx_Pdf("Pi_dEdx_Pdf", "#pi dE/dx pdf", dEdx_x,dEdx_Pi_mean, dEdx_Pi_sig);

	//TSFunc1DGaussPdf P_dEdx_Pdf("P_dEdx_Pdf", "P dE/dx pdf", dEdx_x, dEdx_P_mean, dEdx_P_sig);

	//TSFunc1DGaussPdf K_dEdx_Pdf("K_dEdx_Pdf", "K dE/dx pdf", dEdx_x, dEdx_K_mean, dEdx_K_sig);

	TSFunc1DGaussPdf E_dEdx_Pdf("E_dEdx_Pdf", "E dE/dx pdf", dEdx_x, dEdx_E_mean, dEdx_E_sig);



	TSFunc1DAsymGaussPdf Pi_dEdx_Pdf("Pi_dEdx_Pdf", "#pi dE/dx pdf", dEdx_x,dEdx_Pi_mean, dEdx_Pi_sig, dEdx_sig_delta);

	TSFunc1DAsymGaussPdf P_dEdx_Pdf("P_dEdx_Pdf", "P dE/dx pdf", dEdx_x, dEdx_P_mean, dEdx_P_sig, dEdx_sig_delta);

	TSFunc1DAsymGaussPdf K_dEdx_Pdf("K_dEdx_Pdf", "K dE/dx pdf", dEdx_x, dEdx_K_mean, dEdx_K_sig, dEdx_sig_delta);


	// Build Single Particle dEdx Model

	TSFuncModel1D Model_dEdx_Pi("Model_dEdx_Pi", "model dE/dx #pi", TSArgList(Pi_dEdx_Pdf), TSArgList(NPi));

	TSFuncModel1D Model_dEdx_P("Model_dEdx_P", "model dE/dx P", TSArgList(P_dEdx_Pdf), TSArgList(NP));

	TSFuncModel1D Model_dEdx_K("Model_dEdx_K", "model dE/dx K", TSArgList(K_dEdx_Pdf), TSArgList(NK));

	TSFuncModel1D Model_dEdx_E("Model_dEdx_E", "model dE/dx e", TSArgList(E_dEdx_Pdf), TSArgList(NE));

	//Build 1D dEdx Model  Pdf
	TSFuncModel1D Model_dEdx("Model_dEdx","model dE/dx", TSArgList(P_dEdx_Pdf,K_dEdx_Pdf,Pi_dEdx_Pdf), TSArgList(NP,NK,NPi));

	TSFuncModel1D Model_dEdx_4Parts("Model_dEdx_4Parts","model dE/dx", TSArgList(P_dEdx_Pdf,K_dEdx_Pdf,Pi_dEdx_Pdf,E_dEdx_Pdf), TSArgList(NP,NK,NPi,NE));


	//Build 1D Mass2 Single Particles Pdf
	TSFunc1DGaussPdf Pi_m2_Pdf("Pi_m2_Pdf","#pi m^{2} pdf",m2_x,m2_Pi_mean,m2_Pi_sig);
	TSFunc1DGaussPdf P_m2_Pdf("P_m2_Pdf","P m^{2} pdf",m2_x,m2_P_mean,m2_P_sig);
	TSFunc1DGaussPdf K_m2_Pdf("K_m2_Pdf","K m^{2} pdf",m2_x,m2_K_mean,m2_K_sig);
	TSFunc1DGaussPdf E_m2_Pdf("E_m2_Pdf","e m^{2} pdf",m2_x,m2_E_mean,m2_E_sig);


	//Build 1D dEdx Model  Pdf
	TSFuncModel1D Model_m2("Model_m2","model m^{2}",TSArgList(P_m2_Pdf,K_m2_Pdf,Pi_m2_Pdf),TSArgList(NP,NK,NPi));

	TSFuncModel1D Model_m2_4Parts("Model_m2_4Parts","model m^{2}",TSArgList(P_m2_Pdf,K_m2_Pdf,Pi_m2_Pdf,E_m2_Pdf),TSArgList(NP,NK,NPi,NE));

	TSFuncModel1D Model_m2_Pi("Model_m2_Pi","model m^{2} #pi",TSArgList(Pi_m2_Pdf),TSArgList(NPi));
	TSFuncModel1D Model_m2_P("Model_m2_P","model m^{2} P",TSArgList(P_m2_Pdf),TSArgList(NP));
	TSFuncModel1D Model_m2_K("Model_m2_K","model m^{2} K",TSArgList(K_m2_Pdf),TSArgList(NK));
	TSFuncModel1D Model_m2_E("Model_m2_E","model m^{2} e",TSArgList(E_m2_Pdf),TSArgList(NE));

	// Build 2D Single Particle Pdf
	TSFunc2DProd P_2D_Pdf("P_2D_Pdf","P dE/dx-m^{2} pdf",P_dEdx_Pdf,P_m2_Pdf);

	TSFunc2DProd K_2D_Pdf("K_2D_Pdf","K dE/dx-m^{2} pdf",K_dEdx_Pdf,K_m2_Pdf);

	TSFunc2DProd Pi_2D_Pdf("Pi_2D_Pdf","#pi dE/dx-m^{2} pdf",Pi_dEdx_Pdf,Pi_m2_Pdf);

	TSFunc2DProd E_2D_Pdf("E_2D_Pdf","e dE/dx-m^{2} pdf",E_dEdx_Pdf,E_m2_Pdf);


	// Build 2D Model

	//single peaks
	TSFuncModel2D Model_2D_Pi("Model_2D_Pi","model dEdx-m^{2} #pi's",TSArgList(Pi_2D_Pdf),TSArgList(NPi));
	TSFuncModel2D Model_2D_P("Model_2D_P","model dEdx-m^{2} P's",TSArgList(P_2D_Pdf),TSArgList(NP));
	TSFuncModel2D Model_2D_K("Model_2D_K","model dEdx-m^{2} K's",TSArgList(K_2D_Pdf),TSArgList(NK));

	//TSFuncModel2D Model_2D("Model_2D","model dEdx-m^{2}",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf),TSArgList(NP,NK,NPi));

	TSFuncModel2D Model_2D_PPi("Model_2D_PPi","model dEdx-m^{2}",TSArgList(P_2D_Pdf,Pi_2D_Pdf),TSArgList(NP,NPi));

	TSFuncModel2D Model_2D_4Parts("Model_2D_4Parts","model dEdx-m^{2}",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf,E_2D_Pdf),TSArgList(NP,NK,NPi,NE));

	//TSFuncModel2D Model_2D("Model_2D","model dEdx-m^{2}",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf)
		//												,TSArgList(NP,NK,NPi));


	TSFuncModel2D Model_2D("Model_2D_4Parts","model dEdx-m^{2}",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf,E_2D_Pdf)
															,TSArgList(NP,NK,NPi,NE));

	//TSFuncModel2D Model_2D("Model_2D","model dEdx-m^{2}","P_2D_Pdf+K_2D_Pdf-Pi_2D_Pdf",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf));//,TSArgList(NP,NK,NPi));
	//TSFuncModel2D Model_2D("Model_2D","model dEdx-m^{2}","NP*P_2D_Pdf+NK*K_2D_Pdf+NPi*Pi_2D_Pdf",TSArgList(P_2D_Pdf,K_2D_Pdf,Pi_2D_Pdf),TSArgList(NP,NK,NPi));



        //
	// Print primary param list:
        // it's the list of non-derived (independent) parameters
	// parameters that are functions of others
        // are not included, its base params are
	// added to the list instead (operation is
        // repeated until only primary params are left)
	// This is for example the list of parameters
        // that the minimizer will use while fitting
        // the model to data 
        //
	P_dEdx_Pdf.PrintPrimaryParamList();
	K_dEdx_Pdf.PrintPrimaryParamList();
	Pi_dEdx_Pdf.PrintPrimaryParamList();


	//
	// -- Get Pdf Graphs (both 1D and 2D)
	//
	TGraph* g_P_dEdx_Pdf=P_dEdx_Pdf.BuildGraphX();
	TGraph* g_K_dEdx_Pdf=K_dEdx_Pdf.BuildGraphX();
	TGraph* g_Pi_dEdx_Pdf=Pi_dEdx_Pdf.BuildGraphX();

	TGraph* g_Model_Pi_dEdx=Model_dEdx_Pi.BuildGraphX();
	TGraph* g_Model_K_dEdx=Model_dEdx_K.BuildGraphX();
	TGraph* g_Model_P_dEdx=Model_dEdx_P.BuildGraphX();


	TGraph* g_Model_dEdx=Model_dEdx.BuildGraphX();

	TGraph* g_P_m2_Pdf=P_m2_Pdf.BuildGraphX();
	TGraph* g_K_m2_Pdf=K_m2_Pdf.BuildGraphX();
	TGraph* g_Pi_m2_Pdf=Pi_m2_Pdf.BuildGraphX();


	TGraph* g_Model_Pi_m2=Model_m2_Pi.BuildGraphX();
	TGraph* g_Model_K_m2=Model_m2_K.BuildGraphX();
	TGraph* g_Model_P_m2=Model_m2_P.BuildGraphX();


	TGraph* g_Model_m2=Model_m2.BuildGraphX();

	TGraph2D* g2_P_2D_Pdf = P_2D_Pdf.BuildGraph2D(50,50);
	TGraph2D* g2_K_2D_Pdf = K_2D_Pdf.BuildGraph2D(50,50);
	TGraph2D* g2_Pi_2D_Pdf = Pi_2D_Pdf.BuildGraph2D(50,50);
	TGraph2D* g2_Model_2D = Model_2D.BuildGraph2D(50,50);

	TGraph2D *g2_Model_2D_PPi=Model_2D_PPi.BuildGraph2D();


	g_P_dEdx_Pdf->Write();
	g_K_dEdx_Pdf->Write();
	g_Pi_dEdx_Pdf->Write();

	g_Model_dEdx->Write();

	g_P_m2_Pdf->Write();
	g_K_m2_Pdf->Write();
	g_Pi_m2_Pdf->Write();
	g_Model_m2->Write();

	g2_P_2D_Pdf->Write();
	g2_K_2D_Pdf->Write();
	g2_Pi_2D_Pdf->Write();
	g2_Model_2D->Write();

	g2_Model_2D_PPi->Write();

	g_Model_Pi_dEdx->Write();
	g_Model_K_dEdx->Write();
	g_Model_P_dEdx->Write();

	g_Model_Pi_m2->Write();
	g_Model_K_m2->Write();
	g_Model_P_m2->Write();

	g2_P_2D_Pdf->Write();
	g2_Pi_2D_Pdf->Write();
	g2_K_2D_Pdf->Write();


	g2_Model_2D->Write();



	//
	// Dump Model Configuration
	//
	Model_dEdx.Print();


	Model_2D.Print();




	C("\n ----- Fake Data Set  ----");

	//
	// Book Automatic histos to cross-check distributions from the Model build before 
	//

	int npx=100,npy=100;

	TH2FAuto modTF2H("modTF2H","",npx,dEdx_x.GetMin(),dEdx_x.GetMax(),npy,m2_x.GetMin(),m2_x.GetMax());
	TH2FAuto modTF2H_P("modTF2H_P","",npx,dEdx_x.GetMin(),dEdx_x.GetMax(),npy,m2_x.GetMin(),m2_x.GetMax());
	TH2FAuto modTF2H_K("modTF2H_K","",npx,dEdx_x.GetMin(),dEdx_x.GetMax(),npy,m2_x.GetMin(),m2_x.GetMax());
	TH2FAuto modTF2H_Pi("modTF2H_Pi","",npx,dEdx_x.GetMin(),dEdx_x.GetMax(),npy,m2_x.GetMin(),m2_x.GetMax());

	TH1FAuto modTF1Hx("modTF1Hx","",npx,dEdx_x.GetMin(),dEdx_x.GetMax());
	TH1FAuto modTF1Hx_P("modTF1Hx_P","",npx,dEdx_x.GetMin(),dEdx_x.GetMax());
	TH1FAuto modTF1Hx_K("modTF1Hx_K","",npx,dEdx_x.GetMin(),dEdx_x.GetMax());
	TH1FAuto modTF1Hx_Pi("modTF1Hx_Pi","",npx,dEdx_x.GetMin(),dEdx_x.GetMax());

	TH1FAuto modTF1Hy("modTF1Hy","",npy,m2_x.GetMin(),m2_x.GetMax());
	TH1FAuto modTF1Hy_P("modTF1Hy_P","",npy,m2_x.GetMin(),m2_x.GetMax());
	TH1FAuto modTF1Hy_K("modTF1Hy_K","",npy,m2_x.GetMin(),m2_x.GetMax());
	TH1FAuto modTF1Hy_Pi("modTF1Hy_Pi","",npy,m2_x.GetMin(),m2_x.GetMax());



	//try with same relative particle ratio
	NP.SetValue(1);
	NK.SetValue(1);
	NPi.SetValue(1);

	//Build  TF1/TF2 from the Model
	TF2* TF2_Model2D = Model_2D.BuildTF2();

	TF2*  TF2_P_2D_Pdf= P_2D_Pdf.BuildTF2();
	TF2*  TF2_K_2D_Pdf= K_2D_Pdf.BuildTF2();
	TF2*  TF2_Pi_2D_Pdf= Pi_2D_Pdf.BuildTF2();

	TF1* TF1_dEdx = Model_dEdx.BuildTF1();
	TF1* TF1_dEdx_K = Model_dEdx_K.BuildTF1();
	TF1* TF1_dEdx_P = Model_dEdx_P.BuildTF1();
	TF1* TF1_dEdx_Pi = Model_dEdx_Pi.BuildTF1();

	TF1* TF1_m2 = Model_m2.BuildTF1();

	//
	// Create a DataBin container to hold "data" histos
	//
	TSDataBin myData("myData","test Data bin","test Data bin");
	myData.SetUnbinned(true);
	myData.SetVariable(dEdx_x,100,dEdx_x.GetMin(),dEdx_x.GetMax());
	myData.SetVariable(m2_x,100,m2_x.GetMin(),m2_x.GetMax());
	myData.SetBidimensional(dEdx_x,m2_x);


	double NP_frac=1;
	double NK_frac=1;
	double NPi_frac=1;

	int NENTRIES=10000;//

	int NPtrue= NENTRIES*NP_frac;
	int NPitrue= NENTRIES*NPi_frac;
	int NKtrue= NENTRIES*NK_frac;


	double xr,yr;
	double xr_p,xr_pi,xr_k;
	double yr_p,yr_pi,yr_k;

	for(int ev=0;ev<NPtrue;++ev){

	  TF2_P_2D_Pdf->GetRandom2(xr,yr);
	  dEdx_x.SetValue( xr );
	  m2_x.SetValue(  yr );

	  //the data container
	  myData.Fill();

	  //external equivalent histos
	  modTF2H.Fill(xr,yr);
	  modTF1Hx.Fill(xr);
	  modTF1Hy.Fill(yr);

	  modTF2H_P.Fill(xr,yr);
	  modTF1Hx_P.Fill(xr);
	  modTF1Hy_P.Fill(yr);
	  
	}


	for(int ev=0;ev<NPitrue;++ev){

	  TF2_Pi_2D_Pdf->GetRandom2(xr,yr);
	  dEdx_x.SetValue( xr );
	  m2_x.SetValue(  yr );

	  //the data container
	  myData.Fill();

	  //external equivalent histos
	  modTF2H.Fill(xr,yr);
	  modTF1Hx.Fill(xr);
	  modTF1Hy.Fill(yr);
	  
	  modTF2H_Pi.Fill(xr,yr);
	  modTF1Hx_Pi.Fill(xr);
	  modTF1Hy_Pi.Fill(yr);
	  
	}



	for(int ev=0;ev<NKtrue;++ev){

	  TF2_K_2D_Pdf->GetRandom2(xr,yr);
	  dEdx_x.SetValue( xr );
	  m2_x.SetValue(  yr );

	  //the data container
	  myData.Fill();

	  //external equivalent histos
	  modTF2H.Fill(xr,yr);
	  modTF1Hx.Fill(xr);
	  modTF1Hy.Fill(yr);

	  modTF2H_K.Fill(xr,yr);
	  modTF1Hx_K.Fill(xr);
	  modTF1Hy_K.Fill(yr);
	  	  
	}

	
	for(int ev=0; ev<-NENTRIES; ev++){


	  //sample form the 2D pdf of the model
	  TF2_Model2D->GetRandom2(xr,yr);

	  TF2_P_2D_Pdf->GetRandom2(xr_p,yr_p);
	  TF2_K_2D_Pdf->GetRandom2(xr_k,yr_k);
	  TF2_Pi_2D_Pdf->GetRandom2(xr_pi,yr_pi);

	  double xv[3]={xr_p,xr_k,xr_pi};
	  double yv[3]={yr_p,yr_k,yr_pi};


	  for(int i=0;i<3;++i){

	    dEdx_x.SetValue( xv[i] );
	    m2_x.SetValue(  yv[i] );

	    myData.Fill();

	    modTF2H.Fill(xv[i],yv[i]);
	    modTF1Hx.Fill(xv[i]);
	    modTF1Hy.Fill(yv[i]);
	  }


	  modTF2H_P.Fill(xr_p,yr_p);
	  modTF2H_K.Fill(xr_k,yr_k);
	  modTF2H_Pi.Fill(xr_pi,yr_pi);

	  modTF1Hx_P.Fill(xr_p);
	  modTF1Hx_K.Fill(xr_k);
	  modTF1Hx_Pi.Fill(xr_pi);

	  modTF1Hy_P.Fill(yr_p);
	  modTF1Hy_K.Fill(yr_k);
	  modTF1Hy_Pi.Fill(yr_pi);
	  
	}


	myData.GetHistoFolder()->Print();
	myData.GetHistoFolder()->Write();
	if(myData.GetDataTree()) myData.GetDataTree()->Write();


	//
	// Get true distribution function, normalized to data stat in order
        // to superimpose Function to the Histograms
	//
	Model_dEdx.NormalizeToHistogram(modTF1Hx);
	TF1* TF1_dEdx_norm = Model_dEdx.BuildTF1("TF1_Model_dEdx_normalized");
	TF1_dEdx_norm->Write();

	Model_dEdx.ResetNormalization();

	Model_dEdx_K.NormalizeToHistogram(modTF1Hx_K);
	TF1* TF1_dEdx_K_norm = Model_dEdx_K.BuildTF1("TF1_Model_dEdx_K_normalized");
	TF1_dEdx_K_norm->Write();
	Model_dEdx_K.ResetNormalization();

	Model_dEdx_P.NormalizeToHistogram(modTF1Hx_P);
	TF1* TF1_dEdx_P_norm = Model_dEdx_P.BuildTF1("TF1_Model_dEdx_P_normalized");
	TF1_dEdx_P_norm->Write();
	Model_dEdx_P.ResetNormalization();

	Model_dEdx_Pi.NormalizeToHistogram(modTF1Hx_Pi);
	TF1* TF1_dEdx_Pi_norm = Model_dEdx_Pi.BuildTF1("TF1_Model_dEdx_Pi_normalized");
	TF1_dEdx_Pi_norm->Write();
	Model_dEdx_Pi.ResetNormalization();


	Model_m2.NormalizeToHistogram(modTF1Hy);
	TF1* TF1_m2_norm = Model_m2.BuildTF1("TF1_Model_m2_normalized");
	TF1_m2_norm->Write();

	Model_m2.ResetNormalization();

	Model_2D.NormalizeToHistogram(modTF2H);
	TF2* TF2_Model_norm = Model_2D.BuildTF2("TF2_Model_2D_normalized");
	TF2_Model_norm->Write();
	
	Model_2D.Print();
	
	Model_2D.ResetNormalization();

	TF2_Model2D->Write();
	TF2_P_2D_Pdf->Write();
	TF2_K_2D_Pdf->Write();
	TF2_Pi_2D_Pdf->Write();

	modTF2H.Write();
	modTF2H_P.Write();
	modTF2H_K.Write();
	modTF2H_Pi.Write();

	modTF1Hx.Write();
	modTF1Hx_P.Write();
	modTF1Hx_K.Write();
	modTF1Hx_Pi.Write();

	modTF1Hy.Write();
	modTF1Hy_P.Write();
	modTF1Hy_K.Write();
	modTF1Hy_Pi.Write();



	C("\n ------- Chi2/Likelihood Calculator --------- ");
	

	//
	// 
	// Create different type of Calculators
        // User defined Calculators can be implemented via inheritance
	//
	//  Needed Inputs are
	//        1- Data Distribution (1D/2D Binned or Unbinned) 
	//        2- Model (1D/2D) 
        //        
	// 
	//

	//Chi2, 1D binned (dEdx distrib)
	TSChisquareCalculator myChi2_x("myChi2_x","myChi2_x");
	myChi2_x.SetModel(Model_dEdx);
	//save a tree with all the input parameters and the corresponding 
        // Chi2 values, it can be used to track the minimizatin steps of a
        // given minimization procedure
	myChi2_x.SaveHistory(false);
	// data distr taken from the myData container filled
        // by sampling from the TSModel distributons
	myChi2_x.SetBinnedData(myData,TSArgList(dEdx_x));

	myChi2_x.GetModel()->Print();

	//Likelihood, 1D binned (dEdx distrib)
	TSLikelihoodCalculator myLL_x("myLL_x","myLL_x");
	myLL_x.SetModel(Model_dEdx);
	myLL_x.SaveHistory(false);
	myLL_x.SetBinnedData(myData,TSArgList(dEdx_x));

	myLL_x.GetModel()->Print();


	//Chi2 2D binned (dEdx-m2)
	TSChisquareCalculator myChi2_2d("myChi2_2d","myChi2_2d");
	myChi2_2d.SetModel(Model_2D);
	myChi2_2d.SaveHistory(false);
	myChi2_2d.SetBinnedData(myData,TSArgList(dEdx_x,m2_x));

	myChi2_2d.GetModel()->Print();

	//Likelihood 2D binned (dEdx-m2)
	TSLikelihoodCalculator myLL_2d("myLL_2d","myLL_2d");
	myLL_2d.SetModel(Model_2D);
	myLL_2d.SaveHistory(false);

	myLL_2d.SetBinnedData(myData,TSArgList(dEdx_x,m2_x));

	myLL_2d.GetModel()->Print();


	//set some input values
	NP.SetValue(NENTRIES*NP_frac);
	NPi.SetValue(NENTRIES*NPi_frac);
	NK.SetValue(NENTRIES*NK_frac);
	dEdx_sig_delta.SetValue(truth_delta);



	//
	// Param. Scan: 1D,2D 3D grid scan
        // Sample Minimization function (Chi2/Likelihood)
        // which can be 1D (e.g.: dEdx, m2) or 2D (e.g.: dEdx-vs-m2)
	//

        //normalize Chi2/Likelihood min to 0
	myLL_x.SetNormalizedScannedMin();


	Model_dEdx.NormalizeToHistogram(modTF1Hx);
	TF1* TF1_dEdx_pre = Model_dEdx.BuildTF1("TF1_Model_dEdx_pre");
	TF1_dEdx_pre->Write();

   
	myChi2_x.SetNormalizedScannedMin();

	// Scan Chi2 values vs a given parameter
	// Get graph of Chi2 vs param delta: 300 points in the range [0.,0.5]
	TGraph* gScan_delta=myChi2_x.ScanParameter(dEdx_sig_delta,300,0.,0.5);
	gScan_delta->Write();

	double fmin,par_min;//par_min should be an array if scan is multidimensional
	myChi2_x.GetScannedMinima(fmin,&par_min);

	dEdx_sig_delta.SetValue(par_min);

	//build function corresponding the minimized value
	Model_dEdx.NormalizeToHistogram(modTF1Hx);
	TF1* TF1_dEdx_post = Model_dEdx.BuildTF1("TF1_Model_dEdx_post");
	TF1_dEdx_post->Write();

	Model_dEdx.ResetNormalization();


	cout<<"scanned minima: "<<fmin<<" "<<par_min<<endl;


	// Scan Likelihood over Proton Yield
	Model_dEdx.NormalizeToHistogram(modTF1Hx);

	TGraph* gScan_NP=myLL_x.ScanParameter(NP,300,0.,3*NENTRIES);
	gScan_NP->Write();

	myLL_x.GetScannedMinima(fmin,&par_min);

	//get function corresponding to the minized value
	NP.SetValue(par_min);
	
	Model_dEdx.NormalizeToHistogram(modTF1Hx);
	TF1* TF1_dEdx_post2 = Model_dEdx.BuildTF1("TF1_Model_dEdx_post2");
	TF1_dEdx_post2->Write();

	cout<<"scanned minima: "<<fmin<<" "<<par_min<<endl;

	//
	// 2D Chi2/Lilelihood Scanning
	// 
	NP.SetValue(NENTRIES*NP_frac);
	NPi.SetValue(NENTRIES*NPi_frac);
	NK.SetValue(NENTRIES*NK_frac);
	dEdx_sig_delta.SetValue(truth_delta);

	Model_dEdx.NormalizeToHistogram(modTF1Hx);

	// NK vs NPi, compare Chi2 vs LL
	TGraph2D* gScan_NK_NPi_LL=myLL_x.ScanParameters(NK,NPi,50,0.,2*NENTRIES*NK_frac,50,0.,2*NENTRIES*NPi_frac);
	gScan_NK_NPi_LL->Write();

	TGraph2D* gScan_NK_NPi_Chi2=myChi2_x.ScanParameters(NK,NPi,50,0.,2*NENTRIES*NK_frac,50,0.,2*NENTRIES*NPi_frac);
	gScan_NK_NPi_Chi2->Write();

	//NK, compare Chi2 vs LL
	TGraph* gScan_NK_LL=myLL_x.ScanParameter(NK,300,0.,2*NENTRIES*NK_frac);
	gScan_NK_LL->Write();

	TGraph* gScan_NK_Chi2=myChi2_x.ScanParameter(NK,300,0.,2*NENTRIES*NK_frac);
	gScan_NK_Chi2->Write();


        //Test 2D Minimization functions..
	//
	//myLL_2d.SetNormalizedScannedMin();
	//myChi2_2d.SetNormalizedScannedMin();

	NP.SetValue(NENTRIES*NP_frac);
	NPi.SetValue(NENTRIES*NPi_frac);
	NK.SetValue(NENTRIES*NK_frac);
	dEdx_sig_delta.SetValue(truth_delta);

	Model_2D.NormalizeToHistogram(modTF2H);

	//1D Scan over NK for the 2D Minimization Function
	//LL
	TGraph* gScan_NK_LL_2D = myLL_2d.ScanParameter(NK,300,0.,2*NENTRIES*NK_frac);
	gScan_NK_LL_2D->Write();

	NP.SetValue(NENTRIES*NP_frac);
	NPi.SetValue(NENTRIES*NPi_frac);
	NK.SetValue(NENTRIES*NK_frac);
	dEdx_sig_delta.SetValue(truth_delta);

	Model_2D.NormalizeToHistogram(modTF2H);

	//1D Scan over NK for the 2D Minimization Function
	//Chi2
	//TGraph* gScan_NK_Chi2_2D=myChi2_2d.ScanParameter(NK,300,0.,2*NENTRIES*NK_frac);
	//gScan_NK_Chi2_2D->Write();


	C("\n --- Setup Minimizers ---- ");

	//
	// Minimize the Chi2/Likelihood function over the space
        // of the independent parameters
	//

	//
        // Choose a minimizaition Algo as defined in TMinuit2
	// (Combined is the generic one)
	TSMinimizer TSMin("Minuit2","Combined");
	//TSMinimizer TSMin("GSLMultiMin","SteepestDescent");
	TSMin.GetMinimizer()->SetPrintLevel(5);
	TSMin.GetMinimizer()->SetMaxFunctionCalls(1e6);
	TSMin.GetMinimizer()->SetMaxIterations(1e4);
	TSMin.GetMinimizer()->SetTolerance(0.001);

	//
	//Prepare all the param input configuration for the fit
	//

	
	// Create lists of params according to typology
	// in order to handle fit settings on groups of params
        // directly
	//
	TSParamList listYields(NP,NK,NPi);
	TSParamList listMass2Means(m2_P_mean,m2_Pi_mean,m2_K_mean);
	TSParamList listMass2Widths(m2_P_sig,m2_Pi_sig,m2_K_sig);
	
	TSParamList listdEdxRelativeMeans(dEdx_mean_PtoPi,dEdx_mean_KtoPi);

	listYields.SetRange(1,NENTRIES);
	//equivalent to....
	//NP.SetRange(1.,NENTRIES);
	//NK.SetRange(1.,NENTRIES);
	//NPi.SetRange(1.,NENTRIES);

	//some random input val
	NP.SetValue(NENTRIES*NP_frac/4);
	NPi.SetValue(NENTRIES*NPi_frac/2);
	NK.SetValue(NENTRIES*NK_frac/3);
	

	dEdx_sig_delta.SetRange(0.01,0.9);
	dEdx_sig_delta.Fix();//
	//dEdx_sig_delta.SetValue(truth_delta*4);
  
	dEdx_Pi_mean.Fix();

	listdEdxRelativeMeans.Fix();
	//dEdx_mean_PtoPi.Fix();
	//dEdx_mean_KtoPi.Fix();

	dEdx_sig_0.SetRange(0.01,0.06);
	dEdx_sig_0.Fix();
	dEdx_sig_alpha.Fix();

	
	m2_P_mean.Fix();//SetRange(0.7,1.1);
	m2_P_sig.Fix();
	m2_Pi_mean.Fix();//SetRange(0.,0.25);
	m2_Pi_sig.Fix();
	m2_K_mean.Fix();
	m2_K_sig.Fix();

	//NP.Fix();
	//NPi.Fix();

	//No ele signal in this case
	dEdx_E_mean.Fix();
	dEdx_E_sig.Fix();
	NE.Fix();
	m2_E_mean.Fix();
	m2_E_sig.Fix();


	//Model_2D.NormalizeToHistogram(modTF2H);

	myLL_2d.SaveHistory(true);
       
        myLL_2d.GetModel()->Print();

	//Choose the function to minimize
        // 2D (dEdx-m2) Likelihood
	TSMin.SetFunction(myLL_2d);

	//Exe minimization
	TSMin.Minimize();

	//
	//Write out the history tree of the Minimization
	// you can check the convergency of a given param
	// as a function of the iteration step or the value
	// of the chi2(LogL), or the correlation between params
	// e.g.: TTree_myLL_2d->Draw("NK:NCall")
	//       TTree_myLL_2d->Draw("Value:NK")
	//
	if( myLL_2d.SaveHistoryIsSet() ){
	  myLL_2d.GetHistoryTree()->Write();
	}

	myLL_2d.GetModel()->Print();


	//Retrieve list of fitted params
	TSParamList *bfit_pars_2d=TSMin.GetBestFitParamList();

	bfit_pars_2d->Print();

	//Print Model setup
	Model_2D.Print();
	
        //Check integral
	//Reset Norm so that Integral equals sum
	// of Yields
	Model_2D.ResetNormalization();	
	Model_dEdx.ResetNormalization();	
	Model_m2.ResetNormalization();	
	
	cout<<"Integral "<<Model_2D.BuildTF2("")->Integral(dEdx_x.GetMin(),dEdx_x.GetMax(),m2_x.GetMin(),m2_x.GetMax())<<endl;

	cout<<"Sum of Yields "<<listYields.ComputeSum()<<" NE: "<<NE.GetValue()<<endl;

	//Write fit functions
	//Normalize in order to be able to plot functions over
	// corresponding histograms
	
	Model_2D.NormalizeToHistogram(modTF2H);
	Model_dEdx.NormalizeToHistogram(modTF1Hx);	
	Model_m2.NormalizeToHistogram(modTF1Hy);	
	
	//Build TF2 of the model with the fitted params
	TF2* TF2_Model_PostFit = Model_2D.BuildTF2("TF2_Model_2D_PostFit");
	TF2_Model_PostFit->Write();
	
	//Build TF1-dEdx of the model with the fitted params
	TF1* TF1_Model_dEdx_PostFit = Model_dEdx.BuildTF1("TF1_Model_dEdx_PostFit");
	TF1_Model_dEdx_PostFit->Write();

	//Build TF1-m2 of the model with the fitted params
	TF1* TF1_Model_m2_PostFit = Model_m2.BuildTF1("TF1_Model_m2_PostFit");
	TF1_Model_m2_PostFit->Write();

	//Build Indivial Particle fitted distributions
	Model_2D_Pi.NormalizeToHistogram(modTF2H_Pi);
	Model_dEdx_Pi.NormalizeToHistogram(modTF1Hx_Pi);
	Model_m2_Pi.NormalizeToHistogram(modTF1Hy_Pi);

	TF2* TF2_Model_2D_Pi_PostFit = (TF2*)Model_2D_Pi.BuildTF2("TF2_Model_2D_Pi_PostFit");
	TF1* TF1_Model_m2_Pi_PostFit = (TF1*)Model_m2_Pi.BuildTF1("TF1_Model_m2_Pi_PostFit");
	TF1* TF1_Model_dEdx_Pi_PostFit = (TF1*)Model_dEdx_Pi.BuildTF1("TF1_Model_dEdx_Pi_PostFit");

	TF2_Model_2D_Pi_PostFit->Write();
	TF1_Model_m2_Pi_PostFit->Write();
	TF1_Model_dEdx_Pi_PostFit->Write();
	

	
	C("\n --- Fit Data Histogram ---- ");

	//
	// From the hosto title one sees that it refers to the
        // Momentum bin [2.4,3.2] and thea bin [0,20]
	// We will then calculate the input for the <dEdx> and
        // m2 at the center of those intervals
	
	double pcenter = 2.8;
	
	dEdx_x.SetRange(hRST_Data->GetXaxis()->GetXmin(),hRST_Data->GetXaxis()->GetXmax());
	m2_x.SetRange(hRST_Data->GetYaxis()->GetXmin(),hRST_Data->GetYaxis()->GetXmax());

	myLL_2d.SetBinnedData(*hRST_Data);

	TH1F* hRST_Data_x=(TH1F*)hRST_Data->ProjectionX();
	TH1F* hRST_Data_y=(TH1F*)hRST_Data->ProjectionY();

	hRST_Data_x->Write("hRST_Data_x");
	hRST_Data_y->Write("hRST_Data_y");
	
	m2_P_mean.SetValue(tofGen.GetParamValue("proton",0,pcenter));
	m2_P_sig.SetValue(tofGen.GetParamValue("proton",1,pcenter));

	m2_Pi_mean.SetValue(tofGen.GetParamValue("pion",0,pcenter));
	m2_Pi_sig.SetValue(tofGen.GetParamValue("pion",1,pcenter));

	m2_K_mean.SetValue(tofGen.GetParamValue("kaon",0,pcenter));
	m2_K_sig.SetValue(tofGen.GetParamValue("kaon",1,pcenter));
		  
	dEdx_Pi_mean.Release();
	dEdx_Pi_mean.SetValue(bbFunc.Eval(pcenter,TSParticle::MassPiPlus));

	dEdx_mean_PtoPi.SetValue(bbFunc.Eval(pcenter,TSParticle::MassP)-bbFunc.Eval(pcenter,TSParticle::MassPiPlus));
	dEdx_mean_KtoPi.SetValue(bbFunc.Eval(pcenter,TSParticle::MassKPlus)-bbFunc.Eval(pcenter,TSParticle::MassPiPlus));
	
	dEdx_sig_0.Release();
	dEdx_sig_0.SetRange(0.01,0.05);
	dEdx_sig_0.SetValue(0.02);
			  
	dEdx_sig_alpha.SetValue(0);
	dEdx_sig_alpha.Fix();
	dEdx_sig_delta.SetValue(0);
	dEdx_sig_delta.Fix();

	dEdx_E_mean.Release();
	dEdx_E_sig.Release();
	dEdx_E_mean.SetValue(1.5);
	dEdx_E_sig.SetValue(0.04);

	listMass2Means.Release();
	listMass2Widths.Release();
	
	
	m2_E_mean.SetValue(0.);
	m2_E_sig.SetValue(0.1);
	m2_E_mean.Release();
	m2_E_sig.Release();


	listdEdxRelativeMeans.Release();
	
	//this will count also outliers (use GetEffectiveEntries() instead)
	int NDATA = hRST_Data->GetEntries();

	listYields.Release();
	listYields.SetRange(1,NDATA);

	NP.SetValue(NDATA/2.);
	NPi.SetValue(NDATA/2.);
	NK.SetValue(NDATA/2.);

	NE.Release();
	NE.SetRange(1,NDATA);
	NE.SetValue(NDATA/4.);

	//Use 4 Particles Model (fitting positrons as well)
	myLL_2d.SetModel(Model_2D_4Parts);

	//Exe minimization
	TSMin.Minimize();
	

	Model_2D.Print();
	


	//Write fit functions
	//Normalize in order to be able to plot functions over
	//corresponding histograms

	Model_2D_4Parts.ResetNormalization();
	Model_dEdx_4Parts.ResetNormalization();
	Model_m2_4Parts.ResetNormalization();

	//will give the real numnber of entries in the fitted range
	cout<<"Integral "<<Model_2D_4Parts.BuildTF2("")->Integral(hRST_Data->GetXaxis()->GetXmin(),hRST_Data->GetXaxis()->GetXmax(),hRST_Data->GetYaxis()->GetXmin(),
							   hRST_Data->GetYaxis()->GetXmax())<<endl;

	cout<<"Sum of Yields "<<listYields.ComputeSum()<<" NE: "<<NE.GetValue()<<endl;
	
	Model_2D_4Parts.NormalizeToHistogram(*hRST_Data);
	Model_dEdx_4Parts.NormalizeToHistogram(*hRST_Data_x);	
	Model_m2_4Parts.NormalizeToHistogram(*hRST_Data_y);	
	
	//Build TF2 of the model with the fitted params
	TF2* TF2_Model_Data_PostFit = Model_2D_4Parts.BuildTF2("TF2_Model_Data_2D_PostFit");
	TF2_Model_Data_PostFit->Write();
	
	//Build TF1-dEdx of the model with the fitted params
	TF1* TF1_Model_Data_dEdx_PostFit = Model_dEdx_4Parts.BuildTF1("TF1_Model_Data_dEdx_PostFit");
	TF1_Model_Data_dEdx_PostFit->Write();

	//Build TF1-m2 of the model with the fitted params
	TF1* TF1_Model_Data_m2_PostFit = Model_m2_4Parts.BuildTF1("TF1_Model_Data_m2_PostFit");
	TF1_Model_Data_m2_PostFit->Write();


	C("\n --- Loop on a Data Set/Phase Space  ---- ");

	//
	// Skeleton 
	//
	

	//can be the 2D or the 3D
	int NumOfVols=mapKin2D.GetNumOfVolumes();


	for(int ivol=1; ivol<=NumOfVols; ++ivol){

	  if(ivol!=4) continue;

	  double theta_center=mapKin2D.GetVolume(ivol)->GetAxis(2)->GetRangeCenter();
	  double theta_min=mapKin2D.GetVolume(ivol)->GetAxis(2)->GetMin();
	  double theta_max=mapKin2D.GetVolume(ivol)->GetAxis(2)->GetMax();

	  double ptot_center=mapKin2D.GetVolume(ivol)->GetAxis(1)->GetRangeCenter();
	  double ptot_min=mapKin2D.GetVolume(ivol)->GetAxis(1)->GetMin();
	  double ptot_max=mapKin2D.GetVolume(ivol)->GetAxis(1)->GetMax();

	  cout<<"Volume: "<<ivol<<endl;
	  cout<<"  Theta: "<<theta_min<<" "<<theta_center<<" "<<theta_max<<endl;
	  cout<<"  Ptot: "<<ptot_min<<" "<<ptot_center<<" "<<ptot_max<<endl;

	  TSDataBin* data_bin=dataSetMgr2D.GetDataBin(ivol);

	  TH1F *hx=(TH1F*)dataSetMgr2D.GetDataBin(ivol)->GetHistoFolder()->GetTagHisto("dEdx_x");
	  TH1F *hy=(TH1F*)dataSetMgr2D.GetDataBin(ivol)->GetHistoFolder()->GetTagHisto("m2_x");
	  
	  TH2F *hxy=(TH2F*)dataSetMgr2D.GetDataBin(ivol)->GetHistoFolder()->GetTagHisto("dEdx_x","m2_x");


	   //Eval bethe-bloch at the center of the bin
	   double bb_p = bbFunc.Eval(ptot_center,TSParticle::MassP);
	   double bb_k = bbFunc.Eval(ptot_center,TSParticle::MassKPlus);
	   double bb_pi = bbFunc.Eval(ptot_center,TSParticle::MassPiPlus);

	   //eval m2 at the center of the bin
	   double m2_p_mean=tofGen.GetParamValue("proton",0,ptot_center);
	   double m2_p_sig=tofGen.GetParamValue("proton",1,ptot_center);

	   double m2_k_mean=tofGen.GetParamValue("kaon",0,ptot_center);
	   double m2_k_sig=tofGen.GetParamValue("kaon",1,ptot_center);

	   double m2_pi_mean=tofGen.GetParamValue("pion",0,ptot_center);
	   double m2_pi_sig=tofGen.GetParamValue("pion",1,ptot_center);


	   cout<<" Volume Model Params "<<endl;
	   cout<<"Mom, Theta: "<<ptot_center<<" "<<theta_center<<endl;
	   cout<<"BB pi: "<<bb_pi<<" p: "<<bb_p<<" k: "<<bb_k<<"; p-pi: "<<bb_p-bb_pi<<" k-pi: "<<bb_k-bb_pi<<endl;
	   cout<<"ToF m2 mean  pi,p,k: "<<m2_pi_mean<<" "<<m2_p_mean<<" "<<m2_k_mean<<endl;
	   cout<<"ToF m2 width pi,p,k: "<<m2_pi_sig<<" "<<m2_p_sig<<" "<<m2_k_sig<<endl;


	    //set param inputs


	    //setup your  minimization functions and minimizer...
	    
	   

	}//




	
	C("\t == END ==");
	
	fout.cd();



	return 1;
    


}//ana_first





//
//
//
//
//
//
//
//







int main() {


  return run_ana();


}
