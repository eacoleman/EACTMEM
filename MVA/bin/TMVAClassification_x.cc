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
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//ROOT libraries
#include "TBenchmark.h"

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
   //
   // evaluate command-line / configuration file options
   //
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   bool            train            = cl.getValue<bool>         ("train",            true);
   bool            plot             = cl.getValue<bool>         ("plot",             true);
   TString         myMethodList     = cl.getValue<TString>      ("myMethodList",     "MLP:::BDT:::KNN");
   vector<TString> plots            = cl.getVector<TString>     ("plots",            "");
   TString         ofileBase        = cl.getValue<TString>      ("ofileBase",        "TMVA");
   TString         ofile            = cl.getValue<TString>      ("ofile",            "");
   string          jetBinS          = cl.getValue<string>       ("jetBin",           "jets2");
   DEFS::JetBin    jetBin           = DEFS::getJetBin(jetBinS);
   string          tagcatS          = cl.getValue<string> ("tagcat",           "pretag");
   DEFS::TagCat    tagcat           = DEFS::getTagCat(tagcatS);
   //TString         ifilePath        = cl.getValue<TString>  ("ifilePath", "/uscms_data/d2/aperloff/Summer12ME8TeV/MEResults/microNtuples_optimized");
   //vector<TString> ifilesSignal     = cl.getVector<TString> ("ifilesSignal",     "ggH125:::qqH125:::WH125");
   //vector<TString> ifilesBackground = cl.getVector<TString> ("ifilesBackground", "WW:::WZ:::WJets:::ZJets:::TTbar:::STopS_T:::STopS_Tbar:::STopT_T:::STopT_Tbar:::STopTW_T:::STopTW_Tbar:::QCD_Electron_Dp6p7");
   //TString         treeName         = cl.getValue<TString>  ("treeName",         "METree");
   //double          luminosity       = cl.getValue<double>   ("luminosity",       19148.0);
   
   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   vector<PhysicsProcess*> processes = DefaultValues::getProcessesHiggs(jetBin, tagcat, false, false, DEFS::MicroNtuple);

   //TAMUWWMVA* mva = new TAMUWWMVA(myMethodList, ifilePath, ifilesSignal, ifilesBackground,
   //                               treeName, luminosity, plots, ofileBase, ofile);
   TAMUWWMVA* mva = new TAMUWWMVA(myMethodList, processes, plots, ofileBase, ofile);

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
