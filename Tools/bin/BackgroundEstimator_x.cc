// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012
// Last edit was on the 20th of July, 2012

//C++ Libraries
#include <iostream>
#include <vector>
#include <map>

//Our Libraries
#include "TAMUWW/Tools/interface/Plots.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//Root Libraries
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TMinuit.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

#include "TRandom3.h"

//Root Math Core headers
#include <Math/SpecFuncMathCore.h>
#include <Math/PdfFuncMathCore.h>
#include <Math/ProbFuncMathCore.h>

using namespace std;
using namespace ROOT::Minuit2;

map<string, TH1D*> monteCarloHistograms;
TH1D* dataHistogram;

//##################################################
//########## BACKGROUND ESTIMATION CLASS ###########
//##################################################

class BackgroundEstimator
{
private:
   //read location
   string inRootFileLocation;
   //write location
   string outRootFileLocation;
   // The lepton and object we will perform on
   string leptonName;
   string objectName;
   
   //names of the histograms
   vector<string> histNames;
   
   //output histograms
   THStack* resultStack;
   
public:
   BackgroundEstimator(string lepton, string object, string inFileLoc, string outFileLoc);
   ~BackgroundEstimator();
   
   //reads histograms from inRootFileLocation
   void readHistograms();
   //performs the fitting
   void fitHistograms();
   //writes the results to outRootFileLocation
   void writeHistograms();
   //Returns the numbers that we scale by to fit data
   //double[0] is the scale factor for QCD, double[1] is for WJets
   const double* fitAndReturnParameters();
   //These are the functions that return chi^2
   static double fitFunc(const double *par);
   //This fills resultStack
   void produceResultStacks(const double* pars);
   
private:
   void initializeFileLocations(string inFileLoc, string outFileLoc);
   void initializeHistNames();
};

//##################################################
//###################### MAIN ######################
//##################################################

int BackgroundEstimator_x(string lep, string cat, string inFileLoc, string outFileLoc)
{
   BackgroundEstimator histFit(lep, cat, inFileLoc, outFileLoc);
   
   histFit.readHistograms();
   
   // DEBUG
   double sumYay = 0;
   cout << "Data Integral: ";
   cout << dataHistogram->Integral() << endl;
   for(map<string, TH1D*>::iterator mapit = monteCarloHistograms.begin(); mapit != monteCarloHistograms.end(); mapit++)
   {
      cout << mapit->first << " Integral: ";
      cout << mapit->second->Integral() << endl;
      
      sumYay += mapit->second->Integral();
   }
   cout << "SUM: " << sumYay << endl;
   
   histFit.fitHistograms();
   
   // DEBUG
   sumYay = 0;
   cout << "Data Integral: ";
   cout << dataHistogram->Integral() << endl;
   for(map<string, TH1D*>::iterator mapit = monteCarloHistograms.begin(); mapit != monteCarloHistograms.end(); mapit++)
   {
      cout << mapit->first << " Integral: ";
      cout << mapit->second->Integral() << endl;
      
      sumYay += mapit->second->Integral();
   }
   cout << "SUM: " << sumYay << endl;
   
   histFit.writeHistograms();
   
   return 0;
}

//##################################################
//################ PUBLIC FUNCTIONS ################
//##################################################

BackgroundEstimator::BackgroundEstimator(string lepton, string object, string inFileLoc, string outFileLoc)
{
   leptonName = lepton;
   objectName = object;
   
   initializeFileLocations(inFileLoc, outFileLoc);
   initializeHistNames();
   
   resultStack = new THStack((objectName + "_MonteCarlo").c_str(), (objectName + "_MonteCarlo").c_str());
}

BackgroundEstimator::~BackgroundEstimator()
{
   
}

void BackgroundEstimator::readHistograms()
{
   TFile rootInFile;
   rootInFile.Open(inRootFileLocation.c_str(), "READ");
   
   // MC Histograms
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      string name = histNames[i];
      string prefix = objectName + "_" + leptonName + "_";
      
         monteCarloHistograms[name] = (TH1D*)gDirectory->Get((prefix + name).c_str());
   }
   
   string prefix = objectName + "_" + leptonName + "_";
   // Data histogram
   dataHistogram = (TH1D*)gDirectory->Get((prefix + "SingleEl_Data").c_str());
   
   rootInFile.Close();
}

void BackgroundEstimator::fitHistograms()
{
   const double* parameters = fitAndReturnParameters();
   
   produceResultStacks(parameters);
}

void BackgroundEstimator::writeHistograms()
{
   TFile rootOutFile;
   rootOutFile.Open(outRootFileLocation.c_str(), "RECREATE");
   
   resultStack->Write();
   
   dataHistogram->Write();
   
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      monteCarloHistograms[histNames[i]]->Write();
   }
   
   rootOutFile.Close();
}

//##################################################
//############### PRIVATE FUNCTIONS ################
//##################################################

void BackgroundEstimator::initializeFileLocations(string inFileLoc, string outFileLoc)
{
   //inRootFileLocation = "/uscms/home/travlamb/CMSSW_5_2_5/src/TAMUWW/Tools/bin/" + leptonName + ".root";
   //outRootFileLocation = "/uscms_data/d3/travlamb/BackgroundEstimation_" + leptonName + "_" + objectName + ".root";
   inRootFileLocation = inFileLoc;
   outRootFileLocation = outFileLoc;
}

void BackgroundEstimator::initializeHistNames()
{
   histNames.push_back("WJets");
   histNames.push_back("WW");
   histNames.push_back("WZ");
   histNames.push_back("DYJets");
   histNames.push_back("TTbar");
   histNames.push_back("STopT_T");
   histNames.push_back("STopS_T");
   histNames.push_back("STopS_Tbar");
   histNames.push_back("QCD");
   histNames.push_back("STopTW_T");
   histNames.push_back("STopTW_Tbar");
}

const double* BackgroundEstimator::fitAndReturnParameters()
{
   ROOT::Math::Functor funcFit(&fitFunc,2);
   Minuit2Minimizer* minFit = new Minuit2Minimizer(kMigrad);
   
   //Tolerance and printouts
   minFit->SetPrintLevel(3);
   minFit->SetStrategy(2);
   minFit->SetMaxFunctionCalls(8000);
   minFit->SetMaxIterations(8000);
   minFit->SetTolerance(0.1);
   minFit->SetErrorDef(0.5);
   minFit->SetValidError(true);

   // Fitting
   minFit->SetFunction(funcFit);
   
   double parameter[2] = {1.0, 10.0};
   double step[2] = {0.01, 0.01};
   //double lower[2] = {0.0, 0.0};
   
   minFit->SetVariable(0, "Par0", parameter[0], step[0]);
   minFit->SetVariable(1, "Par1", parameter[1], step[1]);
   //minFit->SetLowerLimitedVariable(0, "Par0", parameter[0], step[0], lower[0]);
   //minFit->SetLowerLimitedVariable(1, "Par1", parameter[1], step[1], lower[1]);
   
   cout << endl << "Beginning Fit..." << endl;
   if(minFit->Minimize())
      cout << endl << "Fit Successful!" << endl;
   else
      cout << endl << "Fit Failed." << endl;
   
   cout << endl << "##### FIT RESULTS #####" << endl;
   minFit->PrintResults();
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << dataHistogram->GetNbinsX() - 2 << endl;
   cout << "Chi2/NDF\t  = " << minFit->MinValue() / (dataHistogram->GetNbinsX() - 2) << endl;
   
   return minFit->X();
}

void BackgroundEstimator::produceResultStacks(const double* pars)
{
   // results
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      string name = histNames[i];
      if(name == "QCD")
      {
         monteCarloHistograms[name]->Scale(pars[0]);
      }
      if(name == "WJets")
      {
         monteCarloHistograms[name]->Scale(pars[1]);
      }
      
      resultStack->Add(monteCarloHistograms[name]);
   }
}

//##################################################
//################ STATIC FUNCTIONS ################
//##################################################

double BackgroundEstimator::fitFunc(const double *par)
{
   dataHistogram->GetNbinsX();
   double chiSquare = 0;
   
   for(int bin = 1; bin <= dataHistogram->GetNbinsX(); bin++)
   {
      double mc = 0;
      double mcError = 0;
      double data = 0;
      
      mc += par[0]*monteCarloHistograms["QCD"]->GetBinContent(bin);
      mc += par[1]*monteCarloHistograms["WJets"]->GetBinContent(bin);
      
      mcError += par[0]*monteCarloHistograms["QCD"]->GetBinError(bin);
      mcError += par[1]*monteCarloHistograms["WJets"]->GetBinError(bin);
      
      data += dataHistogram->GetBinContent(bin);
   
      for(map<string, TH1D*>::iterator it = monteCarloHistograms.begin(); it != monteCarloHistograms.end(); it++)
      {
         if( ((*it).first != "QCD") && ((*it).first != "WJets") )
         {
            mc += (*it).second->GetBinContent(bin);
            mcError += (*it).second->GetBinError(bin);
         }
      }
      
      if(mcError == 0 || mc == 0 || data == 0)
         continue;
      
      double delta = (data - mc) / mcError;
      
      chiSquare += delta*delta;
   }
   return chiSquare;
}

//##################################################
//###################### MAIN ######################
//##################################################

int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string leptonCL = cl.getValue<string> ("lepton", "");
   string objectCL = cl.getValue<string> ("object", "");
   string inFileLocCL = cl.getValue<string> ("readLocation", "/uscms/home/travlamb/CMSSW_5_2_5/src/TAMUWW/Tools/bin/electron.root");
   string outFileLocCL = cl.getValue<string> ("writeLocation", "/uscms_data/d3/travlamb/BackgroundEstimation_electron_MET.root");
   
   if (!cl.check()) return 0;
   cl.print();
   
   if(!(leptonCL == "muon" || leptonCL == "electron"))
   {
      cout << "ERROR: lepton was not properly set. Options are electron and muon." << endl;
      return 1;
   }
   if(!(objectCL == "MET" || objectCL == "WmT"))
   {
      cout << "ERROR: object was not properly set. Options are MET and WmT." << endl;
      return 1;
   }
   
   return BackgroundEstimator_x(leptonCL, objectCL, inFileLocCL, outFileLocCL);
}
