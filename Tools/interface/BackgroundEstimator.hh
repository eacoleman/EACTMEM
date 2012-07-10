// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012

// C++ Libraries
#include <iostream>
#include <vector>
#include <map>

// Root Libraries
#include "TH1.h"
#include "THStack.h"

namespace Histograms
{
   std::map<std::string, TH1D*> monteCarloHistograms;
   TH1D* dataHistogram;
}

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
   
public:
   BackgroundEstimator(std::string lepton, std::string object, std::string inFileLoc, std::string outFileLoc);
   ~BackgroundEstimator();
   
   //reads histograms from inRootFileLocation
   void readHistograms();
   //performs the fitting
   const double* fitHistograms();
   //writes the results to outRootFileLocation
   void writeHistograms();
   //Returns the numbers that we scale by to fit data
   //double[0] is the scale factor for QCD, double[1] is for WJets
   const double* fitAndReturnParameters();
   //These are the functions that return chi^2
   static double fitFunc(const double *par);
   //Colors each monteCarlo Process
   void colorMonteCarlo();
   //This fills resultStack
   void produceResultStacks(const double* pars);
   //A debug option that rebins the histograms as it reads them in.
   void setRebinSizeDEBUG(unsigned int rebinSize);
   
private:
   void initializeFileLocations(std::string inFileLoc, std::string outFileLoc);
   void initializeHistNames();
};
