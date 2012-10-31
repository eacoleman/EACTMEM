#ifndef TRIGGEREFF_DEF
#define TRIGGEREFF_DEF

// Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"

// C++ libraries
#include <string>
#include <map>

// ROOT libraries
#include "TH2D.h"

// This namespace holds the definitions of all physics processes 
// author: Ricardo Eusebi, Feb 12, 2012

class TriggerEfficiency{

 public:

  bool addEfficiency(DEFS::LeptonCat leptCat);
  void createEfficiencies8TeV();
  double getWeight(DEFS::LeptonCat leptCat, double pt, double eta);

 private:

  TH2 * getTH2D(DEFS::LeptonCat);

  // the map holding an efficiency for each lepton category
  std::map<DEFS::LeptonCat,  TH2*> mapEffs;

};

#endif
