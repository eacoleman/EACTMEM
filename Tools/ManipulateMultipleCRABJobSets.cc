/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Tool For Working with a single set of CRAB Jobs
//// initialize the proxy: voms-proxy-init -voms cms


//#include <stdlib> 
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

#include <../../Tools/ManipulateSingleCRABJobSet.cc>

void ManipulateMultipleCRABJobSets () {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    Main Functions  ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /// the allSetsLog is of the form
// "-----------------------------------------------------------------------"
// _setName = 
// _dataset =
// _lumimask = 
// _runselection = 
// _baseDir =
// _nEvtsTotal = 
// _nEvtsPerJob =
// _nReps = 
// _nJobsPerPart =
// _availableAtFNAL =
// "-----------------------------------------------------------------------"

void CreateJobs_All(const char* allSetsLogName, bool createAtFNAL_ifPossible) 
//// Runs the standard sequence for all of the Sets
{
  char logline[500];
  TString label, token1, valstr, token3;
  int valint;
  bool createAtFNAL=false;
  TString setName,baseDir,dataset,lumimask,runselection;
  int nEvtsPerJob,nEvtsTotal,nReps,nJobsPerPart;
  ifstream allSetsLogFile(allSetsLogName);

  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str0(logline);
    str0 >> label >> token1 >> valstr >> token3;
    if ( label=="_setName" ) {
      setName=valstr;
      allSetsLogFile.getline(logline,500);
      istrstream str1(logline);
      str1 >> label >> token1 >> valstr >> token3;
      if ( label=="_dataset" ) {
	dataset=valstr; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str2(logline);
      str2 >> label >> token1 >> valstr >> token3;
      if ( label=="_lumimask" ) {
	lumimask=valstr; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str3(logline);
      str3 >> label >> token1 >> valstr >> token3;
      if ( label=="_runselection" ) {
	runselection=valstr; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str4(logline);
      str4 >> label >> token1 >> valstr >> token3;
      if ( label=="_baseDir" ) {
	baseDir=valstr; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str5(logline);
      str5 >> label >> token1 >> valint >> token3;
      if ( label=="_nEvtsTotal" ) {
	nEvtsTotal=valint; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str6(logline);
      str6 >> label >> token1 >> valint >> token3;
      if ( label=="_nEvtsPerJob" ) {
	nEvtsPerJob=valint; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str7(logline);
      str7 >> label >> token1 >> valint >> token3;
      if ( label=="_nReps" ) {
	nReps=valint; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str8(logline);
      str8 >> label >> token1 >> valint >> token3;
      if ( label=="_nJobsPerPart" ) {
	nJobsPerPart=valint; 
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str9(logline);
      str9 >> label >> token1 >> valstr >> token3;
      if ( label=="_availableAtFNAL" ) {
	if ( createAtFNAL_ifPossible&&(valstr=="yes") ) {
	  createAtFNAL=true;
	} else {
	  createAtFNAL=false;
	}
      } else {
	cerr << "ERROR : Lines in the log file are not in proper order" << endl;
      }
      CreateJobs(setName,nEvtsPerJob,nEvtsTotal,baseDir,dataset,lumimask,runselection,nReps,nJobsPerPart,createAtFNAL);
    }
  }
}

void StartJobs_All(const char* allSetsLogName) 
//// Starts the jobs for all of the sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      //cout << "_setName=" << setName << endl;
      StartJobs(setName);
    }
  }
}

void ResubmitJobs_All(const char* allSetsLogName, bool resubmitAllUnfinished) 
//// Resubmits the jobs already scheduled as in need of resubmisssion or all unfinished jobs for each set
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      ResubmitJobs(setName,resubmitAllUnfinished);
    }
  }
}

void UpdateJobStatus_All(const char* allSetsLogName) 
//// Updates the status for all of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      UpdateJobStatus(setName);
    }
  }
}

void ExtractJobs_All(const char* allSetsLogName, bool extractEverything) 
//// Extracts the information of the jobs (.root files are still kept at the FNAL dcache directory) to a /res directory for all of the jobs
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      ExtractJobs(setName,extractEverything);
    }
  }
}

void CopyJobs_All(const char* allSetsLogName) 
//// Copies data from storage to a /res directory for all sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      CopyJobs(setName);
    }
  }
}

void DirectCopy_All(const char* allSetsLogName) 
//// Directly (i.e. without the use of CRAB) copies all of the output for all of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      DirectCopy(setName);
    }
  }
}

void DirectRemove_All(const char* allSetsLogName) 
//// Directly (i.e. without relying on CRAB) removes the files in the user_remote_dir for each of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      DirectRemove(setName);
    }
  }
}

void MoveJobs_All(const char* allSetsLogName) 
//// Moves the files from res to ProcessedDir for each of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      cout << "SetName = " << setName << endl;
      MoveJobs(setName);
    }
  }
}

void RunStandardSequence_All(const char* allSetsLogName, bool resubmitAllUnfinished, bool extract) 
//// Runs the standard sequence for all of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      RunStandardSequence(setName,resubmitAllUnfinished,extract);
    }
  }
}

void CheckProcessedJobs_All(const char* allSetsLogName) 
//// Checks which jobs have already been processed (& overwrites the JobsUnfinished string) for all of the Sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      cout << "SetName = " << setName << endl;
      CheckProcessedJobs(setName);
    }
  }
}

void KillJobs_All(const char* allSetsLogName) 
//// Kills all jobs for all sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      KillJobs(setName);
    }
  }
}

void CleanUpJobs_All(const char* allSetsLogName, bool removeAll ) 
//// Cleans up the storage space for all of the sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      CleanUpJobs(setName,removeAll);
    }
  }
}


// void _All(const char* allSetsLogName) 
// //// Runs the standard sequence for all of the Sets
// {
//   char logline[500];
//   TString label, token1, setName, token3;
//   ifstream allSetsLogFile(allSetsLogName);
//   while ( allSetsLogFile.good() ) {
//     allSetsLogFile.getline(logline,500);
//     istrstream str(logline);
//     str >> label >> token1 >> setName >> token3;
//     if ( label=="_setName" ) {
      
//     }
//   }
// }


