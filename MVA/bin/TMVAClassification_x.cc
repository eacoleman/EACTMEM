///////////////////////////////////////////////////////////////////
//
// TMVAClassification_x
// -------------------------
//
//            08/27/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l TMVAClassification.C\(\"Fisher,Likelihood\"\)                       *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set is used.                                     *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 **********************************************************************************/

//Our libraries
#include "TAMUWW/MVA/interface/TAMUWWMVA.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//ROOT libraries
#include "TBenchmark.h"
#include "TApplication.h"

//C++ Libraries
#include <string>

using namespace TMVA;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
/*
  EXAMPLE::
  TMVAClassification_x -myMethodList BDT
  TMVAClassification_x -myMethodList BDT,Likelihood,KNN
*/
int main(int argc, char**argv) {
   TApplication theApp("App", &argc, argv);

   //
   // evaluate command-line / configuration file options
   //
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   bool            train            = cl.getValue<bool>         ("train",            true);
   bool            plot             = cl.getValue<bool>         ("plot",             true);
   bool            batch            = cl.getValue<bool>         ("batch",            false);
   TString         myMethodList     = cl.getValue<TString>      ("myMethodList",     "MLP:::BDT:::KNN");
   vector<TString> plots            = cl.getVector<TString>     ("plots",            "");
   TString         ofileBase        = cl.getValue<TString>      ("ofileBase",        "TMVA");
   TString         ofile            = cl.getValue<TString>      ("ofile",            "");
   if(!ofile.IsNull())
      ofile += ".root";
   string          lepCat           = cl.getValue<string>       ("lep",              "both");
   DEFS::LeptonCat leptonCat        = DEFS::getLeptonCat(lepCat);
   string          jetBinS          = cl.getValue<string>       ("jetBin",           "jets2");
   DEFS::JetBin    jetBin           = DEFS::getJetBin(jetBinS);
   string          tagcatS          = cl.getValue<string>       ("tagcat",           "pretag");
   DEFS::TagCat    tagcat           = DEFS::getTagCat(tagcatS);
   vector<TString> signals          = cl.getVector<TString>     ("signals",          "ggH125:::qqH125:::WH125");
   vector<TString> backgrounds      = cl.getVector<TString>     ("backgrounds",      "WJets");
   vector<int>     eventProbs       = cl.getVector<int>         ("eventProbs",       "");
   
   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   vector<PhysicsProcess*> processes = DefaultValues::getProcessesHiggs(jetBin, tagcat, false, false, DEFS::MicroNtuple);

   TAMUWWMVA* mva = new TAMUWWMVA();
   mva->IsBatch(batch);
   mva->setMethodList(myMethodList);
   mva->setProcesses(processes);
   mva->setSignals(signals);
   mva->setBackgrounds(backgrounds);
   mva->setLeptonCat(leptonCat);
   mva->setPlots(plots);
   mva->setOfileBase(ofileBase);
   mva->setOfile(ofile);
   if(eventProbs.size()>0) {
      mva->setEventProbsToRun(eventProbs);
   }

   if (train)
      mva->TMVAClassification();
   if (plot)
      mva->Plot();

   m_benchmark->Stop("event");
   cout << "TMVAClassification_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;
}
