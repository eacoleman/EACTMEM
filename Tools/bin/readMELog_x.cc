//Our libraries
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2D.h"
#include "TChain.h"

// C++ libraries
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <utility>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  Local Functions
////////////////////////////////////////////////////////////////////////////////

/// prints out the average CPU times and real times for each of the ME processes
void average_times_per_process(string ifile);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
 
   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   
   string ifile    = cl.getValue<string> ("ifile");
   string mcSample = cl.getValue<string> ("mcSample", "");
   
   if (!cl.check()) return 0;
   cl.print();
   
   // benchmark this executable
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   // Analyze the log file
   cout << "readMELog::Average times for " << mcSample << " sample:" << endl;
   average_times_per_process(ifile);
   
   // benchmark this executable
   m_benchmark->Stop("event");
   cout << "readMELog_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main

////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void average_times_per_process(string ifile) {

   // open the log file
   fstream fin;
   fin.open(ifile,fstream::in);

   map<string,vector<double> > CPUTimes;
   map<string,vector<double> > realTimes;
   char line[1024];
   string currentME;
   double currentCPUTime = 0;
   double currentRealTime = 0;
   size_t CPUPos;
   size_t realPos;
   size_t sPos;

   // clear header
   //cout << endl << endl << "First while" << endl << endl;
   //fin.getline(line,1024);
   while(TString(line).Contains("Starting")!=1) {
      fin.getline(line,1024);
      //cout << TString(line) << endl;
   }
   //cout << endl << endl << "First for" << endl << endl;
   //50 to first event
   //56 to first process
   for(int i=0; i<50; i++) {
      fin.getline(line,1024);
      //cout << TString(line) << endl;
   }

   while(TString(line).Contains("Finished")!=1) {
      if(TString(line).Contains("Processing")) {
         if(string(line).substr(19,string(line).find("with")-20)=="1")
            cout << "\tEvent " << string(line).substr(19,string(line).find("with")-20) << "...";
         else
            cout << "DONE" << endl
                 << "\tEvent " << string(line).substr(19,string(line).find("with")-20) << "...";  
      }
      while(TString(line).Contains("Starting")!=1) {
         fin.getline(line,1024);
      }
      currentME = string(line).substr(11,string(line).size()-24);
      while(TString(line).Contains("CPU")!=1) {
         fin.getline(line,1024);
      }
      CPUPos = string(line).find("CPU");
      sPos = string(line).find("s");
      currentCPUTime = atof(string(line).substr(CPUPos+10,sPos-15).c_str());
      realPos = string(line).find("Real");
      sPos = string(line).rfind("s");
      currentRealTime = atof(string(line).substr(realPos+11,sPos-realPos-12).c_str());

      CPUTimes[currentME].push_back(currentCPUTime);
      realTimes[currentME].push_back(currentRealTime);
      fin.getline(line,1024);
   }
   cout << "DONE" << endl;

   map<string,vector<double> >::iterator it_real=realTimes.begin();
   //cout << "sfsg1 " << (*it_real).second.size()<< endl;
   for(map<string,vector<double> >::iterator it_cpu=CPUTimes.begin(); it_cpu!=CPUTimes.end(); it_cpu++) {
      cout << endl << "\tME Process: " << (*it_cpu).first << endl;
      double averageCPU = 0;
      double averageReal = 0;
      double sumCPU = 0;
      double sumReal = 0;
      cout << "\tAveraging CPU times ";
      for(unsigned int i=0; i<(*it_cpu).second.size(); i++) {
         cout << ".";
         sumCPU+=(*it_cpu).second[i];
      }
      cout << endl << "\tAveraging real times ";
      for(unsigned int i=0; i<(*it_real).second.size(); i++) {
         cout << ".";
         sumReal+=(*it_real).second[i];
      }
      averageCPU = sumCPU/(*it_cpu).second.size();
      averageReal = sumReal/(*it_real).second.size();

      
      cout << endl << "\t\tCPU: " << averageCPU << endl
           << "\t\tReal: " << averageReal << endl;

      it_real++;
   }

   // close the log file
   fin.close();

}//average_times_per_process
