//Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//ROOT Libraries
#include <TStyle.h>
#include "TDirectory.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TMath.h"
#include "TGraph.h"

//C++ libraries
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
 
  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         basepath          = cl.getValue<TString> ("basepath",          "/uscms_data/d2/aperloff/Summer12ME8TeV/2014_03_21_ValidationPlots_PU_QCDEta_Scale_CSV_TTbar/signal_optimizedEPD/QCD_ElFULL/");
  TString         histogramFileBase = cl.getValue<TString> ("histogramFileBase", "histos");
  TString         channel           = cl.getValue<TString> ("channel",           "Mlvjj");
  TString         lepCat            = cl.getValue<TString> ("lepton",            "both");
  DEFS::LeptonCat leptonCat         = DEFS::getLeptonCat(string(lepCat));
  string          jetBinS           = cl.getValue<string>  ("jetBin",            "jets2");
  DEFS::JetBin    jetBin            = DEFS::getJetBin(jetBinS);
  string          tagcatS           = cl.getValue<string>  ("tagcat",            "pretag");
  DEFS::TagCat    tagcat            = DEFS::getTagCat(tagcatS);
  string          anaTypeS          = cl.getValue<string>  ("anaType",           "HiggsAnalysis");
  DEFS::Ana::Type anaType           = DEFS::Ana::getAnaType(anaTypeS);
  bool            debug             = cl.getValue<bool>    ("debug",             false);

  if (!cl.check()) return 0;
  cl.print();

  TString histogramFile = histogramFileBase+"_"+lepCat+".root";

  // Get the input processes
  vector<PhysicsProcess*> processes = DefaultValues::getProcessesHiggs(jetBin, tagcat, false, false, DEFS::MicroNtuple);
  if(debug) {
     processes.erase(processes.begin(),processes.begin()+11);
  }

  // Name the bin for the processes
  vector <TString> vecSig;

  // Name the bin for the processes
  vector <TString> vecBak;

  for(unsigned int iprocess=0; iprocess<processes.size(); iprocess++) {

    TString processName = processes[iprocess]->name;
    //Skip Data
    if(DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::SingleEl_Data) {
       continue;
    }
    else if(DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::SingleMu_Data) {
       continue;
    }

    if(leptonCat==DEFS::electron && DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::QCD_MuFULL)
      continue;
    if(leptonCat==DEFS::muon && DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::QCD_ElFULL)
      continue;


    if ((anaType == DEFS::Ana::WWAnalysis && (processName.EqualTo("WW") || processName.EqualTo("WZ"))) || 
        (anaType == DEFS::Ana::HiggsAnalysis && (processName.Contains("WH") || processName.Contains("ggH") || processName.Contains("qqH"))) ||
        (anaType == DEFS::Ana::UNKNOWN && (processName.Contains("WH") || processName.Contains("ggH") || processName.Contains("qqH")))){

      // Input each signal
      vecSig.push_back(channel+"_"+processName+"_"+lepCat);
      
      cout<<"\tputting process "<<processName<<" into signal."<<endl;
    
    }
    else{ 
      // Input each background
      vecBak.push_back(channel+"_"+processName+"_"+lepCat);

      cout<<"\tputting process "<<processName<<" into background."<<endl;  
    }
  }

  TFile* in_file = TFile::Open(basepath+histogramFile,"READ"); // Input the root file filled with histograms

  // Creating the datacard
  ofstream outFile(string("histos_")+string(channel)+"_"+string(lepCat)+string("_datacard.txt"));

  // process counter line
  int jmax = vecSig.size()+vecBak.size();

  // Input total number of bins
  outFile << "imax " << "1" << endl;
  
  // Input total number of processes in bin1 minus one
  outFile << "jmax " << jmax - 1 << endl; 
  
  // Input total number of systematics
  outFile << "kmax " << "*" << endl;
  outFile << "------------------------" << endl;
  
  // Input the root file name and the data_obs file name
  TH1D* proc = 0;
  if(leptonCat==DEFS::electron) {
    outFile << "shapes data_obs * " << basepath << histogramFile << " " << channel << "_SingleEl_Data_" << lepCat << endl;
    proc = (TH1D*) in_file->Get(channel+"_SingleEl_Data_"+lepCat);
  }
  else if(leptonCat==DEFS::muon) {
    outFile << "shapes data_obs * " << basepath << histogramFile << " " << channel << "_SingleMu_Data_" << lepCat << endl;
    proc = (TH1D*) in_file->Get(channel+"_SingleMu_Data_"+lepCat);
  }

  // Input the root file name and whatever edits to the names you wish to make
  outFile << "shapes * * " << basepath << histogramFile << " $PROCESS" << endl;
  
  outFile << "------------------------" << endl;
  
  // Input the name of your bin
  outFile << "bin " << "channel1" << endl; 
  
  outFile << "observation " << std::setprecision(8) << proc->Integral() << endl;
  
  outFile << "------------------------" << endl;
  
  outFile << "bin "; 

  //Bin line
  for (unsigned int num_bins = 0 ; num_bins < (vecSig.size()+vecBak.size()); num_bins++){
    
    outFile << "channel1 ";
    
  } //for bin line
  
  outFile << endl;
  
  outFile << "process ";

  // signal process name line
  for (unsigned int name_sig = 0 ; name_sig < vecSig.size(); name_sig++){
    
    TH1D* sig_name = (TH1D*) in_file->Get(vecSig[name_sig]);
    
    outFile << sig_name->GetName() << " ";

  } // for sig process name line

  // background process name line
  for (unsigned int name_bak = 0 ; name_bak < vecBak.size(); name_bak++){
    
    TH1D* bak_name = (TH1D*) in_file->Get(vecBak[name_bak]);
    
    outFile << bak_name->GetName() << " ";

  } // for bak process name line
  
  outFile << endl;
  
  outFile << "process ";
  
  // Making an array to output the signal process IDs
  for (unsigned int sig_num = 0 ; sig_num < vecSig.size(); sig_num++){ 

    // Output a negative number (or zero) for each signal process
    outFile << -int(sig_num) << " ";

  } // for sig num ID

  // Making an array to output the background process IDs
  for (unsigned int bak_num = 0; bak_num < vecBak.size(); bak_num++){

    // Output a positive number for each background process
    outFile << bak_num+1 << " ";

  } // for bak num ID

  outFile << endl;
  
  outFile << "rate ";
  
  // Making array to create integral data
  for (unsigned int r_sig = 0 ; r_sig < vecSig.size(); r_sig++){
    
    // Get the histograms out
    TH1D* sig = (TH1D*) in_file->Get(vecSig[r_sig]);  
    
    // Integrate each histogram
    double data_store = sig->Integral(); 
    
    // Print signal integral data
    outFile << std::setprecision(8) << data_store << " "; 
    
  } // for signal integral loop
  
    // Creating array for background rates
  for (unsigned int r_bak = 0 ; r_bak < vecBak.size(); r_bak++){
    
    // Get the histograms out
   TH1D* bak = (TH1D*) in_file->Get(vecBak[r_bak]);  
    
    // Integrate each histogram
    double data_store = bak->Integral(); 
    
    // Print signal integral data
    outFile << std::setprecision(8) << data_store << " "; 
    
  } //for background integral loop

  outFile << endl;
  
  outFile << "------------------------" << endl;

  // adding luminosity systematic
  outFile << "lumi lnN ";

  // adding a counter for luminosity, constant across all processes
  for (unsigned int n_lumi = 0 ; n_lumi < (vecBak.size() + vecSig.size()); n_lumi++){

    outFile << "1.045 ";

  } //for luminosity loop

  outFile << endl;

  outFile.close();
  
} //LimitsDatacard_ggH125
