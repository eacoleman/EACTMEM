// This code plots the results from BackgroundEstimator for multiple rebin sizes
// 
// Author: Travis Lamb
// Started: 10th of July, 2012

// C++ Libraries
#include <iostream>
#include <vector>

// TAMU Libraries
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"

// Root Libraries
#include "TH1.h"
#include "TFile.h"

using namespace std;

int main()
{
   BackgroundEstimator testBGEst("electron", "MET", "electron.root", "RebinTest_el_MET.root");
   
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
