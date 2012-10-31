// This code plots the results from BackgroundEstimator for multiple rebin sizes
// 
// Author: Travis Lamb
// Started: 10th of July, 2012

// C++ Libraries
#include <iostream>
#include <vector>

// TAMU Libraries
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

// Root Libraries
#include "TH1.h"
#include "TFile.h"

using namespace std;

int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string leptonCL     = cl.getValue<string> ("lepton",        "");
   string objectCL     = cl.getValue<string> ("object",        "");
   string inFileLocCL  = cl.getValue<string> ("readLocation",  "");
   string outFileLocCL = cl.getValue<string> ("writeLocation", "");
   
   if (!cl.check()) 
      return 0;
   cl.print();
   
   BackgroundEstimator testBGEst(leptonCL, objectCL, inFileLocCL, outFileLocCL);
   
   vector<double> parameterZero;
   vector<double> parameterOne;
   vector<double> chiSquared;
   
   parameterZero.push_back(0);
   parameterOne.push_back(0);
   chiSquared.push_back(0);
   
   for(unsigned int i = 1; i <= 20; i++)
   {
      cout << "Running a test with rebin size = " << i << "..." << endl;
      
      testBGEst.setRebinSizeDEBUG(i);
      testBGEst.readHistograms();
      
      testBGEst.fitHistograms();
      
      pair<double, double> parameters = testBGEst.getParameters();
      
      parameterZero.push_back(parameters.first);
      parameterOne.push_back(parameters.second);
      chiSquared.push_back(testBGEst.getReducedChiSquared());
      
      cout << "Par0:" << parameters.first << "\tPar1:" << parameters.second << endl;
   }
   
   TH1D* parZeroPlot = new TH1D("ParameterZero", "ParameterZero", 15, 0, 1.5);
   TH1D* parOnePlot = new TH1D("ParameterOne", "ParameterOne", 30, 0, 1.5);
   
   cout << endl << "##### RESULTS #####" << endl;
   cout << "Rebin   Par0   Par1   Chi2" << endl;
   for(unsigned int i = 1; i <= 20; i++)
   {
      cout << i << "   " << parameterZero[i] << "   " << parameterOne[i] << "   " << chiSquared[i] << endl;
      
      parZeroPlot->Fill(parameterZero[i], 1/chiSquared[i]);
      parOnePlot->Fill(parameterOne[i], 1/chiSquared[i]);
   }
   
   parZeroPlot->GetXaxis()->SetTitle("QCD Scale Factor");
   parOnePlot->GetXaxis()->SetTitle("WJets Scale Factor");
   
   TFile output("parameterPlots.root", "RECREATE");
   
   parZeroPlot->Write();
   parOnePlot->Write();
   
   output.Close();
   
   cout << "Done." << endl;
   return 0;
}
