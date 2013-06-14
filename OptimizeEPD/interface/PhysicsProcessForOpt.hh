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
  

};//class PhysicsProcessForOpt

#endif
