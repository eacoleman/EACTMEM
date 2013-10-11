//Our libraries
#include "TAMUWW/Tools/interface/MicroNtupleMaker.hh"
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

   TString         inputPath                 = cl.getValue<TString>  ("inputPath",   "/uscms_data/d3/ilyao/Winter12to13ME8TeV/MEResults/rootOutput/");
   TString         outputPath                = cl.getValue<TString>  ("outputPath", "/uscms_data/d2/aperloff/Summer12ME8TeV/MEResults/microNtuples/");
   int             largeProcessCase          = cl.getValue<int>      ("largeProcessCase",                                                          0);
   //TString         smallProcessLabel         = cl.getValue<TString>  ("smallProcessLabel",                                                  "ggH170");
   vector<TString> processes                 = cl.getVector<TString> ("processes", "ggH125:::qqH125:::WH125:::WW:::WZ:::ZZ:::ZJets:::TTbar:::STopS_T:::STopS_Tbar:::STopT_T:::STopT_Tbar:::STopTW_T:::STopTW_Tbar:::WJets");
   TString         mergeNewEventNtuple       = cl.getValue<TString>  ("mergeNewEventNtuple",       "/uscms_data/d2/aperloff/Summer12ME8TeV/MEInput/");

   if (!cl.check()) return 0;
   cl.print();
   
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

//   if(!scriptDir.EndsWith("/")) scriptDir += "/";

   MicroNtupleMaker* mnm = new MicroNtupleMaker();
   for(unsigned int i=0; i<processes.size(); i++) {
      mnm->setEventNtuplePath(mergeNewEventNtuple);
      mnm->setProcess(processes[i]);
      mnm->createMicroNtuple(inputPath,outputPath,largeProcessCase,processes[i]);
   }      

   m_benchmark->Stop("event");
   cout << "makeMicroNtuple_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main
