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

#include "TAMUWW/Selection/interface/PerformSelectionCrab525.h"

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
   //bsSource                   =         iConfig.getParameter<edm::InputTag> ("bsSource");;
   //convSource                 =         iConfig.getParameter<edm::InputTag> ("convSource");;
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
   runtype                    = TString(iConfig.getParameter<string>          ("runtype"));

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

   MCpTrigger                 =         iConfig.getParameter<bool>            ("MCpTrigger");
   SQWaT_Version              =         iConfig.getParameter<int>             ("SQWaT_Version");
   doTrackerIso               =         iConfig.getParameter<bool>            ("doTrackerIso");
   doDetectorIso              =         iConfig.getParameter<bool>            ("doDetectorIso");
   doPFIso                    =         iConfig.getParameter<bool>            ("doPFIso");
   doMVAeleSel                =         iConfig.getParameter<bool>            ("doMVAeleSel");

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
   zvtx                       =         iConfig.getParameter<double>          ("zvtx");
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
   lTotIso                    =         iConfig.getParameter<double>          ("lTotIso");
   lecalIso                   =         iConfig.getParameter<double>          ("lecalIso");
   lhcalIso                   =         iConfig.getParameter<double>          ("lhcalIso");
   ltrkIso                    =         iConfig.getParameter<double>          ("ltrkIso");
   ldetIso                    =         iConfig.getParameter<double>          ("ldetIso");
   lpfIso                     =         iConfig.getParameter<double>          ("lpfIso");
   Mjj                        =         iConfig.getParameter<double>          ("Mjj");
   lQ                         =         iConfig.getParameter<int>             ("lQ");
   lEta                       =         iConfig.getParameter<double>          ("lEta");

   //-----Constant Inputs
   etaBarrelMax               =         iConfig.getParameter<double>          ("etaBarrelMax");

   InitializeIntMatrix(PassEl);
   InitializeIntMatrix(PassMu);
   InitializeIntMatrix(PassLp);

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

   // create the tree
   tableEl = fs->make<Table>(string(outtablenameEl));
   tableMu = fs->make<Table>(string(outtablenameMu));
   tableLp = fs->make<Table>(string(outtablenameLp));
   EvtTree = fs->make<TTree>("EvtTree", "Output tree for matrix element");
   EvtNtuple = new EventNtuple();
   EvtTree->Branch("EvtNtuple", "EventNtuple", &EvtNtuple);
   if (!Data)
      TPUDist = fs->make<TH1D>("TPUDist","TPUDist",600,0,60);
}


//______________________________________________________________________________
void PerformSelection::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
   //
   //find the runNumber and eventNumber here
   //
   eventNumber       = iEvent.id().event();
   runNumber         = iEvent.id().run();
   bxNumber          = iEvent.eventAuxiliary().bunchCrossing();
   orbitNumber       = iEvent.eventAuxiliary().orbitNumber();
   storeNumber       = iEvent.eventAuxiliary().storeNumber();
   timeNumber        = iEvent.eventAuxiliary().time().value();
   lumiSegmentNumber = iEvent.eventAuxiliary().luminosityBlock();
      
   //if (eventNumber!=2535884 && eventNumber!=3093466 && eventNumber!=9993322 && eventNumber!=3145918 && eventNumber!=3146025 && eventNumber!=4938770 && eventNumber!=4464867)
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
   /*
   //
   // JES Uncertainty : tested & works
   //
   double uncer;
   j_pt=25.5;
   j_eta=1.3;
   cout << "JES UNCERTAINTY" << endl;
   string JEC_PATH("CondFormats/JetMETObjects/data/");
   //edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5Calo.txt");
   edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5PF.txt");
   // edm::FileInPath fip(JEC_PATH+"Spring10_Uncertainty_AK5JPT.txt");
   JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(fip.fullPath());
   jecUnc->setJetEta(j_eta); // Give rapidity of jet you want uncertainty on
   jecUnc->setJetPt(j_pt);// Also give the corrected pt of the jet you want the uncertainty on
   //
   // The following function gives the relative uncertainty in the jet Pt.
   // i.e. ptCorSmeared = (1 +- uncer) * ptCor  
   //
   uncer = jecUnc->getUncertainty(true); // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.
   cout << "pt=" << j_pt << " eta=" << j_eta << " : uncer=" << uncer << endl;
   */
   jetSelection();
     
   //
   // Compute the invariant mass, when only two jets are present
   //
   computeInvariantMass();

   //
   // MET Selection
   //
   metSelection();
     
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
   for(Int_t Nj=0; Nj<NJETS;Nj++) {
      if(jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) {
         PassLp[1][Nj]++;
         PassMu[1][Nj]++;
         PassEl[1][Nj]++;
         printEventInformation(printEventInfo, 0, true);
         
         if (el_passTrigger) {
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,el_passTrigger);
            //PassLp[2][Nj]++;
            //PassMu[2][Nj]++;
            //PassEl[2][Nj]++;
            printEventInformation(printEventInfo, 1, true);

            if (PVfound) {
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,el_passTrigger);
               //PassLp[3][Nj]++;
               //PassMu[3][Nj]++;
               //PassEl[3][Nj]++;
               printEventInformation(printEventInfo, 2, true);
               //
               // Record the electron info
               //
               if (elcnt_Prim>=1) {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                  if (!muONLY) printEventInformation(printEventInfo, 3, false);
                  if (mucnt_Loose==0) {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     if (!muONLY) printEventInformation(printEventInfo, 4, false);
                     if (elcnt_Loose==1) {
                        incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                        if (!muONLY) printEventInformation(printEventInfo, 5, false);
                        el_passStandard=true;
                        if (MET_Pass) {
                           incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                           if (!muONLY) printEventInformation(printEventInfo, 6, false);
                           el_passAll=true;
                           //
                           // Record the Btag info
                           //
                           if (nBtagSSV==0)
                              incrementCounter(18,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                           else {
                              if (nBtagSSV==1)
                                 incrementCounter(19,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                              else {
                                 if (nBtagSSV==2)
                                    incrementCounter(20,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                 else
                                    incrementCounter(21,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                              }
                           }//BTags

                           /////////////FNAL CUTs
                           if (MET_Et > 30) {
                              incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                              TLorentzVector ll(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
                              TLorentzVector met(METp4.Px(),METp4.Py(),METp4.Py(),METp4.E());
                              TLorentzVector mt(lp4.Px()+METp4.Px(),lp4.Py()+METp4.Py(),0,lp4.Et()+METp4.Et());
                              //TLorentzVector mt(lp4.Px()+METp4.Px(),lp4.Py()+METp4.Py(),0,lp4.Pt()+METp4.Pt());
                              if (mt.M() > 30) {
                                 incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                 if (lp4.Pt() > 30) {
                                    incrementCounter(10,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                    if (abs(lp4.Eta()) < 2.5) {
                                       incrementCounter(11,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                       if (jp4.size()>1)
                                          ptSort(jp4);
                                       if (jp4.size()!=0 && jp4[0].Pt() > 30) {
                                          incrementCounter(12,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                          if (jp4.size() > 1 && jp4[1].Pt() > 30) {
                                             incrementCounter(13,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                             if (abs(jp4[0].Eta()) < 2.4) {
                                                incrementCounter(14,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                                if (abs(jp4[1].Eta()) < 2.4) {
                                                   incrementCounter(15,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                                                   bool dr = true;
                                                   for (int i = 0; i<Nj; i++){
                                                      TLorentzVector j(jp4[i].Px(),jp4[i].Py(),jp4[i].Pz(),jp4[i].E());
                                                      if (j.DeltaR(ll) <= 0.5)
                                                         dr = false;
                                                      else
                                                         continue;
                                                   }
                                                   if (dr) {
                                                      incrementCounter(16,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
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
                                                         incrementCounter(17,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
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
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,mu_passTrigger,0);
            printEventInformation(printEventInfo, 1, true);
            if (PVfound) {
               //printEventInformation(true, -1, true);
               //printEventInformation(true, 7, true);
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,mu_passTrigger,0);
               printEventInformation(printEventInfo, 2, true);
               //
               // Record the muon info
               //
               if (mucnt_Prim>=1) {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                  if (!elONLY) printEventInformation(printEventInfo, 3, true);
                  if (mucnt_Loose==1) {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     if (!elONLY)printEventInformation(printEventInfo, 4, true);
                     if (elcnt_Loose==0) {
                        incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                        if (!elONLY) printEventInformation(printEventInfo, 5, true);
                        mu_passStandard=true;
                        if (MET_Pass) {
                           incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                           if (!elONLY) printEventInformation(printEventInfo, 6, true);
                           mu_passAll=true;
                           //
                           // Record the Btag info
                           //
                           if (nBtagSSV==0 )
                              incrementCounter(18,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                           else {
                              if (nBtagSSV==1) 
                                 incrementCounter(19,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                              else {
                                 if (nBtagSSV==2)
                                    incrementCounter(20,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                 else
                                    incrementCounter(21,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                              }
                           }//BTags
                           
                           /////////////FNAL CUTs
                           if (MET_Et > 30) {
                              incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                              TLorentzVector ll(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
                              TLorentzVector met(METp4.Px(),METp4.Py(),METp4.Py(),METp4.E());
                              TLorentzVector mt(lp4.Px()+METp4.Px(),lp4.Py()+METp4.Py(),0,lp4.Et()+METp4.Et());
                              //TLorentzVector mt(lp4.Px()+METp4.Px(),lp4.Py()+METp4.Py(),0,lp4.Pt()+METp4.Pt());
                              if (mt.M() > 30) {
                                 incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                 if (lp4.Pt() > 25) {
                                    incrementCounter(10,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                    if (abs(lp4.Eta()) < 2.1) {
                                       incrementCounter(11,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                       if (jp4.size()>1)
                                          ptSort(jp4);
                                       if (jp4.size()!=0 && jp4[0].Pt() > 30) {
                                          incrementCounter(12,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                          if (jp4.size() > 1 && jp4[1].Pt() > 30) {
                                             incrementCounter(13,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                             if (abs(jp4[0].Eta()) < 2.4) {
                                                incrementCounter(14,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                                if (abs(jp4[1].Eta()) < 2.4) {
                                                   incrementCounter(15,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                                                   bool dr = true;
                                                   for (int i = 0; i<Nj; i++){
                                                      TLorentzVector j(jp4[i].Px(),jp4[i].Py(),jp4[i].Pz(),jp4[i].E());
                                                      if (j.DeltaR(ll) <= 0.5)
                                                         dr = false;
                                                      else
                                                         continue;
                                                   }
                                                   if (dr) {
                                                      incrementCounter(16,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
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
                                                         incrementCounter(17,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
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
      } //if(jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) 
   } //for(Int_t Nj=0; Nj<NJETS;Nj++)
   printEventInformation(printEventInfo, 7, true);
   if (printJetInfo)
      printJetInformation();
   if (printLeptonInfo)
      printLeptonInformation();
   
   //
   // Fill the Ntuple to be used in Matrix Element computations
   //    (requiring two jets & a lepton passing all of the cuts)
   // Usual option: fill in the elements which pass all of the cuts, *except* the MET cut
   //
   setFlags();

   if ((el_passFlag || mu_passFlag) && jcnt_tot==2) {
      setlp4LV();
      setlp3();
      EvtNtuple->lLV.clear();
      EvtNtuple->lLV.push_back(lp4LV);

      setj1p4LV();
      setj2p4LV();
      setjjp3();
      EvtNtuple->jLV.clear();
      EvtNtuple->jLV.push_back(j1p4LV);
      EvtNtuple->jLV.push_back(j2p4LV);
      EvtNtuple->rawjLV = rawjp4;

      //
      // QGLikelihood variables
      //
      EvtNtuple->rho=*rhoHandle;
      EvtNtuple->jChargedMultiplicity = JChargedMultiplicity;
      EvtNtuple->jNeutralMultiplicity = JNeutralMultiplicity;
      EvtNtuple->jPtD = JPtD;

      //
      // trigger infromation
      //
      EvtNtuple->triggerMap = triggerMap;

      setMETp4LV();
      EvtNtuple->METLV.clear();
      EvtNtuple->METLV.push_back(METp4LV);

      EvtNtuple->lPhi=lp4.Phi();
      EvtNtuple->METEt=METp4LV.Et();

      EvtNtuple->jBtagSSV.clear();
      EvtNtuple->jBtagTC.clear();
      EvtNtuple->jBtagSSV = jBtagSSV;
      EvtNtuple->jBtagTC = jBtagTC;
      EvtNtuple->jBtagDiscriminatorSSV.clear();
      EvtNtuple->jBtagDiscriminatorTC.clear();
      EvtNtuple->jBtagDiscriminatorSSV = jBtagDiscriminatorSSV;
      EvtNtuple->jBtagDiscriminatorTC = jBtagDiscriminatorTC;
      EvtNtuple->lQ=lQ;

      if (abs(lEta)<etaBarrelMax) {
         //
         // barrel
         //
         EvtNtuple->ldetComp=0;
      } 
      else {
         //
         // endcap
         //
         EvtNtuple->ldetComp=1;
      }
      EvtNtuple->run=runNumber;
      EvtNtuple->event=eventNumber;
       
      //
      // Transfer Function Variables
      //
      EvtNtuple->matchedGenParticles.clear();
      EvtNtuple->matchedpdgId.clear();
      EvtNtuple->matchedDeltaR.clear();
      //
      // Do the matching between the pat::Jets and the reco::genParticles
      //
      pair<int, TLorentzVector> matchedPair;
      if (!Data) {
         matchedPair = matchToGen(j1p4LV.Eta(),j1p4LV.Phi());
         EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
         EvtNtuple->matchedpdgId.push_back(matchedPair.first);
         EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j1p4LV.Eta(), j1p4LV.Phi()));
         matchedPair = matchToGen(j2p4LV.Eta(),j2p4LV.Phi());
         EvtNtuple->matchedGenParticles.push_back(matchedPair.second);
         EvtNtuple->matchedpdgId.push_back(matchedPair.first);
         EvtNtuple->matchedDeltaR.push_back(reco::deltaR(matchedPair.second.Eta(), matchedPair.second.Phi(), j2p4LV.Eta(), j2p4LV.Phi()));
         EvtNtuple->refjLV = refjp4;
         EvtNtuple->refpdgid = refpdgid;
      }
      if (!Data && saveGenParticles)
         saveGenPart();

      //
      // Additional Variables
      //
      EvtNtuple->lTotIso=lTotIso;
      EvtNtuple->lecalIso=lecalIso;
      EvtNtuple->lhcalIso=lhcalIso;
      EvtNtuple->ltrkIso=ltrkIso;
      EvtNtuple->ldetIso=ldetIso;
      EvtNtuple->lpfIso=lpfIso;
      EvtNtuple->Mjj=Mjj;
      EvtNtuple->leptonCat=DEFS::none;
      EvtNtuple->leptonCat_passAll=DEFS::none;
      if (el_passFlag)
         EvtNtuple->leptonCat=DEFS::electron;
      if (mu_passFlag)
         EvtNtuple->leptonCat=DEFS::muon;
      if (el_passAll)
         EvtNtuple->leptonCat_passAll=DEFS::electron;
      if (mu_passAll)
         EvtNtuple->leptonCat_passAll=DEFS::muon;

      setDRlj1();
      setDRlj2();
      setThetalj1pj2();
       
      EvtNtuple->DRlj1=DRlj1;
      EvtNtuple->DRlj2=DRlj2;
      EvtNtuple->Thetalj1pj2=Thetalj1pj2;

      //
      // Pileup information
      // 
      EvtNtuple->npv=0;
      const reco::VertexCollection::const_iterator vtxEnd = vtxHandle->end();
      for (reco::VertexCollection::const_iterator vtxIter = vtxHandle->begin(); vtxEnd != vtxIter; ++vtxIter) {
         if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24 && fabs(vtxIter->position().Rho())<=2.0)
            ++EvtNtuple->npv;
      }
      EvtNtuple->tnpus.clear();
      EvtNtuple->npus.clear();
      if (!Data) {
         for(unsigned int i=0; i<pileupHandle->size(); i++) {
            EvtNtuple->tnpus.push_back((*pileupHandle)[i].getTrueNumInteractions());
            EvtNtuple->npus.push_back((*pileupHandle)[i].getPU_NumInteractions());
         }
      }

      //
      // Fill The Tree
      //
      EvtTree->Fill();
   } //if((el_passFlag || mu_passFlag) && jcnt_tot==2)

   EvtTotCount++;

   return;
}


//______________________________________________________________________________
void PerformSelection::endJob()
{
   for (Int_t Nj=0; Nj<NJETS;Nj++) {
      PassLp[0][Nj]=EvtTotCount;
      PassMu[0][Nj]=EvtTotCount;
      PassEl[0][Nj]=EvtTotCount;
   }
   InitializeLabels(PLabel,CLabel);
   
   //getProcessTable(table,PassEl,PassMu,PassLp);
   getProcessTable(tableEl,PassEl);
   getProcessTable(tableMu,PassMu);
   getProcessTable(tableLp,PassLp);
   if (runtype.CompareTo("cmsRun")==0) {
      //
      // won't work for CRAB
      // only use if running using cmsRun
      //
      outtablefile.open(outtablefilename,ios::out);
      writeIntProcessTable(outtablefile,PassEl,PassMu,PassLp);
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
   PVfound = (vtxHandle->begin() != vtxHandle->end());
   zvtx = -999;
   if (PVfound) {
      pv = (*vtxHandle->begin());
      if (pv.ndof()>=4 && fabs(pv.z())<=24 && fabs(pv.position().Rho())<=2.0) {
         vtxcnt++;
         zvtx = pv.z();
      }
      else
         PVfound=false; 
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
   refjp4.clear();
   rawjp4.clear();
   jp4.clear();
   JEta.clear();
   JPhi.clear();
   refpdgid.clear();
   nBtagSSV=0;
   nBtagTC=0;
   jBtagSSV.clear();
   jBtagTC.clear();
   jBtagDiscriminatorSSV.clear();
   jBtagDiscriminatorTC.clear();
   JChargedMultiplicity.clear();
   JNeutralMultiplicity.clear();
   JPtD.clear();
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
      for (unsigned int i=0; i<l_Eta.size(); i++) {
         adphi=abs(l_Phi[i]-j_phi);
         //
         // we're on a circle
         //
         if (adphi>pi_)
            adphi=2*pi_-adphi;
         
         j_DRlepton=pow((j_eta-l_Eta[i])*(j_eta-l_Eta[i])+adphi*adphi,0.5);
         
         if ((l_Type[i]==0 && j_DRlepton<j_DRelMin) || (l_Type[i]==1 && j_DRlepton<muPrim_DRjMin))
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
	  
         //
         // store the parameters
         //

         //
         // Gen (ref) jet
         //
         if (!Data) {
            if (jetIter->genJet()) {
               math::XYZTLorentzVector v = jetIter->genJet()->p4();
               TLorentzVector lv(v.Px(),v.Py(),v.Pz(),v.E());
               refjp4.push_back(lv);
               refpdgid.push_back(jetIter->genJet()->pdgId());
            }
            else {
               TLorentzVector lv(0,0,0,0);
               refjp4.push_back(lv);
               refpdgid.push_back(0);
            }
         }

         //
         // Uncorrected (raw) jet
         //
         math::XYZTLorentzVector vv = jetIter->correctedJet("Uncorrected").p4();
         TLorentzVector lvv(vv.Px(),vv.Py(),vv.Pz(),vv.E());
         rawjp4.push_back(lvv);

         //
         // Corrected (reco) jet
         //
         jp4.push_back(jetIter->p4());
         JEta.push_back(j_eta); 
         JPhi.push_back(j_phi);
         JChargedMultiplicity.push_back(jetIter->chargedMultiplicity());
         JNeutralMultiplicity.push_back(jetIter->neutralMultiplicity());
         //if (typeid(*jetIter) == typeid(reco::PFJet))
         //PFConstituents not in SQWaT PATtuples created by V00-00-04 or earlier
         if (SQWaT_Version >= 5)
            JPtD.push_back(getPtD(jetIter->getPFConstituents()));
         else
            JPtD.push_back(0);
         jNEntries++;
	 
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
         jBtagDiscriminatorSSV.push_back(jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags"));
         jBtagDiscriminatorTC.push_back(jetIter->bDiscriminator("trackCountingHighEffBJetTags"));
         if (jetIter->bDiscriminator("simpleSecondaryVertexHighEffBJetTags")>bDiscriminatorSSVMin) {
            jBtagSSV.push_back(1);
            nBtagSSV++;
         } 
         else
            jBtagSSV.push_back(0);
         if (jetIter->bDiscriminator("trackCountingHighEffBJetTags")>bDiscriminatorTCMin) {
            jBtagTC.push_back(1);
            nBtagTC++;
         } 
         else
            jBtagTC.push_back(0);
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
      //if (abs(zvtx - muIter->vertex().z())<muPrim_dzMax && muIter->dB()<muPrim_dBMax)
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
            lp4=muIter->p4();
            lQ=muIter->charge();
            l_Eta.push_back(mu_eta);
            l_Phi.push_back(mu_phi);
            l_Type.push_back(1);
            ltrkIso=mu_TrkIso;
            ldetIso=mu_DetIso;
            if (doDetectorIso)
               lTotIso=mu_DetIso;
            else
               lTotIso=mu_TrkIso;

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
   l_Eta.clear();
   l_Phi.clear();
   l_Type.clear();
   lstreams.clear();
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
         lp4=elIter->p4();
         lQ=elIter->charge();
         l_Eta.push_back(el_eta);
         l_Phi.push_back(el_phi);
         l_Type.push_back(0);
         ltrkIso=el_TrkIso;
         ldetIso=el_DetIso;
         if (doDetectorIso)
            lTotIso=el_DetIso;
         else
            lTotIso=el_TrkIso;

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
      if ( (el_aetasc < 0.8 && elIter->electronID("mvaNonTrigV0") > 0.877 && el_PFIso < 0.426)
           || (el_aetasc > 0.8 && el_aetasc < 1.479 && elIter->electronID("mvaNonTrigV0") > 0.811 && el_PFIso < 0.481)
           || (el_aetasc > 1.479 && el_aetasc < 2.5 && elIter->electronID("mvaNonTrigV0") > 0.707 && el_PFIso < 0.390) )
         return true;
   }
   return false;
}

//______________________________________________________________________________
bool PerformSelection::eleIso(vector< pat::Electron >::const_iterator elIter, bool tight) {
   //
   // calculate the pf/detector/tracker isolation of the electron
   //
   Double_t rhoPrime = std::max((*rhoHandle),0.0);
   Double_t AEff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, elIter->eta(), ElectronEffectiveArea::kEleEAData2011);
   el_PFIso = (elIter->chargedHadronIso() + max( 0.0, elIter->neutralHadronIso() + elIter->photonIso() - AEff*rhoPrime )) / elIter->pt();
   if (elIter->isEB()) {
      el_DetIso = ( elIter->dr03TkSumPt() + std::max(0.,elIter->dr03EcalRecHitSumEt() -1.) 
                    + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*TMath::Pi()*pow(elDetIsoConeSize,2)) / elIter->p4().Pt();
   }
   else {
      el_DetIso = ( elIter->dr03TkSumPt() + elIter->dr03EcalRecHitSumEt()
                    + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*TMath::Pi()*pow(elDetIsoConeSize,2)) / elIter->p4().Pt();
   }
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

   METp4=METHandle->front().p4();
   MET_Et=METp4.E();

   MET_Pass = MET_Et > MET_EtMin;
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

   //iEvent.getByLabel(bsSource, bsHandle);
   //assert ( bsHandle.isValid() );

   //iEvent.getByLabel(convSource, convHandle);
   //assert ( convHandle.isValid() ); 

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
void PerformSelection::setlp4LV() {
   lp4LV.SetPxPyPzE(lp4.Px(),lp4.Py(),lp4.Pz(),lp4.E());
}

//______________________________________________________________________________
void PerformSelection::setlp3()
{
   lp3.SetXYZ(lp4.Px(),lp4.Py(),lp4.Pz());
}

//______________________________________________________________________________
void PerformSelection::setj1p4LV() {
   j1p4LV.SetPxPyPzE(jp4[0].Px(),jp4[0].Py(),jp4[0].Pz(),jp4[0].E());
}

//______________________________________________________________________________
void PerformSelection::setj2p4LV() {
   j2p4LV.SetPxPyPzE(jp4[1].Px(),jp4[1].Py(),jp4[1].Pz(),jp4[1].E());
}

//______________________________________________________________________________
void PerformSelection::setjjp3() {
   jjp3.SetXYZ(jp4[0].Px()+jp4[1].Px(),jp4[0].Py()+jp4[1].Py(),jp4[0].Pz()+jp4[1].Pz());
}

//______________________________________________________________________________
void PerformSelection::setMETp4LV() {
   METp4LV.SetPxPyPzE(METp4.Px(),METp4.Py(),METp4.Pz(),METp4.E());
}

//______________________________________________________________________________
void PerformSelection::setDRlj1() {
   DRlj1=lp4LV.DeltaR(j1p4LV);
}

//______________________________________________________________________________
void PerformSelection::setDRlj2() {
   DRlj2=lp4LV.DeltaR(j2p4LV);
}

//______________________________________________________________________________
void PerformSelection::setThetalj1pj2() {
   Thetalj1pj2=lp3.Dot(jjp3)/(lp3.Mag()*jjp3.Mag());
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
      cout << "sfsg1" << endl;
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
void PerformSelection::ptSort(vector<math::XYZTLorentzVector> & vec) {
   for (unsigned int next = 0; next < vec.size()-1; next++) {
      unsigned int max_pos = (unsigned int)max_position(vec, next, vec.size()-1);
      if (max_pos != next)
         swap(vec[max_pos],vec[next]);
   }
}

//______________________________________________________________________________
void PerformSelection::swap(math::XYZTLorentzVector & x, math::XYZTLorentzVector & y) {
   math::XYZTLorentzVector temp = x;
   x = y;
   y = temp;
}

//______________________________________________________________________________
int PerformSelection::max_position(vector<math::XYZTLorentzVector> & vec, int from, int to) {
   int max_pos = from;
   int i;
   for (i = from + 1; i <= to; i++)
      if (vec[i].Pt() > vec[max_pos].Pt()) max_pos = i;
   return max_pos;
}

//______________________________________________________________________________
void PerformSelection::incrementCounter(int nCut, int nJets, int ElPass[NCUTS][NJETS],
                                        int MuPass[NCUTS][NJETS], int LpPass[NCUTS][NJETS], 
                                        int MuPresent, int ElPresent) {
   if ( (MuPresent>=1)&&(ElPresent==0) ) {
      MuPass[nCut][nJets]++;
      LpPass[nCut][nJets]++;
   }
   if ( (MuPresent==0)&&(ElPresent>=1) ) {
      ElPass[nCut][nJets]++;
      LpPass[nCut][nJets]++;
   }
   /*
   if ( (MuPresent>=1)&&(ElPresent>=1) ) {
      MuPass[nCut][nJets]++;
      ElPass[nCut][nJets]++;
      LpPass[nCut][nJets]++;
   }
   */
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
