/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Use to skim the PATNtuples and create a custom made .root file containing the needed information.

// // system include files
// #include <memory>

// // user include files
// #include "FWCore/Framework/interface/Frameworkfwd.h"
// #include "FWCore/Framework/interface/EDAnalyzer.h"

// #include "FWCore/Framework/interface/Event.h"
// #include "FWCore/Framework/interface/MakerMacros.h"

// #include "FWCore/ParameterSet/interface/ParameterSet.h"

// CMS includes
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
//#include "DataFormats/JetReco/interface/Jet.h"
//#include "DataFormats/JetReco/interface/PFJet.h"
//#include "DataFormats/PatCandidates/interface/PATObject.h"
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

//#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
//#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
//#include "MagneticField/Engine/interface/MagneticField.h"
//#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackExtra.h"

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
#include <TAMUWW/Tools/GlobalTools_HWWMC_Ex3.cc>

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
  optutl::CommandLineParser parser ("Plots Jet Pt");
   
  ////////////////////////////////////////////////
  // Change any defaults or add any new command //
  //      line options you would like here.     //
  ////////////////////////////////////////////////
  
  //// Output Files:
  // corresponds to the last string on the command line
  TString modename;
  modename=argv[argc-1];
  // To record the output in ascii format:
  ofstream outResFile;
  outResFile.open(modename,ios::out);

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
  TTree *SyncTree = new TTree("SyncTree","SyncTree"); //Temporary for Synchronization.
//   TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
//   EventNtuple * EvtNtuple = new EventNtuple();
//   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);

  //// Define the constants
  double pi_=TMath::Pi();


  //////////////////////// Selection Parameters and Cut Values ////////////////////////////////
  //// Vertex:
  int vtx_ndofMin=4;
  double vtx_zMax=24.0;
  double vtx_RhoMax=2.0;
  vector <double> PVz; 

  //// Jets (apply after the Primary Electrons but before the Primary Muons):
  vector <math::XYZTLorentzVector> jp4, jp4_temp;
  vector <double> JEta, JEta_temp, JPhi, JPhi_temp;
  double j_pt;
  double j_ptMin=20.0;
  double j_eta;
  double j_aetaMax=2.4;
  double j_DRel;
  double j_DRelMin=0.3;
  double adphi;
  double muPrim_DRjMin=0.1;

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
  double el_pt;
  double elPrim_ptMin=20.0;
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
//   int elPrim_eid1=1;
//   int elPrim_eid2=3;
//   int elPrim_eid3=5;
//   int elPrim_eid4=7;
  int elPrim_trackerInnerHitsMin=0;

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

  //// Loose Electrons (used in muon selection):
  double elLoose_ptMin=15.0;
  double elLoose_aetaMax=2.5;
  double elLoose_RelIsoMax=0.2;
  double elLoose_sigmaIetaIetaMaxEB=0.01;
  double elLoose_aDeltaPhiMaxEB=0.8;
  double elLoose_aDeltaEtaMaxEB=0.007;
  double elLoose_HoEMaxEB=0.15;
  double elLoose_sigmaIetaIetaMaxEE=0.03;
  double elLoose_aDeltaPhiMaxEE=0.7;
  double elLoose_aDeltaEtaMaxEE=0.01;
  double elLoose_HoEMaxEE=0.07;


  double elLoose_Pt;
  double elLoose_Eta;
  double elLoose_Iso;
  bool elLoose_isEB;
  bool elLoose_isEE;
  double elLoose_sigmaIetaIeta;
  double elLoose_aDeltaPhi;
  double elLoose_aDeltaEta;
  double elLoose_HoEMax;

//   //// Electron ZVeto (used in electron selection):
//   double elZVeto_etMin=20.0;
//   double elZVeto_aetaMax=2.5;
//   double elZVeto_RelIsoMax=1.0;
//   double elZVeto_MZMin=76.0;
//   double elZVeto_MZMax=106.0;

//   int elZVeto_eid1=1;
//   int elZVeto_eid2=3;
//   int elZVeto_eid3=5;
//   int elZVeto_eid4=7;

  /// Equivalent Quantities:


  /// Electron Flags
  bool el_vtxPass=false;
  bool el_aetaPass=false;
  bool el_convPass_a=false;
  bool el_convPass_b=false;
  bool el_ZVeto=false;
	 
  /// Primary Muons (used in muon selection):
  double mu_pt;
  double muPrim_ptMin=20.0;
  double mu_eta;
  double mu_Eta=-1;
  double mu_phi;
  double mu_Phi=-1;

  double mu_Pt;
  double mu_Iso;
  bool mu_isGlobalMuon;
  bool mu_isTrackerMuon;
  double mu_normalizedChi2;
  int mu_numberOfValidMuonHits;
  int mu_numberOfValidHits;
  double mu_dz;
  double mu_dB;
  int mu_pixelLayersWithMeasurement;
  int mu_numberOfMatches;



//   vector <math::XYZTLorentzVector> Mup4,Mup4_temp;
//   double mu_ecalIso, mu_hcalIso, mu_trkIso;
//   vector <double> MuEta,MuEta_temp,MuPhi,MuPhi_temp,MuecalIso,MuecalIso_temp,MuhcalIso,MuhcalIso_temp,MutrkIso,MutrkIso_temp;
//   vector <int> MuQ,MuQ_temp;
      
  double muPrim_aetaMax=2.1;
  //  int muPrim_gtNormChi2Max=10;
  //  int muPrim_gtNMuHitsMin=10;
  double muPrim_itNHits;
  //  double muPrim_itNHitsMin=10.5;//??
  double muPrim_DRj;
//   double muPrim_DRjLowerMax=0.1;//Reject *jets* with DRj<DRjLowerMax
//   double muPrim_DRjUpperMax=0.3;//Reject *muons* with DRj<DRjUpperMax
  double muPrim_dBMax=0.02;
  double mu_RelIso;
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
  bool RecordEvent;
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
  int PVcnt=0;
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
      //fwlite::Handle< vector< reco::Vertex > > vtxHandle;
      fwlite::Handle< reco::VertexCollection > vtxHandle;
      vtxHandle.getByLabel (eventCont, "offlinePrimaryVertices");
      assert ( vtxHandle.isValid() );
      //const fwlite::Handle< reco::VertexCollection > myvtxHandle=vtxHandle;

      fwlite::Handle< vector< pat::Electron > > electronHandle;
      electronHandle.getByLabel (eventCont, "selectedPatElectrons");
      assert ( electronHandle.isValid() );

      fwlite::Handle< vector< pat::Jet > > pfJetHandle;
      pfJetHandle.getByLabel (eventCont, "selectedPatJetsAK5PF");
      assert ( pfJetHandle.isValid() );
 
      fwlite::Handle< vector< pat::Muon > > muonHandle;
      muonHandle.getByLabel (eventCont, "selectedPatMuons");
      assert ( muonHandle.isValid() );

      fwlite::Handle< vector< pat::MET > > METHandle;
      METHandle.getByLabel (eventCont, "patMETsPF");
      assert ( METHandle.isValid() );



      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ***************************************Perform The Selection*************************************** ///////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      RecordEvent=false;

      ////// ********************************     Vertex Selection     ******************************** //////
      reco::Vertex pv;
      bool PVfound = (vtxHandle -> size() != 0);
      double zvtx = -999;

     // Sort PV collection
     if(PVfound){
       PrimaryVertexSorter PVSorter;
       std::vector<reco::Vertex> sortedVertices = PVSorter.sortedList( *(vtxHandle.product()) );
       pv = sortedVertices.front();
       // vtxcnt++;
       zvtx = pv.z();
     } else {
       throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event." << std::endl;
     }






      PVcnt=0;
      PVz.clear();
      const vector< reco::Vertex >::const_iterator vtxEnd = vtxHandle->end();
      for (vector< reco::Vertex >::const_iterator vtxIter = vtxHandle->begin(); ( vtxEnd != vtxIter ); ++vtxIter ) {
	if ( (vtxIter->ndof()>=4)&&(abs(vtxIter->z())<24)&&(vtxIter->position().Rho()<2.0) ) {
	  PVz.push_back(vtxIter->z());
	  PVcnt++;
	}
      }
      if ( PVcnt>0 ) {
	vtxcnt++;
      } else {
	cout << "EVENT DID NOT PASS THE VERTEX CUT" << " : eventNumber=" << eventNumber << endl;
      }


      ////// ********************************     Electron Selection     ******************************** //////
      elcnt_Prim=0;
      elcnt_Loose=0;
      el1Marker=-1;
      lpCounter=0;
      
      const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
      for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 	
	//el_pt=elIter->pt();
	el_pt=elIter->p4().Pt();
	el_eta=elIter->eta();
	el_aetasc=abs(elIter->superCluster()->eta());

	el_aetaPass=false;
	if ( abs(el_eta)<elPrim_aetaMax ) {
	  el_aetaPass=true; //noscv
	}
	if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
	  el_aetaPass=false;
	}

 	/// We can either directly apply the electronID cuts or look at each individual step:
	//temp//
	//el_eid=elIter->electronID("simpleEleId70relIso");
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
	if ( el_PassEIDcnt==4 ) {
	  el_PassEID=true;
	}

	//el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_pt;
	//el_RelIso=(elIter->trackIso())/el_pt;
	el_RelIso=(elIter->dr03TkSumPt())/el_pt;

	///Electron Conversion
	el_convPass_a=true;
	if ( elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()!=0 ) {
	  el_convPass_a=false;
	}
	el_convPass_b=true;
	if ( (abs(elIter->convDist())<0.02)&&(abs(elIter->convDcot())<0.02) ) {
	  el_convPass_b=false;
	}

	/// Apply The Primary Cuts
	if ( (el_pt>elPrim_ptMin)&&
	     el_aetaPass&&
	     (el_RelIso<elPrim_RelIsoMax)&&
	     el_PassEID&&
	     (abs(elIter->dB())<elPrim_dBMax)&&
	     el_convPass_a&&
	     el_convPass_b
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
	  lecalIso=(elIter->dr03EcalRecHitSumEt())/el_pt;
	  lhcalIso=(elIter->dr03HcalTowerSumEt())/el_pt;
	  ltrkIso=(elIter->dr03TkSumPt())/el_pt;
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

// 	  elLoose_Pt=el_pt;
// 	  elLoose_Eta=el_eta;
// 	  elLoose_Iso=el_RelIso;
// 	  elLoose_isEB=elIter->isEB();
// 	  elLoose_isEE=elIter->isEE();
// 	  elLoose_sigmaIetaIeta=el_sigmaIetaIeta;
// 	  elLoose_aDeltaPhi=el_aDeltaPhi;
// 	  elLoose_aDeltaEta=el_aDeltaEta;
// 	  elLoose_HoEMax=el_HoE;
   
	}
	
	lpCounter++;
      }

      ////// ********************************    Muon Selection    ******************************** //////
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
       
       //mu_vtxPass=true;
       mu_vtxPass=false;
       if ( abs(zvtx - muIter->vertex().z())<1 ) {
	 mu_vtxPass=true;
       }

       /// Apply The Primary Cuts
       if (  
	     muisGmTm&&
	     (mu_pt>muPrim_ptMin)&&
	     (abs(mu_eta)<muPrim_aetaMax)&&
	     (mu_RelIso<muPrim_RelIsoMax)&&
	     muisTightPrompt&&
	     (muPrim_itNHits>10)&&
	     (abs(muIter->dB())<muPrim_dBMax)&&
	     //((-1.0 * muIter->innerTrack->dxy(beamPoint))<muPrim_dBMax)&&
	     mu_vtxPass&&
	     (muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>=1)&&
	     (muIter->numberOfMatches()>1)
	     ) {
	 
	 /// Record the muon quantities:
	 mucnt_Prim++;
	 lp4=muIter->p4();
	 lQ=muIter->charge();

	 mu_Pt=mu_pt;
	 mu_Iso=mu_RelIso;
	 mu_isGlobalMuon=muIter->isGlobalMuon();
	 mu_isTrackerMuon=muIter->isTrackerMuon();
	 mu_normalizedChi2=muIter->globalTrack()->normalizedChi2();
	 mu_numberOfValidMuonHits=muIter->globalTrack()->hitPattern().numberOfValidMuonHits();
	 mu_numberOfValidHits=muPrim_itNHits;
	 //mu_dz=abs(Vtx.z() - muIter->vertex().z());
	 mu_dB=muIter->dB();
	 mu_pixelLayersWithMeasurement=muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement();
	 mu_numberOfMatches=muIter->numberOfMatches();

	 mu_Eta=mu_eta;
	 mu_Phi=muIter->phi();
	 lTotIso=mu_RelIso;
       }

       /// Apply The Loose Cuts
       if ( (muIter->isGlobalMuon())&&(mu_pt>muLoose_ptMin)&&(abs(mu_eta)<muLoose_aetaMax)&&(mu_RelIso<muLoose_RelIsoMax) ) {
	 mucnt_Loose++;	       
       }

      }
       


      ////// ********************************     Jet Selection (without muon cleaning)    ******************************** //////
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

      //cout << "event=" << eventNumber << " : mucnt_Prim=" << mucnt_Prim << " mucnt_Loose=" << mucnt_Loose << " elcnt_Prim=" << elcnt_Prim << " elcnt_Loose=" << elcnt_Loose << endl;
      const vector< pat::Jet >::const_iterator pfJetEnd = pfJetHandle->end();
      for (vector< pat::Jet >::const_iterator jetIter = pfJetHandle->begin(); ( pfJetEnd != jetIter ); ++jetIter ) {
	j_pt=jetIter->pt();
	j_eta=jetIter->eta();




	adphi=abs(el_Phi-jetIter->phi());
	//we're on a circle
	if ( adphi>pi_ ) {
	  adphi=2*pi_-adphi;
	}
	j_DRel=pow((j_eta-el_Eta)*(j_eta-el_Eta)+adphi*adphi,0.5);

	adphi=abs(mu_Phi-jetIter->phi());
	//we're on a circle
	if ( adphi>pi_ ) {
	  adphi=2*pi_-adphi;
	}
	muPrim_DRj=pow((j_eta-mu_Eta)*(j_eta-mu_Eta)+adphi*adphi,0.5);

	//	cout << "j_pt=" << j_pt << endl;
	/// Apply The Cut:
	if ( (j_pt>20.0)&&
	     (abs(j_eta)<j_aetaMax)&&
	     ( ((j_DRel>j_DRelMin)&&(elcnt_Prim==1))||((muPrim_DRj>muPrim_DRjMin)&&(mucnt_Prim==1)) )&&
	     (jetIter->neutralHadronEnergyFraction()<0.99)&&
	     (jetIter->neutralEmEnergyFraction()<0.99)&&
	     (jetIter->numberOfDaughters()>1)&&
	     (jetIter->chargedHadronEnergyFraction()>0)&&
	     (jetIter->chargedMultiplicity()>0)&&
	     (jetIter->chargedEmEnergyFraction()<0.99)
	     ) {
	  //cout << "Jet Passes the cuts (j_pt=" << j_pt << ")" << endl;
	  jcnt_tot++;	  
	  
	  // store the parameters 
	  jp4.push_back(jetIter->p4());
	  JEta.push_back(j_eta); 
	  JPhi.push_back(jetIter->phi());
	  jNEntries++;
	  
	  /// b-tagging:
	  //cout << "bDiscriminator=" << jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags") << endl;
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

//       //// MET Selection:
//       METp4=METHandle->front().p4();
//       MET_Et=METp4.E();
//       //MET_Et=METHandle->front().et();


     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /////// ****************************************Record The Results**************************************** ////////////////
     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     //// scan over the jets
     passAll=0;
     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {
	 
	 //electrons:
	 if ( elcnt_Prim>=1 ) {
	   RecordEvent=true;
	   //incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	   if ( mucnt_Loose==0 ) {
	     //incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	     if ( elcnt_Loose==1 ) {
	       //incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);

	       for (Int_t nj=0; nj<(Nj+1);nj++) {
		 PassLp[7][nj]++;
		 //PassMu[7][nj]++;
		 PassEl[7][nj]++;
	       }
		   
	       if ( mucnt_Prim==1 ) {
		 passAll=1;
	       }
	       if ( elcnt_Prim==1 ) {
		 passAll=2;
	       }
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
	 //muons:
	 if ( mucnt_Prim>=1 ) {
	   //RecordEvent=true;
	   //incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   if ( mucnt_Loose==1 ) {
	     //incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	     if ( elcnt_Loose==0 ) {
	       //RecordEvent=true;
	       //incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	       
	       for (Int_t nj=0; nj<(Nj+1);nj++) {
		 PassLp[7][nj]++;
		 PassMu[7][nj]++;
		 //PassEl[7][nj]++;
	       }
	       
	       if ( mucnt_Prim==1 ) {
		 passAll=1;
	       }
	       if ( elcnt_Prim==1 ) {
		 passAll=2;
	       }
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
	   }
	 }
	 
	 
       }
     } // for Njets

     // record the results
     cout << "event=" << eventNumber << endl;

     if ( (eventNumber==108805)||(eventNumber==30281)||(eventNumber==3873)||(eventNumber==25544) ) {
       PVcnt=0;
       const vector< reco::Vertex >::const_iterator vtxEnd = vtxHandle->end();
       for (vector< reco::Vertex >::const_iterator vtxIter = vtxHandle->begin(); ( vtxEnd != vtxIter ); ++vtxIter ) {
	 cout << "ndof=" << vtxIter->ndof() << " vtxz=" << vtxIter->z() << " vtxRho=" << vtxIter->position().Rho() << endl;
       }

       int Nlepton=0;
       cout << "EXAMINING EVENT " << eventNumber << endl;

       ///Muons:
       double gt_Chi2;
       int gt_numberOfValidMuonHits;
       cout << " mucnt_Prim=" << mucnt_Prim << " mucnt_Loose=" << mucnt_Loose << endl;
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
	 gt_Chi2=-1;
	 gt_numberOfValidMuonHits=-1;
	 if ( muIter->globalTrack().isNonnull() ) {
	   if ( ((muIter->globalTrack()->normalizedChi2())<10)&&((muIter->globalTrack()->hitPattern().numberOfValidMuonHits())>0) ) {
	     muisTightPrompt=true;
	   }
	   gt_Chi2=muIter->globalTrack()->normalizedChi2();
	   gt_numberOfValidMuonHits=muIter->globalTrack()->hitPattern().numberOfValidMuonHits();
	 }
	 if ( muIter->innerTrack().isNonnull() ) {
	   muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
	 } else {
	   muPrim_itNHits=-1;
	 }
	 
	 mu_vtxPass=false;
	 //for (Int_t i=0; i<PVcnt;i++) {
	 if ( abs(zvtx - muIter->vertex().z())<1 ) {
	   mu_vtxPass=true;
	 }
	 //}

	 /// Record the muon quantities:
	 cout << "Nmuon=" << Nlepton << endl;
	 Nlepton++;



	 cout << "mu_Pt=" << mu_pt << " mu_Eta=" << mu_eta << " mu_Iso=" << mu_RelIso << " mu_isGlobalMuon=" << muIter->isGlobalMuon() << " mu_isTrackerMuon=" << muIter->isTrackerMuon() << " mu_normalizedChi2=" << gt_Chi2 << " mu_numberOfValidMuonHits=" << gt_numberOfValidMuonHits << " mu_numberOfValidHits=" << muPrim_itNHits << " mu_dB=" << muIter->dB() << " mu_pixelLayersWithMeasurement=" << muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement() << " mu_numberOfMatches=" << muIter->numberOfMatches() << " mu_dz=" << abs(zvtx - muIter->vertex().z()) << endl;
	 
	 cout << "mu_vtxPass=" << mu_vtxPass << endl;
	 //<< " mu_dz=" << abs(Vtx.z() - muIter->vertex().z())

	 //        cout << " mucnt_Prim=" << mucnt_Prim << "mucnt_Loose=" << mucnt_Loose << " mu_Pt=" << mu_Pt << " mu_Eta=" << mu_Eta << " mu_Iso=" << mu_Iso << " mu_isGlobalMuon=" << mu_isGlobalMuon << " mu_isTrackerMuon=" << mu_isTrackerMuon << " mu_normalizedChi2=" << mu_normalizedChi2 << " mu_numberOfValidMuonHits=" << mu_numberOfValidMuonHits << " mu_numberOfValidHits=" << mu_numberOfValidHits << " mu_dz=" << mu_dz << " mu_dB=" << mu_dB << " mu_pixelLayersWithMeasurement=" << mu_pixelLayersWithMeasurement << " mu_numberOfMatches=" << mu_numberOfMatches << endl;
       }
       
       ///Electrons:
       Nlepton=0;
       cout << " elcnt_Prim=" << elcnt_Prim << " elcnt_Loose=" << elcnt_Loose << endl;
       const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
       for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 
	 cout << "Nelectron=" << Nlepton << endl;
	 Nlepton++;
	 //el_pt=elIter->pt();
	 el_pt=elIter->p4().Pt();
	 el_eta=elIter->eta();
	 el_aetasc=abs(elIter->superCluster()->eta());
	 
	 el_aetaPass=false;
	 if ( abs(el_eta)<elPrim_aetaMax ) {
	   el_aetaPass=true; //noscv
	 }
	 if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
	   el_aetaPass=false;
	 }
	 
	 /// We can either directly apply the electronID cuts or look at each individual step:
	 //temp//
	 //el_eid=elIter->electronID("simpleEleId70relIso");
	 el_PassEIDcnt=-1;
	 el_PassEID=false;
	 el_sigmaIetaIeta=elIter->sigmaIetaIeta();
	 el_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
	 el_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
	 el_HoE=elIter->hadronicOverEm();
	 //el_RelIso=(elIter->trackIso())/el_pt;
	 el_RelIso=(elIter->dr03TkSumPt())/el_pt;

	///Electron Conversion
	el_convPass_a=true;
	if ( elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()!=0 ) {
	  el_convPass_a=false;
	}
	el_convPass_b=true;
	if ( (abs(elIter->convDist())<0.02)&&(abs(elIter->convDcot())<0.02) ) {
	  el_convPass_b=false;
	}

	cout << "el_Pt=" << el_pt << " el_Eta=" << el_eta << " el_Iso=" << el_RelIso << " el_dB=" << elIter->dB() << " el_numberOfHits=" << elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits() << " el_aconvDist=" << abs(elIter->convDist()) << " el_aconvDcot" << abs(elIter->convDcot()) << " el_isEB=" << elIter->isEB() << " el_isEE=" << elIter->isEE() << " el_sigmaIetaIeta=" << el_sigmaIetaIeta << " el_aDeltaPhi=" << el_aDeltaPhi << " el_aDeltaEta=" << el_aDeltaEta << " el_HoEMax=" << el_HoE << endl;

       }

     }
     
     if ( RecordEvent==true ) {
       //check if we want elEt, elPt or muPt
       SyncTree->Fill();
       outResFile << eventNumber ;

       outResFile << endl;
     }
     
 
     EvtTotCount++;

     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       
       //electrons
       if ( (elcnt_Prim>=1) ) {
	 incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,1);
	 if ( mucnt_Loose==0 ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,1);
	   if ( elcnt_Loose==1 ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,1);
	     incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,1);
	     incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,1);
	   }
	 }
       }

       //muons
       if ( mucnt_Prim>=1 ) {
	 incrementCounter(2,Nj,PassEl,PassMu,PassLp,1,0);
	 if ( mucnt_Loose==1 ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,1,0);
	   if ( elcnt_Loose==0 ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,1,0);
	     incrementCounter(5,Nj,PassEl,PassMu,PassLp,1,0);
	     incrementCounter(6,Nj,PassEl,PassMu,PassLp,1,0);
	   }
	 }
       }

     }

//      ///TEMPORARY (for syncronization):
//      EvtTree->Fill();



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


