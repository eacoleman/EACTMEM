// Author: Ricardo Eusebi
// TAMU, eusebi@physics.tamu.edu
// Created: March 13, 2011.
// $Id:$

#ifndef EVENTPROBDEFS_HH
#define EVENTPROBDEFS_HH
#include <string>

namespace DEFS {

  // This describes all the possible Matrix Element probabilities 
  // that could be calculated in this framework. For backwards compatibility 
  // never remove an existing probability from this list, only add them.
  namespace EP {

    enum Type {
      
      // UNKNOWN. Used for initializations.
      UNKNOWN = -1,

      // Related to Higgs 
      WH      = 100,   
      HWW     = 101,
      
      // Related to single top production
      TopS    = 200,
      TopT    = 201,
      TopTAlt = 202,
      
      // Related to top pair production
      TTbar   = 250,
      
      // Related to single vector boson production
      WLight  = 300,
      ZLight  = 301,
      Wbb     = 302,
      Wc      = 303,
      Wgg     = 304,
      
      // Related to double vector boson production
      WW      = 400,
      WZ      = 401,
      ZZ      = 402,
      
      // Related to QCD
      QCD     = 500,
      
      // Related to the correlated EPD
      CorrEPD  = 1000

    };
    
    std::string getTypeString(Type type);

  } // EP namespace

}// DEFS namespace

#endif

