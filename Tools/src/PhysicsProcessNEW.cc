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
                                      string fileNameTEMP,
                                      double cross_section,
                                      double lum, 
                                      unsigned int in_ev,
                                      string treeName):
   name(procName),
   fileName(fileNameTEMP),
   chain (0),
//   ntuple(0),
//   metree(0),
//   mnt(0),
   sigma(cross_section),
   intLum(lum),
   initial_events(in_ev)
{
   TFile * file = TFile::Open(fileName);
   if (!file->IsOpen())
   {
      cout << "ERROR proc::proc() could not open file " << fileName << endl;
      return;
   }
/*
   if (!file->cd("PS"))
   {
      cout << "ERROR proc::proc() could not CD into directory PS in file " << fileName << endl;
      return;
   }
*/ 
   chain = (TChain*) file->Get(treeName.c_str());
   if (chain == 0)
   {
      cout << "ERROR proc::proc() could not find tree named " << treeName << " in file " << fileName << endl;
      return;
   }
/*
   if (chain->GetBranch("EvtTree")) {
      ntuple = new EventNtuple();
      chain->SetBranchAddress("EvtTree",&ntuple);
   }
   else if (chain->GetBranch("EvtNtuple")) {
      ntuple = new EventNtuple();
      chain->SetBranchAddress("EvtNtuple",&ntuple);
   }
   else {
      cout << "\nWARNING proc::proc()::run EvtTree and EvtNtuple branches not found." << endl
           << "\tSetting EventNtuple pointer to 0x0." << endl;
   }
   if (chain->GetBranch("METree")) {
      metree = new METree();
      chain->SetBranchAddress("METree",&metree);
   }
   else {
      cout << "\nWARNING proc::proc()::run METree branch not found." << endl
           << "\tSetting METree pointer to 0x0." << endl;
   }
   if (chain->GetBranch("mnt")) {
      mnt = new MicroNtuple(2);
      chain->SetBranchAddress("mnt",&mnt);
   }
   else {
      cout << "\nWARNING proc::proc()::run mnt branch not found." << endl
           << "\tSetting MicroNtuple pointer to 0x0." << endl;
   }
*/
}

// ##################################################
// ############ COLORED PHYSICS PROCESS #############
// ##################################################

PlotterPhysicsProcessNEW::PlotterPhysicsProcessNEW (string procName,
                                                    string fileNameTEMP,
                                                    double cross_section,
                                                    double lum, 
                                                    unsigned int in_ev,
                                                    int col,
                                                    string treeName,
                                                    DEFS::LeptonCat lepCat):
   PhysicsProcessNEW(procName, fileNameTEMP, cross_section, lum, in_ev, treeName),
   color(col),
   leptonCat(lepCat)
{
   
}
