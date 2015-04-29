#ifndef TTBARREWEIGHT_DEF
#define TTBARREWEIGHT_DEF

// TAMUWW libraries
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

// C++ libraries
#include <iostream>
#include <string>
#include <cmath>
#include <string>
#include <utility>

// ROOT libraries
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH1D.h"

class TTbarreweight{

public:

   TTbarreweight();
   // Returns the TTbar event weight
   double getWeight(const EventNtuple * ntuple, const PhysicsProcess* proc);
   // Returns the pt of the two top particles in an event
   std::pair<double,double> ttbarPt(const EventNtuple * ntuple);
   
private:

};

#endif
