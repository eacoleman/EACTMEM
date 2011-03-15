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
#include <TAMUWW/Tools/GlobalTools_SyncExV4.cc>

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
  TTree *EvtTree = new TTree("EvtTree", "EvtTree"); //Temporary for Synchronization.
//   TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
//   EventNtuple * EvtNtuple = new EventNtuple();
//   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);

  //////////////////////// Selection Parameters and Cut Values ////////////////////////////////
  /// Vertex:
  double vtx_ndofMin=4.5;
  double vtx_zMax=24.0;
  double vtx_RhoMax=2.0;

  /// Jets (apply after the Primary Electrons but before the Primary Muons):
  double j_pt;
  double j_ptMin=30.0;
  double j_eta;
  double j_aetaMax=2.4;
  double j_DRel;
  double j_DRelMin=0.3;
  double pi_=TMath::Pi();
  double el_Eta=-1;
  double el_Phi=-1;
  double adphi;

  //Loose JetID Cuts (https://twiki.cern.ch/twiki/bin/view/CMS/JetID)
  double jcalo_emFracMin=0.01;
  double jcalo_n90HitsMin=1.01;
  double jcalo_fHPDMax=0.98;
  double jpf_neutralHadronEFMax=0.99;
  double jpf_neutralEmEFMax=0.99;
  double jpf_nConstituentsMin=1.5;
  double jpf_chargedHadronEFMin=0.0;
  double jpf_chargedMultiplicityMin=0.0;
  double jpf_chargedEmEFMax=0.99;


  /// Primary Electrons (used in electron selection):
  double el_et;
  double elPrim_etMin=30.0;
  double el_eta;
  double elPrim_aetaMax=2.5;
  double el_aetasc;
  double elPrim_aetascExcludeMax=1.5660;
  double elPrim_aetascExcludeMin=1.4442;
  double elPrim_dBMax=0.02;
  double el_RelIso;
  double elPrim_RelIsoMax=0.1;
  if ( (modename.Contains("QCD"))||(modename.Contains("qcd")) ) {
    ///open the isolation requirement for QCD
    elPrim_RelIsoMax=1.0;
  }
  int el_eid;
  int elPrim_eid1=1;
  int elPrim_eid2=3;
  int elPrim_eid3=5;
  int elPrim_eid4=7;
  int elPrim_trackerInnerHitsMin=0;

  //Equivalent to the EID cut (https://twiki.cern.ch/twiki/bin/view/CMS/SimpleCutBasedEleID)
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

  int el_PassEID;

  /// Loose Electrons (used in muon selection):
  double elLoose_etMin=15.0;
  double elLoose_aetaMax=2.5;
  double elLoose_RelIsoMax=0.2;

  /// Electron ZVeto (used in electron selection):
  double elZVeto_etMin=20.0;
  double elZVeto_aetaMax=2.5;
  double elZVeto_RelIsoMax=1.0;
  double elZVeto_MZMin=76.0;
  double elZVeto_MZMax=106.0;

  int elZVeto_eid1=1;
  int elZVeto_eid2=3;
  int elZVeto_eid3=5;
  int elZVeto_eid4=7;

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
	 
  /// Primary Muons (used in muon selection):
  vector < double > JEta, JPhi;
  double mu_pt;
  double muPrim_ptMin=20.0;
  double mu_eta;
  double mu_phi;
  double muPrim_aetaMax=2.1;
  double muPrim_gtNormChi2Max=10.0;
  double muPrim_gtNMuHitsMin=0.5;
  double muPrim_itNHits;
  double muPrim_itNHitsMin=10.5;
  double muPrim_DRj;
  double muPrim_DRjMax=0.3;
  double muPrim_dBMax=0.02;
  double mu_RelIso;
  double muPrim_RelIsoMax=0.05;
  if ( (modename.Contains("QCD"))||(modename.Contains("qcd")) ) {
    ///open the isolation requirement for QCD
    muPrim_RelIsoMax=1.0;
  }
  bool muisGmTm;
  bool muisTightPrompt;
  bool muDRPass;


  /// Loose Muons (used in both electron and muon selection):
  double muLoose_ptMin=10.0;
  double muLoose_aetaMax=2.5;
  double muLoose_RelIsoMax=0.2;

  /// Optional (i.e. not a part of V2 Synchronization Exercize):
  double MET_Et;
  double MET_EtMin=30;
  double bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.
  double etaBarrelMax=1.442; //The Maximum value of eta for the Barrel 

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
  bool isPrimaryVertex=false;
  bool el_vtxPass=false;
  bool el_aetaPass=false;
  bool el_conv_a=false;
  bool el_conv_b=false;
  int elcnt_Prim=0;
  int elcnt_Loose=0;
  bool IsjBtag=false;
  int jNEntries;
  int mucnt_Prim=0;
  int mucnt_Loose=0;
  bool el_ZVeto=false;
  int jcnt_tot=0;
  int nBtag=0;
  int passAll=0;//=1 for mu, =2 for el if the event passes all of the cuts
  ///Event Based Markers:
  int el1Marker, mu1Marker;
  //  int el2Marker, mu2Marker;
  int lpCounter;

  /// Additional Parameters
  int passStandard=0;//=1 for mu, =2 for el if the event passes all of the cuts
  vector <math::XYZTLorentzVector> jp4;
  math::XYZTLorentzVector lp4, METp4;
  TLorentzVector lp4LV, j1p4LV, j2p4LV, METp4LV;
  TVector3 lp3, jjp3;
  double Thetalj1pj2, DRlj1, DRlj2;
  double lTotIso,lecalIso,lhcalIso,ltrkIso;
  double Mjj=-1;
  int lQ=-10;
  double lEta=-1;
  vector <int> jBtag;

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
  
  // Setup a style
  gROOT->SetStyle ("Plain");
  
  // Book those histograms (not used)
  eventCont.add( new TH1F( "jetPt", "jetPt", 1000, 0, 1000) );
  
  
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
      } //if
      
      //////////////////////////////////
      // Take What We Need From Event //
      //////////////////////////////////
      fwlite::Handle< vector< reco::Vertex > > VtxHandle;
      VtxHandle.getByLabel (eventCont, "offlinePrimaryVertices");
      assert ( VtxHandle.isValid() );

      fwlite::Handle< vector< pat::Electron > > electronHandle;
      electronHandle.getByLabel (eventCont, "selectedPatElectrons");
      assert ( electronHandle.isValid() );
      
      fwlite::Handle< vector< pat::Jet > > caloJetHandle;
      caloJetHandle.getByLabel (eventCont, "selectedPatJets");
      assert ( caloJetHandle.isValid() );
      
      fwlite::Handle< vector< pat::Jet > > pfJetHandle;
      pfJetHandle.getByLabel (eventCont, "selectedPatJetsAK5PF");
      assert ( pfJetHandle.isValid() );

//       fwlite::Handle< vector< reco::PFJet > > pfJetHandle;
//       pfJetHandle.getByLabel (eventCont, "selectedPatJetsAK5PF");
//       assert ( pfJetHandle.isValid() );

      fwlite::Handle< vector< pat::Muon > > muonHandle;
      muonHandle.getByLabel (eventCont, "selectedPatMuons");
      assert ( muonHandle.isValid() );
      
      fwlite::Handle< vector< pat::MET > > METHandle;
      METHandle.getByLabel (eventCont, "patMETsPF");
      assert ( METHandle.isValid() );
      


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////// ***************************************Perform The Selection*************************************** ////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      //// Vertex Selection:
      isPrimaryVertex=false;
      reco::Vertex Vtx=VtxHandle->front();
      if ( (!Vtx.isFake())&&(Vtx.ndof()>vtx_ndofMin)&&(abs(Vtx.z())<vtx_zMax)&&(Vtx.position().Rho()<vtx_RhoMax) ) {
	vtxcnt++;
	isPrimaryVertex=true;
      }
      
      //// Electron Selection:
      elcnt_Prim=0;
      elcnt_Loose=0;
      el_conv_a=false;
      el1Marker=-1;
      //      el2Marker=-1;
      lpCounter=0;
      
      /// Necessary for the conversion rejection b
      el_conv_b=false;
//       fwlite::Handle<reco::TrackCollection> tracks_h;
//       tracks_h.getByLabel(eventCont, "generalTracks");

      double evt_bField=0;
      //fwlite::Handle<MagneticField> magneticField;
      //magneticField.getByLabel (eventCont,IdealMagneticFieldRecord);
      //iSetup.get<IdealMagneticFieldRecord>().get(magneticField);
      //eventCont.get<IdealMagneticFieldRecord>().get(magneticField);


      //evt_bField = magneticField->inTesla(GlobalPoint(0.,0.,0.)).z();

 //      fwlite::Handle<reco::GsfElectron> eHandle;
//       eHandle.getByLabel(eventCont,"selectedPatElectrons");
//       const vector<reco::GsfElectron>::const_iterator GsfelEnd=eHandle->end();

//       for(vector< reco::GsfElectron >::const_iterator el = eHandle->begin(); ( GsfelEnd != el ) ; ++el) {
	

//       }


      //vector< pat::Electron > const & electronVec = *electronHandle;
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
	el_eid=elIter->electronID("simpleEleId70relIso");

	el_PassEID=0;
	elPrim_sigmaIetaIeta=elIter->sigmaIetaIeta();
	elPrim_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
	elPrim_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
	elPrim_HoE=elIter->hadronicOverEm();
	if ( (elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB)&&(elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEB)&&(elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEB)&&(elPrim_HoE<elPrim_HoEMaxEB)&&(elIter->isEB()) ) {
	  el_PassEID=1;
	}
	if ( (elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE)&&(elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEE)&&(elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEE)&&(elPrim_HoE<elPrim_HoEMaxEE)&&(elIter->isEE()) ) {
	  el_PassEID=1;
	}

	el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_et;

	/// Apply The Primary Cuts
	if ( el_vtxPass&&
	     (el_et>elPrim_etMin)&&
	     el_aetaPass&&
	     (abs(elIter->dB())<elPrim_dBMax)&&
	     ((el_eid==elPrim_eid1)||(el_eid==elPrim_eid2)||(el_eid==elPrim_eid3)||(el_eid==elPrim_eid4))&&
	     //(el_PassEID==1)&&
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
	  

	
	if ( el_eid<3.5 ) {
	  el_conv_b=true;
	}


 
	}
	/// Apply The Loose Cuts
// 	if ( (el_et>elLoose_etMin)&&(abs(el_eta)<elLoose_aetaMax)&&(el_RelIso<elLoose_RelIsoMax) ) {
// 	  elcnt_Loose++;	       
// 	}
	
	lpCounter++;
      }
      
      //// JET Selection:
//       /// Calo Jets:
//       jcnt_tot=0;
//       nBtag=0;
//       jBtag.clear();
//       j_DRel=100;
//       Mjj=-1;

//       jp4.clear();
//       jNEntries=0;
//       JEta.clear();
//       JPhi.clear();

//       const vector< pat::Jet >::const_iterator caloJetEnd = caloJetHandle->end();
//       for (vector< pat::Jet >::const_iterator jetIter = caloJetHandle->begin(); ( caloJetEnd != jetIter ); ++jetIter ) { 
// 	j_pt=jetIter->pt();
// 	j_eta=jetIter->eta();
// 	if ( elcnt_Prim==1 ){
// 	  adphi=abs(el_Phi-jetIter->phi());
// 	  //we're on a circle
// 	  if ( adphi>pi_ ) {
// 	    adphi=2*pi_-adphi;
// 	  }
// 	  j_DRel=pow((j_eta-el_Eta)*(j_eta-el_Eta)+adphi*adphi,0.5);
// 	}
	
// 	/// Apply The Cut:
// 	if ( (j_pt>j_ptMin)&&
// 	     (abs(j_eta)<j_aetaMax)&&
// 	     ((jetIter->emEnergyFraction())>jcalo_emFracMin)&&
// 	     ((jetIter->jetID().n90Hits)>jcalo_n90HitsMin)&&
// 	     ((jetIter->jetID().fHPD)<jcalo_fHPDMax)&&
// 	     (j_DRel>j_DRelMin)
// 	     ) {
	  
// 	  jcnt_tot++;
	  
// 	  // store parameters for muon selection
// 	  JEta.push_back(j_eta); 
// 	  JPhi.push_back(jetIter->phi());
// 	  jNEntries++;
	  
// 	  /// b-tagging:
// 	  if ( (jetIter->bDiscriminator("simpleSecondaryVertexBJetTags"))>bDiscriminator_min ) {
// 	    IsjBtag=true;
// 	    nBtag++;
// 	  } else {
// 	    IsjBtag=false;
// 	  }
// 	  if ( jcnt_tot==1 ) {
// 	    jp4.push_back(jetIter->p4());
// 	    if (IsjBtag==true) {
// 	      jBtag.push_back(1);
// 	    } else {
// 	      jBtag.push_back(0);
// 	    }
// 	  }
// 	  /// Compute the invariant mass, when only two jets are present
// 	  if ( jcnt_tot==2 ) {
// 	    jp4.push_back(jetIter->p4());
// 	    if (IsjBtag) {
// 	      jBtag.push_back(1);
// 	    } else {
// 	      jBtag.push_back(0);
// 	    }
	    
// 	    Mjj=(jp4[0]+jp4[1]).M();
// 	  }
// 	} // for Selection Cuts

//       } // for jetIter

      ///PF Jets:
      jcnt_tot=0;
      nBtag=0;
      jBtag.clear();
      j_DRel=100;
      Mjj=-1;

      jp4.clear();
      jNEntries=0;
      JEta.clear();
      JPhi.clear();

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
	     ) {
	  
	  jcnt_tot++;
	  // store parameters for muon selection
	  JEta.push_back(j_eta); 
	  JPhi.push_back(jetIter->phi());
	  jNEntries++;
	  
	  /// b-tagging:
	  if ( (jetIter->bDiscriminator("simpleSecondaryVertexBJetTags"))>bDiscriminator_min ) {
	    IsjBtag=true;
	    nBtag++;
	  } else {
	    IsjBtag=false;
	  }
	  if ( jcnt_tot==1 ) {
	    jp4.push_back(jetIter->p4());
	    if (IsjBtag==true) {
	      jBtag.push_back(1);
	    } else {
	      jBtag.push_back(0);
	    }
	  }
	  /// Compute the invariant mass, when only two jets are present
	  if ( jcnt_tot==2 ) {
	    jp4.push_back(jetIter->p4());
	    if (IsjBtag) {
	      jBtag.push_back(1);
	    } else {
	      jBtag.push_back(0);
	    }
	    
	    Mjj=(jp4[0]+jp4[1]).M();
	  }
	} // for Selection Cuts

      } // for jetIter

      if ( jcnt_tot!=2 ) {
	Mjj=-2;
      }


      //// Muon Selection:
      mucnt_Prim=0;
     mucnt_Loose=0;
     mu1Marker=-1;
     //      mu2Marker=-1;
     lpCounter=0;
     const vector< pat::Muon >::const_iterator muEnd = muonHandle->end();
     for (vector< pat::Muon >::const_iterator muIter = muonHandle->begin(); ( muEnd != muIter ); ++muIter ) { 
       mu_pt=muIter->pt();
       mu_eta=muIter->eta();
       mu_phi=muIter->phi();
       muDRPass=true;
       if ( jNEntries!=0 ) {
	 for (Int_t jn=0; jn<jNEntries;jn++) {
	   adphi=abs(JPhi[jn]-mu_phi);
	   //we're on a circle
	   if ( adphi>pi_ ) {
	     adphi=2*pi_-adphi;
	   }
	   
	   muPrim_DRj=pow((JEta[jn]-mu_eta)*(JEta[jn]-mu_eta)+adphi*adphi,0.5);
	   if ( muPrim_DRj<muPrim_DRjMax ) {
	     //i.e. we must be outside DRjMax for every jet
	     muDRPass=false;
	   }
	 }
	 
       }
       if ( (muIter->isGlobalMuon())&&(muIter->isTrackerMuon()) ) {
	 muisGmTm=true;
       } else {
	 muisGmTm=false;
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
       mu_RelIso=(muIter->trackIso()+muIter->ecalIso()+muIter->hcalIso())/mu_pt;
       
       /// Apply The Primary Cuts
       if ( muisGmTm&&
	    (mu_pt>muPrim_ptMin)&&
	    (abs(mu_eta)<muPrim_aetaMax)&&
	    muisTightPrompt&&
	    (muPrim_itNHits>muPrim_itNHitsMin)&&
	    muDRPass&&
	    ((muIter->dB())<muPrim_dBMax)&&
	    (mu_RelIso<muPrim_RelIsoMax)
	    ) {
	 if ( mu1Marker==-1 ) {
	   mu1Marker=lpCounter;
	 }
	 mucnt_Prim++;
	 //lp4=muIter->p4();
	 lQ=muIter->charge();
	 lEta=mu_eta;
	 
	 lecalIso=(muIter->ecalIso())/mu_pt;
	 lhcalIso=(muIter->hcalIso())/mu_pt;
	 ltrkIso=(muIter->trackIso())/mu_pt;
	 lTotIso=mu_RelIso;
	 
       }
       /// Apply The Loose Cuts
       if ( (muIter->isGlobalMuon())&&(mu_pt>muLoose_ptMin)&&(abs(mu_eta)<muLoose_aetaMax)&&(mu_RelIso<muLoose_RelIsoMax) ) {
	 mucnt_Loose++;	       
       }
       
       lpCounter++; 
     }

     /// Electron ZVeto (apply only if we have one Primary electron).
     el_ZVeto=false;
     lpCounter=0;
     if ( (elcnt_Prim==1)&&(mucnt_Loose==0) ){
       const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
       for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 	
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
	 
	 el_eid=elIter->electronID("simpleEleId95relIso");
	 
	 el_PassEID=0;
	 elZVeto_sigmaIetaIeta=elIter->sigmaIetaIeta();
	 elZVeto_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
	 elZVeto_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
	 elZVeto_HoE=elIter->hadronicOverEm();
	 if ( (elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEB)&&(elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEB)&&(elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEB)&&(elZVeto_HoE<elZVeto_HoEMaxEB)&&(elIter->isEB()) ) {
	   el_PassEID=1;
	 }
	 if ( (elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEE)&&(elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEE)&&(elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEE)&&(elZVeto_HoE<elZVeto_HoEMaxEE)&&(elIter->isEE()) ) {
	   el_PassEID=1;
	 }
	 
	 /// Select an electron which is not the Primary one.
	 if ( (lpCounter!=el1Marker) && 
	      (
	       ((el_eid==elZVeto_eid1)||(el_eid==elZVeto_eid2)||(el_eid==elZVeto_eid3)||(el_eid==elZVeto_eid4))&&
	       //(el_PassEID==1)&&
	       el_aetaPass&&
	       (el_et>elZVeto_etMin)&&(el_RelIso<elZVeto_RelIsoMax) )
	      )
	   {
	     if ( 
		 //((elIter->charge() * lQ)<0.0)&&
		 ((elIter->p4() + lp4).M()>elZVeto_MZMin)&&
		 ((elIter->p4() + lp4).M()<elZVeto_MZMax) ) {
	       el_ZVeto=true;
	     }
	   }

	 lpCounter++;
       }
      //  }
     }
     
     //// MET Selection:
     METp4=METHandle->front().p4();
     MET_Et=METp4.E();
     //MET_Et=METHandle->front().et();

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /////// ****************************************Record The Results**************************************** ////////////////
     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//      //// Record the number of events passing the cuts for each jet count
//      passStandard=0;
//      passAll=0;
//      //cout << "elcnt_Prim=" << elcnt_Prim << endl;
//      for (Int_t Nj=0; Nj<NJETS;Nj++) {
//        if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {
// 	 PassLp[2][Nj]++;
// 	 PassMu[2][Nj]++;
// 	 PassEl[2][Nj]++;

// 	 //record the electron info
// 	 if ( (elcnt_Prim==1)&&(mucnt_Prim==0)&&isPrimaryVertex ) {
// 	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 	   if ( el_conv_a==false ) {
// 	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 	     if ( mucnt_Loose==0 ) {
// 	       incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 	       if ( el_ZVeto==false ) {
// 		 incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		 passStandard=2;
// 		 if ( MET_Et>MET_EtMin ) {
// 		   incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		   passAll=2;
// 		   /// Record the Btag info
// 		   if ( nBtag==0 ) {
// 		     incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		   } else {
// 		     if ( nBtag==1 ) {
// 		       incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		     } else {
// 		       if ( nBtag==2 ) {
// 			 incrementCounter(10,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		       } else {
// 			 incrementCounter(11,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
// 		       }
// 		     }
// 		   }//BTags
// 		 }
// 	       }
// 	     }
// 	   }
// 	 }//electrons
	 
// // 	 //record the muon info
// // 	 if ( (mucnt_Prim==1)&&(elcnt_Prim==0)&&isPrimaryVertex ) {
// // 	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 	   if ( mucnt_Loose==1 ) {
// // 	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 	     if ( elcnt_Loose==0 ) {
// // 	       incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 	       //if ( ZVeto==false ) {
// // 	       incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 	       passStandard=1;
// // 	       if ( MET_Et>MET_EtMin ) {
// // 		 incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 		 passAll=1;
// // 		 /// Record the Btag info
// // 		 if ( nBtag==0 ) {
// // 		   incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 		 } else {
// // 		   if ( nBtag==1 ) {
// // 		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 		   } else {
// // 		     if ( nBtag==2 ) {
// // 		       incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 		     } else {
// // 		       incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
// // 		     }
// // 		   }
// // 		 }//BTags
// // 	       }
// // 	       // }
// // 	     }
// // 	   }
// // 	 }//muons
	 
//        }
//      } // for Njets
// //      if ( (passStandard>0.5) ) {
// //        cout << "passStd=" << passStandard << ", passAll=" << passAll << endl;
// //        cout << "MET_Et=" << MET_Et << endl;
// //        METp4LV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
// //        cout << "METp4LV.E()=" << METp4LV.E() << ", METp4LV.Et()=" << METp4LV.Et() << endl;
// //      }


//      ///Fill the Ntuple to be used in Matrix Element computations (requiring two jets & a lepton passing all of the cuts).
//      // Only fill in the elements which pass the MET cut:
//      passStandard=passAll;


//      if ( (passStandard>0.5)&&(jcnt_tot==2) ) {
//        EvtNtuple->lLV.clear();
//        lp4LV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
//        lp3.SetXYZ(lp4.Px(),lp4.Py(),lp4.Pz());
//        EvtNtuple->lLV.push_back(lp4LV);
//        //EvtNtuple->lLV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());

//        EvtNtuple->jLV.clear();
//        j1p4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
//        EvtNtuple->jLV.push_back(j1p4LV);
//        j2p4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
//        EvtNtuple->jLV.push_back(j2p4LV);
//        jjp3.SetXYZ(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz());

//        EvtNtuple->METLV.clear();
//        METp4LV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
//        EvtNtuple->METLV.push_back(METp4LV);

//        EvtNtuple->lPhi=lp4.Phi();
//        EvtNtuple->METEt=METp4LV.Et();


//        EvtNtuple->jBtag.clear();
//        EvtNtuple->jBtag.push_back(jBtag[0]);
//        EvtNtuple->jBtag.push_back(jBtag[1]);
//        //       cout << "2.EvtNtuple->METLV.E()=" << EvtNtuple->METLV.E() << ", EvtNtuple->METEt=" << EvtNtuple->METEt << endl;
//        EvtNtuple->lQ=lQ;
//        if (abs(lEta)<etaBarrelMax) {
// 	 //barrel
// 	 EvtNtuple->ldetComp=0;
//        } else {
// 	 //endcap
// 	 EvtNtuple->ldetComp=1;
//        }
//        EvtNtuple->run=runNumber;
//        EvtNtuple->event=eventNumber;
       
       
//        //Additional Variables
//        EvtNtuple->lTotIso=lTotIso;
//        EvtNtuple->lecalIso=lecalIso;
//        EvtNtuple->lhcalIso=lhcalIso;
//        EvtNtuple->ltrkIso=ltrkIso;
//        EvtNtuple->Mjj=Mjj;
//        EvtNtuple->passStd=passStandard;
//        EvtNtuple->passAll=passAll;

//        //cout << "3.EvtNtuple->METLV.E()=" << EvtNtuple->METLV.E() << ", EvtNtuple->METEt=" << EvtNtuple->METEt << endl;
//        DRlj1=lp4LV.DeltaR(j1p4LV);
//        DRlj2=lp4LV.DeltaR(j2p4LV);
//        //Thetalj1pj2=(lp4.Px()*(jp4[0].Px()+jp4[1].Px())+lp4.Py()*(jp4[0].Py()+jp4[1].Py())+lp4.Pz()*(jp4[0].Pz()+jp4[1].Pz()))/(    ) ;
//        Thetalj1pj2=lp3.Dot(jjp3)/(lp3.Mag()*jjp3.Mag());

//        EvtNtuple->DRlj1=DRlj1;
//        EvtNtuple->DRlj2=DRlj2;
//        EvtNtuple->Thetalj1pj2=Thetalj1pj2;
//        //Fill The Tree 
//        EvtTree->Fill();

//      }
    

     //// scan over the jets
     passAll=0;
     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {

// 	 if ( lcnt_kin>0.5 ) {
// 	   PassLp[2][Nj]++;
// 	   if ( mucnt_kin>0.5) {
// 	     PassMu[2][Nj]++;
// 	   }
// 	   if ( elcnt_kin>0.5) {
// 	     PassEl[2][Nj]++;
// 	   }
// 	 }
	 
	 if ( (elcnt_Prim==1)&&(isPrimaryVertex) ) {
	   //incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	   if ( mucnt_Loose==0 ) {
	     //incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	     if ( el_ZVeto==false ) {
	       //incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,elcnt_Prim);
	       if ( el_conv_a==false ) {
		 //incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
		 if ( el_conv_b==false ) {
		   //incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
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
		     incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,elcnt_Prim);
		   } else {
		     if ( nBtag==1 ) {
		       incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,elcnt_Prim);
		     } else {
		       if ( nBtag==2 ) {
			 incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_Prim,elcnt_Prim);
		       } else {
			 incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_Prim,elcnt_Prim);
		       }
		     }
		   }//BTags
		 }
	       }
	     }
	   }
	 }
	 
       }
     } // for Njets
//      // record the L+j hypernews
//      if ( (passAll==2)&&(jcnt_tot>3.5) ) {
//        //check if we want elEt, elPt or muPt
//        outResFile << runNumber << ":" << eventNumber << ":" << luminosityBlock  << ":" << setprecision(8) << elEt << endl; 
//      }

 
     EvtTotCount++;

     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       
//        if ( lcnt_kin>0.5 ) {
// 	 PassLp[2][Nj]++;
// 	 if ( mucnt_kin>0.5) {
// 	   PassMu[2][Nj]++;
// 	 }
// 	 if ( elcnt_kin>0.5) {
// 	   PassEl[2][Nj]++;
// 	 }
//        }
       
       if ( (elcnt_Prim==1)&&(isPrimaryVertex) ) {
	 incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	 if ( mucnt_Loose==0 ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	   if ( el_ZVeto==false ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	     if ( el_conv_a==false ) {
	       incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	       if ( el_conv_b==false ) {
		 incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
	       }
	     }
	   }
	 }
       }

     }

     ///TEMPORARY (for syncronization):
     EvtTree->Fill();



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


