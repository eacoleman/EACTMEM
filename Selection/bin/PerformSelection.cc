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
   // corresponds to the next to last string on the command line
   TString outrootfilename;
   outrootfilename=argv[argc-2];
   TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
   // adjust argc to account for inputing the two character strings above
   argc=argc-2;

   // create the tree
   TTree* EvtTree = new TTree("EvtTree", "Output tree for matrix element");
   EventNtuple * EvtNtuple = new EventNtuple();
   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);



   ////Selection Parameters
   // cuts   
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
   Double_t bDiscriminator_min=2.03; //2.03 corresponds to the 'medium' cut.

   // jets
   Int_t jcnt_tot;
   Int_t nBtag;
   Double_t jpt;
   Double_t jeta;
   Double_t jphi;
   Double_t Mjj;
   vector <math::XYZTLorentzVector> jp4;
   bool IsjBtag;
   vector <int> jBtag;

   //leptons
   Double_t let;
   Double_t lPhi;
   Double_t lrelIso;
   Double_t leta;
   Double_t lEta;
   Double_t lvtxChi2;
   Double_t muEcalE;
   Double_t muHcalE;
   Int_t lQ;
   bool ZVeto=false;
   bool ConvVeto=false;//Always set to false for now
   math::XYZTLorentzVector lp4;
   TLorentzVector jp4LV;
   Int_t passAll=0; //=1 for mu, =2 for el if the event passes all of the cuts


   Int_t lcnt_kin=0;
   Int_t lcnt_iso=0;
   Int_t mucnt_kin=0;
   Int_t mucnt_iso=0;
   Int_t elcnt_kin=0;
   Int_t elcnt_iso=0;


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
     lcnt_kin=0;
     lcnt_iso=0;

     /// Electrons:
     elcnt_kin=0;
     elcnt_iso=0;
     vector< pat::Electron > const & electronVec = *electronHandle;
     if (electronVec.begin() != electronVec.end()) {
       const vector< pat::Electron >::const_iterator EndelIter       = electronVec.end();
       for ( vector< pat::Electron >::const_iterator elIter = electronVec.begin(); ( (elIter!=EndelIter) ) ; ++elIter ) {
	 let=elIter->et();
	 leta=elIter->eta();

	 /// Luminous Region Cut
	 if ( abs(elIter->vz())<Vz0_Max ) {
	   if ( LRPresent==false ) {
	     cnt_LR++;
	     LRPresent=true;
	   }
	   
	   /// Kinematics Cut
	   if ( (let>l_EtMin)&&(abs(leta)<l_etaMax)&&(abs(elIter->dB())<dB_Max)&&((elIter->electronID("eidRobustTight"))>0.99)&&((elIter->vertexChi2())<lvtxChi2_Max) ) {
	     lcnt_kin++;
	     elcnt_kin++;
	     ///Isolation Cut
	     lrelIso=(elIter->caloIso()+elIter->trackIso())/let;
	     if ( lrelIso<l_relIsoMax ) {
	       lEta=leta;
	       lPhi=elIter->phi();
	       
	       lcnt_iso++;
	       elcnt_iso++;
	       lp4=elIter->p4();
	       lQ=elIter->charge();
	     }
	   }
	 }
       }
     }

     /// Muons:
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
	     lcnt_kin++;
	     mucnt_kin++;
	     ///Isolation Cut
	     lrelIso=(muIter->caloIso()+muIter->trackIso())/let;
	     if ( lrelIso<l_relIsoMax ) {
	       lEta=leta;
	       lPhi=muIter->phi();
	       
	       lcnt_iso++;
	       mucnt_iso++;
	       lp4=muIter->p4();
	       lQ=muIter->charge();	       
	     }
	   }
	 }
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
	 }
       } // for jetIter
     if ( jcnt_tot!=2 ) {
       Mjj=-2;
     }

     //// METs:
     METp4=METHandle->front().p4();
     METEt=METp4.E();
     
     ///Perform the Z veto
     ZVeto=false;
     //Z veto the muons:
     if ( (mucnt_iso==1)&&(lcnt_iso==1) ) {
       vector< pat::Muon > const & muonVec = *muonHandle;
       if (muonVec.begin() != muonVec.end()) {
	 const vector< pat::Muon >::const_iterator EndmuIter       = muonVec.end();
	 for ( vector< pat::Muon >::const_iterator muIter = muonVec.begin(); ( (muIter!=EndmuIter) ) ; ++muIter ) {
	   let=muIter->et();
	   reco::MuonEnergy muonEnergy=muIter->calEnergy();
	   muEcalE=muonEnergy.em;
	   muHcalE=muonEnergy.had;
	   //lvtxChi2=muIter->vertexChi2();
	   lrelIso=(muIter->caloIso()+muIter->trackIso())/let;

	   if ( !((let>l_EtMin)&&(abs(muIter->dB())<dB_Max)&&(muEcalE<muEcalE_Max)&&(muHcalE<muHcalE_Max)&&((muIter->vertexChi2())<lvtxChi2_Max)&&(lrelIso<l_relIsoMax))
		&&((let>sl_EtMin)&&(abs(muIter->eta())<l_etaMax)&&((muIter->isGood("All"))>0.99)&&(abs(muIter->vz())<Vz0_Max))    ) {
	     if ( ((muIter->charge() * lQ)<0.0)&&
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
	   lrelIso=(elIter->caloIso()+elIter->trackIso())/let;

	   if ( !((let>l_EtMin)&&(abs(elIter->dB())<dB_Max)&&((elIter->electronID("eidRobustTight"))>0.99)&&(lrelIso<l_relIsoMax)&&((elIter->vertexChi2())<lvtxChi2_Max))
		&&((let>sl_EtMin)&&(abs(leta)<l_etaMax)&&( (elIter->electronID("eidRobustLoose"))>0.99 )&&(abs(elIter->vz())<Vz0_Max)) ) {
	     //
	     if ( ((elIter->charge() * lQ)<0.0)&&
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

	 if ( lcnt_iso==1 ) {
	   incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	   if ( METEt>MET_EtMin ) {
	     incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	     if ( ZVeto==false ) {
	       incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
	       if ( ConvVeto==false ) {
		 incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		 if ( abs(lEta)<l_etaMaxBARREL ) {
		   incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		   if ( mucnt_iso==1 ) {
		   passAll=1;
		   }
		   if ( elcnt_iso==1 ) {
		   passAll=2;
		   }
		   if ( nBtag==0 ) {
		     incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		   } else {
		     if ( nBtag==1 ) {
		       incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		     } else {
		       if ( nBtag==2 ) {
			 incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
		       } else {
			 incrementCounter(12,Nj,PassEl,PassMu,PassLp,mucnt_iso,elcnt_iso);
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

     ///fill the Ntuple to be used in Matrix Element computations.

     if ( (passAll>0.5)&&(jcnt_tot==2) ) {
       EvtNtuple->lLV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
       EvtNtuple->jLV.clear();
       jp4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
       EvtNtuple->jLV.push_back(jp4LV);
       jp4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
       EvtNtuple->jLV.push_back(jp4LV);
       EvtNtuple->METLV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
       
       EvtNtuple->jBtag.clear();
       EvtNtuple->jBtag.push_back(jBtag[0]);
       EvtNtuple->jBtag.push_back(jBtag[1]);

       EvtNtuple->lQ=lQ;
       if (abs(lEta)<l_etaMaxBARREL) {
	 //barrel
	 EvtNtuple->ldetComp=0;
       } else {
	 //endcap (should never happen with the current selection):
	 EvtNtuple->ldetComp=1;
       }
       EvtNtuple->run=runNumber;
       EvtNtuple->event=eventNumber;
       
       
       //Additional Variables
       EvtNtuple->Mjj=Mjj;
       EvtNtuple->passAll=passAll;

       //Fill The Tree  
       EvtTree->Fill();
     }
     
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

   return 0;

}


