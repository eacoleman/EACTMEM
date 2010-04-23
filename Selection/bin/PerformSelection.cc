////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Preliminary Sensitivity Analysis
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

// CMS includes
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "PhysicsTools/FWLite/interface/EventContainer.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 

//User Defined Ntuple
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

//User Defined Includes
#include "RunEventSet.h"

// Root includes
#include "TROOT.h"
#include <TTree.h>
#include <TLorentzVector.h>
// #include "HTValOrderedVector.h"
// #include "Tuple.h"
// #include "TupleManager.h"

#include <iostream>
#include <strstream>
#include <fstream>

using namespace std;
#include <TAMUWW/Tools/GlobalTools.cc>
//#include <GlobalTools.cc>

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
   //parser.stringValue ("outputFile") = "jetPt"; // .root added automatically


   //// Output Files:
   // corresponds to the last string on the command line
   TString outtablefilename;
   outtablefilename=argv[argc-1];
   ofstream outtablefile;
   outtablefile.open(outtablefilename,ios::out);
   //const char* outtablefilename=argv[argc-1];
   // corresponds to the next to last string on the command line
   TString outrootfilename;
   outrootfilename=argv[argc-2];
   TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
   //TTree *EvtTree = new TTree("EvtTree", "EvtTree");
   // adjust argc to account for inputing the two character strings above
   argc=argc-2;

   TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
   EventNtuple * EvtNtuple = new EventNtuple();
   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);

   //fill the evtNtuple object
   EvtNtuple->Mjj = -1;

   //Testing:
   //   Double_t lId;
   // Double_t lId_loose;
//    Double_t j1BDiscr;
//    Double_t lM=-1;
//    Double_t j1emFrac;
//    Double_t DRj1=-1;
//    Double_t lEt=-1;			
//    Double_t lDB=-1;
//    Double_t lVz0=-1;
//    Int_t isEl=-1; //=1 for El, 0 for Mu, -1 by default 
//    Double_t DRMin=1000;
//    Int_t MucntIso; 
//    Int_t ElcntIso;


   ////Selection Parameters
   // cuts   
   //Int_t jet_NMax=6;
   Double_t MET_EtMin=30;
   Double_t j_PtMin=30;
   Double_t l_EtMin=30;
   Double_t sl_EtMin=20;
   Double_t j_etaMax=2.4; //2.4; A cut on (the absolute vaulue of) eta.
   Double_t l_etaMax=2.5;
   Double_t jemFrac_Min=0.01;
   Double_t muEcalE_Max=4.0;
   Double_t muHcalE_Max=6.0;
   Double_t lvtxChi2_Max=10.0;


   Double_t l_relIsoMax=0.1;
   Double_t Rlj_Min=0.3;

   Double_t l_etaMaxBARREL=1.442;
   Double_t Vz0_Max=7.5; //7.465 gives 95% efficiency
   Double_t dB_Max=0.2;
   Double_t MZ_min=76.0;
   Double_t MZ_max=106.0;

   // jets
   Int_t jcnt_tot;
   Int_t jBtag;
   // Int_t jet_Pass=0; // Counts the number of jets with jpt>jet_PtMin
   // Int_t jetCounter=-1;
   Double_t jpt;
   Double_t jeta;
   Double_t jphi;
   //Double_t jemFrac;
   Double_t Mjj;
   math::XYZTLorentzVector j1p4;


   //leptons
   Double_t let;
   //Double_t lphi;
   Double_t lPhi;
   Double_t lrelIso;
   Double_t leta;
   Double_t lEta;
   Double_t lvtxChi2;
   Double_t muEcalE;
   Double_t muHcalE;
   //Double_t lId;
   Int_t lq;
   bool ZVeto=false;
   bool ConvVeto=false;//Always set to false for now
   math::XYZTLorentzVector lp4;
   Int_t passAll=0; //=1 for mu, =2 for el if the event passes all of the cuts

   Int_t lcnt_kin=0;
   Int_t lcnt_iso=0;
   Int_t mucnt_kin=0;
   Int_t mucnt_iso=0;
   Int_t elcnt_kin=0;
   Int_t elcnt_iso=0;


   // other
   //Double_t dB;
   Double_t Rlj;
   Double_t METEt=-1;
   //Double_t Vz0=-1;
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

//    // For Testing Purposes Only
//    Double_t j1M;
//    Double_t j1Vz;
//    Double_t j1Eta;
//    Double_t jPt[7];

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
//    EvtTree->Branch("LcntKin",&lcnt_kin,"LcntKin/I");
//    EvtTree->Branch("MucntKin",&mucnt_kin,"MucntKin/I");
//    EvtTree->Branch("ElcntKin",&elcnt_kin,"ElcntKin/I");
//    EvtTree->Branch("JcntTot",&jcnt_tot,"JcntTot/I");
//    EvtTree->Branch("LcntIso",&lcnt_iso,"LcntIso/I");
//    EvtTree->Branch("MucntIso",&mucnt_iso,"MucntIso/I");
//    EvtTree->Branch("ElcntIso",&elcnt_iso,"ElcntIso/I");
//    EvtTree->Branch("muEcalE",&muEcalE,"muEcalE/D");
//    EvtTree->Branch("muHcalE",&muHcalE,"muHcalE/D");

//    EvtTree->Branch("METEt",&METEt,"METEt/D");

// // // // //    EvtTree->Branch("passAll",&passAll,"passAll/I");
// // // // //    EvtTree->Branch("Mjj",&Mjj,"Mjj/D");


//    // Component Quantities.
//    vector < double > Jv_Pt, Jv_DR, Jv_Eta, Jv_Phi, Elv_Et, Muv_Et, Elv_Eta, Muv_Eta, Elv_DB, Muv_DB, Elv_Vz0, Muv_Vz0, Elv_Id, Muv_Id, Elv_relIso, Muv_relIso, Elv_Phi, Muv_Phi, Muv_EcalE, Muv_HcalE;
//    EvtTree->Branch("Jv_Pt",&Jv_Pt);
//    EvtTree->Branch("Jv_DR",&Jv_DR);
//    EvtTree->Branch("Jv_Eta",&Jv_Eta);
//    EvtTree->Branch("Jv_Phi",&Jv_Phi);
//    EvtTree->Branch("Elv_Et",&Elv_Et);
//    EvtTree->Branch("Muv_Et",&Muv_Et);
//    EvtTree->Branch("Elv_Eta",&Elv_Eta);
//    EvtTree->Branch("Muv_Eta",&Muv_Eta);
//    EvtTree->Branch("Elv_DB",&Elv_DB);
//    EvtTree->Branch("Muv_DB",&Muv_DB);
//    EvtTree->Branch("Elv_Vz0",&Elv_Vz0);
//    EvtTree->Branch("Muv_Vz0",&Muv_Vz0);
//    EvtTree->Branch("Elv_Id",&Elv_Id);
//    EvtTree->Branch("Muv_Id",&Muv_Id);
//    EvtTree->Branch("Elv_relIso",&Elv_relIso);
//    EvtTree->Branch("Muv_relIso",&Muv_relIso);
//    EvtTree->Branch("Elv_Phi",&Elv_Phi);
//    EvtTree->Branch("Muv_Phi",&Muv_Phi);
//    EvtTree->Branch("Muv_EcalE",&Muv_EcalE);
//    EvtTree->Branch("Muv_HcalE",&Muv_HcalE);

   

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
   //eventCont.add( new TH1F( "jetPtg20", "jetPtg20", 1000, 0, 1000) );
   //   eventCont.add( new TTree("jetTree", "jetTree") );

   //jetTree->Branch("jPt",&Pt,"jPt/D");

   // eventCont.add( new TH1F( "jetPtMETPtg20", "jetPtMETPtg20", 1000, 0, 1000) );


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
     fwlite::Handle< vector< pat::Jet > > jetHandle; //? define the vector
     jetHandle.getByLabel (eventCont, "selectedLayer1Jets"); //? load all of the (layer 1???) jets for the event into jetHandle
     assert ( jetHandle.isValid() );//?? ensure that there's at least one jet
	
     fwlite::Handle< vector< pat::Muon > > muonHandle;
     muonHandle.getByLabel (eventCont, "selectedLayer1Muons");
     assert ( muonHandle.isValid() );
     fwlite::Handle< vector< pat::Electron > > electronHandle;
     electronHandle.getByLabel (eventCont, "selectedLayer1Electrons");
     assert ( electronHandle.isValid() );

     fwlite::Handle< vector< pat::MET > > METHandle;
     METHandle.getByLabel (eventCont, "layer1METs");
     assert ( METHandle.isValid() );


     ////// Perform The Selection
     LRPresent=false;

     //// LEPTONS:
//      isEl=-1;
//      lEt=-1;
//      lDB=-1;
//      lVz0=-1;
//      lrelIso=-1;
//      lId=-1;
//      lM=-1;
     lcnt_kin=0;
     lcnt_iso=0;

     /// Electrons:
//      Elv_Et.clear();
//      Elv_Eta.clear();
//      Elv_DB.clear();
//      Elv_Vz0.clear();
//      Elv_Id.clear();
//      Elv_Phi.clear();
// //      Elv_EcalE.clear();
// //      Elv_HcalE.clear();
//      lrelIso=-1;
//      Elv_relIso.clear();

     elcnt_kin=0;
     elcnt_iso=0;
     vector< pat::Electron > const & electronVec = *electronHandle;
     if (electronVec.begin() != electronVec.end()) {
       const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
       for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {
	 let=elIter->et();
	 leta=elIter->eta();
	 //lphi=elIter->phi();
	 //Vz0=elIter->vz();
	 //	 dB=elIter->dB();
	 //	 lId=elIter->electronID("eidRobustTight");
// 	 lEcalE=elIter->ecalEnergy();
// 	 lHcalE=(elIter->hcalOverEcal())*lEcalE;
//	 lvtxChi2=elIter->vertexChi2();

	 /// Luminous Region Cut
	 if ( abs(elIter->vz())<Vz0_Max ) {
	   if ( LRPresent==false ) {
	     cnt_LR++;
	     LRPresent=true;
	   }
	   
	   /// Kinematics Cut
	   if ( (let>l_EtMin)&&(abs(leta)<l_etaMax)&&(abs(elIter->dB())<dB_Max)&&((elIter->electronID("eidRobustTight"))>0.99)&&((elIter->vertexChi2())<lvtxChi2_Max) ) {
	     //&&(lEcalE>lEcalE_Min)&&(lHcalE>lHcalE_Min)
	     //	    lPt=lpt;
	     lcnt_kin++;
	     elcnt_kin++;
	     ///Isolation Cut
	     lrelIso=(elIter->caloIso()+elIter->trackIso())/let;
	     if ( lrelIso<l_relIsoMax ) {
// 	       lEt=let;
// 	       lDB=dB;
// 	       lVz0=Vz0;
	       lEta=leta;
	       lPhi=elIter->phi();
	       
	       lcnt_iso++;
	       elcnt_iso++;
	       lp4=elIter->p4();
	       lq=elIter->charge();
	       //	       lM=elIter->mass();
	     }
	   }
	 }
// 	 Elv_Et.push_back(let);
// 	 Elv_Eta.push_back(leta);
// 	 Elv_DB.push_back(dB);
// 	 Elv_Vz0.push_back(Vz0);
// 	 Elv_Id.push_back(lId);
// 	 Elv_Phi.push_back(lphi);
// 	 Elv_relIso.push_back(lrelIso);
	 // 	 Elv_EcalE.push_back(lEcalE);
	 // 	 Elv_HcalE.push_back(lHcalE);
	 
       }
     }

     /// Muons:
//      Muv_Et.clear();
//      Muv_Eta.clear();
//      Muv_DB.clear();
//      Muv_Vz0.clear();
//      Muv_Id.clear();
//      Muv_Phi.clear();
//      Muv_EcalE.clear();
//      Muv_HcalE.clear();
//      lrelIso=-1;
//      Muv_relIso.clear();

     muEcalE=100;
     muHcalE=100;

     mucnt_kin=0;
     mucnt_iso=0;
     vector< pat::Muon > const & muonVec = *muonHandle;
     if (muonVec.begin() != muonVec.end()) {
       const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
       for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {
	 let=muIter->et();
	 leta=muIter->eta();
// 	 lphi=muIter->phi();
// 	 Vz0=muIter->vz();
// 	 dB=muIter->dB();
// 	 lId=muIter->isGood("All");
	 reco::MuonEnergy muonEnergy=muIter->calEnergy();
	 muEcalE=muonEnergy.em;
	 muHcalE=muonEnergy.had;
	 lvtxChi2=muIter->vertexChi2();

	 /// Luminous Region Cut
	 if ( abs(muIter->vz())<Vz0_Max ) {
	   if ( LRPresent==false ) {
	     cnt_LR++;
	     LRPresent=true;
	   }
	   
	   ///Kinematics Cut
	   if ( (let>l_EtMin)&&(abs(leta)<l_etaMax)&&(abs(muIter->dB())<dB_Max)&&((muIter->isGood("All"))>0.99)&&(muEcalE<muEcalE_Max)&&(muHcalE<muHcalE_Max)&&((muIter->vertexChi2())<lvtxChi2_Max) ) {
	     //
	     //	    lPt=lpt;
	     lcnt_kin++;
	     mucnt_kin++;
	     ///Isolation Cut
	     lrelIso=(muIter->caloIso()+muIter->trackIso())/let;
	     if ( lrelIso<l_relIsoMax ) {
	       //	       lEt=let;
// 	       lDB=dB;
// 	       lVz0=Vz0;
	       lEta=leta;
	       lPhi=muIter->phi();
	       
	       lcnt_iso++;
	       mucnt_iso++;
	       lp4=muIter->p4();
	       lq=muIter->charge();
// 	       lM=muIter->mass();
	       
	     }
	   }
	 }
// 	 Muv_Et.push_back(let);
// 	 Muv_Eta.push_back(leta);
// 	 Muv_DB.push_back(dB);
// 	 Muv_Vz0.push_back(Vz0);
// 	 Muv_Id.push_back(lId);	
// 	 Muv_Phi.push_back(lphi);
// 	 Muv_relIso.push_back(lrelIso);
//  	 Muv_EcalE.push_back(muEcalE);
//  	 Muv_HcalE.push_back(muHcalE);

       }
     }

     //// JETS:
//      Jv_Pt.clear();
//      Jv_Eta.clear();
//      Jv_Phi.clear();
     Rlj=-1;
//      Jv_DR.clear();
     //DRMin=1000;
     Mjj=-1;
     //isEl=-1;
     jBtag=0;
     //j_DRfail=false;
     jcnt_tot=0;


     const vector< pat::Jet >::const_iterator kJetEnd = jetHandle->end();
     //     jetCounter=1;
     //     jet_Pass=0;
//      jPt[1]=-1;
//      jPt[2]=-1;
//      jPt[3]=-1;
//      jPt[4]=-1;
//      jPt[5]=-1;
//      jPt[6]=-1;
     
     for (vector< pat::Jet >::const_iterator jetIter = jetHandle->begin();
	  ( kJetEnd != jetIter ); 
	  ++jetIter ) 
       { 
   	 jpt=jetIter->pt();
	 jeta=jetIter->eta();
// 	 jemFrac=jetIter->emEnergyFraction();
// 	 Vz0=jetIter->vz();
	 /// Luminous Region Cut
	 if ( abs(jetIter->vz())<Vz0_Max ) {
	   if ( LRPresent==false ) {
	     cnt_LR++;
	     LRPresent=true;
	   }

	   if ( lcnt_iso==1 ){
	     jphi=jetIter->phi();
	     Rlj=pow((jeta-lEta)*(jeta-lEta)+(jphi-lPhi)*(jphi-lPhi),0.5);
	   }
	   /// Jet Kinematics And Cleaning Cut:
	   if ( (jpt>j_PtMin)&&(abs(jeta)<j_etaMax)&&((jetIter->emEnergyFraction())>jemFrac_Min) 
		&&((lcnt_iso!=1)||(Rlj>Rlj_Min)) ) {
	     //jet_Pass++;
	     jcnt_tot++;
	     if ( jcnt_tot==1 ) {
	       j1p4=jetIter->p4();
	     }
	     if ( jcnt_tot==2 ) {
	       Mjj=(j1p4+jetIter->p4()).M();
	     }
	     if ( (jetIter->bDiscriminator("simpleSecondaryVertexBJetTags"))>2.03 ) {
	       //2.03 corresponds to the 'medium' cut.
	       jBtag++;
	     }
	   }
	 }
  
// 	 Jv_Pt.push_back(jpt);
// 	 Jv_Eta.push_back(jeta);
// 	 Jv_Phi.push_back(jphi);
// 	 Jv_DR.push_back(Rlj);
	 

//          //eventCont.hist("jetPt")->Fill (jpt);
// 	 if ( jetCounter< (jet_NMax+1) ) {
// 	   jPt[jetCounter]=jpt;
// 	 }
// 	 if ( jet_Pass==1 ) {
// 	   j1Eta=jetIter->eta();
// 	   j1M=jetIter->mass();
// 	   j1Vz=jetIter->vz();
// 	   j1emFrac=jetIter->emEnergyFraction();
// 	   //j1BDiscr=jetIter->bDiscriminator( "jetProbabilityBJetTags" );
// 	   j1BDiscr=jetIter->bDiscriminator("simpleSecondaryVertexBJetTags");
// 	 }
// 	 jetCounter++;

       } // for jetIter
     if ( jcnt_tot!=2 ) {
       Mjj=-2;
     }

     //METs
     METEt=METHandle->front().et();

     
     ///Perform the Z veto
     ZVeto=false;
     //Z veto the muons:
     if ( (mucnt_iso==1)&&(lcnt_iso==1) ) {
       vector< pat::Muon > const & muonVec = *muonHandle;
       if (muonVec.begin() != muonVec.end()) {
	 const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
	 for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {
	   let=muIter->et();
	   //leta=muIter->eta();
	   //lphi=muIter->phi();
// 	   Vz0=muIter->vz();
// 	   dB=muIter->dB();
// 	   lId=muIter->isGood("All");
	   reco::MuonEnergy muonEnergy=muIter->calEnergy();
	   muEcalE=muonEnergy.em;
	   muHcalE=muonEnergy.had;
	   //lvtxChi2=muIter->vertexChi2();
	   lrelIso=(muIter->caloIso()+muIter->trackIso())/let;

	   if ( !((let>l_EtMin)&&(abs(muIter->dB())<dB_Max)&&(muEcalE<muEcalE_Max)&&(muHcalE<muHcalE_Max)&&((muIter->vertexChi2())<lvtxChi2_Max)&&(lrelIso<l_relIsoMax))
		&&((let>sl_EtMin)&&(abs(muIter->eta())<l_etaMax)&&((muIter->isGood("All"))>0.99)&&(abs(muIter->vz())<Vz0_Max))    ) {
	     if ( ((muIter->charge() * lq)<0.0)&&
		  ((muIter->p4() + lp4).M()>MZ_min)&&
		  ((muIter->p4() + lp4).M()<MZ_max) ) {
	       ZVeto=true;
	     }
	   }
	 }
       }
     }
     //Z veto the electrons:
     if ( (elcnt_iso==1)&&(lcnt_iso==1) ){
       vector< pat::Electron > const & electronVec = *electronHandle;
       if (electronVec.begin() != electronVec.end()) {
	 const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
	 for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {
	   let=elIter->et();
// 	   leta=elIter->eta();
// 	   lphi=elIter->phi();
// 	   Vz0=elIter->vz();
// 	   dB=elIter->dB();
	   //lId=elIter->electronID("eidRobustTight");
	   //	   lvtxChi2=elIter->vertexChi2();
	   lrelIso=(elIter->caloIso()+elIter->trackIso())/let;

	   if ( !((let>l_EtMin)&&(abs(elIter->dB())<dB_Max)&&((elIter->electronID("eidRobustTight"))>0.99)&&(lrelIso<l_relIsoMax)&&((elIter->vertexChi2())<lvtxChi2_Max))
		&&((let>sl_EtMin)&&(abs(leta)<l_etaMax)&&( (elIter->electronID("eidRobustLoose"))>0.99 )&&(abs(elIter->vz())<Vz0_Max)) ) {
	     //
	     if ( ((elIter->charge() * lq)<0.0)&&
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
	 PassLp[2][Nj]++;
	 PassMu[2][Nj]++;
	 PassEl[2][Nj]++;
	 
	 if ( lcnt_kin>0.5 ) {
	   PassLp[3][Nj]++;
	   if ( mucnt_kin>0.5) {
	     PassMu[3][Nj]++;
	   }
	   if ( elcnt_kin>0.5) {
	     PassEl[3][Nj]++;
	   }
	 }
	 
	 if (lcnt_iso==1) {
	   PassLp[4][Nj]++;
	   if ( mucnt_iso==1) {
	     PassMu[4][Nj]++;
	   }
	   if ( elcnt_iso==1) {
	     PassEl[4][Nj]++;
	   }
	 
	   if ( METEt>MET_EtMin ) {
	     PassLp[5][Nj]++;
	     if ( mucnt_iso==1) {
	       PassMu[5][Nj]++;
	     }
	     if ( elcnt_iso==1) {
	       PassEl[5][Nj]++;
	     }
	     if ( ZVeto==false ) {
	       PassLp[6][Nj]++;
	       if ( mucnt_iso==1) {
		 PassMu[6][Nj]++;
	       }
	       if ( elcnt_iso==1) {
		 PassEl[6][Nj]++;
	       } 
	       if ( ConvVeto==false ) {
		 //all events should pass this cut
		 PassLp[7][Nj]++;
		 if ( mucnt_iso==1) {
		   PassMu[7][Nj]++;
		 }
		 if ( elcnt_iso==1) {
		   PassEl[7][Nj]++;
		 }   
		 
		 if ( abs(lEta)<l_etaMaxBARREL ) {
		   //EvtPassCount[Nj]++;
		   PassLp[8][Nj]++;
		   if ( mucnt_iso==1) {
		     PassMu[8][Nj]++;
		     passAll=1;
		   }
		   if ( elcnt_iso==1) {
		     PassEl[8][Nj]++;
		     passAll=2;
		   }

		   if ( jBtag==0 ) {
		     //BTag[0][Nj]++;
		     PassLp[9][Nj]++;
		     if ( mucnt_iso==1) {
		       //MuBTag[0][Nj]++;
		       PassMu[9][Nj]++;
		     }
		     if ( elcnt_iso==1) {
		       //ElBTag[0][Nj]++;
		       PassEl[9][Nj]++;
		     }
		   } else {
		     if ( jBtag==1 ) {
		       //BTag[1][Nj]++;
		       PassLp[10][Nj]++;
		       if ( mucnt_iso==1) {
			 //MuBTag[1][Nj]++;
			 PassMu[10][Nj]++;
		       }
		       if ( elcnt_iso==1) {
			 //ElBTag[1][Nj]++;
			 PassEl[10][Nj]++;
		       }
		     } else {
		       if ( jBtag==2 ) {
			 //BTag[2][Nj]++;
			 PassLp[11][Nj]++;
			 if ( mucnt_iso==1) {
			   //MuBTag[2][Nj]++;
			   PassMu[11][Nj]++;
			 }
			 if ( elcnt_iso==1) {
			   //ElBTag[2][Nj]++;
			   PassEl[11][Nj]++;
			 }
		       } else {
			 //BTag[3][Nj]++;
			 PassLp[12][Nj]++;
			 if ( mucnt_iso==1) {
			   //MuBTag[3][Nj]++;
			   PassMu[12][Nj]++;
			 }
			 if ( elcnt_iso==1) {
			   //ElBTag[3][Nj]++;
			   PassEl[12][Nj]++;
			 }
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

     EvtNtuple->Mjj=Mjj;
     EvtTree->Fill();
     EvtTotCount++;

   } // for eventCont

   for (Int_t Nj=0; Nj<NJETS;Nj++) {
     PassLp[0][Nj]=EvtTotCount;
     PassMu[0][Nj]=EvtTotCount;
     PassEl[0][Nj]=EvtTotCount;
     PassLp[1][Nj]=cnt_LR;
     PassMu[1][Nj]=cnt_LR;
     PassEl[1][Nj]=cnt_LR;
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

   // Histograms will be automatically written to the root file
   // specificed by command line options.

   // All done!  Bye bye.
   //cout << "TESTING 123" << endl;
   return 0;

}


