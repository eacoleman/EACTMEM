/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef EVENTNTUPLE_HH
#define EVENTNTUPLE_HH

//
// User Defined Includes
//
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "ElectroWeakAnalysis/VPlusJets/interface/QGLikelihoodCalculator.h"
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"
#include "TAMUWW/MEPATNtuple/interface/Vertex.hh"
#include "TAMUWW/MEPATNtuple/interface/Jet.hh"
#include "TAMUWW/MEPATNtuple/interface/Lepton.hh"
#include "TAMUWW/MEPATNtuple/interface/MET.hh"

//
// ROOT includes
//
#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TRegexp.h"

//
// Standard Library Includes
//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <utility>

//
// Namespace
//
using std::vector;
using std::map;
using std::string;
using std::pair;
using std::cout;
using std::setw;
using std::left;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class GenParticle
{
public:
   //
   // Construction/Destruction
   //
   GenParticle();
   ~GenParticle();

   double charge;
   TLorentzVector p4;
   TVector3 vtx;
   int pdgId;
   int status;
   size_t particlePosition;
   int numberOfMothers;
   int numberOfDaughters;
   vector<size_t> motherPositions;
   vector<size_t> daughterPositions;

   ClassDef(GenParticle,1)
};

class EventNtuple : public TObject
{
public:
   //
   // Construction/Destruction
   //
   EventNtuple();
   ~EventNtuple();

   //
   // Grouped Cuts
   //
   bool baseCuts();
   bool FNALcutsElectron();
   bool FNALcutsMuon();

   //
   // Individual Cuts
   //
   bool METEtMin(double EtMin = 25.0);

   //
   // QGLikelihood
   //
   double getQGLikelihood(unsigned int index, QGLikelihoodCalculator* qglikeli = 0);
   vector < double > getQGLikelihoods(QGLikelihoodCalculator* qglikeli = 0);

   //
   // Jet Energy Resolution
   //
   void doJER();
   double getJERfactor(double pt, double eta, double ptgen);

   //
   // MET-Phi Corrections
   //
   void doMETPhiCorrection(TString eraType = "pfMEtSysShiftCorrParameters_2012runAvsNvtx_data");
   pair<double,double> getMETPhiCorrection(TString eraType = "pfMEtSysShiftCorrParameters_2012runAvsNvtx_data");

   //
   // Generator-Level Particle Information
   //
   // Print Event Information
   void printDecayInformation(int decayParticle);
   void printHiggsDecayInformation();

    //
    // Trigger Information
    //
    // Get wasAccept information for a specific trigger
    bool findSpecificTrigger(string triggerName);
    // Print wasAccept information for a specific trigger
    void printSpecificTrigger(string triggerName);
    // Get wasAccept information for multiple triggers. Return the && if true and || if false.
    bool findTriggers(TString triggerName, bool andor = true);
    // Print wasAccept information for multiple triggers
    void printTriggers(TString triggerName);

   //Needed for ME
   int run;
   int event;
   double rho;
   vector < Vertex > vLV;
   vector < Jet > jLV;
   double Mjj;
   vector < Lepton > lLV;
   vector < MET > METLV;
   vector < GenParticle > genParticleCollection;

   map<string,bool> triggerMap;

   ClassDef(EventNtuple,10)
};

#endif
