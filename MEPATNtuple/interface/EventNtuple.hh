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

//
// Standard Library Includes
//
#include <string>
#include <vector>
#include <map>
#include <cassert>

//
// Namespace
//
using std::vector;
using std::map;
using std::string;

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
   // Generator-Level Particle Information
   //
   // Print Event Information
   void printDecayInformation(int decayParticle);
   void printHiggsDecayInformation();


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
