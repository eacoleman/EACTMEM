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
#include <sstream>

void read_from_file_3_new(){

  TFile* in_file = TFile::Open("histos_electron.root"); // Input the root file filled with histograms

  string channel1 = "Mlvjj_";

  // Name the bin for the processes
  vector <TString> vecSig;

  // Name the bin for the processes
  vector <TString> vecBak;

  in_file >> int_var >> double_var;

  //  for (string = channel1* ; string in in_file ;) {

  //for (hstring = "*H*" ; hstring in in_file ;) {

      vecSig.push_back(int_var);
  // Input each signal
  //  vecSig.push_back("ggH125_electron"); 
  //vecSig.push_back("Mlvjj_qqH125_electron");
  //vecSig.push_back("Mlvjj_WH125_electron");
    }
  }
  // Input each signal
  vecBak.push_back("Mlvjj_WW_electron");
  vecBak.push_back("Mlvjj_WZ_electron");
  vecBak.push_back("Mlvjj_ZZ_electron");
  vecBak.push_back("Mlvjj_DYJets_electron");
  vecBak.push_back("Mlvjj_TTbar_electron");
  vecBak.push_back("Mlvjj_STopT_T_electron");
  vecBak.push_back("Mlvjj_STopT_Tbar_electron");
  vecBak.push_back("Mlvjj_STopS_T_electron");
  vecBak.push_back("Mlvjj_STopS_Tbar_electron");
  vecBak.push_back("Mlvjj_STopTW_T_electron");
  vecBak.push_back("Mlvjj_STopTW_Tbar_electron");
  vecBak.push_back("Mlvjj_QCD_electron");
  vecBak.push_back("Mlvjj_WJets_electron");

  // Creating the datacard
  ofstream outFile("histos_electron_datacard.txt");

  // process counter line
  for (int jmax = 0 ; jmax < int (vecSig.size()+vecBak.size()); jmax++){ 

  } // for process counter

  // Input total number of bins
  outFile << "imax " << "1" << endl;
  
  // Input total number of processes in bin1 minus one
  outFile << "jmax " << jmax - 1 << endl; 
  
  // Input total number of systematics
  outFile << "kmax " << "*" << endl;
  outFile << "------------------------" << endl;
  
  // Input the root file name and the data_obs file name
  outFile << "shapes data_obs * histos_electron.root Mlvjj_Data_electron_electron" << endl;

  // Input the root file name and whatever edits to the names you wish to make
  outFile << "shapes * * histos_electron.root $PROCESS" << endl;
  
  outFile << "------------------------" << endl;
  
  // Input the name of your bin
  outFile << "bin " << "channel1" << endl; 
  
  TH1D* proc = (TH1D*) in_file->Get("Mlvjj_Data_electron_electron");
  outFile << "observation " << proc->Integral() << endl;
  
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
  for (int sig_num = 0 ; sig_num < vecSig.size(); sig_num++){ 

    // Output a negative number (or zero) for each signal process
    outFile << -sig_num << " ";

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
    outFile << data_store << " "; 
    
  } // for signal integral loop
  
    // Creating array for background rates
  for (unsigned int r_bak = 0 ; r_bak < vecBak.size(); r_bak++){
    
    // Get the histograms out
   TH1D* bak = (TH1D*) in_file->Get(vecBak[r_bak]);  
    
    // Integrate each histogram
    double data_store = bak->Integral(); 
    
    // Print signal integral data
    outFile << data_store << " "; 
    
  } //for background integral loop

  outFile << endl;
  
  outFile << "------------------------" << endl;

  // adding luminosity systematic
  outFile << "lumi lnN ";

  // adding a counter for luminosity, constant across all processes
  for (unsigned int n_lumi = 0 ; n_lumi < (vecBak.size() + vecSig.size()); n_lumi++){

    outFile << "1.045 ";

  } //for luminosity loop

  outFile.close();
  
} //read_from_file_3
