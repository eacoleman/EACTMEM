#ifndef MICRONTUPLEMAKER_HH
#define MICRONTUPLEMAKER_HH

// C++ libraries
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// ROOT libraries
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TBranch.h"
#include "TString.h"
#include "TBenchmark.h"

// This code libraries
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::set;

////////////////////////////////////////////////////////////////////////////////
//  class declaration
////////////////////////////////////////////////////////////////////////////////
// This struct is used to hold the signature of the microntuple
struct Signature
{
  int run;
  int event;

  // operator < is needed for the set ordering
  bool operator<(const Signature& rhs) const{
    if (run != rhs.run)
      return run < rhs.run;    
    return event < rhs.event;
  }//operator<

};

/*
//Created by RE to automatize the doing of MicroNtuples
//use like this:
// first do the ttbar splitting and HF removal at Event_Probs level using mergeAll.C
.X mergeAll.C+

//Then do all the microntuple by doing
root -l 
.L ../lib/slc5_ia32_gcc434/libTAMUWWMEPATNtuple.so
.L TAMUWW/Tools/createMicroNtuples.C+
createMicroNtuples(...)
*/

//RE auxiliary struct to make all the microntuples automatically
class  MyStr{
public:
  MyStr(TString d, TString n, bool m, bool nw, bool nwg){
    dirs.push_back(d);
    name = n;
    mistag = m ;
    nonw = nw;
    untag = nwg;
  }
  void   AddDir(TString d){ dirs.push_back(d);}
  vector<TString> GetLocationVector(TString path){
    vector<TString> res;
    for (unsigned d=0;d<dirs.size();d++)
      res.push_back(path + dirs[d]);
    return res;
  }
  vector<TString> dirs  ; // the set of directories to run over
  TString         name  ; // the file name of the output w/o the ".root" extension
  bool           mistag; // whether this is a mistag or
  bool           nonw  ; // whether this is nonW or not
  bool           untag  ; // whether this is untag or not
};

class MicroNtupleMaker {
public:
   //C'tor
   MicroNtupleMaker();
   //D'tor
   ~MicroNtupleMaker();

   //Member Functions
   // WARNING!!! is the microNtuple as bigger than 2Gs the code crashes
   void createMicroNtuple(TString inputPath = "/uscms_data/d3/ilyao/Winter12to13ME8TeV/MEResults/rootOutput/", TString outputPath = "/uscms_data/d3/ilyao/Winter12to13ME8TeV/MEResults/microNtuples/", int largeProcessCase=0, TString smallProcessLabel="");
   //Created by RE to automatize the doing of MicroNtuples
   void makeMicroNtuple(TString location, TString output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false);
   //Created by RE to automatize the doing of MicroNtuples
   void makeMicroNtuple(vector<TString> locations, TString output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false);
   // This is the core of the makeMicroNtuple algorithm.
   void makeMicroNtuple(TChain & chain, TString output, unsigned nJets, 
                        bool doLight=false, bool doNonW= false, bool doUntag=false);
   

private:

};
#endif
