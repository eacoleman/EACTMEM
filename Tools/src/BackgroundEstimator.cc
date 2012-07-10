// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012

//C++ Libraries
#include <iostream>
#include <vector>
#include <map>

// TAMU Libraries
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"
#include "TAMUWW/Tools/interface/Plots.hh"

// Root Libraries
#include "TROOT.h"
#include "TFile.h"
#include "TMinuit.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

// Root Math Core headers
#include <Math/SpecFuncMathCore.h>
#include <Math/PdfFuncMathCore.h>
#include <Math/ProbFuncMathCore.h>

using namespace std;
using namespace ROOT::Minuit2;

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
   
   // Set default debug options
   rebinSizeDEBUG = 1;
   debug = false;
}

BackgroundEstimator::~BackgroundEstimator()
{
   // Destroy montecarlo histograms
   for(map<string, TH1D*>::iterator mapIt = Histograms::monteCarloHistograms.begin(); mapIt != Histograms::monteCarloHistograms.begin(); mapIt++)
   {
      mapIt->second->~TH1D();
      delete mapIt->second;
   }
   
   Histograms::monteCarloHistograms.clear();
   
   // Destroy data histogram
   Histograms::dataHistogram->~TH1D();
   delete Histograms::dataHistogram;
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
      
      Histograms::monteCarloHistograms[name] = (TH1D*)gDirectory->Get((prefix + name).c_str());
      if (debug)
         Histograms::monteCarloHistograms[name]->Rebin(rebinSizeDEBUG);
   }
   
   string prefix = objectName + "_" + leptonName + "_";
   // Data histogram
   Histograms::dataHistogram = (TH1D*)gDirectory->Get((prefix + "SingleEl_Data").c_str());
   if (debug)
      Histograms::dataHistogram->Rebin(rebinSizeDEBUG);
   
   rootInFile.Close();
}

const double* BackgroundEstimator::fitHistograms()
{
   const double* parameters = fitAndReturnParameters();
   
   colorMonteCarlo();
   produceResultStacks(parameters);
   
   return parameters;
}

void BackgroundEstimator::writeHistograms()
{
   TFile rootOutFile;
   rootOutFile.Open(outRootFileLocation.c_str(), "RECREATE");
   
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      Histograms::monteCarloHistograms[histNames[i]]->Write();
   }
   
   resultStack->Write();
   
   Histograms::dataHistogram->Write();
   
   rootOutFile.Close();
}

void BackgroundEstimator::setRebinSizeDEBUG(unsigned int rebinSize)
{
   debug = true;
   rebinSizeDEBUG = rebinSize;
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
   //Minuit2Minimizer* minFit = new Minuit2Minimizer();
   
   // Tolerance and printouts
   minFit->SetPrintLevel(3);
   minFit->SetStrategy(2);
   minFit->SetMaxFunctionCalls(8000);
   minFit->SetMaxIterations(8000);
   minFit->SetTolerance(0.1);
   minFit->SetErrorDef(0.5);
   minFit->SetValidError(true);
   
   // Fitting
   minFit->SetFunction(funcFit);
   
   double parameter[2] = {1.0, 1.0};
   double step[2] = {0.01, 0.01};
   double lower[2] = {0.0, 0.0};
   
   //minFit->SetVariable(0, "Par0", parameter[0], step[0]);
   //minFit->SetVariable(1, "Par1", parameter[1], step[1]);
   minFit->SetLowerLimitedVariable(0, "Par0", parameter[0], step[0], lower[0]);
   minFit->SetLowerLimitedVariable(1, "Par1", parameter[1], step[1], lower[1]);
   
   cout << endl << "Beginning Fit..." << endl;
   if(minFit->Minimize())
      cout << endl << "Fit Successful!" << endl;
   else
      cout << endl << "Fit Failed." << endl;
   
   cout << endl << "##### FIT RESULTS #####" << endl;
   minFit->PrintResults();
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << Histograms::dataHistogram->GetNbinsX() - 2 << endl;
   cout << "Chi2/NDF\t  = " << minFit->MinValue() / (Histograms::dataHistogram->GetNbinsX() - 2) << endl;
   
   return minFit->X();
}

void BackgroundEstimator::colorMonteCarlo()
{
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      string name = histNames[i];
      if (name == "WJets")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kTeal+2);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kTeal+2);
         Histograms::monteCarloHistograms[name]->SetLineColor(kTeal+2);
      }
      else if (name == "WW")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kPink);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kPink);
         Histograms::monteCarloHistograms[name]->SetLineColor(kPink);
      }
      else if (name == "WZ")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kBlue);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kBlue);
         Histograms::monteCarloHistograms[name]->SetLineColor(kBlue);
      }
      else if (name == "DYJets")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kViolet+5);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kViolet+5);
         Histograms::monteCarloHistograms[name]->SetLineColor(kViolet+5);
      }
      else if (name == "TTbar")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kAzure-2);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kAzure-2);
         Histograms::monteCarloHistograms[name]->SetLineColor(kAzure-2);
      }
      else if (name == "STopT_T")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kOrange+1);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kOrange+1);
         Histograms::monteCarloHistograms[name]->SetLineColor(kOrange+1);
      }
      else if (name == "STopS_T")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kBlue);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kBlue);
         Histograms::monteCarloHistograms[name]->SetLineColor(kBlue);
      }
      else if (name == "STopS_Tbar")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kBlue+3);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kBlue+3);
         Histograms::monteCarloHistograms[name]->SetLineColor(kBlue+3);
      }
      else if (name == "QCD")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kCyan);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kCyan);
         Histograms::monteCarloHistograms[name]->SetLineColor(kCyan);
      }
      else if (name == "STopTW_T")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kMagenta);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kMagenta);
         Histograms::monteCarloHistograms[name]->SetLineColor(kMagenta);
      }
      else if (name == "STopTW_Tbar")
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kGreen+3);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kGreen+3);
         Histograms::monteCarloHistograms[name]->SetLineColor(kGreen+3);
      }
      else
      {
         Histograms::monteCarloHistograms[name]->SetFillColor(kYellow);
         Histograms::monteCarloHistograms[name]->SetMarkerColor(kYellow);
         Histograms::monteCarloHistograms[name]->SetLineColor(kYellow);
      }
   }
   
   Histograms::dataHistogram->SetLineColor(kBlack);
   Histograms::dataHistogram->SetMarkerColor(kBlack);
   Histograms::dataHistogram->SetFillColor(kBlack);
   Histograms::dataHistogram->SetMarkerStyle(8);
}

void BackgroundEstimator::produceResultStacks(const double* pars)
{
   // results
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      string name = histNames[i];
      if(name == "QCD")
      {
         Histograms::monteCarloHistograms[name]->Scale(pars[0]);
      }
      if(name == "WJets")
      {
         Histograms::monteCarloHistograms[name]->Scale(pars[1]);
      }
      
      resultStack->Add(Histograms::monteCarloHistograms[name], "hist");
   }
}

//##################################################
//################ STATIC FUNCTIONS ################
//##################################################

double BackgroundEstimator::fitFunc(const double *par)
{
   Histograms::dataHistogram->GetNbinsX();
   double chiSquare = 0;
   
   for(int bin = 1; bin <= Histograms::dataHistogram->GetNbinsX(); bin++)
   {
      double mc = 0;
      double mcError2 = 0;
      double data = 0;
      double dataError2 = 0;
      
      mc += par[0]*Histograms::monteCarloHistograms["QCD"]->GetBinContent(bin);
      mc += par[1]*Histograms::monteCarloHistograms["WJets"]->GetBinContent(bin);
      
      mcError2 += pow(par[0]*Histograms::monteCarloHistograms["QCD"]->GetBinError(bin), 2);
      mcError2 += pow(par[1]*Histograms::monteCarloHistograms["WJets"]->GetBinError(bin), 2);
      
      data += Histograms::dataHistogram->GetBinContent(bin);
      dataError2 += pow(Histograms::dataHistogram->GetBinError(bin), 2);
   
      for(map<string, TH1D*>::iterator it = Histograms::monteCarloHistograms.begin(); it != Histograms::monteCarloHistograms.end(); it++)
      {
         if( ((*it).first != "QCD") && ((*it).first != "WJets") )
         {
            mc += (*it).second->GetBinContent(bin);
            mcError2 += pow((*it).second->GetBinError(bin), 2);
         }
      }
      
      if(mcError2 == 0 || mc == 0 || data == 0)
         continue;
      else
      {
         double chi = (data - mc) / sqrt(mcError2 + dataError2);
         chiSquare += chi*chi;
         
         // DEBUG
         // if (fabs(par[0] - par[1]) > 0.1)
         //    cout << "Bin:" << bin
         //         << "   Data:" << data 
         //         << "   dataError:" << sqrt(dataError2) 
         //         << "   MC:" << mc 
         //         << "   MCError:" << sqrt(mcError2) 
         //         << "   Par0:" << par[0] 
         //         << "   Par1:" << par[1] 
         //         << "   Chi2:" << chiSquare << endl;
      }
   }
   // cout << endl << "Par0:" << par[0] << " Par1:" << par[1] << " Chi2:" << chiSquare << endl << endl;
   return chiSquare;
}
