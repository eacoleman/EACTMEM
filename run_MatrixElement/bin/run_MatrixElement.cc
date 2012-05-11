/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

// C++ libraris
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// CMSSW libraries
#include "FWCore/ParameterSet/interface/FileInPath.h"

// This code libraries
#include "TAMUWW/Integrator/interface/CubaIntegrator.hh"
#include "TAMUWW/Integrator/interface/RootIntegrator.hh"
#include "TAMUWW/MatrixElement/interface/EventFile.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/MEJob.hh"
#include "TAMUWW/MatrixElement/interface/sChannelEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/tChannelEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/STopTWEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/ttEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WcEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WHEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WWEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WZEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WZtobbEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/HWWEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WLgEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WLgSubleadingEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WggEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WLLEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WbbEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WLbEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/ZLightEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/QCDEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/WHEventProb3Jet.hh"
#include "TAMUWW/MatrixElement/interface/sChannelEventProb3Jet.hh"
#include "TAMUWW/MatrixElement/interface/tChannelEventProb3Jet.hh"
#include "TAMUWW/MatrixElement/interface/ttEventProb3Jet.hh"

using std::atoi;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char* argv[]){
  if (argc != 7)
   {
     cerr << "Usage: ./run_MatrixElement <inputfile> <outputfile> <treename> [nEvents] [nSkip] [nPrescale]\n";
     return 1;
   }
  
  string inputFilename(argv[1]);
  string outputFilename(argv[2]);
  string treeName(argv[3]);
  int nEvents   = atoi(argv[4]);
  int nSkip     = atoi(argv[5]);
  int nPrescale = atoi(argv[6]);
  
  cout << " ++++ Reading from " << inputFilename << " ++++\n"
       << " ++++ Writing to " << outputFilename << " ++++\n"
       << " ++++ Reading tree " << treeName << " ++++\n"
       << " ++++ Running over " << nEvents << " events ++++\n"
       << " ++++ Skipping the first " << nSkip << " events ++++\n"
       << " ++++ Prescale by " << nPrescale << " ++++" << endl;

// ****************************
// **** Transfer Functions ****
// ****************************

  // NEW TF's
  edm::FileInPath tf_ttbar_uds_file("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_UDS_00_24.txt");
  edm::FileInPath tf_ttbar_g_file("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_G_00_24.txt");
  edm::FileInPath tf_ttbar_b_file("TAMUWW/ConfigFiles/Official/TransferFunctions/TF_TTbarMG_B_00_24.txt");
 
  // The double-gaussian transfer functions
  DGTransferFunction bTF    ( tf_ttbar_b_file.fullPath());
  DGTransferFunction lightTF( tf_ttbar_uds_file.fullPath());
  DGTransferFunction gluonTF( tf_ttbar_g_file.fullPath());
  

  
// ------------------------------------------------------------------
  
// *************************
// **** Input File Type ****
// *************************
  //RecoRootEventFile inputFile(inputFilename, "EvtTree", nEvents, nSkip, nPrescale, false);

  EventNtupleEventFile inputFile(inputFilename, treeName, nEvents, nSkip, nPrescale, false);

  // MadEventFile inputFile(inputFilename, nEvents, nSkip, nPrescale, false);
  // SmearedMadEventFile inputFile(inputFilename, nEvents, nSkip, nPrescale,false);
  //   SingleTopNtupleEventFile inputFile(inputFilename, "SingleTopNtuple",
  //                                      nEvents, nSkip, nPrescale, false);
  //  inputFile.setJetCorrLevel(5);
  
// ------------------------------------------------------------------

// ********************
// **** Integrator ****
// ********************
// The type of integration
//   NullIntegrator integrator;
  
  RootIntegrator rootInt;
  rootInt.setNcomp(1);
  rootInt.setEpsilon(.01, 0);
  rootInt.setVerbose(0);
  rootInt.setSampleSet(true);
  rootInt.setPseudoRandom(true);
  rootInt.setNeval(0, 1000000);
  
//   CubaInt::Cuhre integrator;
//   integrator.setCubatureRule(9);

//   CubaInt::Vegas integrator;
//   integrator.setParams(1000, 500);

//   CubaInt::Suave integrator;
//   integrator.setNew(1000);
//   integrator.setFlatness(25);

  CubaInt::Divonne divonneInt;
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


// ------------------------------------------------------------------

  MEJob job(inputFile, outputFilename);

// ************************
// **** Dynamic Bounds ****
// ************************
  bool dynBounds;
  dynBounds = true;
  job.setDynamicBounds(dynBounds);
  
  cout << "Using b-jet " << bTF.getName() << endl;
  cout << "Using light-jet " << lightTF.getName() << endl;
  cout << "Using gluon-jet " << gluonTF.getName() << endl;
  if (dynBounds)
    cout << "Using dynamic bounds" << endl;
  
// ------------------------------------------------------------------

// *************************
// **** Matrix Elements ****
// *************************
  vector<EventProb*> eventProbs2jet;
  vector<EventProb*> eventProbs3jet;


  ///Diboson:
  eventProbs2jet.push_back(new WWEventProb2Jet(rootInt, lightTF));
  eventProbs2jet.push_back(new WZEventProb2Jet(rootInt, lightTF));
  eventProbs2jet.push_back(new WZtobbEventProb2Jet(rootInt, bTF));

  ///W or Z backgrounds:
  eventProbs2jet.push_back(new WLgEventProb2Jet(rootInt, lightTF, gluonTF));
  eventProbs2jet.push_back(new WLgSubleadingEventProb2Jet(rootInt, lightTF, gluonTF));
  eventProbs2jet.push_back(new WLLEventProb2Jet(rootInt, lightTF));
  eventProbs2jet.push_back(new WLbEventProb2Jet(rootInt, lightTF, bTF));
  eventProbs2jet.push_back(new WbbEventProb2Jet(rootInt, bTF));
  eventProbs2jet.push_back(new ZLightEventProb2Jet(rootInt, lightTF));

  ///Top and QCD:
   eventProbs2jet.push_back(new ttEventProb2Jet(divonneInt, bTF));
  eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
  eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
  eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
  eventProbs2jet.push_back(new tChannelEventProb2Jet(rootInt, bTF, lightTF));
  eventProbs2jet.push_back(new sChannelEventProb2Jet(rootInt, bTF));
  eventProbs2jet.push_back(new STopTWEventProb2Jet(divonneInt, lightTF));
  eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
  eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
  eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
  eventProbs2jet.push_back(new QCDEventProb2Jet(rootInt, gluonTF));


//   ///HWW:
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 120));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 130));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 140));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 150));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 160));

//   ///WH:
//   eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, bTF, 115));
//   eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, bTF, 120));
//   eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, bTF, 125));
//   eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, bTF, 130));
//   eventProbs2jet.push_back(new WHEventProb2Jet(rootInt, bTF, 135));


  // ***************************
  // **** Loading into MEJob
  // ***************************

  // Load the matrix elements for 2-jets into the job
  for (vector<EventProb*>::const_iterator it = eventProbs2jet.begin();
       it != eventProbs2jet.end(); ++it){

      (*it)->setBounds(0, -MEConstants::beamEnergy, MEConstants::beamEnergy);
      (*it)->setBounds(1, 0, MEConstants::beamEnergy);
      (*it)->setBounds(2, 0, MEConstants::beamEnergy);
      
      job.addEventProb(**it, 2);
  }
  
  // Load the matrix elements for 3-jets into the job
  for (vector<EventProb*>::const_iterator it = eventProbs3jet.begin();
       it != eventProbs3jet.end(); ++it)   {
    
    (*it)->setBounds(0, -MEConstants::beamEnergy, MEConstants::beamEnergy);
    (*it)->setBounds(1, 0, MEConstants::beamEnergy);
    (*it)->setBounds(2, 0, MEConstants::beamEnergy);
    
    job.addEventProb(**it, 3); // 3 means 3 jets
    
  }//for


  // ***************************
  // **** Command execution ****
  // ***************************
  job.loopOverEvents();
  // job.massScan(100, 150, 10, "Top");
  // job.massScan(100, 150, 50, "Higgs");
  // job.massScan(100, 250, 50,"Top");
  // job.massScan(50, 200, 50,"Top");


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


   return 0;


}//main


