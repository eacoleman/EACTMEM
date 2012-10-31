// This Program Takes input Monte Carlo histograms of several processes
// and fits them to a data histogram. It only scales two processes.
// 
// Written by Alexx Perloff
// Started on the 16th of October, 2012

// TAMU Libraries
#include "TAMUWW/Tools/interface/Fitter.hh"

using namespace std;
using namespace ROOT::Minuit2;

//##################################################
//################ PUBLIC FUNCTIONS ################
//##################################################

Fitter::Fitter(string lepton, string object, vector<string> fproc, string inFileLoc, string outFileLoc)
{
   leptonName = lepton;
   objectName = object;
   HistogramsFitter::fProcessNames = fproc;
   
   initializeFileLocations(inFileLoc, outFileLoc);
   initializeHistNames();
   
   resultStack = new THStack((objectName + "_MonteCarlo").c_str(), (objectName + "_MonteCarlo").c_str());
   
   // The default values (before fitting) of Chi2 and parameters
   //scaleParameters.first = 1.0;
   //scaleParameters.second = 1.0;
   scaleParameters = vector<double> (2,1.0);
   reducedChiSquared = 0.0;
   
   // Set default debug options
   rebinSizeDEBUG = 1;
   debug = false;
}

Fitter::~Fitter()
{
   // Destroy montecarlo histograms
   for(map<string, TH1D*>::iterator mapIt = HistogramsFitter::monteCarloHistograms.begin(); mapIt != HistogramsFitter::monteCarloHistograms.begin(); mapIt++)
   {
      mapIt->second->~TH1D();
      delete mapIt->second;
   }

   HistogramsFitter::fProcessNames.clear();
   HistogramsFitter::monteCarloHistograms.clear();

   // Destroy data histogram
   HistogramsFitter::dataHistogram->~TH1D();
   delete HistogramsFitter::dataHistogram;

   // Destroy other histograms
   HistogramsFitter::signalHistogram->~TH1D();
   delete HistogramsFitter::signalHistogram;
   HistogramsFitter::backgroundHistogram->~TH1D();
   delete HistogramsFitter::backgroundHistogram;
}

void Fitter::readHistograms()
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
            HistogramsFitter::monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_electronEnriched").c_str());
         else
            HistogramsFitter::monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_muonEnriched").c_str());
      }
      else if(name == "WJets")
      {
         if(leptonName == "electron")
            HistogramsFitter::monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_electron").c_str());
         else
            HistogramsFitter::monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name + "_muon").c_str());
      }
      else
         HistogramsFitter::monteCarloHistograms[name] = (TH1D*) mcList->FindObject((prefix + "_" + name).c_str());
      
      if (debug)
         HistogramsFitter::monteCarloHistograms[name]->Rebin(rebinSizeDEBUG);
   }
   
   // Data histogram
   if(leptonName == "electron")
      //HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data (el)").c_str());
      //HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_SingleEl_Data").c_str());
      HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data_electron").c_str());
   else
      //HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data (mu)").c_str());
      //HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_SingleMu_Data").c_str());
      HistogramsFitter::dataHistogram = (TH1D*) dataList->FindObject((prefix + "_Data_muon").c_str());
   if (debug)
      HistogramsFitter::dataHistogram->Rebin(rebinSizeDEBUG);
   
   rootInFile.Close();
}

void Fitter::fitHistograms()
{
   const double* parameters = fitAndReturnParameters();
   
   for (unsigned int i=0; i<HistogramsFitter::fProcessNames.size(); i++) {
      HistogramsFitter::monteCarloHistograms[HistogramsFitter::fProcessNames[i]]->Scale(parameters[i]);
   }
   
   mcStack->Modified();
}

void Fitter::writeHistograms()
{
   if (outRootFileLocation == "")
   {
      cout << "ERROR: writeLocation has not been set. Cannot write Histograms." << endl;
      return;
   }

   TFile* rootInFile = TFile::Open(inRootFileLocation.c_str());
   if (!rootInFile->IsOpen()) {  cout<<"Can't open "<<inRootFileLocation<<endl; return; }
   TIter next(rootInFile->GetListOfKeys());
   TKey* key(0);
   //vector<string> plotPrefixes = getPlotNames();
   
   vector<TCanvas*> inputCanvases;
   
   TCanvas* inputCanvas;
   TPad* inputPad;
   THStack* inputMCStack;
   TList* inputMCList;
   //TH1D* histQCD;
   //TH1D* histWJets;
   vector<TH1D*> histsToScale;
   
   cout << "\tFitter::writeHistograms Scaling input histograms...";
   //for(unsigned int i = 0; i < plotPrefixes.size(); i++) {
   while ((key=(TKey*)next())) {
      if (strcmp(key->GetClassName(),"TCanvas")!=0) continue;

      //string prefix = plotPrefixes[i] + "_" + leptonName;
      string prefix = key->GetName();

      //inputCanvas = (TCanvas*)(key->ReadObj())->Clone();
      inputCanvas = (TCanvas*)(gDirectory->Get((prefix).c_str()))->Clone();
      inputPad = (TPad *) inputCanvas->GetPrimitive((prefix + "_1").c_str());
      inputMCStack = (THStack*) inputPad->GetPrimitive((prefix + "_stackMC").c_str());
      inputMCList = inputMCStack->GetHists();

      for (unsigned int nproc=0; nproc<HistogramsFitter::fProcessNames.size(); nproc++) {
         if(HistogramsFitter::fProcessNames[nproc] == "QCD") {
            if(leptonName == "electron")
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_QCD_electronEnriched").c_str()));
            else if (leptonName == "muon")
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_QCD_muonEnriched").c_str()));
            else
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_QCD").c_str()));
         }
         else if(HistogramsFitter::fProcessNames[nproc] == "WJets") {
           if(leptonName == "electron")
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_WJets_electron").c_str()));
            else if (leptonName == "muon")
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_WJets_muon").c_str()));
            else
               histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_WJets").c_str()));
         }
         else histsToScale.push_back((TH1D*)inputMCList->FindObject((prefix + "_" + HistogramsFitter::fProcessNames[nproc]).c_str()));

         histsToScale[nproc]->Scale(scaleParameters[nproc]);
      }
/*
      if(leptonName == "electron") {
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD_ElEnriched").c_str());
         histWJets = (TH1D*)inputMCList->FindObject((prefix + "_WJets_electron").c_str());
      }
      else if (leptonName == "muon") {
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD_MuEnriched").c_str());
         histWJets = (TH1D*)inputMCList->FindObject((prefix + "_WJets_muon").c_str());
      }
      else {
         histQCD = (TH1D*)inputMCList->FindObject((prefix + "_QCD").c_str());
         histWJets = (TH1D*)inputMCList->FindObject((prefix + "_WJets").c_str());
      }

      histQCD->Scale(scaleParameters.first);
      histWJets->Scale(scaleParameters.second);
*/

      inputMCStack->Modified();
      
      inputCanvases.push_back(inputCanvas);
   }
   rootInFile->Close();  
   cout << "DONE" << endl; 

   cout << "\tFitter::writeHistograms Writing output histograms...";
   TFile rootOutFile;
   rootOutFile.Open(outRootFileLocation.c_str(), "RECREATE");
   
   //canvas->Write();
   
   for(unsigned int i = 0; i < inputCanvases.size(); i++)
   {
      inputCanvases[i]->Write();
   }
   HistogramsFitter::signalHistogram->Write();
   HistogramsFitter::backgroundHistogram->Write();

   
   rootOutFile.Close();
   cout << "DONE" << endl;
}

void Fitter::addSigBkgHistograms(vector<string> sig) {
   HistogramsFitter::signalHistogram = new TH1D("signalHistogram","signalHistograms",
                                                HistogramsFitter::monteCarloHistograms.begin()->second->GetNbinsX(),
                                                HistogramsFitter::monteCarloHistograms.begin()->second->GetXaxis()->GetXmin(),
                                                HistogramsFitter::monteCarloHistograms.begin()->second->GetXaxis()->GetXmax());
   HistogramsFitter::signalHistogram->Sumw2();
   HistogramsFitter::backgroundHistogram = new TH1D("backgroundHistogram","backgroundHistogram",
                                                    HistogramsFitter::monteCarloHistograms.begin()->second->GetNbinsX(),
                                                    HistogramsFitter::monteCarloHistograms.begin()->second->GetXaxis()->GetXmin(),
                                                    HistogramsFitter::monteCarloHistograms.begin()->second->GetXaxis()->GetXmax());
   HistogramsFitter::backgroundHistogram->Sumw2();

   for(map<string,TH1D*>::iterator it=HistogramsFitter::monteCarloHistograms.begin(); it!=HistogramsFitter::monteCarloHistograms.end(); it++) {
/*
      for(unsigned int nsig=0; nsig<sig.size(); nsig++) {
         if ((*it).first=="WJets") {
            if (leptonName=="muon") {
               if (objectName=="Mjj") {
                  (*it).second->Scale(0.984361);
               }
               else if (objectName=="epdPretagWWandWZ") {
                  (*it).second->Scale(0.971676);
               }
               else if (objectName=="MVADiscriminator") {
                  (*it).second->Scale(0.975234);
               }
            }
            else if (leptonName=="electron") {
               if (objectName=="Mjj") {
                  (*it).second->Scale(1.0096);
               }
               else if (objectName=="epdPretagWWandWZ") {
                  (*it).second->Scale(0.945736);
               }
               else if (objectName=="MVADiscriminator") {
                  (*it).second->Scale(0.990551);
               }
            }
         }

         if(sig[nsig] == (*it).first) {
            HistogramsFitter::signalHistogram->Add((*it).second);
            cout << "Fitter::Added the " << (*it).first << " histogram to the signalHistogram." << endl;
            cout << "\t" << setw(26) << (*it).first << " Entries: " << (*it).second->GetEntries() << endl;
            cout << "\t" << setw(26) << "signalHistogram Entries: " << HistogramsFitter::signalHistogram->GetEntries() << endl;
         }
         else {
            HistogramsFitter::backgroundHistogram->Add((*it).second);
            cout << "Fitter::Added the " << (*it).first << " histogram to the backgroundHistogram." << endl;
            cout << "\t" << setw(26) << (*it).first << " Entries: " << (*it).second->GetEntries() << endl;
            cout << "\t" << setw(26) << "backgroundHistogram Entries: " << HistogramsFitter::backgroundHistogram->GetEntries() << endl; 
         }

      }
*/
      if (vfind(sig,(*it).first)!=-1) {
         HistogramsFitter::signalHistogram->Add((*it).second);
         cout << "Fitter::Added the " << (*it).first << " histogram to the signalHistogram." << endl;
         cout << "\t" << setw(26) << (*it).first << " Entries: " << (*it).second->GetEntries() << endl;
         cout << "\t" << setw(26) << "signalHistogram Entries: " << HistogramsFitter::signalHistogram->GetEntries() << endl;
      }
      else {
         HistogramsFitter::backgroundHistogram->Add((*it).second);
         cout << "Fitter::Added the " << (*it).first << " histogram to the backgroundHistogram." << endl;
         cout << "\t" << setw(26) << (*it).first << " Entries: " << (*it).second->GetEntries() << endl;
         cout << "\t" << setw(26) << "backgroundHistogram Entries: " << HistogramsFitter::backgroundHistogram->GetEntries() << endl; 
      }
         
   }
}

void Fitter::setWriteLocation(string outFileLoc)
{
   outRootFileLocation = outFileLoc;
}

void Fitter::setReadLocation(string inFileLoc)
{
   inRootFileLocation = inFileLoc;
}

void Fitter::setRebinSizeDEBUG(unsigned int rebinSize)
{
   debug = true;
   rebinSizeDEBUG = rebinSize;
}

//pair<double, double> Fitter::getParameters()
vector<double> Fitter::getParameters()
{
   return scaleParameters;
}

double Fitter::getReducedChiSquared()
{
   return reducedChiSquared;
}

double Fitter::getFOM(double FOM) {
   if(HistogramsFitter::signalHistogram->GetEntries()==0 || HistogramsFitter::backgroundHistogram->GetEntries()==0) {
      cout << "Fitter::WARNING Either the signal or background histogram has zero entries!" << endl;
      cout << "\t" << setw(22) << "Signal Entries: " << HistogramsFitter::signalHistogram->GetEntries() << endl;    
      cout << "\t" << setw(22) << "Background Entries: " << HistogramsFitter::backgroundHistogram->GetEntries() << endl;
   }

   // set signal bins to zero if background bin is zero
   // set background bins to zero if signal bin is zero
   for(int i=1; i<=HistogramsFitter::backgroundHistogram->GetNbinsX(); i++) {
      if (HistogramsFitter::signalHistogram->GetBinContent(i)>0 && HistogramsFitter::backgroundHistogram->GetBinContent(i)==0) {
         HistogramsFitter::signalHistogram->SetBinContent(i,0.0);
         HistogramsFitter::signalHistogram->SetBinError(i,0.0);
      }
//      else if (HistogramsFitter::signalHistogram->GetBinContent(i)==0 && HistogramsFitter::backgroundHistogram->GetBinContent(i)>0) {
//         HistogramsFitter::backgroundHistogram->SetBinContent(i,0.0);
//         HistogramsFitter::backgroundHistogram->SetBinError(i,0.0);
//      }
   }

   if (FOM==1) {
      double fom1 = 0.0;
      for(int i=1; i<=HistogramsFitter::backgroundHistogram->GetNbinsX(); i++)
         fom1 += TMath::Power((HistogramsFitter::signalHistogram->GetBinContent(i)/TMath::Sqrt(HistogramsFitter::backgroundHistogram->GetBinContent(i))),2);
      return TMath::Sqrt(fom1);
   }
   else if (FOM==2) {
      double fom2 = 0.0;
      for(int i=1; i<=HistogramsFitter::backgroundHistogram->GetNbinsX(); i++)
         fom2 += TMath::Power((HistogramsFitter::signalHistogram->GetBinContent(i)/TMath::Sqrt(HistogramsFitter::signalHistogram->GetBinContent(i)+HistogramsFitter::backgroundHistogram->GetBinContent(i))),2);
      return TMath::Sqrt(fom2);
   }
   else if (FOM==2.5)
      return FigureOfMerit::usingChi2(HistogramsFitter::signalHistogram,
                                      HistogramsFitter::backgroundHistogram,0.0001);
   else if (FOM==3)
      return FigureOfMerit::usingShapeFromTemplates(HistogramsFitter::signalHistogram,
                                                    HistogramsFitter::backgroundHistogram,0.0001);
   else {
      cout << "Fitter::WARNING The FOM type was not specified. Using defaul (FOM3)." << endl;
      return FigureOfMerit::usingShapeFromTemplates(HistogramsFitter::signalHistogram,
                                                    HistogramsFitter::backgroundHistogram,0.0001);
   }
}

int Fitter::vfind(vector<string> a, string b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (TString(a[i]).CompareTo(TString(b))==0)
         return i;
   }
   return -1;
}

//##################################################
//############### PRIVATE FUNCTIONS ################
//##################################################

void Fitter::initializeFileLocations(string inFileLoc, string outFileLoc)
{
   inRootFileLocation = inFileLoc;
   outRootFileLocation = outFileLoc;
}

void Fitter::initializeHistNames()
{
   histNames.push_back("Diboson");
   histNames.push_back("STop");
   histNames.push_back("WJets");
   histNames.push_back("DYJets");
   histNames.push_back("TTbar");
   histNames.push_back("QCD");
   //histNames.push_back("ggH125");
}

const double* Fitter::fitAndReturnParameters()
{
   ROOT::Math::Functor funcFit(&fitFunc,2);
   Minuit2Minimizer* minFit = new Minuit2Minimizer(kMigrad);
   
   // Tolerance and printouts
   minFit->SetPrintLevel(3);
   minFit->SetStrategy(2);
   minFit->SetMaxFunctionCalls(10000);
   minFit->SetMaxIterations(10000);
   minFit->SetTolerance(0.1);
   minFit->SetErrorDef(0.5);
   minFit->SetValidError(true);
   
   // Fitting
   minFit->SetFunction(funcFit);
   
   double parameter[2];
   if(vfind(HistogramsFitter::fProcessNames,"QCD")>=0) {
      parameter[0] = 1.0;
      parameter[1] = 1.0;
   }
   else {
      parameter[0] = 1.0;
      parameter[1] =  0.00001;
   }
   double step[2] = {0.001, 0.001};
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
   
   reducedChiSquared = minFit->MinValue() / (HistogramsFitter::dataHistogram->GetNbinsX() - 2);
   
   cout << "Chi2\t  = " << minFit->MinValue() << endl;
   cout << "NDF\t  = " << HistogramsFitter::dataHistogram->GetNbinsX() - 2 << endl;
   cout << "Chi2/NDF\t  = " << reducedChiSquared << endl;
   
   //scaleParameters.first = minFit->X()[0];
   //scaleParameters.second = minFit->X()[1];
   scaleParameters[0] = minFit->X()[0];
   scaleParameters[1] = minFit->X()[1];
   
   return minFit->X();
}

vector<string> Fitter::getPlotNames()
{
   vector<string> names;
   
   names.push_back("AngleJ1J2");
   names.push_back("BetaJ1BetaJ2");
   names.push_back("DeltaEtaJ1J2");
   names.push_back("DeltaPhi_J1J2");
   names.push_back("DeltaPhi_LJ1");
   names.push_back("DeltaPhi_LJ1_vs_J1J2");
   //names.push_back("DeltaPhi_LJ1_vs_J1J2_Positive");
   //names.push_back("DeltaPhi_LJ1_vs_J1J2_Negative");
   //names.push_back("DeltaPhi_LJ1_vs_J1J2_Subtracted");
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
   names.push_back("MWjjVsMWlv");
   names.push_back("Ptjj");
   names.push_back("WmT");
   //names.push_back("WmT_Positive");
   //names.push_back("WmT_Negative");
   //names.push_back("WmT_Subtracted");
   names.push_back("j1Pt_Mjj");
   names.push_back("j2Pt_Mjj");
   names.push_back("jjlvPhi");
   //names.push_back("npv");
   
   return names;
}

//##################################################
//################ STATIC FUNCTIONS ################
//##################################################

double Fitter::fitFunc(const double *par)
{
   HistogramsFitter::dataHistogram->GetNbinsX();
   double chiSquare = 0;

   for(int bin = 1; bin <= HistogramsFitter::dataHistogram->GetNbinsX(); bin++)
   {
      double mc = 0;
      double mcError2 = 0;
      double data = 0;
      double dataError2 = 0;

      for (unsigned int nproc=0; nproc<HistogramsFitter::fProcessNames.size(); nproc++) {
         mc += par[nproc]*HistogramsFitter::monteCarloHistograms[HistogramsFitter::fProcessNames[nproc]]->GetBinContent(bin);
         mcError2 += pow(par[nproc]*HistogramsFitter::monteCarloHistograms[HistogramsFitter::fProcessNames[nproc]]->GetBinError(bin), 2);
      }

      data += HistogramsFitter::dataHistogram->GetBinContent(bin);
      dataError2 += pow(HistogramsFitter::dataHistogram->GetBinError(bin), 2);

      for(map<string, TH1D*>::iterator it = HistogramsFitter::monteCarloHistograms.begin(); it != HistogramsFitter::monteCarloHistograms.end(); it++)
      {
         bool notFProc = true;
         for (unsigned int nproc=0; nproc<HistogramsFitter::fProcessNames.size(); nproc++) {
            if ((*it).first == HistogramsFitter::fProcessNames[nproc])
               notFProc = false;
         }
         if(notFProc)
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
