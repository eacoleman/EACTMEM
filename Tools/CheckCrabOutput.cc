/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Tools for skimming large amounts of PATNtuples and generating tables summarizing the results.

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



void CheckCRABOutput(const char* inFileName, int cntBeg, int cntEnd) {
//// Checks if files of the form inDirName/inMameCNT.root, with CNT=cntBeg-cntEnd, are present.
  TString Command, sstr;
  char line[500];
  char tempCnt[5];
  //bool getnextline=true;
//   LaunchStr="*.root > rootnames.txt";
//   LaunchStr=inFileName+LaunchStr;
//   LaunchStr="/"+LaunchStr;
//   LaunchStr=inDirName+LaunchStr;
//   LaunchStr="ls "+LaunchStr;
//   system(LaunchStr);

//  ifstream infile("rootnames.txt");
  cout << "Missing The Following Files:" << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    Command=".root >& tempErr.txt";
    sprintf(tempCnt,"%i",i);
    Command=tempCnt+Command;
    Command=inFileName+Command;
    Command="ls "+Command;
    system(Command);
    //cout << "Command= " << Command << endl;
    ifstream infile("tempErr.txt");
    infile.getline(line,500);
    sstr = line;

    if (sstr.Contains("No such file or directory") ) {
      ///We're missing the current the file number:
      cout << i << ",";
    }
//     if ( !infile.good() ) {
//       cout << "ERROR: Went through all file names" << endl;
//       i=cntEnd+2;
//     }
  }
  cout << endl;
  
}

void MoveCRABOutput(const char* inDirName, const char* outDirName, const char* FileName, int cntBeg, int cntEnd) {
//// Moves files inDirName/FileNameCNT_*.root to outDirName/FileNameCNT.root, where CNT=cntBeg-cntEnd. A list of files which could not be moved is provided
  TString Command, sstr;
  char line[200];
  char tempCnt[5];
  int notmoved=0;

  cout << "Unable To Move The Following Files:" << endl;
  for (Int_t i=cntBeg; i<(cntEnd+1);i++) {
    ///e.g. mv ZpJMG_35_*.root ZpJMG_35.root >& tempErr.txt
    Command=".root >& tempErr.txt";
    sprintf(tempCnt,"%i",i);
    Command=tempCnt+Command;
    Command=FileName+Command;
    Command="/"+Command;
    Command=outDirName+Command;
    Command="_*.root "+Command;
    Command=tempCnt+Command;
    Command=FileName+Command;
    Command="/"+Command;
    Command=inDirName+Command;
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
    if ( sstr.Contains("is not a directory") ) {
      ///We have more than one match.
      //cout << "We Have a repeating file" << endl;
      Command="*.root >& tempErr.txt";
      Command=tempCnt+Command;
      Command=FileName+Command;
      Command="/"+Command;
      Command=inDirName+Command;
      Command="ls "+Command;
      system(Command);
      ifstream infile("tempErr.txt");
      infile.getline(line,500);
      sstr = line;
      /// sstr should be the first file name.
      Command=".root";
      Command=tempCnt+Command;
      Command=FileName+Command;
      Command="/"+Command;
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
