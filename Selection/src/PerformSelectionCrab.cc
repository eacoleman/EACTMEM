////////////////////////////////////////////////////////////////////////////////
//
// PerformSelectionCrab
// --------------------
//
//                         01/27/2012 Ilya Osipenkov <ilyao@fnal.gov> 
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

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/UserData.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/deltaR.h"

#include <Math/VectorUtil.h>

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexSorter.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
//#include "PhysicsTools/FWLite/interface/EventContainer.h"
//#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 

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
#include <strstream>
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
class PerformSelection : public edm::EDAnalyzer
{
public:
   // construction/destruction
   explicit PerformSelection(const edm::ParameterSet& iConfig);
   virtual ~PerformSelection();
   
private:
   // member functions
   void beginJob();
   void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
   void endJob();
   // event functions
   void setFlags();
   // vertex functions
   void vertexSelection();
   // jet functions
   void jetSelection();
   void computeInvariantMass();
   // b-tag functions
   // muon functions
   void muonSelection();
   // electron functions
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
   /// matches jets to generator level objects
   pair<int, TLorentzVector> matchToGen(double eta, double phi);
   /// increments the specified counters: either El&Lp or Mu&Lp depending on whether MuPresent or ElPresent = 1 (the other counter should = 0 )
   void incrementCounter(int nCut, int nJets, int ElPass[NCUTS][NJETS], int MuPass[NCUTS][NJETS],
                         int LpPass[NCUTS][NJETS], int MuPresent, int ElPresent);

private:
   //
   // member data
   //
   // file variables
   TString modename;
   TString outtablefilename;
   ofstream outtablefile;
   // table variables
   Table* table;
   TString outtablename;
   TString runtype;
   // tree variables
   TTree* EvtTree;
   EventNtuple* EvtNtuple;
   // handle InputTags/sources
   edm::InputTag vtxSource;
   edm::InputTag genParticleSource;
   edm::InputTag pfJetSource;
   edm::InputTag electronSource;
   edm::InputTag muonSource;
   edm::InputTag METSource;
   // handles
   edm::Handle<reco::VertexCollection> vtxHandle;
   edm::Handle<reco::GenParticleCollection> genParticleHandle;
   edm::Handle<vector<pat::Jet> > pfJetHandle;
   edm::Handle<vector<pat::Electron> > electronHandle;
   edm::Handle<vector<pat::Muon> > muonHandle;
   edm::Handle<vector<pat::MET> > METHandle;
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
   // b-tag variables
   vector <int> jBtag, jBtag_temp;
   int nBtag;
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
   double muPrim_RelIsoMax;

   bool muisGmTm;
   bool muisTightPrompt;
   bool mu_vtxPass;
   bool isProperlyIsolatedMu;

   // Loose Muons (used in both electron and muon selection)
   double muLoose_ptMin;
   double muLoose_aetaMax;
   double muLoose_RelIsoMax;
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
   double elPrim_RelIsoMax;

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
   double l_Eta;
   double l_Phi;
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
   double Mjj;
   int lQ;
   double lEta;
   // global counters
   int vtxcnt;
   int EvtTotCount;
   int PassEl[NCUTS][NJETS];
   int PassMu[NCUTS][NJETS];
   int PassLp[NCUTS][NJETS];
};

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
PerformSelection::PerformSelection(const edm::ParameterSet& iConfig)
   // event variables
   : elcnt_Prim(0)
   , elcnt_Loose(0)
   , mucnt_Prim(0)
   , mucnt_Loose(0)
   , jcnt_tot(0)
   , mu_passAll(false)
   , mu_passStandard(false)
   , mu_passFlag(false)
   , el_passAll(false)
   , el_passStandard(false)
   , el_passFlag(false)
     // vertex variables
   , PVfound(false)
   , zvtx(-999)
     // jet variables
   , j_ptMin(20.0)
   , j_aetaMax(2.4)
   , j_DRelMin(0.3)
   , muPrim_DRjMin(0.1)
     // b-tag variables
   , nBtag(0)
     // muon variables
   , muPrim_ptMin(20.0)
   , muPrim_aetaMax(2.1)
   , muPrim_dBMax(0.02)
   , muPrim_RelIsoMax(0.05)
   , isProperlyIsolatedMu(false)
   , muLoose_ptMin(10.0)
   , muLoose_aetaMax(2.5)
   , muLoose_RelIsoMax(0.2)
     // electron variables
   , elPrim_ptMin(20.0)
   , elPrim_aetaMax(2.5)
   , elPrim_aetascExcludeMax(1.5660)
   , elPrim_aetascExcludeMin(1.4442)
   , elPrim_dBMax(0.02)
   , elPrim_RelIsoMax(0.1)
   
   , elPrim_sigmaIetaIetaMaxEB(0.01)
   , elPrim_aDeltaPhiMaxEB(0.06)
   , elPrim_aDeltaEtaMaxEB(0.004)
   , elPrim_HoEMaxEB(0.040)
   , elPrim_sigmaIetaIetaMaxEE(0.03)
   , elPrim_aDeltaPhiMaxEE(0.03)
   , elPrim_aDeltaEtaMaxEE(0.007)
   , elPrim_HoEMaxEE(0.025)
    
   , elLoose_ptMin(15.0)
   , elLoose_RelIsoMax(0.2)
   , elLoose_sigmaIetaIetaMaxEB(0.01)
   , elLoose_aDeltaPhiMaxEB(0.8)
   , elLoose_aDeltaEtaMaxEB(0.007)
   , elLoose_HoEMaxEB(0.15)
   , elLoose_sigmaIetaIetaMaxEE(0.03)
   , elLoose_aDeltaPhiMaxEE(0.7)
   , elLoose_aDeltaEtaMaxEE(0.01)
   , elLoose_HoEMaxEE(0.07)
   
   , el_aetaPass(false)
   , el_convPass_a(false)
   , el_convPass_b(false)
     // lepton variables
   , l_Eta(-1)
   , l_Phi(-1)
     // MET variables
   , MET_EtMin(25)
     // additional variables
   , lTotIso(-1)
   , lecalIso(-10)
   , lhcalIso(-10)
   , ltrkIso(-1)
   , Mjj(-1)
   , lQ(-10)
   , lEta(-1)
     // global counters
   , vtxcnt(0)
   , EvtTotCount(0)
{
   //-----Input Tags For Handles
   vtxSource         =iConfig.getParameter<edm::InputTag> ("vtxSource");
   genParticleSource =iConfig.getParameter<edm::InputTag> ("genParticleSource");
   pfJetSource       =iConfig.getParameter<edm::InputTag> ("pfJetSource");
   electronSource    =iConfig.getParameter<edm::InputTag> ("electronSource");
   muonSource        =iConfig.getParameter<edm::InputTag> ("muonSource");
   METSource         =iConfig.getParameter<edm::InputTag> ("METSource");

   //-----Program Level Inputs
   outtablefilename  = TString(iConfig.getParameter<string> ("outtablefilename"));
   outtablename      = TString(iConfig.getParameter<string> ("outtablename"));
   runtype           = TString(iConfig.getParameter<string> ("runtype"));
   modename          = TString(iConfig.getParameter<string> ("modename"));

   InitializeIntMatrix(PassEl);
   InitializeIntMatrix(PassMu);
   InitializeIntMatrix(PassLp);

} //COMPLETED


//______________________________________________________________________________
PerformSelection::~PerformSelection()
{

} //COMPLETED


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::beginJob()
{
   edm::Service<TFileService> fs;
   if (!fs) throw edm::Exception(edm::errors::Configuration,
                                 "TFileService missing from configuration!");

   // create the tree
   table = fs->make<Table>(string(outtablename));
   EvtTree = fs->make<TTree>("EvtTree", "Output tree for matrix element");
   EvtNtuple = new EventNtuple();
   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
} //COMPLETED


//______________________________________________________________________________
void PerformSelection::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   
   //
   //find the runNumber and eventNumber here
   //
   eventNumber       = iEvent.id().event();
   runNumber         = iEvent.id().run();
   bxNumber          = iEvent.eventAuxiliary().bunchCrossing();
   orbitNumber       = iEvent.eventAuxiliary().orbitNumber();
   storeNumber       = iEvent.eventAuxiliary().storeNumber();
   timeNumber        = iEvent.eventAuxiliary().time().value();
   lumiSegmentNumber = iEvent.eventAuxiliary().luminosityBlock();
      
   //
   // Create a set containing all the run and event numbers
   // so we can keep track of duplicate events.
   //
   RunEventSet runEventSet;
   //
   //Check and remove duplicates here
   //
   if ( runEventSet.alreadySeen(runNumber, eventNumber)) {
      cout << "WARNING, event Run = " << runNumber << ", Event = " << eventNumber
           <<" is duplicated" << endl << "We will skip this event." << endl;
      return;
   }
      
   //
   // Load the objects of interest
   //
   getCollections(iEvent, iSetup);

   //
   //Perform The Selection
   //
      
   //
   // Vertex Selection
   //
   vertexSelection();

   //
   // Electron Selection
   //
   elcnt_Prim=0;
   elcnt_Loose=0;
   el_convPass_a=false;
   el_convPass_b=false;
   eleSelection();

   //
   // Muon Selection (No Jet Cleaning)
   //
   mucnt_Prim=0;
   mucnt_Loose=0;
   muonSelection();

      
   //
   // Jet Selection (without muon cleaning)
   // Electron Cleaning (i.e. remove the jets which are likely to be electorns using the DR cut).
   // Using PF Jets
   //
   jcnt_tot=0;
   j_DRlepton=100;
   Mjj=-1;
     
   jNEntries=0;
   jp4.clear();
   JEta.clear();
   JPhi.clear();
   nBtag=0;
   jBtag.clear();
   /*
   //
   // JES Uncertainty : tested & works
   //
   double uncer;
   j_pt=25.5;
   j_eta=1.3;
   cout << "JES UNCERTAINTY" << endl;
   string JEC_PATH("CondFormats/JetMETObjects/data/");
   //edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5Calo.txt");
   edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5PF.txt");
   // edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5JPT.txt");
   JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(fip.fullPath());
   jecUnc->setJetEta(j_eta); // Give rapidity of jet you want uncertainty on
   jecUnc->setJetPt(j_pt);// Also give the corrected pt of the jet you want the uncertainty on
   //
   // The following function gives the relative uncertainty in the jet Pt.
   // i.e. ptCorSmeared = (1 +- uncer) * ptCor  
   //
   uncer = jecUnc->getUncertainty(true); // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.
   cout << "pt=" << j_pt << " eta=" << j_eta << " : uncer=" << uncer << endl;
   */
   jetSelection();
     
   //
   // Compute the invariant mass, when only two jets are present
   //
   computeInvariantMass();

   //
   // MET Selection
   //
   metSelection();
     
   //
   // Record The Results
   //
      
   //
   // Record the number of events passing the cuts for each jet count
   //
   el_passStandard=false;
   el_passAll=false;
   mu_passStandard=false;
   mu_passAll=false;


   for(Int_t Nj=0; Nj<NJETS;Nj++)
   {
      if(jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) 
      {
         PassLp[1][Nj]++;
         PassMu[1][Nj]++;
         PassEl[1][Nj]++;
	  
         //
         // Record the electron info
         //
         if(elcnt_Prim>=1 && PVfound)
         {
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
            if(mucnt_Loose==0)
            {
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
               if(elcnt_Loose==1)
               {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                  el_passStandard=true;
                  if(MET_Et>MET_EtMin)
                  {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     el_passAll=true;
                  }
                  //
                  // Record the Btag info
                  //
                  if(nBtag==0)
                     incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                  else
                  {
                     if(nBtag==1)
                        incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     else
                     {
                        if(nBtag==2)
                           incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                        else
                           incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     }
                  }//BTags
               }
            }
         }//electrons
	 
         //
         // Record the muon info
         //
         if(mucnt_Prim>=1 && PVfound)
         {
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
            if(mucnt_Loose==1)
            {
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
               if(elcnt_Loose==0)
               {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                  mu_passStandard=true;
                  if(MET_Et>MET_EtMin)
                  {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     mu_passAll=true;
                  }
                  //
                  // Record the Btag info
                  //
                  if (nBtag==0 )
                     incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                  else 
                  {
                     if(nBtag==1) 
                        incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     else 
                     {
                        if(nBtag==2)
                           incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                        else
                           incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     }
                  }//BTags
               }
            }
         } //if(mucnt_Prim>=1 && PVfound)
      } //if(jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) 
   } //for(Int_t Nj=0; Nj<NJETS;Nj++)

   //
   // Fill the Ntuple to be used in Matrix Element computations
   //    (requiring two jets & a lepton passing all of the cuts)
   // Usual option: fill in the elements which pass all of the cuts, *except* the MET cut
   //
   setFlags();

   if((el_passFlag || mu_passFlag) && jcnt_tot==2)
   {
      setlp4LV();
      setlp3();
      EvtNtuple->lLV.clear();
      EvtNtuple->lLV.push_back(lp4LV);

      setj1p4LV();
      setj2p4LV();
      setjjp3();
      EvtNtuple->jLV.clear();
      EvtNtuple->jLV.push_back(j1p4LV);
      EvtNtuple->jLV.push_back(j2p4LV);

      setMETp4LV();
      EvtNtuple->METLV.clear();
      EvtNtuple->METLV.push_back(METp4LV);

      EvtNtuple->lPhi=lp4.Phi();
      EvtNtuple->METEt=METp4LV.Et();


      EvtNtuple->jBtag.clear();
      EvtNtuple->jBtag.push_back(jBtag[0]);
      EvtNtuple->jBtag.push_back(jBtag[1]);
      EvtNtuple->lQ=lQ;
      //
      // the Maximum value of eta for the Barrel//**fix when needed
      //
      double etaBarrelMax=1.442;
      if (abs(lEta)<etaBarrelMax)
      {
         //
         // barrel
         //
         EvtNtuple->ldetComp=0;
      } 
      else
      {
         //
         // endcap
         //
         EvtNtuple->ldetComp=1;
      }
      EvtNtuple->run=runNumber;
      EvtNtuple->event=eventNumber;
       
      //
      // Transfer Function Variables
      //
      EvtNtuple->matchedGenParticles.clear();
      EvtNtuple->matchedpdgId.clear();
      EvtNtuple->matchedDeltaR.clear();
      //
      // Do the matching between the pat::Jets and the reco::genParticles
      //
      pair<int, TLorentzVector> matchedPair;
      if(!modename.Contains("Data"))
      {
         matchedPair = matchToGen(j1p4LV.Eta(),j1p4LV.Phi());
         EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
         EvtNtuple->matchedpdgId.push_back(matchedPair.first);
         EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j1p4LV.Eta(), j1p4LV.Phi()));
         matchedPair = matchToGen(j2p4LV.Eta(),j2p4LV.Phi());
         EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
         EvtNtuple->matchedpdgId.push_back(matchedPair.first);
         EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j2p4LV.Eta(), j2p4LV.Phi()));
      }

      //
      // Additional Variables
      //
      EvtNtuple->lTotIso=lTotIso;
      EvtNtuple->lecalIso=lecalIso;
      EvtNtuple->lhcalIso=lhcalIso;
      EvtNtuple->ltrkIso=ltrkIso;
      EvtNtuple->Mjj=Mjj;
      EvtNtuple->leptonCat=DEFS::none;
      EvtNtuple->leptonCat_passAll=DEFS::none;
      if(el_passFlag)
         EvtNtuple->leptonCat=DEFS::electron;
      if(mu_passFlag)
         EvtNtuple->leptonCat=DEFS::muon;
      if(el_passAll)
         EvtNtuple->leptonCat_passAll=DEFS::electron;
      if(mu_passAll)
         EvtNtuple->leptonCat_passAll=DEFS::muon;

      setDRlj1();
      setDRlj2();
      setThetalj1pj2();
       
      EvtNtuple->DRlj1=DRlj1;
      EvtNtuple->DRlj2=DRlj2;
      EvtNtuple->Thetalj1pj2=Thetalj1pj2;

      //
      // Fill The Tree
      //
      EvtTree->Fill();
   } //if((el_passFlag || mu_passFlag) && jcnt_tot==2)

   EvtTotCount++;

   return;
} //COMPLETE


//______________________________________________________________________________
void PerformSelection::endJob()
{
   for (Int_t Nj=0; Nj<NJETS;Nj++)
   {
      PassLp[0][Nj]=EvtTotCount;
      PassMu[0][Nj]=EvtTotCount;
      PassEl[0][Nj]=EvtTotCount;
   }
   InitializeLabels(PLabel,CLabel);
   
   getProcessTable(table,PassEl,PassMu,PassLp);
   if(runtype.CompareTo("cmsRun")==0)
   {
      //
      // won't work for CRAB
      // only use if running using cmsRun
      //
      outtablefile.open(outtablefilename,ios::out);
      writeIntProcessTable(outtablefile,PassEl,PassMu,PassLp);
   }
} //COMPLETE

////////////////////////////////////////////////////////////////////////////////
// implement event functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::setFlags()
{
   el_passFlag=el_passAll;
   mu_passFlag=mu_passAll;
   if(modename.Contains("noMETcut"))
   {
      el_passFlag=el_passStandard;
      mu_passFlag=mu_passStandard;
   }
   if(modename.Contains("elONLY"))
      mu_passFlag=false;
   if(modename.Contains("muONLY"))
      el_passFlag=false;
}


////////////////////////////////////////////////////////////////////////////////
// implement vertex functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::vertexSelection()
{
   PVfound = (vtxHandle->begin() != vtxHandle->end());
   zvtx = -999;
   if(PVfound)
   {
      pv = (*vtxHandle->begin());
      vtxcnt++;
      zvtx = pv.z();
   }
   else
   {
      throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event."
                                           << std::endl;
   }
} //RE-WRITTEN SLIGHTLY


////////////////////////////////////////////////////////////////////////////////
// implement jet functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::jetSelection()
{
   //
   // Define the constants
   //
   double pi_=TMath::Pi();
   //
   // corresponds to the 'medium' cut (per https://twiki.cern.ch/twiki/bin/view/CMS/BTV10001)
   //
   double bDiscriminator_min=1.74;

   const vector< pat::Jet >::const_iterator pfJetEnd = pfJetHandle->end();
   for(vector<pat::Jet>::const_iterator jetIter = pfJetHandle->begin(); pfJetEnd != jetIter; ++jetIter)
   { 
      j_pt=jetIter->pt();
      j_eta=jetIter->eta();
      j_phi=jetIter->phi();

      adphi=abs(l_Phi-j_phi);

      //
      // we're on a circle
      //
      if(adphi>pi_)
         adphi=2*pi_-adphi;

      j_DRlepton=pow((j_eta-l_Eta)*(j_eta-l_Eta)+adphi*adphi,0.5);
       
      //
      // Apply The Cut
      //
      if(j_pt>j_ptMin && abs(j_eta)<j_aetaMax && 
         ((j_DRlepton>j_DRelMin && elcnt_Prim==1) || (j_DRlepton>muPrim_DRjMin && mucnt_Prim==1)) && 
         jetIter->neutralHadronEnergyFraction()<0.99 && jetIter->neutralEmEnergyFraction()<0.99 && 
         jetIter->numberOfDaughters()>1 && jetIter->chargedHadronEnergyFraction()>0 && 
         jetIter->chargedMultiplicity()>0 && jetIter->chargedEmEnergyFraction()<0.99)
      {
         jcnt_tot++;
	  
         //
         // store the parameters
         //
         jp4.push_back(jetIter->p4());
         JEta.push_back(j_eta); 
         JPhi.push_back(j_phi);
         jNEntries++;
	 
         //
         // b-tagging
         //
         if(jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags")>bDiscriminator_min)
         {
            jBtag.push_back(1);
            nBtag++;
         } 
         else
            jBtag.push_back(0);
      } //for(Selection Cuts)
   } //for(jetIter)
}

//______________________________________________________________________________
void PerformSelection::computeInvariantMass()
{
   if(jcnt_tot==2)
      Mjj=(jp4[0]+jp4[1]).M();
   else
      Mjj=-2;
}


////////////////////////////////////////////////////////////////////////////////
// implement b-tag functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// implement muon functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::muonSelection()
{
   const vector< pat::Muon >::const_iterator muEnd = muonHandle->end();
   for(vector<pat::Muon>::const_iterator muIter = muonHandle->begin(); muEnd != muIter; ++muIter)
   { 
      mu_pt=muIter->pt();
      mu_eta=muIter->eta();
      mu_phi=muIter->phi();
      mu_RelIso=(muIter->trackIso())/mu_pt;
      
      muisGmTm=false;
      if(muIter->isGlobalMuon() && muIter->isTrackerMuon())
         muisGmTm=true;

      muisTightPrompt=false;
      if(muIter->globalTrack().isNonnull())
      {
         if(muIter->globalTrack()->normalizedChi2()<10 &&
            muIter->globalTrack()->hitPattern().numberOfValidMuonHits()>0)
            muisTightPrompt=true;
      }

      if(muIter->innerTrack().isNonnull())
         muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
      else
         muPrim_itNHits=-1;
       
      mu_vtxPass=false;
      if(abs(zvtx - muIter->vertex().z())<1)
         mu_vtxPass=true;
       
      //
      // Apply The Primary Cuts
      //
      if(muisGmTm && mu_pt>muPrim_ptMin && abs(mu_eta)<muPrim_aetaMax && muisTightPrompt &&
         muPrim_itNHits>10 && muIter->dB()<muPrim_dBMax && mu_vtxPass && 
         muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>=1 &&
         muIter->numberOfMatches()>1)
      {
         isProperlyIsolatedMu=false;
         
         if(mu_RelIso<muPrim_RelIsoMax)
            isProperlyIsolatedMu=true;
            
         //
         // Record the muon quantities:
         //
         if(isProperlyIsolatedMu)
         {
            mucnt_Prim++;
            lp4=muIter->p4();
            lQ=muIter->charge();
            l_Eta=mu_eta;
            l_Phi=mu_phi;
            ltrkIso=mu_RelIso;
            lTotIso=mu_RelIso;
         }
      }

      //
      // Apply The Loose Cuts
      //
      if(muIter->isGlobalMuon() && mu_pt>muLoose_ptMin && abs(mu_eta)<muLoose_aetaMax &&
         mu_RelIso<muLoose_RelIsoMax)
         mucnt_Loose++;
   }//for muons
}


////////////////////////////////////////////////////////////////////////////////
// implement electron functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::eleSelection()
{
   const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
   for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); elEnd != elIter; ++elIter )
   { 
      el_pt=elIter->p4().Pt();
      el_eta=elIter->eta();
      el_phi=elIter->phi();
      el_aetasc=abs(elIter->superCluster()->eta());

      el_aetaPass=false;
      if(abs(el_eta)<elPrim_aetaMax)
         el_aetaPass=true; //noscv
      if(el_aetasc<elPrim_aetascExcludeMax && el_aetasc>elPrim_aetascExcludeMin)
         el_aetaPass=false;

      //
      // EID Cuts are applied directly
      //
      el_PassEIDcnt=-1;
      el_PassEID=false;
      el_sigmaIetaIeta=elIter->sigmaIetaIeta();
      el_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
      el_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
      el_HoE=elIter->hadronicOverEm();
      if(elIter->isEB())
      {
         el_PassEIDcnt=0;
         if(el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB)
            el_PassEIDcnt++;
         if(el_aDeltaPhi<elPrim_aDeltaPhiMaxEB)
            el_PassEIDcnt++;
         if(el_aDeltaEta<elPrim_aDeltaEtaMaxEB)
            el_PassEIDcnt++;
         if(el_HoE<elPrim_HoEMaxEB)
            el_PassEIDcnt++;
      }
      if(elIter->isEE())
      {
         el_PassEIDcnt=0;
         if(el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE)
            el_PassEIDcnt++;
         if(el_aDeltaPhi<elPrim_aDeltaPhiMaxEE)
            el_PassEIDcnt++;
         if(el_aDeltaEta<elPrim_aDeltaEtaMaxEE)
            el_PassEIDcnt++;
         if(el_HoE<elPrim_HoEMaxEE)
            el_PassEIDcnt++;
      }
      if(el_PassEIDcnt==4 ) el_PassEID=true;

      el_RelIso=(elIter->dr03TkSumPt())/el_pt;

      //
      // Apply The Primary Cuts
      //
      if(el_pt>elPrim_ptMin && el_aetaPass && el_RelIso<elPrim_RelIsoMax && el_PassEID  &&
         abs(elIter->dB())<elPrim_dBMax &&
         elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()==0 &&
         !((abs(elIter->convDist())<0.02)&&(abs(elIter->convDcot())<0.02)) ) 
      {
         elcnt_Prim++;
         lp4=elIter->p4();
         lQ=elIter->charge();
         l_Eta=el_eta;
         l_Phi=el_phi;
         ltrkIso=el_RelIso;
         lTotIso=el_RelIso;
      }

      //
      // Apply The Loose Cuts
      //
      el_PassEIDcnt=-1;
      el_PassEID=false;
      if( elIter->isEB())
      {
         el_PassEIDcnt=0;
         if(el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEB)
            el_PassEIDcnt++;
         if(el_aDeltaPhi<elLoose_aDeltaPhiMaxEB)
            el_PassEIDcnt++;
         if(el_aDeltaEta<elLoose_aDeltaEtaMaxEB)
            el_PassEIDcnt++;
         if(el_HoE<elLoose_HoEMaxEB)
            el_PassEIDcnt++;
      }

      if(elIter->isEE())
      {
         el_PassEIDcnt=0;
         if(el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEE)
            el_PassEIDcnt++;
         if(el_aDeltaPhi<elLoose_aDeltaPhiMaxEE)
            el_PassEIDcnt++;
         if(el_aDeltaEta<elLoose_aDeltaEtaMaxEE)
            el_PassEIDcnt++;
         if(el_HoE<elLoose_HoEMaxEE)
            el_PassEIDcnt++;
      }
      if(el_PassEIDcnt==4) el_PassEID=true;

      if(el_pt>elLoose_ptMin && el_aetaPass && el_RelIso<elLoose_RelIsoMax && el_PassEID) 
         elcnt_Loose++;
   }
}


////////////////////////////////////////////////////////////////////////////////
// implement lepton functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// implement MET functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::metSelection()
{
   METp4=METHandle->front().p4();
   MET_Et=METp4.E();
}


////////////////////////////////////////////////////////////////////////////////
// implement additional functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByLabel(vtxSource, vtxHandle);
   assert ( vtxHandle.isValid() ); 

   if ( !modename.Contains("Data") ) {
      iEvent.getByLabel(genParticleSource, genParticleHandle);
      assert ( genParticleHandle.isValid() );
   }

   iEvent.getByLabel(pfJetSource, pfJetHandle);
   assert ( pfJetHandle.isValid() );

   iEvent.getByLabel(electronSource, electronHandle);
   assert ( electronHandle.isValid() );
 
   iEvent.getByLabel(muonSource, muonHandle);
   assert ( muonHandle.isValid() );

   iEvent.getByLabel(METSource, METHandle);
   assert ( METHandle.isValid() );
}


//______________________________________________________________________________
void PerformSelection::setlp4LV()
{
   lp4LV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
}

//______________________________________________________________________________
void PerformSelection::setlp3()
{
   lp3.SetXYZ(lp4.Px(),lp4.Py(),lp4.Pz());
}

//______________________________________________________________________________
void PerformSelection::setj1p4LV()
{
   j1p4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
}

//______________________________________________________________________________
void PerformSelection::setj2p4LV()
{
   j2p4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
}

//______________________________________________________________________________
void PerformSelection::setjjp3()
{
   jjp3.SetXYZ(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz());
}

//______________________________________________________________________________
void PerformSelection::setMETp4LV()
{
   METp4LV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
}

//______________________________________________________________________________
void PerformSelection::setDRlj1()
{
   DRlj1=lp4LV.DeltaR(j1p4LV);
}

//______________________________________________________________________________
void PerformSelection::setDRlj2()
{
   DRlj2=lp4LV.DeltaR(j2p4LV);
}

//______________________________________________________________________________
void PerformSelection::setThetalj1pj2()
{
   Thetalj1pj2=lp3.Dot(jjp3)/(lp3.Mag()*jjp3.Mag());
}

//______________________________________________________________________________
pair<int, TLorentzVector> PerformSelection::matchToGen(double eta, double phi)
{
   double bestDeltaR = 9999;
   int bestPDGID = 0;
   TLorentzVector theGenObject(0,0,0,0);
   TLorentzVector theBestGenObject(0,0,0,0);
   for(reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
      if(genParticle->pt()==0)
      {
         continue;
      }
      if(genParticle->status() == 3)
      {
         theGenObject.SetPxPyPzE(genParticle->px(),genParticle->py(),genParticle->pz(),genParticle->energy());
         if(reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi) < bestDeltaR)
         {
            bestDeltaR = reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi);
            theBestGenObject = theGenObject;
            bestPDGID = genParticle->pdgId();
         }
      }
   }
   pair<int, TLorentzVector> GenMatchedInformation(bestPDGID,theBestGenObject);
   return GenMatchedInformation;
}

//______________________________________________________________________________
void PerformSelection::incrementCounter(int nCut, int nJets, int ElPass[NCUTS][NJETS],
                                        int MuPass[NCUTS][NJETS], int LpPass[NCUTS][NJETS], 
                                        int MuPresent, int ElPresent)
{
   if ( (MuPresent==1)&&(ElPresent==0) ) {
      MuPass[nCut][nJets]++;
      LpPass[nCut][nJets]++;
   }
   if ( (MuPresent==0)&&(ElPresent==1) ) {
      ElPass[nCut][nJets]++;
      LpPass[nCut][nJets]++;
   }
} //COMPLETED


////////////////////////////////////////////////////////////////////////////////
// define PerformSelection as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(PerformSelection); //COMPLETED
