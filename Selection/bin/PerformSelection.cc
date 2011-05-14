/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Stage 2 Skimming of the PAT-tuples to be used in Matrix Element calculations.
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Designed to work with 3_8_7 PAT-tuples and tested on V4 l+j sync exercises.
//// Use to skim the PATNtuples and create a custom made .root file containing the needed information.
//// Specialized mode names: 
//// - noMETcut - disregard the MET cut when storing the events.
//// - invertEID - electrons with *fail* at least two of the EID requirements will be kept instead.
//// - PFlowLoose - use the collections with the PF2PAT postfix=PFlowLoose (instead of PFlow).
//// - elONLY - only save the output if the lepton is an electron.
//// - muONLY - only save the output if the lepton is a muon.

// CMS includes
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/UserData.h"


#include "PhysicsTools/FWLite/interface/EventContainer.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 

//User Defined Ntuple
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

//User Defined Includes
#include "RunEventSet.h"

// Root includes
#include "TROOT.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <iostream>
#include <strstream>
#include <fstream>

using namespace std;
//using namespace reco;
#include <TAMUWW/Tools/GlobalTools.cc>

///////////////////////////
///// Custom Functions/////
///////////////////////////
void incrementCounter(int nCut, int nJets, int ElPass[NCUTS][NJETS], int MuPass[NCUTS][NJETS], int LpPass[NCUTS][NJETS], int MuPresent, int ElPresent ) {
//// increments the specified counters: either El&Lp or Mu&Lp depending on whether MuPresent or ElPresent = 1 (the other counter should = 0 ).
  if ( (MuPresent==1)&&(ElPresent==0) ) {
    MuPass[nCut][nJets]++;
    LpPass[nCut][nJets]++;
  }
  if ( (MuPresent==0)&&(ElPresent==1) ) {
    ElPass[nCut][nJets]++;
    LpPass[nCut][nJets]++;
  }
}

//-----Matching Jets to generator level objects
//pair<int, reco::Candidate::LorentzVector> matchToGen(const pat::Jet& theObject, fwlite::Handle< vector< reco::GenParticleCollection > > genParticleHandle) {
//pair<int, TLorentzVector> matchToGen(const pat::Jet& theObject, fwlite::Handle<GenParticleCollection> genParticleHandle) {
pair<int, TLorentzVector> matchToGen(double eta, double phi, fwlite::Handle<reco::GenParticleCollection> genParticleHandle) {
  double bestDeltaR = 9999;
  int bestPDGID = 0;
  TLorentzVector theGenObject(0,0,0,0);
  TLorentzVector theBestGenObject(0,0,0,0);
  for(reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
     if(genParticle->pt()==0)
     {
        if(genParticle->pdgId()!=2212) cout << "matchToGen::The current genParticle is a " << genParticle->pdgId() << " with a pt of 0" << endl;
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

// //-----Matching Jets to generator level objects
// //pair<int, reco::Candidate::LorentzVector> matchToGen(const pat::Jet& theObject, fwlite::Handle< vector< reco::GenParticleCollection > > genParticleHandle) {
// //pair<int, TLorentzVector> matchToGen(const pat::Jet& theObject, fwlite::Handle<GenParticleCollection> genParticleHandle) {
// pair<int, TLorentzVector> matchToGen(double eta, double phi, fwlite::Handle<reco::GenParticleCollection> genParticleHandle) {
//   double bestDeltaR = 9999;
//   int bestPDGID = 0;
//   //reco::Candidate::LorentzVector theGenObject(0,0,0,0);
//   //reco::Candidate::LorentzVector theBestGenObject(0,0,0,0);
//   TLorentzVector theGenObject(0,0,0,0);
//   TLorentzVector theBestGenObject(0,0,0,0);
//   //for(vector< reco::GenParticleCollection >::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
//   for(reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
//     if(genParticle->status() == 2)
//       {
//         theGenObject.SetPxPyPzE(genParticle->px(),genParticle->py(),genParticle->pz(),genParticle->energy());
//         // theGenObject = genParticle->p4();
//         if(reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi) < bestDeltaR)
//           //if(reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), theObject.eta(), theObject.phi()) < bestDeltaR)
//           //if(reco::deltaR(theGenObject.eta(), theGenObject.phi(), theObject.eta(), theObject.phi()) < bestDeltaR)
//           { 
//             bestDeltaR = reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi);
//             //bestDeltaR = reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), theObject.eta(), theObject.phi());
//             theBestGenObject = theGenObject;
//             bestPDGID = genParticle->pdgId();
//           }
//       }
//   }
//   //pair<int, reco::Candidate::LorentzVector> GenMatchedInformation(bestPDGID,theBestGenObject);
//   pair<int, TLorentzVector> GenMatchedInformation(bestPDGID,theBestGenObject);
//   return GenMatchedInformation;
// }

///////////////////////////
// ///////////////////// //
// // Main Subroutine // //
// ///////////////////// //
///////////////////////////

int main (int argc, char* argv[]) 
{
  ////////////////////////////////
  // ////////////////////////// //
  // // Command Line Options // //
  // ////////////////////////// //
  ////////////////////////////////
  
  // Tell people what this analysis code does and setup default options.
  optutl::CommandLineParser parser ("Plots Jet Pt");//**fix???
   
  ////////////////////////////////////////////////
  // Change any defaults or add any new command //
  //      line options you would like here.     //
  ////////////////////////////////////////////////
  
  //// Output Files:
  // corresponds to the last string on the command line
  TString modename;
  modename=argv[argc-1];
  TString outtablefilename;
  outtablefilename=argv[argc-2];
  ofstream outtablefile;
  outtablefile.open(outtablefilename,ios::out);
  // corresponds to the next to last string on the command line
  TString outrootfilename;
  outrootfilename=argv[argc-3];
  TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
  // adjust argc to account for inputing the two character strings above
  argc=argc-3;
  
  // create the tree
  TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
  EventNtuple * EvtNtuple = new EventNtuple();
  EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);

  //// Define the constants
  double pi_=TMath::Pi();


  //////////////////////// Selection Parameters and Cut Values ////////////////////////////////
  //// Vertex:
  int vtx_ndofMin=4;
  double vtx_zMax=24.0;
  double vtx_RhoMax=2.0;

  //// Jets (apply after the Primary Electrons but before the Primary Muons):
  vector <math::XYZTLorentzVector> jp4, jp4_temp;
  vector <double> JEta, JEta_temp, JPhi, JPhi_temp;
  double j_pt;
  double j_ptMin=30.0;
  double j_eta;
  double j_aetaMax=2.4;
  double j_DRel;
  double j_DRelMin=0.3;
  double adphi;

  ///Loose JetID Cuts (https://twiki.cern.ch/twiki/bin/view/CMS/JetID)
//   double jcalo_emFracMin=0.01;
//   double jcalo_n90HitsMin=1.01;
//   double jcalo_fHPDMax=0.98;
  double jpf_neutralHadronEFMax=0.99;
  double jpf_neutralEmEFMax=0.99;
  double jpf_nConstituentsMin=1.5;
  double jpf_chargedHadronEFMin=0.0;
  double jpf_chargedMultiplicityMin=0.0;
  double jpf_chargedEmEFMax=0.99;

  ///B-tagging
  vector <int> jBtag, jBtag_temp;
  int nBtag=0;

  //// Primary Electrons (used in electron selection):
  double el_et;
  double elPrim_etMin=30.0;
  double el_Eta=-1;
  double el_eta;
  double el_Phi=-1;
  double elPrim_aetaMax=2.5;
  double el_aetasc;
  double elPrim_aetascExcludeMax=1.5660;
  double elPrim_aetascExcludeMin=1.4442;
  double elPrim_dBMax=0.02;
  double el_RelIso;
  double elPrim_RelIsoMax=0.1;

  int el_eid;
  int elPrim_eid1=1;
  int elPrim_eid2=3;
  int elPrim_eid3=5;
  int elPrim_eid4=7;
  int elPrim_trackerInnerHitsMin=0;

  ///Equivalent to the EID cut (https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID)
  int el_PassEIDcnt;
  bool el_PassEID;
  double elPrim_sigmaIetaIeta;
  double elPrim_aDeltaPhi;
  double elPrim_aDeltaEta;
  double elPrim_HoE;
  double elPrim_sigmaIetaIetaMaxEB=0.01;
  double elPrim_aDeltaPhiMaxEB=0.03;
  double elPrim_aDeltaEtaMaxEB=0.004;
  double elPrim_HoEMaxEB=0.025;
  double elPrim_sigmaIetaIetaMaxEE=0.03;
  double elPrim_aDeltaPhiMaxEE=0.02;
  double elPrim_aDeltaEtaMaxEE=0.005;
  double elPrim_HoEMaxEE=0.025;

  //// Loose Electrons (used in muon selection):
  double elLoose_etMin=15.0;
  double elLoose_aetaMax=2.5;
  double elLoose_RelIsoMax=0.2;

  //// Electron ZVeto (used in electron selection):
  double elZVeto_etMin=20.0;
  double elZVeto_aetaMax=2.5;
  double elZVeto_RelIsoMax=1.0;
  double elZVeto_MZMin=76.0;
  double elZVeto_MZMax=106.0;

  int elZVeto_eid1=1;
  int elZVeto_eid2=3;
  int elZVeto_eid3=5;
  int elZVeto_eid4=7;

  /// Equivalent Quantities:
  double elZVeto_sigmaIetaIeta;
  double elZVeto_aDeltaPhi;
  double elZVeto_aDeltaEta;
  double elZVeto_HoE;
  double elZVeto_sigmaIetaIetaMaxEB=1.0e-02;
  double elZVeto_aDeltaPhiMaxEB=8.0e-01;
  double elZVeto_aDeltaEtaMaxEB=7.0e-03;
  double elZVeto_HoEMaxEB=1.5e-01;
  double elZVeto_sigmaIetaIetaMaxEE=3.0e-02;
  double elZVeto_aDeltaPhiMaxEE=7.0e-01;
  double elZVeto_aDeltaEtaMaxEE=1.0e-02;
  double elZVeto_HoEMaxEE=7.0e-02;

  /// Electron Flags
  bool el_vtxPass=false;
  bool el_aetaPass=false;
  bool el_conv_a=false;
  bool el_conv_b=false;
  bool el_ZVeto=false;
	 
  /// Primary Muons (used in muon selection):
  double mu_pt;
  double muPrim_ptMin=20.0;
  double mu_eta;
  double mu_phi;
  vector <math::XYZTLorentzVector> Mup4,Mup4_temp;
  double mu_ecalIso, mu_hcalIso, mu_trkIso;
  vector <double> MuEta,MuEta_temp,MuPhi,MuPhi_temp,MuecalIso,MuecalIso_temp,MuhcalIso,MuhcalIso_temp,MutrkIso,MutrkIso_temp;
  vector <int> MuQ,MuQ_temp;
      
  double muPrim_aetaMax=2.1;
  double muPrim_gtNormChi2Max=10.0;
  double muPrim_gtNMuHitsMin=0.5;
  double muPrim_itNHits;
  double muPrim_itNHitsMin=10.5;
  double muPrim_DRj;
  double muPrim_DRjLowerMax=0.1;//Reject *jets* with DRj<DRjLowerMax
  double muPrim_DRjUpperMax=0.3;//Reject *muons* with DRj<DRjUpperMax
  double muPrim_dBMax=0.02;
  double mu_RelIso;

  bool isProperlyIsolatedMu=false;
  double muPrim_RelIsoMax=0.05;

  bool muisGmTm;
  bool muisTightPrompt;
  bool muDRPass;
  bool mu_vtxPass;

  /// Loose Muons (used in both electron and muon selection):
  double muLoose_ptMin=10.0;
  double muLoose_aetaMax=2.5;
  double muLoose_RelIsoMax=0.2;

  /// Optional (i.e. not a part of V2 Synchronization Exercize):
  double MET_Et;
  double MET_EtMin=30;
  double bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.
  double etaBarrelMax=1.442; //The Maximum value of eta for the Barrel//**fix

  //////////////////////// Counters & Flags ////////////////////////////////
  ///Global Counters:
  bool EventFail;
  int vtxcnt=0;
  int EvtTotCount=0;
  int PassEl[NCUTS][NJETS];
  int PassMu[NCUTS][NJETS];
  int PassLp[NCUTS][NJETS];
  InitializeIntMatrix(PassEl);
  InitializeIntMatrix(PassMu);
  InitializeIntMatrix(PassLp);
  ///Event Based Counters & Flags:
  long runNumber;
  long eventNumber;
  bool isPrimaryVertex=false;
  int elcnt_Prim=0;
  int elcnt_Loose=0;
  int mucnt_Prim=0;
  int mucnt_Loose=0;
  int mucnt_temp=0;
  int jcnt_tot=0;
  int jNEntries;
  int passAll=0;//=1 for mu, =2 for el if the event passes all of the cuts
  int passStandard=0;
  int passFlag=0;
  ///Event Based Markers:
  int el1Marker;//, mu1Marker;
  //  int el2Marker, mu2Marker;
  int lpCounter;

  /// Additional Parameters
  math::XYZTLorentzVector lp4, METp4;
  TLorentzVector lp4LV, j1p4LV, j2p4LV, METp4LV;
  TVector3 lp3, jjp3;
  double Thetalj1pj2, DRlj1, DRlj2;
  double lTotIso,lecalIso,lhcalIso,ltrkIso;
  double Mjj=-1;
  int lQ=-10;
  double lEta=-1;


  // Parse the command line arguments
  parser.parseArguments (argc, argv);
  
  //////////////////////////////////
  // //////////////////////////// //
  // // Create Event Container // //
  // //////////////////////////// //
  //////////////////////////////////

  // This object 'event' is used both to get all information from the
  // event as well as to store histograms, etc.
  fwlite::EventContainer eventCont (parser);
  
  ////////////////////////////////////////
  // ////////////////////////////////// //
  // //         Begin Run            // //
  // // (e.g., book histograms, etc) // //
  // ////////////////////////////////// //
  ////////////////////////////////////////
  
//   // Setup a style
//   gROOT->SetStyle ("Plain");
  
//   // Book those histograms (not used)
//   eventCont.add( new TH1F( "jetPt", "jetPt", 1000, 0, 1000) );//**fix
  
  
  //////////////////////
  // //////////////// //
  // // Event Loop // //
  // //////////////// //
  //////////////////////
  
  
  // Create a set containing all the run and event numbers
  // so we can keep track of duplicate events.
  RunEventSet runEventSet;
  
  for (eventCont.toBegin(); ! eventCont.atEnd(); ++eventCont) 
    {
      
      //find the runNumber and eventNumber here
      
      runNumber = eventCont.id().run();
      eventNumber = eventCont.id().event();
      
      //Check and remove duplicates here
      if ( runEventSet.alreadySeen(runNumber, eventNumber)) {
	cout << "WARNING, event Run = " << runNumber
	     << ", Event = " << eventNumber
	     <<" is duplicated" << endl
	     << "We will skip this event." << endl;
	continue;
      }
      
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ***************************************Load the objects of interest*************************************** ////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      fwlite::Handle< vector< reco::Vertex > > VtxHandle;
      VtxHandle.getByLabel (eventCont, "offlinePrimaryVertices");
      assert ( VtxHandle.isValid() ); 

      fwlite::Handle<reco::GenParticleCollection> genParticleHandle;
      if ( !modename.Contains("Data") ) {
	genParticleHandle.getByLabel (eventCont, "genParticles");
	assert ( genParticleHandle.isValid() );
      }

      ///From PF2PAT, standard postfix=PFlow
      fwlite::Handle< vector< pat::Electron > > electronHandle;
      if ( !modename.Contains("PFlowLoose") ) {
	electronHandle.getByLabel (eventCont, "selectedPatElectronsPFlow");
      } else {
	electronHandle.getByLabel (eventCont, "selectedPatElectronsPFlowLoose");
      }
      assert ( electronHandle.isValid() );

      fwlite::Handle< vector< pat::Jet > > pfJetHandle;
      if ( !modename.Contains("PFlowLoose") ) {
	pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlow");
      } else {
	pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlowLoose");
      }
      assert ( pfJetHandle.isValid() );
 
      fwlite::Handle< vector< pat::Muon > > muonHandle;
      if ( !modename.Contains("PFlowLoose") ) {
	muonHandle.getByLabel (eventCont, "selectedPatMuonsPFlow");
      } else {
	muonHandle.getByLabel (eventCont, "selectedPatMuonsPFlowLoose");
      }
      assert ( muonHandle.isValid() );

      fwlite::Handle< vector< pat::MET > > METHandle;
      if ( !modename.Contains("PFlowLoose") ) {
	METHandle.getByLabel (eventCont, "patMETsPFlow");
      } else {
	METHandle.getByLabel (eventCont, "patMETsPFlowLoose");
      }
      assert ( METHandle.isValid() );



      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ***************************************Perform The Selection*************************************** ///////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      EventFail=false;

      ////// ********************************     Vertex Selection     ******************************** //////
      isPrimaryVertex=false;
      reco::Vertex Vtx=VtxHandle->front();
      if ( (!Vtx.isFake())&&(Vtx.ndof()>=vtx_ndofMin)&&(abs(Vtx.z())<vtx_zMax)&&(Vtx.position().Rho()<vtx_RhoMax) ) {
	vtxcnt++;
	isPrimaryVertex=true;
      }
      
      ////// ********************************     Electron Selection     ******************************** //////
      elcnt_Prim=0;
      elcnt_Loose=0;
      el_conv_a=false;
      el_conv_b=false;
      el1Marker=-1;
      lpCounter=0;
      
      const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
      for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 	
	el_et=elIter->et();
	el_eta=elIter->eta();
	el_aetasc=abs(elIter->superCluster()->eta());
	
	el_vtxPass=false;
	if ( abs(Vtx.z() - elIter->vertex().z())<1 ) {
	  el_vtxPass=true;
	}

	el_aetaPass=false;
	if ( abs(el_eta)<elPrim_aetaMax ) {
	  el_aetaPass=true; //noscv
	}
	if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
	  el_aetaPass=false;
	}

 	/// We can either directly apply the electronID cuts or look at each individual step:
	//temp//
	el_eid=elIter->electronID("simpleEleId70relIso");
	el_PassEIDcnt=-1;
	el_PassEID=false;
	elPrim_sigmaIetaIeta=elIter->sigmaIetaIeta();
	elPrim_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
	elPrim_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
	elPrim_HoE=elIter->hadronicOverEm();
	if ( elIter->isEB() ) {
	  el_PassEIDcnt=0;
	  if ( elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEB ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEB ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_HoE<elPrim_HoEMaxEB ) {
	    el_PassEIDcnt++;
	  }
	}
	if ( elIter->isEE() ) {
	  el_PassEIDcnt=0;
	  if ( elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEE ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEE ) {
	    el_PassEIDcnt++;
	  }
	  if ( elPrim_HoE<elPrim_HoEMaxEE ) {
	    el_PassEIDcnt++;
	  }
	}
	/// Choose whether we're doing the usual EID or inverting the selection
	if ( modename.Contains("invertEID") ) {
	  if ( el_PassEIDcnt<2.5 ) {
	    el_PassEID=true;
	  }
	} else {
	  //((el_eid==elPrim_eid1)||(el_eid==elPrim_eid2)||(el_eid==elPrim_eid3)||(el_eid==elPrim_eid4))&&
	  if ( el_PassEIDcnt==4 ) {
	    el_PassEID=true;
	  }
	}

	el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_et;
	
	/// Apply The Primary Cuts
	if ( el_vtxPass&&
	     (el_et>elPrim_etMin)&&
	     el_aetaPass&&
	     (abs(elIter->dB())<elPrim_dBMax)&&
	     el_PassEID&&
	     (el_RelIso<elPrim_RelIsoMax)
	     ) {
	  if ( el1Marker==-1 ) {
	    el1Marker=lpCounter;
	  }
	  elcnt_Prim++;
	  lp4=elIter->p4();
	  lQ=elIter->charge();
	  lEta=el_eta;
	  el_Eta=el_eta;
	  el_Phi=elIter->phi();
	  lecalIso=(elIter->dr03EcalRecHitSumEt())/el_et;
	  lhcalIso=(elIter->dr03HcalTowerSumEt())/el_et;
	  ltrkIso=(elIter->dr03TkSumPt())/el_et;
	  lTotIso=el_RelIso;
	  
	  ///Electron Conversion
	  if ( elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()>elPrim_trackerInnerHitsMin ) {
	    el_conv_a=true;
	  }
	  //temp//
	  if ( el_eid<3.5 ) {
	    el_conv_b=true;
	  }
	}
	/// Apply The Loose Cuts
	if ( (el_et>elLoose_etMin)&&(abs(el_eta)<elLoose_aetaMax)&&(el_RelIso<elLoose_RelIsoMax) ) {
	  elcnt_Loose++;	       
	}
	
	lpCounter++;
      }

      ////// ********************************     Jet Selection (without muon cleaning)    ******************************** //////
      //// Electron Cleaning (i.e. remove the jets which are likely to be electorns using the DR cut).
      //// Using PF Jets
      jcnt_tot=0;
      j_DRel=100;
      Mjj=-1;

      jNEntries=0;
      jp4.clear();
      JEta.clear();
      JPhi.clear();
      nBtag=0;
      jBtag.clear();

      const vector< pat::Jet >::const_iterator pfJetEnd = pfJetHandle->end();
      for (vector< pat::Jet >::const_iterator jetIter = pfJetHandle->begin(); ( pfJetEnd != jetIter ); ++jetIter ) { 
	j_pt=jetIter->pt();
	j_eta=jetIter->eta();
	if ( elcnt_Prim==1 ){
	  adphi=abs(el_Phi-jetIter->phi());
	  //we're on a circle
	  if ( adphi>pi_ ) {
	    adphi=2*pi_-adphi;
	  }
	  j_DRel=pow((j_eta-el_Eta)*(j_eta-el_Eta)+adphi*adphi,0.5);
	}
	
	/// Apply The Cut:
	if ( (j_pt>j_ptMin)&&
	     (abs(j_eta)<j_aetaMax)&&
	     (j_DRel>j_DRelMin)&&
	     (jetIter->neutralHadronEnergyFraction()<jpf_neutralHadronEFMax)&&
	     (jetIter->neutralEmEnergyFraction()<jpf_neutralEmEFMax)&&
	     (jetIter->nConstituents()>jpf_nConstituentsMin)&&
	     (jetIter->chargedHadronEnergyFraction()>jpf_chargedHadronEFMin)&&
	     (jetIter->chargedMultiplicity()>jpf_chargedMultiplicityMin)&&
	     (jetIter->chargedEmEnergyFraction()<jpf_chargedEmEFMax)
// 	     ((jetIter->emEnergyFraction())>j_emFracMin)&&
// 	     ((jetIter->jetID().n90Hits)>j_n90HitsMin)&&
// 	     ((jetIter->jetID().fHPD)<j_fHPDMax)
	     ) {
	  
	  jcnt_tot++;
	  
	  // store the parameters 
	  jp4.push_back(jetIter->p4());
	  JEta.push_back(j_eta); 
	  JPhi.push_back(jetIter->phi());
	  jNEntries++;
	  
	  /// b-tagging:
	  if ( (jetIter->bDiscriminator("simpleSecondaryVertexBJetTags"))>bDiscriminator_min ) {
	    jBtag.push_back(1);
	  } else {
	    jBtag.push_back(0);
	  }

 	} // for Selection Cuts
 
      } // for jetIter

      ////// ********************************    Muon Selection/Muon Cleaning of the Jets    ******************************** //////
      //// Muon Cleaning (i.e. remove the jets which are likely to be muons using the DR cut).
      //// Muon Selection:
      mucnt_Prim=0;
      mucnt_Loose=0;
      //mu1Marker=-1;
      lpCounter=0;

      const vector< pat::Muon >::const_iterator muEnd = muonHandle->end();
      for (vector< pat::Muon >::const_iterator muIter = muonHandle->begin(); ( muEnd != muIter ); ++muIter ) { 
       mu_pt=muIter->pt();
       mu_eta=muIter->eta();
       mu_phi=muIter->phi();
       mu_ecalIso=(muIter->ecalIso())/mu_pt;
       mu_hcalIso=(muIter->hcalIso())/mu_pt;
       mu_trkIso=(muIter->trackIso())/mu_pt;
       mu_RelIso=mu_ecalIso+mu_hcalIso+mu_trkIso;

       muisGmTm=false;
       if ( (muIter->isGlobalMuon())&&(muIter->isTrackerMuon()) ) {
	 muisGmTm=true;
       }
       muisTightPrompt=false;
       if ( muIter->globalTrack().isNonnull() ) {
	 if ( ((muIter->globalTrack()->normalizedChi2())<muPrim_gtNormChi2Max)&&((muIter->globalTrack()->hitPattern().numberOfValidMuonHits())>muPrim_gtNMuHitsMin) ) {
	   muisTightPrompt=true;
	 }
       }
       if ( muIter->innerTrack().isNonnull() ) {
	 muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
       } else {
	 muPrim_itNHits=-1;
       }
       
       mu_vtxPass=false;
       if ( abs(Vtx.z() - muIter->vertex().z())<1 ) {
	 mu_vtxPass=true;
       }

       /// Apply The Primary Cuts
       if ( muisGmTm&&
	    (mu_pt>muPrim_ptMin)&&
	    (abs(mu_eta)<muPrim_aetaMax)&&
	    //	    (mu_RelIso<muPrim_RelIsoMax)&&
	    muisTightPrompt&&
	    (muPrim_itNHits>muPrim_itNHitsMin)&&
	    // muDRPass&&
	    ((muIter->dB())<muPrim_dBMax)&&
	    mu_vtxPass&&
	    (muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>0.5)&&
	    (muIter->numberOfMatches()>1)
	    ) {
	 ///Go through the jets and throw out the ones which are sufficiently close to (& are likely to be) muons & record the muon info.
	 isProperlyIsolatedMu=false;
	 if ( ( (mu_RelIso<muPrim_RelIsoMax)&&(!modename.Contains("PFlowLoose")) )||
	      ( (mu_RelIso>muPrim_RelIsoMax)&&(modename.Contains("PFlowLoose")) )
	      ) {
	   isProperlyIsolatedMu=true;
	 }
	 if ( isProperlyIsolatedMu ) {
	   if ( jNEntries!=0 ) {
	     jp4_temp.clear();
	     JEta_temp.clear();
	     JPhi_temp.clear();
	     jBtag_temp.clear();
	     jcnt_tot=0;
	     nBtag=0;
	     for (Int_t jn=0; jn<jNEntries;jn++) {
	       adphi=abs(JPhi[jn]-mu_phi);
	       //we're on a circle
	       if ( adphi>pi_ ) {
		 adphi=2*pi_-adphi;
	       }
	       muPrim_DRj=pow((JEta[jn]-mu_eta)*(JEta[jn]-mu_eta)+adphi*adphi,0.5);
	       if ( muPrim_DRj>muPrim_DRjLowerMax ) {
		 jp4_temp.push_back(jp4[jn]);
		 JEta_temp.push_back(JEta[jn]);
		 JPhi_temp.push_back(JPhi[jn]);
		 nBtag=nBtag+jBtag[jn];
		 jBtag_temp.push_back(jBtag[jn]);
		 jcnt_tot++;
	       }
	     }
	     jp4=jp4_temp;
	     JEta=JEta_temp;
	     JPhi=JPhi_temp;
	     jBtag=jBtag_temp;
	     jNEntries=jcnt_tot;
	   }
	 /// Record the muon quantities:
	 mucnt_Prim++;
	 Mup4.push_back(muIter->p4());
	 MuEta.push_back(mu_eta);
	 MuPhi.push_back(mu_phi);
	 MuQ.push_back(muIter->charge());
	 MuecalIso.push_back(mu_ecalIso);
	 MuhcalIso.push_back(mu_hcalIso);
	 MutrkIso.push_back(mu_trkIso);
	 }
// 	 /// Record the muon quantities:
// 	 mucnt_Prim++;
// 	 Mup4.push_back(muIter->p4());
// 	 MuEta.push_back(mu_eta);
// 	 MuPhi.push_back(mu_phi);
// 	 MuQ.push_back(muIter->charge());
// 	 MuecalIso.push_back(mu_ecalIso);
// 	 MuhcalIso.push_back(mu_hcalIso);
// 	 MutrkIso.push_back(mu_trkIso);
	 
       }
       /// Apply The Loose Cuts
       if ( (muIter->isGlobalMuon())&&(mu_pt>muLoose_ptMin)&&(abs(mu_eta)<muLoose_aetaMax)&&
	    ( ((!modename.Contains("PFlowLoose"))&&(mu_RelIso<muLoose_RelIsoMax))||((modename.Contains("PFlowLoose"))&&(mu_RelIso>muPrim_RelIsoMax)) ) 
	    ) {
	 mucnt_Loose++;	       
       }
       
       //lpCounter++; 
      }

      /// Compute the invariant mass, when only two jets are present
      if ( jcnt_tot==2 ) {    
	Mjj=(jp4[0]+jp4[1]).M();
      } else {
	Mjj=-2;
      }

      //// Run over the (selected primary) muons and remove the ones which are too close to the (cleaned) jets


      mucnt_temp=0;
      Mup4_temp.clear();
      MuEta_temp.clear();
      MuPhi_temp.clear();
      MuQ_temp.clear();
      MuecalIso_temp.clear();
      MuhcalIso_temp.clear();
      MutrkIso_temp.clear();
   
//       if ( mucnt_Prim!=0 ) {
// 	cout << "mucnt_Prim=" << mucnt_Prim << endl;
//       }
      for (Int_t mun=0; mun<mucnt_Prim ;mun++) {
	muDRPass=true;
	// Clean up the muons (with respect to the jets)
	if ( jNEntries!=0 ) {
	  for (Int_t jn=0; jn<jNEntries;jn++) {
	    adphi=abs(JPhi[jn]-MuPhi[mun]);
	    //we're on a circle
	    if ( adphi>pi_ ) {
	      adphi=2*pi_-adphi;
	    }
	    
	    muPrim_DRj=pow((JEta[jn]-MuEta[mun])*(JEta[jn]-MuEta[mun])+adphi*adphi,0.5);
	    if ( muPrim_DRj<muPrim_DRjUpperMax ) {
	      //i.e. we must be outside DRjMax for every jet
	      muDRPass=false;
	    }
	  }
	}
	// For clean muons:
	if ( muDRPass ) {
	  mu_RelIso=MuecalIso[mun]+MuhcalIso[mun]+MutrkIso[mun];
	  isProperlyIsolatedMu=false;
	  if ( mu_RelIso<muPrim_RelIsoMax ) {
	    if ( modename.Contains("PFlowLoose") ) {
	      EventFail=true;
	      //cout << "EVENT FLAGGED AS FAILING" << endl;
	    } else {
	      isProperlyIsolatedMu=true;
	    }
	  } else {
	    if ( modename.Contains("PFlowLoose") ) {
	      isProperlyIsolatedMu=true;
	    }
	  }

	  if ( isProperlyIsolatedMu ) {
	    mucnt_temp++;
	    Mup4_temp.push_back(Mup4[mun]);
	    MuEta_temp.push_back(MuEta[mun]);
	    MuPhi_temp.push_back(MuPhi[mun]);
	    MuQ_temp.push_back(MuQ[mun]);
	    MuecalIso_temp.push_back(MuecalIso[mun]);
	    MuhcalIso_temp.push_back(MuhcalIso[mun]);
	    MutrkIso_temp.push_back(MutrkIso[mun]);
	  }
	}

	//cout << "mun=" << mun << " :   mu_RelIso=" << mu_RelIso << endl;
      }

      mucnt_Prim=mucnt_temp;
      Mup4=Mup4_temp;
      MuEta=MuEta_temp;
      MuPhi=MuPhi_temp;
      MuQ=MuQ_temp;
      MuecalIso=MuecalIso_temp;
      MuhcalIso=MuhcalIso_temp;
      MutrkIso=MutrkIso_temp;

      if ( mucnt_Prim==1 ) {
	lp4=Mup4[0];
	lQ=MuQ[0];
	lEta=MuEta[0];
	lecalIso=MuecalIso[0];
	lhcalIso=MuhcalIso[0];
	ltrkIso=MutrkIso[0];
	lTotIso=lecalIso+lhcalIso+ltrkIso;
      }

      ////// ********************************    Electron ZVeto    ******************************** //////
      /// Apply only if we have just one Primary electron
      el_ZVeto=false;
      lpCounter=0;
      if ( (elcnt_Prim==1)&&(mucnt_Loose==0) ){
	const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
	for ( vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( (elEnd !=elIter) ) ; ++elIter ) {
	  el_et=elIter->et();
	  el_eta=elIter->eta();
	  el_aetasc=abs(elIter->superCluster()->eta());
	  
	  el_aetaPass=false;
	  if ( abs(el_eta)<elZVeto_aetaMax ) {
	    el_aetaPass=true; //noscv
	  }
	  if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
	    el_aetaPass=false;
	  }
	  el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_et;
	  
	  //el_eid=elIter->electronID("simpleEleId95relIso");
	  //Alternatively:	 
	  el_PassEIDcnt=-1;
	  el_PassEID=false;
	  elZVeto_sigmaIetaIeta=elIter->sigmaIetaIeta();
	  elZVeto_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
	  elZVeto_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
	  elZVeto_HoE=elIter->hadronicOverEm();
	  if ( elIter->isEB() ) {
	    el_PassEIDcnt=0;
	    if ( elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEB ) {
	      el_PassEIDcnt++;
	    }
	    if ( elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEB ) {
	      el_PassEIDcnt++;
	  }
	    if ( elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEB ) {
	      el_PassEIDcnt++;
	    }
	    if ( elZVeto_HoE<elZVeto_HoEMaxEB ) {
	      el_PassEIDcnt++;
	    }
	  }
	  if ( elIter->isEE() ) {
	    el_PassEIDcnt=0;
	    if ( elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEE ) {
	      el_PassEIDcnt++;
	    }
	    if ( elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEE ) {
	      el_PassEIDcnt++;
	    }
	    if ( elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEE ) {
	      el_PassEIDcnt++;
	    }
	    if ( elZVeto_HoE<elZVeto_HoEMaxEE ) {
	      el_PassEIDcnt++;
	    }
	  }
	  /// Choose whether we're doing the usual EID or inverting the selection
	  if ( modename.Contains("invertEID") ) {
	    if ( el_PassEIDcnt<2.5 ) {
	      el_PassEID=true;
	    }
	  } else {
	    //((el_eid==elZVeto_eid1)||(el_eid==elZVeto_eid2)||(el_eid==elZVeto_eid3)||(el_eid==elZVeto_eid4))&&
	    if ( el_PassEIDcnt==4 ) {
	      el_PassEID=true;
	    }
	  }
	  /// Select an electron which is not the Primary one.
	  if ( (lpCounter!=el1Marker) && 
	       (
		(el_et>elZVeto_etMin)&&
		el_aetaPass&&
		(el_RelIso<elZVeto_RelIsoMax)&& 
		//((el_eid==elZVeto_eid1)||(el_eid==elZVeto_eid2)||(el_eid==elZVeto_eid3)||(el_eid==elZVeto_eid4))
		el_PassEID
		)
	       ) {
	    if ( 
		//((elIter->charge() * lQ)<0.0)&&
		((elIter->p4() + lp4).M()>elZVeto_MZMin)&&
		((elIter->p4() + lp4).M()<elZVeto_MZMax) ) {
	      el_ZVeto=true;
	    }
	  }
	  lpCounter++;
	}
	// }
      }
      
      //// MET Selection:
      METp4=METHandle->front().p4();
      MET_Et=METp4.E();
      //MET_Et=METHandle->front().et();
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /////// ****************************************Record The Results**************************************** ////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
      //// Record the number of events passing the cuts for each jet count
      passStandard=0;
      passAll=0;
      for (Int_t Nj=0; Nj<NJETS;Nj++) {
	if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {
	  PassLp[2][Nj]++;
	  PassMu[2][Nj]++;
	  PassEl[2][Nj]++;
	  
	 //record the electron info
	 if ( (elcnt_Prim==1)&&(mucnt_Prim==0)&&isPrimaryVertex&&(!EventFail) ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	   if ( mucnt_Loose==0 ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	     if ( el_ZVeto==false ) {
	       incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	       if ( (el_conv_a==false)&&(el_conv_b==false) ) {
		 incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		 passStandard=2;
		 if ( MET_Et>MET_EtMin ) {
		   incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		   passAll=2;
		   /// Record the Btag info
		   if ( nBtag==0 ) {
		     incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		   } else {
		     if ( nBtag==1 ) {
		       incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		     } else {
		       if ( nBtag==2 ) {
			 incrementCounter(10,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		       } else {
			 incrementCounter(11,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		       }
		     }
		   }//BTags
		 }
	       }
	     }
	   }
	 }//electrons
	 
	 //record the muon info
	 if ( (mucnt_Prim==1)&&(elcnt_Prim==0)&&isPrimaryVertex&&(!EventFail) ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   if ( mucnt_Loose==1 ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	     if ( elcnt_Loose==0 ) {
	       incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	       //if ( ZVeto==false ) {
	       incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	       passStandard=1;
	       if ( MET_Et>MET_EtMin ) {
		 incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		 passAll=1;
		 /// Record the Btag info
		 if ( nBtag==0 ) {
		   incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		 } else {
		   if ( nBtag==1 ) {
		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		   } else {
		     if ( nBtag==2 ) {
		       incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		     } else {
		       incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		     }
		   }
		 }//BTags
	       }
	       // }
	     }
	   }
	 }//muons
	 
       }
     } // for Njets


     ///Fill the Ntuple to be used in Matrix Element computations (requiring two jets & a lepton passing all of the cuts).
     //Usual option: only fill in the elements which pass the MET cut:
     passFlag=passAll;
     if ( modename.Contains("noMETcut") ) {
       passFlag=passStandard;
     }
//      if ( modename.Contains("elONLY") ) {
//        if ( passFlag!=2 ) {
// 	 passFlag=-1;
//        }
//      }
//      if ( modename.Contains("muONLY") ) {
//        if ( passFlag!=1 ) {
// 	 passFlag=-1;
//        }
//      }

     if ( (passFlag>0.5)&&(!EventFail)&&(jcnt_tot==2) ) {
       //if ( (passFlag==2)&&(!EventFail)&&(jcnt_tot==2) ) {
       EvtNtuple->lLV.clear();
       lp4LV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
       lp3.SetXYZ(lp4.Px(),lp4.Py(),lp4.Pz());
       EvtNtuple->lLV.push_back(lp4LV);
       //EvtNtuple->lLV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());

       EvtNtuple->jLV.clear();
       j1p4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
       EvtNtuple->jLV.push_back(j1p4LV);
       j2p4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
       EvtNtuple->jLV.push_back(j2p4LV);
       jjp3.SetXYZ(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz());

       EvtNtuple->METLV.clear();
       METp4LV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
       EvtNtuple->METLV.push_back(METp4LV);

       EvtNtuple->lPhi=lp4.Phi();
       EvtNtuple->METEt=METp4LV.Et();


       EvtNtuple->jBtag.clear();
       EvtNtuple->jBtag.push_back(jBtag[0]);
       EvtNtuple->jBtag.push_back(jBtag[1]);
       //       cout << "2.EvtNtuple->METLV.E()=" << EvtNtuple->METLV.E() << ", EvtNtuple->METEt=" << EvtNtuple->METEt << endl;
       EvtNtuple->lQ=lQ;
       if (abs(lEta)<etaBarrelMax) {
	 //barrel
	 EvtNtuple->ldetComp=0;
       } else {
	 //endcap
	 EvtNtuple->ldetComp=1;
       }
       EvtNtuple->run=runNumber;
       EvtNtuple->event=eventNumber;
       
       //Transfer Function Variables
       EvtNtuple->matchedGenParticles.clear();
       EvtNtuple->matchedpdgId.clear();
       EvtNtuple->matchedDeltaR.clear();
       //Do the matching between the pat::Jets and the reco::genParticles
       //pair<int, reco::Candidate::LorentzVector> matchedPair = matchToGen((*jetIter),genParticleHandle);
       //pair<int, TLorentzVector> matchedPair = matchToGen((*jetIter),genParticleHandle);
       pair<int, TLorentzVector> matchedPair;
       if ( !modename.Contains("Data") ) {
	 matchedPair = matchToGen(j1p4LV.Eta(),j1p4LV.Phi(),genParticleHandle);
	 EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
	 EvtNtuple->matchedpdgId.push_back(matchedPair.first);
	 //EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.eta(), matchedPair.second.phi(), (*jetIter).eta(), (*jetIter).phi()));
	 //EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), (*jetIter).eta(), (*jetIter).phi()));
	 EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j1p4LV.Eta(), j1p4LV.Phi()));
	 matchedPair = matchToGen(j2p4LV.Eta(),j2p4LV.Phi(),genParticleHandle);
	 EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
	 EvtNtuple->matchedpdgId.push_back(matchedPair.first);
	 EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j2p4LV.Eta(), j2p4LV.Phi()));
       }
       //Additional Variables
       EvtNtuple->lTotIso=lTotIso;
       EvtNtuple->lecalIso=lecalIso;
       EvtNtuple->lhcalIso=lhcalIso;
       EvtNtuple->ltrkIso=ltrkIso;
       EvtNtuple->Mjj=Mjj;
       EvtNtuple->passStd=passStandard;
       EvtNtuple->passAll=passAll;

       //cout << "3.EvtNtuple->METLV.E()=" << EvtNtuple->METLV.E() << ", EvtNtuple->METEt=" << EvtNtuple->METEt << endl;
       DRlj1=lp4LV.DeltaR(j1p4LV);
       DRlj2=lp4LV.DeltaR(j2p4LV);
       //Thetalj1pj2=(lp4.Px()*(jp4[0].Px()+jp4[1].Px())+lp4.Py()*(jp4[0].Py()+jp4[1].Py())+lp4.Pz()*(jp4[0].Pz()+jp4[1].Pz()))/(    ) ;
       Thetalj1pj2=lp3.Dot(jjp3)/(lp3.Mag()*jjp3.Mag());

       EvtNtuple->DRlj1=DRlj1;
       EvtNtuple->DRlj2=DRlj2;
       EvtNtuple->Thetalj1pj2=Thetalj1pj2;
       //Fill The Tree 
       EvtTree->Fill();

    }
     
     EvtTotCount++;

   } // for eventCont

   for (Int_t Nj=0; Nj<NJETS;Nj++) {
     PassLp[0][Nj]=EvtTotCount;
     PassMu[0][Nj]=EvtTotCount;
     PassEl[0][Nj]=EvtTotCount;
     PassLp[1][Nj]=vtxcnt;
     PassMu[1][Nj]=vtxcnt;
     PassEl[1][Nj]=vtxcnt;
   }


   InitializeLabels(PLabel,CLabel);
   writeIntProcessTable(outtablefile,PassEl,PassMu,PassLp);


   outrootfile->Write();
   outrootfile->Close();

   return 0;

}


