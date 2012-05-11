// Author: Ricardo Eusebi
// TAMU, eusebi@physics.tamu.edu
// Created: March 13, 2011.
// $Id: EventProbDefs.hh,v 1.3 2012/04/30 03:41:38 ilyao Exp $

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
      STopT    = 200,
      STopS    = 201,
      STopTW   = 202,

      TopTAlt  = 250,//Used for tChannel3Jets
      
      // Related to top pair production
      TTbar   = 299,
      
      // Related to single vector boson production
      WJets   = 300,
      WLg     = 301,
      Wgg     = 302,
      WLL     = 303,
      WLb     = 304,
      Wbb     = 305,
      Wc      = 310,
      ZLight  = 350,
      WLight  = 399,


      // Related to double vector boson production
      WW      = 400,
      WZ      = 401,
      WZbb    = 403,
      ZZ      = 404,
     
      // Related to QCD
      QCD     = 500,
      
      // Related to the correlated EPD
      CorrEPD  = 1000

    };
    
    std::string getTypeString(Type type);

  } // EP namespace

}// DEFS namespace

#endif

