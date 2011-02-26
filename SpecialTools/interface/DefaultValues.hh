#ifndef DefaultValues_HH
#define DefaultValues_HH


//C++ libraries
#include <map>
#include <vector>
#include <string>

//Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"

class DefaultValues{

  public:

  static std::string getWeightForCategory(DEFS::TagCat tagcat,  
					  DEFS::PhysicsProcessType, int );

  // Return a table with normalization of process for the given tag category
  static Table getNormTable(DEFS::TagCat tagcat);

  // Return a table with location of files for the given tag category
  static Table getFileLocationTable(DEFS::TagCat tagcat);

  // Return a vector of PhysicsProcess's for the requested processName's
  // Each PhysicsProces has norm obtained from getNormTable, and 
  // ntuples chained from files in getFileLocationTable
  // Calls the getSingleProcess recursively
  static std::vector < PhysicsProcess * > getProcesses(
						       std::vector<DEFS::PhysicsProcessType> processName,
						       DEFS::JetBin jetBin, 
						       DEFS::TagCat tagcat);

  // Return a single PhysicsProces with norm obtained from normTable, and 
  // ntuples chained from files in fileTable
  static PhysicsProcess * getSingleProcess(DEFS::PhysicsProcessType process,
					   DEFS::JetBin jetBin,
					   Table normTable,
					   Table fileTable);


};
 
#endif
