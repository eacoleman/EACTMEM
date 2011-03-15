/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Use to skim the PATNtuples and create a custom made .root file containing the needed information.

// CMS includes
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
//#include "PhysicsTools/SelectorUtils/interface/Selector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/UserData.h"


#include "PhysicsTools/FWLite/interface/EventContainer.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 

// #include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
// #include "TrackingTools/TransientTrack/interface/TransientTrack.h"
// #include "TrackingTools/Records/interface/TransientTrackRecord.h"


// #include "FWCore/Framework/interface/MakerMacros.h"
// #include "FWCore/Framework/interface/Frameworkfwd.h"
// #include "FWCore/Framework/interface/EDAnalyzer.h"
// #include "FWCore/Framework/interface/Event.h"
// #include "FWCore/Framework/interface/EventSetup.h"
// #include "FWCore/Framework/interface/ESHandle.h"
// #include "FWCore/ParameterSet/interface/ParameterSet.h"

// #include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
// #include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"


// #include "FWCore/Framework/interface/ESHandle.h"
// #include "DataFormats/TrackReco/interface/Track.h"


//#include "TrackingTools/IPTools/interface/IPTools.h"




//User Defined Ntuple
//#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

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

// ///////////////////// //
// // Custom Functions// //
// ///////////////////// //
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
   // for L+j hypernews only:
//    ofstream outResFile;
//    outResFile.open("TAMU_V2Mu_Selection.txt",ios::out);
   // corresponds to the next to last string on the command line
   TString outrootfilename;
   outrootfilename=argv[argc-3];
   TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
   // adjust argc to account for inputing the three character strings above
   argc=argc-3;


   // create the tree
   TTree *EvtTree = new TTree("EvtTree", "EvtTree"); //Temporary for Synchronization.
//    TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
//    EventNtuple * EvtNtuple = new EventNtuple();
//    EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);

   double pi_=TMath::Pi();

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
  bool mu_vtxPass;

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


//    double muPt;
//    ////Selection Parameters
//    // cuts  
//    Double_t ndof_Min=3.5;
//    Double_t Vz0_Max=15;  //7.5; //7.465 gives 95% efficiency
//    Double_t d0_Max=0.2; //???
//    Double_t rho_Max=2.0;
//    Double_t mu_ptMin=20;
//    Double_t mu_etaMax=2.1;
// //    Double_t mu_etascExcludeMax=1.5660;
// //    Double_t mu_etascExcludeMin=1.4442;
//    Double_t mu_ImpactSigMax=3.0;
//    Double_t mu_RelIsoMax=0.05;
//    Double_t mu_NSiHitsMin=10.5;
//    Double_t muEcalE_Max=4.0;
//    Double_t muHcalE_Max=6.0;
//    Double_t lgTrkChi2red_Max=10;

//    //   Double_t MET_EtMin=30;
//    Double_t smu_ptMin=10.0;
//    Double_t smu_etaMax=2.5;
//    Double_t smu_RelIsoMax=0.2;

//    Double_t sel_EtMin=20;
//    //Double_t sel_etaMax=2.5;
//    Double_t sel_RelIsoMax=0.2;

//    Double_t j_PtMin=30;
//    Double_t j_etaMax=2.4;
//    Double_t j_emFracMin=0.01;
//    Double_t j_n90HitsMin=1.01;
//    Double_t j_fHPDMax=0.98;
//    //   Double_t Rlj_Min=0.3;

//    Double_t MZ_min=76.0;
//    Double_t MZ_max=106.0;
  



   
// //    Double_t l_etaMaxBARREL=1.442;
   
// //    Double_t dB_Max=0.2;
 
   
//    Double_t bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.

//    // vertices
//    //Int_t vndof;

//    // jets
//    Int_t jcnt_tot;
//    Int_t nBtag;
//    Double_t jpt;
//    Double_t jeta;
//    Double_t jphi;
//    Double_t adphi;
//    Double_t Mjj;
//    vector <math::XYZTLorentzVector> jp4;
//    bool IsjBtag;
//    vector <int> jBtag;

//    //leptons
//    Double_t lpt;
//    Double_t lPhi;
//    Double_t lRelIso;
//    Double_t leta;
//    Double_t lphi;
//    Double_t aeta_sc;
//    Double_t lEta;
//    Double_t lvtxChi2;
//    bool muDRPass=true;
//    reco::MuonEnergy muonEnergy;
//    Double_t muEcalE;
//    Double_t muHcalE;
//    Int_t lQ;
//    bool ZVeto=false;
//    bool ConvVeto=false;//Always set to false for now
//    math::XYZTLorentzVector lp4;
//    TLorentzVector jp4LV;
//    Int_t passAll=0; //=1 for mu, =2 for el if the event passes all of the cuts
//    //secondary
//    Double_t smupt;
//    Double_t asmueta;
//    Double_t smuRelIso;
//    Double_t selet;
//    Double_t aseleta;
//    Double_t selRelIso;

//    Int_t vtxcnt=0;
//    Int_t lcnt_kin=0;
//    Int_t lcnt_iso=0;
//    Int_t mucnt_kin=0;
//    Int_t mucnt_iso=0;
//    Int_t elcnt_kin=0;
//    Int_t elcnt_iso=0;
// //    Int_t elcnt_nonmu=0;
//    Int_t mucnt_jetsep=0;
//    Int_t mucnt_loosemu=0;
//    Int_t mucnt_el=0;
//    // other
//    vector < double > JEta, JPhi;
//    Int_t Curr_jEntries=0;
//    Double_t Rlj;
//    Double_t METEt=-1;
//    math::XYZTLorentzVector METp4;
//    Int_t EvtTotCount=0;
//    bool LRPresent=false;
//    Int_t cnt_LR=0;

//    // global counters
//    Int_t PassEl[NCUTS][NJETS];
//    Int_t PassMu[NCUTS][NJETS];
//    Int_t PassLp[NCUTS][NJETS];
//    InitializeIntMatrix(PassEl);
//    InitializeIntMatrix(PassMu);
//    InitializeIntMatrix(PassLp);

  
//    ////Record All Relevant Information
// //    EvtTree->Branch("j1Pt",&jPt[1],"j1Pt/D");
// //    EvtTree->Branch("j2Pt",&jPt[2],"j2Pt/D");
// //    EvtTree->Branch("j1emFrac",&j1emFrac,"j1emFrac/D");
// //    EvtTree->Branch("j1BDiscr",&j1BDiscr,"j1BDiscr/D");
// //    EvtTree->Branch("j1M",&j1M,"j1M/D");
// //    EvtTree->Branch("j1Vz",&j1Vz,"j1Vz/D");
// //    EvtTree->Branch("j1Eta",&j1Eta,"j1Eta/D");
// //    EvtTree->Branch("DRj1",&DRj1,"DRj1/D");
// //    EvtTree->Branch("DRMin",&DRMin,"DRMin/D");

// //    EvtTree->Branch("lEt",&lEt,"lEt/D");
// //    EvtTree->Branch("lEta",&lEta,"lEta/D");
// //    EvtTree->Branch("lPhi",&lPhi,"lPhi/D");

// //    EvtTree->Branch("lId",&lId,"lId/D");
// //    EvtTree->Branch("lM",&lM,"lM/D");
// //    EvtTree->Branch("lvtxChi2",&lvtxChi2,"lvtxChi2/D");
// //    EvtTree->Branch("lrelIso",&lrelIso,"lrelIso/D");
// //    EvtTree->Branch("lDB",&lDB,"lDB/D");
// //    EvtTree->Branch("lVz0",&lVz0,"lVz0/D");

// //    //EvtTree->Branch("isEl",&isEl,"isEl/I");
   
// //    EvtTree->Branch("muEcalE",&muEcalE,"muEcalE/D");
// //    EvtTree->Branch("muHcalE",&muHcalE,"muHcalE/D");
// //    EvtTree->Branch("METEt",&METEt,"METEt/D");
// // //    EvtTree->Branch("Vz0J",&Vz0J,"Vz0J/D");
// // //    EvtTree->Branch("Vz0Mu",&Vz0Mu,"Vz0Mu/D");
// // //    EvtTree->Branch("Vz0El",&Vz0El,"Vz0El/D");

//    // Component Quantities.
//    vector < double > Jv_Eta, Jv_Phi;
//    vector < double > Muv_Pt, Muv_Eta, Muv_relIso, Muv_NHits, Muv_DRmin, Muv_dB;
//    vector < int > Muv_JcntDRmin;
//    vector < bool > Muv_GmTm, Muv_TightPrompt, Muv_DRPass;
//    bool isGmTm, isTightPrompt;
//    int JcntDRmin;
//    double muDRmin, muNHits, mudB;

//    EvtTree->Branch("Jv_Eta",&JEta);
//    EvtTree->Branch("Jv_Phi",&JPhi);

//    EvtTree->Branch("Muv_Pt",&Muv_Pt);
//    EvtTree->Branch("Muv_Eta",&Muv_Eta);
//    EvtTree->Branch("Muv_relIso",&Muv_relIso);
//    EvtTree->Branch("Muv_NHits",&Muv_NHits);
//    EvtTree->Branch("Muv_DRmin",&Muv_DRmin);
//    EvtTree->Branch("Muv_dB",&Muv_dB);
//    EvtTree->Branch("Muv_JcntDRmin",&Muv_JcntDRmin);
//    EvtTree->Branch("Muv_GmTm",&Muv_GmTm);
//    EvtTree->Branch("Muv_TightPrompt",&Muv_TightPrompt);

// //     EvtTree->Branch("Muv_",&Muv_);


// //   EvtTree->Branch("LcntKin",&lcnt_kin,"LcntKin/I");
//    EvtTree->Branch("MucntKin",&mucnt_kin,"MucntKin/I");
//    EvtTree->Branch("MucntIso",&mucnt_iso,"MucntIso/I");
//    EvtTree->Branch("MucntLooseMu",&mucnt_loosemu,"MucntLooseMu/I");
//    EvtTree->Branch("MucntEl",&mucnt_el,"MucntEl/I");
//    EvtTree->Branch("JcntTot",&jcnt_tot,"JcntTot/I");
//    EvtTree->Branch("PassAll",&passAll,"PassAll/I");


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
     //edm::EventID evtid = eventCont.id();

     long runNumber = eventCont.id().run();
     long eventNumber = eventCont.id().event();
     long luminosityBlock = eventCont.id().luminosityBlock();

     //     cout << "run=" << runNumber << "  event=" << eventNumber << endl;

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
      
//       fwlite::Handle< vector< pat::Jet > > pfJetHandle;
//       pfJetHandle.getByLabel (eventCont, "selectedPatJetsAK5PF");
//       assert ( pfJetHandle.isValid() );

      ///Using PF2PAT
      fwlite::Handle< vector< pat::Jet > > pfJetHandle;
      pfJetHandle.getByLabel (eventCont, "selectedPatJetsPFlow");
      assert ( pfJetHandle.isValid() );

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
	//	el_eid=elIter->electronID("simpleEleId70relIso");

// 	el_PassEID=0;
// 	elPrim_sigmaIetaIeta=elIter->sigmaIetaIeta();
// 	elPrim_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
// 	elPrim_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
// 	elPrim_HoE=elIter->hadronicOverEm();
// 	if ( (elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB)&&(elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEB)&&(elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEB)&&(elPrim_HoE<elPrim_HoEMaxEB)&&(elIter->isEB()) ) {
// 	  el_PassEID=1;
// 	}
// 	if ( (elPrim_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE)&&(elPrim_aDeltaPhi<elPrim_aDeltaPhiMaxEE)&&(elPrim_aDeltaEta<elPrim_aDeltaEtaMaxEE)&&(elPrim_HoE<elPrim_HoEMaxEE)&&(elIter->isEE()) ) {
// 	  el_PassEID=1;
// 	}

	el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_et;

	/// Apply The Primary Cuts
	if ( el_vtxPass&&
	     (el_et>elPrim_etMin)&&
	     el_aetaPass&&
	     //	     (abs(elIter->dB())<elPrim_dBMax)&&
	     //	     ((el_eid==elPrim_eid1)||(el_eid==elPrim_eid2)||(el_eid==elPrim_eid3)||(el_eid==elPrim_eid4))&&
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
	///	Apply The Loose Cuts
	if ( (el_et>elLoose_etMin)&&(abs(el_eta)<elLoose_aetaMax)&&(el_RelIso<elLoose_RelIsoMax) ) {
	  elcnt_Loose++;	       
	}
	
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
// // 	if ( elcnt_Prim==1 ){
// // 	  adphi=abs(el_Phi-jetIter->phi());
// // 	  //we're on a circle
// // 	  if ( adphi>pi_ ) {
// // 	    adphi=2*pi_-adphi;
// // 	  }
// // 	  j_DRel=pow((j_eta-el_Eta)*(j_eta-el_Eta)+adphi*adphi,0.5);
// // 	}
	
// 	/// Apply The Cut:
// 	if ( (j_pt>j_ptMin)&&
// 	     (abs(j_eta)<j_aetaMax)&&
// 	     ((jetIter->emEnergyFraction())>jcalo_emFracMin)&&
// 	     ((jetIter->jetID().n90Hits)>jcalo_n90HitsMin)&&
// 	     ((jetIter->jetID().fHPD)<jcalo_fHPDMax)
// 	     //	     (j_DRel>j_DRelMin)
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

// 	if ( elcnt_Prim==1 ){
// 	  adphi=abs(el_Phi-jetIter->phi());
// 	  //we're on a circle
// 	  if ( adphi>pi_ ) {
// 	    adphi=2*pi_-adphi;
// 	  }
// 	  j_DRel=pow((j_eta-el_Eta)*(j_eta-el_Eta)+adphi*adphi,0.5);
// 	}
  
	/// Apply The Cut:
	if ( (j_pt>j_ptMin)&&
	     (abs(j_eta)<j_aetaMax)&&
	     //	     (j_DRel>j_DRelMin)&&
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
       mu_RelIso=(muIter->trackIso()+muIter->ecalIso()+muIter->hcalIso())/mu_pt;

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
       
       mu_vtxPass=false;
       if ( abs(Vtx.z() - muIter->vertex().z())<1 ) {
	 mu_vtxPass=true;
       }

       /// Apply The Primary Cuts
       if ( muisGmTm&&
	    (mu_pt>muPrim_ptMin)&&
	    (abs(mu_eta)<muPrim_aetaMax)&&
	    (mu_RelIso<muPrim_RelIsoMax)&&
	    muisTightPrompt&&
	    (muPrim_itNHits>muPrim_itNHitsMin)&&
	    muDRPass&&
	    ((muIter->dB())<muPrim_dBMax)&&
	    mu_vtxPass&&
	    (muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>0.5)&&
	    (muIter->numberOfMatches()>1)
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
//      lpCounter=0;
//      if ( (elcnt_Prim==1)&&(mucnt_Loose==0) ){
//        const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
//        for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); ( elEnd != elIter ); ++elIter ) { 	
// 	 el_et=elIter->et();
// 	 el_eta=elIter->eta();
// 	 el_aetasc=abs(elIter->superCluster()->eta());
	 
// 	 el_aetaPass=false;
// 	 if ( abs(el_eta)<elZVeto_aetaMax ) {
// 	   el_aetaPass=true; //noscv
// 	 }
// 	 if ( (el_aetasc<elPrim_aetascExcludeMax)&&(el_aetasc>elPrim_aetascExcludeMin) ) {
// 	   el_aetaPass=false;
// 	 }
// 	 el_RelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/el_et;
	 
// 	 el_eid=elIter->electronID("simpleEleId95relIso");
	 
// 	 el_PassEID=0;
// 	 elZVeto_sigmaIetaIeta=elIter->sigmaIetaIeta();
// 	 elZVeto_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
// 	 elZVeto_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
// 	 elZVeto_HoE=elIter->hadronicOverEm();
// 	 if ( (elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEB)&&(elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEB)&&(elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEB)&&(elZVeto_HoE<elZVeto_HoEMaxEB)&&(elIter->isEB()) ) {
// 	   el_PassEID=1;
// 	 }
// 	 if ( (elZVeto_sigmaIetaIeta<elZVeto_sigmaIetaIetaMaxEE)&&(elZVeto_aDeltaPhi<elZVeto_aDeltaPhiMaxEE)&&(elZVeto_aDeltaEta<elZVeto_aDeltaEtaMaxEE)&&(elZVeto_HoE<elZVeto_HoEMaxEE)&&(elIter->isEE()) ) {
// 	   el_PassEID=1;
// 	 }
	 
// 	 /// Select an electron which is not the Primary one.
// 	 if ( (lpCounter!=el1Marker) && 
// 	      (
// 	       ((el_eid==elZVeto_eid1)||(el_eid==elZVeto_eid2)||(el_eid==elZVeto_eid3)||(el_eid==elZVeto_eid4))&&
// 	       //(el_PassEID==1)&&
// 	       el_aetaPass&&
// 	       (el_et>elZVeto_etMin)&&(el_RelIso<elZVeto_RelIsoMax) )
// 	      )
// 	   {
// 	     if ( 
// 		 //((elIter->charge() * lQ)<0.0)&&
// 		 ((elIter->p4() + lp4).M()>elZVeto_MZMin)&&
// 		 ((elIter->p4() + lp4).M()<elZVeto_MZMax) ) {
// 	       el_ZVeto=true;
// 	     }
// 	   }

// 	 lpCounter++;
//        }
//       //  }
//      }
     
     //// MET Selection:
     METp4=METHandle->front().p4();
     MET_Et=METp4.E();
     //MET_Et=METHandle->front().et();

   
     //// scan over the jets
     passAll=0;
     for (Int_t Nj=0; Nj<NJETS;Nj++) {
       if ( (jcnt_tot==Nj)||((Nj==(NJETS-1))&&(jcnt_tot>(NJETS-1))) ) {

	 if ( (mucnt_Prim==1)&&(isPrimaryVertex) ) {
	   //incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   if ( mucnt_Loose==1 ) {
	     //incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	     if ( elcnt_Loose==0 ) {
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
     } // for Njets
//      // record the L+j hypernews
//      if ( (passAll==1)&&(jcnt_tot>3.5) ) {
//        //check if we want elEt, elPt or muPt
//        outResFile << runNumber << ":" << eventNumber << ":" << luminosityBlock  << ":" << setprecision(8) << muPt << endl; 
//      }


//      ///fill the Ntuple to be used in Matrix Element computations.

//      if ( (passAll>0.5)&&(jcnt_tot==2) ) {
//        EvtNtuple->lLV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
//        EvtNtuple->jLV.clear();
//        jp4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
//        EvtNtuple->jLV.push_back(jp4LV);
//        jp4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
//        EvtNtuple->jLV.push_back(jp4LV);
//        EvtNtuple->METLV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
       
//        EvtNtuple->jBtag.clear();
//        EvtNtuple->jBtag.push_back(jBtag[0]);
//        EvtNtuple->jBtag.push_back(jBtag[1]);

//        EvtNtuple->lQ=lQ;
//        if (abs(lEta)<l_etaMaxBARREL) {
// 	 //barrel
// 	 EvtNtuple->ldetComp=0;
//        } else {
// 	 //endcap (should never happen with the current selection):
// 	 EvtNtuple->ldetComp=1;
//        }
//        EvtNtuple->run=runNumber;
//        EvtNtuple->event=eventNumber;
       
       
//        //Additional Variables
//        EvtNtuple->Mjj=Mjj;
//        EvtNtuple->passAll=passAll;

//        //Fill The Tree  
//        EvtTree->Fill();
//      }
     
     EvtTotCount++;

     for (Int_t Nj=0; Nj<NJETS;Nj++) {
      
       if ( (mucnt_Prim==1)&&(isPrimaryVertex) ) {
	 incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	 if ( mucnt_Loose==1 ) {
	   incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   if ( elcnt_Loose==0 ) {
	     incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
	   }
	 }
       }

     }

     ///TEMPORARY:
     EvtTree->Fill();

   } // for eventCont

   for (Int_t Nj=0; Nj<NJETS;Nj++) {
     PassLp[0][Nj]=EvtTotCount;
     PassMu[0][Nj]=EvtTotCount;
     PassEl[0][Nj]=EvtTotCount;
     PassLp[1][Nj]=vtxcnt;
     PassMu[1][Nj]=vtxcnt;
     PassEl[1][Nj]=vtxcnt;

//      if ( lcnt_kin>0.5 ) {
//        PassLp[2][Nj]++;
//        if ( mucnt_kin>0.5) {
// 	 PassMu[2][Nj]++;
//        }
//        if ( elcnt_kin>0.5) {
// 	 PassEl[2][Nj]++;
//        }
//      }

//      if ( lcnt_iso==1 ) {
//        incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
//        if ( elcnt_nonmu==0 ) {
// 	 incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
//        }
//      }
     

   }

   InitializeLabels(PLabel,CLabel);
   writeIntProcessTable(outtablefile,PassEl,PassMu,PassLp);


   outrootfile->Write();
   outrootfile->Close();


   ////////////////////////
   // ////////////////// //
   // // Clean Up Job // //
   // ////////////////// //
   ////////////////////////

   return 0;

}

