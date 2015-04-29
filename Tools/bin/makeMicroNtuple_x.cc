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

   vector<TString> inputPaths                = cl.getVector<TString> ("inputPaths",   "/uscms_data/d3/ilyao/Winter12to13ME8TeV/MEResults/rootOutput/");
   TString         outputPath                = cl.getValue<TString>  ("outputPath",  "/uscms_data/d2/aperloff/Summer12ME8TeV/MEResults/microNtuples/");
   int             largeProcessCase          = cl.getValue<int>      ("largeProcessCase",                                                           0);
   //TString         smallProcessLabel         = cl.getValue<TString>  ("smallProcessLabel",                                                  "ggH170");
   vector<TString> processes                 = cl.getVector<TString> ("processes", "ggH125:::qqH125:::WH125:::WW:::WZ:::ZZ:::ZJets:::TTbar:::STopS_T:::STopS_Tbar:::STopT_T:::STopT_Tbar:::STopTW_T:::STopTW_Tbar:::WJets");
   TString         mergeNewEventNtuple       = cl.getValue<TString>  ("mergeNewEventNtuple",                                                       "");
   bool            saveMissingEvents         = cl.getValue<bool>     ("saveMissingEvents",                                                      false);
   bool            fillBDT                   = cl.getValue<bool>     ("fillBDT",                                                                false);
   bool            debug                     = cl.getValue<bool>     ("debug",                                                                  false);

   if (!cl.check()) return 0;
   cl.print();
   
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   // This loads the TMVA library
   TMVA::Tools::Instance();

   if(inputPaths.size()>1 && inputPaths.size()!=processes.size()) {
      cout << "ERROR::makeMicroNtuple_x The size of the inputPaths vector and the processes vector are not the same" << endl
           << "The program will now exit" << endl;
      return 0;
   }
   else if(inputPaths.size()==1 && inputPaths.size()!=processes.size()) {
      vector<TString> tmp(processes.size(),inputPaths.front());
      inputPaths = tmp;
   }
   if(!outputPath.EndsWith("/")) outputPath += "/";
   for(unsigned int ip=0; ip<inputPaths.size(); ip++)
      if(!inputPaths[ip].EndsWith("/")) inputPaths[ip] += "/";

   MicroNtupleMaker* mnm;
   for(unsigned int i=0; i<processes.size(); i++) {
      mnm = new MicroNtupleMaker(); 
      mnm->setEventNtuplePath(mergeNewEventNtuple);
      mnm->setProcess(processes[i]);
      mnm->setOutputPath(outputPath);
      mnm->setMissingEventsFlag(saveMissingEvents);
      mnm->setFillBDT(fillBDT);
      mnm->setDebug(debug);
      mnm->createMicroNtuple(inputPaths[i],outputPath,largeProcessCase,processes[i]);
      delete mnm;
   }      

   m_benchmark->Stop("event");
   cout << "makeMicroNtuple_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main
