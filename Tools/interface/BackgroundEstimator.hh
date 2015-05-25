#ifndef BACKGROUNDESTIMATOR_HH
#define BACKGROUNDESTIMATOR_HH
// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012

//TAMU Libraries
#include "TAMUWW/Tools/interface/Plots.hh"

// C++ Libraries
#include <iostream>
#include <vector>
#include <map>
#include <utility>

// Root Libraries
#include "TROOT.h"
#include "TPad.h"
#include "TH1.h"
#include "THStack.h"
#include "TFile.h"
#include "TMinuit.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

// Root Math Core headers
#include <Math/SpecFuncMathCore.h>
#include <Math/PdfFuncMathCore.h>
#include <Math/ProbFuncMathCore.h>

//##################################################
//########## BACKGROUND ESTIMATION CLASS ###########
//##################################################

class BackgroundEstimator
{
private:
   //read location
   std::string inRootFileLocation;
   //write location
   std::string outRootFileLocation;
   // The lepton and object we will perform on
   std::string leptonName;
   std::string objectName;
   
   //names of the histograms
   std::vector<std::string> histNames;
   
   //output histograms
   THStack* resultStack;
   
   //Debug options
   unsigned int rebinSizeDEBUG;
   bool debug;
   
   std::pair<double, double> scaleParameters;
   double reducedChiSquared;
   
   //These are used to clone the input canvas
   TCanvas* canvas;
   TPad* pad;
   THStack* mcStack;
   THStack* dataStack;
   TList* mcList;
   TList* dataList;

   ROOT::Minuit2::Minuit2Minimizer*  minFit; 
   ROOT::Math::IMultiGenFunction * funcFit;
   
public:
   BackgroundEstimator(std::string lepton, std::string object, std::string inFileLoc, std::string outFileLoc = "");
   ~BackgroundEstimator();
   
   //reads histograms from inRootFileLocation
   void readHistograms();
   //performs the fitting
   void fitHistograms();
   //writes the results to outRootFileLocation
   void writeHistograms();
   
   //These are the functions that return chi^2
   double fitFunc(const double *par);
   std::map<std::string, TH1D*> monteCarloHistograms;
   TH1D* dataHistogram;

   //Changes the write location
   void setWriteLocation(std::string outFileLoc);
   //Changes the write location
   void setReadLocation(std::string inFileLoc);
   //A debug option that rebins the histograms as it reads them in.
   void setRebinSizeDEBUG(unsigned int rebinSize);
   
   // Returns a pair with the parameters
   // first is QCD scale factor, second is WJets scale factor
   std::pair<double, double> getParameters();
   // Returns the Chi2/NDF
   double getReducedChiSquared();
   
private:
   void initializeFileLocations(std::string inFileLoc, std::string outFileLoc);
   void initializeHistNames();
   //Returns the numbers that we scale by to fit data
   //double[0] is the scale factor for QCD, double[1] is for WJets
   const double* fitAndReturnParameters();
   //Colors each monteCarlo Process
   void colorMonteCarlo();
   //This fills resultStack
   void produceResultStacks(const double* pars);
   //Returns a vector filled with the different plot names (including MET)
   // e.g. "AngleJ1J2", "MET", "j1Pt_Mjj"
   std::vector<std::string> getPlotNames();
};

#endif
