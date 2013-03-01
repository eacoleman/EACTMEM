#ifndef CUTOPTIMIZATION_HH
#define CUTOPTIMIZATION_HH

//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/FigureOfMerit.hh"

#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2D.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TString.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  class declaration
////////////////////////////////////////////////////////////////////////////////
class CutOptimization : public FigureOfMerit {
public:
   //C'tor
   CutOptimization();
   //D'tor
   ~CutOptimization();

   //Member Functions
   void drawSaveProjections(TString ofile, vector<bool> logScales);
   Double_t getFOM(Double_t FOM, vector<pair<int,int> > ranges);
   void getMaxFOMAndBin();
   void getRanges(int depth, vector<Int_t>& coord);
   void loopOverCuts(Double_t FOM, TString option = "");
   TProfileMDF* readAndSumTProfileMDF(TString ifile, vector<TString> treeNames, TString profNameTitle);
   TProfileMDF* readInTProfileMDF(TString ifile, TString treeName);
   void setSignal(TProfileMDF* sig) {signal = sig;}
   void setBackground(TProfileMDF* back) {background = back;}
   void setCuts(vector<TString> c) {cuts = c;}
   void setFOMContainer();

private:
   TProfileMDF* signal;
   TProfileMDF* background;
   TProfileMDF* fomContainer;
   vector<TString> cuts;
   vector<vector<pair<int, int> > > ranges;
   double maxFOM;
   vector<int> maxFOMBin;
};

#endif
