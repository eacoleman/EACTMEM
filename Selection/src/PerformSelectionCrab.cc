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
   runtype                    = TString(iConfig.getParameter<string>          ("runtype"));

   printEventInfo             =         iConfig.getParameter<bool>            ("printEventInfo");
   printJetInfo               =         iConfig.getParameter<bool>            ("printJetInfo");
   printLeptonInfo            =         iConfig.getParameter<bool>            ("printLeptonInfo");
   Data                       =         iConfig.getParameter<bool>            ("Data");
   QCDcuts                    =         iConfig.getParameter<bool>            ("QCDcuts");
   saveGenParticles           =         iConfig.getParameter<bool>            ("saveGenParticles");
   noMETCut                   =         iConfig.getParameter<bool>            ("noMETCut");
   invertEID                  =         iConfig.getParameter<bool>            ("invertEID");
   PFlowLoose                 =         iConfig.getParameter<bool>            ("PFlowLoose");
   elONLY                     =         iConfig.getParameter<bool>            ("elONLY");
   muONLY                     =         iConfig.getParameter<bool>            ("muONLY");

   //   cutOnTrigger               =         iConfig.getParameter<bool>            ("cutOnTrigger");
   SQWaT_Version              =         iConfig.getParameter<int>             ("SQWaT_Version");
   doRelIso                   =         iConfig.getParameter<bool>            ("doRelIso");

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

   //-----B-Tag Variable Inputs
   nBtagSSV                   =         iConfig.getParameter<int>             ("nBtagSSV");
   nBtagTC                    =         iConfig.getParameter<int>             ("nBtagTC");
   bDiscriminatorSSVMin       =         iConfig.getParameter<double>          ("bDiscriminatorSSVMin");
   bDiscriminatorTCMin        =         iConfig.getParameter<double>          ("bDiscriminatorTCMin");

   //-----Muon Variable Inputs
   muPrim_ptMin               =         iConfig.getParameter<double>          ("muPrim_ptMin");
   muPrim_aetaMax             =         iConfig.getParameter<double>          ("muPrim_aetaMax");
   muPrim_dBMax               =         iConfig.getParameter<double>          ("muPrim_dBMax");
   muPrim_RelIsoMax           =         iConfig.getParameter<double>          ("muPrim_RelIsoMax");
   muPrim_TrkIsoMax           =         iConfig.getParameter<double>          ("muPrim_TrkIsoMax");
   isProperlyIsolatedMu       =         iConfig.getParameter<bool>            ("isProperlyIsolatedMu");
   muLoose_ptMin              =         iConfig.getParameter<double>          ("muLoose_ptMin");
   muLoose_aetaMax            =         iConfig.getParameter<double>          ("muLoose_aetaMax");
   muLoose_RelIsoMax          =         iConfig.getParameter<double>          ("muLoose_RelIsoMax");
   muLoose_TrkIsoMax          =         iConfig.getParameter<double>          ("muLoose_TrkIsoMax");

   //-----Electron Variable Inputs
   elPrim_ptMin               =         iConfig.getParameter<double>          ("elPrim_ptMin");
   elPrim_aetaMax             =         iConfig.getParameter<double>          ("elPrim_aetaMax");
   elPrim_aetascExcludeMax    =         iConfig.getParameter<double>          ("elPrim_aetascExcludeMax");
   elPrim_aetascExcludeMin    =         iConfig.getParameter<double>          ("elPrim_aetascExcludeMin");
   elPrim_dBMax               =         iConfig.getParameter<double>          ("elPrim_dBMax");
   elPrim_RelIsoMax           =         iConfig.getParameter<double>          ("elPrim_RelIsoMax");
   elPrim_TrkIsoMax           =         iConfig.getParameter<double>          ("elPrim_TrkIsoMax");
   
   elPrim_sigmaIetaIetaMaxEB  =         iConfig.getParameter<double>          ("elPrim_sigmaIetaIetaMaxEB");
   elPrim_aDeltaPhiMaxEB      =         iConfig.getParameter<double>          ("elPrim_aDeltaPhiMaxEB");
   elPrim_aDeltaEtaMaxEB      =         iConfig.getParameter<double>          ("elPrim_aDeltaEtaMaxEB");
   elPrim_HoEMaxEB            =         iConfig.getParameter<double>          ("elPrim_HoEMaxEB"); 
   elPrim_sigmaIetaIetaMaxEE  =         iConfig.getParameter<double>          ("elPrim_sigmaIetaIetaMaxEE");
   elPrim_aDeltaPhiMaxEE      =         iConfig.getParameter<double>          ("elPrim_aDeltaPhiMaxEE");
   elPrim_aDeltaEtaMaxEE      =         iConfig.getParameter<double>          ("elPrim_aDeltaEtaMaxEE");
   elPrim_HoEMaxEE            =         iConfig.getParameter<double>          ("elPrim_HoEMaxEE");

   elLoose_ptMin              =         iConfig.getParameter<double>          ("elLoose_ptMin");
   elLoose_RelIsoMax          =         iConfig.getParameter<double>          ("elLoose_RelIsoMax");
   elLoose_TrkIsoMax          =         iConfig.getParameter<double>          ("elLoose_TrkIsoMax");

   elLoose_sigmaIetaIetaMaxEB =         iConfig.getParameter<double>          ("elLoose_sigmaIetaIetaMaxEB");
   elLoose_aDeltaPhiMaxEB     =         iConfig.getParameter<double>          ("elLoose_aDeltaPhiMaxEB");
   elLoose_aDeltaEtaMaxEB     =         iConfig.getParameter<double>          ("elLoose_aDeltaEtaMaxEB");
   elLoose_HoEMaxEB           =         iConfig.getParameter<double>          ("elLoose_HoEMaxEB");
   elLoose_sigmaIetaIetaMaxEE =         iConfig.getParameter<double>          ("elLoose_sigmaIetaIetaMaxEE");
   elLoose_aDeltaPhiMaxEE     =         iConfig.getParameter<double>          ("elLoose_aDeltaPhiMaxEE");
   elLoose_aDeltaEtaMaxEE     =         iConfig.getParameter<double>          ("elLoose_aDeltaEtaMaxEE");
   elLoose_HoEMaxEE           =         iConfig.getParameter<double>          ("elLoose_HoEMaxEE"); 
   
   el_aetaPass                =         iConfig.getParameter<bool>            ("el_aetaPass");

   //-----Lepton Variable Inputs

   //-----MET Variable Inputs
   MET_EtMin                  =         iConfig.getParameter<double>          ("MET_EtMin");

   //-----Additional Variable Inputs
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


   printEventInformation(printEventInfo, 0, true);
   for(Int_t Nj=0; Nj<NJETS;Nj++) {
      if( (jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) && (mu_passTrigger||el_passTrigger)   ) {
         PassLp[1][Nj]++;
         PassMu[1][Nj]++;
         PassEl[1][Nj]++;
         printEventInformation(printEventInfo, 1, true);
         
         //
         // Record the electron info
         //
         if (elcnt_Prim>=1 && PVfound) {
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
            printEventInformation(printEventInfo, 2, false);
            if (mucnt_Loose==0) {
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
               printEventInformation(printEventInfo, 3, false);
               if (elcnt_Loose==1) {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                  printEventInformation(printEventInfo, 4, false);
                  el_passStandard=true;
                  if ( MET_Pass||(QCDcuts&&(METp4[0].Et()>20.0)) ) {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     printEventInformation(printEventInfo, 5, false);
                     el_passAll=true;
                  }
                  //
                  // Record the Btag info
                  //
                  if (nBtagSSV==0)
                     incrementCounter(6,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                  else {
                     if (nBtagSSV==1)
                        incrementCounter(7,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     else {
                        if (nBtagSSV==2)
                           incrementCounter(8,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                        else
                           incrementCounter(9,Nj,PassEl,PassMu,PassLp,0,elcnt_Prim);
                     }
                  }//BTags
               }
            }
         }//electrons
	 
         //
         // Record the muon info
         //
         if (mucnt_Prim>=1 && PVfound) {
            incrementCounter(2,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
            printEventInformation(printEventInfo, 2, true);
            if (mucnt_Loose==1) {
               incrementCounter(3,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
               printEventInformation(printEventInfo, 3, true);
               if (elcnt_Loose==0) {
                  incrementCounter(4,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                  printEventInformation(printEventInfo, 4, true);
                  mu_passStandard=true;
                  if ( MET_Pass||(QCDcuts&&(METp4[0].Et()>20.0)) ) {
                     incrementCounter(5,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     printEventInformation(printEventInfo, 5, true);
                     mu_passAll=true;
                  }
                  //
                  // Record the Btag info
                  //
                  if (nBtagSSV==0 )
                     incrementCounter(6,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                  else {
                     if (nBtagSSV==1) 
                        incrementCounter(7,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     else {
                        if (nBtagSSV==2)
                           incrementCounter(8,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                        else
                           incrementCounter(9,Nj,PassEl,PassMu,PassLp,mucnt_Prim,0);
                     }
                  }//BTags
               }
            }
         } //if(mucnt_Prim>=1 && PVfound)
      } //if(jcnt_tot==Nj || (Nj==(NJETS-1) && jcnt_tot>(NJETS-1))) 
   } //for(Int_t Nj=0; Nj<NJETS;Nj++)
   printEventInformation(printEventInfo, 6, true);
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
      //
      // Event Level Information
      //
      EvtNtuple->run=runNumber;
      EvtNtuple->event=eventNumber;
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
      EvtNtuple->Mjj = Mjj;

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

      //
      // MET Information
      //
      EvtNtuple->METLV.clear();
      EvtNtuple->METLV = METp4;

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
  if (Data) {
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
   //
   // corresponds to the 'medium' cut (per https://twiki.cern.ch/twiki/bin/view/CMS/BTV10001)
   //
   double bDiscriminator_min=1.74;

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
          jetIter->neutralHadronEnergyFraction()<0.99 && jetIter->neutralEmEnergyFraction()<0.99 && 
          jetIter->numberOfDaughters()>1 && jetIter->chargedHadronEnergyFraction()>0 && 
          jetIter->chargedMultiplicity()>0 && jetIter->chargedEmEnergyFraction()<0.99) {
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
         //if (typeid(*jetIter) == typeid(reco::PFJet))
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
      mu_RelIso = (muIter->ecalIso() + muIter->hcalIso() + muIter->trackIso() -
                   (*rhoHandle * 3.1415 * pow(0.3,2)));
      mu_RelIso /= mu_pt;
      mu_TrkIso=(muIter->trackIso())/mu_pt;
      
      muisGmTm=false;
      if (muIter->isGlobalMuon() && muIter->isTrackerMuon())
         muisGmTm=true;

      muisTightPrompt=false;
      if (muIter->globalTrack().isNonnull()) {
         if (muIter->globalTrack()->normalizedChi2()<10 &&
            muIter->globalTrack()->hitPattern().numberOfValidMuonHits()>0)
            muisTightPrompt=true;
      }

      if (muIter->innerTrack().isNonnull())
         muPrim_itNHits=muIter->innerTrack()->numberOfValidHits();
      else
         muPrim_itNHits=-1;
       
      mu_vtxPass=false;
      if (abs(vp4[0].Z() - muIter->vertex().z())<1)
         mu_vtxPass=true;
       
      //
      // Apply The Primary Cuts
      //
      if (muisGmTm && mu_pt>muPrim_ptMin && abs(mu_eta)<muPrim_aetaMax && muisTightPrompt &&
          muPrim_itNHits>10 && muIter->dB()<muPrim_dBMax && mu_vtxPass && 
          muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement()>=1 &&
          muIter->numberOfMatches()>1) {
         isProperlyIsolatedMu=false;
         
         if (((doRelIso && mu_RelIso<muPrim_RelIsoMax)||(!doRelIso && mu_TrkIso<muPrim_TrkIsoMax))&&(!QCDcuts))
            isProperlyIsolatedMu=true;

         if ( (mu_RelIso>0.1)&&(QCDcuts))
            isProperlyIsolatedMu=true;
            
         //
         // Record the muon quantities:
         //
         if (isProperlyIsolatedMu) {
            mucnt_Prim++;
            if ( QCDcuts&&(mu_RelIso>muLoose_RelIsoMax) ) {
               //Account for the fact that if we are inverting the isolation, the tight muon may no longer pass the default loose cuts.
               mucnt_Loose++;
            }
            math::XYZTLorentzVector lv = muIter->p4();
            lp4.push_back(Lepton(lv.Px(),lv.Py(),lv.Pz(),lv.E()));
            lp4.back().lQ=muIter->charge();
            lp4.back().leptonCat = DEFS::muon;
            lp4.back().ltrkIso=mu_TrkIso;
            lp4.back().lrelIso=mu_RelIso;
            if (doRelIso)
               lp4.back().lTotIso=mu_RelIso;
            else
               lp4.back().lTotIso=mu_TrkIso;
            lp4.back().mIsGlobal = muisGmTm;
            lp4.back().mIsTracker = muIter->isTrackerMuon();
            lp4.back().mNormalizedChi2 = muIter->globalTrack()->normalizedChi2();
            lp4.back().mNumberOfValidMuonHits = muIter->globalTrack()->hitPattern().numberOfValidMuonHits();
            lp4.back().mNumberValidHits = muPrim_itNHits;
            lp4.back().mNumberOfPixelLayersWithMeasurement = muIter->innerTrack()->hitPattern().pixelLayersWithMeasurement();
            lp4.back().mNumberOfMatches = muIter->numberOfMatches();
            lp4.back().ldz = abs(vp4[0].Z() - muIter->vertex().z());
            lp4.back().ldB = muIter->dB();

            //lstream << "mu_tight trkIso " << mu_TrkIso << " relIso " << mu_RelIso << endl;
            lstream << "mu_tight trkIso " << muIter->trackIso() << " ecalIso " << muIter->ecalIso() << " hcalIso " << muIter->hcalIso() << " relIso " << mu_RelIso << " rho "<< *rhoHandle << endl;
            lstreams.push_back(lstream.str());
            lstream.str("");
         }
      }

      //
      // Apply The Loose Cuts
      //
      if (muIter->isGlobalMuon() && mu_pt>muLoose_ptMin && abs(mu_eta)<muLoose_aetaMax &&
          ((doRelIso && mu_RelIso<muLoose_RelIsoMax)||(!doRelIso && mu_TrkIso<muLoose_TrkIsoMax))) {
         mucnt_Loose++;

         //lstream << "mu_loose trkIso " << mu_TrkIso << " relIso " << mu_RelIso << endl;
         lstream << "mu_loose trkIso " << muIter->trackIso() << " ecalIso " << muIter->ecalIso() << " hcalIso " << muIter->hcalIso() << " relIso " << mu_RelIso << " rho "<< *rhoHandle << endl;
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
      // EID Cuts are applied directly
      //
      el_PassEIDcnt=-1;
      el_PassEID=false;
      el_sigmaIetaIeta=elIter->sigmaIetaIeta();
      el_aDeltaPhi=abs(elIter->deltaPhiSuperClusterTrackAtVtx());
      el_aDeltaEta=abs(elIter->deltaEtaSuperClusterTrackAtVtx());
      el_HoE=elIter->hadronicOverEm();
      if (elIter->isEB()) {
         el_PassEIDcnt=0;
         if (el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEB)
            el_PassEIDcnt++;
         if (el_aDeltaPhi<elPrim_aDeltaPhiMaxEB)
            el_PassEIDcnt++;
         if (el_aDeltaEta<elPrim_aDeltaEtaMaxEB)
            el_PassEIDcnt++;
         if (el_HoE<elPrim_HoEMaxEB)
            el_PassEIDcnt++;
      }
      if (elIter->isEE()) {
         el_PassEIDcnt=0;
         if (el_sigmaIetaIeta<elPrim_sigmaIetaIetaMaxEE)
            el_PassEIDcnt++;
         if (el_aDeltaPhi<elPrim_aDeltaPhiMaxEE)
            el_PassEIDcnt++;
         if (el_aDeltaEta<elPrim_aDeltaEtaMaxEE)
            el_PassEIDcnt++;
         if (el_HoE<elPrim_HoEMaxEE)
            el_PassEIDcnt++;
      }
      if (el_PassEIDcnt==4 ) el_PassEID=true;

      //
      // calculate the relative/tracker isolation of the electron
      //
      if (elIter->isEB()) {
         el_RelIso = ( elIter->dr03TkSumPt() + std::max(0.,elIter->dr03EcalRecHitSumEt() -1.) 
                       + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*3.1415*pow(0.3,2)) / el_pt;
      }
      else {
         el_RelIso = ( elIter->dr03TkSumPt() + elIter->dr03EcalRecHitSumEt()
                       + elIter->dr03HcalTowerSumEt() - (*rhoHandle)*3.1415*pow(0.3,2)) / el_pt;
      }
      el_TrkIso=(elIter->dr03TkSumPt())/el_pt;

      //
      // Apply The Primary Cuts
      //
      if (el_pt>elPrim_ptMin && el_aetaPass && 
	  ((!QCDcuts && doRelIso && el_RelIso<elPrim_RelIsoMax)||(!QCDcuts && !doRelIso && el_TrkIso<elPrim_TrkIsoMax)||(QCDcuts && el_RelIso>0.1)) && 
         el_PassEID  &&
         abs(elIter->dB())<elPrim_dBMax &&
         elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits()==0 &&
         !((abs(elIter->convDist())<0.02)&&(abs(elIter->convDcot())<0.02))) {
         elcnt_Prim++;
         if ( QCDcuts && (el_RelIso>elLoose_RelIsoMax) ){
            //Account for the fact that if we are inverting the isolation and the tight electron may no longer pass the default loose cuts.
            elcnt_Loose++;
         }
         math::XYZTLorentzVector lv = elIter->p4();
         lp4.push_back(Lepton(lv.Px(),lv.Py(),lv.Pz(),lv.E()));
         lp4.back().lQ=elIter->charge();
         lp4.back().leptonCat = DEFS::electron;
         lp4.back().ltrkIso=el_TrkIso;
         lp4.back().lrelIso=el_RelIso;
         if (doRelIso)
            lp4.back().lTotIso=el_RelIso;
         else
            lp4.back().lTotIso=el_TrkIso;
         lp4.back().ldB = abs(elIter->dB());
         lp4.back().eIsEB = elIter->isEB();
         lp4.back().eIsEE = elIter->isEE();
         lp4.back().eSuperClusterEta = elIter->superCluster()->eta();
         lp4.back().eEcalEnergy = elIter->ecalEnergy();
         lp4.back().eSigmaIetaIeta = elIter->sigmaIetaIeta();
         lp4.back().eDeltaPhi = elIter->deltaPhiSuperClusterTrackAtVtx();
         lp4.back().eDeltaEta = elIter->deltaEtaSuperClusterTrackAtVtx();
         lp4.back().eHadronicOverEm = elIter->hadronicOverEm();
         lp4.back().eNumberOfHits = elIter->gsfTrack()->trackerExpectedHitsInner().numberOfHits();
         lp4.back().eConvDist = abs(elIter->convDist());
         lp4.back().eConvDcot = abs(elIter->convDcot());

         //lstream << "el_tight trkIso " << el_TrkIso << " relIso " << el_RelIso << endl;
         lstream << "el_tight trkIso " << elIter->dr03TkSumPt() << " ecalIso " << elIter->dr03EcalRecHitSumEt() << " hcalIso " << elIter->dr03HcalTowerSumEt() << " relIso " << el_RelIso << " rho "<< *rhoHandle << endl;
         lstreams.push_back(lstream.str());
         lstream.str("");
      }

      /*
        reco::TrackBase::Point beamPoint(0,0,0);
        reco::BeamSpot beamSpot;
        edm::Handle<reco::BeamSpot> beamSpotHandle;
        event.getByLabel(edm::InputTag("offlineBeamSpot"), beamSpotHandle);
        if( beamSpotHandle.isValid() ){
        beamSpot = *beamSpotHandle;
        } else{
        edm::LogError("DataNotAvailable")
        << "No beam spot available from EventSetup, not adding high level selection \n";
        }
        beamPoint = reco::TrackBase::Point ( beamSpot.x0(), beamSpot.y0(), beamSpot.z0() );
        double d0bs = ielectron->gsfTrack()->dxy(beamPoint);
       */

      //
      // Apply The Loose Cuts
      //
      el_PassEIDcnt=-1;
      el_PassEID=false;
      if (elIter->isEB()) {
         el_PassEIDcnt=0;
         if (el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEB)
            el_PassEIDcnt++;
         if (el_aDeltaPhi<elLoose_aDeltaPhiMaxEB)
            el_PassEIDcnt++;
         if (el_aDeltaEta<elLoose_aDeltaEtaMaxEB)
            el_PassEIDcnt++;
         if (el_HoE<elLoose_HoEMaxEB)
            el_PassEIDcnt++;
      }

      if (elIter->isEE()) {
         el_PassEIDcnt=0;
         if (el_sigmaIetaIeta<elLoose_sigmaIetaIetaMaxEE)
            el_PassEIDcnt++;
         if (el_aDeltaPhi<elLoose_aDeltaPhiMaxEE)
            el_PassEIDcnt++;
         if (el_aDeltaEta<elLoose_aDeltaEtaMaxEE)
            el_PassEIDcnt++;
         if (el_HoE<elLoose_HoEMaxEE)
            el_PassEIDcnt++;
      }
      if (el_PassEIDcnt==4) el_PassEID=true;

      if (el_pt>elLoose_ptMin && el_aetaPass && 
          ((doRelIso && el_RelIso<elLoose_RelIsoMax)||(!doRelIso && el_TrkIso<elLoose_TrkIsoMax)) &&
          el_PassEID) {
         elcnt_Loose++;

         //lstream << "el_loose trkIso " << el_TrkIso << " relIso " << el_RelIso << endl;
         lstream << "el_loose trkIso " << elIter->dr03TkSumPt() << " ecalIso " << elIter->dr03EcalRecHitSumEt() << " hcalIso " << elIter->dr03HcalTowerSumEt() << " relIso " << el_RelIso << " rho "<< *rhoHandle << endl;
         lstreams.push_back(lstream.str());
         lstream.str("");
      }
   }
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
      EvtNtuple->genParticleCollection[i].particlePosition = memMap[EvtNtuple->genParticleCollection[i].particlePosition].second;
      for(unsigned int j=0; j<EvtNtuple->genParticleCollection[i].motherPositions.size(); j++) {
         EvtNtuple->genParticleCollection[i].motherPositions[j] = memMap[EvtNtuple->genParticleCollection[i].motherPositions[j]].second; 
      }
      for(unsigned int j=0; j<EvtNtuple->genParticleCollection[i].daughterPositions.size(); j++) {
         EvtNtuple->genParticleCollection[i].daughterPositions[j] = memMap[EvtNtuple->genParticleCollection[i].daughterPositions[j]].second;
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
}

//______________________________________________________________________________
void PerformSelection::printEventInformation(bool print, int cLevel, bool muon) {
   if (!print) return;
   if (cLevel == 0)
      cout << "Event " << eventNumber << " NJets " << jcnt_tot << " Primary Muons " << mucnt_Prim
           << " Primary Electrons " << elcnt_Prim << " Loose Muons " << mucnt_Loose << " Loose Electrons " 
           << elcnt_Loose;
   else if (cLevel == 1)
      cout << " c1";
   else if (cLevel == 2) {
      if (muon)
         cout << " c2";  
      else if (!muon)
         cout << " c2";  
      else
         cout << " WARNING::Unknown primary lepton type!";
   }
   else if (cLevel == 3)
      cout << " c3";
   else if (cLevel == 4)
      cout << " c4";
   else if (cLevel == 5)
      cout << " c5";
   else if (cLevel == 6)
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
