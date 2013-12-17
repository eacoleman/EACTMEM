#ifndef DefaultValues_HH
#define DefaultValues_HH

//ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TError.h"
#include "TMath.h"
#include "TList.h"
#include "TString.h"
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"

//C++ libraries
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <assert.h>


//Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;

class DefaultValues{

public:

   // Returns the path to the config files
   // basically returns $CMSSW_BASE+"/src/TAMUWW/Config/Official/"
   static std::string getConfigPath();
  
   static std::string getWeightForCategory(DEFS::TagCat tagcat,  
                                           DEFS::PhysicsProcessType, int );

   // Return a table with normalization of process for the given tag category
   static Table getNormTable(DEFS::LeptonCat evtcat, DEFS::TagCat tagcat);

   // Return a table with location of files for the given tag category
   static Table getFileLocationTable(DEFS::TagCat tagcat);

   // Return a vector of PhysicsProcess's for the requested processName's
   // Each PhysicsProces has norm obtained from getNormTable, and 
   // ntuples chained from files in getFileLocationTable
   // Calls the getSingleProcess recursively
   static std::vector < PhysicsProcess * > getProcesses(std::vector<DEFS::PhysicsProcessType> processName,
                                                        DEFS::JetBin jetBin, 
                                                        DEFS::TagCat tagcat,
                                                        bool for_plots,
                                                        DEFS::NtupleType ntuple_type);

   
   // Return a single PhysicsProces with norm obtained from normTable, and 
   // ntuples chained from files in fileTable
   static PhysicsProcess * getSingleProcess(DEFS::PhysicsProcessType process,
                                            DEFS::JetBin jetBin,
                                            std::map<DEFS::LeptonCat,Table> normTable,
                                            Table fileTable,
                                            bool for_plots,
                                            DEFS::NtupleType ntuple_type);



  
   //GIVE THE OFFICIAL LIST OF PROCESSES FOR THE GIVEN ANALYSIS
   // DON'T MESS WITH THIS METHOD, CREATE ANOTHER ONE FOR TEST PURPOSES
   static std::vector < PhysicsProcess * > getProcessesWW(DEFS::JetBin,
                                                          DEFS::TagCat,
                                                          bool include_data,
                                                          bool for_plots,
                                                          DEFS::NtupleType ntuple_type);

   static std::vector < PhysicsProcess * > getProcessesHiggs(DEFS::JetBin,
                                                             DEFS::TagCat,
                                                             bool include_data,
                                                             bool for_plots,
                                                             DEFS::NtupleType ntuple_type);
   static std::vector < PhysicsProcess * > getProcessesTest(DEFS::JetBin jetBin,
                                                            DEFS::TagCat tagcat, 
                                                            bool include_data,
                                                            bool forPlots);

   // Returns the cross section for the given process
   static pair<double,double> getCrossSectionAndError(TString channelName);
   
   // Returns the branching ratio for the given process
   static double getBranchingRatio(TString channelName);
   
   // Returns the number of Monte Carlo events for the given process
   static double getNumMCEvts(TString channelName);

   // Returns the scale factor for the given process
   static double getScaleFactor(TString channelName);

   // Returns the index of the location of b within a, or -1 if b is not found in a
   static int vfind(vector<string> a, string b);
   static int vfind(vector<TString> a, TString b);

   // Destroy all open canvases
   static void DestroyCanvases();

   // Returns an object from any ROOT file in TAMUWW/ConfigFiles/Official/
   static TObject* getConfigTObject(TString histoFile, TString hname, TString newName);
   static TH1* getConfigTH1(TString histoFile, TString hname, TString newName);
   static TH2* getConfigTH2(TString histoFile, TString hname, TString newName);

   // Returns a rebinned TH2 object
   // There is still a problem with rebinning the underflow and overflow bins
   // Everything else works though
   TH2* Rebin2D(TH2* old, Int_t nxgroup, Int_t nygroup, const char*newname, const Double_t *xbins, const Double_t *ybins, TString Options = "");

   // Tests the Rebin2D function
   void Rebin2DTest(TString Options = "");

};
 
#endif
