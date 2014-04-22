////////////////////////////////////////////////////////////////////////////////
//
// TAMUWWMVA
// ---------
//
//                         08/28/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is responsible for the training and plotting of the TAMUWW MVA
// based descriminators. One can either choose to train the desciminatore,
// draw the resulting plots, or do both.
////////////////////////////////////////////////////////////////////////////////

#ifndef TAMUWWMVA_HH
#define TAMUWWMVA_HH

//
// User Defined Includes
//
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

//
// ROOT includes
//
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"
#include "TKey.h"
#include "TCut.h"
#include "TString.h"
#include "TH1.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TDatime.h"
#include "TPluginManager.h"
#include "TMath.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TLorentzVector.h"
#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif
#ifdef __CINT__                                                                                                      
#pragma link C++ class std::vector<TLorentzVector>+;                                                                 
#endif

//
// Standard Library Includes
//
#include <cstdlib>
#include <iostream> 
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

//
// Namespace
//
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class TAMUWWMVA {
public:
   //
   // Construction/Destruction
   //
   TAMUWWMVA();
   TAMUWWMVA(TString ml, vector<PhysicsProcess*> proc, vector<TString> s, vector<TString> b,
             vector<int> ep, DEFS::LeptonCat lc, vector<TString> p, TString ofb, TString of, bool ba);
   virtual ~TAMUWWMVA();

   //
   // Member Functions
   //

   /// reads the filenames in the output directory and appends an integer to the
   /// end if the filename already exists. The default integer to append is 1.
   TString getFilename(TString ofile = "TMVA");

   /// returns the number of variables that will be trained upon
   int getTSize();
   
   /// major function for booking and analyzing MVA methods
   void TMVAClassification();
   
   /// find all input variables types
   TList* GetKeyList(const TString& pattern, TList* TMVAGui_keyContent);

   /// utility function
   void ActionButton( vector<TString>& TMVAGui_inactiveButtons, TList* TMVAGui_keyContent, 
                      const TString& title, TString requiredKey = "" );

   /// check if a file is already open. If not, open it.
   TFile* OpenFile( const TString& fin );

   /// get the number of variables in a directory
   Int_t GetNumberOfInputVariables( TDirectory *dir );

   /// method to make and save plots relating to the MVA
   void Plot();

   void IsBatch(bool b) {batch = b;}
   void setMethodList(TString ml) {myMethodList = ml;}
   void setProcesses(vector<PhysicsProcess*> proc) {processes = proc;}
   void setSignals(vector<TString> s) {signals = s;}
   void setBackgrounds(vector<TString> b) {backgrounds = b;}
   void setLeptonCat(DEFS::LeptonCat lc) {leptonCat = lc;}
   void setPlots(vector<TString> p) {plots = p;}
   void setOfileBase(TString ofb) {ofileBase = ofb;}
   void setOfile(TString of) {ofile = of;}
   void setEventProbsToRun(vector<int> ep){eventProbs = ep;}

private:
   //
   // Member Data
   //
   bool batch;
   TString myMethodList, ofileBase, ofile, odir;
   vector<PhysicsProcess*> processes;
   vector<TString> signals, backgrounds, plots;
   vector<int> eventProbs;
   DEFS::LeptonCat leptonCat;

   ClassDef(TAMUWWMVA,1)
};

#endif
