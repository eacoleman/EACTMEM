// This code plots the results from BackgroundEstimator for multiple rebin sizes
// 
// Author: Travis Lamb
// Started: 10th of July, 2012

// C++ Libraries
#include <iostream>
#include <vector>

// TAMU Libraries
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"

using namespace std;

int main()
{
   BackgroundEstimator testBGEst("electron", "MET", "electron.root", "RebinTest_el_MET.root");
   
   //vector<double> parameterZero;
   //vector<double> parameterOne;
   
   //parameterZero.push_back(0);
   //parameterOne.push_back(0);
   
   //for(unsigned int i = 1; i <= 20; i++)
   //{
   //cout << "Running a test with rebin size = " << i << "..." << endl;
      
   //testBGEst.setRebinSizeDEBUG(i);
      testBGEst.readHistograms();
      
      //const double* parameters = testBGEst.fitHistograms();
      
      //parameterZero.push_back(parameters[0]);
      //parameterOne.push_back(parameters[1]);
      
      //cout << "Par0:" << parameters[0] << "\tPar1:" << parameters[1] << endl;
      //}
   
      //cout << endl << "##### RESULTS #####" << endl;
      //cout << "Rebin\tPar0\tPar1" << endl;
      //for(unsigned int i = 1; i <= 20; i++)
      //{
      //cout << i << "\t" << parameterZero[i] << "\t" << parameterOne[i] << endl;
      //}
   
   cout << "Done." << endl;
   return 0;
}
