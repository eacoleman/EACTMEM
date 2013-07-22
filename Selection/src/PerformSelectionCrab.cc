////////////////////////////////////////////////////////////////////////////////
//
// PerformSelectionCrab
// --------------------
//
//                         01/27/2012 Ilya Osipenkov <ilyao@fnal.gov> 
//                                    Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// CMS 
// Stage 2 Skimming of the PAT-tuples to be used in Matrix Element calculations.
// WW/H->WW CrossSection Measurement using Matrix Element 
////////////////////////////////////////////////////////////////////////////////
// Designed to work with 4_2_8 PAT-tuples and tested on V3 HWW sync exercises.
// Use to skim the PATNtuples and create a custom made .root file containing the needed information.
////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/Selection/interface/PerformSelectionCrab.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
PerformSelection::PerformSelection(const edm::ParameterSet& iConfig)
   // Sample initialization
   //: s1(0) //An int defined in the header
   //, s2(false) //A bool defined in the header
   //, s3(0.1) //A double defined in the header
{
   //-----Input Tags For Handles
   triggerSource              =         iConfig.getParameter<edm::InputTag>   ("triggerSource");
   vtxSource                  =         iConfig.getParameter<edm::InputTag>   ("vtxSource");
   genParticleSource          =         iConfig.getParameter<edm::InputTag>   ("genParticleSource");
   pfJetSource                =         iConfig.getParameter<edm::InputTag>   ("pfJetSource");
   electronSource             =         iConfig.getParameter<edm::InputTag>   ("electronSource");
   muonSource                 =         iConfig.getParameter<edm::InputTag>   ("muonSource");
   METSource                  =         iConfig.getParameter<edm::InputTag>   ("METSource");
   rhoSource                  =         iConfig.getParameter<edm::InputTag>   ("rhoSource");
   pileupSource               =         iConfig.getParameter<edm::InputTag>   ("pileupSource");

   //-----Trigger Information
   muTrigger                  =         iConfig.getParameter<vector<string> > ("muTrigger");
   eleTrigger                 =         iConfig.getParameter<vector<string> > ("eleTrigger");

   //-----Program Level Inputs
   outtablefilename           = TString(iConfig.getParameter<string>          ("outtablefilename"));
   outtablenameEl             = TString(iConfig.getParameter<string>          ("outtablenameEl"));
   outtablenameMu             = TString(iConfig.getParameter<string>          ("outtablenameMu"));
   outtablenameLp             = TString(iConfig.getParameter<string>          ("outtablenameLp"));

   printEventInfo             =         iConfig.getParameter<bool>            ("printEventInfo");
   printJetInfo               =         iConfig.getParameter<bool>            ("printJetInfo");
   printLeptonInfo            =         iConfig.getParameter<bool>            ("printLeptonInfo");
   Data                       =         iConfig.getParameter<bool>            ("Data");
   saveGenParticles           =         iConfig.getParameter<bool>            ("saveGenParticles");
   noMETCut                   =         iConfig.getParameter<bool>            ("noMETCut");
   invertEID                  =         iConfig.getParameter<bool>            ("invertEID");
   PFlowLoose                 =         iConfig.getParameter<bool>            ("PFlowLoose");
   elONLY                     =         iConfig.getParameter<bool>            ("elONLY");
   muONLY                     =         iConfig.getParameter<bool>            ("muONLY");
   OnVsOffShell               =         iConfig.getParameter<bool>            ("OnVsOffShell");
   StoreJets01                =         iConfig.getParameter<bool>            ("StoreJets01");

   MCpTrigger                 =         iConfig.getParameter<bool>            ("MCpTrigger");
   SQWaT_Version              =         iConfig.getParameter<int>             ("SQWaT_Version");
   doTrackerIso               =         iConfig.getParameter<bool>            ("doTrackerIso");
   doDetectorIso              =         iConfig.getParameter<bool>            ("doDetectorIso");
   doPFIso                    =         iConfig.getParameter<bool>            ("doPFIso");
   doMVAeleSel                =         iConfig.getParameter<bool>            ("doMVAeleSel");
   noMVAIsoCut                =         iConfig.getParameter<bool>            ("noMVAIsoCut");

   //-----Event Variable Inputs
   elcnt_Prim                 =         iConfig.getParameter<int>             ("elcnt_Prim");
   elcnt_Loose                =         iConfig.getParameter<int>             ("elcnt_Loose");
   mucnt_Prim                 =         iConfig.getParameter<int>             ("mucnt_Prim");
   mucnt_Loose                =         iConfig.getParameter<int>             ("mucnt_Loose");
   jcnt_tot                   =         iConfig.getParameter<int>             ("jcnt_tot");
   EvtTotCount                =         iConfig.getParameter<int>             ("EvtTotCount");
   mu_passAll                 =         iConfig.getParameter<bool>            ("mu_passAll");
   mu_passStandard            =         iConfig.getParameter<bool>            ("mu_passStandard");
   mu_passFlag                =         iConfig.getParameter<bool>            ("mu_passFlag");
   el_passAll                 =         iConfig.getParameter<bool>            ("el_passAll");
   el_passStandard            =         iConfig.getParameter<bool>            ("el_passStandard");
   el_passFlag                =         iConfig.getParameter<bool>            ("el_passFlag");

   //-----Trigger Variable Inputs
   mu_passTrigger             =         iConfig.getParameter<bool>            ("mu_passTrigger");
   el_passTrigger             =         iConfig.getParameter<bool>            ("el_passTrigger");

   //-----Vertex Variable Inputs
   PVfound                    =         iConfig.getParameter<bool>            ("PVfound");
   vtxcnt                     =         iConfig.getParameter<int>             ("vtxcnt");

   //-----Jet Variable Inputs
   j_ptMin                    =         iConfig.getParameter<double>          ("j_ptMin");
   j_aetaMax                  =         iConfig.getParameter<double>          ("j_aetaMax");
   j_DRelMin                  =         iConfig.getParameter<double>          ("j_DRelMin");
   muPrim_DRjMin              =         iConfig.getParameter<double>          ("muPrim_DRjMin");
   CHEFMin                    =         iConfig.getParameter<double>          ("CHEFMin");
   CEMEFMax                   =         iConfig.getParameter<double>          ("CEMEFMax");
   NHEFMax                    =         iConfig.getParameter<double>          ("NHEFMax");
   NEMEFMax                   =         iConfig.getParameter<double>          ("NEMEFMax");
   NDaughtersMin              =         iConfig.getParameter<unsigned int>    ("NDaughtersMin");
   CMultiplicityMin           =         iConfig.getParameter<int>             ("CMultiplicityMin");

   //-----B-Tag Variable Inputs
   nBtagSSV                   =         iConfig.getParameter<int>             ("nBtagSSV");
   nBtagTC                    =         iConfig.getParameter<int>             ("nBtagTC");
   bDiscriminatorSSVMin       =         iConfig.getParameter<double>          ("bDiscriminatorSSVMin");
   bDiscriminatorTCMin        =         iConfig.getParameter<double>          ("bDiscriminatorTCMin");
   bDiscriminatorCSVMin       =         iConfig.getParameter<double>          ("bDiscriminatorCSVMin");
   bDiscriminatorCSVMVAMin    =         iConfig.getParameter<double>          ("bDiscriminatorCSVMVAMin");

   //-----Muon Variable Inputs
   muPrim_ptMin               =         iConfig.getParameter<double>          ("muPrim_ptMin");
   muPrim_aetaMax             =         iConfig.getParameter<double>          ("muPrim_aetaMax");
   muPrim_dBMax               =         iConfig.getParameter<double>          ("muPrim_dBMax");
   muPrim_dzMax               =         iConfig.getParameter<double>          ("muPrim_dzMax");
   muPrim_TrkIsoMax           =         iConfig.getParameter<double>          ("muPrim_TrkIsoMax");
   muPrim_DetIsoMax           =         iConfig.getParameter<double>          ("muPrim_DetIsoMax");
   muPrim_PFIsoMax            =         iConfig.getParameter<double>          ("muPrim_PFIsoMax");
   muPrim_normChi2Max         =         iConfig.getParameter<double>          ("muPrim_normChi2Max");
   muPrim_nValidHitsMin       =         iConfig.getParameter<int>             ("muPrim_nValidHitsMin");
   muPrim_nValidMuonHitsMin   =         iConfig.getParameter<int>             ("muPrim_nValidMuonHitsMin");
   muPrim_nValidPixelHitsMin  =         iConfig.getParameter<int>             ("muPrim_nValidPixelHitsMin");
   muPrim_nMatchedStationsMin =         iConfig.getParameter<int>             ("muPrim_nMatchedStationsMin");
   isProperlyIsolatedMu       =         iConfig.getParameter<bool>            ("isProperlyIsolatedMu");
   muDetIsoConeSize           =         iConfig.getParameter<double>          ("muDetIsoConeSize");
   muPFIsoConeSize            =         iConfig.getParameter<double>          ("muPFIsoConeSize");
   muLoose_ptMin              =         iConfig.getParameter<double>          ("muLoose_ptMin");
   muLoose_aetaMax            =         iConfig.getParameter<double>          ("muLoose_aetaMax");
   muLoose_TrkIsoMax          =         iConfig.getParameter<double>          ("muLoose_TrkIsoMax");
   muLoose_DetIsoMax          =         iConfig.getParameter<double>          ("muLoose_DetIsoMax");
   muLoose_PFIsoMax           =         iConfig.getParameter<double>          ("muLoose_PFIsoMax");

   //-----Electron Variable Inputs
   elPrim_ptMin               =         iConfig.getParameter<double>          ("elPrim_ptMin");
   elPrim_aetaMax             =         iConfig.getParameter<double>          ("elPrim_aetaMax");
   elPrim_aetascExcludeMax    =         iConfig.getParameter<double>          ("elPrim_aetascExcludeMax");
   elPrim_aetascExcludeMin    =         iConfig.getParameter<double>          ("elPrim_aetascExcludeMin");
   elPrim_sigmaIetaIetaMaxEB  =         iConfig.getParameter<double>          ("elPrim_sigmaIetaIetaMaxEB");
   elPrim_aDeltaPhiMaxEB      =         iConfig.getParameter<double>          ("elPrim_aDeltaPhiMaxEB");
   elPrim_aDeltaEtaMaxEB      =         iConfig.getParameter<double>          ("elPrim_aDeltaEtaMaxEB");
   elPrim_HoEMaxEB            =         iConfig.getParameter<double>          ("elPrim_HoEMaxEB"); 
   elPrim_sigmaIetaIetaMaxEE  =         iConfig.getParameter<double>          ("elPrim_sigmaIetaIetaMaxEE");
   elPrim_aDeltaPhiMaxEE      =         iConfig.getParameter<double>          ("elPrim_aDeltaPhiMaxEE");
   elPrim_aDeltaEtaMaxEE      =         iConfig.getParameter<double>          ("elPrim_aDeltaEtaMaxEE");
   elPrim_HoEMaxEE            =         iConfig.getParameter<double>          ("elPrim_HoEMaxEE");
   elPrim_dBMax               =         iConfig.getParameter<double>          ("elPrim_dBMax");
   elPrim_dzMax               =         iConfig.getParameter<double>          ("elPrim_dzMax");
   elPrim_EinvMinusPinvMax    =         iConfig.getParameter<double>          ("elPrim_EinvMinusPinvMax");
   elPrim_TrkIsoMax           =         iConfig.getParameter<double>          ("elPrim_TrkIsoMax");
   elPrim_DetIsoMax           =         iConfig.getParameter<double>          ("elPrim_DetIsoMax");
   elPrim_PFIsoMax            =         iConfig.getParameter<double>          ("elPrim_PFIsoMax");
   elPrim_PFIsoMin_invertEID  =         iConfig.getParameter<double>          ("elPrim_PFIsoMin_invertEID");

   elLoose_ptMin              =         iConfig.getParameter<double>          ("elLoose_ptMin");
   elLoose_sigmaIetaIetaMaxEB =         iConfig.getParameter<double>          ("elLoose_sigmaIetaIetaMaxEB");
   elLoose_aDeltaPhiMaxEB     =         iConfig.getParameter<double>          ("elLoose_aDeltaPhiMaxEB");
   elLoose_aDeltaEtaMaxEB     =         iConfig.getParameter<double>          ("elLoose_aDeltaEtaMaxEB");
   elLoose_HoEMaxEB           =         iConfig.getParameter<double>          ("elLoose_HoEMaxEB");
   elLoose_sigmaIetaIetaMaxEE =         iConfig.getParameter<double>          ("elLoose_sigmaIetaIetaMaxEE");
   elLoose_aDeltaPhiMaxEE     =         iConfig.getParameter<double>          ("elLoose_aDeltaPhiMaxEE");
   elLoose_aDeltaEtaMaxEE     =         iConfig.getParameter<double>          ("elLoose_aDeltaEtaMaxEE");
   elLoose_HoEMaxEE           =         iConfig.getParameter<double>          ("elLoose_HoEMaxEE"); 
   elLoose_dBMax              =         iConfig.getParameter<double>          ("elLoose_dBMax");
   elLoose_dzMax              =         iConfig.getParameter<double>          ("elLoose_dzMax");
   elLoose_TrkIsoMax          =         iConfig.getParameter<double>          ("elLoose_TrkIsoMax");
   elLoose_DetIsoMax          =         iConfig.getParameter<double>          ("elLoose_DetIsoMax");
   elLoose_PFIsoMax           =         iConfig.getParameter<double>          ("elLoose_PFIsoMax");
   
   elDetIsoConeSize           =         iConfig.getParameter<double>          ("elDetIsoConeSize");
   elPFIsoConeSize            =         iConfig.getParameter<double>          ("elPFIsoConeSize");

   el_aetaPass                =         iConfig.getParameter<bool>            ("el_aetaPass");

   //-----Lepton Variable Inputs

   //-----MET Variable Inputs
   if (invertEID)
      MET_EtMin               =         iConfig.getParameter<double>          ("MET_EtMin_invertEID");
   else
      MET_EtMin               =         iConfig.getParameter<double>          ("MET_EtMin");

   //-----Additional Variable Inputs
   Mjj                        =         iConfig.getParameter<double>          ("Mjj");
   lQ                         =         iConfig.getParameter<int>             ("lQ");
   lEta                       =         iConfig.getParameter<double>          ("lEta");

   //-----Constant Inputs
   etaBarrelMax               =         iConfig.getParameter<double>          ("etaBarrelMax");

}


//______________________________________________________________________________
PerformSelection::~PerformSelection() {}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::beginJob() {
   edm::Service<TFileService> fs;
   if (!fs) throw edm::Exception(edm::errors::Configuration,
                                 "TFileService missing from configuration!");
   
   vector<string> cuts;
   for (unsigned int c = 0; c<DEFS::nCutLevel; c++) {
      cuts.push_back(DEFS::getCutLevelString((DEFS::CutLevel)c));
   }
   vector<string> jets;
   for (unsigned int j = 0; j<DEFS::NJETS; j++) {
      jets.push_back(DEFS::getJetBinString((DEFS::JetBin)j));
   }

   // create the tree
   tableEl = fs->make<Table>(string(outtablenameEl),cuts,jets,"TableCellVal");
   tableMu = fs->make<Table>(string(outtablenameMu),cuts,jets,"TableCellVal");
   tableLp = fs->make<Table>(string(outtablenameLp),cuts,jets,"TableCellVal");
   EvtNtuple = new EventNtuple();
   if (StoreJets01) {
      EvtTree_0Jets = fs->make<TTree>(getJetBinString(DEFS::jets0).c_str(), "Output tree");
      EvtTree_1Jets = fs->make<TTree>(getJetBinString(DEFS::jet1).c_str(), "Output tree");
      EvtTree_0Jets->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
      EvtTree_1Jets->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
   }
   EvtTree_2pJets = fs->make<TTree>((getJetBinString(DEFS::jets2)+"p").c_str(), "Output tree for matrix element");
   EvtTree_2pJets->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
   if (!Data)
      TPUDist = fs->make<TH1D>("TPUDist","TPUDist",600,0,60);
   PFIsoDist = fs->make<TH1D>("PFIsoDist","PFIsoDist",1000,0,2);
/*
   PhotonIsoVsEta = fs->make<TH2D>("PhotonIsoVsEta","PhotonIsoVsEta",1000,0,2,200,-5,5);
   NeutralHadronIsoVsEta = fs->make<TH2D>("NeutralHadronIsoVsEta","NeutralHadronIsoVsEta",1000,0,2,200,-5,5);
   ChargedHadronIsoVsEta = fs->make<TH2D>("ChargedHadronIsoVsEta","ChargedHadronIsoVsEta",1000,0,2,200,-5,5);
   EffectiveAreaVsEta = fs->make<TH2D>("EffectiveAreaVsEta","EffectiveAreaVsEta",1000,0,2,200,-5,5);
   rhoPrimeVsEta = fs->make<TH2D>("rhoPrimeVsEta","rhoPrimeVsEta",1000,0,2,200,-5,5);
   ptVsEta = fs->make<TH2D>("ptVsEta","ptVsEta",400,0,2000,200,-5,5);
*/
}


//______________________________________________________________________________
void PerformSelection::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
   //
   //find the runNumber and eventNumber here
   //
   eventNumber       = iEvent.id().event();
   runNumber         = iEvent.id().run();
   lumiNumber        = iEvent.id().luminosityBlock();
   bxNumber          = iEvent.eventAuxiliary().bunchCrossing();
   orbitNumber       = iEvent.eventAuxiliary().orbitNumber();
   storeNumber       = iEvent.eventAuxiliary().storeNumber();
   timeNumber        = iEvent.eventAuxiliary().time().value();
   lumiSegmentNumber = iEvent.eventAuxiliary().luminosityBlock();
      
   //if (eventNumber!=2535884 && eventNumber!=3093466 && eventNumber!=9993322 && eventNumber!=3145918 && eventNumber!=3146025 && eventNumber!=4938770 && eventNumber!=4464867)
   //   return;
   //if (eventNumber!=705349865)
   //   return;

   //
   // Create a set containing all the run and event numbers
   // so we can keep track of duplicate events.
   //
   RunEventSet runEventSet;
   //
   //Check and remove duplicates here
   //
   if (runEventSet.alreadySeen(runNumber, eventNumber)) {
      cout << "WARNING, event Run = " << runNumber << ", Event = " << eventNumber
           <<" is duplicated" << endl << "We will skip this event." << endl;
      return;
   }
      
   //
   // Load the objects of interest
   //
   getCollections(iEvent, iSetup);

   //
   // Make the distribution of the true pileup interactions
   //
   if (!Data)
      makeTPUDist();

   //
   //Perform The Selection
   //

   //
   // Trigger Selection
   //
   triggerSelection();

   //
   // Vertex Selection
   //
   vertexSelection();

   //
   // Electron Selection
   //
   eleClear();
   eleSelection();

   //
   // Muon Selection (No Jet Cleaning)
   //
   muonClear();
   muonSelection();

      
   //
   // Jet Selection (without muon cleaning)
   // Electron Cleaning (i.e. remove the jets which are likely to be electorns using the DR cut).
   // Using PF Jets
   //
   jetClear();
   jetSelection();
   // Make sure the jets are sorted by pt
   if (jp4.size()>1)
      ptSort(jp4);

   //
   // MET Selection
   //
   metSelection();

   //
   // Set any further quantities that require all of the physics objects to be selected first
   //
   // Compute the invariant mass, when only two jets are present
   computeInvariantMass();
   setDRlj();
   setThetalj1pj2();

   //
   // Record The Results
   //
      
   //
   // Record the number of events passing the cuts for each jet count
   //
   el_passStandard=false;
   el_passAll=false;
   mu_passStandard=false;
   mu_passAll=false;


   printEventInformation(printEventInfo, -1, true);
   for(unsigned int Nj=0; Nj<DEFS::NJETS; Nj++) {
      if((unsigned int)jcnt_tot==Nj || (Nj==(DEFS::NJETS-1) && (unsigned int)jcnt_tot>(DEFS::NJETS-1))) {
         incrementCounter(1,Nj,tableEl,tableMu,tableLp);
         printEventInformation(printEventInfo, 0, true);
         
         if (el_passTrigger) {
            incrementCounter(2,Nj,tableEl,tableLp);
            printEventInformation(printEventInfo, 1, true);

            if (PVfound) {
               incrementCounter(3,Nj,tableEl,tableLp);
               printEventInformation(printEventInfo, 2, true);
               //
               // Record the electron info
               //
               if (elcnt_Prim>=1) {
                  incrementCounter(4,Nj,tableEl,tableLp);
                  if (!muONLY) printEventInformation(printEventInfo, 3, false);
                  if (mucnt_Loose==0) {
                     incrementCounter(5,Nj,tableEl,tableLp);
                     if (!muONLY) printEventInformation(printEventInfo, 4, false);
                     if (elcnt_Loose==1) {
                        incrementCounter(6,Nj,tableEl,tableLp);
                        if (!muONLY) printEventInformation(printEventInfo, 5, false);
                        el_passStandard=true;
                        if (MET_Pass) {
                           incrementCounter(7,Nj,tableEl,tableLp);
                           if (!muONLY) printEventInformation(printEventInfo, 6, false);
                           el_passAll=true;
                           //
                           // Record the Btag info
                           //
                           if (nBtagSSV==0)
                              incrementCounter(18,Nj,tableEl,tableLp);
                           else {
                              if (nBtagSSV==1)
                                 incrementCounter(19,Nj,tableEl,tableLp);
                              else {
                                 if (nBtagSSV==2)
                                    incrementCounter(20,Nj,tableEl,tableLp);
                                 else
                                    incrementCounter(21,Nj,tableEl,tableLp);
                              }
                           }//BTags

                           /////////////FNAL CUTs
                           if (METp4[0].Et() > 30) {
                              incrementCounter(8,Nj,tableEl,tableLp);
                              TLorentzVector mt(lp4[0].Px()+METp4[0].Px(),lp4[0].Py()+METp4[0].Py(),0,lp4[0].Et()+METp4[0].Et());
                              if (mt.M() > 30) {
                                 incrementCounter(9,Nj,tableEl,tableLp);
                                 if (lp4[0].Pt() > 30) {
                                    incrementCounter(10,Nj,tableEl,tableLp);
                                    if (abs(lp4[0].Eta()) < 2.5) {
                                       incrementCounter(11,Nj,tableEl,tableLp);
                                       if (jp4.size()!=0 && jp4[0].Pt() > 30) {
                                          incrementCounter(12,Nj,tableEl,tableLp);
                                          if (jp4.size() > 1 && jp4[1].Pt() > 30) {
                                             incrementCounter(13,Nj,tableEl,tableLp);
                                             if (abs(jp4[0].Eta()) < 2.4) {
                                                incrementCounter(14,Nj,tableEl,tableLp);
                                                if (abs(jp4[1].Eta()) < 2.4) {
                                                   incrementCounter(15,Nj,tableEl,tableLp);
                                                   bool dr = true;
                                                   for (unsigned int i = 0; i<Nj; i++){
                                                      TLorentzVector j(jp4[i].Px(),jp4[i].Py(),jp4[i].Pz(),jp4[i].E());
                                                      if (j.DeltaR(lp4[0]) <= 0.3)
                                                         dr = false;
                                                      else
                                                         continue;
                                                   }
                                                   if (dr) {
                                                      incrementCounter(16,Nj,tableEl,tableLp);
                                                      TLorentzVector jj(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz(),jp4[0].E()+jp4[1].E());
                                                      TLorentzVector j4;
                                                      if (jp4.size()>3) {
                                                         j4.SetPx(jp4[3].Px());
                                                         j4.SetPy(jp4[3].Py());
                                                         j4.SetPz(jp4[3].Pz());
                                                         j4.SetE(jp4[3].E());
                                                      }
                                                      else {
                                                         j4.SetPx(0);
                                                         j4.SetPy(0);
                                                         j4.SetPz(0);
                                                         j4.SetE(0);
                                                      }
                                                      if (j4.Pt()<30 && jj.M()>65 && jj.M()<95) {
                                                         incrementCounter(17,Nj,tableEl,tableLp);
                                                      }
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                           }//First FNAL Cut


                        }
                     }
                  }
               }//electrons
            } //if (PVfound)
         } //if (el_passTrigger)

         if (mu_passTrigger) {
            incrementCounter(2,Nj,tableMu,tableLp);
            printEventInformation(printEventInfo, 1, true);
            if (PVfound) {
               //printEventInformation(true, -1, true);
               //printEventInformation(true, 7, true);
               incrementCounter(3,Nj,tableMu,tableLp);
               printEventInformation(printEventInfo, 2, true);
               //
               // Record the muon info
               //
               if (mucnt_Prim>=1) {
                  incrementCounter(4,Nj,tableMu,tableLp);
                  if (!elONLY) printEventInformation(printEventInfo, 3, true);
                  if (mucnt_Loose==1) {
                     incrementCounter(5,Nj,tableMu,tableLp);
                     if (!elONLY)printEventInformation(printEventInfo, 4, true);
                     if (elcnt_Loose==0) {
                        incrementCounter(6,Nj,tableMu,tableLp);
                        if (!elONLY) printEventInformation(printEventInfo, 5, true);
                        mu_passStandard=true;
                        if (MET_Pass) {
                           incrementCounter(7,Nj,tableMu,tableLp);
                           if (!elONLY) printEventInformation(printEventInfo, 6, true);
                           mu_passAll=true;
                           //
                           // Record the Btag info
                           //
                           if (nBtagSSV==0 )
                              incrementCounter(18,Nj,tableMu,tableLp);
                           else {
                              if (nBtagSSV==1) 
                                 incrementCounter(19,Nj,tableMu,tableLp);
                              else {
                                 if (nBtagSSV==2)
                                    incrementCounter(20,Nj,tableMu,tableLp);
                                 else
                                    incrementCounter(21,Nj,tableMu,tableLp);
                              }
                           }//BTags
                           
                           /////////////FNAL CUTs
                           if (METp4[0].Et() > 25) {
                              incrementCounter(8,Nj,tableMu,tableLp);
                              TLorentzVector mt(lp4[0].Px()+METp4[0].Px(),lp4[0].Py()+METp4[0].Py(),0,lp4[0].Et()+METp4[0].Et());
                              if (mt.M() > 30) {
                                 incrementCounter(9,Nj,tableMu,tableLp);
                                 if (lp4[0].Pt() > 25) {
                                    incrementCounter(10,Nj,tableMu,tableLp);
                                    if (abs(lp4[0].Eta()) < 2.1) {
                                       incrementCounter(11,Nj,tableMu,tableLp);
                                       if (jp4.size()!=0 && jp4[0].Pt() > 30) {
                                          incrementCounter(12,Nj,tableMu,tableLp);
                                          if (jp4.size() > 1 && jp4[1].Pt() > 30) {
                                             incrementCounter(13,Nj,tableMu,tableLp);
                                             if (abs(jp4[0].Eta()) < 2.4) {
                                                incrementCounter(14,Nj,tableMu,tableLp);
                                                if (abs(jp4[1].Eta()) < 2.4) {
                                                   incrementCounter(15,Nj,tableMu,tableLp);
                                                   bool dr = true;
                                                   for (unsigned int i = 0; i<Nj; i++){
                                                      TLorentzVector j(jp4[i].Px(),jp4[i].Py(),jp4[i].Pz(),jp4[i].E());
                                                      if (j.DeltaR(lp4[0]) <= 0.3)
                                                         dr = false;
                                                      else
                                                         continue;
                                                   }
                                                   if (dr) {
                                                      incrementCounter(16,Nj,tableMu,tableLp);
                                                      TLorentzVector jj(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz(),jp4[0].E()+jp4[1].E());
                                                      TLorentzVector j4;
                                                      if (jp4.size()>3) {
                                                         j4.SetPx(jp4[3].Px());
                                                         j4.SetPy(jp4[3].Py());
                                                         j4.SetPz(jp4[3].Pz());
                                                         j4.SetE(jp4[3].E());
                                                      }
                                                      else {
                                                         j4.SetPx(0);
                                                         j4.SetPy(0);
                                                         j4.SetPz(0);
                                                         j4.SetE(0);
                                                      }
                                                      if (j4.Pt()<30 && jj.M()>65 && jj.M()<95) {
                                                         incrementCounter(17,Nj,tableMu,tableLp);
                                                      }
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                           }//First FNAL Cut

                        }
                     }
                  }
               } //if(mucnt_Prim>=1 && PVfound)
            } //if (PVfound)
         } //if (mu_passTrigger||el_passTrigger)
      } //if(jcnt_tot==Nj || (Nj==(DEFS::NJETS-1) && jcnt_tot>(DEFS::NJETS-1))) 
   } //for(Int_t Nj=0; Nj<DEFS::NJETS;Nj++)
   printEventInformation(printEventInfo, 7, true);
   if (printJetInfo)
      printJetInformation();
   if (printLeptonInfo)
      printLeptonInformation();

   //
   // Fill the Ntuple to be used in Matrix Element computations
   //    and the other ntuples for future analysis
   //    (requiring 2 jets and 1 tight lepton passing all of the cuts)
   // Usual option: fill in the elements which pass all of the cuts, *except* the MET cut
   //
   setFlags();
   if (lp4.size()>0) {
      for(unsigned int i=0; i<lp4.size(); i++){
         PFIsoDist->Fill(lp4[i].lpfIso);
      }
   }
   if ((el_passFlag || mu_passFlag) || OnVsOffShell) {
      //
      // Event Level Information
      //
      EvtNtuple->run=runNumber;
      EvtNtuple->event=eventNumber;
      EvtNtuple->lumi=lumiNumber;
      EvtNtuple->rho=*rhoHandle;       

      if (!Data && saveGenParticles)
         saveGenPart();

      //
      // Trigger Infromation
      //
      EvtNtuple->triggerMap = triggerMap;

      //
      // Vertex Information
      //
      EvtNtuple->vLV.clear();
      EvtNtuple->vLV = vp4;

      //
      // Jet Information
      //
      EvtNtuple->jLV.clear();
      EvtNtuple->jLV = jp4;
      // Transfer Function Variables
      // Do the matching between the pat::Jets and the reco::genParticles
      pair<int, TLorentzVector> matchedPair;
      if (!Data) {
         for (unsigned int j = 0; j<jp4.size(); j++) {
            matchedPair = matchToGen(jp4[j].Eta(),jp4[j].Phi());
            EvtNtuple->jLV[j].matchedGenParticles = matchedPair.second;
            EvtNtuple->jLV[j].matchedpdgId = matchedPair.first;
            EvtNtuple->jLV[j].matchedDeltaR = reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), jp4[j].Eta(), jp4[j].Phi());
         }
      }
      EvtNtuple->Mjj=Mjj;

      //
      // Lepton Information
      //
      EvtNtuple->lLV.clear();
      EvtNtuple->lLV = lp4;
      if (abs(lEta)<etaBarrelMax) {
         //
         // barrel
         //
         EvtNtuple->lLV[0].ldetComp = 0;
      } 
      else {
         //
         // endcap
         //
         EvtNtuple->lLV[0].ldetComp = 1;
      }
      if (el_passAll)
         EvtNtuple->lLV[0].leptonCat_passAll=DEFS::electron;
      if (mu_passAll)
         EvtNtuple->lLV[0].leptonCat_passAll=DEFS::muon;
      /*
      if (el_passFlag)
         EvtNtuple->leptonCat=DEFS::electron;
      if (mu_passFlag)
         EvtNtuple->leptonCat=DEFS::muon;
      */

      //
      // MET Information
      //
      EvtNtuple->METLV.clear();
      EvtNtuple->METLV = METp4;

      //
      // Fill The Tree
      //
      int treeSwitch = jcnt_tot;
      if (treeSwitch > 2)
         treeSwitch = 2;
      if (OnVsOffShell)
         treeSwitch = 2;
      switch (treeSwitch) {
         case 0:
            if (StoreJets01)
               EvtTree_0Jets->Fill();
            break;
         case 1:
            if (StoreJets01)
               EvtTree_1Jets->Fill();
            break;
         case 2:
            EvtTree_2pJets->Fill();
            break;
         default:
            cout << "WARNING::Value of jcnt_tot does not correspond to a known tree." << endl
                 << "\tNo tree will be filled with event " << eventNumber << "." << endl;
      }

   } //if((el_passFlag || mu_passFlag)) //&& jcnt_tot==2)

   EvtTotCount++;

   return;
}


//______________________________________________________________________________
void PerformSelection::endJob()
{
   for (unsigned int Nj=0; Nj<DEFS::NJETS;Nj++) {
      (*(*tableLp)(DEFS::getCutLevelString((DEFS::CutLevel)0),DEFS::getJetBinString((DEFS::JetBin)Nj))) = Value(EvtTotCount,0.0);
      (*(*tableMu)(DEFS::getCutLevelString((DEFS::CutLevel)0),DEFS::getJetBinString((DEFS::JetBin)Nj))) = Value(EvtTotCount,0.0);
      (*(*tableEl)(DEFS::getCutLevelString((DEFS::CutLevel)0),DEFS::getJetBinString((DEFS::JetBin)Nj))) = Value(EvtTotCount,0.0);
   }
}


////////////////////////////////////////////////////////////////////////////////
// implement trigger functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::triggerSelection() {
   ///Do not apply trigger selection to MC
   mu_passTrigger = true;
   el_passTrigger = true;
   if (Data || MCpTrigger) {
      triggerMap.clear();
      mu_passTrigger = false;
      el_passTrigger = false;
      pat::TriggerEvent const * trig = &*triggerHandle;
      
      const pat::TriggerPathCollection* triggerPaths = trig->paths();
      for (unsigned int i=0; i<triggerPaths->size(); i++) {
         if ((*triggerPaths)[i].wasRun() && !(*triggerPaths)[i].wasError())
            triggerMap[(*triggerPaths)[i].name()] = (*triggerPaths)[i].wasAccept();
      }
      
      if (trig->wasRun() && trig->wasAccept()) {
         //
         // match muon trigger names to our wild card expression                                                         
         //
         for (unsigned int iTrig=0; iTrig<muTrigger.size(); iTrig++) {
            TRegexp  muTrigRegexp(muTrigger[iTrig]);
            bool matchMuTrigName = false;
            for(pat::TriggerPathCollection::const_iterator iPath = trig->paths()->begin(); iPath != trig->paths()->end(); ++iPath){
               TString thisTrigPath = iPath->name();
               matchMuTrigName =  thisTrigPath.Contains(muTrigRegexp);
               if(matchMuTrigName == true){
                  pat::TriggerPath const * muPath = trig->path(iPath->name());
                  if (muPath != 0 && muPath->wasAccept()) {
                     mu_passTrigger = true;
                  }
               }
            }
         }
               
         //
         // match electron trigger names to our wild card expression
         //
         for (unsigned int iTrig=0; iTrig<eleTrigger.size(); iTrig++) {
            TRegexp  eleTrigRegexp(eleTrigger[iTrig]);
            bool matchElTrigName = false;
            for(pat::TriggerPathCollection::const_iterator iPath = trig->paths()->begin(); iPath != trig->paths()->end(); ++iPath){
               TString thisTrigPath = iPath->name();
               matchElTrigName =  thisTrigPath.Contains(eleTrigRegexp);
               if(matchElTrigName == true){
                  pat::TriggerPath const * elePath = trig->path(iPath->name());
                  if (elePath != 0 && elePath->wasAccept()) {
                     el_passTrigger = true;
                  }
               }
            }
         }
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
// implement event functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::setFlags()
{
   el_passFlag=el_passAll;
   mu_passFlag=mu_passAll;
   if(noMETCut) {
      el_passFlag=el_passStandard;
      mu_passFlag=mu_passStandard;
   }
   if(elONLY)
      mu_passFlag=false;
   if(muONLY)
      el_passFlag=false;
}


////////////////////////////////////////////////////////////////////////////////
// implement vertex functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::vertexSelection()
{
   vp4.clear();
   PVfound = (vtxHandle->begin() != vtxHandle->end());
   if (PVfound) {
      pv = (*vtxHandle->begin());
      if (pv.ndof()>=4 && fabs(pv.z())<=24 && fabs(pv.position().Rho())<=2.0) {
         vtxcnt++;
         vp4.push_back(Vertex(pv.x(),pv.y(),pv.z(),0));
         vp4.back().vNDOF = pv.ndof();

         //
         // Pileup information
         // 
         const reco::VertexCollection::const_iterator vtxEnd = vtxHandle->end();
         for (reco::VertexCollection::const_iterator vtxIter = vtxHandle->begin(); vtxEnd != vtxIter; ++vtxIter) {
            if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24 && fabs(vtxIter->position().Rho())<=2.0)
               ++vp4.back().npv;
         }
         vp4.back().tnpus.clear();
         vp4.back().npus.clear();
         if (!Data) {
            for(unsigned int i=0; i<pileupHandle->size(); i++) {
               vp4.back().tnpus.push_back((*pileupHandle)[i].getTrueNumInteractions());
               vp4.back().npus.push_back((*pileupHandle)[i].getPU_NumInteractions());
            }
         }
      }
      else {
         PVfound=false; 
         vp4.push_back(Vertex(-999,-999,-999,-999));
      }
   }
   else {
      throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event."
                                           << std::endl;
   }
}


////////////////////////////////////////////////////////////////////////////////
// implement jet functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::jetClear() {
   jcnt_tot=0;
   j_DRlepton=999;
   Mjj=-1;
     
   jNEntries=0;
   jp4.clear();
   nBtagSSV=0;
   nBtagTC=0;
   jstreams.clear();
}

//______________________________________________________________________________
void PerformSelection::jetSelection() {
   //
   // Define the constants
   //
   double pi_=TMath::Pi();

   const vector< pat::Jet >::const_iterator pfJetEnd = pfJetHandle->end();
   for (vector<pat::Jet>::const_iterator jetIter = pfJetHandle->begin(); pfJetEnd != jetIter; ++jetIter) { 
      j_pt=jetIter->pt();
      j_eta=jetIter->eta();
      j_phi=jetIter->phi();

      //
      // loop through all of the leptons to check the DR with the current jet
      //
      bool jDR = true;
      for (unsigned int i=0; i<lp4.size(); i++) {
         adphi=abs(lp4[i].Phi()-j_phi);
         //
         // we're on a circle
         //
         if (adphi>pi_)
            adphi=2*pi_-adphi;
         
         j_DRlepton=pow((j_eta-lp4[i].Eta())*(j_eta-lp4[i].Eta())+adphi*adphi,0.5);
         
         if ((lp4[i].leptonCat==DEFS::electron && j_DRlepton<j_DRelMin) || (lp4[i].leptonCat==DEFS::muon && j_DRlepton<muPrim_DRjMin))
            jDR = false;
         else
            continue;
      }
      
      //
      // Apply The Cut
      //
      if (j_pt>j_ptMin && abs(j_eta)<j_aetaMax && jDR &&
          jetIter->neutralHadronEnergyFraction()<NHEFMax && jetIter->neutralEmEnergyFraction()<NEMEFMax && 
          jetIter->numberOfDaughters()>NDaughtersMin && jetIter->chargedHadronEnergyFraction()>CHEFMin && 
          jetIter->chargedMultiplicity()>CMultiplicityMin && jetIter->chargedEmEnergyFraction()<CEMEFMax) {
         jcnt_tot++;
         jNEntries++;
         //
         // store the parameters
         //

         //
         // Corrected (reco) jet
         //
         math::XYZTLorentzVector vvv = jetIter->p4();
         jp4.push_back(Jet(vvv.Px(),vvv.Py(),vvv.Pz(),vvv.E()));
         jp4.back().jChargedMultiplicity = jetIter->chargedMultiplicity();
         jp4.back().jNeutralMultiplicity = jetIter->neutralMultiplicity();
         jp4.back().jNeutralHadronEnergyFraction = jetIter->neutralHadronEnergyFraction();
         jp4.back().jNeutralEmEnergyFraction = jetIter->neutralEmEnergyFraction();
         jp4.back().jChargedHadronEnergyFraction = jetIter->chargedHadronEnergyFraction();
         jp4.back().jChargedEmEnergyFraction = jetIter->chargedEmEnergyFraction();
         jp4.back().jNumberOfDaughters = jetIter->numberOfDaughters();
         //PFConstituents not in SQWaT PATtuples created by V00-00-04 or earlier
         if (SQWaT_Version >= 5)
            jp4.back().jPtD = getPtD(jetIter->getPFConstituents());
         else
            jp4.back().jPtD = 0;

         //
         // Uncorrected (raw) jet
         //
         math::XYZTLorentzVector vv = jetIter->correctedJet("Uncorrected").p4();
         jp4.back().rawLV = TLorentzVector(vv.Px(),vv.Py(),vv.Pz(),vv.E());
         
         //
         // Gen (ref) jet
         //
         if (!Data) {
            if (jetIter->genJet()) {
               math::XYZTLorentzVector v = jetIter->genJet()->p4();
               jp4.back().refLV = TLorentzVector(v.Px(),v.Py(),v.Pz(),v.E());
               jp4.back().refpdgId = jetIter->genJet()->pdgId();
            }
            else {
               TLorentzVector lv(0,0,0,0);
               jp4.back().refLV = lv;
               jp4.back().refpdgId = 0;
            }
         }

         jstream << "pt " << j_pt << " eta " << j_eta << " j_DRlepton " << j_DRlepton
                 << " NHEF " << jetIter->neutralHadronEnergyFraction()
                 << " NEMEF " << jetIter->neutralEmEnergyFraction()
                 << " CHEF " << jetIter->chargedHadronEnergyFraction()
                 << " CEMEF " << jetIter->chargedEmEnergyFraction()
                 << " num daughters " << jetIter->numberOfDaughters()
                 << " charged multiplicity " << jetIter->chargedMultiplicity()
                 << endl;
         jstreams.push_back(jstream.str());
         jstream.str("");

         //
         // b-tagging
         //
         jp4.back().jBtagDiscriminatorSSV = jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags");
         jp4.back().jBtagDiscriminatorTC = jetIter->bDiscriminator("trackCountingHighEffBJetTags");
         jp4.back().jBtagDiscriminatorCSV = jetIter->bDiscriminator("combinedSVBJetTags");
         jp4.back().jBtagDiscriminatorCSVMVA = jetIter->bDiscriminator("combinedSVMVABJetTags");
         if (jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags")>bDiscriminatorSSVMin) {
            jp4.back().jBtagSSV = 1;
            nBtagSSV++;
         } 
         else
            jp4.back().jBtagSSV = 0;
         if (jetIter->bDiscriminator("trackCountingHighEffBJetTags")>bDiscriminatorTCMin) {
            jp4.back().jBtagTC = 1;
            nBtagTC++;
         } 
         else
            jp4.back().jBtagTC = 0;
         if (jetIter->bDiscriminator("combinedSecondaryVertexBJetTags")>bDiscriminatorCSVMin) {
            jp4.back().jBtagCSV = 1;
            nBtagCSV++;
         } 
         else
            jp4.back().jBtagCSV = 0;
         if (jetIter->bDiscriminator("combinedSVMVABJetTags")>bDiscriminatorCSVMVAMin) {
            jp4.back().jBtagCSVMVA = 1;
            nBtagCSVMVA++;
         } 
         else
            jp4.back().jBtagCSVMVA = 0;
      } //for(Selection Cuts)
   } //for(jetIter)
}

//______________________________________________________________________________
void PerformSelection::computeInvariantMass() {
   if (jcnt_tot==2)
      Mjj=(jp4[0]+jp4[1]).M();
   else
      Mjj=-2;
}

//______________________________________________________________________________
double PerformSelection::getPtD(vector<reco::PFCandidatePtr> pfCandidates) {
   double sumPt_cands=0.;
   double sumPt2_cands=0.;
   
   for (std::vector<reco::PFCandidatePtr>::const_iterator jt = pfCandidates.begin();
        jt != pfCandidates.end(); ++jt) {
      
      // Convert particle momentum to normal TLorentzVector, wrong type :(
      math::XYZTLorentzVectorD const& p4t = (*jt)->p4();
      TLorentzVector p4(p4t.px(), p4t.py(), p4t.pz(), p4t.energy());
      if(p4.Pt()!=0){
	     sumPt_cands += p4.Pt();
	     sumPt2_cands += (p4.Pt()*p4.Pt());
      }
   }

   if (sumPt_cands != 0)
      return sqrt( sumPt2_cands )/sumPt_cands;
   else
      return 0.0;
}


////////////////////////////////////////////////////////////////////////////////
// implement b-tag functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// implement muon functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::muonClear() {
   mucnt_Prim=0;
   mucnt_Loose=0;
}

//______________________________________________________________________________
void PerformSelection::muonSelection() {
   const vector< pat::Muon >::const_iterator muEnd = muonHandle->end();
   for(vector<pat::Muon>::const_iterator muIter = muonHandle->begin(); muEnd != muIter; ++muIter) { 
      mu_pt=muIter->pt();
      mu_eta=muIter->eta();
      mu_phi=muIter->phi();

      //
      // calculate the relative/tracker isolation of the muon
      //
      //Double_t rhoPrime = std::max((*rhoHandle),0.0);
      //Double_t AEff = MuonEffectiveArea::GetMuonEffectiveArea(MuonEffectiveArea::kMuGammaAndNeutralHadronIso04, mu_eta, MuonEffectiveArea::kMuEAData2011);
      mu_PFIso = (muIter->pfIsolationR04().sumChargedHadronPt + max(0.0, muIter->pfIsolationR04().sumNeutralHadronEt + muIter->pfIsolationR04().sumPhotonEt - 0.5*(muIter->pfIsolationR04().sumPUPt))) / mu_pt;
      mu_DetIso = (muIter->ecalIso() + muIter->hcalIso() + muIter->trackIso() -
                   (*rhoHandle * TMath::Pi() * pow(muDetIsoConeSize,2)));
      mu_DetIso /= mu_pt;
      mu_TrkIso=(muIter->trackIso())/mu_pt;
      
      muisGmTm=false;
      if (muIter->isGlobalMuon()/* && muIter->isTrackerMuon()*/)
         muisGmTm=true;

      muisPFm=false;
      if (muIter->isPFMuon())
         muisPFm=true;

      muisTightPrompt=false;
      if (muIter->globalTrack().isNonnull()) {
         if (muIter->globalTrack()->normalizedChi2()<muPrim_normChi2Max &&
            muIter->globalTrack()->hitPattern().numberOfValidMuonHits()>muPrim_nValidMuonHitsMin)
            muisTightPrompt=true;
      }

      //if (muIter->innerTrack().isNonnull())
         //muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
      //else
      //muPrim_itNHits=-1;
      if (muIter->innerTrack().isNonnull())
         muPrim_itNHits=muIter->globalTrack()->hitPattern().trackerLayersWithMeasurement();
      else
         muPrim_itNHits=-1;
      
       
      mu_vtxPass=false;
      if (abs(muIter->innerTrack()->dz(vtxHandle->at(0).position()))<muPrim_dzMax && muIter->dB()<muPrim_dBMax)
         mu_vtxPass=true;
       
      //
      // Apply The Primary Cuts
      //
      if (mu_pt>muPrim_ptMin && abs(mu_eta)<muPrim_aetaMax &&
          muisGmTm && muisPFm && muisTightPrompt && mu_vtxPass &&
          muPrim_itNHits>muPrim_nValidHitsMin &&
          muIter->innerTrack()->hitPattern().numberOfValidPixelHits()>muPrim_nValidPixelHitsMin &&
          muIter->numberOfMatchedStations()>muPrim_nMatchedStationsMin) {
         isProperlyIsolatedMu=false;
         
         if ( mu_PFIso<muPrim_PFIsoMax ) //((doDetectorIso && mu_DetIso<muPrim_DetIsoMax)||(doTrackerIso && mu_TrkIso<muPrim_TrkIsoMax)) )
            isProperlyIsolatedMu=true;
            
         //
         // Record the muon quantities:
         //
         if (isProperlyIsolatedMu) {
            mucnt_Prim++;
            math::XYZTLorentzVector lv = muIter->p4();
            lp4.push_back(Lepton(lv.Px(),lv.Py(),lv.Pz(),lv.E())); 
            lp4.back().lQ = muIter->charge();
            lp4.back().leptonCat = DEFS::muon;
            lp4.back().ltrkIso = mu_TrkIso;
            lp4.back().ldetIso = mu_DetIso;
            lp4.back().lpfIso = mu_PFIso;
            if (doDetectorIso)
               lp4.back().lTotIso = mu_DetIso;
            else if (doTrackerIso)
               lp4.back().lTotIso = mu_TrkIso;
            else
               lp4.back().lTotIso = mu_PFIso;
            lp4.back().mIsGlobal = muisGmTm;
            lp4.back().mIsPF = muisPFm;
            lp4.back().mIsTracker = muIter->isTrackerMuon();
            lp4.back().mNormalizedChi2 = muIter->globalTrack()->normalizedChi2();
            lp4.back().mNumberOfValidMuonHits = muIter->globalTrack()->hitPattern().numberOfValidMuonHits();
            lp4.back().mNumberValidHits = muPrim_itNHits;
            lp4.back().mNumberOfValidPixelHits = muIter->innerTrack()->hitPattern().numberOfValidPixelHits();
            lp4.back().mNumberOfMatchedStations = muIter->numberOfMatchedStations();
            lp4.back().ldz = abs(muIter->innerTrack()->dz(vtxHandle->at(0).position()));
            lp4.back().ldB = muIter->dB();

            lstream << "mu_tight pt " << mu_pt << " eta " << abs(mu_eta) << " mu_vtxPass " << mu_vtxPass << " dz " << abs(muIter->innerTrack()->dz(vtxHandle->at(0).position())) << " d0 " << abs(muIter->dB()) << " PFIso " << mu_PFIso << " global " << muIter->isGlobalMuon() << " pf " << muIter->isPFMuon() << " nMuonHits " << muIter->globalTrack()->hitPattern().numberOfValidMuonHits() << " nHits " << muIter->innerTrack()->numberOfValidHits() << " nPixelHits " << muIter->innerTrack()->hitPattern().numberOfValidPixelHits() << " Chi2 " << muIter->globalTrack()->normalizedChi2() << " nMatchedStations " << muIter->numberOfMatchedStations() << endl;
            lstreams.push_back(lstream.str());
            lstream.str("");
         }
      }

      //
      // Apply The Loose Cuts
      //
      if (mu_pt>muLoose_ptMin && abs(mu_eta)<muLoose_aetaMax &&
          (muIter->isGlobalMuon() || muIter->isTrackerMuon()) && muisPFm &&
          //((doDetectorIso && mu_DetIso<muLoose_DetIsoMax)||(doTrackerIso && mu_TrkIso<muLoose_TrkIsoMax))
          mu_PFIso<muLoose_PFIsoMax) {
         mucnt_Loose++;

         lstream << "mu_loose pt " << mu_pt << " eta " << abs(mu_eta) << " mu_vtxPass " << mu_vtxPass << " dz " << abs(muIter->innerTrack()->dz(vtxHandle->at(0).position())) << " d0 " << abs(muIter->dB()) << " PFIso " << mu_PFIso << " global " << muIter->isGlobalMuon() << " pf " << muIter->isPFMuon() << " nMuonHits " << muIter->globalTrack()->hitPattern().numberOfValidMuonHits() << " nHits " << muIter->innerTrack()->numberOfValidHits() << " nPixelHits " << muIter->innerTrack()->hitPattern().numberOfValidPixelHits() << " Chi2 " << muIter->globalTrack()->normalizedChi2() << " nMatchedStations " << muIter->numberOfMatchedStations() << endl;
         lstreams.push_back(lstream.str());
         lstream.str("");
      }
   }//for muons
}


////////////////////////////////////////////////////////////////////////////////
// implement electron functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::eleClear() {
   elcnt_Prim=0;
   elcnt_Loose=0;
   lstreams.clear();
   lp4.clear();
}

//______________________________________________________________________________
void PerformSelection::eleSelection() {
   const vector< pat::Electron >::const_iterator elEnd = electronHandle->end();
   for (vector< pat::Electron >::const_iterator elIter = electronHandle->begin(); elEnd != elIter; ++elIter) { 
      el_pt=elIter->p4().Pt();
      el_eta=elIter->eta();
      el_phi=elIter->phi();
      el_aetasc=abs(elIter->superCluster()->eta());

      el_aetaPass=false;
      if (abs(el_eta)<elPrim_aetaMax)
         el_aetaPass=true; //noscv
      if (el_aetasc<elPrim_aetascExcludeMax && el_aetasc>elPrim_aetascExcludeMin)
         el_aetaPass=false;

      //
      // Tight EID Cuts
      //
      el_CutEIDPass = CutBasedEID(elIter,true);
      el_EinvMinusPinvPass = eleEP(elIter);

      //
      // Tight Isolation Cuts
      //
      el_IsoPass = eleIso(elIter,true);

      //
      // Tight Vertex Cuts
      //
      el_vtxPass = eleVtx(elIter,true);

      //
      // Tight conversion Cuts
      //
      el_convPass = elIter->passConversionVeto();

      //
      // Tight MVA based EID
      //
      el_MVAEIDPass = MVABasedEID(elIter,true);
         
      //
      // Combine the Tight Cuts
      //
      if (doMVAeleSel)
         el_PassTight = (el_pt>elPrim_ptMin) && el_aetaPass && el_MVAEIDPass && el_vtxPass && el_convPass;
      else
         el_PassTight = (el_pt>elPrim_ptMin) && el_aetaPass && el_CutEIDPass && el_vtxPass && el_EinvMinusPinvPass && el_convPass && el_IsoPass;

      //
      // Apply The Primary Cuts
      //
      if (el_PassTight) {
         //
         // Record the electron quantities:
         //
         elcnt_Prim++;
         math::XYZTLorentzVector lv = elIter->p4();
         lp4.push_back(Lepton(lv.Px(),lv.Py(),lv.Pz(),lv.E()));
         lp4.back().lQ = elIter->charge();
         lp4.back().leptonCat = DEFS::electron;
         lp4.back().ltrkIso = el_TrkIso;
         lp4.back().ldetIso = el_DetIso;
         lp4.back().lpfIso = el_PFIso;
         if (doDetectorIso)
            lp4.back().lTotIso = el_DetIso;
         else if(doTrackerIso)
            lp4.back().lTotIso = el_TrkIso;
         else
            lp4.back().lTotIso = el_PFIso;
         lp4.back().lphotonIso = elIter->photonIso();
         lp4.back().lchargedHadronIso = elIter->chargedHadronIso();
         lp4.back().lneutralHadronIso = elIter->neutralHadronIso();
         lp4.back().lAEff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, elIter->eta(), ElectronEffectiveArea::kEleEAData2012);
         lp4.back().ldz = abs(elIter->gsfTrack()->dz(pv.position()));
         lp4.back().ldB = abs(elIter->dB());
         lp4.back().emvaTrig = el_mvaTrig;
         lp4.back().emvaNonTrig = el_mvaNonTrig;
         lp4.back().ePassConversionVeto = el_convPass;
         lp4.back().eIsEB = elIter->isEB();
         lp4.back().eIsEE = elIter->isEE();
         lp4.back().eSuperClusterEta = elIter->superCluster()->eta();
         lp4.back().eEcalEnergy = elIter->ecalEnergy();
         lp4.back().eESuperClusterOverP = elIter->eSuperClusterOverP();
         lp4.back().eSigmaIetaIeta = elIter->sigmaIetaIeta();
         lp4.back().eDeltaPhi = elIter->deltaPhiSuperClusterTrackAtVtx();
         lp4.back().eDeltaEta = elIter->deltaEtaSuperClusterTrackAtVtx();
         lp4.back().eHadronicOverEm = elIter->hadronicOverEm();

         lstream << "el_tight pt " << el_pt << " eta " << abs(el_eta) << " dz " << abs(elIter->gsfTrack()->dz(pv.position())) << " d0 " << abs(elIter->dB()) << " PFIso " << el_PFIso << " EB " << elIter->isEB() << " Passed conversion veto " << el_convPass << " sigmaIetaIeta " << elIter->sigmaIetaIeta() << " DeltaPhi " << abs(elIter->deltaPhiSuperClusterTrackAtVtx()) << " DeltaEta " << abs(elIter->deltaEtaSuperClusterTrackAtVtx()) << " HoE " << elIter->hadronicOverEm() << " EinvMinusPinv " << el_EinvMinusPinvPass << " nHits " << elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits() << endl; 
         lstreams.push_back(lstream.str());
         lstream.str("");
      }

      //
      // Loose EID Cuts
      //
      el_CutEIDPass = CutBasedEID(elIter,false);

      //
      // Loose Isolation Cuts
      //
      el_IsoPass = eleIso(elIter,false);

      //
      // Loose Vertex Cuts
      //
      el_vtxPass = eleVtx(elIter,false);

      //
      // Loose MVA based EID
      //
      el_MVAEIDPass = MVABasedEID(elIter,false);

      //
      // Combine the Loose Cuts
      //
      if (doMVAeleSel)
         el_PassLoose = (el_pt>elLoose_ptMin) && el_aetaPass && el_vtxPass && el_MVAEIDPass && el_convPass;
      else
         el_PassLoose = (el_pt>elLoose_ptMin) && el_aetaPass && el_vtxPass && el_CutEIDPass && el_IsoPass;

      //
      // Apply The Loose Cuts
      //
      if (el_PassLoose) {

         elcnt_Loose++;

         lstream << "el_loose pt " << el_pt << " eta " << abs(el_eta) << " dz " << abs(elIter->gsfTrack()->dz(pv.position())) << " d0 " << abs(elIter->dB()) << " PFIso " << el_PFIso << " EB " << elIter->isEB() << " Passed conversion veto " << el_convPass << " sigmaIetaIeta " << elIter->sigmaIetaIeta() << " DeltaPhi " << abs(elIter->deltaPhiSuperClusterTrackAtVtx()) << " DeltaEta " << abs(elIter->deltaEtaSuperClusterTrackAtVtx()) << " HoE " << elIter->hadronicOverEm() << " EinvMinusPinv " << el_EinvMinusPinvPass << " nHits " << elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits() << endl; 
         lstreams.push_back(lstream.str());
         lstream.str("");
      }
   }
}

//______________________________________________________________________________
bool PerformSelection::CutBasedEID(vector< pat::Electron >::const_iterator elIter, bool tight){
   el_sigmaIetaIeta=elIter->sigmaIetaIeta();
   el_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
   el_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
   el_HoE=elIter->hadronicOverEm();
   if (tight) {
      if (invertEID) {
         if (elIter->isEB()) {
            if ( !(el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB) || 
                 !(el_aDeltaPhi<elPrim_aDeltaPhiMaxEB) || 
                 !(el_aDeltaEta<elPrim_aDeltaEtaMaxEB) || 
                 !(el_HoE<elPrim_HoEMaxEB) )
               return true;
         }
         if (elIter->isEE()) {
            if ( !(el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE) || 
                 !(el_aDeltaPhi<elPrim_aDeltaPhiMaxEE) || 
                 !(el_aDeltaEta<elPrim_aDeltaEtaMaxEE) || 
                 !(el_HoE<elPrim_HoEMaxEE) )
               return true;
         }
      }
      else {
         if (elIter->isEB()) {
            if ( (el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB) && 
                 (el_aDeltaPhi<elPrim_aDeltaPhiMaxEB) && 
                 (el_aDeltaEta<elPrim_aDeltaEtaMaxEB) && 
                 (el_HoE<elPrim_HoEMaxEB) )
               return true;
         }
         if (elIter->isEE()) {
            if ( (el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE) && 
                 (el_aDeltaPhi<elPrim_aDeltaPhiMaxEE) && 
                 (el_aDeltaEta<elPrim_aDeltaEtaMaxEE) && 
                 (el_HoE<elPrim_HoEMaxEE) )
               return true;
         }
      }
   }
   else {
      if (elIter->isEB()) {
         if ( (el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEB) && 
              (el_aDeltaPhi<elLoose_aDeltaPhiMaxEB) && 
              (el_aDeltaEta<elLoose_aDeltaEtaMaxEB) && 
              (el_HoE<elLoose_HoEMaxEB) )
            return true;
      }
      if (elIter->isEE()) {
         if ( (el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEE) && 
              (el_aDeltaPhi<elLoose_aDeltaPhiMaxEE) && 
              (el_aDeltaEta<elLoose_aDeltaEtaMaxEE) )
            return true;
      }
   }
   return false;
}

//______________________________________________________________________________
bool PerformSelection::MVABasedEID(vector< pat::Electron >::const_iterator elIter, bool tight){
   el_mvaTrig = elIter->electronID("mvaTrigV0");
   el_mvaNonTrig = elIter->electronID("mvaNonTrigV0");
   if(noMVAIsoCut) {
      return true;
   }
   if (tight) {
      if (invertEID) {
         if (el_PFIso>elPrim_PFIsoMin_invertEID && elIter->electronID("mvaTrigV0") > -1.0)
            return true;
      }
      else {
         if ( (el_aetasc < 0.8 && elIter->electronID("mvaTrigV0") > 0.977 && el_PFIso < 0.093)
              || (el_aetasc > 0.8 && el_aetasc < 1.479 && elIter->electronID("mvaTrigV0") > 0.956 && el_PFIso < 0.095)
              || (el_aetasc > 1.479 && el_aetasc < 2.5 && elIter->electronID("mvaTrigV0") > 0.966 && el_PFIso < 0.171) )
            return true;
      }
   }
   else {
      if (invertEID) {
         if (el_PFIso>elPrim_PFIsoMin_invertEID && elIter->electronID("mvaNonTrigV0") > -1.0)
            return true;
      }
      else {
         if ( (el_aetasc < 0.8 && elIter->electronID("mvaNonTrigV0") > 0.877 && el_PFIso < 0.426)
             || (el_aetasc > 0.8 && el_aetasc < 1.479 && elIter->electronID("mvaNonTrigV0") > 0.811 && el_PFIso < 0.481)
              || (el_aetasc > 1.479 && el_aetasc < 2.5 && elIter->electronID("mvaNonTrigV0") > 0.707 && el_PFIso < 0.390) )
            return true;
      }
   }
   return false;
}

//______________________________________________________________________________
bool PerformSelection::eleIso(vector< pat::Electron >::const_iterator elIter, bool tight) {
   //
   // calculate the pf/detector/tracker isolation of the electron
   //
   //PF Isolation
   Double_t rhoPrime = std::max((*rhoHandle),0.0);
   Double_t AEff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, elIter->eta(), ElectronEffectiveArea::kEleEAData2012);
   //if (invertEID)
   //   el_PFIso = (elIter->pfIsolationVariables().chargedHadronIso+ max(0.,elIter->pfIsolationVariables().neutralHadronIso+elIter->pfIsolationVariables().photonIso - AEff*rhoPrime))/elIter->p4().Pt();
   //else
   el_PFIso = (elIter->chargedHadronIso() + max( 0.0, elIter->neutralHadronIso() + elIter->photonIso() - AEff*rhoPrime )) / elIter->pt();
   //Detector Isolation
   if (elIter->isEB()) {
      el_DetIso = ( elIter->dr03TkSumPt() + std::max(0.,elIter->dr03EcalRecHitSumEt() -1.) 
                    + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*TMath::Pi()*pow(elDetIsoConeSize,2)) / elIter->p4().Pt();
   }
   else {
      el_DetIso = ( elIter->dr03TkSumPt() + elIter->dr03EcalRecHitSumEt()
                    + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*TMath::Pi()*pow(elDetIsoConeSize,2)) / elIter->p4().Pt();
   }
   //Tracker Isolation
   el_TrkIso=(elIter->dr03TkSumPt())/elIter->p4().Pt();


   if (tight) {
      if (invertEID) {
         if (el_PFIso>elPrim_PFIsoMin_invertEID)
            return true;
      }
      else {
         if ( (doDetectorIso && el_DetIso<elPrim_DetIsoMax)||(doTrackerIso && el_TrkIso<elPrim_TrkIsoMax)||(doPFIso && el_PFIso<elPrim_PFIsoMax) )
            return true;
      }
   }
   else {
      if ( (doDetectorIso && el_DetIso<elLoose_DetIsoMax)||(doTrackerIso && el_TrkIso<elLoose_TrkIsoMax)||(doPFIso && el_PFIso<elLoose_PFIsoMax) )
         return true;
      }
   return false;
}

//______________________________________________________________________________
bool PerformSelection::eleVtx(vector< pat::Electron >::const_iterator elIter, bool tight) {
   if (tight) {
      if (abs(elIter->gsfTrack()->dz(pv.position()))<elPrim_dzMax && abs(elIter->dB())<elPrim_dBMax)
         return true;
   }
   else {
      if (abs(elIter->gsfTrack()->dz(pv.position()))<elLoose_dzMax && abs(elIter->dB())<elLoose_dBMax)
         return true;
   }
   return false;
}

//______________________________________________________________________________
bool PerformSelection::eleEP(vector< pat::Electron >::const_iterator elIter) {
   if (abs((1/elIter->ecalEnergy())-(1/(elIter->ecalEnergy()/elIter->eSuperClusterOverP())))<elPrim_EinvMinusPinvMax)
      return true;
   else
      return false;
}


////////////////////////////////////////////////////////////////////////////////
// implement lepton functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// implement MET functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::metSelection() {
   MET_Pass = false;
   METp4.clear();

   math::XYZTLorentzVector v = METHandle->front().p4();
   METp4.push_back(TLorentzVector(v.Px(),v.Py(),v.Pz(),v.E()));

   MET_Pass = METp4[0].Et() > MET_EtMin;
}


////////////////////////////////////////////////////////////////////////////////
// implement additional functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void PerformSelection::getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
   iEvent.getByLabel(triggerSource, triggerHandle);
   assert ( triggerHandle.isValid() );

   iEvent.getByLabel(vtxSource, vtxHandle);
   assert ( vtxHandle.isValid() ); 

   if ( !Data ) {
      iEvent.getByLabel(genParticleSource, genParticleHandle);
      assert ( genParticleHandle.isValid() );

      iEvent.getByLabel(pileupSource, pileupHandle);
      assert ( pileupHandle.isValid() );
   }

   iEvent.getByLabel(pfJetSource, pfJetHandle);
   assert ( pfJetHandle.isValid() );

   iEvent.getByLabel(electronSource, electronHandle);
   assert ( electronHandle.isValid() );
 
   iEvent.getByLabel(muonSource, muonHandle);
   assert ( muonHandle.isValid() );

   iEvent.getByLabel(METSource, METHandle);
   assert ( METHandle.isValid() );

   iEvent.getByLabel(rhoSource, rhoHandle);
   assert ( rhoHandle.isValid() );
}

//______________________________________________________________________________
void PerformSelection::makeTPUDist() {
   TPUDist->Fill((*pileupHandle)[1].getTrueNumInteractions());
}

//______________________________________________________________________________
void PerformSelection::setDRlj() {
   for (int i=0; i<jcnt_tot; i++)
      if (lp4.size()>0)
         jp4[i].DRlj = lp4[0].DeltaR(jp4[i]);
}

//______________________________________________________________________________
void PerformSelection::setThetalj1pj2() {
   if (jp4.size()>=2 && lp4.size()>0) {
      lp3.SetXYZ(lp4[0].Px(),lp4[0].Py(),lp4[0].Pz());
      jjp3.SetXYZ(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz());
      lp4[0].Thetalj1pj2 = lp3.Dot(jjp3)/(lp3.Mag()*jjp3.Mag());
   }
   else if (lp4.size()>0)
      lp4[0].Thetalj1pj2 = 0;
   else
      return;
}

//______________________________________________________________________________
void PerformSelection::saveGenPart() {
   EvtNtuple->genParticleCollection.clear();
   map<size_t,pair<int,int> > memMap;
   int counter = 0;
   for (reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle, ++counter) {
      memMap[size_t((const reco::Candidate*)(& *genParticle))] = make_pair(counter, 9999);
      if (genParticle->status() == 3) {
         GenParticle g;
         g.charge = genParticle->charge();
         math::XYZTLorentzVectorD const& p4t = genParticle->p4();
         TLorentzVector p4(p4t.px(), p4t.py(), p4t.pz(), p4t.energy());
         g.p4 = p4;
         g.vtx.SetXYZ(genParticle->vertex().X(),genParticle->vertex().Y(),genParticle->vertex().Z());
         g.pdgId = genParticle->pdgId();
         g.status = genParticle->status();
         g.particlePosition = size_t((const reco::Candidate*)(& *genParticle));
         g.numberOfMothers = genParticle->numberOfMothers();
         g.numberOfDaughters = genParticle->numberOfDaughters();
         for (unsigned int i = 0; i<genParticle->numberOfMothers(); i++) {
            g.motherPositions.push_back(size_t(genParticle->mother(i)));
         }
         for (unsigned int i = 0; i<genParticle->numberOfDaughters(); i++) {
            g.daughterPositions.push_back(size_t(genParticle->daughter(i)));
         }
         EvtNtuple->genParticleCollection.push_back(g);
         memMap[size_t((const reco::Candidate*)(& *genParticle))].second = EvtNtuple->genParticleCollection.size() - 1;
         //cout << "Particle Position = " << memMap[size_t((const reco::Candidate*)(& *genParticle))].second << endl;
         //cout << "Particle pointer = " << size_t((const reco::Candidate*)(& *genParticle)) << end;
         //cout << "Particle key = " << EvtNtuple->genParticleCollection.particlePosition << endl;
         //cout << "EvtNtuple->genParticleCollection.size() = " << EvtNtuple->genParticleCollection.size() << endl;
/*
          for (reco::GenParticleCollection::const_iterator genParticle2 = genParticleHandle->begin();genParticle2 != genParticleHandle->end();++genParticle2) {
             if (genParticle->mother(0)==(const reco::Candidate*)(& *genParticle2))
             //if (matchGenParticles(genParticle->mother(0),(const reco::Candidate*)(& *genParticle2)))
                cout << "Found mother match at: " << genParticle->mother(0) << endl;
          }
*/
      }
   }
   for(unsigned int i=0; i< EvtNtuple->genParticleCollection.size(); i++) {
      //cout << "Particle Position (" << EvtNtuple->genParticleCollection[i].particlePosition << "," << memMap[EvtNtuple->genParticleCollection[i].particlePosition].second << ")" << endl;
      EvtNtuple->genParticleCollection[i].particlePosition = memMap[EvtNtuple->genParticleCollection[i].particlePosition].second;
      for(unsigned int j=0; j<EvtNtuple->genParticleCollection[i].motherPositions.size(); j++) {
         //cout << "Expected Mother Position  = " << memMap[EvtNtuple->genParticleCollection[i].motherPositions[j]].second << endl;
         EvtNtuple->genParticleCollection[i].motherPositions[j] = memMap[EvtNtuple->genParticleCollection[i].motherPositions[j]].second; 
         //cout << "Actual Mother Position  = " << EvtNtuple->genParticleCollection[i].motherPositions[j] << endl;
      }
      for(unsigned int j=0; j<EvtNtuple->genParticleCollection[i].daughterPositions.size(); j++) {
         //cout << "Expected Daughter Position  = " << memMap[EvtNtuple->genParticleCollection[i].daughterPositions[j]].second << endl;
         EvtNtuple->genParticleCollection[i].daughterPositions[j] = memMap[EvtNtuple->genParticleCollection[i].daughterPositions[j]].second;
         //cout << "Actual Daughter Position  = " << EvtNtuple->genParticleCollection[i].daughterPositions[j] << endl;
      }
   }
}

//______________________________________________________________________________
bool PerformSelection::matchGenParticles(const reco::Candidate* p1, const reco::Candidate* p2) {
   if (p1->status()==p2->status() && p1->pdgId()==p2->pdgId() && p1->charge()==p2->charge()) {
      math::XYZTLorentzVectorD const& tl1p4 = p1->p4();
      math::XYZTLorentzVectorD const& tl2p4 = p2->p4();
      if (tl1p4.px()==tl2p4.px() && tl1p4.py()==tl2p4.py() && tl1p4.pz()==tl2p4.pz() && tl1p4.energy()==tl2p4.energy())
         return true;
   }
   return false;
}

//______________________________________________________________________________
pair<int, TLorentzVector> PerformSelection::matchToGen(double eta, double phi) {
   double bestDeltaR = 9999;
   int bestPDGID = 0;
   TLorentzVector theGenObject(0,0,0,0);
   TLorentzVector theBestGenObject(0,0,0,0);
   for (reco::GenParticleCollection::const_iterator genParticle = genParticleHandle->begin();genParticle != genParticleHandle->end();++genParticle) {
      if (genParticle->pt()==0)
         continue;
      if (genParticle->status() == 3) {
         theGenObject.SetPxPyPzE(genParticle->px(),genParticle->py(),genParticle->pz(),genParticle->energy());
         if (reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi) < bestDeltaR) {
            bestDeltaR = reco::deltaR(theGenObject.Eta(), theGenObject.Phi(), eta, phi);
            theBestGenObject = theGenObject;
            bestPDGID = genParticle->pdgId();
         }
      }
   }
   pair<int, TLorentzVector> GenMatchedInformation(bestPDGID,theBestGenObject);
   return GenMatchedInformation;
}

//______________________________________________________________________________
void PerformSelection::ptSort(vector<Jet> & vec) {
   for (unsigned int next = 0; next < vec.size()-1; next++) {
      unsigned int max_pos = (unsigned int)max_position(vec, next, vec.size()-1);
      if (max_pos != next)
         swap(vec[max_pos],vec[next]);
   }
}

//______________________________________________________________________________
void PerformSelection::swap(TLorentzVector & x, TLorentzVector & y) {
   TLorentzVector temp = x;
   x = y;
   y = temp;
}

//______________________________________________________________________________
int PerformSelection::max_position(vector<Jet> & vec, int from, int to) {
   int max_pos = from;
   int i;
   for (i = from + 1; i <= to; i++)
      if (vec[i].Pt() > vec[max_pos].Pt()) max_pos = i;
   return max_pos;
}

//______________________________________________________________________________
void PerformSelection::incrementCounter(int nCut, unsigned int nJets, Table* t1, Table* t2, Table* t3) {
   (*(*t1)(DEFS::getCutLevelString((DEFS::CutLevel)nCut),DEFS::getJetBinString((DEFS::JetBin)nJets)))++;
   (*(*t2)(DEFS::getCutLevelString((DEFS::CutLevel)nCut),DEFS::getJetBinString((DEFS::JetBin)nJets)))++;   
   if (t3 != 0)
      (*(*t3)(DEFS::getCutLevelString((DEFS::CutLevel)nCut),DEFS::getJetBinString((DEFS::JetBin)nJets)))++;   
}

//______________________________________________________________________________
void PerformSelection::printEventInformation(bool print, int cLevel, bool muon) {
   if (!print) return;
   if (cLevel == -1)
      cout << "Event " << eventNumber << " NJets " << jcnt_tot << " Primary Muons " << mucnt_Prim
           << " Primary Electrons " << elcnt_Prim << " Loose Muons " << mucnt_Loose-mucnt_Prim << " Loose Electrons " 
           << elcnt_Loose-elcnt_Prim;
   else if (cLevel == 0)
      cout << " c0";
   else if (cLevel == 1)
      cout << " c1";
   else if (cLevel == 2)
      cout << " c2";
   else if (cLevel == 3) {
      if (muon)
         cout << " c3";  
      else if (!muon)
         cout << " c3";  
      else
         cout << " WARNING::Unknown primary lepton type!";
   }
   else if (cLevel == 4)
      cout << " c4";
   else if (cLevel == 5)
      cout << " c5";
   else if (cLevel == 6)
      cout << " c6";
   else if (cLevel == 7)
      cout << endl;
   else
      cout << "\tWARNING::Unknown cut level!" << endl;
}

//______________________________________________________________________________
void PerformSelection::printJetInformation() {
   for(unsigned int i=0; i<jstreams.size(); i++)
      cout << "Jet " << i << " " << jstreams[i];
}

//______________________________________________________________________________
void PerformSelection::printLeptonInformation() {
   for(unsigned int i=0; i<lstreams.size(); i++)
      cout << lstreams[i];
}


////////////////////////////////////////////////////////////////////////////////
// define PerformSelection as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(PerformSelection);
