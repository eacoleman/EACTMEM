#ifndef DefaultValues_HH
#define DefaultValues_HH


//C++ libraries
#include <map>
#include <vector>
#include <string>

//Our libraries
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"

class DefaultValues{

  public:

  static std::string getWeightForCategory(DEFS::TagCat tagcat,  DEFS::PhysicsProcessType, int );

  static Value getNorm(DEFS::PhysicsProcessType, DEFS::TagCat tagcat, DEFS::JetBin, int detector);

  static std::map < DEFS::PhysicsProcessType , std::string> getMapOfFiles(DEFS::JetBin jetBin);
  static Table * getM2Table(DEFS::PhysicsProcessType processType, DEFS::TagCat tagcat, int detector);

  static std::vector < PhysicsProcess * > getAllProcesses(DEFS::JetBin jetbin, DEFS::TagCat tagcat);
  static std::vector < PhysicsProcess * > getProcesses(DEFS::JetBin jetbin, DEFS::TagCat tagcat, int HiggsMassIndex);
  static std::vector < PhysicsProcess * > getProcesses(DEFS::JetBin jetbin, DEFS::TagCat tagcat, DEFS::PhysicsProcessType HiggsType);
  
  static PhysicsProcess * getSingleProcess(DEFS::JetBin jetbin, DEFS::TagCat tagcat,  DEFS::PhysicsProcessType processType);
  
};
 
#endif
