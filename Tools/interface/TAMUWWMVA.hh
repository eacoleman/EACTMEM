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
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"

//
// ROOT includes
//
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TKey.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TDatime.h"
#include "TPluginManager.h"
#include "TMath.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
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
   TAMUWWMVA(TString ml, TString ifp, vector<TString> ifs, vector<TString> ifb,
             TString tn, double lum, vector<TString> p, TString ofb, TString of);
   virtual ~TAMUWWMVA();

   //
   // Member Functions
   //

   /// reads the filenames in the output directory and appends an integer to the
   /// end if the filename already exists. The default integer to append is 1.
   TString getFilename(TString ofile = "TMVA");
   
   /// returns the cross section for the given process
   double getCrossSection(TString channelName);
   
   /// returns the branching ratio for the given process
   double getBranchingRatio(TString channelName);
   
   /// returns the number of Monte Carlo events for the given process
   double getNumMCEvts(TString channelName);
   
   /// major function for booking and analyzing MVA methods
   void TMVAClassification();
   
   /// find all input variables types
   TList* GetKeyList(const TString& pattern, TList* TMVAGui_keyContent);

   /// utility function
   void ActionButton( vector<TString>& TMVAGui_inactiveButtons, TList* TMVAGui_keyContent, 
                      const TString& title, TString requiredKey = "" );
   
   /// find function for a vector
   int vfind(vector<TString> a, TString b);

   /// check if a file is already open. If not, open it.
   TFile* OpenFile( const TString& fin );

   /// destroy all open canvases
   void DestroyCanvases();

   /// get the number of variables in a directory
   Int_t GetNumberOfInputVariables( TDirectory *dir );

   /// method to make and save plots relating to the MVA
   void Plot();

private:
   //
   // Member Data
   //
   TString myMethodList, ifilePath, treeName, ofileBase, ofile;
   vector<TString> ifilesSignal, ifilesBackground, plots;
   double luminosity;

   ClassDef(TAMUWWMVA,1)
};

#endif
