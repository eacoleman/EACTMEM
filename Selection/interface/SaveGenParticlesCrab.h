////////////////////////////////////////////////////////////////////////////////
//
// SaveGenParticlesCrab
// --------------------
//
//                         07/31/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// Save the gen particle collection in an EventNtuple format and do nothing else
////////////////////////////////////////////////////////////////////////////////

//
// CMS includes
//
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "DataFormats/Provenance/interface/LuminosityBlockAuxiliary.h"
#include <Math/VectorUtil.h>
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/Selection/bin/RunEventSet.h"

//
// Root includes
//
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TString.h"

//
// Standard Library Includes
//
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

//
// Namespace
//
using namespace std;
//using namespace edm;
//using namespace reco;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class SaveGenParticles : public edm::EDAnalyzer {
public:
   // construction/destruction
   explicit SaveGenParticles(const edm::ParameterSet& iConfig);
   virtual ~SaveGenParticles();
   
private:
   // member functions
   void beginJob();
   void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
   void endJob();
   // additional (local) functions
   void getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup);
   void saveGenPart();

private:
   //
   // member data
   //
   // tree variables
   TTree* EvtTree_2Jets;
   EventNtuple* EvtNtuple;
   // handle InputTags/sources
   edm::InputTag genParticleSource;
   // handles
   edm::Handle<reco::GenParticleCollection> genParticleHandle;
   // event variables
   long runNumber;
   long eventNumber;
   long bxNumber;
   long orbitNumber;   
   long storeNumber;
   long timeNumber;
   long lumiSegmentNumber;
};
