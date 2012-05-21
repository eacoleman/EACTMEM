////////////////////////////////////////////////////////////////////////////////
//
// PerformSelectionCrab
// --------------------
//
//                         03/30/2012 Ilya Osipenkov <ilyao@fnal.gov> 
//                                    Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// CMS 
// Stage 2 Skimming of the PAT-tuples to be used in Matrix Element calculations.
// WW/H->WW CrossSection Measurement using Matrix Element 
////////////////////////////////////////////////////////////////////////////////
// Designed to work with 4_2_8 PAT-tuples and tested on V3 HWW sync exercises.
// Use to skim the PATNtuples and create a custom made .root file containing the needed information.
////////////////////////////////////////////////////////////////////////////////

//
// CMS includes
//

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "DataFormats/Provenance/interface/LuminosityBlockAuxiliary.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/UserData.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include <Math/VectorUtil.h>

#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexSorter.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/Tools/src/GlobalTools428.cc"
#include "TAMUWW/Selection/bin/RunEventSet.h"

//
// Root includes
//
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
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
class PerformSelection : public edm::EDAnalyzer {
public:
   // construction/destruction
   explicit PerformSelection(const edm::ParameterSet& iConfig);
   virtual ~PerformSelection();
   
private:
   // member functions
   void beginJob();
   void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
   void endJob();
   // trigger functions
   void triggerSelection();
   // event functions
   void setFlags();
   // vertex functions
   void vertexSelection();
   // jet functions
   void jetClear();
   void jetSelection();
   void computeInvariantMass();
   double getPtD(vector<reco::PFCandidatePtr> pfCandidates);
   // b-tag functions
   // muon functions
   void muonClear();
   void muonSelection();
   // electron functions
   void eleClear();
   void eleSelection();
   // lepton functions
   // MET functions
   void metSelection();
   // additional (local) functions
   void getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup);
   void setlp4LV();
   void setlp3();
   void setj1p4LV();
   void setj2p4LV();
   void setjjp3();
   void setMETp4LV();
   void setDRlj1();
   void setDRlj2();
   void setThetalj1pj2();
   void saveGenPart();
   bool matchGenParticles(const reco::Candidate* p1, const reco::Candidate* p2);
   /// matches jets to generator level objects
   pair<int, TLorentzVector> matchToGen(double eta, double phi);
   /// increments the specified counters: either El&Lp or Mu&Lp depending on whether MuPresent or ElPresent = 1 (the other counter should = 0 )
   void incrementCounter(int nCut, int nJets, int ElPass[NCUTS][NJETS], int MuPass[NCUTS][NJETS],
                         int LpPass[NCUTS][NJETS], int MuPresent, int ElPresent);
   void printEventInformation(bool print, int cLevel, bool muon);
   void printJetInformation(); vector<string> jstreams; stringstream jstream;
   void printLeptonInformation(); vector<string> lstreams; stringstream lstream;

private:
   //
   // member data
   //
   // program variables
   bool Data;
   bool saveGenParticles;
   bool noMETCut;
   bool invertEID;
   bool PFlowLoose;
   bool elONLY;
   bool muONLY;
   int SQWaT_Version;
   bool doRelIso;
   bool printEventInfo;
   bool printJetInfo;
   bool printLeptonInfo;
   // file variables
   TString outtablefilename;
   ofstream outtablefile;
   // table variables
   Table* tableEl;
   Table* tableMu;
   Table* tableLp;
   TString outtablenameEl;
   TString outtablenameMu;
   TString outtablenameLp;
   TString runtype;
   int PassEl[NCUTS][NJETS];
   int PassMu[NCUTS][NJETS];
   int PassLp[NCUTS][NJETS];
   // tree variables
   TTree* EvtTree;
   EventNtuple* EvtNtuple;
   // handle InputTags/sources
   edm::InputTag triggerSource;
   edm::InputTag vtxSource;
   edm::InputTag genParticleSource;
   edm::InputTag pfJetSource;
   edm::InputTag electronSource;
   edm::InputTag muonSource;
   edm::InputTag METSource;
   edm::InputTag rhoSource;
   // handles
   edm::Handle<pat::TriggerEvent> triggerHandle;
   edm::Handle<reco::VertexCollection> vtxHandle;
   edm::Handle<reco::GenParticleCollection> genParticleHandle;
   edm::Handle<vector<pat::Jet> > pfJetHandle;
   edm::Handle<vector<pat::Electron> > electronHandle;
   edm::Handle<vector<pat::Muon> > muonHandle;
   edm::Handle<vector<pat::MET> > METHandle;
   edm::Handle<double> rhoHandle;
   // trigger variables
   bool mu_passTrigger;
   bool el_passTrigger;
   string muTrigger;
   string eleTrigger;
   //bool cutOnTrigger;
   bool passTrigger;
   //bool passTrigger;
   map<string,bool> triggerMap;
   // event variables
   long runNumber;
   long eventNumber;
   long bxNumber;
   long orbitNumber;    
   long storeNumber; 
   long timeNumber;  
   long lumiSegmentNumber;
   int elcnt_Prim;
   int elcnt_Loose;
   int mucnt_Prim;
   int mucnt_Loose;
   int jcnt_tot;
   int jNEntries;
   int EvtTotCount;

   bool mu_passAll;
   bool mu_passStandard;
   bool mu_passFlag;
   bool el_passAll;
   bool el_passStandard;
   bool el_passFlag;
   // vertex variables
   reco::Vertex pv;
   bool PVfound;
   double zvtx;
   int vtxcnt;
   // jet variables
   vector <math::XYZTLorentzVector> jp4, jp4_temp;
   vector <double> JEta, JEta_temp, JPhi, JPhi_temp;
   double j_pt;
   double j_ptMin;
   double j_eta;
   double j_aetaMax;
   double j_phi;
   double j_DRlepton;
   double j_DRelMin;
   double muPrim_DRjMin;
   double adphi;
   vector<double> JChargedMultiplicity, JNeutralMultiplicity, JPtD;
   
   // b-tag variables
   vector <int> jBtagSSV, jBtagTC;
   int nBtagSSV, nBtagTC;
   vector <double> jBtagDiscriminatorSSV, jBtagDiscriminatorTC;
   double bDiscriminatorSSVMin, bDiscriminatorTCMin;

   // muon variables
   // Primary Muons (used in muon selection)
   double mu_pt;
   double muPrim_ptMin;
   double mu_eta;
   double mu_phi;
   
   double muPrim_aetaMax;
   double muPrim_itNHits;
   double muPrim_dBMax;
   double mu_RelIso;
   double mu_TrkIso;
   double muPrim_RelIsoMax;
   double muPrim_TrkIsoMax;

   bool muisGmTm;
   bool muisTightPrompt;
   bool mu_vtxPass;
   bool isProperlyIsolatedMu;

   // Loose Muons (used in both electron and muon selection)
   double muLoose_ptMin;
   double muLoose_aetaMax;
   double muLoose_RelIsoMax;
   double muLoose_TrkIsoMax;
   // electron variables
   // Primary Electrons (used in electron selection):
   double el_pt;
   double elPrim_ptMin;
   double el_eta;
   double elPrim_aetaMax;
   double el_phi;
   double el_aetasc;
   double elPrim_aetascExcludeMax;
   double elPrim_aetascExcludeMin;
   double elPrim_dBMax;
   double el_RelIso;
   double el_TrkIso;
   double elPrim_RelIsoMax;
   double elPrim_TrkIsoMax;

   // Equivalent to the EID cut (https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID)
   int el_PassEIDcnt;
   bool el_PassEID;
   double el_sigmaIetaIeta;
   double el_aDeltaPhi;
   double el_aDeltaEta;
   double el_HoE;
   double elPrim_sigmaIetaIetaMaxEB;
   double elPrim_aDeltaPhiMaxEB;
   double elPrim_aDeltaEtaMaxEB;
   double elPrim_HoEMaxEB;
   double elPrim_sigmaIetaIetaMaxEE;
   double elPrim_aDeltaPhiMaxEE;
   double elPrim_aDeltaEtaMaxEE;
   double elPrim_HoEMaxEE;

   // Loose Electrons
   double elLoose_ptMin;
   double elLoose_RelIsoMax;
   double elLoose_TrkIsoMax;
   double elLoose_sigmaIetaIetaMaxEB;
   double elLoose_aDeltaPhiMaxEB;
   double elLoose_aDeltaEtaMaxEB;
   double elLoose_HoEMaxEB;
   double elLoose_sigmaIetaIetaMaxEE;
   double elLoose_aDeltaPhiMaxEE;
   double elLoose_aDeltaEtaMaxEE;
   double elLoose_HoEMaxEE;

   //
   // Electron Flags
   //
   bool el_aetaPass;
   bool el_convPass_a;
   bool el_convPass_b;
   // lepton variables
   //double l_Eta; //Modified for synchronization 03/26/2012
   //double l_Phi;
   vector<double> l_Eta; //Added for synchronization 03/26/2012
   vector<double> l_Phi;
   vector<int> l_Type; //0 for electrons, 1 for muons
   // MET variables
   double MET_Et;
   double MET_EtMin;
   // additional variables
   math::XYZTLorentzVector lp4, METp4;
   TLorentzVector lp4LV, j1p4LV, j2p4LV, METp4LV;
   TVector3 lp3, jjp3;
   double Thetalj1pj2, DRlj1, DRlj2;
   double lTotIso;
   double lecalIso;
   double lhcalIso;
   double ltrkIso;
   double lrelIso;
   double Mjj;
   int lQ;
   double lEta;
   //constants
   double etaBarrelMax;
};
