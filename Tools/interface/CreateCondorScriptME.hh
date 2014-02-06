#ifndef CREATECONDORSCRIPTME_HH
#define CREATECONDORSCRIPTME_HH

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TError.h"
#include "TStyle.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TString.h"

// C++ libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <strstream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <cerrno>
#include <exception>
#include <sys/stat.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  class declaration
////////////////////////////////////////////////////////////////////////////////
class CreateCondorScriptME {
public:
   //C'tor
   CreateCondorScriptME();
   //D'tor
   ~CreateCondorScriptME();

   //Member Functions
   vector<TString> read_directory( const string& path = string() );
   int get_file_number(TString filename);
   void checkForBadFiles(bool verbose = false);
   void checkForMissingFiles();
   pair<TString, int> getInvalidFiles();
   void setCompletedOutputDir(string cod) {completedOutputDir = cod;}
   void setDisjointJob(bool dj = false) {createDisjointJobScript = dj;}
   void setDisjointJobString(TString djs = "") {disjointJobsString = djs;}
   void setEventsPerJob(int epj = 400) {nEventsPerJob = epj;}
   void setFirstJob(int fj = 0) {firstJob = fj;}
   void setGlobalRunMESuffix(TString grmes = "") {globalRunMESuffix = grmes;}
   void setOutputName(TString on) {OutputName = on;}
   void setRootInputDir(TString rid) {rootInputDir = rid;}
   void setRootInputName(TString rin) {rootInputName = rin;}
   void setScriptDir(TString sd) {ScriptDir = sd;}
   void setScriptNameSuffix(TString sns = "") {ScriptNameSuffix = sns;}
   void setSpecificEventsFile(bool use = false, TString sefl = "1") {useSpecificEvents = use; specificEventsFileLocation = sefl;}
   void setTotalEvents(int et = -1) {nEventsTot = -1;}
   void setTotalJobs(int nj = -1) {nJobsTot = nj;}
   void setTreeName(TString tn = "METree") {tree_name = tn;}
   void writeCondorScripts();
   void writePBSScripts(TString pbsQueue, TString pbsWalltimeStr, TString pbsUser);

private:
   vector<TString> files;
   vector<int> all_files, good_files, bad_files, missing_files;
   bool bad, createDisjointJobScript, useSpecificEvents;
   int nJobsTot, nEventsTot, counter, nEventsPerJob, firstJob;
   TString ScriptDir, tree_name, ScriptNameSuffix, rootInputDir, rootInputName, OutputName, disjointJobsString, globalRunMESuffix, specificEventsFileLocation;
   string completedOutputDir;
};

#endif
