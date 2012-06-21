// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012
// Last edit was on the 20th of July, 2012

//map<string, aPlot> plots = getPlots(leptonCatString);
//

//C++ Libraries
#include <iostream>
#include <vector>
#include <map>

//Our Libraries
#include "TAMUWW/Tools/interface/PlotterAux.hh"
//#include "TAMUWW/Tools/bin/plotter_x.cc"

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

map<string, TH1D*> monteCarloMETHistograms;
map<string, TH1D*> monteCarloWmTHistograms;
TH1D* dataMETHistogram;
TH1D* dataWmTHistogram;

//##################################################
//############### HISTOGRAMFIT CLASS ###############
//##################################################

class HistogramFit
{
private:
   //read location
   string inRootFileLocation;
   //write location
   string outRootFileLocation;
   
   //names of the histograms
   vector<string> METHistNames;
   vector<string> WmTHistNames;
   
   //output histograms
   THStack* METResult;
   THStack* WmTResult;
   
public:
   HistogramFit();
   ~HistogramFit();
   
   //reads histograms from inRootFileLocation
   void readHistograms();
   //performs the fitting
   void fitHistograms();
   //writes the results to outRootFileLocation
   void writeHistograms();
   //Returns the numbers that we scale by to fit data
   //double[0] is the scale factor for QCD, double[1] is for WJets
   const double* fitMETAndReturnParameters();
   const double* fitWmTAndReturnParameters();
   //These are the functions that return chi^2
   static double fitFuncMET(const double *par);
   static double fitFuncWmT(const double *par);
   //This fills METResult and WmTResult
   void produceResultStacks(const double* METPars, const double* WmTPars);
   
private:
   void initializeFileLocations();
   void initializeHistNames();
};

//##################################################
//###################### MAIN ######################
//##################################################

int HistFit_x()
{
   /*HistogramFit histFit;
   
   histFit.readHistograms();
   histFit.fitHistograms();
   histFit.writeHistograms();
   
   //DRAW HISTOGRAMS
   
   TH1D* h1 = new TH1D("h1", "h1", 100, -100, 100);
   TH1D* h2 = new TH1D("h2", "h2", 100, -100, 100);
   
   TH1D* hdata = new TH1D("hdata", "hdata", 100, -100, 100);
   
   TRandom3 rn;
   
   for(unsigned int i = 0; i < 10000; i++)
   {
      h1->Fill(rn.Gaus(-20,10));
      h2->Fill(rn.Gaus(20,10));
      
      hdata->Fill(rn.Gaus(-20,10));
      hdata->Fill(rn.Gaus(-20,10));
      hdata->Fill(rn.Gaus(20,10));
      hdata->Fill(rn.Gaus(20,10));
   }
   
   THStack* hstack = new THStack;
   
   hstack->Add(h1);
   hstack->Add(h2);
   
   hdata->Draw("Ep");
   
   h1->Draw("Same");
   h2->Draw("Same");
   hstack->Draw("Same");*/
   
   return 0;
}

//##################################################
//################ PUBLIC FUNCTIONS ################
//##################################################

HistogramFit::HistogramFit()
{
   initializeFileLocations();
   initializeHistNames();
   
   METResult = new THStack("METMonteCarlo", "METMonteCarlo");
   WmTResult = new THStack("WmTMonteCarlo", "WmTMonteCarlo");
}

HistogramFit::~HistogramFit()
{
   
}

void HistogramFit::readHistograms()
{
   TFile rootInFile;
   rootInFile.Open(inRootFileLocation.c_str(), "READ");
   
   // MET MC Histograms
   for(unsigned int i = 0; i < METHistNames.size(); i++)
   {
      string name = METHistNames[i];
      
      monteCarloMETHistograms[name] = (TH1D*)gDirectory->Get(name.c_str());
   }
   
   // WmT MC Histograms
   for(unsigned int i = 0; i < WmTHistNames.size(); i++)
   {
      string name = WmTHistNames[i];
      
      monteCarloWmTHistograms[name] = (TH1D*)gDirectory->Get(name.c_str());
   }
   
   // Data histograms
   dataMETHistogram = (TH1D*)gDirectory->Get("MET_muon_Data (mu)");
   dataWmTHistogram = (TH1D*)gDirectory->Get("WmT_muon_Data (mu)");
   
   rootInFile.Close();
}

void HistogramFit::fitHistograms()
{
   const double* METParameters = fitMETAndReturnParameters();
   const double* WmTParameters = fitWmTAndReturnParameters();
   
   produceResultStacks(METParameters, WmTParameters);
}

void HistogramFit::writeHistograms()
{
   TFile rootOutFile;
   rootOutFile.Open("/uscms_data/d3/travlamb/HistFit.root", "RECREATE");
   
   METResult->Write();
   WmTResult->Write();
   
   dataMETHistogram->Write();
   dataWmTHistogram->Write();
   
   for(unsigned int i = 0; i < METHistNames.size(); i++)
   {
      monteCarloMETHistograms[METHistNames[i]]->Write();
   }
   for(unsigned int i = 0; i < WmTHistNames.size(); i++)
   {
      monteCarloWmTHistograms[WmTHistNames[i]]->Write();
   }
   
   //TESTING
   
   rootOutFile.Close();
}

//##################################################
//############### PRIVATE FUNCTIONS ################
//##################################################

void HistogramFit::initializeFileLocations()
{
   inRootFileLocation = "/uscms_data/d3/travlamb/outputFile_muon_travis.root";
   outRootFileLocation = "/uscms_data/d3/travlamb/HistFit.root";
}

void HistogramFit::initializeHistNames()
{
   METHistNames.push_back("MET_muon_WJets");
   METHistNames.push_back("MET_muon_WW");
   METHistNames.push_back("MET_muon_WZ");
   METHistNames.push_back("MET_muon_DY");
   METHistNames.push_back("MET_muon_STopT_T");
   METHistNames.push_back("MET_muon_STopS_Tbar");
   
   //METHistNames.push_back("MET_muon_QCD_Mu");//EMPTY
   //METHistNames.push_back("MET_muon_STopTW_T");//EMPTY
   //METHistNames.push_back("MET_muon_STopTW_Tbar");//EMPTY
   
   //METHistNames.push_back("MET_muon_Data (mu)");
   
   WmTHistNames.push_back("WmT_muon_WJets");
   WmTHistNames.push_back("WmT_muon_WW");
   WmTHistNames.push_back("WmT_muon_WZ");
   WmTHistNames.push_back("WmT_muon_DY");
   WmTHistNames.push_back("WmT_muon_STopT_T");
   WmTHistNames.push_back("WmT_muon_STopS_Tbar");
   
   //WmTHistNames.push_back("WmT_muon_QCD_Mu");//EMPTY
   //WmTHistNames.push_back("WmT_muon_STopTW_T");//EMPTY
   //WmTHistNames.push_back("WmT_muon_STopTW_Tbar");//EMPTY
   
   //WmTHistNames.push_back("WmT_muon_Data (mu)");
}

const double* HistogramFit::fitMETAndReturnParameters()
{
   ROOT::Math::Functor funcMET(&fitFuncMET,2);
   Minuit2Minimizer* minFit = new Minuit2Minimizer(kMigrad);
   
   // Fitting MET
   minFit->SetFunction(funcMET);
   
   double parameter[2] = {1.0, 1.0};
   double step[2] = {0.01, 0.01};
   
   minFit->SetVariable(0, "Par0", parameter[0], step[0]);
   minFit->SetVariable(1, "Par1", parameter[1], step[1]);

   cout << endl << "Beginning Fit..." << endl;
   if(minFit->Minimize())
      cout << endl << "Fit Successful!" << endl;
   else
      cout << endl << "Fit Failed." << endl;
   
   cout << endl << "##### MET FIT RESULTS #####" << endl;;
   minFit->PrintResults();
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << 2 << endl;
   cout << "Chi2/NDF\t  = " << minFit->MinValue() / 2 << endl;
   
   return minFit->X();
}

const double* HistogramFit::fitWmTAndReturnParameters()
{
   ROOT::Math::Functor funcWmT(&fitFuncWmT,2);
   Minuit2Minimizer* minFit = new Minuit2Minimizer(kMigrad);
   
   // Fitting WmT
   minFit->SetFunction(funcWmT);
   
   double parameter[2] = {1.0, 1.0};
   double step[2] = {0.01, 0.01};
   
   minFit->SetVariable(0, "Par0", parameter[0], step[0]);
   minFit->SetVariable(1, "Par1", parameter[1], step[1]);

   cout << endl << "Beginning Fit..." << endl;
   if(minFit->Minimize())
      cout << endl << "Fit Successful!" << endl;
   else
      cout << endl << "Fit Failed." << endl;
   
   cout << endl << "##### WmT FIT RESULTS #####" << endl;;
   minFit->PrintResults();
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << 2 << endl;
   cout << "Chi2/NDF\t  = " << minFit->MinValue() / 2 << endl;
   
   return minFit->X();
}

void HistogramFit::produceResultStacks(const double* METPars, const double* WmTPars)
{
   //MET results
   for(unsigned int i = 0; i < METHistNames.size(); i++)
   {
      string name = METHistNames[i];
      //if(name == "MET_muon_QCD_Mu")
      if(name == "MET_muon_WW")//TEMP
      {
         monteCarloMETHistograms[name]->Scale(METPars[0]);
      }
      if(name == "MET_muon_WJets")
      {
         monteCarloMETHistograms[name]->Scale(METPars[1]);
      }
      
      METResult->Add(monteCarloMETHistograms[name]);
   }
   
   //WmT results
   for(unsigned int i = 0; i < WmTHistNames.size(); i++)
   {
      string name = WmTHistNames[i];
      //if(name == "WmT_muon_QCD_Mu")
      if(name == "WmT_muon_WW")//TEMP
      {
         monteCarloWmTHistograms[name]->Scale(WmTPars[0]);
      }
      if(name == "WmT_muon_WJets")
      {
         monteCarloWmTHistograms[name]->Scale(WmTPars[1]);
      }
      
      WmTResult->Add(monteCarloWmTHistograms[name]);
   }
}

//##################################################
//################ STATIC FUNCTIONS ################
//##################################################

double HistogramFit::fitFuncMET(const double *par)
{
   double chiSquare = 0;
   
   for(int bin = 1; bin <= dataMETHistogram->GetNbinsX(); bin++)
   {
      double mc = 0;
      //double mcError = 0;
      double mcError = 1;//TEMP
      double data = 0;
      
      //mc += par[0]*monteCarloMETHistograms["MET_muon_QCD_Mu"]->GetBinContent(bin);
      mc += par[0]*monteCarloMETHistograms["MET_muon_WW"]->GetBinContent(bin);//TEMP
      mc += par[1]*monteCarloMETHistograms["MET_muon_WJets"]->GetBinContent(bin);
      
      //mcError += par[0]*monteCarloMETHistograms["MET_muon_QCD_Mu"]->GetBinError(bin);
      mcError += par[0]*monteCarloMETHistograms["MET_muon_WW"]->GetBinError(bin);//TEMP
      mcError += par[1]*monteCarloMETHistograms["MET_muon_WJets"]->GetBinError(bin);
      
      data += dataMETHistogram->GetBinContent(bin);
   
      for(map<string,TH1D*>::iterator it = monteCarloMETHistograms.begin(); it != monteCarloMETHistograms.end(); it++)
      {
         //if( ((*it).first != "MET_muon_QCD_Mu") || ((*it).first != "MET_muon_WJets") )
         if( ((*it).first != "MET_muon_WW") || ((*it).first != "MET_muon_WJets") )//TEMP
         {
            mc += (*it).second->GetBinContent(bin);
            mcError += (*it).second->GetBinError(bin);
         }
      }
      
      if(mcError == 0)
      {
         cout << endl << "##### WARNING: mcError == 0 #####" << endl;
         continue;
      }
      
      double delta = (data - mc) / mcError;
      
      chiSquare += delta*delta;
   }
   
   return chiSquare;
}

double HistogramFit::fitFuncWmT(const double *par)
{
   double chiSquare = 0;
   
   for(int bin = 1; bin <= dataWmTHistogram->GetNbinsX(); bin++)
   {
      double mc = 0;
      //double mcError = 0;
      double mcError = 1;//TEMP
      double data = 0;
      
      //mc += par[0]*monteCarloWmTHistograms["WmT_muon_QCD_Mu"]->GetBinContent(bin);
      mc += par[0]*monteCarloWmTHistograms["WmT_muon_WW"]->GetBinContent(bin);//TEMP
      mc += par[1]*monteCarloWmTHistograms["WmT_muon_WJets"]->GetBinContent(bin);
      
      //mcError += par[0]*monteCarloWmTHistograms["WmT_muon_QCD_Mu"]->GetBinError(bin);
      mcError += par[0]*monteCarloWmTHistograms["WmT_muon_WW"]->GetBinError(bin);//TEMP
      mcError += par[1]*monteCarloWmTHistograms["WmT_muon_WJets"]->GetBinError(bin);
      
      data += dataWmTHistogram->GetBinContent(bin);
   
      for(map<string,TH1D*>::iterator it = monteCarloWmTHistograms.begin(); it != monteCarloWmTHistograms.end(); it++)
      {
         //if( ((*it).first != "WmT_muon_QCD_Mu") || ((*it).first != "WmT_muon_WJets") )
         if( ((*it).first != "WmT_muon_WW") || ((*it).first != "WmT_muon_WJets") )//TEMP
         {
            mc += (*it).second->GetBinContent(bin);
            mcError += (*it).second->GetBinError(bin);
         }
      }
      
      if(mcError == 0)
      {
         cout << endl << "##### WARNING: mcError == 0 #####" << endl;
         continue;
      }
      
      double delta = (data - mc) / mcError;
      
      chiSquare += delta*delta;
   }
   
   return chiSquare;
}

int main()
{
   return HistFit_x();
}
