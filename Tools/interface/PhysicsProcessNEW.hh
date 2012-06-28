// Name: PhysicsProcessNEW.hh
// Author: Travis Lamb

// This class will eventually replace an older Physics class. It is a base class that simply holds data members.

#ifndef PHYSICSPROCESSNEW_DEF
#define PHYSICSPROCESSNEW_DEF

#include "TString.h"
#include "TChain.h"

#include <iostream>

class PhysicsProcessNEW
{
public:
   PhysicsProcessNEW(std::string procName,
                     std::string fileName,
                     double cross_section,
                     double lum, 
                     unsigned int in_ev,
                     std::string treeName = "PS/EvtTree");
   
   double getScaleFactor() {return sigma*intLum / initial_events;}
   
   // The given name of the process
   TString  name;
   // The pointer to the file's TChain
   TChain* chain;
   // The production cross section
   double  sigma;
   // The integrated luminosity to which to normalize
   double  intLum;
   // The initial number of events in the MC sample
   unsigned int initial_events;
};

class ColoredPhysicsProcessNEW: public PhysicsProcessNEW
{
public:
   ColoredPhysicsProcessNEW(std::string procName,
                            std::string fileName,
                            double cross_section,
                            double lum, 
                            unsigned int in_ev,
                            int col,
                            std::string treeName = "PS/EvtTree");
   
   // the color used to draw this process
   int color;
};

#endif
