/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Stage 2 Skimming of the PAT-tuples to be used in Matrix Element calculations.
//// WW/H->WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Designed to work with 4_2_8 PAT-tuples and tested on V3 HWW sync exercises.
//// Use to skim the PATNtuples and create a custom made .root file containing the needed information.
//// Specialized mode names: 
//// - noMETcut - disregard the MET cut when storing the events.
// //// - invertEID - electrons with *fail* at least two of the EID requirements will be kept instead.
// //// - PFlowLoose - use the collections with the PF2PAT postfix=PFlowLoose (instead of PFlow).
//// - elONLY - only save the output if the lepton is an electron.
//// - muONLY - only save the output if the lepton is a muon.

// CMS includes
#include "DataFormats/FWLite/interface/Handle.h"
//#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/UserData.h"

#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexSorter.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "PhysicsTools/FWLite/interface/EventContainer.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 

//User Defined Ntuple
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
//#include "TAMUWW/SpecialTools/interface/Defs.hh"

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
#include <TAMUWW/Tools/GlobalTools428.cc>
//#include <TAMUWW/Tools/GlobalTools_HWWMC_Ex3.cc>

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
pair<int, TLorentzVector> matchToGen(double eta, double phi, fwlite::Handle<reco::GenParticleCollection> genParticleHandle) {
  double bestDeltaR = 9999;
  int bestPDGID = 0;
  TLorentzVector theGenObject(0,0,0,0);
  TLorentzVector theBestGenObject(0,0,0,0);
  for(reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
     if(genParticle->pt()==0)
     {
       //if(genParticle->pdgId()!=2212) cout << "matchToGen::The current genParticle is a " << genParticle->pdgId() << " with a pt of 0" << endl;
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
  optutl::CommandLineParser parser ("Does what it should");
   
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
  reco::Vertex pv;
  bool PVfound = false;
  double zvtx = -999;


  //// Jets (apply after the Primary Electrons but before the Primary Muons):
  vector <math::XYZTLorentzVector> jp4, jp4_temp;
  vector <double> JEta, JEta_temp, JPhi, JPhi_temp;
  double j_pt;
  double j_ptMin=20.0;
  double j_eta;
  double j_aetaMax=2.4;
  double j_phi;
  double j_DRlepton;
  double j_DRelMin=0.3;
  double muPrim_DRjMin=0.1;
  double adphi;

  ///B-tagging
  vector <int> jBtag, jBtag_temp;
  int nBtag=0;

  //// Leptons:
  double l_Eta=-1;
  double l_Phi=-1;

  //// Primary Electrons (used in electron selection):
  double el_pt;
  double elPrim_ptMin=20.0;
  double el_eta;
  double elPrim_aetaMax=2.5;
  double el_phi;
  double el_aetasc;
  double elPrim_aetascExcludeMax=1.5660;
  double elPrim_aetascExcludeMin=1.4442;
  double elPrim_dBMax=0.02;
  double el_RelIso;
  double elPrim_RelIsoMax=0.1;

  ///Equivalent to the EID cut (https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID)
  int el_PassEIDcnt;
  bool el_PassEID;
  double el_sigmaIetaIeta;
  double el_aDeltaPhi;
  double el_aDeltaEta;
  double el_HoE;
  double elPrim_sigmaIetaIetaMaxEB=0.01;
  double elPrim_aDeltaPhiMaxEB=0.06;
  double elPrim_aDeltaEtaMaxEB=0.004;
  double elPrim_HoEMaxEB=0.040;
  double elPrim_sigmaIetaIetaMaxEE=0.03;
  double elPrim_aDeltaPhiMaxEE=0.03;
  double elPrim_aDeltaEtaMaxEE=0.007;
  double elPrim_HoEMaxEE=0.025;

  //// Loose Electrons:
  double elLoose_ptMin=15.0;
  //  double elLoose_aetaMax=2.5;
  double elLoose_RelIsoMax=0.2;

  double elLoose_sigmaIetaIetaMaxEB=0.01;
  double elLoose_aDeltaPhiMaxEB=0.8;
  double elLoose_aDeltaEtaMaxEB=0.007;
  double elLoose_HoEMaxEB=0.15;
  double elLoose_sigmaIetaIetaMaxEE=0.03;
  double elLoose_aDeltaPhiMaxEE=0.7;
  double elLoose_aDeltaEtaMaxEE=0.01;
  double elLoose_HoEMaxEE=0.07;

  /// Electron Flags
  bool el_aetaPass=false;
  bool el_convPass_a=false;
  bool el_convPass_b=false;
	 
  /// Primary Muons (used in muon selection):
  double mu_pt;
  double muPrim_ptMin=20.0;
  double mu_eta;
  double mu_phi;
   
  double muPrim_aetaMax=2.1;
  double muPrim_itNHits;
  double muPrim_dBMax=0.02;
  double mu_RelIso;
  double muPrim_RelIsoMax=0.05;

  bool muisGmTm;
  bool muisTightPrompt;
  bool mu_vtxPass;
  bool isProperlyIsolatedMu=false;

  /// Loose Muons (used in both electron and muon selection):
  double muLoose_ptMin=10.0;
  double muLoose_aetaMax=2.5;
  double muLoose_RelIsoMax=0.2;

  /// Additional:
  double MET_Et;
  double MET_EtMin=25;
  double bDiscriminator_min=1.74; //corresponds to the 'medium' cut (per https://twiki.cern.ch/twiki/bin/view/CMS/BTV10001)
  double etaBarrelMax=1.442; //The Maximum value of eta for the Barrel//**fix when needed

  //////////////////////// Counters & Flags ////////////////////////////////
  ///Global Counters:
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
  int elcnt_Prim=0;
  int elcnt_Loose=0;
  int mucnt_Prim=0;
  int mucnt_Loose=0;
  int jcnt_tot=0;
  int jNEntries;
//   int passAll=0;//=1 for mu, =2 for el if the event passes all of the cuts
//   int passStandard=0;
//   int passFlag=0;
  bool mu_passAll=false;
  bool mu_passStandard=false;
  bool mu_passFlag=false;
  bool el_passAll=false;
  bool el_passStandard=false;
  bool el_passFlag=false;

  /// Additional Parameters
  math::XYZTLorentzVector lp4, METp4;
  TLorentzVector lp4LV, j1p4LV, j2p4LV, METp4LV;
  TVector3 lp3, jjp3;
  double Thetalj1pj2, DRlj1, DRlj2;
  double lTotIso=-1;
  double lecalIso=-10;
  double lhcalIso=-10;
  double ltrkIso=-1;
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

  // This object 'event' is used both to get all information from the event as well as to store histograms, etc.
  fwlite::EventContainer eventCont (parser);

  
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

      fwlite::Handle< reco::VertexCollection > vtxHandle;
      vtxHandle.getByLabel (eventCont, "offlinePrimaryVertices");
      assert ( vtxHandle.isValid() ); 

      fwlite::Handle<reco::GenParticleCollection> genParticleHandle;
      if ( !modename.Contains("Data") ) {
	genParticleHandle.getByLabel (eventCont, "genParticles");
	assert ( genParticleHandle.isValid() );
      }

      ///From PF2PAT, standard postfix=PFlow
      fwlite::Handle< vector< pat::Jet > > pfJetHandle;
      //      pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlow");
//       if ( !modename.Contains("PFlowLoose") ) {
      pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlow");
//       } else {
      //pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlowLoose");
      //      }
      assert ( pfJetHandle.isValid() );


      fwlite::Handle< vector< pat::Electron > > electronHandle;
      electronHandle.getByLabel (eventCont, "selectedPatElectronsPFlow");
      assert ( electronHandle.isValid() );

 
      fwlite::Handle< vector< pat::Muon > > muonHandle;
      muonHandle.getByLabel (eventCont, "selectedPatMuonsPFlow");
      assert ( muonHandle.isValid() );

      fwlite::Handle< vector< pat::MET > > METHandle;
      METHandle.getByLabel (eventCont, "patMETsPFlow");
      assert ( METHandle.isValid() );



      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ***************************************Perform The Selection*************************************** ///////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ********************************     Vertex Selection     ******************************** //////
      PVfound = (vtxHandle -> size() != 0);
      zvtx = -999;
      
      // Sort PV collection
      if(PVfound){
	PrimaryVertexSorter PVSorter;
	std::vector<reco::Vertex> sortedVertices = PVSorter.sortedList( *(vtxHandle.product()) );
	pv = sortedVertices.front();
	vtxcnt++;
	zvtx = pv.z();
      } else {
	throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event." << std::endl;
      }
   
     ////// ********************************     Electron Selection     ******************************** //////
     elcnt_Prim=0;
     elcnt_Loose=0;
     el_convPass_a=false;
     el_convPass_b=false;
     //lpCounter=0;
     
     const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
     for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 	
       el_pt=elIter->p4().Pt();
       el_eta=elIter->eta();
       el_phi=elIter->phi();
       el_aetasc=abs(elIter->superCluster()->eta());
       

       el_aetaPass=false;
       if ( abs(el_eta)<elPrim_aetaMax ) {
	 el_aetaPass=true; //noscv
       }
       if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
	 el_aetaPass=false;
       }

       // EID Cuts are applied directly
       el_PassEIDcnt=-1;
       el_PassEID=false;
       el_sigmaIetaIeta=elIter->sigmaIetaIeta();
       el_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
       el_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
       el_HoE=elIter->hadronicOverEm();
       if ( elIter->isEB() ) {
	 el_PassEIDcnt=0;
	 if ( el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaPhi<elPrim_aDeltaPhiMaxEB ) {
	   el_PassEIDcnt++;
	  }
	 if ( el_aDeltaEta<elPrim_aDeltaEtaMaxEB ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_HoE<elPrim_HoEMaxEB ) {
	   el_PassEIDcnt++;
	 }
       }
       if ( elIter->isEE() ) {
	 el_PassEIDcnt=0;
	 if ( el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaPhi<elPrim_aDeltaPhiMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaEta<elPrim_aDeltaEtaMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_HoE<elPrim_HoEMaxEE ) {
	   el_PassEIDcnt++;
	 }
       }
       /// Choose whether we're doing the usual EID or inverting the selection
//        if ( modename.Contains("invertEID") ) {
// 	 if ( el_PassEIDcnt<2.5 ) {
// 	   el_PassEID=true;
// 	 }
//        } else {
       if ( el_PassEIDcnt==4 ) {
	 el_PassEID=true;
       }
	 //       }

       el_RelIso=(elIter->dr03TkSumPt())/el_pt;

       /// Apply The Primary Cuts
       if ( (el_pt>elPrim_ptMin)&&
	    el_aetaPass&&
	    (el_RelIso<elPrim_RelIsoMax)&&
	    el_PassEID&&
	    (abs(elIter->dB())<elPrim_dBMax)&&
	    (elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()==0)&&
	    !( (abs(elIter->convDist())<0.02)&&(abs(elIter->convDcot())<0.02) )
	    ) {
	 elcnt_Prim++;
	 lp4=elIter->p4();
	 lQ=elIter->charge();
	 l_Eta=el_eta;
	 l_Phi=el_phi;
	 ltrkIso=el_RelIso;
	 lTotIso=el_RelIso;
       }
	  
       /// Apply The Loose Cuts
       el_PassEIDcnt=-1;
       el_PassEID=false;
       if ( elIter->isEB() ) {
	 el_PassEIDcnt=0;
	 if ( el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEB ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaPhi<elLoose_aDeltaPhiMaxEB ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaEta<elLoose_aDeltaEtaMaxEB ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_HoE<elLoose_HoEMaxEB ) {
	   el_PassEIDcnt++;
	 }
       }
       if ( elIter->isEE() ) {
	 el_PassEIDcnt=0;
	 if ( el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaPhi<elLoose_aDeltaPhiMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_aDeltaEta<elLoose_aDeltaEtaMaxEE ) {
	   el_PassEIDcnt++;
	 }
	 if ( el_HoE<elLoose_HoEMaxEE ) {
	   el_PassEIDcnt++;
	 }
       }
       if ( el_PassEIDcnt==4 ) {
	 el_PassEID=true;
       }
       
       
       if ( 
	   (el_pt>elLoose_ptMin)&&
	   el_aetaPass&&
	   (el_RelIso<elLoose_RelIsoMax)&&
	   el_PassEID
	   ) {
	 elcnt_Loose++;
       }

       //lpCounter++;
     }

     ////// ********************************    Muon Selectione (No Jet Cleaning)    ******************************** //////
     mucnt_Prim=0;
     mucnt_Loose=0;
     //lpCounter=0;

     const vector< pat::Muon >::const_iterator muEnd = muonHandle->end();
     for (vector< pat::Muon >::const_iterator muIter = muonHandle->begin(); ( muEnd != muIter ); ++muIter ) { 
       mu_pt=muIter->pt();
       mu_eta=muIter->eta();
       mu_phi=muIter->phi();
       mu_RelIso=(muIter->trackIso())/mu_pt;

       muisGmTm=false;
       if ( (muIter->isGlobalMuon())&&(muIter->isTrackerMuon()) ) {
	 muisGmTm=true;
       }
       muisTightPrompt=false;
       if ( muIter->globalTrack().isNonnull() ) {
	 if ( ((muIter->globalTrack()->normalizedChi2())<10)&&((muIter->globalTrack()->hitPattern().numberOfValidMuonHits())>0) ) {
	   muisTightPrompt=true;
	 }
       }
       if ( muIter->innerTrack().isNonnull() ) {
	 muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
       } else {
	 muPrim_itNHits=-1;
       }
       
       mu_vtxPass=false;
       if ( abs(zvtx - muIter->vertex().z())<1 ) {
	 mu_vtxPass=true;
       }
       
       /// Apply The Primary Cuts
       if ( muisGmTm&&
	    (mu_pt>muPrim_ptMin)&&
	    (abs(mu_eta)<muPrim_aetaMax)&&
	    muisTightPrompt&&
	    (muPrim_itNHits>10)&&
	    ((muIter->dB())<muPrim_dBMax)&&
	    mu_vtxPass&&
	    (muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>=1)&&
	    (muIter->numberOfMatches()>1)
	    ) {
	 /// Either select or antiselect
	 isProperlyIsolatedMu=false;
// 	 if ( ( (mu_RelIso<muPrim_RelIsoMax)&&(!modename.Contains("PFlowLoose")) )||
// 	      ( (mu_RelIso>muPrim_RelIsoMax)&&(modename.Contains("PFlowLoose")) )
// 	      ) {
	 if ( mu_RelIso<muPrim_RelIsoMax ) {
	   isProperlyIsolatedMu=true;
	 }
	 /// Record the muon quantities:
	 if ( isProperlyIsolatedMu ) {
	   mucnt_Prim++;
	   lp4=muIter->p4();
	   lQ=muIter->charge();
	   l_Eta=mu_eta;
	   l_Phi=mu_phi;
	   ltrkIso=mu_RelIso;
	   lTotIso=mu_RelIso;
	 }
       }

       /// Apply The Loose Cuts
       if ( (muIter->isGlobalMuon())&&(mu_pt>muLoose_ptMin)&&(abs(mu_eta)<muLoose_aetaMax)&&
	    //	    ( ((!modename.Contains("PFlowLoose"))&&(mu_RelIso<muLoose_RelIsoMax))||((modename.Contains("PFlowLoose"))&&(mu_RelIso>muPrim_RelIsoMax)) )
	    (mu_RelIso<muLoose_RelIsoMax)
	    ) {
	 mucnt_Loose++;	       
       }

     }

     ////// ********************************     Jet Selection (without muon cleaning)    ******************************** //////
     //// Electron Cleaning (i.e. remove the jets which are likely to be electorns using the DR cut).
     //// Using PF Jets
     jcnt_tot=0;
     j_DRlepton=100;
     Mjj=-1;
     
     jNEntries=0;
     jp4.clear();
     JEta.clear();
     JPhi.clear();
     nBtag=0;
     jBtag.clear();

//       //// JES Uncertainty : tested & works ////
//       double uncer;
//       j_pt=25.5;
//       j_eta=1.3;
//       cout << "JES UNCERTAINTY" << endl;
//       string JEC_PATH("CondFormats/JetMETObjects/data/");
//       //edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5Calo.txt");
//       edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5PF.txt");
//       // edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5JPT.txt");
//       JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(fip.fullPath());
//       jecUnc->setJetEta(j_eta); // Give rapidity of jet you want uncertainty on
//       jecUnc->setJetPt(j_pt);// Also give the corrected pt of the jet you want the uncertainty on
//       // The following function gives the relative uncertainty in the jet Pt.
//       // i.e. ptCorSmeared = (1 +- uncer) * ptCor  
//       uncer = jecUnc->getUncertainty(true); // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.
//       cout << "pt=" << j_pt << " eta=" << j_eta << " : uncer=" << uncer << endl;

     const vector< pat::Jet >::const_iterator pfJetEnd = pfJetHandle->end();
     for (vector< pat::Jet >::const_iterator jetIter = pfJetHandle->begin(); ( pfJetEnd != jetIter ); ++jetIter ) { 
       j_pt=jetIter->pt();
       j_eta=jetIter->eta();
       j_phi=jetIter->phi();

       adphi=abs(l_Phi-j_phi);
       //we're on a circle
       if ( adphi>pi_ ) {
	 adphi=2*pi_-adphi;
       }
       j_DRlepton=pow((j_eta-l_Eta)*(j_eta-l_Eta)+adphi*adphi,0.5);
       
       
       /// Apply The Cut:
       if ( (j_pt>j_ptMin)&&
	    (abs(j_eta)<j_aetaMax)&&
	    ( ((j_DRlepton>j_DRelMin)&&(elcnt_Prim==1))||((j_DRlepton>muPrim_DRjMin)&&(mucnt_Prim==1)) )&&
	    (jetIter->neutralHadronEnergyFraction()<0.99)&&
	    (jetIter->neutralEmEnergyFraction()<0.99)&&
	    (jetIter->numberOfDaughters()>1)&&
	    (jetIter->chargedHadronEnergyFraction()>0)&&
	    (jetIter->chargedMultiplicity()>0)&&
	    (jetIter->chargedEmEnergyFraction()<0.99)
	    ) {
	 
	 jcnt_tot++;
	  
	 // store the parameters 
	 jp4.push_back(jetIter->p4());
	 JEta.push_back(j_eta); 
	 JPhi.push_back(j_phi);
	 jNEntries++;
	 
	 /// b-tagging:
	 if ( (jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags"))>bDiscriminator_min ) {
	   jBtag.push_back(1);
	   nBtag++;
	 } else {
	   jBtag.push_back(0);
	 }
	 
       } // for Selection Cuts
       
     } // for jetIter
     
     
     /// Compute the invariant mass, when only two jets are present
     if ( jcnt_tot==2 ) {
       Mjj=(jp4[0]+jp4[1]).M();
     } else {
       Mjj=-2;
     }

     //// MET Selection:
     METp4=METHandle->front().p4();
     MET_Et=METp4.E();
     
     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /////// ****************************************Record The Results**************************************** ////////////////
     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     
     //// Record the number of events passing the cuts for each jet count
     el_passStandard=false;
     el_passAll=false;
     mu_passStandard=false;
     mu_passAll=false;


     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {
	 PassLp[1][Nj]++;
	 PassMu[1][Nj]++;
	 PassEl[1][Nj]++;
	  
	 //record the electron info
	 if ( (elcnt_Prim>=1)&&PVfound ) {
	   incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	   if ( mucnt_Loose==0 ) {
	     incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	     if ( elcnt_Loose==1 ) {
	       incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	       //passStandard=2;
	       el_passStandard=true;
	       if ( MET_Et>MET_EtMin ) {
		 incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		 el_passAll=true;
	       }
	       /// Record the Btag info
	       if ( nBtag==0 ) {
		 incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	       } else {
		 if ( nBtag==1 ) {
		   incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		 } else {
		   if ( nBtag==2 ) {
		     incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		   } else {
		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		   }
		 }
	       }//BTags
	     }
	   }
	 }//electrons
	 
	 //record the muon info
	 if ( (mucnt_Prim>=1)&&PVfound ) {
	   incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   if ( mucnt_Loose==1 ) {
	     incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	     if ( elcnt_Loose==0 ) {
	       incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	       mu_passStandard=true;
	       if ( MET_Et>MET_EtMin ) {
		 incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		 mu_passAll=true;
	       }
	       /// Record the Btag info
	       if ( nBtag==0 ) {
		 incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	       } else {
		 if ( nBtag==1 ) {
		   incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		 } else {
		   if ( nBtag==2 ) {
		     incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		   } else {
		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
		   }
		 }
	       }//BTags
	     }
	   }
	 }//muons
	 
       }
     } // for Njets


     ///Fill the Ntuple to be used in Matrix Element computations (requiring two jets & a lepton passing all of the cuts).
     //Usual option: fill in the elements which pass all of the cuts, *except* the MET cut:
     el_passFlag=el_passAll;
     mu_passFlag=mu_passAll;
     if ( modename.Contains("noMETcut") ) {
       el_passFlag=el_passStandard;
       mu_passFlag=mu_passStandard;
     }
     if ( modename.Contains("elONLY") ) {
       mu_passFlag=false;
     }
     if ( modename.Contains("muONLY") ) {
       el_passFlag=false;
     }
// EvtNtuple->evtCat=DEFS::electron;

     if ( (el_passFlag||mu_passFlag)&&(jcnt_tot==2) ) {
       EvtNtuple->lLV.clear();
       lp4LV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
       lp3.SetXYZ(lp4.Px(),lp4.Py(),lp4.Pz());
       EvtNtuple->lLV.push_back(lp4LV);

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


       EvtNtuple->jBtagSSV.clear();
       EvtNtuple->jBtagSSV.push_back(jBtag[0]);
       EvtNtuple->jBtagSSV.push_back(jBtag[1]);
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
       pair<int, TLorentzVector> matchedPair;
       if ( !modename.Contains("Data") ) {
	 matchedPair = matchToGen(j1p4LV.Eta(),j1p4LV.Phi(),genParticleHandle);
	 EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
	 EvtNtuple->matchedpdgId.push_back(matchedPair.first);
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
//        EvtNtuple->passStd=passStandard;
//        EvtNtuple->passAll=passAll;
       EvtNtuple->leptonCat=DEFS::none;
       EvtNtuple->leptonCat_passAll=DEFS::none;
       if ( el_passFlag ) {
	 EvtNtuple->leptonCat=DEFS::electron;
       }
       if ( mu_passFlag ) {
	 EvtNtuple->leptonCat=DEFS::muon;
       }
       if ( el_passAll ) {
	 EvtNtuple->leptonCat_passAll=DEFS::electron;
       }
       if ( mu_passAll ) {
	 EvtNtuple->leptonCat_passAll=DEFS::muon;
       }

       DRlj1=lp4LV.DeltaR(j1p4LV);
       DRlj2=lp4LV.DeltaR(j2p4LV);
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
  }
  
  
  InitializeLabels(PLabel,CLabel);
  writeIntProcessTable(outtablefile,PassEl,PassMu,PassLp);


  outrootfile->Write();
  outrootfile->Close();
  
  return 0;
  
}


