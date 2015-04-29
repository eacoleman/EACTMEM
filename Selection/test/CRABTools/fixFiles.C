#include "TString.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void fixFiles(char* ifile, int nFiles, int highestNum, char* goodOutFile, char* badOutFile, TString specialString, bool verbose = 0){
   ifstream inFile;

   TString tmpStr;

   ofstream oStreamGood;
   ofstream oStreamBad;
   oStreamGood.open(goodOutFile);
   oStreamBad.open(badOutFile);

   vector< vector<TString> > files;
   vector<TString> holder;
   files.clear();
   holder.clear();
   for(int i = 0; i<highestNum; i++) files.push_back(holder);

   vector<bool> gotThisFile;
   gotThisFile.clear();
   for(int i = 0; i<highestNum; i++) gotThisFile.push_back(0);
   
   if(verbose) cout<<"specialString = "<<specialString<<endl;

   inFile.open(ifile);

   int tmpIndex;

   for(int i=1; i<=nFiles; i++){
      inFile >> tmpStr;
 
      tmpIndex = tmpStr.Index(specialString) + specialString.Length();
      
      TString tmpNum;
      char tmpChar;
      int count=0;

      while(1){
         if(verbose) cout<<"getting char "<<tmpIndex+count<<endl;
         tmpChar = tmpStr[tmpIndex+count];          
         if(tmpChar!='_'){
            tmpNum+=tmpChar;
            count++;
         }
         else break;
      }

      if(verbose){
         cout<<"tmpNum = "<<tmpNum<<endl;      
         cout<<"files["<<tmpNum.Atoi()-1<<"].push_back("<<tmpStr<<")"<<endl;
      }

      files[tmpNum.Atoi()-1].push_back(tmpStr);
      gotThisFile[tmpNum.Atoi() - 1] = 1;
   }		

   for(int i = 0; i<highestNum; i++){
      if(gotThisFile[i]){
         for(int j = 0; j<files[i].size()-1; j++) {
            if(verbose) cout<<"writing "<<i+1<<"_"<<j+1<<"..."<<endl; 
            oStreamBad<<files[i][j]<<endl;
         }
         if(verbose) cout<<"writing "<<i+1<<"_"<<files[i].size()<<"..."<<endl; 
         oStreamGood<<files[i][files[i].size()-1]<<endl;
      }
   }

   oStreamGood.close();
   oStreamBad.close();
   inFile.close();
}

