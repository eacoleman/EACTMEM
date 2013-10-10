////////////////////////////////////////////////////////////////////////////////
//
// MatrixElementCrab
// -----------------
//
//                         11/19/2012 Ilya Osipenkov <ilyao@fnal.gov> 
//                                    Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// CMS 
// Higgs CrossSection Measurement using Matrix Element 
////////////////////////////////////////////////////////////////////////////////
// Calculates the event probabilities of a certain MC event to come from a set
// of matrix elements (physics processes).
////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/run_MatrixElement/interface/MatrixElementCrab.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
MatrixElement::MatrixElement(const edm::ParameterSet& iConfig) {
   //-----Program Level Inputs
   inputFilename      = TString(iConfig.getParameter<string> ("inputFilename"));
   outputFilename     = TString(iConfig.getParameter<string> ("outputFilename"));
   inputDirectoryName = TString(iConfig.getParameter<string> ("inputDirectoryName"));
   inputTreeName      = TString(iConfig.getParameter<string> ("inputTreeName"));
   nEvents            =         iConfig.getParameter<int>    ("nEvents");
   nPrescale          =         iConfig.getParameter<int>    ("nPrescale");
   char* nJobChar;
   int nJob;
   nJobChar = getenv ("NJob");
   if (nJobChar!=NULL)
      nJob = atoi(nJobChar);
   else
      nJob = -1;
   nSkip              =  nJob * iConfig.getParameter<int>    ("nSkip");

  if (inputDirectoryName!="") {
    useTDirectoryFile = true;
  }
  else 
     useTDirectoryFile = false;

  cout << " ++++ Reading from " << inputFilename << " ++++\n"
       << " ++++ Writing to " << outputFilename << " ++++\n"
       << " ++++ Reading TDirectoryFile " << inputDirectoryName << " ++++\n"
       << " ++++ Reading tree " << inputTreeName << " ++++\n"
       << " ++++ Running over " << nEvents << " events ++++\n"
       << " ++++ Skipping the first " << nSkip << " events ++++\n"
       << " ++++ Prescale by " << nPrescale << " ++++" << endl
       << " ++++ Job number is " << nJob << " ++++\n";
}


//______________________________________________________________________________
MatrixElement::~MatrixElement() {
   delete tf_ttbar_uds_file;
   delete tf_ttbar_g_file;
   delete tf_ttbar_b_file;
   delete bTF;
   delete lightTF;
   delete gluonTF;
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void MatrixElement::beginJob() {
   //edm::Service<TFileService> fs;
   //if (!fs) throw edm::Exception(edm::errors::Configuration,
   //                              "TFileService missing from configuration!");

   // create the output tree
   //outputTree = fs->make<TTree>("METree", "Output tree for matrix element");
   //outputTree->Branch("METree", "METree", &m_output);

// ****************************
// **** Transfer Functions ****
// ****************************

   // NEW TF's
   //tf_ttbar_uds_file = new edm::FileInPath("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_UDS_00_24.txt");
   //tf_ttbar_g_file = new edm::FileInPath("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_G_00_24.txt");
   //tf_ttbar_b_file = new edm::FileInPath("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_B_00_24.txt");
   tf_ttbar_uds_file = new edm::FileInPath("../TF_TTbarMG_UDS_00_24.txt");
   tf_ttbar_g_file = new edm::FileInPath("../TF_TTbarMG_G_00_24.txt");
   tf_ttbar_b_file = new edm::FileInPath("../TF_TTbarMG_B_00_24.txt");
 

  // The double-gaussian transfer functions
  bTF     = new DGTransferFunction( tf_ttbar_b_file->fullPath());
  lightTF = new DGTransferFunction( tf_ttbar_uds_file->fullPath());
  gluonTF = new DGTransferFunction( tf_ttbar_g_file->fullPath());

// *************************
// **** Input File Type ****
// *************************

  //FIX ME!!!!!!!!!!!!!!!!!
  inputFile = new EventNtupleEventFile(string(inputFilename), string(inputTreeName), nEvents, nSkip, nPrescale, useTDirectoryFile, string(inputDirectoryName), false);

  job = new MEJobForCrab((*inputFile), string(outputFilename));

// ********************
// **** Integrator ****
// ********************
// The type of integration
//   NullIntegrator integrator;
  
  rootInt.setNcomp(1);
  rootInt.setEpsilon(.01, 0);
  rootInt.setVerbose(0);
  rootInt.setSampleSet(true);
  rootInt.setPseudoRandom(true);
  rootInt.setNeval(0, 1000000);

  // For ZLight (to reduce the time required)
  rootIntZLight.setNcomp(1);
  rootIntZLight.setEpsilon(.05, 0);
  rootIntZLight.setVerbose(0);
  rootIntZLight.setSampleSet(true);
  rootIntZLight.setPseudoRandom(true);
  rootIntZLight.setNeval(0, 1000000);
  
//   CubaInt::Cuhre integrator;
//   integrator.setCubatureRule(9);

//   CubaInt::Vegas integrator;
//   integrator.setParams(1000, 500);

//   CubaInt::Suave integrator;
//   integrator.setNew(1000);
//   integrator.setFlatness(25);

  divonneInt.setPartitioningRule(47);
  divonneInt.setIntegrationRule(1);
  divonneInt.setRefinementRule(1);
  divonneInt.setMaxPass(5);
  divonneInt.setBorder(0);
  divonneInt.setChi2(10);
  divonneInt.setMinDev(.25);
//   divonneInt.setMaxPeaks(4);
//   divonneInt.setPeakFinder(MEJob::sm_peakFinder);
  divonneInt.setNcomp(1);
  divonneInt.setEpsilon(.01, 0);//change if changing integration time
  divonneInt.setVerbose(0);
  divonneInt.setSampleSet(true);
  divonneInt.setPseudoRandom(true);
  divonneInt.setNeval(0, static_cast<int>(1e7));

  ///Reduce computing time for STopTW & ttbar
  divonneInt_reduceComputingTime.setPartitioningRule(47);
  divonneInt_reduceComputingTime.setIntegrationRule(1);
  divonneInt_reduceComputingTime.setRefinementRule(1);
  divonneInt_reduceComputingTime.setMaxPass(5);
  divonneInt_reduceComputingTime.setBorder(0);
  divonneInt_reduceComputingTime.setChi2(10);
  divonneInt_reduceComputingTime.setMinDev(.25);
//   divonneInt_reduceComputingTime.setMaxPeaks(4);
//   divonneInt_reduceComputingTime.setPeakFinder(MEJob::sm_peakFinder);
  divonneInt_reduceComputingTime.setNcomp(1);
  divonneInt_reduceComputingTime.setEpsilon(1.0, 0);//change (e.g. 0.01 to 0.1, 0) if changing integration time 
  divonneInt_reduceComputingTime.setVerbose(0);
  divonneInt_reduceComputingTime.setSampleSet(true);//possibly change
  divonneInt_reduceComputingTime.setPseudoRandom(true);//possibly change
  divonneInt_reduceComputingTime.setNeval(0, static_cast<int>(1e7));

  //MEJob job((*inputFile), string(outputFilename));
   
// ************************
// **** Dynamic Bounds ****
// ************************
   bool dynBounds;
   dynBounds = true;
   job->setDynamicBounds(dynBounds);
   
   cout << "Using b-jet " << bTF->getName() << endl;
   cout << "Using light-jet " << lightTF->getName() << endl;
   cout << "Using gluon-jet " << gluonTF->getName() << endl;
   if (dynBounds)
      cout << "Using dynamic bounds" << endl;
   
// *************************
// **** Matrix Elements ****
// *************************
   
   ///Test computing time:
//   eventProbs2jet.push_back(new STopTWEventProb2Jet(divonneInt_reduceComputingTime, (*lightTF)));
//   cout << "2" << endl;
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
//   ////Tests:
//   eventProbs2jet.push_back(new WWEventProb2Jet(rootInt, (*lightTF)));
//   eventProbs2jet.push_back(new WZEventProb2Jet(rootInt, (*lightTF)));
   
   //Diboson:
   eventProbs2jet.push_back(new WWEventProb2Jet(rootInt, (*lightTF)));
   eventProbs2jet.push_back(new WZEventProb2Jet(rootInt, (*lightTF)));
   eventProbs2jet.push_back(new WZtobbEventProb2Jet(rootInt, (*bTF)));
   
   
  ///W or Z backgrounds:
  eventProbs2jet.push_back(new WLgEventProb2Jet(rootInt, (*lightTF), (*gluonTF)));
  eventProbs2jet.push_back(new WLgSubleadingEventProb2Jet(rootInt, (*lightTF), (*gluonTF)));
  eventProbs2jet.push_back(new WggEventProb2Jet(rootInt, (*gluonTF)));
  eventProbs2jet.push_back(new WLLEventProb2Jet(rootInt, (*lightTF)));
  eventProbs2jet.push_back(new WLbEventProb2Jet(rootInt, (*lightTF), (*bTF)));
  eventProbs2jet.push_back(new WbbEventProb2Jet(rootInt, (*bTF)));
  eventProbs2jet.push_back(new ZLightEventProb2Jet(rootIntZLight, (*lightTF)));


  ///Top and QCD (reduced version doesn't compute ttbar and STopTW MEs, which take up the majority of computing time due to additional integration)
//   eventProbs2jet.push_back(new ttEventProb2Jet(divonneInt, (*bTF)));
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
  eventProbs2jet.push_back(new tChannelEventProb2Jet(rootInt, (*bTF), (*lightTF)));
  eventProbs2jet.push_back(new sChannelEventProb2Jet(rootInt, (*bTF)));
//   eventProbs2jet.push_back(new STopTWEventProb2Jet(divonneInt_reduceComputingTime, (*lightTF)));
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
  eventProbs2jet.push_back(new QCDEventProb2Jet(rootInt, (*gluonTF)));


  ///HWW:
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 110));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 115));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 120));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 122));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 123));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 124));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 125));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 126));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 127));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 128));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 130));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 135));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 140));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 150));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 160));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 170));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 180));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 190));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 200));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 250));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 300));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 350));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 400));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 450));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 500));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 550));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 600));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 650));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 700));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 750));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 800));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 850));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 900));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 950));
  eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, (*lightTF), 1000));


  ///WH:
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 110));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 115));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 120));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 122));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 123));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 124));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 125));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 126));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 127));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 128));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 130));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 135));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 140));
  eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, (*bTF), 150));
  
  ///VBF H:
  //eventProbs2jet.push_back();

  // ***************************
  // **** Loading into MEJob
  // ***************************

  // Load the matrix elements for 2-jets into the job
  for (vector<EventProb*>::const_iterator it = eventProbs2jet.begin();
       it != eventProbs2jet.end(); ++it){

      (*it)->setBounds(0, -MEConstants::beamEnergy, MEConstants::beamEnergy);
      (*it)->setBounds(1, 0, MEConstants::beamEnergy);
      (*it)->setBounds(2, 0, MEConstants::beamEnergy);
      
      job->addEventProb(**it, 2);
  }
  
  // Load the matrix elements for 3-jets into the job
  for (vector<EventProb*>::const_iterator it = eventProbs3jet.begin();
       it != eventProbs3jet.end(); ++it)   {
    
    (*it)->setBounds(0, -MEConstants::beamEnergy, MEConstants::beamEnergy);
    (*it)->setBounds(1, 0, MEConstants::beamEnergy);
    (*it)->setBounds(2, 0, MEConstants::beamEnergy);
    
    job->addEventProb(**it, 3); // 3 means 3 jets
    
  }//for

  // ***************************
  // **** Command execution ****
  // ***************************
  cout << "Before Event Loop" << endl;
  job->loopOverEvents();
  cout << "After Event Loop" << endl;
  // job.massScan(100, 150, 10, "Top");
  // job.massScan(100, 150, 50, "Higgs");
  // job.massScan(100, 250, 50,"Top");
  // job.massScan(50, 200, 50,"Top");

}

//______________________________________________________________________________
void MatrixElement::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {}


//______________________________________________________________________________
void MatrixElement::endJob() {
  // ***************************
  // ****   Cleaning up     ****
  // ***************************
 
   // Clean up for the 2-jet ME's
   for (vector<EventProb*>::const_iterator it = eventProbs2jet.begin();
        it != eventProbs2jet.end(); ++it)
     delete *it;

   // Clean up for the 3-jet ME's
   for (vector<EventProb*>::const_iterator it = eventProbs3jet.begin();
        it != eventProbs3jet.end(); ++it)
     delete *it;
}


////////////////////////////////////////////////////////////////////////////////
// define MatrixElement as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(MatrixElement);
