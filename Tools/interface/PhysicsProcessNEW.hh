// Name: PhysicsProcessNEW.hh
// Author: Travis Lamb

// This class will eventually replace an older Physics class. It is a base class that simply holds data members.

#ifndef PHYSICSPROCESSNEW_DEF
#define PHYSICSPROCESSNEW_DEF

#include "TString.h"
#include "TChain.h"

#include <iostream>

#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

class PhysicsProcessNEW
{
public:
   PhysicsProcessNEW(std::string procName,
                     std::string fileNameTEMP,
                     double cross_section,
                     double lum, 
                     unsigned int in_ev,
                     std::string treeName = "PS/EvtTree");
   
   double getScaleFactor() {return sigma*intLum / initial_events;}
   
   // The given name of the process
   TString  name;
   // The file path of the process
   TString  fileName;
   // The pointer to the file's TChain
   TChain* chain;
   // The pointer to the file's EventNtuple
   //EventNtuple* ntuple;
   // The pointer to the file's METree
   //METree* metree;
   // The pointer to the file's MicroNtuple
   //MicroNtuple* mnt;
   // The production cross section
   double  sigma;
   // The integrated luminosity to which to normalize
   double  intLum;
   // The initial number of events in the MC sample
   unsigned int initial_events;
};

class PlotterPhysicsProcessNEW: public PhysicsProcessNEW
{
public:
   PlotterPhysicsProcessNEW(std::string procName,
                            std::string fileNameTEMP,
                            double cross_section,
                            double lum, 
                            unsigned int in_ev,
                            int col,
                            std::string treeName = "PS/EvtTree",
                            DEFS::LeptonCat lepCat = DEFS::both);
   
   // the color used to draw this process
   int color;
   // The lepton this process is for (default is both)
   DEFS::LeptonCat leptonCat;
};

#endif
