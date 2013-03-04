// Name: PhysicsProcessNEW.cc
// Author: Travis Lamb

// This class will eventually replace an older Physics class. It is a base class that simply holds data members.

#include "TFile.h"

#include "TAMUWW/Tools/interface/PhysicsProcessNEW.hh"

using namespace std;

// ##################################################
// ################ PHYSICS PROCESS #################
// ##################################################

PhysicsProcessNEW::PhysicsProcessNEW (string procName,
				      string groupingName,
                                      string fileNameTEMP,
                                      double cross_section,
                                      double lum, 
                                      unsigned int in_ev,
                                      string treeName,
				      DEFS::LeptonCat lepcat):
   name(procName),
   groupName(groupingName),
   fileName(fileNameTEMP),
   chain (0),
   sigma(cross_section),
   intLum(lum),
   initial_events(in_ev),
   leptonCat(lepcat)
{
   TFile * file = TFile::Open(fileName);
   if (!file->IsOpen())
   {
      cout << "ERROR proc::proc() could not open file " << fileName << endl;
      return;
   }

   if (!file->cd("PS"))
   {
      cout << "ERROR proc::proc() could not CD into directory PS in file " << fileName << endl;
      return;
   }
  
   chain = (TChain*) file->Get(treeName.c_str());
   if (chain == 0)
   {
      cout << "ERROR proc::proc() could not find tree named " << treeName << " in file " << fileName << endl;
      return;
   }
}

// ##################################################
// ############ COLORED PHYSICS PROCESS #############
// ##################################################

PlotterPhysicsProcessNEW::PlotterPhysicsProcessNEW (string procName,
						    string groupingName,
                                                    string fileNameTEMP,
                                                    double cross_section,
                                                    double lum, 
                                                    unsigned int in_ev,
                                                    int col,
                                                    string treeName,
                                                    DEFS::LeptonCat lepCat):
  PhysicsProcessNEW(procName, groupingName, fileNameTEMP, cross_section, lum, in_ev, treeName, lepCat),
  color(col){
   
}
