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

//
// CMS includes
//
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// User Defined Includes
//
#include "TAMUWW/Integrator/interface/CubaIntegrator.hh"
#include "TAMUWW/Integrator/interface/RootIntegrator.hh"
#include "TAMUWW/MatrixElement/interface/EventFile.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/MEJobForCrab.hh"
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

//
// Standard Library Includes
//
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

//
// Namespaces
//
using std::atoi;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class MatrixElement : public edm::EDAnalyzer {
public:
   // construction/destruction
   explicit MatrixElement(const edm::ParameterSet& iConfig);
   virtual ~MatrixElement();
   
private:
   // member functions
   void beginJob();
   void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
   void endJob();

private:
   //
   // member data
   //
   // program variables
   TString inputFilename;
   TString outputFilename;
   TString inputDirectoryName;
   bool useTDirectoryFile;
   TString inputTreeName;
   int nEvents;
   int nSkip;
   int nPrescale;

   // input file type
   EventNtupleEventFile* inputFile;
   
   // trees
   TTree* outputTree;

   // me job
   MEJobForCrab* job;

   // transfer function variables
   edm::FileInPath* tf_ttbar_uds_file;
   edm::FileInPath* tf_ttbar_g_file;
   edm::FileInPath* tf_ttbar_b_file;
   DGTransferFunction* bTF;
   DGTransferFunction* lightTF;
   DGTransferFunction* gluonTF;

   // integrators
   RootIntegrator rootInt;
   RootIntegrator rootIntZLight;
   CubaInt::Divonne divonneInt;
   CubaInt::Divonne divonneInt_reduceComputingTime;

   // matrix elements
   vector<EventProb*> eventProbs2jet;
   vector<EventProb*> eventProbs3jet;

};
