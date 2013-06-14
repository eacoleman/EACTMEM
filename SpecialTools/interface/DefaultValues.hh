#ifndef DefaultValues_HH
#define DefaultValues_HH

//ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TList.h"
#include "TString.h"

//C++ libraries
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>

//Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcessNEW.hh"
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
  static std::vector < PhysicsProcessNEW * > getProcesses(std::vector<DEFS::PhysicsProcessType> processName,
                                                          DEFS::JetBin jetBin, 
                                                          DEFS::TagCat tagcat,
                                                          bool forPlots);
   
  // Return a single PhysicsProces with norm obtained from normTable, and 
  // ntuples chained from files in fileTable
  static PhysicsProcessNEW * getSingleProcess(DEFS::PhysicsProcessType process,
                                              DEFS::JetBin jetBin,
                                              std::map<DEFS::LeptonCat,Table> normTable,
                                              Table fileTable,
                                              bool forPlots);


  
  //GIVE THE OFFICIAL LIST OF PROCESSES FOR THE GIVEN ANALYSIS
  // DON'T MESS WITH THIS METHOD, CREATE ANOTHER ONE FOR TEST PURPOSES
  static std::vector < PhysicsProcessNEW * > getProcessesWW(DEFS::JetBin,
                                                            DEFS::TagCat,
                                                            bool include_data);
   static std::vector < PhysicsProcessNEW * > getProcessesHiggs(DEFS::JetBin,
                                                                DEFS::TagCat,
                                                                bool include_data,
                                                                bool forPlots);

   // Returns the cross section for the given process
   static pair<double,double> getCrossSectionAndError(TString channelName);
   
   // Returns the branching ratio for the given process
   static double getBranchingRatio(TString channelName);
   
   // Returns the number of Monte Carlo events for the given process
   static double getNumMCEvts(TString channelName);

   // Returns the index of the location of b within a, or -1 if b is not found in a
   static int vfind(vector<string> a, string b);
   static int vfind(vector<TString> a, TString b);

   // Destroy all open canvases
   static void DestroyCanvases();


};
 
#endif
