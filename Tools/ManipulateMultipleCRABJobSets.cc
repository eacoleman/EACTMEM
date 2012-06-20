/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Tool For Working with a multiple sets of CRAB Jobs
//// initialize the proxy: voms-proxy-init -voms cms

///temporary fix
#include </uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/src/TAMUWW/Tools/ManipulateSingleCRABJobSet.cc>

//#include <stdlib> 
#include <iostream>
#include <iomanip>
#include <strstream>
//#include <ios>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include <TString.h>
#include <TChain.h>



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
// _nEvtsOrJobsTotal = 
// _nEvtsOrLumisPerJob =
// _nReps = 
// _nJobsPerPart =
// _pset = 
// _publishDataName =
// _availableAtFNAL =
// "-----------------------------------------------------------------------"

void CreateJobs_All(const char* allSetsLogName, bool createAtFNAL_ifPossible) 
//// Runs the standard sequence for all of the Sets
{
  char logline[500];
  TString label, token1, valstr, token3;
  int valint;
  bool createAtFNAL=false;
  TString setName,baseDir,dataset,lumimask,runselection,pset,publishDataName;
  int nEvtsPerJob,nEvtsTotal,nReps,nJobsPerPart;

  //TString endlog_checkString;
  ifstream allSetsLogFile(allSetsLogName);
//   allSetsLogFile.getline(logline,500);
//   cout << "logline = " << logline << endl;
  //allSetsLogFile.open():
  //cout << allSetsLogFile.good() << endl;

  while ( allSetsLogFile.good() ) {
    //while ( endlog_checkString!="ENDOFLOG" ) {
    //cout << "1" << endl;
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
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str2(logline);
      str2 >> label >> token1 >> valstr >> token3;
      if ( label=="_lumimask" ) {
	lumimask=valstr; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str3(logline);
      str3 >> label >> token1 >> valstr >> token3;
      if ( label=="_runselection" ) {
	runselection=valstr; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str4(logline);
      str4 >> label >> token1 >> valstr >> token3;
      if ( label=="_baseDir" ) {
	baseDir=valstr; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str5(logline);
      str5 >> label >> token1 >> valint >> token3;
      if ( label=="_nEvtsOrJobsTotal" ) {
	nEvtsTotal=valint; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str6(logline);
      str6 >> label >> token1 >> valint >> token3;
      if ( label=="_nEvtsOrLumisPerJob" ) {
	nEvtsPerJob=valint; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str7(logline);
      str7 >> label >> token1 >> valint >> token3;
      if ( label=="_nReps" ) {
	nReps=valint; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str8(logline);
      str8 >> label >> token1 >> valint >> token3;
      if ( label=="_nJobsPerPart" ) {
	nJobsPerPart=valint; 
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str9(logline);
      str9 >> label >> token1 >> valstr >> token3;
      if ( label=="_pset" ) {
	pset=valstr;
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str10(logline);
      str10 >> label >> token1 >> valstr >> token3;
      if ( label=="_publishDataName" ) {
	publishDataName=valstr;
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      allSetsLogFile.getline(logline,500);
      istrstream str11(logline);
      str11 >> label >> token1 >> valstr >> token3;
      if ( label=="_availableAtFNAL" ) {
	if ( createAtFNAL_ifPossible&&(valstr=="yes") ) {
	  createAtFNAL=true;
	} else {
	  createAtFNAL=false;
	}
      } else {
	cerr << "ERROR : Lines in the input file are not in proper order" << endl;
      }
      CreateJobs(setName,nEvtsPerJob,nEvtsTotal,baseDir,dataset,lumimask,runselection,nReps,nJobsPerPart,pset,publishDataName,createAtFNAL);
    } 
// else {
//       endlog_checkString=label;
//     }

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


void PreResubmissionCleanup_All(const char* allSetsLogName) 
//// Removes FullDir + "/res/" + CMSSW_N.stderr, CMSSW_N.stdout, crab_fjr_N.xml; where N=each job to resubmit
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      cout << " setName = " << setName << endl;
      PreResubmissionCleanup(setName);
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

void UpdateJobStatus_All(const char* allSetsLogName, bool printJobStatus=true) 
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
      if ( printJobStatus ) {
	cout << " setName = " << setName << endl;
      }
      UpdateJobStatus(setName,printJobStatus);
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

void PublishJobs_All(const char* allSetsLogName) 
//// Publishes all completed jobs for all sets
{
  char logline[500];
  TString label, token1, setName, token3;
  ifstream allSetsLogFile(allSetsLogName);
  while ( allSetsLogFile.good() ) {
    allSetsLogFile.getline(logline,500);
    istrstream str(logline);
    str >> label >> token1 >> setName >> token3;
    if ( label=="_setName" ) {
      cout << "setName = " << setName << endl;
      PublishJobs(setName);
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

void CleanUpJobs_All(const char* allSetsLogName, bool removeUnpDirs, bool removeLogs) 
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
      CleanUpJobs(setName,removeUnpDirs,removeLogs);
    }
  }
}

void HADDJobsInADirectory(const char* dirName, const char* filePrefix) 
//// Do hadd for the jobs in all subdirectories of a given directory. Bypasses the 1K file limit (by creating subdirectories and adding files in them).
{

  TString Command, partialDir;
  char line[500];
  char I_char[5];
  vector <TString> subdirectoryName;
  subdirectoryName.clear();
  int Nsubdirs=0;
  int Nfiles, Nparts;

  // Clean up dirName
  Command="*";
  Command=dirName+Command;
  Command="rm "+Command;
  cout << "Command=" << Command << endl;
  system(Command);

  ///Store the subdirectory names
  Command = " >& dirNames.txt";
  Command = dirName + Command;
  Command = "ls " + Command;
  cout << "Command=" << Command << endl;
  system(Command);
  ifstream infiledirs("dirNames.txt");
  while ( infiledirs.good() ) {
    infiledirs.getline(line,500);
    //cout << "line=[" << line << "]" << endl;
    subdirectoryName.push_back(line);
    Nsubdirs++;
  }
  Nsubdirs=Nsubdirs-1;

  for (Int_t i=0; i<Nsubdirs;i++) {
    Command = "*.root >& fileNames.txt";
    Command=filePrefix+Command;
    Command="/"+Command;
    Command=subdirectoryName[i]+Command;
    Command=dirName+Command;
    Command="ls " + Command;
    cout << "Command=" << Command << endl;
    system(Command);
    ifstream infn("fileNames.txt");
    Nfiles=0;
    Nparts=0;
    while ( infn.good() ) {
      infn.getline(line,500);
      Nfiles++;
      //If we go over 900 files start moving to a (new) partial dir
      if ( Nfiles>900 ) {
	Nparts++;
	Nfiles=0;
	sprintf(I_char,"%i",Nparts);
	partialDir=I_char;
	partialDir="_pt"+partialDir;
	partialDir=subdirectoryName[i]+partialDir;
	partialDir=dirName+partialDir;
	Command="mkdir "+partialDir;
	cout << "Command=" << Command << endl;
	system(Command);
      }
      if ( Nparts>0 ) {
	Command=" " + partialDir;
	Command=line+Command;
	//Command=dirName+Command;
	Command="mv "+Command;
	cout << "Command=" << Command << endl;
	system(Command);
      }
    }
    if ( Nparts>0 ) {
      sprintf(I_char,"%i",0);
      partialDir=I_char;
      partialDir="_pt"+partialDir;
      partialDir=subdirectoryName[i]+partialDir;
      partialDir=dirName+partialDir;
      Command=" " + partialDir;
      Command=subdirectoryName[i]+Command;
      Command=dirName+Command;
      Command="mv "+Command;
      cout << "Command=" << Command << endl;
      system(Command);
    }
    //Perform the hadd
    if ( Nparts==0 ) {
      //hadd ${n}_CMSSW428.root ${n}/MC428*.root
      Command="*.root";
      Command=filePrefix+Command;
      Command="/"+Command;
      Command=subdirectoryName[i]+Command;
      Command=dirName+Command;
      Command=".root "+Command;
      Command=subdirectoryName[i]+Command;
      Command=dirName+Command;
      Command="hadd "+Command;
      cout << "Command=" << Command << endl;
      system(Command);
    } else {
      //hadd the partial dirs
      cout << "Adding the Files in the subdirectories" << endl;
      for ( Int_t j=0; j<(Nparts+1);j++ ) {
	sprintf(I_char,"%i",j);
	partialDir=I_char;
	partialDir="_pt"+partialDir;
	partialDir=subdirectoryName[i]+partialDir;
	partialDir=dirName+partialDir;
	Command="*.root";
	Command=filePrefix+Command;
	Command="/"+Command;
	Command=partialDir+Command;
	Command="_CMSSW428.root "+Command;
	Command=I_char+Command;
	Command="_pt"+Command;
	Command=subdirectoryName[i]+Command;
	Command=dirName+Command;
	Command="hadd "+Command;
	cout << "Command=" << Command << endl;
	system(Command);
      }
      //hadd the partial files
      cout << "Adding the partial files" << endl;
      Command="_pt*.root";
      Command=subdirectoryName[i]+Command;
      Command=dirName+Command;
      Command="_CMSSW428.root "+Command;
      Command=subdirectoryName[i]+Command;
      Command=dirName+Command;
      Command="hadd "+Command;
      cout << "Command=" << Command << endl;
      system(Command);
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


