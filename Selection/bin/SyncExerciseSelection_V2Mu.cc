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
   outResFile.open("TAMU_V2Mu_Selection.txt",ios::out);
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
   double muPt;
   ////Selection Parameters
   // cuts  
   Double_t ndof_Min=3.5;
   Double_t Vz0_Max=15;  //7.5; //7.465 gives 95% efficiency
   Double_t d0_Max=0.2; //???
   Double_t rho_Max=2.0;
   Double_t mu_ptMin=20;
   Double_t mu_etaMax=2.1;
//    Double_t mu_etascExcludeMax=1.5660;
//    Double_t mu_etascExcludeMin=1.4442;
   Double_t mu_ImpactSigMax=3.0;
   Double_t mu_RelIsoMax=0.05;
   Double_t mu_NSiHitsMin=10.5;
   Double_t muEcalE_Max=4.0;
   Double_t muHcalE_Max=6.0;
   Double_t lgTrkChi2red_Max=10;

   //   Double_t MET_EtMin=30;
   Double_t smu_ptMin=10.0;
   Double_t smu_etaMax=2.5;
   Double_t smu_RelIsoMax=0.2;

   Double_t sel_EtMin=20;
   //Double_t sel_etaMax=2.5;
   Double_t sel_RelIsoMax=0.2;

   Double_t j_PtMin=30;
   Double_t j_etaMax=2.4;
   Double_t j_emFracMin=0.01;
   Double_t j_n90HitsMin=1.01;
   Double_t j_fHPDMax=0.98;
   //   Double_t Rlj_Min=0.3;

   Double_t MZ_min=76.0;
   Double_t MZ_max=106.0;
  



   
//    Double_t l_etaMaxBARREL=1.442;
   
//    Double_t dB_Max=0.2;
 
   
   Double_t bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.

   // vertices
   //Int_t vndof;

   // jets
   Int_t jcnt_tot;
   Int_t nBtag;
   Double_t jpt;
   Double_t jeta;
   Double_t jphi;
   Double_t adphi;
   Double_t Mjj;
   vector <math::XYZTLorentzVector> jp4;
   bool IsjBtag;
   vector <int> jBtag;

   //leptons
   Double_t lpt;
   Double_t lPhi;
   Double_t lRelIso;
   Double_t leta;
   Double_t lphi;
   Double_t aeta_sc;
   Double_t lEta;
   Double_t lvtxChi2;
   bool muDRPass=true;
   reco::MuonEnergy muonEnergy;
   Double_t muEcalE;
   Double_t muHcalE;
   Int_t lQ;
   bool ZVeto=false;
   bool ConvVeto=false;//Always set to false for now
   math::XYZTLorentzVector lp4;
   TLorentzVector jp4LV;
   Int_t passAll=0; //=1 for mu, =2 for el if the event passes all of the cuts
   //secondary
   Double_t smupt;
   Double_t asmueta;
   Double_t smuRelIso;
   Double_t selet;
   Double_t aseleta;
   Double_t selRelIso;

   Int_t vtxcnt=0;
   Int_t lcnt_kin=0;
   Int_t lcnt_iso=0;
   Int_t mucnt_kin=0;
   Int_t mucnt_iso=0;
   Int_t elcnt_kin=0;
   Int_t elcnt_iso=0;
//    Int_t elcnt_nonmu=0;
   Int_t mucnt_jetsep=0;
   Int_t mucnt_loosemu=0;
   Int_t mucnt_el=0;
   // other
   vector < double > JEta, JPhi;
   Int_t Curr_jEntries=0;
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

  
   ////Record All Relevant Information
//    EvtTree->Branch("j1Pt",&jPt[1],"j1Pt/D");
//    EvtTree->Branch("j2Pt",&jPt[2],"j2Pt/D");
//    EvtTree->Branch("j1emFrac",&j1emFrac,"j1emFrac/D");
//    EvtTree->Branch("j1BDiscr",&j1BDiscr,"j1BDiscr/D");
//    EvtTree->Branch("j1M",&j1M,"j1M/D");
//    EvtTree->Branch("j1Vz",&j1Vz,"j1Vz/D");
//    EvtTree->Branch("j1Eta",&j1Eta,"j1Eta/D");
//    EvtTree->Branch("DRj1",&DRj1,"DRj1/D");
//    EvtTree->Branch("DRMin",&DRMin,"DRMin/D");

//    EvtTree->Branch("lEt",&lEt,"lEt/D");
//    EvtTree->Branch("lEta",&lEta,"lEta/D");
//    EvtTree->Branch("lPhi",&lPhi,"lPhi/D");

//    EvtTree->Branch("lId",&lId,"lId/D");
//    EvtTree->Branch("lM",&lM,"lM/D");
//    EvtTree->Branch("lvtxChi2",&lvtxChi2,"lvtxChi2/D");
//    EvtTree->Branch("lrelIso",&lrelIso,"lrelIso/D");
//    EvtTree->Branch("lDB",&lDB,"lDB/D");
//    EvtTree->Branch("lVz0",&lVz0,"lVz0/D");

//    //EvtTree->Branch("isEl",&isEl,"isEl/I");
   
//    EvtTree->Branch("muEcalE",&muEcalE,"muEcalE/D");
//    EvtTree->Branch("muHcalE",&muHcalE,"muHcalE/D");
//    EvtTree->Branch("METEt",&METEt,"METEt/D");
// //    EvtTree->Branch("Vz0J",&Vz0J,"Vz0J/D");
// //    EvtTree->Branch("Vz0Mu",&Vz0Mu,"Vz0Mu/D");
// //    EvtTree->Branch("Vz0El",&Vz0El,"Vz0El/D");

   // Component Quantities.
   vector < double > Jv_Eta, Jv_Phi;
   vector < double > Muv_Pt, Muv_Eta, Muv_relIso, Muv_NHits, Muv_DRmin, Muv_dB;
   vector < int > Muv_JcntDRmin;
   vector < bool > Muv_GmTm, Muv_TightPrompt, Muv_DRPass;
   bool isGmTm, isTightPrompt;
   int JcntDRmin;
   double muDRmin, muNHits, mudB;

   EvtTree->Branch("Jv_Eta",&JEta);
   EvtTree->Branch("Jv_Phi",&JPhi);

   EvtTree->Branch("Muv_Pt",&Muv_Pt);
   EvtTree->Branch("Muv_Eta",&Muv_Eta);
   EvtTree->Branch("Muv_relIso",&Muv_relIso);
   EvtTree->Branch("Muv_NHits",&Muv_NHits);
   EvtTree->Branch("Muv_DRmin",&Muv_DRmin);
   EvtTree->Branch("Muv_dB",&Muv_dB);
   EvtTree->Branch("Muv_JcntDRmin",&Muv_JcntDRmin);
   EvtTree->Branch("Muv_GmTm",&Muv_GmTm);
   EvtTree->Branch("Muv_TightPrompt",&Muv_TightPrompt);

//     EvtTree->Branch("Muv_",&Muv_);


//   EvtTree->Branch("LcntKin",&lcnt_kin,"LcntKin/I");
   EvtTree->Branch("MucntKin",&mucnt_kin,"MucntKin/I");
   EvtTree->Branch("MucntIso",&mucnt_iso,"MucntIso/I");
   EvtTree->Branch("MucntLooseMu",&mucnt_loosemu,"MucntLooseMu/I");
   EvtTree->Branch("MucntEl",&mucnt_el,"MucntEl/I");
   EvtTree->Branch("JcntTot",&jcnt_tot,"JcntTot/I");
   EvtTree->Branch("PassAll",&passAll,"PassAll/I");


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


     fwlite::Handle< vector< pat::Electron > > electronHandle;
     electronHandle.getByLabel (eventCont, "cleanPatElectrons");
     assert ( electronHandle.isValid() );

     fwlite::Handle< vector< pat::Jet > > jetHandle; //? define the vector
     jetHandle.getByLabel (eventCont, "cleanPatJets"); //? load all of the (layer 1???) jets for the event into jetHandle
     assert ( jetHandle.isValid() );//?? ensure that there's at least one jet
	
     fwlite::Handle< vector< pat::Muon > > muonHandle;
     muonHandle.getByLabel (eventCont, "cleanPatMuons");
     assert ( muonHandle.isValid() );

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
     reco::Vertex Vtx=VtxHandle->front();
     if ( (!Vtx.isFake())&&(Vtx.ndof()>4)&&(abs(Vtx.z())<15)&&(Vtx.position().Rho()<2.0) ) {
       vtxcnt++;
     }

     //// JETS:
     Mjj=-1;
     nBtag=0;
     jp4.clear();
     jBtag.clear();
     jcnt_tot=0;

     Curr_jEntries=0;
     JEta.clear();
     JPhi.clear();

     const vector< pat::Jet >::const_iterator kJetEnd = jetHandle->end();
     
     for (vector< pat::Jet >::const_iterator jetIter = jetHandle->begin();
	  ( kJetEnd != jetIter ); 
	  ++jetIter ) 
       { 
   	 jpt=jetIter->pt();
	 jeta=jetIter->eta();

	 /// Jet Kinematics And Cleaning Cut:
	 if ( (jpt>30.0)&&
	      (abs(jeta)<2.4)&&
	      ((jetIter->emEnergyFraction())>0.01)&&
	      //((jetIter->jetID().restrictedEMF)>0.01)&&
	      ((jetIter->jetID().n90Hits)>1)&&
	      ((jetIter->jetID().fHPD)<0.98) 
	      ) {
	   //if ( (jpt>j_PtMin)&&(jeta<j_etaMax)&&((jetIter->emEnergyFraction())>jemFrac_Min)&&((lcnt_iso!=1)||(Rlj>Rlj_Min)) )
	   jcnt_tot++;

	   jphi=jetIter->phi();
	   JEta.push_back(jeta); 
	   JPhi.push_back(jphi);
	   Curr_jEntries++;

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



     //// LEPTONS:
     lcnt_kin=0;
     lcnt_iso=0;

     /// Muons:
     muDRmin=100;
     JcntDRmin=-1;
     Muv_Pt.clear();
     Muv_Eta.clear();
     Muv_relIso.clear();
     Muv_NHits.clear();
     Muv_DRmin.clear();
     Muv_dB.clear();
     Muv_GmTm.clear();
     Muv_TightPrompt.clear();
     Muv_DRPass.clear();
     Muv_JcntDRmin.clear();


  
     //muDRPass=false;
     mucnt_kin=0;
     mucnt_iso=0;

     vector< pat::Muon > const & muonVec = *muonHandle;
     if (muonVec.begin() != muonVec.end()) {
       const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
       for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {
	 lpt=muIter->pt();
	 leta=muIter->eta();
	 lphi=muIter->phi();
	 muDRPass=true;
	 if ( Curr_jEntries!=0 ) {
	   for (Int_t jn=0; jn<Curr_jEntries;jn++) {
	     adphi=abs(JPhi[jn]-lphi);
	     //we're on a circle
	     if ( adphi>pi_ ) {
	       adphi=2*pi_-adphi;
	     }
	     
	     Rlj=pow((JEta[jn]-leta)*(JEta[jn]-leta)+adphi*adphi,0.5);
	     if ( Rlj<muDRmin ) {
	       muDRmin=Rlj;
	       JcntDRmin=jn;
	     }
	     if ( Rlj<0.3 ) {
	       //i.e. we must be outside 0.3 for every jet
	       muDRPass=false;
	     }
	       // 	     if ( Rlj>0.3 ) {
	       // 	       muDRPass=true;
	       // 	     }
	   }
	   
	 }
	 
	 ///Kinematics Cut
	 if ( (muIter->isGlobalMuon())&&(muIter->isTrackerMuon()) ) {
	   isGmTm=true;
	 } else {
	   isGmTm=false;
	 }

	 isTightPrompt=false;
	 if ( muIter->globalTrack().isNonnull() ) {
	   isTightPrompt=false;
	   if ( ((muIter->globalTrack()->normalizedChi2())<10.0)&&((muIter->globalTrack()->hitPattern().numberOfValidMuonHits())>0) ) {
	     isTightPrompt=true;
	   }
	 }

	 if ( muIter->innerTrack().isNonnull() ) {
	   muNHits=muIter->innerTrack()->numberOfValidHits();
	 } else {
	   muNHits=-1;
	 }
 	 mudB=muIter->dB();


	 Muv_GmTm.push_back(isGmTm);
	 Muv_Pt.push_back(lpt);
	 Muv_Eta.push_back(leta);
	 Muv_TightPrompt.push_back(isTightPrompt);
	 Muv_NHits.push_back(muNHits);
	 Muv_DRPass.push_back(muDRPass);
	 Muv_dB.push_back(mudB);

	 Muv_DRmin.push_back(muDRmin);
	 Muv_JcntDRmin.push_back(JcntDRmin);

	 ///Isolation Cut
	 lRelIso=(muIter->trackIso()+muIter->ecalIso()+muIter->hcalIso())/lpt;
	 Muv_relIso.push_back(lRelIso);
	 
	 if ( //(muIter->isGlobalMuon())&&(muIter->isTrackerMuon())&&
	     isGmTm&&
	     (lpt>20.0)&&
	     (fabs(leta)<2.1)&&
	     //((muIter->globalTrack()->normalizedChi2())<10.0)&&((muIter->globalTrack()->hitPattern().numberOfValidMuonHits())>0)&&
	     //((muIter->innerTrack()->numberOfValidHits())>10)&&
	     isTightPrompt&&
	     (muNHits>10)&&
	     muDRPass&&
	     //(muIter->dB()<0.02)
	     (mudB<0.02)
	     ) {
	   lcnt_kin++;
	   mucnt_kin++;
	   if ( lRelIso<0.05 ) {
	     lEta=leta;
	     lPhi=muIter->phi();
	     
	     lcnt_iso++;
	     mucnt_iso++;
	     lp4=muIter->p4();
	     lQ=muIter->charge();
	     muPt=lpt;
	   }
	 }
	 
       }
     }


     /// LOOSE Muons:
     mucnt_loosemu=0;

     vector< pat::Muon > const & smuonVec = *muonHandle;
     if (smuonVec.begin() != smuonVec.end()) {
       const vector< pat::Muon >::const_iterator EndmuIter       = smuonVec.end();
       for ( vector< pat::Muon >::const_iterator muIter = smuonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {

	 smupt=muIter->pt();
	 asmueta=abs(muIter->eta());
	 smuRelIso=(muIter->trackIso()+muIter->ecalIso()+muIter->hcalIso())/smupt;
	 if ( (muIter->isGlobalMuon()>0.99)&&(smupt>10.0)&&(asmueta<2.5)&&(smuRelIso<0.2) ) {
	     mucnt_loosemu++;	       
	 }
	 
       }
     }

     /// LOOSE Electrons:
     mucnt_el=0;

     vector< pat::Electron > const & electronVec = *electronHandle;
     if (electronVec.begin() != electronVec.end()) {
       const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
       for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {

	 selet=elIter->et();
	 aseleta=abs(elIter->eta());
	 selRelIso=(elIter->dr03TkSumPt()+elIter->dr03EcalRecHitSumEt()+elIter->dr03HcalTowerSumEt())/selet;

	 if ( (selet>15.0)&&(aseleta<2.5)&&(selRelIso<0.2) ) {
	     mucnt_el++;	       
	 }
	 
       }
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


     ///Z veto nothing:
     ZVeto=false;
//      if ( (lcnt_iso==1) ){
//        vector< pat::Electron > const & electronVec = *electronHandle;
//        if (electronVec.begin() != electronVec.end()) {
// 	 const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
// 	 for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {
// 	   let=elIter->et();
// 	   leta=elIter->eta();
// 	   aeta_sc=abs(elIter->superCluster().get()->eta());

//  	   selRelIso=(elIter->dr04TkSumPt()+elIter->dr04EcalRecHitSumEt()+elIter->dr04HcalTowerSumEt())/let;
// 	   if ( !((let>el_EtMin)&&(abs(leta)<el_etaMax)&&((aeta_sc>el_etascExcludeMax)||(aeta_sc<el_etascExcludeMin))&&(abs(elIter->dB())<d0_Max)&&((elIter->electronID("eidRobustTight"))>0.0001))
// 		//
// 		&&((let>sel_EtMin)&&(abs(leta)<sel_etaMax)&&( (elIter->electronID("eidRobustLoose"))>0.99 )&&(selRelIso<sel_RelIsoMax)) ) {
// 	     //
// 	     if ( ((elIter->charge() * lQ)<0.0)&&
// 		  ((elIter->p4() + lp4).M()>MZ_min)&&
// 		  ((elIter->p4() + lp4).M()<MZ_max) ) {
// 	       ZVeto=true;
// 	     }
// 	   }
// 	 }
//        }
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
	 //if ( mucnt_kin==1 ) {
	   //incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	   if ( mucnt_iso==1 ) {
	     //incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	     if ( mucnt_loosemu==1 ) {
	       //incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	       if ( mucnt_el==0 ) {
		 // incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);	 
		 for (Int_t nj=0; nj<(Nj+1);nj++) {
		   PassLp[6][nj]++;
		   PassMu[6][nj]++;
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
	   //}
	 
       }
     } // for Njets
     // record the L+j hypernews
     if ( (passAll==1)&&(jcnt_tot>3.5) ) {
       //check if we want elEt, elPt or muPt
       outResFile << runNumber << ":" << eventNumber << ":" << luminosityBlock  << ":" << setprecision(8) << muPt << endl; 
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
       
       if ( mucnt_iso>1.5 ) {
	 incrementCounter(2,Nj,PassEl,PassMu,PassLp,1,elcnt_iso);
       }
       if ( mucnt_iso==1 ) {
	 incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	 if ( mucnt_loosemu==1 ) {
	   incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	   if ( mucnt_el==0 ) {
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

