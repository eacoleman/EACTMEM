/////////////////////////////////////////////////////////////////////////////////////////////////
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

//User Defined Includes
#include "RunEventSet.h"

// Root includes
#include "TROOT.h"
#include <TTree.h>

#include <iostream.h>
#include <strstream>
#include <fstream>

using namespace std;


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
   //=".txt";
   outtablefilename=argv[argc-1];
   //+outtablefilename;
   ofstream outtablefile;
   outtablefile.open(outtablefilename,ios::out);
   // corresponds to the next to last string on the command line
   TString outrootfilename;
   //=".root";
   outrootfilename=argv[argc-2];
   //+outrootfilename;
   TFile * outrootfile = new TFile(outrootfilename, "RECREATE");
   TTree *EvtTree = new TTree("EvtTree", "EvtTree");
   // adjust argc to account for inputing the two character strings above
   argc=argc-2;


   ////Selection Parameters
   // cuts   
   Int_t jet_NMax=6; // Loop over the jets from 1 to jet_NMax+1
   Double_t MET_EtMin=25;
   Double_t jet_PtMin=20;
   Double_t l_PtMin=20;
   Double_t eta_Max=2.4; //2.4; A cut on (the absolute vaulue of) eta. Applies to jets and leptons.
   Double_t Vz0_Max=5.0;
   // jets
   Int_t jet_Pass=0; // Counts the number of jets with jpt>jet_PtMin
   Int_t jetCounter=-1;
   Double_t jpt;
   Double_t jeta;
   Double_t jPt[7];
   //Double_t j1Et;
   Double_t j1M;
   Double_t j1Vz;
   Double_t j1Eta;
   //leptons
   Double_t lpt;
   Double_t lPt;
   Double_t leta;
   Double_t lEta;
//    Double_t lEt;
//    Double_t lM;
//    Double_t lVz;
   Int_t lCnt=0;
   Int_t muCnt=0;
   Int_t elCnt=0;

   // other
   Double_t METEt=-1;
   Double_t Vz0=-1;
   // global counters
   Int_t EvtPassCount[6];
   Int_t EvtPassMu[6];
   Int_t EvtPassEl[6];
   Int_t EvtJetCutPass[6];
   Int_t EvtLeptonPass[6];
   for (Int_t Nj=1; Nj<6;Nj++) {
     EvtPassCount[Nj]=0;
     EvtJetCutPass[Nj]=0;
     EvtLeptonPass[Nj]=0;
     EvtPassMu[Nj]=0;
     EvtPassEl[Nj]=0;
   }

   Int_t EvtTotCount=0; //Total Number of Events
   Int_t EvtLRCount=0; //Number of Events within the luminous regions |Vz0|<Vz0_Max;
   // For Testing Purposes Only
//    Double_t Vz0J;
//    Double_t Vz0El;
//    Double_t Vz0Mu;

   EvtTree->Branch("j1Pt",&jPt[1],"j1Pt/D");
   EvtTree->Branch("j2Pt",&jPt[2],"j2Pt/D");
   //   EvtTree->Branch("j6Pt",&jPt[6],"j6Pt/D");
//    EvtTree->Branch("j4Pt",&jPt[4],"j4Pt/D");
//    EvtTree->Branch("j5Pt",&jPt[5],"j5Pt/D");
//    EvtTree->Branch("j1Et",&j1Et,"j1Et/D");
   EvtTree->Branch("j1M",&j1M,"j1M/D");
   EvtTree->Branch("j1Vz",&j1Vz,"j1Vz/D");
   EvtTree->Branch("j1Eta",&j1Eta,"j1Eta/D");
   //   EvtTree->Branch("jetPass",&jetPass,"jetPass/I");
   EvtTree->Branch("lPt",&lPt,"lPt/D");
   EvtTree->Branch("lEta",&lEta,"lEta/D");
//    EvtTree->Branch("lEt",&lEt,"lEt/D");
//    EvtTree->Branch("lM",&lM,"lM/D");
//    EvtTree->Branch("lVz",&lVz,"lVz/D");
//   EvtTree->Branch("lCnt",&lCnt,"lCnt/I");
   EvtTree->Branch("METEt",&METEt,"METEt/D");
//    EvtTree->Branch("Vz0J",&Vz0J,"Vz0J/D");
//    EvtTree->Branch("Vz0Mu",&Vz0Mu,"Vz0Mu/D");
//    EvtTree->Branch("Vz0El",&Vz0El,"Vz0El/D");



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


 
				
      // Loop over the jets & record the needed pts
      const vector< pat::Jet >::const_iterator kJetEnd = jetHandle->end();
      jetCounter=1;
      jet_Pass=0;
      jPt[1]=-1;
      jPt[2]=-1;
      jPt[3]=-1;
      jPt[4]=-1;
      jPt[5]=-1;
      jPt[6]=-1;

      for (vector< pat::Jet >::const_iterator jetIter = jetHandle->begin();
           ( (kJetEnd != jetIter)&&(jet_Pass< (jet_NMax+1) ) ); 
           ++jetIter) 
      {      
   	 jpt=jetIter->pt();
	 jeta=jetIter->eta();
	 Vz0=jetIter->vz();
	 Vz0J=Vz0;
	 if ( (jpt>jet_PtMin)&&(abs(jeta)<eta_Max)&&(abs(Vz0)<Vz0_Max) ) {
	   jet_Pass++;
	 }

         //eventCont.hist("jetPt")->Fill (jpt);
	 if ( jetCounter< (jet_NMax+1) ) {
	   jPt[jetCounter]=jpt;
	 }
	 if ( jet_Pass==1 ) {
	   j1Eta=jetIter->eta();
	   j1M=jetIter->mass();
	   j1Vz=jetIter->vz();
	 }
	 //jetTree->Fill();
	 //eventCont.tree("jetTree")->Fill();




	 jetCounter++;
      } // for jetIter


      lCnt=0;
      //Loop over the Muons
      muCnt=0;
      vector< pat::Muon > const & muonVec = *muonHandle;
      if (muonVec.begin() != muonVec.end()) {
	const vector< pat::Muon >::const_iterator EndIter       = muonVec.end();
	for ( vector< pat::Muon >::const_iterator Iter = muonVec.begin(); ( (Iter!=EndIter)&&(lCnt<2) ) ; ++Iter ) {
	  lpt=Iter->pt();
	  leta=Iter->eta();
	  Vz0=Iter->vz();
	  Vz0Mu=Vz0;
	  if ( (lpt>l_PtMin)&&(abs(leta)<eta_Max)&&(abs(Vz0)<Vz0_Max) ) {
	    lEta=leta;
	    lPt=lpt;
	    lCnt++;
	    muCnt++;
	    //lEt=Iter->et();
	    //lM=Iter->mass();
	    //lVz=Iter->vz();

	  }
	}
      }

      //Loop over the Electrons
      elCnt=0;
      vector< pat::Electron > const & electronVec = *electronHandle;
      if (electronVec.begin() != electronVec.end()) {
	const vector< pat::Electron >::const_iterator EndIter       = electronVec.end();
	for ( vector< pat::Electron >::const_iterator Iter = electronVec.begin(); ( (Iter!=EndIter)&&(lCnt<2) ) ; ++Iter ) {
	  lpt=Iter->pt();
	  leta=Iter->eta();
	  Vz0=Iter->vz();
	  Vz0El=Vz0;
	  if ( (lpt>l_PtMin)&&(abs(leta)<eta_Max)&&(abs(Vz0)<Vz0_Max) ) {
	    lEta=leta;
	    lPt=lpt;
	    lCnt++;
	    elCnt++;
	    //lEt=Iter->et();
	    //lM=Iter->mass();
	    //lVz=Iter->vz();
	  }
	}
      }

      //Check the MET
      METEt=METHandle->front().et();

      
      //       if ( (jet_Pass==2)&&(lCnt==1)&&(METPt>METCutoff) ) {
      // 	EvtTree->Fill();
      // 	EvtPassCount++;
      //       }



      //// scan over the jets
      for (Int_t Nj=1; Nj<6;Nj++) {
	if ( jet_Pass==Nj ) {
	  EvtJetCutPass[Nj]++;
	  if (lCnt==1) {
	    EvtLeptonPass[Nj]++;
	    if ( METEt>MET_EtMin ) {
	      
	      EvtPassCount[Nj]++;
	      if ( muCnt==1) {
		EvtPassMu[Nj]++;
	      }
	      if ( elCnt==1) {
		EvtPassEl[Nj]++;
	      }


	      // if the number of passing jets is 2, fill out the output tree
// 	      if ( Nj==2 ) {
// 		EvtTree->Fill();
// 	      }	      
	    }
	  }
	}
      } // for Njets
      EvtTree->Fill();
      EvtTotCount++;
      if ( abs(Vz0)<Vz0_Max ) {
	EvtLRCount++;
      }


   } // for eventCont
   
   ////produce the output table
   float fracpass = 0.0;
   float fracLR=0;
   float fracLRpass=0.0;
   float fracmu=0.0;
   float fracmuLR=0.0;
   float fracel=0.0;
   float fracelLR=0.0;
   float Epass;
   float Etot, EtotLR;
   float ElPercentage=0;
   float MuPercentage=0;
   Etot=EvtTotCount;
   EtotLR=EvtLRCount;
   for (Int_t Nj=1; Nj<6;Nj++) {
     Epass=EvtPassCount[Nj];
	
     if ( EvtTotCount!=0 ) {
       fracpass=Epass/Etot;
       fracLR=EtotLR/EvtTotCount;
       fracmu=EvtPassMu[Nj]/Etot;
       fracmuLR=EvtPassMu[Nj]/EtotLR;
       fracel=EvtPassEl[Nj]/Etot;
       fracelLR=EvtPassEl[Nj]/EtotLR;
       fracLRpass=Epass/EtotLR;
       ElPercentage=fracel/fracpass;
       MuPercentage=fracmu/fracpass;
     } else {
       fracpass=-1;
       fracmu=-1;
       fracel=-1;
     }

     outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "ELECTRONS: " << ElPercentage*100 << "%" << endl;
     outtablefile << Nj << " Jets " << setprecision(3) << fracel << "  elPass/Tot   " << fracelLR <<  " =elPass/NLR   ||  InEvts=" << EvtTotCount << " JetCuts=" << EvtJetCutPass[Nj] << " ElectronCut=" << ElPercentage*EvtLeptonPass[Nj] <<" PassEvts=" << ElPercentage*EvtPassCount[Nj] <<  endl;
       
     //outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "MUONS: " << MuPercentage*100 << "%" << endl;
     outtablefile << Nj << " Jets " << setprecision(3) << fracmu << "  muPass/Tot   " << fracmuLR <<  " =muPass/NLR   ||  InEvts=" << EvtTotCount << " JetCuts=" << EvtJetCutPass[Nj] << "     MuonCut=" << MuPercentage*EvtLeptonPass[Nj] <<" PassEvts=" << MuPercentage*EvtPassCount[Nj] <<  endl;
     
     //outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "LEPTONS:" << endl;
     outtablefile << Nj << " Jets " << setprecision(3) << fracpass  << " TotPass/Tot   " << fracLRpass <<  " =TotPass/NLR    ||  InEvts=" << EvtTotCount << " JetCuts=" << EvtJetCutPass[Nj] << "   LeptonCut=" << EvtLeptonPass[Nj] <<" PassEvts=" << EvtPassCount[Nj]  << endl;

   }

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
