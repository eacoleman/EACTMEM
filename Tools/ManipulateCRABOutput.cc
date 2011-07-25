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

// void CompensateForGaps(const char* inDirName, const char* baseName, const char* outName, int cntBeg, int cntEnd, int offset=0)
// //// Input files should be of the form baseName_CNT.root (in the inDirName directory), where CNT=cntBeg-cntEnd. The code moves files at the end to replace the missing ones, checks the logs for how many events are omitted and stores the information in compensationlog.log (in the inDirName directory).
// {
//   vector <int> FilePresent;
//   FilePresent.clear();

//   int Npresent=0;
//   int Nmissing=0;
//   int Nmissing_beforelast=0;//
//   TString Command, sstr;
//   char line[200];
//   char tempCnt[5], tempCntoffset[5];
//   int notmoved=0;

//   //Fill the FilePresent array
//   for (Int_t i=0; i<cntBeg;i++) {
//     FilePresent.push-1;
//   }
//   for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
//     ///e.g. mv ZpJMG_35.root ZpJMG_57.root >& tempErr.txt
//     Command=".root >& tempErr.txt";
//     sprintf(tempCnt,"%i",i);
//     sprintf(tempCntoffset,"%i",i+offset);
//     Command=tempCntoffset+Command;
//     Command=outName+Command;
//     Command=".root "+Command;
//     Command=tempCnt+Command;
//     Command=inName+Command;
//     Command="mv "+Command;
//     system(Command);
//     //cout << "Command= " << Command << endl; 
//     ifstream infile("tempErr.txt");
//     infile.getline(line,500);
//     sstr = line;
//     //cout << "Error Line= " << sstr << endl;


//     if (sstr.Contains("No such file or directory") ) {
//       ///We're missing the current the file number:
//       cout << i << ",";
//       notmoved++;
//     }

//   }
//   cout << endl;
//   cout << "Unable To Move " << notmoved << " files" << endl;

// }


