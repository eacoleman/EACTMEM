// Name: PhysicsProcess.hh
// Author: Travis Lamb

// This class will eventually replace an older Physics class. It is a base class that simply holds data members.

#ifndef PHYSICSPROCESS_DEF
#define PHYSICSPROCESS_DEF

//ROOT libraries
#include "TFile.h"
#include "TString.h"
#include "TChain.h"

//C++ libraries
#include <iostream>
#include <utility>

#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/MEPATNtuple/interface/METree.hh"

using namespace std;

typedef map<DEFS::LeptonCat,double> PhysParMap;
typedef map<DEFS::LeptonCat,unsigned int> PhysParMapUI;

class PhysicsProcess
{
public:
   PhysicsProcess(){;}
   PhysicsProcess(std::string procName,
                  std::string groupingName,
                  std::string fileNameTEMP,
                  std::string treeName = "PS/EvtTree");

   double getScaleFactor(DEFS::LeptonCat lepCat) {
     return sigma[lepCat]*intLum[lepCat]*scaleFactor[lepCat] * branching_ratio[lepCat] / initial_events[lepCat];
   }
   void setPhysicsParameters(PhysParMap cross_section, PhysParMap lum, PhysParMap br, PhysParMapUI in_ev, PhysParMap sf);
   void fillMETreeIndexMap();

   // The given name of the process
   TString  name;
   inline TString getName() const {return name;}
   // The group with which you will merge it in canvases (Diboson, Single Top, etc)
   TString groupName;
   // The file path of the process
   TString  fileName;
   // The pointer to the file's TChain
   TChain* chain;
   // The production cross section
   PhysParMap sigma;
   // The branching ratio
   PhysParMap branching_ratio;
   // The integrated luminosity to which to normalize
   PhysParMap  intLum;
   // The initial number of events in the MC sample
   PhysParMapUI initial_events;
   // A scale factor which can be applied to any process for any purpose (i.e. QCD/WJets scaling)
   PhysParMap scaleFactor;
   // A map of the physics processes used in the MEs and their position in the array that
   // stores the event probabilities
   indexMap2 indexMapOfME;
};

class PlotterPhysicsProcess: public PhysicsProcess
{
public:
   PlotterPhysicsProcess(std::string procName,
                         std::string groupingName,
                         std::string fileNameTEMP,
                         int col,
                         std::string treeName = "PS/EvtTree");
   
   // the color used to draw this process
   int color;

};

#endif
