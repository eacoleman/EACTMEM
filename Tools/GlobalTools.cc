/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Preliminary Sensitivity Analysis
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Common Set of variables and functions.


// ofstream outTableFile;
//ofstream outtablefile;
const int NPROCESSES=11;
const int NCUTS=13;
const int FINALCUT=8;
const int NJETS=5;

//Global Variables
TString PLabel[NPROCESSES];
TString CLabel[NCUTS];

void InitializeLabels(TString pcsLbl[NPROCESSES], TString cutLbl[NCUTS])
{
  pcsLbl[0]="TTbar";
  pcsLbl[1]="WpJ";
  pcsLbl[2]="ZpJ";
  pcsLbl[3]="WW";
  pcsLbl[4]="WZ";
  pcsLbl[5]="ZZ";
  pcsLbl[6]="QCDHT100to250";
  pcsLbl[7]="QCDHT250to500";
  pcsLbl[8]="STopT";
  pcsLbl[9]="STopS";
  pcsLbl[10]="STopTW";


  cutLbl[0]="c0:HLT+Kin";
  cutLbl[1]="c1:LREvts";
  cutLbl[2]="c2:Jets";
  cutLbl[3]="c3:LKin";
  cutLbl[4]="c4:LIso";
  cutLbl[5]="c5:METET";
  cutLbl[6]="c6:ZVeto";
  cutLbl[7]="c7:!Conv";
  cutLbl[8]="c8:Barrel";  
  cutLbl[9]="BTag0";
  cutLbl[10]="BTag1";
  cutLbl[11]="BTag2";
  cutLbl[12]="BTag3+";
}

void InitializeMatrix(double Mtx[NCUTS][NJETS]) {
  for (Int_t nc=0; nc<NCUTS;nc++) {
    for (Int_t nj=0; nj<NJETS;nj++) {
      Mtx[nc][nj]=0;
    }
  }
}

void AddMatrices(double MtxPrimary[NCUTS][NJETS], double MtxSource[NCUTS][NJETS]) {
////Adds the Source Matrix to the Primary Matrix.
  for (Int_t nc=0; nc<NCUTS;nc++) {
    for (Int_t nj=0; nj<NJETS;nj++) {
      MtxPrimary[nc][nj]=MtxPrimary[nc][nj]+MtxSource[nc][nj];
    }
  }
}

void InitializeIntMatrix(int Mtx[NCUTS][NJETS]) {
  for (Int_t nc=0; nc<NCUTS;nc++) {
    for (Int_t nj=0; nj<NJETS;nj++) {
      Mtx[nc][nj]=0;
    }
  }
}


void writeCutLine(ofstream& outtablefile, double Evt[NCUTS][NJETS], int nJ) {
//// Writes out the event counts passing each cut in outtablefile
  // Standard cuts
  outtablefile << nJ << " Jets || " ;
  for (Int_t nc=0; nc<NCUTS;nc++) {
    outtablefile << CLabel[nc] << "= " << Evt[nc][nJ] << " ";
  }
  outtablefile << endl;
}

void writeIntCutLine(ofstream& outtablefile, int Evt[NCUTS][NJETS], int nJ) {
//// Writes out the event counts passing each cut in outtablefile
  // Standard cuts
  outtablefile << nJ << " Jets || " ;
  for (Int_t nc=0; nc<NCUTS;nc++) {
    outtablefile << CLabel[nc] << "= " << Evt[nc][nJ] << " ";
  }
  outtablefile << endl;
}

void readCutLine(const char* linein, double Evt[NCUTS][NJETS], TString *token, int& nJ) {
//// Takes linein, extracts the number of events passing each successive cut and records the count (for the nth cut) in Evt[n][nJ].
  istrstream str(linein);
  TString cut[NCUTS];
  //input the number of Jets
  str >> nJ >> token[0] >> token[1];
  //input the counts passing the cuts
  for (Int_t nc=0; nc<NCUTS;nc++) {
    str >> cut[nc] >> Evt[nc][nJ];
  }
}

void writeProcessTable(ofstream& outtablefile, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS]) {
  //const char* outTableFileName
//// Writes a .txt table file for the process
  //ofstream outtablefile;
  //outtablefile.open(outtablefileName,ios::out);
   // loop over the desired jet counts
   for (Int_t nj=0; nj<NJETS;nj++) {
     outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "ELECTRONS:  " << setprecision(3) << double(EvtTableEl[8][nj])/double(EvtTableLp[8][nj])*100 << "% of electrons (after cut 8)" << endl;
     writeCutLine(outtablefile,EvtTableEl,nj);
     outtablefile << "MUONS:  " << setprecision(3) << double(EvtTableMu[8][nj])/double(EvtTableLp[8][nj])*100 << "% of muons (after cut 8)" << endl;
     writeCutLine(outtablefile,EvtTableMu,nj);
     outtablefile << "LEPTONS:  " << EvtTableLp[0][nj] << " Events Total (after cut 0)" << endl;
     writeCutLine(outtablefile,EvtTableLp,nj);
   }

}

void writeIntProcessTable(ofstream& outtablefile, int EvtTableEl[NCUTS][NJETS], int EvtTableMu[NCUTS][NJETS], int EvtTableLp[NCUTS][NJETS]) {
  //const char* outTableFileName
//// Writes a .txt table file for the process
  //ofstream outtablefile;
  //outtablefile.open(outtablefileName,ios::out);
   // loop over the desired jet counts
   for (Int_t nj=0; nj<NJETS;nj++) {
     outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "ELECTRONS:  " << setprecision(3) << double(EvtTableEl[8][nj])/double(EvtTableLp[8][nj])*100 << "% of electrons (after cut 8)" << endl;
     writeIntCutLine(outtablefile,EvtTableEl,nj);
     outtablefile << "MUONS:  " << setprecision(3) << double(EvtTableMu[8][nj])/double(EvtTableLp[8][nj])*100 << "% of muons (after cut 8)" << endl;
     writeIntCutLine(outtablefile,EvtTableMu,nj);
     outtablefile << "LEPTONS:  " << EvtTableLp[0][nj] << " Events Total (after cut 0)" << endl;
     writeIntCutLine(outtablefile,EvtTableLp,nj);
   }

}

void readProcessTable(const char* selFileName, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS]) {
//// Extracts the variables from a Selection File
  char linein[300];
  TString token[12];
  int nJ;
  ifstream selFile;
  selFile.open(selFileName);
  //selFile.getline(linein,300);
  
  // loop over the desired jet counts
  for (Int_t nj=0; nj<NJETS;nj++) {
    selFile.getline(linein,300);
    //read in the electrons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    readCutLine(linein,EvtTableEl,token,nJ);
    if ( nJ!=nj ) {
      //cout << "File: " << selFileName << endl;
      cout << "Error - Electrons: nJ=" << nJ << ",while nj=" << nj << endl;
    }
    //read in the muons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    //cout << "preprocessing linein=" << linein << endl;
    readCutLine(linein,EvtTableMu,token,nJ);
    if ( nJ!=nj ) {
      //cout << "File: " << selFileName << endl;
      cout << "Error - Muons: nJ=" << nJ << ",while nj=" << nj << endl;
    }	
    //read in all leptons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    readCutLine(linein,EvtTableLp,token,nJ);
    if ( nJ!=nj ) {
      //cout << "File: " << selFileName << endl;
      cout << "Error - All Leptons: nJ=" << nJ << ",while nj=" << nj << endl;
    }
      
  }// end the loop over nj
}


void getInputInfo(const char* inXSecFile, double XSec[NPROCESSES], double NTot[NPROCESSES]) {
  //read in the crosssection info and the event counts
  char linein[500];
  TString token[NPROCESSES];
  TString eqls="=";
  ifstream XSecFile(inXSecFile);
  XSecFile.getline(linein,500);
  XSecFile.getline(linein,500);
  istrstream str1(linein);
  for (Int_t i=0; i<NPROCESSES;i++) {
    str1 >> token[i] >> eqls >> XSec[i];
    if ( token[i]!=PLabel[i] ) {
      cout << "ERROR: Unable to correctly read in the XSection for the process " << PLabel[i] << endl;
    }
  }
  XSecFile.getline(linein,500);
  XSecFile.getline(linein,500);
  istrstream str2(linein);
  for (Int_t i=0; i<NPROCESSES;i++) {
    str2 >> token[i] >> eqls >> NTot[i];
    NTot[i]=1000*NTot[i];
    if ( token[i]!=PLabel[i] ) {
      cout << "ERROR: Unable to correctly read in the Total Event Count for the process " << PLabel[i] << endl;
    }
  }

}

void computeProcessResults(const char* inXSecFile, int processNumber, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS], double FracPassEl[NJETS], double FracPassMu[NJETS], double FracPassLp[NJETS], double NExpectedEl[NJETS], double NExpectedMu[NJETS], double NExpectedLp[NJETS], double& Evts) {
//// Computes the Fractions which pass all of the cuts as well as the corresponding numbers of expected events (per pb^-1).
  double XSec[NPROCESSES];
  double NTotEvts[NPROCESSES];

  getInputInfo(inXSecFile,XSec,NTotEvts);
  for (Int_t k=0; k<NJETS;k++) { 
    FracPassEl[k]=EvtTableEl[FINALCUT][k]/NTotEvts[processNumber];
    FracPassMu[k]=EvtTableMu[FINALCUT][k]/NTotEvts[processNumber];
    FracPassLp[k]=EvtTableLp[FINALCUT][k]/NTotEvts[processNumber];
    NExpectedEl[k]=XSec[processNumber]*FracPassEl[k];
    NExpectedMu[k]=XSec[processNumber]*FracPassMu[k];
    NExpectedLp[k]=XSec[processNumber]*FracPassLp[k];
  }
  Evts=NTotEvts[processNumber];
}
