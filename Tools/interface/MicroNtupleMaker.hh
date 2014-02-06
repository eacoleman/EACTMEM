#ifndef MICRONTUPLEMAKER_HH
#define MICRONTUPLEMAKER_HH

// C++ libraries
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <sstream>
#include <fstream>

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
#include "TTreeIndex.h"

// This code libraries
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellInt.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

using namespace std;

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
   void makeMicroNtuple(TChain& chain, TString output, unsigned nJets, 
                        bool doLight=false, bool doNonW= false, bool doUntag=false);
   void setEventNtuplePath(TString mnen) {mergeNewEventNtuple = mnen;}
   void setProcess(TString p) {currentProcess = p;}
   void setOutputPath(TString p) {outputPath = p;}
   void setMissingEventsFlag(bool f) {missingEventsFlag = f;}
   void writeMissingEventsFile(map<int,int> &missingME);
   int  getIntLength(int i);

   static inline void loadbar(unsigned int x, unsigned int n, unsigned int w = 50) {
    if ( (x != n) && (x % (n/100) != 0) ) return;
 
    float ratio  =  x/(float)n;
    int   c      =  ratio * w;
 
    cout << setw(3) << (int)(ratio*100) << "% [";
    for (int x=0; x<c; x++) cout << "=";
    for (unsigned int x=c; x<w; x++) cout << " ";
    cout << "]\r" << flush;
  }
  static inline void loadbar2(unsigned int x, unsigned int n, unsigned int w = 50) {
    if ( (x != n) && (x % (n/100) != 0) ) return;
 
    float ratio  =  x/(float)n;
    int   c      =  ratio * w;
 
    cout << setw(3) << (int)(ratio*100) << "% [";
    for (int x=0; x<c; x++) cout << "=";
    for (unsigned int x=c; x<w; x++) cout << " ";
    cout << "] (" << x << "/" << n << ")\r" << flush;
  }

// Process has done i out of n rounds,
// and we want a bar of width w and resolution r.
static inline void loadBar(int x, int n, int r, int w)
{
    // Only update r times.
    if ( x % (n/r) != 0 ) return;
 
    // Calculuate the ratio of complete-to-incomplete.
    float ratio = x/(float)n;
    int   c     = ratio * w;
 
    // Show the percentage complete.
    printf("%3d%% [", (int)(ratio*100) );
 
    // Show the load bar.
    for (int x=0; x<c; x++)
       printf("=");
 
    for (int x=c; x<w; x++)
       printf(" ");
 
    // ANSI Control codes to go back to the
    // previous line and clear it.
    //printf("]\n\033[F\033[J");
    printf("]\n33[1A33[2K");
}


private:

   TString outputPath;
   TString currentProcess;
   unsigned nentries;
   bool missingEventsFlag;
   TString mergeNewEventNtuple;   
   TChain* mergeChain;
   TTree* mergeTree;
   TTreeIndex* index;
   EventNtuple * mergeEventNtuple;
   map<int,int> eventIndex;
   bool imFilled;
   map<int,int> missingME;

};
#endif
