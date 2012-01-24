/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Tool for creating scripts & launch files to run_MatrixElement on CONDOR.

#include <iostream>
#include <iomanip>
#include <strstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include <TString.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <TAMUWW/Tools/GlobalTools428.cc>

void CreateCondorScript_runME(const char* ScriptDir, const char* ScriptName, const char* rootInputDir, const char* rootInputName, const char* OutputName, int FirstEvt, int NEvtsPerJob, int FirstJob, int NJobs)
//// Creates a condor launcher (ScriptDir/CondorLauncher_ScriptName) script and a .csh script used by it (ScriptDir/CondorScript_ScriptName.csh).
//// These can be used from any directory, provided it has cteq5l.tbl & cteq6l.tbl libraries (normally located in TAMUWW/PDFs/Pdfdata/), a /log subdirectory as well as run_MatrixElement macro in it. Note: rootInputDir is the location of the .root input file as seen from this directory.
//// Running the CondorLauncher will generate root files of the form ScriptName_CondorRun#.root (with #=FirstJob,..,FirstJob+NJobs-1).
{
  ofstream outlauncher, outscript;
  TString tempStr;
  tempStr=ScriptName;
  tempStr="CondorLauncher_" + tempStr;
  tempStr=ScriptDir + tempStr;
  outlauncher.open(tempStr,ios::out);
  tempStr=".csh";
  tempStr=ScriptName + tempStr;
  tempStr="CondorScript_" + tempStr;
  tempStr=ScriptDir + tempStr;
  outscript.open(tempStr,ios::out);

  //Temporary Variables
  //  TString str1;
  char FirstEvt_C[6], NEvtsPerJob_C[6], FirstJob_C[6], NJobs_C[6];
  sprintf(FirstEvt_C,"%i",FirstEvt);
  sprintf(NEvtsPerJob_C,"%i",NEvtsPerJob);
  sprintf(FirstJob_C,"%i",FirstJob);
  sprintf(NJobs_C,"%i",NJobs);

  /// Make the launcher
  outlauncher << "universe = vanilla" << endl;
  outlauncher << "Executable = " << tempStr << endl;
  outlauncher << "Requirements = Memory >= 199 &&OpSys == \"LINUX\"&& (Arch != \"DUMMY\" )&& Disk > 1000000" << endl;
  outlauncher << "Should_Transfer_Files = YES" << endl;
  outlauncher << "WhenToTransferOutput = ON_EXIT" << endl;
  outlauncher << "Transfer_Input_Files = " << rootInputDir << rootInputName << ", run_MatrixElement, cteq5l.tbl, cteq6l.tbl" << endl;
  //outlauncher << "Transfer_Input_Files = " << rootInputDir << rootInputName << ", run_MatrixElement" << endl;
  outlauncher << "Output = log/CondorME_" << ScriptName << "_C$(Cluster)_$(Process).stdout" << endl;
  outlauncher << "Error = log/CondorME_" << ScriptName << "_C$(Cluster)_$(Process).stderr" << endl;
  outlauncher << "Log = log/CondorME_" << ScriptName << "_C$(Cluster)_$(Process).log" << endl;
  outlauncher << "notify_user = none" << endl;
  outlauncher << "Arguments = $(Process) " << NEvtsPerJob_C << endl;
  outlauncher << "Queue " << NJobs_C << endl;

  /// Make the internal script
  outscript << "#! /bin/csh" << endl;
  outscript << "echo \"Starting\" " << endl;
  outscript << "cd /uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/src" << endl;//***SET DEPENDENT ON THE CMSSW RELEASE***
  outscript << "source /uscmst1/prod/sw/cms/cshrc uaf" << endl;
  // outscript << "source /uscmst1/prod/grid/gLite_SL5.csh" << endl;
  // outscript << "setenv SCRAM_ARCH slc5_ia32_gcc434"<< endl;//only needed for CMSSW_3_X_Y
  outscript << "pwd" << endl;
  //  outscript << "cmsenv" << endl;
  outscript << "eval `scram runtime -csh` " << endl;
  outscript << "cd -" << endl;
  outscript << "echo \"Process=$argv[1]\"" << endl;
  outscript << "echo \"NEvts=$argv[2]\"" << endl;
  outscript << "@ StartEvt = $argv[2] * $argv[1] + " << FirstEvt_C << endl;
  outscript << "echo \"StartEvt=NEvts*Process+" << FirstEvt_C << "=$StartEvt\"" << endl;
  outscript << "@ StartJob = $argv[1] + " << FirstJob_C << endl;
  outscript << "run_MatrixElement " << rootInputName << " " << OutputName << "$StartJob.root EvtTree " << NEvtsPerJob_C << " $StartEvt 1" << endl;
  outscript << "echo \"Finished\"" << endl;

  tempStr="chmod +x " + tempStr;
  system(tempStr);  
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void CreateMultipleCondorScripts_runME(const char* inputmode, const char* ScriptDir, const char* rootInputDir, int StartProcess, int EndProcess, int NEvtsPerJob, bool runOnJESShifted=false )
//// Calls CreateCondorScript_runME for each process, with the option of running on JES shifted files as well (runOnJESShifted=true).
//// ScriptName=LME+PLabel+suffix, OutputName=PLabel+ME+suffix; where suffix="DEF","JESp1s","JESm1s"
//// rootInputName=PLabel+inputmode+inputsuffix, where inputsuffix="_outfile.root" and potentially "_JESp1s.root", "_JESm1s.root"
{
  TString scriptBaseName,rootinputBaseName,outputBaseName;
  int nEntries,nJobs,nEvtsPerJob;
  InitializeLabels(PLabel,CLabel);

  for (Int_t np=StartProcess; np<(EndProcess+1);np++) {
    cout << "Process=" << PLabel[np] << endl;
    scriptBaseName=PLabel[np];
    scriptBaseName=inputmode+scriptBaseName;
    rootinputBaseName=inputmode;
    rootinputBaseName=PLabel[np]+rootinputBaseName;
    outputBaseName=inputmode;
    outputBaseName=PLabel[np]+outputBaseName;

    TFile* infile = new TFile(rootInputDir+rootinputBaseName+"_outfile.root");
    TTree* InTree = (TTree*)infile->Get("EvtTree");
    nEntries=InTree->GetEntries();
    cout << "nEntries=" << nEntries ;
    nJobs=nEntries/NEvtsPerJob;
    nEvtsPerJob=NEvtsPerJob;
    //when the number of events is less than number of jobs, run on all of the events
    if ( nJobs==0 ) {
      nJobs=1;
      nEvtsPerJob=nEntries;
    }

    cout << ", nJobs=" << nJobs << endl;

    CreateCondorScript_runME(ScriptDir,scriptBaseName+"DEF",rootInputDir,rootinputBaseName+"_outfile.root",outputBaseName+"DEF",0,nEvtsPerJob,0,nJobs);
    if ( runOnJESShifted ) {
      CreateCondorScript_runME(ScriptDir,scriptBaseName+"JESp1s",rootInputDir,rootinputBaseName+"_JESp1s.root",outputBaseName+"JESp1s",0,nEvtsPerJob,0,nJobs);
      CreateCondorScript_runME(ScriptDir,scriptBaseName+"JESm1s",rootInputDir,rootinputBaseName+"_JESm1s.root",outputBaseName+"JESm1s",0,nEvtsPerJob,0,nJobs);
    }
  }
  
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void GetMax_runME(const char* inFileName, int indexME)
////Gets the maximum value for m_tProbStat.tEventProb[indexME] in the file inFileName (in the tree METree)
///The usual approach to getting tree contents (e.g. InTree->SetBranchAddress("METEt",&METEt)) doesn't work.
{
  TFile* f = new TFile(inFileName);
  TTree* InTree = (TTree*)f->Get("METree");
  double globalMax = InTree->GetMaximum("m_tProbStat.tEventProb");

  TH1F* hist1 = new TH1F("hist1","hist1",1000000,0,globalMax);

  double Max=-1;
  double binCnt;
  int i_MaxBin=-1;
  char indexME_char[5];
  TString drawstr;
  drawstr = "]>>hist1";
  sprintf(indexME_char,"%i",indexME);
  drawstr = indexME_char + drawstr;
  drawstr = "m_tProbStat.tEventProb[" + drawstr;
  //  cout << "drawstr = " << drawstr << endl;
  InTree->Draw(drawstr);
//   hist1->SetLineColor(2);
//   hist1->Draw();

  for (Int_t i=1; i<1000001;i++) {
       binCnt=hist1->GetBinContent(i);
       //cout << "binCnt(" << i << ") = " << hst->GetBinContent(i) << endl;
    if ( binCnt>0.5 ) {
      Max=hist1->GetBinLowEdge(i+1);
    }
  }
  //  cout << "Max_hist1=" << Max << endl;

  TH1F* hist2 = new TH1F("hist2","hist2",1000000,0,Max);
  Max=-1;
  drawstr = "]>>hist2";
  drawstr = indexME_char + drawstr;
  drawstr = "m_tProbStat.tEventProb[" + drawstr;
  InTree->Draw(drawstr);
  hist2->SetLineColor(2);
  hist2->Draw();
  for (Int_t i=1; i<1000001;i++) {
       binCnt=hist2->GetBinContent(i);
    if ( binCnt>0.5 ) {
      Max=hist2->GetBinLowEdge(i+1);
      i_MaxBin=i;
    }
  }
  cout << "Max=" << Max << " for bin " << i_MaxBin << endl;

}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Merge_runME(const char* rootFilesDir, const char* fileBaseName, const char* MergeName, int StartJob, int NJobs)
////Merges the .root output files (with a tree METree).
////The input files should be of the form rootFilesDirfileBaseName#.root, where #=StartJob to StartJob+NJobs-1
{
  TChain ch("METree");
  TString tempStr;
  char Job_C[6];

  for (Int_t jn=StartJob; jn<(StartJob+NJobs);jn++) {
    tempStr=".root";
    sprintf(Job_C,"%i",jn);
    tempStr=Job_C+tempStr;
    tempStr=fileBaseName+tempStr;
    tempStr=rootFilesDir+tempStr;
    ch.Add(tempStr);
  }
  ch.Merge(MergeName);

}


