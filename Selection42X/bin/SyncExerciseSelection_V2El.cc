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
#include <TAMUWW/Tools/GlobalTools.cc>

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
   TString outtablefilename;
   outtablefilename=argv[argc-1];
   ofstream outtablefile;
   outtablefile.open(outtablefilename,ios::out);
   // for L+j hypernews only:
   ofstream outResFile;
   outResFile.open("TAMU_V2El_Selection.txt",ios::out);
   // corresponds to the next to last string on the command line
   TString outrootfilename;
   outrootfilename=argv[argc-2];
   TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
   TTree *EvtTree = new TTree("EvtTree", "EvtTree"); //Temporary for Synchronization.
   // adjust argc to account for inputing the two character strings above
   argc=argc-2;

//    // create the tree
//    TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
//    EventNtuple * EvtNtuple = new EventNtuple();
//    EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
   double pi_=TMath::Pi();


   ////Selection Parameters
   // cuts  
   Double_t ndof_Min=3.5;
   Double_t Vz0_Max=15;  //7.5; //7.465 gives 95% efficiency
   Double_t rho_Max=2.0;
   Double_t el_EtMin=30.0;
   Double_t el_etaMax=2.5;
   Double_t el_etascExcludeMax=1.5660;
   Double_t el_etascExcludeMin=1.4442;
   Double_t el_ImpactSigMax=3.0;
   Double_t el_RelIsoMax=0.1;
   Double_t d0_Max=0.02;
   int eid;

   //   Double_t MET_EtMin=30;
   Double_t smu_ptMin=10.0;
   Double_t smu_etaMax=2.5;
   Double_t smu_RelIsoMax=0.2;
   Double_t sel_EtMin=20;
   Double_t sel_etaMax=2.5;
   Double_t sel_RelIsoMax=1.0;

   Double_t j_PtMin=30;
   Double_t j_etaMax=2.4;
   Double_t j_emFracMin=0.01;
   Double_t j_n90HitsMin=1.01;
   Double_t j_fHPDMax=0.98;
   Double_t Rlj_Min=0.3;

   Double_t MZ_min=76.0;
   Double_t MZ_max=106.0;
  
//    Double_t muEcalE_Max=4.0;
//    Double_t muHcalE_Max=6.0;
//    Double_t lvtxChi2_Max=10.0;

   
//    Double_t l_etaMaxBARREL=1.442;
   
//    Double_t dB_Max=0.2;
 
   
   Double_t bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.

   // vertices
   bool isPV;
   //Int_t vndof;

   // jets
   Int_t jcnt_tot;
   Int_t nBtag;
   Double_t jpt;
   Double_t jeta;
   Double_t adphi;
   Double_t Mjj;
   vector <math::XYZTLorentzVector> jp4;
   bool IsjBtag;
   vector <int> jBtag;

   //leptons
   Double_t let;
   Double_t elEt;
   Double_t lPhi;
   Double_t lRelIso;
   Double_t leta;
   Double_t aeta_sc;
   Double_t lEta;
   Double_t lvtxChi2;
   Double_t muEcalE;
   Double_t muHcalE;
   Int_t lQ;
   bool aetaPass=false;
   bool ZVeto=false;
   bool el_conv=false;
   math::XYZTLorentzVector lp4;
   TLorentzVector jp4LV;
   Int_t passAll=0; //=1 for mu, =2 for el if the event passes all of the cuts
   //secondary
   Double_t smupt;
   Double_t asmueta;
   Double_t smuRelIso;
   Double_t selRelIso;

   Int_t vtxcnt=0;
   Int_t lcnt_kin=0;
   Int_t lcnt_iso=0;
   Int_t mucnt_kin=0;
   Int_t mucnt_iso=0;
   Int_t elcnt_kin=0;
   Int_t elcnt_iso=0;
   Int_t elcnt_nonmu=0;

   // other
   Double_t Rlj;
   Double_t METEt=-1;
   math::XYZTLorentzVector METp4;
   Int_t EvtTotCount=0;
   bool LRPresent=false;
   Int_t cnt_LR=0;

   // global counters
   Int_t PassEl[NCUTS][NJETS];
   Int_t PassMu[NCUTS][NJETS];
   Int_t PassLp[NCUTS][NJETS];
   InitializeIntMatrix(PassEl);
   InitializeIntMatrix(PassMu);
   InitializeIntMatrix(PassLp);

  

   // Parse the command line arguments
   parser.parseArguments (argc, argv);

 //   ///TEMPORARY
//    float d0;
//    float d0err;
//    float d0sig;
//    float d0T;
//    float d0Terr;
//    float d0Tsig;

//    EvtTree->Branch("d0",&d0,"d0/F");
//    EvtTree->Branch("d0err",&d0err,"d0err/F");
//    EvtTree->Branch("d0sig",&d0sig,"d0sig/F");
//    EvtTree->Branch("d0T",&d0T,"d0T/F");
//    EvtTree->Branch("d0Terr",&d0Terr,"d0Terr/F");
//    EvtTree->Branch("d0Tsig",&d0Tsig,"d0Tsig/F");


   // Component Quantities.
//    vector < double > Elv_aetasc, Elv_eta, Elv_Et, Elv_RelIso;
//    vector <int> Elv_eid;

//    EvtTree->Branch("Elv_aetasc",&Elv_aetasc);
//    EvtTree->Branch("Elv_eta",&Elv_eta);
//    EvtTree->Branch("Elv_Et",&Elv_Et);
//    EvtTree->Branch("Elv_eid",&Elv_eid);
//    EvtTree->Branch("Elv_RelIso",&Elv_RelIso);


   double lEt;
   double laEtasc;
   double ld0sig;
   double leid;

   EvtTree->Branch("d0",&ld0sig,"d0/D");
   EvtTree->Branch("eid",&leid,"eid/D");
   EvtTree->Branch("aetasc",&laEtasc,"aetasc/D");
   EvtTree->Branch("eleta",&lEta,"eleta/D");
   EvtTree->Branch("elEt",&lEt,"elEt/D");

   EvtTree->Branch("elcnt_iso",&elcnt_iso,"elcnt_iso/I");
   EvtTree->Branch("elRelIso",&lRelIso,"elRelIso/D");
   EvtTree->Branch("isPV",&isPV,"isPV/bool");





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
//      fwlite::Handle< vector< pat::Jet > > jetHandle; //? define the vector
//      jetHandle.getByLabel (eventCont, "selectedLayer1Jets"); //? load all of the (layer 1???) jets for the event into jetHandle
//      assert ( jetHandle.isValid() );//?? ensure that there's at least one jet
	
//      fwlite::Handle< vector< pat::Muon > > muonHandle;
//      muonHandle.getByLabel (eventCont, "selectedLayer1Muons");
//      assert ( muonHandle.isValid() );

//      ////for patElectrons_selectedLayer1Electrons_PAT.
//      fwlite::Handle< vector< pat::Electron > > electronHandle;
//      electronHandle.getByLabel (eventCont, "selectedLayer1Electrons");
//      assert ( electronHandle.isValid() );

//      fwlite::Handle< vector< pat::MET > > METHandle;
//      METHandle.getByLabel (eventCont, "layer1METs");
//      assert ( METHandle.isValid() );

// //     for patElectrons_cleanPatElectrons_PAT.
//      fwlite::Handle< vector< pat::Electron > > electronHandle;
//      electronHandle.getByLabel (eventCont, "cleanPatElectrons");
//      assert ( electronHandle.isValid() );


//     for patElectrons_cleanPatElectrons_PAT.
     fwlite::Handle< vector< pat::Electron > > electronHandle;
     electronHandle.getByLabel (eventCont, "cleanPatElectrons");
     assert ( electronHandle.isValid() );

//      fwlite::Handle< vector< pat::Electron > > electronHandle;
//      electronHandle.getByLabel (eventCont, "ModifiedCleanElectrons");
//      assert ( electronHandle.isValid() );

     fwlite::Handle< vector< pat::Jet > > jetHandle; //? define the vector
     jetHandle.getByLabel (eventCont, "cleanPatJets"); //? load all of the (layer 1???) jets for the event into jetHandle
     assert ( jetHandle.isValid() );//?? ensure that there's at least one jet
	
     fwlite::Handle< vector< pat::Muon > > muonHandle;
     muonHandle.getByLabel (eventCont, "cleanPatMuons");
     assert ( muonHandle.isValid() );
//      fwlite::Handle< vector< pat::Muon > > muonHandle;
//      muonHandle.getByLabel (eventCont, "ModifiedCleanMuons");
//      assert ( muonHandle.isValid() );

     fwlite::Handle< vector< pat::MET > > METHandle;
     METHandle.getByLabel (eventCont, "patMETs");
     assert ( METHandle.isValid() );

     // fwlite::Handle< reco::Vertex< pat::VertexAssociation > > VtxHandle;
     fwlite::Handle< vector< reco::Vertex > > VtxHandle;
     VtxHandle.getByLabel (eventCont, "offlinePrimaryVertices");
     assert ( VtxHandle.isValid() );



     ////// Perform The Selection Cuts
     //LRPresent=false;
     //// VERTICES:
     isPV=false;
     reco::Vertex Vtx=VtxHandle->front();
     if ( (!Vtx.isFake())&&(Vtx.ndof()>4)&&(abs(Vtx.z())<15)&&(Vtx.position().Rho()<2.0) ) {
       vtxcnt++;
       isPV=true;
     }


     //// LEPTONS:
     lcnt_kin=0;
     lcnt_iso=0;

     ///Electrons:
     aetaPass=false;
     elcnt_kin=0;
     elcnt_iso=0;
     el_conv=false;
     vector< pat::Electron > const & electronVec = *electronHandle;
     if (electronVec.begin() != electronVec.end()) {
       const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
       for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {

	 eid=elIter->electronID("simpleEleId70relIso");
// 	 eid = (int) elIter->electronID("simpleEleId70cIso");
// 	 bool hadId(eid & 0x1);


	 let=elIter->et();
	 leta=elIter->eta();
	 aeta_sc=abs(elIter->superCluster()->eta());
	 if ( abs(leta)<2.5 ) {
	   aetaPass=true; //noscv
	 }
	 if ( (aeta_sc<el_etascExcludeMax)&&(aeta_sc>el_etascExcludeMin) ) {
	   //aetaPass=true; //scv1
	   aetaPass=false; //scv2
	 }

	 /// Kinematics Cut
	 if ( (let>30.0)&&
	      //((abs(leta))<2.5)&&
	      aetaPass&&
	      //((aeta_sc>el_etascExcludeMax)||(aeta_sc<el_etascExcludeMin))&&
	      //(d0Tsig<3.0)&&
	      (abs(elIter->dB())<0.02)&&
	      ((eid==1)||(eid==3)||(eid==5)||(eid==7)) 
	      ) {
	   //&&(abs(elIter->dB())<d0_Max)&&((elIter->vertexChi2())<lvtxChi2_Max)
	   lcnt_kin++;
	   elcnt_kin++;
	   ///Isolation Cut
	   lRelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/let;
	   if ( lRelIso<0.1 ) {
	     lEta=leta;
	     lPhi=elIter->phi();
	       
	     lEt=let;
	     laEtasc=aeta_sc;
	     // ld0sig=d0sig;
	     //	     leid=elIter->electronID("eidRobustTight");


	     lcnt_iso++;
	     elcnt_iso++;
	     lp4=elIter->p4();
	     lQ=elIter->charge();
	     elEt=let;
	     ///Electron Conversion
	     if ( elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()!=0 ) {
	       el_conv=true;
	     }	 
	   }
	 }
	 
       }
     }
     


     /// LOOSE Muons:
     elcnt_nonmu=0;

//      mucnt_kin=0;
//      mucnt_iso=0;
     vector< pat::Muon > const & muonVec = *muonHandle;
     if (muonVec.begin() != muonVec.end()) {
       const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
       for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {

	 smupt=muIter->pt();
	 asmueta=abs(muIter->eta());

	 smuRelIso=(muIter->isolationR03().sumPt+muIter->isolationR03().emEt+muIter->isolationR03().hadEt)/smupt;
	 ///Secondary Muon Cut
	 if ( ((muIter->isGood("AllGlobalMuons"))>0)&&(smupt>smu_ptMin)&&(asmueta<smu_etaMax)&&(smuRelIso<smu_RelIsoMax) ) {
	     elcnt_nonmu++;	       
	 }
	 
	 //	 }
	 
       }
     }


     //// JETS:
     Rlj=-1;
     Mjj=-1;
     nBtag=0;
     jp4.clear();
     jBtag.clear();
     jcnt_tot=0;


     const vector< pat::Jet >::const_iterator kJetEnd = jetHandle->end();
     
     for (vector< pat::Jet >::const_iterator jetIter = jetHandle->begin();
	  ( kJetEnd != jetIter ); 
	  ++jetIter ) 
       { 
   	 jpt=jetIter->pt();
	 jeta=jetIter->eta();
	 /// Luminous Region Cut
// 	 if ( abs(jetIter->vz())<Vz0_Max ) {
// 	   if ( LRPresent==false ) {
// 	     cnt_LR++;
// 	     LRPresent=true;
// 	   }


	 if ( lcnt_iso==1 ){
	   adphi=abs(lPhi-jetIter->phi());
	   //we're on a circle
	   if ( adphi>pi_ ) {
	     adphi=2*pi_-adphi;
	   }
	   Rlj=pow((jeta-lEta)*(jeta-lEta)+adphi*adphi,0.5);
	 }
	 /// Jet Kinematics And Cleaning Cut:
	 if ( (jpt>j_PtMin)&&(abs(jeta)<j_etaMax)&&((jetIter->emEnergyFraction())>j_emFracMin)&&((jetIter->jetID().n90Hits)>j_n90HitsMin)&&((jetIter->jetID().fHPD)<j_fHPDMax)
	      &&((lcnt_iso!=1)||(Rlj>Rlj_Min)) ) {
	   //if ( (jpt>j_PtMin)&&(jeta<j_etaMax)&&((jetIter->emEnergyFraction())>jemFrac_Min)&&((lcnt_iso!=1)||(Rlj>Rlj_Min)) )
	   jcnt_tot++;
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
	   if ( jcnt_tot==2 ) {
	     jp4.push_back(jetIter->p4());
	     if (IsjBtag==true) {
	       jBtag.push_back(1);
	     } else {
	       jBtag.push_back(0);
	     }
	     Mjj=(jp4[0]+jp4[1]).M();
	   }
	   }
	   //}
       } // for jetIter
     if ( jcnt_tot!=2 ) {
       Mjj=-2;
     }

//      //// METs:
//      METp4=METHandle->front().p4();
//      METEt=METp4.E();
     
//      ///Perform the Z veto
//      ZVeto=false;
//      //Z veto the muons:
//      if ( (mucnt_iso==1)&&(lcnt_iso==1) ) {
//        vector< pat::Muon > const & muonVec = *muonHandle;
//        if (muonVec.begin() != muonVec.end()) {
// 	 const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
// 	 for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {
// 	   let=muIter->et();
// 	   reco::MuonEnergy muonEnergy=muIter->calEnergy();
// 	   muEcalE=muonEnergy.em;
// 	   muHcalE=muonEnergy.had;
// 	   //lvtxChi2=muIter->vertexChi2();
// 	   lrelIso=(muIter->caloIso()+muIter->trackIso())/let;

// 	   if ( !((let>l_EtMin)&&(abs(muIter->dB())<dB_Max)&&(muEcalE<muEcalE_Max)&&(muHcalE<muHcalE_Max)&&((muIter->vertexChi2())<lvtxChi2_Max)&&(lrelIso<l_relIsoMax))
// 		&&((let>sl_EtMin)&&(abs(muIter->eta())<l_etaMax)&&((muIter->isGood("All"))>0.99)&&(abs(muIter->vz())<Vz0_Max))    ) {
// 	     if ( ((muIter->charge() * lQ)<0.0)&&
// 		  ((muIter->p4() + lp4).M()>MZ_min)&&
// 		  ((muIter->p4() + lp4).M()<MZ_max) ) {
// 	       ZVeto=true;
// 	     }
// 	   }
// 	 }
//        }
//      }


     ///Z veto the electrons:
     ZVeto=false;
     aetaPass=false;
     if ( (lcnt_iso==1) ){
       vector< pat::Electron > const & electronVec = *electronHandle;
       if (electronVec.begin() != electronVec.end()) {
	 const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
	 for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {


	   eid=elIter->electronID("simpleEleId70relIso");
	   // 	 eid = (int) elIter->electronID("simpleEleId70cIso");
	   // 	 bool hadId(eid & 0x1);
	   
	   
	   let=elIter->et();
	   leta=elIter->eta();
	   aeta_sc=abs(elIter->superCluster()->eta());
	   if ( abs(leta)<2.5 ) {
	     aetaPass=true; //noscv
	   }
	   if ( (aeta_sc<el_etascExcludeMax)&&(aeta_sc>el_etascExcludeMin) ) {
	     //aetaPass=true; //scv1
	     aetaPass=false; //scv2
	   }
	   
 	   selRelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/let;
	   if ( !((let>30.0)&&
		  aetaPass&&
		  (abs(elIter->dB())<0.02)&&
		  ((eid==1)||(eid==3)||(eid==5)||(eid==7))&& 
		  (selRelIso<0.1)
		  ) && 
		( (elIter->electronID("eidRobustLoose")>0.99)&&(let>sel_EtMin)&&(abs(leta)<sel_etaMax)&&(selRelIso<sel_RelIsoMax) )
		//(elIter->electronID("eidRobustLoose")>0.99)&&
		)
	     {
	       if ( 
		   //((elIter->charge() * lQ)<0.0)&&
		    ((elIter->p4() + lp4).M()>MZ_min)&&
		    ((elIter->p4() + lp4).M()<MZ_max) ) {
		 ZVeto=true;
	       }
	     }
	 }
       }
     }
   
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
	 

	 if ( (lcnt_iso==1)&&isPV ) {
	   //incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	   if ( el_conv==false ) {
	     //incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	     if ( elcnt_nonmu==0 ) {
	       //incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	       for (Int_t nj=0; nj<(Nj+1);nj++) {
		 PassLp[6][nj]++;
		 //PassMu[6][nj]++;
		 PassEl[6][nj]++;
	       }
	       
	       if ( ZVeto==false ) {
		 incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		 if ( mucnt_iso==1 ) {
		   passAll=1;
		 }
		 if ( elcnt_iso==1 ) {
		   passAll=2;
		 }
		 if ( nBtag==0 ) {
		   incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		 } else {
		   if ( nBtag==1 ) {
		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		   } else {
		     if ( nBtag==2 ) {
		       incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		     } else {
		       incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		     }
		   }
		 }//BTags
	       }
	     }
	   }
	 }
	 
       }
     } // for Njets
     // record the L+j hypernews
     if ( (passAll==2)&&(jcnt_tot>3.5) ) {
       //check if we want elEt, elPt or muPt
       outResFile << runNumber << ":" << eventNumber << ":" << luminosityBlock  << ":" << setprecision(8) << elEt << endl; 
     }




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
       
       if ( lcnt_kin>0.5 ) {
	 PassLp[2][Nj]++;
	 if ( mucnt_kin>0.5) {
	   PassMu[2][Nj]++;
	 }
	 if ( elcnt_kin>0.5) {
	   PassEl[2][Nj]++;
	 }
       }
       
       if ( (elcnt_iso==1)&&(isPV) ) {
	 incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	 if ( el_conv==false ) {
	   incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	   if ( elcnt_nonmu==0 ) {
	     incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
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

