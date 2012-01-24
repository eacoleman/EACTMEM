// /////////////////////////////////////////////////////////////////////////////////////////////////
// //// CMS 
// //// Matrix Element Analysis
// //// Created by Osipenkov, Ilya : ilyao@fnal.gov
// /////////////////////////////////////////////////////////////////////////////////////////////////
// //// A tool for working with the .root files created by CRAB
// /////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <strstream>
//#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include <TString.h>
#include <TChain.h>

void ManipulateCRABOutput () {
}


//**--------------------------------------------------------------------------------------------------------------------------------**//


void CheckCRABOutput(const char* inFileName, TString& missingfiles, int cntBeg, int cntEnd, bool suppressoutput) {
//// Checks if files of the form inFileNameCNT.root, with CNT=cntBeg-cntEnd, are present, and returns the string of missing files.
  TString Command, sstr;
  char line[500];
  char tempCnt[5];
  missingfiles="";
  //cout << "cntBeg=" << cntBeg << " cntEnd=" << cntEnd << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    Command=".root >& tempErr.txt";
    sprintf(tempCnt,"%i",i);
    Command=tempCnt+Command;
    Command=inFileName+Command;
    Command="ls "+Command;
    //cout << "Command= " << Command << endl;
    system(Command);
    ifstream infile("tempErr.txt");
    infile.getline(line,500);
    sstr = line;
    if (sstr.Contains("No such file or directory") ) {
      ///We're missing the ith file
      if ( missingfiles!="" ) {
	missingfiles=missingfiles+" , ";
      }
      missingfiles=missingfiles+tempCnt;
    }
  }
  if  ( !suppressoutput ) {
    cout << "Missing The Following Files : " <<  missingfiles << endl;
  }
}


//**--------------------------------------------------------------------------------------------------------------------------------**//


void MoveCRABOutput(const char* inDirName, const char* outDirName, const char* FileName, int cntBeg, int cntEnd) {
//// Use to move the crab output & give it a sequential name.
//// Moves files inDirNameFileNameCNT_*.root to outDirNameFileNameCNT.root, where CNT=cntBeg-cntEnd. A list of files which could not be moved is provided
  TString Command, sstr;
  char line[200];
  char tempCnt[5];
  int notmoved=0;

  // cout << "FileName=" << FileName << endl;
  cout << "Unable To Move The Following Files:" << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    ///e.g. mv ZpJMG_35_*.root ZpJMG_35.root >& tempErr.txt
    Command=".root >& tempErr.txt";
    sprintf(tempCnt,"%i",i);
    Command=tempCnt+Command;
    Command=FileName+Command;
    //    Command="/"+Command;
    Command=outDirName+Command;
    Command="_*.root "+Command;
    Command=tempCnt+Command;
    Command=FileName+Command;
    //    Command="/"+Command;
    Command=inDirName+Command;
    Command="mv "+Command;
    //cout << "Command= " << Command << endl; 
    system(Command);
    ifstream infile("tempErr.txt");
    infile.getline(line,500);
    sstr = line;
    //cout << "Error Line= " << sstr << endl;


    if (sstr.Contains("No such file or directory") ) {
      ///We're missing the current the file number:
      cout << i << ",";
      notmoved++;
    }
    if ( sstr.Contains("is not a directory") ) {
      ///We have more than one match.
      //cout << "We Have a repeating file" << endl;
      Command="*.root >& tempErr.txt";
      Command=tempCnt+Command;
      Command=FileName+Command;
      //      Command="/"+Command;
      Command=inDirName+Command;
      Command="ls "+Command;
      system(Command);
      ifstream infile2("tempErr.txt");
      infile2.getline(line,500);
      sstr = line;
      /// sstr should be the first file name.
      Command=".root";
      Command=tempCnt+Command;
      Command=FileName+Command;
      //      Command="/"+Command;
      Command=outDirName+Command;
      Command=" "+Command;
      Command=sstr+Command;
      Command="mv "+Command;
      system(Command);
    }

  }
  cout << endl;
  cout << "Unable To Move " << notmoved << " files" << endl;

}


//**--------------------------------------------------------------------------------------------------------------------------------**//


void RenameCRABOutput(const char* inName, const char* outName, int cntBeg, int cntEnd, int offset=0) {
//// Use to rename a sequence of files (from CRAB output.
//// Moves file inNameCNT.root to outNameCNToffset.root, where CNT=cntBeg-cntEnd & CNToffset=CNT+offset. A list of files which could not be renamed is provided.
  TString Command, sstr;
  char line[200];
  char tempCnt[5], tempCntoffset[5];
  int notmoved=0;

  cout << "Unable To Rename The Following Files:" << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    ///e.g. mv ZpJMG_35.root ZpJMG_57.root >& tempErr.txt
    Command=".root >& tempErr.txt";
    sprintf(tempCnt,"%i",i);
    sprintf(tempCntoffset,"%i",i+offset);
    Command=tempCntoffset+Command;
    Command=outName+Command;
    Command=".root "+Command;
    Command=tempCnt+Command;
    Command=inName+Command;
    Command="mv "+Command;
    system(Command);
    //cout << "Command= " << Command << endl; 
    ifstream infile("tempErr.txt");
    infile.getline(line,500);
    sstr = line;
    //cout << "Error Line= " << sstr << endl;


    if (sstr.Contains("No such file or directory") ) {
      ///We're missing the current the file number:
      cout << i << ",";
      notmoved++;
    }

  }
  cout << endl;
  cout << "Unable To Move " << notmoved << " files" << endl;

}


//**--------------------------------------------------------------------------------------------------------------------------------**//

void SplitDir(const char* inDirName, int NPerDir=990) {
//// Use to split one directory into several (in order to overcome a limit on the number of open files)
  char logline[5000];
  char I_char[5];
  TString Command, targetDirName, filename;
  int Nmoved, Npt;


  Command=" >& filelist.txt";
  Command=inDirName + Command;
  Command="ls "+Command;
  cout << "Command=" << Command << endl;
  system(Command);

  ifstream inFileListFile("filelist.txt");
  Nmoved=0;
  Npt=0;

  while ( inFileListFile.good() ) {
    if (Nmoved==0) {
      Npt++;
      cout << "Creating part" << Npt << endl;
      sprintf(I_char,"%i",Npt);
      targetDirName=I_char;
      targetDirName="_pt"+targetDirName;
      targetDirName=inDirName+targetDirName;
      Command=targetDirName;
      Command="mkdir "+Command;
      cout << "Command=" << Command << endl;
      system(Command);
    }

    inFileListFile.getline(logline,5000);
    istrstream str(logline);
    str >> filename;
    if ( filename!="" ){
      Command=targetDirName;
      Command=" "+Command;
      Command=filename+Command;
      Command="/"+Command;
      Command=inDirName+Command;
      Command="mv "+Command;
      //cout << "Command=" << Command << endl;
      system(Command);
      Nmoved++;
    }
    if ( Nmoved>=NPerDir ) {
      Nmoved=0;
    }
  }

  Command=inDirName;
  Command="rmdir "+Command;
  cout << "Command=" << Command << endl;
  system(Command);

}


