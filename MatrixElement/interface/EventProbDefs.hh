// Author: Ricardo Eusebi
// TAMU, eusebi@physics.tamu.edu
// Created: March 13, 2011.
// $Id: EventProbDefs.hh,v 1.2 2011/03/24 20:23:33 ilyao Exp $

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
      HWW     = 150,
      
      // Related to single top production
      TopS    = 200,
      TopT    = 201,
      TopTAlt = 202,
      
      // Related to top pair production
      TTbar   = 250,
      
      // Related to single vector boson production
      WJets   = 300,
      WLg     = 301,
      Wgg     = 302,
      WLL     = 303,
      Wbb     = 304,
      Wc      = 305,
      ZLight  = 306,
      WLight  = 399,


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

