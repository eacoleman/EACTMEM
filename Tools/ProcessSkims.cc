/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Tools for skimming large amounts of PATNtuples and generating tables summarizing the results.

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
#include <TFile.h>
#include <TChain.h>
#include <TBranch.h>
#include "TLorentzVector.h"


#include "DataFormats/FWLite/interface/Handle.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

// User defined variables and functions:
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include <TAMUWW/Tools/GlobalTools428.cc>


void ProcessPATSubset(const char* Label, const char* inName, const char* outRootName, const char* outTxtName, int cntStart, int cntEnd, const char * executable) {
//// Process a subset of PAT files to generate a .txt Selection (sub) file. The PAT files should be of the form inName+cnt+".root" (with cnt=cntStart to cntEnd)
  TString Command;
  char tempC[6];
  Command=" inputFiles=";
  Command=executable+Command;

  for (Int_t i=cntStart; i<(cntEnd+1);i++) {
    Command=Command+inName;
    sprintf(tempC,"%i",i);
    Command=Command+tempC;
    if ( i==cntEnd ){
      Command=Command+".root";
    } else {
      Command=Command+".root,";
    }
  }
  Command=Command+" \\outputEvery=10000 ";
  Command=Command+outRootName;
  Command=Command+" ";
  Command=Command+outTxtName;
  Command=Command+" ";
  Command=Command+Label;
  //cout << "Command=" << Command << endl;
  system(Command);
  
}

void ProcessPATSet(const char* Label, const char* inPATName, const char* inRootName, const char* inTxtName, const char* outRootName, const char* outTxtName, int NStart, int NEnd, int stepSize, const char * executable, int NProcess=-1) {
//// Process a set of either PAT files or already processed .txt & .root files to generate a combined .txt (& .root) file for the whole set. The PAT files should be of the form inName+cnt+".root" (with cnt=NStart to NEnd).
/// stepSize=-1 will not process the PAT tuples, but will run over the .txt files (provided they exist). stepSize=-2 merges the .root files as well.
/// executable gives location of the selection excecutable (e.g. /uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/bin/slc5_amd64_gcc434/PerformSelection428.exe )
/// NProcess!=-1 adds the process name to the temporary files (in order to avoid overlap).
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

  TString pRootName, pTxtName, pProcLabel;

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
  if ( NProcess!=-1 ) {
    pProcLabel="_";
    pProcLabel=pProcLabel+PLabel[NProcess];
  } else {
    pProcLabel="";
  }



  TChain ch("EvtTree");

//   //temporary for ZpJ
//   cnt=17;
//   Nbeg=50*17+1;

  while ( cont==true ) {
    Nfin=Nbeg+step-1;
//     Nfin=2062;
//     cont=false;

    if ( Nfin>=NEnd ) {
      //we have reached the end of the files to process
      Nfin=NEnd;
      cont=false;
    }

    InitializeLabels(PLabel,CLabel);
    cnt++;
    sprintf(cntC,"%i",cnt);
    pTxtName=".txt";
    pTxtName=cntC+pTxtName;
    pTxtName="_pt"+pTxtName;
    pTxtName=pProcLabel+pTxtName;
    //pTxtName=inTxtName+pTxtName;
    ///use outTxtName to store the names of individual parts instead
    pTxtName=outTxtName+pTxtName;
    pRootName=".root";
    pRootName=cntC+pRootName;
    pRootName="_pt"+pRootName;
    pRootName=pProcLabel+pRootName;
    //pRootName=inRootName+pRootName;
    ///use outRootName to store the names of individual parts instead
    pRootName=outRootName+pRootName;

//     cout << "pTxtName=" << pTxtName << endl;
//     cout << "pRootName=" << pRootName << endl;

    if ( stepSize>0) {
      ProcessPATSubset(Label,inPATName,pRootName,pTxtName,Nbeg,Nfin,executable);
    }

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


void GetNFiles(const char* inListPrefix, const char* inListSuffix, int& NFiles, bool& missingFiles) {
//// Gets the number of files corresponding to the inListPrefix*inListSuffix (e.g. PATTuplesMC/MC428_*.root)
  missingFiles=false;
  char line[2000];
  char tempCnt[5];
  TString Command, tempstr, missingfilelist;
  Command=" >& tempLS.txt";
  Command=inListSuffix+Command;
  Command="*"+Command;
  Command=inListPrefix+Command;
  Command="ls "+Command;
  //cout << "Command=" << Command << endl;
  system(Command);

  ifstream inListFile("tempLS.txt");
  int NExpectedFiles=0;
  while ( inListFile.good() ) {
    inListFile.getline(line,2000);
    istrstream str(line);
    str >> tempstr;
    if ( tempstr!="" ) { NExpectedFiles++; }
  }

  missingfilelist="";
  NFiles=0;
  int cnt=1;
  while (cnt<(NExpectedFiles+1)) {
    Command=" >& tempErr.txt";
    Command=inListSuffix+Command;
    sprintf(tempCnt,"%i",cnt);
    Command=tempCnt+Command;
    Command=inListPrefix+Command;
    Command="ls "+Command;
    //cout << "Command= " << Command << endl;
    system(Command);
    ifstream infile("tempErr.txt");
    infile.getline(line,2000);
    tempstr = line;
    if (tempstr.Contains("No such file or directory") ) {
      ///We're missing the ith file
      missingFiles=true;
      if ( missingfilelist!="" ) {
	missingfilelist=missingfilelist+" , ";
      }
      missingfilelist=missingfilelist+tempCnt;
      NExpectedFiles++;
    } else {
      NFiles++;
    }
    cnt++;
  }

  if ( missingFiles ) {
    cout << "Missing The Following Files : " <<  missingfilelist << endl;
  }

}


void ProcessMultiplePATSets(const char* inputmode, const char* inStorageDir, const char* inFileLabel, const char* outResultDir, int StartProcess, int EndProcess, int stepSize, const char * executable) 
//// Process multiple sets of PAT files in the inStorageDir, with the input files of the form inStorageDir+PLabel+/inFileLabel*.root and output of the form outResultDir+PLabel+inputmode+"_outfile.root", outResultDir+PLabel+inputmode+"_SelTable.txt".
/// splitting size dictates how many jobs will be processed per subset.
/// executable gives location of the selection excecutable (e.g. /uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/bin/slc5_amd64_gcc434/PerformSelection428.exe )
{
  //TString MCName="MC428_";
  int NFiles=-1;
  bool missingFiles=false;
  TString inPATName, inRootName, inTxtName, outRootName, outTxtName;

  InitializeLabels(PLabel,CLabel);

  for (Int_t np=StartProcess; np<(EndProcess+1);np++) {
    inPATName=inFileLabel;
    inPATName="/"+inPATName;
    inPATName=PLabel[np]+inPATName;
    inPATName=inStorageDir+inPATName;
    GetNFiles(inPATName,".root",NFiles,missingFiles); 
    cout << "NFiles=" << NFiles << endl;

    inRootName="_outfile";
    inTxtName="_Params";
    inRootName=PLabel[np]+inRootName;
    inTxtName=PLabel[np]+inTxtName;
    inRootName="/"+inRootName;
    inTxtName="/"+inTxtName;
    inRootName=PLabel[np]+inRootName;
    inTxtName=PLabel[np]+inTxtName;
    inRootName=inStorageDir+inRootName;
    inTxtName=inStorageDir+inTxtName;

    outRootName="_outfile.root";
    outTxtName="_SelTable.txt";
    outRootName=inputmode+outRootName;
    outTxtName=inputmode+outTxtName;
    outRootName=PLabel[np]+outRootName;
    outTxtName=PLabel[np]+outTxtName;
    outRootName=outResultDir+outRootName;
    outTxtName=outResultDir+outTxtName;

    //temporary for ZpJ
    // ProcessPATSet(inputmode,inPATName,inRootName,inTxtName,outRootName,outTxtName,50*17+1,NFiles,stepSize,executable,np);
    // NFiles=25;
    if ( !missingFiles ) {
      ProcessPATSet(inputmode,inPATName,inRootName,inTxtName,outRootName,outTxtName,1,NFiles,stepSize,executable,np);
    } else {
      cout << "FILES MISSING, CAN NOT EXECUTE THIS STAGE FOR PROCESS : " << PLabel[np] << endl;
    }
  }
  
}


void ProcessSkims()
{

}

// ///////////////////////////////////////////////////////////////////////////////////////
// ////// Functions to shift the pT (and associated values) by +/- JES Uncertainty
// ///////////////////////////////////////////////////////////////////////////////////////
// void ShiftDueToJESUncertainty(const char* inFileName, const char* outFileName, bool ShiftUp)
// //// Shift the pT by the uncertainty JES corrections; either up 1sigma (ShiftUp=true) or down 1sigma (ShiftUp=false).
// //// Load gSystem->Load("~ilyao/MATRIXELEMENT/CMSSW_4_2_8/lib/slc5_amd64_gcc434/libCondFormatsJetMETObjects.so") before compiling
// {

//   //double in_METEt, out_METEt;
//   vector < TLorentzVector > in_jLV, out_jLV, in_lLV;
//   TLorentzVector cjLV;
//   double pTUncorr, pTCorr, pXCorr, pYCorr, ECorr;
//   double out_Mjj, out_DRlj1, out_DRlj2;
//   double uncert, j_eta;

//   //read the input tree
//   TFile* infile = new TFile(inFileName);
//   TTree* InTree = (TTree*)infile->Get("EvtTree");
//   EventNtuple* InEvtTree = new EventNtuple();
//   InTree->SetBranchAddress("EvtNtuple", &InEvtTree);
//   // create the output tree
//   TFile *outfile = new TFile(outFileName, "RECREATE");
//   TTree* OutTree = new TTree("EvtTree", "Output tree for matrix element");
//   EventNtuple * OutEvtTree = new EventNtuple();
//   OutTree->Branch("EvtNtuple", "EventNtuple", &OutEvtTree);


//   int nEntries;
//   nEntries=InTree->GetEntries();
//   cout << "nEntries=" << nEntries << endl;

//   edm::FileInPath fip("mytestV13_AK5PF_Uncertainty.txt");
//   JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(fip.fullPath());

//   for (Int_t i=0; i<nEntries; i++) {
//     InTree->GetEntry(i);
//     /// Record the branches being altered
//     //in_METEt=InEvtTree->METEt;
// //     cout << "in_METEt=" << in_METEt << endl;
// //     out_METEt=in_METEt+10.0;
// //     OutEvtTree->METEt=out_METEt;

//     in_jLV=InEvtTree->jLV;
//     in_lLV=InEvtTree->lLV;
//     out_jLV.clear();
//     OutEvtTree->lLV.clear();

//     for (Int_t j=0; j<2; j++) {
//       //cout << "jLV[" << j << "].Pt()=" << in_jLV[j].Pt() << endl;
//       pTUncorr=in_jLV[j].Pt();
//       j_eta=in_jLV[j].Eta();
//       jecUnc->setJetEta(j_eta); // Give rapidity of jet you want uncertainty on
//       jecUnc->setJetPt(pTUncorr);// Also give the corrected pt of the jet you want the uncertainty on
//       uncert = jecUnc->getUncertainty(true); // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.
//       //cout << "pt=" << pTUncorr << " eta=" << j_eta << " : uncert=" << uncert << endl;
//       if ( ShiftUp ) {
// 	pTCorr=pTUncorr*(1.0+uncert);
//       } else {
// 	pTCorr=pTUncorr*(1.0-uncert);
//       }
//       pXCorr=(pTCorr/pTUncorr)*in_jLV[j].Px();
//       pYCorr=(pTCorr/pTUncorr)*in_jLV[j].Py();
//       ECorr=sqrt(pTCorr*pTCorr+in_jLV[j].Pz()*in_jLV[j].Pz()+in_jLV[j].M()*in_jLV[j].M());
//       cjLV.SetPxPyPzE(pXCorr,pYCorr,in_jLV[j].Pz(),ECorr);
//       //OutEvtNtuple->lLV.push_back(cjLV);
//       out_jLV.push_back(cjLV);
//     }
//     OutEvtTree->jLV=out_jLV;
//     out_Mjj=(out_jLV[0]+out_jLV[1]).M();
//     OutEvtTree->Mjj=out_Mjj;
//     out_DRlj1=in_lLV[0].DeltaR(out_jLV[0]);
//     out_DRlj2=in_lLV[0].DeltaR(out_jLV[1]);
//     OutEvtTree->DRlj1=out_DRlj1;
//     OutEvtTree->DRlj2=out_DRlj2;

//     /// Record the unaltered branches
//     //OutEvtTree->jLV=InEvtTree->jLV;
//     OutEvtTree->METLV=InEvtTree->METLV; 
//     OutEvtTree->lLV=InEvtTree->lLV;
//     OutEvtTree->matchedGenParticles=InEvtTree->matchedGenParticles;
//     OutEvtTree->matchedpdgId=InEvtTree->matchedpdgId;
//     OutEvtTree->matchedDeltaR=InEvtTree->matchedDeltaR;
//     OutEvtTree->jBtag=InEvtTree->jBtag;
//     OutEvtTree->lQ=InEvtTree->lQ;
//     OutEvtTree->ldetComp=InEvtTree->ldetComp;
//     OutEvtTree->run=InEvtTree->run;
//     OutEvtTree->event=InEvtTree->event;
//     //OutEvtTree->Mjj=InEvtTree->Mjj;
//     OutEvtTree->leptonCat=InEvtTree->leptonCat;
//     OutEvtTree->leptonCat_passAll=InEvtTree->leptonCat_passAll;
//     //OutEvtTree->DRlj1=InEvtTree->DRlj1;
//     //OutEvtTree->DRlj2=InEvtTree->DRlj2;
//     OutEvtTree->Thetalj1pj2=InEvtTree->Thetalj1pj2;
//     OutEvtTree->lTotIso=InEvtTree->lTotIso;
//     OutEvtTree->lecalIso=InEvtTree->lecalIso;
//     OutEvtTree->lhcalIso=InEvtTree->lhcalIso;
//     OutEvtTree->ltrkIso=InEvtTree->ltrkIso;
//     OutEvtTree->METEt=InEvtTree->METEt;
//     OutEvtTree->lPhi=InEvtTree->lPhi;
//     //Fill The Output Tree 
//     OutTree->Fill();

//   }

// //   outfile->cd();
// //   OutTree->Write("EvtTree");
//   outfile->Write();
//   outfile->Close();

// }

// void ShiftJESMultiplePATSets(const char* inputmode, const char* targetDir, int StartProcess, int EndProcess) 
// //// Generate the shifted due to JES up and down files. The input the form targetDir+PLabel+inputmode+"_outfile.root", and the output is of the form targetDir+PLabel+inputmode+"_JESp1s.root", targetDir+PLabel+inputmode+"_JESm1s.root".
// {
//   TString infilename, outfilename;
//   InitializeLabels(PLabel,CLabel);

//   for (Int_t np=StartProcess; np<(EndProcess+1);np++) {
//     cout << "Process=" << PLabel[np] << endl;
//     infilename="_outfile.root";
//     infilename=inputmode+infilename;
//     infilename=PLabel[np]+infilename;
//     infilename=targetDir+infilename;
//     outfilename=inputmode;
//     outfilename=PLabel[np]+outfilename;
//     outfilename=targetDir+outfilename;

//     ShiftDueToJESUncertainty(infilename,outfilename+"_JESp1s.root",true);
//     ShiftDueToJESUncertainty(infilename,outfilename+"_JESm1s.root",false);
//   }
  
// }

///////////////////////////////////////////////////////////////////////////////////////
////// Functions to produce ntuples with additional cuts
///////////////////////////////////////////////////////////////////////////////////////
void ApplyExtraCutsSinglePATSet(const char* inFileName, const char* outFileName)
//// Generate the n-tuples with extra cuts applied
{
  bool passAllCuts;
  vector < TLorentzVector > jLV, lLV, METLV;
  double Mjj, WmT, DRlj1, DRlj2, DEtajj, Ptjj;

  //Read the input tree
  TFile* infile = new TFile(inFileName);
  TTree* InTree = (TTree*)infile->Get("EvtTree");
  EventNtuple* InEvtTree = new EventNtuple();
  InTree->SetBranchAddress("EvtNtuple", &InEvtTree);
  //Create the output tree
  TFile *outfile = new TFile(outFileName, "RECREATE");
  TTree* OutTree = new TTree("EvtTree", "Output tree for matrix element");
  EventNtuple * OutEvtTree = new EventNtuple();
  OutTree->Branch("EvtNtuple", "EventNtuple", &OutEvtTree);


  int nEntries;
  nEntries=InTree->GetEntries();
  cout << "nEntries=" << nEntries << endl;


  for (Int_t i=0; i<nEntries; i++) {
    InTree->GetEntry(i);
    passAllCuts=false;

    jLV=InEvtTree->jLV;
    lLV=InEvtTree->lLV;
    METLV=InEvtTree->METLV;
    Mjj=InEvtTree->Mjj;
    DRlj1=InEvtTree->DRlj1;
    DRlj2=InEvtTree->DRlj2;
    WmT=sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()) );
    DEtajj=fabs(jLV[0].Eta()-jLV[1].Eta());
    Ptjj=sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()));

    if (
	(Mjj>30.0)&&
	(Mjj<300.0)&&
	(WmT>40.0)&&
	(fabs(lLV[0].Eta())<2.1)&&
	(lLV[0].Pt()>25.0)&&
	(jLV[0].Pt()>30.0)&&
	(jLV[1].Pt()>30.0)&&
	(METLV[0].Et()>30.0)&&
	(DRlj1>0.5)&&
	(DRlj2>0.5)&&
	(DEtajj<1.5)&&
	(Ptjj>40.0)&&
	(0.3<(jLV[0].Pt()/Mjj))&&
	((jLV[0].Pt()/Mjj)<0.7)
	) { 
      passAllCuts=true;
    }

    if ( passAllCuts ) {
      //Enter the values for each branch
      OutEvtTree->jLV=InEvtTree->jLV;
      OutEvtTree->METLV=InEvtTree->METLV; 
      OutEvtTree->lLV=InEvtTree->lLV;
      OutEvtTree->matchedGenParticles=InEvtTree->matchedGenParticles;
      OutEvtTree->matchedpdgId=InEvtTree->matchedpdgId;
      OutEvtTree->matchedDeltaR=InEvtTree->matchedDeltaR;
      OutEvtTree->jBtag=InEvtTree->jBtag;
      OutEvtTree->lQ=InEvtTree->lQ;
      OutEvtTree->ldetComp=InEvtTree->ldetComp;
      OutEvtTree->run=InEvtTree->run;
      OutEvtTree->event=InEvtTree->event;
      OutEvtTree->Mjj=InEvtTree->Mjj;
      OutEvtTree->leptonCat=InEvtTree->leptonCat;
      OutEvtTree->leptonCat_passAll=InEvtTree->leptonCat_passAll;
      OutEvtTree->DRlj1=InEvtTree->DRlj1;
      OutEvtTree->DRlj2=InEvtTree->DRlj2;
      OutEvtTree->Thetalj1pj2=InEvtTree->Thetalj1pj2;
      OutEvtTree->lTotIso=InEvtTree->lTotIso;
      OutEvtTree->lecalIso=InEvtTree->lecalIso;
      OutEvtTree->lhcalIso=InEvtTree->lhcalIso;
      OutEvtTree->ltrkIso=InEvtTree->ltrkIso;
      OutEvtTree->METEt=InEvtTree->METEt;
      OutEvtTree->lPhi=InEvtTree->lPhi;
      //Fill The Output Tree 
      OutTree->Fill();
    }

  }

  outfile->Write();
  outfile->Close();

}


void ApplyExtraCutsMultiplePATSets(const char* inputmode, const char* targetDir, int StartProcess, int EndProcess, bool includeJESShifted=false) 
//// Generate the files with extra cuts applies. The input the form targetDir+PLabel+inputmode+"_outfile.root", +"_JESp1s.root" or +"_JESm1s.root". The output is of the form targetDir+PLabel+"_FullCutsV01"+"_outfile.root", +"_JESp1s.root" or +"_JESm1s.root".
{
  TString infilename, outfilename;
  InitializeLabels(PLabel,CLabel);

  for (Int_t np=StartProcess; np<(EndProcess+1);np++) {
    cout << "Process=" << PLabel[np] << endl;
    infilename=inputmode;
    infilename=PLabel[np]+infilename;
    infilename=targetDir+infilename;
    outfilename="_FullCutsV01";//The new input mode is _FullCutsV01 instead of _StandardCuts
    outfilename=PLabel[np]+outfilename;
    outfilename=targetDir+outfilename;

    ApplyExtraCutsSinglePATSet(infilename+"_outfile.root",outfilename+"_outfile.root");
    if ( includeJESShifted ) {
      ApplyExtraCutsSinglePATSet(infilename+"_JESp1s.root",outfilename+"_JESp1s.root");
      ApplyExtraCutsSinglePATSet(infilename+"_JESm1s.root",outfilename+"_JESm1s.root");
    }
  }
  
}


///////////////////////////////////////////////////////////////////////////////////////
//////   Create the .tex tables  ///////
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
void writeFinSelRow(ofstream& outTableFile, TString Label, double EvtArray[NPROCESSES], int pStart, int pEnd) {
//// Prints a row in the table combining multiple processes (for a particular numbe of jets & cut).
//outTableFile.open(outTableFileN,ios::out);
  outTableFile << Label;
  outTableFile << setprecision(2) << scientific;
  for (Int_t k=pStart ; k<(pEnd+1); k++) { 
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

void MakeSelectionTable(const char* inXSecFile, const char* outTableFileName, const char* dirSelTable, const char* selSuffix, int pStart, int pEnd)
//// Produce the selection table given the XSections and selection results for the relevant processes.
//// The input files for each process is of the form dirSelTable+PLabel[p]+selSuffix, where p=pStart-pEnd.
//// NJets=number of final state jets (which pass the cuts).
{
  InitializeLabels(PLabel,CLabel);
  //InitializePresLabels(ElPresCLabel,MuPresCLabel,LpPresCLabel);
  ///Check that the selected processes are not out of range
  if ( pEnd>=NPROCESSES ) {
    cerr << "ERROR: Some of the selected processes are out of range" << endl;
  }
  /// Get the input files for each process:
  TString selName[NPROCESSES];
  TString selName_str;
  for (Int_t p=pStart; p<(pEnd+1); p++) {
    selName_str=selSuffix;
    selName_str=PLabel[p]+selName_str;
    selName_str=dirSelTable+selName_str;
    selName[p]=selName_str;

//   selName[0]=TTbar_insel;
//   selName[1]=WpJ_insel;
//   selName[2]=ZpJ_insel;
//   selName[3]=WW_insel;
//   selName[4]=WZ_insel;
//   //selName[5]=ZZ_insel;
//   selName[5]=QCDHT100to250_insel;
//   selName[6]=QCDHT250to500_insel;
//   selName[7]=STopT_insel;
//   selName[8]=STopS_insel;
//   selName[9]=STopTW_insel;
  }
  
  //Combined Tables
  double EvtTableEl[NPROCESSES][NCUTS][NJETS];
  double EvtTableMu[NPROCESSES][NCUTS][NJETS];
  double EvtTableLp[NPROCESSES][NCUTS][NJETS];

  double FracPassEl[NPROCESSES][NJETS];
  double FracPassMu[NPROCESSES][NJETS];
  double FracPassLp[NPROCESSES][NJETS];
  //Number of expected events per pb^{-1}.
  double NExpectedEl[NPROCESSES][NJETS];
  double NExpectedMu[NPROCESSES][NJETS];
  double NExpectedLp[NPROCESSES][NJETS];
  double pEvents[NPROCESSES]; //The number of original MC events.
  double TempArray[NPROCESSES];

  ofstream outTableFile;
  outTableFile.open(outTableFileName,ios::out);


//   for (Int_t p=0; p<NPROCESSES;p++) {
  for (Int_t p=pStart; p<(pEnd+1); p++) {
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
    for (Int_t p=pStart; p<(pEnd+1); p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << "Cut/Mode" ;
    for (Int_t p=pStart; p<(pEnd+1); p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"MCEvts/Lumi(pb)",pEvents,pStart,pEnd);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableEl,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableEl,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassEl,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray,pStart,pEnd);
    storeEvtArray2to1(TempArray,NExpectedEl,nj);
    writeFinSelRow(outTableFile," $Evts/pb^{-1}$ ",TempArray,pStart,pEnd);
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
    for (Int_t p=pStart; p<(pEnd+1); p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << " Cut/Mode " ;
    for (Int_t p=pStart; p<(pEnd+1); p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"MCEvts/Lumi(pb)",pEvents,pStart,pEnd);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableMu,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableMu,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassMu,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray,pStart,pEnd);
    storeEvtArray2to1(TempArray,NExpectedMu,nj);
    writeFinSelRow(outTableFile," $Evts/pb^{-1}$ ",TempArray,pStart,pEnd);
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
    for (Int_t p=pStart; p<(pEnd+1); p++) {
      outTableFile << "c|";
    }
    outTableFile << "l|}" << endl;
    outTableFile << "\\hline " << endl;
    outTableFile << " Cut:$e$/$\\mu$  Modes:" ;
    for (Int_t p=pStart; p<(pEnd+1); p++) { 
      outTableFile << "  &  " << PLabel[p];
    }
    outTableFile << "  \\\\ "<< endl;
    outTableFile << "\\hline "<< endl;
    writeFinSelRow(outTableFile,"MCEvts/Lumi(pb)",pEvents,pStart,pEnd);
    for (Int_t c=0; c<(FINALCUT+1);c++) {
      storeEvtArray3to1(TempArray,EvtTableLp,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the BTags
    outTableFile << "\\hline " << endl;
    for (Int_t c=(FINALCUT+1); c<NCUTS;c++) {
      storeEvtArray3to1(TempArray,EvtTableLp,c,nj);
      writeFinSelRow(outTableFile,CLabel[c],TempArray,pStart,pEnd);
    }
    //add the Summary Information     
    outTableFile << "\\hline "<< endl;
    storeEvtArray2to1(TempArray,FracPassLp,nj);
    writeFinSelRow(outTableFile,"FracPass",TempArray,pStart,pEnd);
    storeEvtArray2to1(TempArray,NExpectedLp,nj);
    writeFinSelRow(outTableFile," $Evts/pb^{-1}$ ",TempArray,pStart,pEnd);
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
