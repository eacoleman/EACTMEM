// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales WJets and QCD.
// 
// Written by Travis Lamb
// Started on the 14th of July, 2012

// TAMU Libraries
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"

using namespace std;
using namespace ROOT::Minuit2;

//##################################################
//################ PUBLIC FUNCTIONS ################
//##################################################

BackgroundEstimator::BackgroundEstimator(string lepton, string object, string inFileLoc, string outFileLoc) :
minFit(0), funcFit(0)
{
   leptonName = lepton;
   objectName = object;
   
   initializeFileLocations(inFileLoc, outFileLoc);
   initializeHistNames();
   
   resultStack = new THStack((objectName + "_MonteCarlo").c_str(), (objectName + "_MonteCarlo").c_str());
   
   // The default values (before fitting) of Chi2 and parameters
   scaleParameters.first = 1.0;
   scaleParameters.second = 1.0;
   reducedChiSquared = 0.0;
   
   // Set default debug options
   rebinSizeDEBUG = 1;
   debug = false;
}

BackgroundEstimator::~BackgroundEstimator()
{
   // Destroy montecarlo histograms
   for(map<string, TH1D*>::iterator mapIt = monteCarloHistograms.begin(); mapIt != monteCarloHistograms.begin(); mapIt++)
   {
      mapIt->second->~TH1D();
      delete mapIt->second;
   }
   
   monteCarloHistograms.clear();
   
   // Destroy data histogram
   dataHistogram->~TH1D();
   delete dataHistogram;
}

void BackgroundEstimator::readHistograms()
{
   TFile rootInFile;
   rootInFile.Open(inRootFileLocation.c_str(), "READ");
   
   string prefix = objectName + "_" + leptonName;
   
   TCanvas* testCanCopy = (TCanvas*)gDirectory->Get(prefix.c_str());
   canvas = (TCanvas*)testCanCopy->Clone();
   
   pad = (TPad *) canvas->GetPrimitive((prefix + "_1").c_str());
   mcStack = (THStack*) pad->GetPrimitive((prefix + "_stackMC").c_str());
   dataStack = (THStack*) pad->GetPrimitive((prefix + "_stackData").c_str());
   mcList = mcStack->GetHists();
   dataList = dataStack->GetHists();
   
   // MC Histograms
   for(unsigned int i = 0; i < histNames.size(); i++)
   {
      string name = histNames[i];
      
      if(name == "QCD")
      {
         if(leptonName == "electron")
            monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_ElEnriched").c_str());
         else
            monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_MuEnriched").c_str());
      }
      else
         monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name).c_str());
      
      if (debug)
         monteCarloHistograms[name]->Rebin(rebinSizeDEBUG);
   }
   
   // Data histogram
   if(leptonName == "electron")
      dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data (el)").c_str());
   else
      dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data (mu)").c_str());
   if (debug)
      dataHistogram->Rebin(rebinSizeDEBUG);
   
   rootInFile.Close();
}

void BackgroundEstimator::fitHistograms()
{
   const double* parameters = fitAndReturnParameters();
   
   monteCarloHistograms["QCD"]->Scale(parameters[0]);
   monteCarloHistograms["WJets"]->Scale(parameters[1]);
   
   mcStack->Modified();
}

void BackgroundEstimator::writeHistograms()
{
   if (outRootFileLocation == "")
   {
      cout << "ERROR: writeLocation has not been set. Cannot write Histograms." << endl;
      return;
   }
   
   TFile rootInFile;
   rootInFile.Open(inRootFileLocation.c_str());
   
   vector<string> plotPrefixes = getPlotNames();
   
   vector<TCanvas*> inputCanvases;
   
   TCanvas* inputCanvas;
   TPad* inputPad;
   THStack* inputMCStack;
   TList* inputMCList;
   TH1D* histQCD;
   TH1D* histWJets;
   
   for(unsigned int i = 0; i < plotPrefixes.size(); i++)
   {
      string prefix = plotPrefixes[i] + "_" + leptonName;
      
      inputCanvas = (TCanvas*)(gDirectory->Get((prefix).c_str()))->Clone();
      inputPad = (TPad *) inputCanvas->GetPrimitive((prefix + "_1").c_str());
      inputMCStack = (THStack*) inputPad->GetPrimitive((prefix + "_stackMC").c_str());
      inputMCList = inputMCStack->GetHists();
      
      if(leptonName == "electron")
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD_ElEnriched").c_str());
      else if (leptonName == "muon")
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD_MuEnriched").c_str());
      else
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD").c_str());
      histWJets = (TH1D*)inputMCList->FindObject((prefix + "_WJets").c_str());
      
      histQCD->Scale(scaleParameters.first);
      histWJets->Scale(scaleParameters.second);
      
      inputMCStack->Modified();
      
      inputCanvases.push_back(inputCanvas);
   }
   
   rootInFile.Close();
   
   TFile rootOutFile;
   rootOutFile.Open(outRootFileLocation.c_str(), "RECREATE");
   
   //canvas->Write();
   
   for(unsigned int i = 0; i < inputCanvases.size(); i++)
   {
      inputCanvases[i]->Write();
   }
   
   rootOutFile.Close();
}

void BackgroundEstimator::setWriteLocation(string outFileLoc)
{
   outRootFileLocation = outFileLoc;
}

void BackgroundEstimator::setReadLocation(string inFileLoc)
{
   inRootFileLocation = inFileLoc;
}

void BackgroundEstimator::setRebinSizeDEBUG(unsigned int rebinSize)
{
   debug = true;
   rebinSizeDEBUG = rebinSize;
}

pair<double, double> BackgroundEstimator::getParameters()
{
   return scaleParameters;
}

double BackgroundEstimator::getReducedChiSquared()
{
   return reducedChiSquared;
}

//##################################################
//############### PRIVATE FUNCTIONS ################
//##################################################

void BackgroundEstimator::initializeFileLocations(string inFileLoc, string outFileLoc)
{
   inRootFileLocation = inFileLoc;
   outRootFileLocation = outFileLoc;
}

void BackgroundEstimator::initializeHistNames()
{
   histNames.push_back("Diboson");
   histNames.push_back("STop");
   histNames.push_back("WJets");
   histNames.push_back("DYJets");
   histNames.push_back("TTbar");
   histNames.push_back("QCD");
   histNames.push_back("ggH125");
}

const double* BackgroundEstimator::fitAndReturnParameters()
{
   funcFit = new ROOT::Math::Functor(this,&BackgroundEstimator::fitFunc,2);
   minFit = new ROOT::Minuit2::Minuit2Minimizer( ROOT::Minuit2::kMigrad );
   
   // Tolerance and printouts
   minFit->SetPrintLevel(3);
   minFit->SetStrategy(2);
   minFit->SetMaxFunctionCalls(8000);
   minFit->SetMaxIterations(8000);
   minFit->SetTolerance(0.1);
   minFit->SetErrorDef(0.5);
   minFit->SetValidError(true);
   
   // Fitting
   minFit->SetFunction(*funcFit);
   
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
   
   reducedChiSquared = minFit->MinValue() / (dataHistogram->GetNbinsX() - 2);
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << dataHistogram->GetNbinsX() - 2 << endl;
   cout << "Chi2/NDF\t  = " << reducedChiSquared << endl;
   
   scaleParameters.first = minFit->X()[0];
   scaleParameters.second = minFit->X()[1];
   
   return minFit->X();
}

vector<string> BackgroundEstimator::getPlotNames()
{
   vector<string> names;
   
   names.push_back("AngleJ1J2");
   names.push_back("BetaJ1BetaJ2");
   names.push_back("DeltaEtaJ1J2");
   names.push_back("DeltaPhi_J1J2");
   names.push_back("DeltaPhi_LJ1");
   names.push_back("DeltaPhi_LJ1_vs_J1J2");
   names.push_back("DeltaRJ1J2");
   names.push_back("DeltaRLepMET");
   names.push_back("EJ1EJ2");
   names.push_back("Jet1Eta");
   names.push_back("Jet1Phi");
   names.push_back("Jet1Pt");
   names.push_back("Jet2Eta");
   names.push_back("Jet2Phi");
   names.push_back("Jet2Pt");
   names.push_back("LeptEta");
   names.push_back("LeptPhi");
   names.push_back("LeptPt");
   names.push_back("MET");
   names.push_back("Mjj");
   names.push_back("MjjmWmT");
   names.push_back("Mlvjj");
   names.push_back("Ptjj");
   names.push_back("WmT");
   names.push_back("j1Pt_Mjj");
   names.push_back("j2Pt_Mjj");
   names.push_back("jjlvPhi");
   names.push_back("npv");
   
   return names;
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
      double mcError2 = 0;
      double data = 0;
      double dataError2 = 0;
      
      mc += par[0]*monteCarloHistograms["QCD"]->GetBinContent(bin);
      mc += par[1]*monteCarloHistograms["WJets"]->GetBinContent(bin);
      
      mcError2 += pow(par[0]*monteCarloHistograms["QCD"]->GetBinError(bin), 2);
      mcError2 += pow(par[1]*monteCarloHistograms["WJets"]->GetBinError(bin), 2);
      
      data += dataHistogram->GetBinContent(bin);
      dataError2 += pow(dataHistogram->GetBinError(bin), 2);
   
      for(map<string, TH1D*>::iterator it = monteCarloHistograms.begin(); it != monteCarloHistograms.end(); it++)
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
