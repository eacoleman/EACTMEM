//Our libraries
#include "TAMUWW/Tools/interface/CutOptimization.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
 int main(int argc,char**argv) {
   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   TString         ifile        = cl.getValue<TString>  ("ifile");
   TString         ofile        = cl.getValue<TString>  ("ofile",                                               "");
   TString         lepton       = cl.getValue<TString>  ("lepton",                                              "");
   TString         treeNameBase = cl.getValue<TString>  ("treeNameBase",                                        "");
   vector<TString> signals      = cl.getVector<TString> ("signals",                              "ggH125:::qqH125");
   vector<TString> backgrounds  = cl.getVector<TString> ("backgrounds", "WW:::WZ:::ZZ:::DYJets:::TTbar:::STopS_T:::STopS_Tbar:::STopT_T:::STopT_Tbar:::STopTW_T:::STopTW_Tbar:::WJets_muon");
   vector<TString> cuts         = cl.getVector<TString> ("cuts",                     "gt:::lt:::gt:::lt:::gt:::lt");
   TString         options      = cl.getValue<TString>  ("options",                                             "");
   vector<bool>    logScales    = cl.getVector<bool>    ("logScales", "true:::false:::true:::false:::true:::false");
   double          fom          = cl.getValue<double>   ("fom",                                                3.0);

   if (!cl.check()) return 0;
   cl.print();
   
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   for(unsigned int i=0; i<signals.size(); i++) {
      signals[i] = treeNameBase + signals[i];
   }
   for(unsigned int i=0; i<backgrounds.size(); i++) {
      backgrounds[i] = treeNameBase + backgrounds[i];
   }

   CutOptimization co;
   co.setSignal(co.readAndSumTProfileMDF(ifile, signals, "signalSum"));
   co.setBackground(co.readAndSumTProfileMDF(ifile, backgrounds, "backgroundSum"));
   co.setCuts(cuts);
   co.setFOMContainer();
   co.loopOverCuts(fom,options);
   co.getMaxFOMAndBin();
   co.drawSaveProjections(ofile,logScales);

   m_benchmark->Stop("event");
   cout << "Optimization_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main

