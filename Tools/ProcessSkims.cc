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

// User defined variables and functions:
#include <TAMUWW/Tools/GlobalTools.cc>


void ProcessPATSubset(const char* Label, const char* inName, const char* outRootName, const char* outTxtName, int cntStart, int cntEnd, const char * executable) {
//// Process a subset of PAT files to generate a .txt Selection (sub) file. The PAT files should be of the form inName+cnt+".root" (with cnt=cntStart to cntEnd)
  TString Lstr;
  char tempC[6];
  //Lstr="/uscms/home/ilyao/MATRIXELEMENT/CMSSW_3_5_4/bin/slc5_ia32_gcc434/PerformSelection.exe inputFiles=";
  Lstr=" inputFiles=";
  Lstr=executable+Lstr;

  for (Int_t i=cntStart; i<(cntEnd+1);i++) {
    Lstr=Lstr+inName;
    sprintf(tempC,"%i",i);
    Lstr=Lstr+tempC;
    if ( i==cntEnd ){
      Lstr=Lstr+".root";
    } else {
      Lstr=Lstr+".root,";
    }
  }
  Lstr=Lstr+" \\outputEvery=10000 ";
  Lstr=Lstr+outRootName;
  Lstr=Lstr+" ";
  Lstr=Lstr+outTxtName;
  Lstr=Lstr+" ";
  Lstr=Lstr+Label;
  //cout << "Lstr=" << Lstr << endl;
  system(Lstr);
  
}

void ProcessPATSet(const char* Label, const char* inPATName, const char* inRootName, const char* inTxtName, const char* outRootName, const char* outTxtName, int NStart, int NEnd, int stepSize, const char * executable) {
//// Process a set of either PAT files or already processed .txt & .root files to generate a combined .txt (& .root) file for the whole set. The PAT files should be of the form inName+cnt+".root" (with cnt=NStart to NEnd).
///  stepSize=-1 will not process the PAT tuples, but will run over the .txt files (provided they exist). stepSize=-2 merges the .root files as well.
/// executable gives location of the selection excecutable (e.g. /uscms/home/ilyao/MATRIXELEMENT/CMSSW_3_6_3/bin/slc5_ia32_gcc434/PerformSelection.exe )
  int cnt=0;
  if (stepSize<0) {
    cnt=NStart-1;
  }
  char cntC[6];
  bool cont=true;
  int Nbeg=NStart;
  int Nfin=0;
  int step;
  if ( stepSize<0 ) {
    step=1;
  } else {
    step=stepSize;
  }

  TString pRootName;
  TString pTxtName;

  double pTableEltemp[NCUTS][NJETS];
  double pTableMutemp[NCUTS][NJETS];
  double pTableLptemp[NCUTS][NJETS];
  double pTableEl[NCUTS][NJETS];
  double pTableMu[NCUTS][NJETS];
  double pTableLp[NCUTS][NJETS];
  InitializeMatrix(pTableEl);
  InitializeMatrix(pTableMu);
  InitializeMatrix(pTableLp);
  InitializeLabels(PLabel,CLabel);

  TChain ch("EvtTree");

  while ( cont==true ) {
    Nfin=Nbeg+step-1;
    if ( Nfin>=NEnd ) {
      //we have reached the end of the files to process
      Nfin=NEnd;
      cont=false;
    }
    cnt++;
    sprintf(cntC,"%i",cnt);
    pTxtName=".txt";
    pTxtName=cntC+pTxtName;
    pTxtName="_pt"+pTxtName;
    pTxtName=inTxtName+pTxtName;
    pRootName=".root";
    pRootName=cntC+pRootName;
    pRootName="_pt"+pRootName;
    pRootName=inRootName+pRootName;

    if ( stepSize>0) {
      ProcessPATSubset(Label,inPATName,pRootName,pTxtName,Nbeg,Nfin,executable);
    }

    InitializeLabels(PLabel,CLabel);

    ///Reaad the .txt sub-tables and record the information
    readProcessTable(pTxtName,pTableEltemp,pTableMutemp,pTableLptemp);
    AddMatrices(pTableEl,pTableEltemp);
    AddMatrices(pTableMu,pTableMutemp);
    AddMatrices(pTableLp,pTableLptemp);
    ch.Add(pRootName);

    Nbeg=Nfin+1;
  }

  ofstream outTableFile;
  outTableFile.open(outTxtName,ios::out);
  writeProcessTable(outTableFile,pTableEl,pTableMu,pTableLp);
  
  ch.Merge(outRootName);

}


void ProcessSkims()
{

}

///////////////////////////////////////////////////////////////////////////////////////
//////   Create the .tex tables  ////
///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//////  Make a Table for a single process:
//////////////////////////////////////////////////////

void writeJetRow(ofstream& outTableFile, TString Label, double Row[NJETS]) {
  outTableFile << Label ;
  for (Int_t k=0; k<NJETS;k++) { 
   outTableFile << "  &  " << Row[k];
  }
  outTableFile << "  \\\\ "<< endl;
}


void MakeProcessTable(const char* outTableFileName, const char* inselFileName, const char* inXSecFile, int processNumber)
//// Produce the selection table given the XSections and selection results for the relevant processes.
/// Use processNumber to read in the relevant information.

{

  // The main tables. Rows: Cuts. Columns: Jets.
  double EvtTableEl[NCUTS][NJETS]; 
  double EvtTableMu[NCUTS][NJETS];
  double EvtTableLp[NCUTS][NJETS];

  double FracPassEl[NJETS];
  double FracPassMu[NJETS];
  double FracPassLp[NJETS];
  //Number of expected events per pb^-1.
  double NExpectedEl[NJETS];
  double NExpectedMu[NJETS];
  double NExpectedLp[NJETS];
  double pEvents; //The number of original MC events.

  ofstream outTableFile;
  outTableFile.open(outTableFileName,ios::out);

  InitializeLabels(PLabel,CLabel);

  readProcessTable(inselFileName,EvtTableEl,EvtTableMu,EvtTableLp);
  computeProcessResults(inXSecFile,processNumber,EvtTableEl,EvtTableMu,EvtTableLp,FracPassEl,FracPassMu,FracPassLp,NExpectedEl,NExpectedMu,NExpectedLp,pEvents);


  //// Make the table(s)
  // open the document
  outTableFile << "\\documentclass[12pt]{article}" << endl;
  outTableFile << "\\usepackage{fullpage}" << endl;
  outTableFile << "\\addtolength{\\topmargin}{-.8in}" << endl;
  outTableFile << "\\begin{document}" << endl;
  outTableFile << " " << endl;

  //Electrons   
  outTableFile << "\\begin{table}[t]" << endl;
  outTableFile << "\\begin{center}" << endl;
  outTableFile << "\\caption{ ELECTRONS: ";
  outTableFile << pEvents << " " << PLabel[processNumber] << " Events";
  outTableFile << "}" << endl;
  outTableFile << "\\begin{tabular}{|l|c|c|c|c|l|}" << endl;
  outTableFile << "\\hline " << endl;
  outTableFile << " Cut (El/Mu) // NJets " ;
  for (Int_t k=0; k<NJETS;k++) { 
    outTableFile << "  &  " << k;
  }
  outTableFile << "+" << "  \\\\ "<< endl;
  outTableFile << "\\hline "<< endl;
  for (Int_t j=0; j<(FINALCUT+1);j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableEl[j]);
  }
  
  //add the BTags
  outTableFile << "\\hline " << endl;
  for (Int_t j=(FINALCUT+1); j<NCUTS;j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableEl[j]);
  }
  
  //add the Process Results
  outTableFile << "\\hline "<< endl;
  writeJetRow(outTableFile, "FracPass", FracPassEl);
  writeJetRow(outTableFile, " $Evts/pb^-1$ ", NExpectedEl);
  outTableFile << "\\hline " << endl;
  // close the table
  outTableFile << "\\hline " << endl;
  outTableFile << "\\end{tabular}" << endl;
  outTableFile << "\\end{center}" << endl;
  outTableFile << "\\end{table}" << endl;
  outTableFile << " " << endl;
 

  //Muons
  outTableFile << "\\begin{table}[b]" << endl;
  outTableFile << "\\begin{center}" << endl;
  outTableFile << "\\caption{ MUONS: ";
  outTableFile << pEvents << " " << PLabel[processNumber] << " Events";
  outTableFile << "}" << endl;
  outTableFile << "\\begin{tabular}{|l|c|c|c|c|l|}" << endl;
  outTableFile << "\\hline " << endl;
  outTableFile << " Cut/NJets " ;
  for (Int_t k=0; k<NJETS;k++) { 
    outTableFile << "  &  " << k;
  }
  outTableFile << "+" << "  \\\\ "<< endl;
  outTableFile << "\\hline "<< endl;
  for (Int_t j=0; j<(FINALCUT+1);j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableMu[j]);
  }
  
  //add the BTags
  outTableFile << "\\hline " << endl;
  for (Int_t j=(FINALCUT+1); j<NCUTS;j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableMu[j]);
  }
  
  //add the Process Results
  outTableFile << "\\hline "<< endl;
  writeJetRow(outTableFile, "FracPass", FracPassMu);
  writeJetRow(outTableFile, " $Evts/pb^-1$ ", NExpectedMu);
  outTableFile << "\\hline " << endl;
  // close the table
  outTableFile << "\\hline " << endl;
  outTableFile << "\\end{tabular}" << endl;
  outTableFile << "\\end{center}" << endl;
  outTableFile << "\\end{table}" << endl;
  outTableFile << " " << endl;


  //All Leptons
  outTableFile << "\\begin{table}[t]" << endl;
  outTableFile << "\\begin{center}" << endl;
  outTableFile << "\\caption{ ALL LEPTONS: ";
  outTableFile << pEvents << " " << PLabel[processNumber] << " Events";
  outTableFile << "}" << endl;
  outTableFile << "\\begin{tabular}{|l|c|c|c|c|l|}" << endl;
  outTableFile << "\\hline " << endl;
  outTableFile << " Cut/NJets " ;
  for (Int_t k=0; k<NJETS;k++) { 
    outTableFile << "  &  " << k;
  }
  outTableFile << "+" << "  \\\\ "<< endl;
  outTableFile << "\\hline "<< endl;
  for (Int_t j=0; j<(FINALCUT+1);j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableLp[j]);
  }
  
  //add the BTags
  outTableFile << "\\hline " << endl;
  for (Int_t j=(FINALCUT+1); j<NCUTS;j++) {
    writeJetRow(outTableFile, CLabel[j],EvtTableLp[j]);
  }
  
  //add the Process Results
  outTableFile << "\\hline "<< endl;
  writeJetRow(outTableFile, "FracPass", FracPassLp);
  writeJetRow(outTableFile, " $Evts/pb^-1$ ", NExpectedLp);
  outTableFile << "\\hline " << endl;
  // close the table
  outTableFile << "\\hline " << endl;
  outTableFile << "\\end{tabular}" << endl;
  outTableFile << "\\end{center}" << endl;
  outTableFile << "\\end{table}" << endl;
  outTableFile << " " << endl;
 
  // close the document
  outTableFile << " " << endl;
  outTableFile << "\\end{document}" << endl;
  
}

//////////////////////////////////////////////////////
//////  Tabulate the combination of processes
//////////////////////////////////////////////////////
void writeFinSelRow(ofstream& outTableFile, TString Label, double EvtArray[NPROCESSES]) {
//// Prints a row in the table combining multiple processes (for a particular numbe of jets & cut).
//outTableFile.open(outTableFileN,ios::out);
  outTableFile << Label;
  outTableFile << setprecision(2) << scientific;
  for (Int_t k=0; k<NPROCESSES;k++) { 
    outTableFile << "  &  "  << EvtArray[k];
  }
  outTableFile << "  \\\\ "<< endl;
}

void storeEvtArray3to1(double EvtArray[NPROCESSES], double Mtx[NPROCESSES][NCUTS][NJETS], int Ncut, int Njet) {
//Store the information for each Process, when given a cut & a jet number.
  for (Int_t k=0; k<NPROCESSES;k++) { 
    EvtArray[k]=Mtx[k][Ncut][Njet];
  }
}
void storeEvtArray2to1(double EvtArray[NPROCESSES], double Mtx[NPROCESSES][NJETS], int Njet) {
//Store the information for each Process, when given a jet number.
  for (Int_t k=0; k<NPROCESSES;k++) { 
    EvtArray[k]=Mtx[k][Njet];
  }
}

void MakeSelectionTable(const char* inXSecFile, const char* outTableFileName, const char* TTbar_insel, const char* WpJ_insel, const char* ZpJ_insel, const char* WW_insel, const char* WZ_insel, const char* ZZ_insel, const char* QCDHT100to250_insel, const char* QCDHT250to500_insel, const char* STopT_insel, const char* STopS_insel, const char* STopTW_insel) {
//// Produce the selection table given the XSections and selection results for the relevant processes.
//// NJets=number of final state jets (which pass the cuts).
// Get the input files for each process:
  TString selName[NPROCESSES];
  selName[0]=TTbar_insel;
  selName[1]=WpJ_insel;
  selName[2]=ZpJ_insel;
  selName[3]=WW_insel;
  selName[4]=WZ_insel;
  selName[5]=ZZ_insel;
  selName[6]=QCDHT100to250_insel;
  selName[7]=QCDHT250to500_insel;
  selName[8]=STopT_insel;
  selName[9]=STopS_insel;
  selName[10]=STopTW_insel;

  //Combined Tables
  double EvtTableEl[NPROCESSES][NCUTS][NJETS];
  double EvtTableMu[NPROCESSES][NCUTS][NJETS];
  double EvtTableLp[NPROCESSES][NCUTS][NJETS];

  double FracPassEl[NPROCESSES][NJETS];
  double FracPassMu[NPROCESSES][NJETS];
  double FracPassLp[NPROCESSES][NJETS];
  //Number of expected events per pb^-1.
  double NExpectedEl[NPROCESSES][NJETS];
  double NExpectedMu[NPROCESSES][NJETS];
  double NExpectedLp[NPROCESSES][NJETS];
  double pEvents[NPROCESSES]; //The number of original MC events.
  double TempArray[NPROCESSES];

  ofstream outTableFile;
  outTableFile.open(outTableFileName,ios::out);
  InitializeLabels(PLabel,CLabel);
  InitializePresLabels(ElPresCLabel,MuPresCLabel,LpPresCLabel);

  for (Int_t p=0; p<NPROCESSES;p++) {
    readProcessTable(selName[p],EvtTableEl[p],EvtTableMu[p],EvtTableLp[p]);
    computeProcessResults(inXSecFile,p,EvtTableEl[p],EvtTableMu[p],EvtTableLp[p],FracPassEl[p],FracPassMu[p],FracPassLp[p],NExpectedEl[p],NExpectedMu[p],NExpectedLp[p],pEvents[p]);
  }

  // loop over the desired jet counts: 1-5
  outTableFile << "\\documentclass[12pt]{article}" << endl;
  outTableFile << "\\usepackage{lscape}" << endl;
  outTableFile << "\\usepackage{fullpage}" << endl;
  outTableFile << "\\addtolength{\\oddsidemargin}{-0.75in}" << endl;
  outTableFile << "\\addtolength{\\textwidth}{5.0in}" << endl;
  outTableFile << "\\addtolength{\\textheight}{1.4in}" << endl;

  outTableFile << "\\begin{document}" << endl;
  outTableFile << "\\begin{landscape}" << endl;
  //outTableFile << " " << endl;

  for (Int_t nj=0; nj<NJETS;nj++) {
    //// Make the table(s)

    //Electrons
    outTableFile << "\\begin{table}[t]" << endl;
    outTableFile << "\\begin{center}" << endl;
    outTableFile << "\\caption{ ELECTRONS: " << nj ;
    if ( nj==4 ) {
      outTableFile << "+";
    }
    outTableFile << " Jets}" << endl;
    outTableFile << "\\small" << endl;
    outTableFile << "\\begin{tabular}{|l|";
    for (Int_t p=0; p<NPROCESSES;p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << "Cut/Mode" ;
    for (Int_t p=0; p<NPROCESSES;p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"InMCEvts",pEvents);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableEl,c,nj);
      writeFinSelRow(outTableFile,ElPresCLabel[c],TempArray);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableEl,c,nj);
      writeFinSelRow(outTableFile,ElPresCLabel[c],TempArray);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassEl,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray);
    storeEvtArray2to1(TempArray,NExpectedEl,nj);
    writeFinSelRow(outTableFile," $Evts/pb^-1$ ",TempArray);
    outTableFile << "\\hline " << endl;
    outTableFile << "\\hline " << endl;
    //close the table
    outTableFile << "\\end{tabular}" << endl;
    outTableFile << "\\normalsize" << endl;
    outTableFile << "\\end{center}" << endl;
    outTableFile << "\\end{table}" << endl;
    //outTableFile << " " << endl;    
  
  
    //Muons
    outTableFile << "\\begin{table}[h]" << endl;
    outTableFile << "\\begin{center}" << endl;
    outTableFile << "\\caption{ MUONS: " << nj ;
    if ( nj==4 ) {
      outTableFile << "+";
    }
    outTableFile << " Jets}" << endl;
    outTableFile << "\\small" << endl;
    outTableFile << "\\begin{tabular}{|l|";
    for (Int_t p=0; p<NPROCESSES;p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << " Cut/Mode " ;
    for (Int_t p=0; p<NPROCESSES;p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"InMCEvts",pEvents);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableMu,c,nj);
      writeFinSelRow(outTableFile,MuPresCLabel[c],TempArray);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableMu,c,nj);
      writeFinSelRow(outTableFile,MuPresCLabel[c],TempArray);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassMu,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray);
    storeEvtArray2to1(TempArray,NExpectedMu,nj);
    writeFinSelRow(outTableFile," $Evts/pb^-1$ ",TempArray);
    outTableFile << "\\hline " << endl;
    outTableFile << "\\hline " << endl;
    //close the table
    outTableFile << "\\end{tabular}" << endl;
    outTableFile << "\\normalsize" << endl;
    outTableFile << "\\end{center}" << endl;
    outTableFile << "\\end{table}" << endl;
    outTableFile << " " << endl;    
    outTableFile << "\\clearpage" << endl;
  
    
    //All Leptons
    outTableFile << "\\begin{table}[htb]" << endl;
    outTableFile << "\\begin{center}" << endl;
    outTableFile << "\\caption{ LEPTONS: " << nj ;
    if ( nj==4 ) {
      outTableFile << "+";
    }
    outTableFile << " Jets}" << endl;
    outTableFile << "\\small" << endl;
    outTableFile << "\\begin{tabular}{|l|";
    for (Int_t p=0; p<NPROCESSES;p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << " Cut:$e$/$\\mu$  Modes:" ;
    for (Int_t p=0; p<NPROCESSES;p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"InMCEvts",pEvents);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableLp,c,nj);
      writeFinSelRow(outTableFile,LpPresCLabel[c],TempArray);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableLp,c,nj);
      writeFinSelRow(outTableFile,LpPresCLabel[c],TempArray);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassLp,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray);
    storeEvtArray2to1(TempArray,NExpectedLp,nj);
    writeFinSelRow(outTableFile," $Evts/pb^-1$ ",TempArray);
    outTableFile << "\\hline " << endl;
    outTableFile << "\\hline " << endl;
    //close the table
    outTableFile << "\\end{tabular}" << endl;
    outTableFile << "\\normalsize" << endl;
    outTableFile << "\\end{center}" << endl;
    outTableFile << "\\end{table}" << endl;
    outTableFile << " " << endl;    
  

    outTableFile << "\\clearpage" << endl;
  }// end the loop over nj
  outTableFile << " " << endl;
  outTableFile << "\\end{landscape}" << endl;
  outTableFile << "\\end{document}" << endl;

}
