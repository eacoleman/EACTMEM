#ifndef PHYSICSPROCESSFOROPT_HH
#define PHYSICSPROCESSFOROPT_HH


#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

#include "TH1.h"


class PhysicsProcessForOpt : public PhysicsProcess {

public:  
  
  // Default C'tors
  PhysicsProcessForOpt(const PhysicsProcess & );

  void setProjectionsForOpt();

  void fillNormEPDHisto(TH1* histo, 
			DEFS::TagCat tagcat,
			double mhiggs, 
			const ProbsForEPD & meProbs);
  

};//class PhysicsProcessForOpt

#endif
