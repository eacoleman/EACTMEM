// Name: PhysicsProcessNEW.cc
// Author: Travis Lamb

// This class will eventually replace an older Physics class. It is a base class that simply holds data members.

#include "TAMUWW/SpecialTools/interface/PhysicsProcessNEW.hh"

// ##################################################
// ################ PHYSICS PROCESS #################
// ##################################################

PhysicsProcessNEW::PhysicsProcessNEW (string procName,
                                      string groupingName,
                                      string fileNameTEMP,
                                      string treeName):
   name(procName),
   groupName(groupingName),
   fileName(fileNameTEMP),
   chain (0)
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

   PhysParMap blankD;
   PhysParMapUI blankUI;
   for(unsigned int i = 0; i<DEFS::nLeptonCat; i++) {
      blankD[(DEFS::LeptonCat)i] = 0.0;
      blankUI[(DEFS::LeptonCat)i] = 0;
   }
   sigma = blankD;
   branching_ratio = blankD;
   intLum = blankD;
   initial_events = blankUI;
}

void PhysicsProcessNEW::setPhysicsParameters(PhysParMap cross_section, PhysParMap lum, PhysParMap br, PhysParMapUI in_ev){
   for(PhysParMap::iterator it = cross_section.begin(); it!=cross_section.end(); it++) {
      sigma[it->first] = it->second;
   }
   for(PhysParMap::iterator it = br.begin(); it!=br.end(); it++) {
      branching_ratio[it->first] = it->second;
   }
   for(PhysParMap::iterator it = lum.begin(); it!=lum.end(); it++) {
      intLum[it->first] = it->second;
   }
   for(PhysParMapUI::iterator it = in_ev.begin(); it!=in_ev.end(); it++) {
      initial_events[it->first] = it->second;
   }
}

// ##################################################
// ############ COLORED PHYSICS PROCESS #############
// ##################################################

PlotterPhysicsProcessNEW::PlotterPhysicsProcessNEW (string procName,
                                                    string groupingName,
                                                    string fileNameTEMP,
                                                    int col,
                                                    string treeName):
   PhysicsProcessNEW(procName, groupingName, fileNameTEMP, treeName),
   color(col){
   
}
