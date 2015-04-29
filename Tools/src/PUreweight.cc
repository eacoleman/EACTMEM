// This code' libraries
#include "TAMUWW/Tools/interface/PUreweight.hh"

//C++ libraries
#include <iostream>
#include <string>
#include <cmath>
#include <utility>

//Root libraries
#include "TFile.h"
#include "TH1.h"

using std::cout;
using std::endl;
using std::string;
using std::pair;



//------------------------------------------------------------------------
PUreweight::PUreweight(TString dataFile, TString MCFile, TString dataHisto, TString MCHisto, pair<int,int> rebin){

  TH1 * PU_intended   = getHistoFromFile(dataFile,dataHisto);
  TH1 * PU_generated  = getHistoFromFile(MCFile,MCHisto);

  if (PU_intended && PU_generated){
    PU_intended->Rebin(rebin.first);
    PU_generated->Rebin(rebin.second);
    PU_intended->Scale( 1.0/ PU_intended->Integral());
    PU_generated->Scale( 1.0/ PU_generated->Integral());
    weights_ = PU_intended;
    weights_->Divide(PU_generated);
    delete PU_generated;
  }
}

//------------------------------------------------------------------------
double PUreweight::getWeight(double tnpu){
    
  int histobin = weights_->GetXaxis()->FindBin(tnpu); //                                                                  
  return weights_->GetBinContent(histobin); 

}//getWeight

//-----------------------------------------------------------------------
TH1 * PUreweight::getHistoFromFile(TString filename, TString histoname){

  TFile * File = TFile::Open(filename);
  if (!File->IsOpen()){
    cout << "Error in PUreweight::getHistoFromFile: Could not open file " << filename << endl;
    return 0; 
  }
   
  TH1 * Histo  = (TH1*)File->Get(histoname);
  if (!Histo){
    cout << "Error in PUreweight::getHistoFromFile: Could not get histo " << histoname << " from " <<filename << endl;
    return 0; 
  }

  TH1 * Clone = (TH1*)Histo->Clone();
  Clone->SetDirectory(0);
  File->Close();

  return Clone;

}

