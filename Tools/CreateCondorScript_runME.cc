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

#include <TAMUWW/Tools/GlobalTools532.cc>

void CreateCondorScript_runME(const char* ScriptDir, const char* ScriptNameSuffix, const char* rootInputDir, const char* rootInputName, const char* OutputName, int NEvtsPerJob, int FirstJob, int NJobs, bool createDisjointJobScript = false, const char* disjointJobsString="")
//// Creates a condor launcher (ScriptDir/CondorLauncher_ScriptNameSuffix) script and a .csh script used by it (ScriptDir/CondorScript_ScriptNameSuffix.csh).
//// These can be used from any directory, provided it has cteq5l.tbl & cteq6l.tbl libraries (normally located in TAMUWW/PDFs/Pdfdata/), a /log subdirectory as well as run_MatrixElement macro in it. Note: rootInputDir is the location of the .root input file as seen from this directory.
//// Running the CondorLauncher will generate root files of the form OutputName#.root (with #=FirstJob,...,FirstJob+NJobs-1).
//// To create a script which will run over disjoint jobs set createDisjointJobScript=false and input disjointJobsString= JobA , JobB , ... , JobZ. Note, the job numbers & corresponding events are the same as in the continuos string, you are simply specifying explicitly which jobs to run over (through a series of if statements).
{
  ofstream outlauncher, outscript;
  TString tempStr;
  tempStr=ScriptNameSuffix;
  tempStr="CondorLauncher_" + tempStr;
  tempStr=ScriptDir + tempStr;
  outlauncher.open(tempStr,ios::out);
  tempStr=".csh";
  tempStr=ScriptNameSuffix + tempStr;
  tempStr="CondorScript_" + tempStr;
  tempStr=ScriptDir + tempStr;
  outscript.open(tempStr,ios::out);

  //Temporary Variables
//   char NEvtsPerJob_C[6], FirstJob_C[6], NJobs_C[6];
//   //sprintf(FirstEvt_C,"%i",FirstEvt);
//   sprintf(NEvtsPerJob_C,"%i",NEvtsPerJob);
//   sprintf(FirstJob_C,"%i",FirstJob);
//   sprintf(NJobs_C,"%i",NJobs);

  /// Make the launcher
  outlauncher << "universe = vanilla" << endl;
  outlauncher << "Executable = " << tempStr << endl;
  outlauncher << "Requirements = Memory >= 199 &&OpSys == \"LINUX\"&& (Arch != \"DUMMY\" )&& Disk > 1000000" << endl;
  outlauncher << "Should_Transfer_Files = YES" << endl;
  outlauncher << "WhenToTransferOutput = ON_EXIT" << endl;
  outlauncher << "Transfer_Input_Files = " << rootInputDir << rootInputName << ", run_MatrixElement, cteq5l.tbl, cteq6l.tbl" << endl;
  outlauncher << "Output = log/CondorME_" << ScriptNameSuffix << "_C$(Cluster)_$(Process).stdout" << endl;
  outlauncher << "Error = log/CondorME_" << ScriptNameSuffix << "_C$(Cluster)_$(Process).stderr" << endl;
  outlauncher << "Log = log/CondorME_" << ScriptNameSuffix << "_C$(Cluster)_$(Process).log" << endl;
  outlauncher << "notify_user = none" << endl;
  outlauncher << "Arguments = $(Process) " << NEvtsPerJob << endl;
  outlauncher << "Queue " << NJobs << endl;

  /// Make the internal script
  outscript << "#! /bin/csh" << endl;
  outscript << "echo \"Starting\" " << endl;
  outscript << "cd /uscms/home/ilyao/MATRIXELEMENT/CMSSW_5_3_2_patch4/src" << endl;//***SET DEPENDENT ON THE CMSSW RELEASE***
  outscript << "source /uscmst1/prod/sw/cms/cshrc uaf" << endl;
  outscript << "pwd" << endl;
  //  outscript << "cmsenv" << endl;
  outscript << "eval `scram runtime -csh` " << endl;
  outscript << "cd -" << endl;
  outscript << "@ JobNumber = $argv[1]" << endl;
  outscript << "echo \"JobNumber=$JobNumber\"" << endl;
  outscript << "@ NEvtsPerJob = $argv[2]" << endl;
  outscript << "echo \"NEvtsPerJob=$NEvtsPerJob\"" << endl;
  outscript << "@ TheJob = " << FirstJob << " + $JobNumber" << endl;

  if ( createDisjointJobScript ) {
    outscript << "###### START: Explicitly Setting Starting Job Number ######" << endl;
    int nJob=0;
    TString nj;
    TString temp="temp";
    istrstream djstr(disjointJobsString);
    while ( temp!="" ) {
      djstr >> nj >> temp;
      outscript << "if ( $JobNumber == " << nJob << " ) then" << endl;
      outscript << "   @ TheJob = " << nj << endl;
      outscript << "endif" << endl;
      nJob++;
    }

    if ( nJob!=NJobs ) { cerr << "Error: the job count in the disjointJobsString does not correspond to NJobs" << endl; }
    outscript << "###### FINISH: Explicitly Setting Starting Job Number ######" << endl;
  }
  //  outscript << "@ StartEvt = $argv[2] * $argv[1] + " << FirstEvt_C << endl;
  // outscript << "echo \"StartEvt=NEvts*Process+" << FirstEvt_C << "=$StartEvt\"" << endl;
  // outscript << "@ StartJob = $argv[1] + " << FirstJob_C << endl;

  outscript << "echo \"TheJob=$TheJob\"" << endl;
  outscript << "@ StartEvt = $TheJob * $NEvtsPerJob" << endl;
  outscript << "echo \"StartEvt=$StartEvt\"" << endl;


  //  outscript << "run_MatrixElement " << rootInputName << " " << OutputName << "$StartJob.root EvtTree " << NEvtsPerJob_C << " $StartEvt 1" << endl;

  outscript << "run_MatrixElement " << rootInputName << " " << OutputName << "$TheJob.root jets2p $NEvtsPerJob $StartEvt 1 1 PS" << endl;
  outscript << "echo \"Finished\"" << endl;

  tempStr="chmod +x " + tempStr;
  system(tempStr);  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void CheckCONDOROutput(const char* inFilePrefix, TString & invalidFileStr, int & nInvalidFiles, int cntBeg, int cntEnd, bool checkMETreePresence=true, bool suppressoutput=false) {
//// Checks if files of the form inFilePrefixCNT.root (CNT=cntBeg,...,cntEnd) are present and returns the string of missing files.
  TString Command, infilename, sstr;
  char line[500];
  char tempCnt[5];
  int nMissingFiles=0;
  int nEmptyFiles=0;
  bool emptyTree=true;
  //cout << "cntBeg=" << cntBeg << " cntEnd=" << cntEnd << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    infilename=".root";
    sprintf(tempCnt,"%i",i);
    infilename=tempCnt+infilename;
    infilename=inFilePrefix+infilename;
    Command=" >& tempErr.txt";
    Command=infilename+Command;
    Command="ls "+Command;
    //cout << "Command= " << Command << endl;
    system(Command);
    ifstream infile("tempErr.txt");
    infile.getline(line,500);
    sstr = line;
    if (sstr.Contains("No such file or directory") ) {
      ///We're missing the ith file
      if ( invalidFileStr!="" ) {
	invalidFileStr=invalidFileStr+" , ";
      }
      invalidFileStr=invalidFileStr+tempCnt;
      nMissingFiles++;
    } else { 
      if ( checkMETreePresence ) {
	//cout << "1" << endl;
	TFile* testfile = new TFile(infilename);
	//cout << "2" << endl;
	TTree* TestTree = (TTree*)testfile->Get("METree");
	//cout << "3" << endl;
	emptyTree=true;
	if (TestTree) {
	  emptyTree=false;
	}
	delete TestTree;
	testfile->Close();
	delete testfile;
	//cout << "4" << endl;
	if ( emptyTree ) {
	  //cout << "Empty File" << endl;
	  ///The i'th file is empty
	  if ( invalidFileStr!="" ) {
	    invalidFileStr=invalidFileStr+" , ";
	  }
	  invalidFileStr=invalidFileStr+tempCnt;
	  nEmptyFiles++;
	}
      }      
    }

  }
  if  ( !suppressoutput ) {
    cout << "NMissing=" << nMissingFiles;
    if ( checkMETreePresence ) { cout << ", NEmpty=" << nEmptyFiles; }
    cout << " | invalidfiles : " <<  invalidFileStr << endl;
  }
  nInvalidFiles=nMissingFiles+nEmptyFiles;

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void CreateMultipleCondorScripts_runME(const char* inputmode, const char* ScriptDir, const char* rootInputDir, int StartProcess, int EndProcess, int NEvtsPerJob, bool checkCompletedJobsInstead=false, const char* completedOutputDir="", bool createCompletionScripts = false)
//// Calls CreateCondorScript_runME for each process, with the option of running on JES shifted files as well (runOnJESShifted=true).
//// ScriptName=LME+PLabel+suffix, OutputName=PLabel+ME+suffix; where suffix="","JESp1s","JESm1s"
//// rootInputName=inputmode+PLabel+filenameSuffix
//// Set checkCompletedJobsInstead=true to check completed jobs (in the directory completedOutputDir) and set createCompletionScripts=true to create scripts to rerun the missing jobs.
{
  //bool runOnJESShifted=false;//obsolete option
  TString filenameSuffix=".root";
  bool insideTDirectoryFile = true;
  TString tdirectoryFileName = "PS";
  TString scriptBaseName,rootinputBaseName,outputBaseName;
  int nJobs,nEvtsPerJob;
  double nEntries;
  InitializeLabels(PLabel,CLabel);

  for (Int_t np=StartProcess; np<(EndProcess+1);np++) {
    cout << "Process=" << PLabel[np] << endl;
    scriptBaseName=PLabel[np];
    scriptBaseName=inputmode+scriptBaseName;
    rootinputBaseName=inputmode;
    rootinputBaseName=PLabel[np]+rootinputBaseName;
    outputBaseName=inputmode;
    outputBaseName=PLabel[np]+outputBaseName;

    TFile* infile = new TFile(rootInputDir+rootinputBaseName+filenameSuffix);
    TTree* InTree;

    if ( !insideTDirectoryFile ) {
      InTree = (TTree*)infile->Get("jets2p");
    } else {
      TDirectoryFile *tdf = (TDirectoryFile*)infile->Get(tdirectoryFileName);
      InTree = (TTree*)tdf->Get("jets2p");
    }

    nEntries=InTree->GetEntries();
    cout << "nEntries=" << nEntries ;
    nJobs=ceil(nEntries/NEvtsPerJob);
    nEvtsPerJob=NEvtsPerJob;
//     //when the number of events is less than number of events in a single job, run on all of the events
//     if ( nJobs==1 ) {
//       nEvtsPerJob=nEntries;
//     }

    cout << ", nJobs=" << nJobs << endl;
    // cout << "CreateCondorScript_runME(" << ScriptDir << "," << scriptBaseName << "" << "," << rootInputDir << "," << rootinputBaseName << filenameSuffix << "," << outputBaseName << "DEF" << ",0," << nEvtsPerJob << ",0," << nJobs << ");" << endl;

    if ( !checkCompletedJobsInstead ) {
      CreateCondorScript_runME(ScriptDir,scriptBaseName,rootInputDir,rootinputBaseName+filenameSuffix,outputBaseName,nEvtsPerJob,0,nJobs);
//       if ( runOnJESShifted ) {
// 	//obsolete:
// 	CreateCondorScript_runME(ScriptDir,scriptBaseName+"JESp1s",rootInputDir,rootinputBaseName+"_JESp1s.root",outputBaseName+"JESp1s",nEvtsPerJob,0,nJobs);
// 	CreateCondorScript_runME(ScriptDir,scriptBaseName+"JESm1s",rootInputDir,rootinputBaseName+"_JESm1s.root",outputBaseName+"JESm1s",nEvtsPerJob,0,nJobs);
//       }
    } else {
      //cout << "Checking Completed Jobs" << endl;
      TString infileprefix="";
      infileprefix=inputmode+infileprefix;
      infileprefix=PLabel[np]+infileprefix;
      infileprefix=completedOutputDir+infileprefix;
      //cout << "infileprefix=" << infileprefix << endl;
      TString invalidFileStr;
      int nInvalidFiles;

      CheckCONDOROutput(infileprefix,invalidFileStr,nInvalidFiles,0,nJobs-1,true,false);
      if ( createCompletionScripts && (nInvalidFiles>0) ) {
	cout << "Creating the completion script" << endl;
	CreateCondorScript_runME(ScriptDir,scriptBaseName+"Completion",rootInputDir,rootinputBaseName+filenameSuffix,outputBaseName,nEvtsPerJob,0,nInvalidFiles,true,invalidFileStr);
      }
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

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void GetEventCounts(const char* rootFilesDir, const char* treeName = "jets2p", bool insideTDirectoryFile = true, const char* tdirectoryFileName="PS", int divideby=300)
////Counts the number of events for each file in rootFilesDir
////The events should be inside the tree 'treeName', which can be inside TDirectoryFile 'tdirectoryFileName'
////If divideby!=-1, then print out the number of events divided by 'divideby'
{

  TString Command, filename;
  char line[500];

  ///Store the subdirectory names
  Command = "*.root >& templog.txt";
  Command = rootFilesDir + Command;
  Command = "ls " + Command;
  cout << "Command=" << Command << endl;
  system(Command);
  ifstream infilelist("templog.txt");
  while ( infilelist.good() ) {
    infilelist.getline(line,500);
    filename = line;
    TFile* f;
    if ( filename!="" ) {
      f = new TFile(filename);
    }
    TTree* InTree;
    if ( f!=0 ) {
      cout << "filename=" << filename << endl;
      if ( !insideTDirectoryFile ) {
	InTree = (TTree*)f->Get(treeName);
      } else {
	TDirectoryFile *tdf = (TDirectoryFile*)f->Get(tdirectoryFileName);
	InTree = (TTree*)tdf->Get(treeName);
      }
      if ( InTree!=0 ) {
	double nentries=InTree->GetEntries();
	cout << nentries << " Events" << endl;
	if ( divideby!=-1 ) {
	  cout << nentries/divideby << " Groups" << endl;
	}
      } else {
	cout << "Unable to open tree : " << treeName << endl;
      }
    }
  }

}
