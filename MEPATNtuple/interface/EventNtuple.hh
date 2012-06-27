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

//
// Root includes
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

//
// Namespace
//
using std::vector;
using std::map;
using std::string;

class GenParticle
{
public:
   
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


   //Needed for ME
   vector < TLorentzVector > matchedGenParticles, refjLV, rawjLV, jLV, METLV, lLV;
   vector < int > matchedpdgId, refpdgid;
   vector < double > matchedDeltaR;
   vector < int > jBtagSSV, jBtagTC;
   vector < double > jBtagDiscriminatorSSV, jBtagDiscriminatorTC;
   vector < GenParticle > genParticleCollection;
   vector < float > tnpus;
   vector < int > npus;
   int npv;
   int lQ;
   int ldetComp;
   int run;
   int event;

   //Additional
   double        Mjj;
//   int           passStd;
//   int           passAll;
   DEFS::LeptonCat     leptonCat;
   DEFS::LeptonCat     leptonCat_passAll;

   double        DRlj1;
   double        DRlj2;
   double        Thetalj1pj2;

   double        lTotIso;
   double        lecalIso;
   double        lhcalIso;
   double        ltrkIso;
   double        ldetIso;
   double        lpfIso;
   double        lmvaTrig;
   double        lmvaNonTrig;

   double        METEt;
   double        lPhi;

   vector < double > jChargedMultiplicity;
   vector < double > jNeutralMultiplicity;
   vector < double > jPtD;
   double rho;

   map<string,bool> triggerMap;

   ClassDef(EventNtuple,7)

};

#endif
