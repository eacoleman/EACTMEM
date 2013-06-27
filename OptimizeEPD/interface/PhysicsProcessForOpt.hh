#ifndef PHYSICSPROCESSFOROPT_HH
#define PHYSICSPROCESSFOROPT_HH


#include "TAMUWW/SpecialTools/interface/PhysicsProcessMemory.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

#include "TH1.h"


class PhysicsProcessForOpt : public PhysicsProcessMemory {

public:  
  
  // Default C'tors
  PhysicsProcessForOpt(const PhysicsProcessMemory & );

  void setProjectionsForOpt();

  void fillNormEPDHisto(TH1* histo, 
			DEFS::TagCat tagcat,
			double mhiggs, 
			const ProbsForEPD & meProbs);
  
  void setEPDFunction(double (*userEPDFunc) (const ProbsForEPD &));

protected:
  //user defined function to return an epd 
  double (*userEPDFunc) (const ProbsForEPD &); 

  //this is the default user defined function
  static double defaultEPDFunc(const ProbsForEPD &){
    return 0;
  }

  std::map <unsigned int, bool> failed_entries_map;

};//class PhysicsProcessForOpt

#endif
