/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Common Set of variables and functions.

const int NPROCESSES=13;
const int NCUTS=10;
const int FINALCUT=5;
const int NJETS=5;

//Global Variables
TString PLabel[NPROCESSES];
TString CLabel[NCUTS];
TString ElPresCLabel[NCUTS];
TString MuPresCLabel[NCUTS];
TString LpPresCLabel[NCUTS];

void InitializeLabels(TString pcsLbl[NPROCESSES], TString cutLbl[NCUTS])
{
  pcsLbl[0]="WW";
  pcsLbl[1]="WZ";
  pcsLbl[2]="WJets";
  pcsLbl[3]="ZJets";
  pcsLbl[4]="TTbar";
  pcsLbl[5]="STopT_T";
  pcsLbl[6]="STopT_Tbar";
  pcsLbl[7]="STopS_T";
  pcsLbl[8]="STopS_Tbar";
  pcsLbl[9]="STopTW_T";
  pcsLbl[10]="STopTW_Tbar";
  pcsLbl[11]="SingleMu_Data_11p966fb";
  pcsLbl[12]="SingleEl_Data_11p828fb";
//   pcsLbl[11]="HWWMH150";
//   pcsLbl[12]="HWWMH160";
//   pcsLbl[13]="HWWMH170";
//   pcsLbl[14]="HWWMH180";
//   pcsLbl[15]="HWWMH190";
//   pcsLbl[16]="HWWMH200";
//   pcsLbl[17]="HWWMH250";
//   pcsLbl[18]="HWWMH300";
//   pcsLbl[19]="HWWMH350";
//   pcsLbl[20]="HWWMH400";
//   pcsLbl[21]="HWWMH450";
//   pcsLbl[22]="HWWMH500";
//   pcsLbl[23]="HWWMH550";
//   pcsLbl[24]="HWWMH600";


  ///Matrix Element Selection Labels
  cutLbl[0]="c0:HLT+Kin";
  cutLbl[1]="c1:NJets";
  cutLbl[2]="c2:PrimaryEl/Mu";
  cutLbl[3]="c3:NotLooseMu";
  cutLbl[4]="c4:NotLooseEl";
  cutLbl[5]="c5:METE";
  cutLbl[6]="BTag0";  
  cutLbl[7]="BTag1";
  cutLbl[8]="BTag2";
  cutLbl[9]="BTag3+";


//   ///V4 Sync Exercise Labels
//   cutLbl[0]="c0:None";
//   cutLbl[1]="c1:Vtx";
//   cutLbl[2]="c2:ELIso/MuIso";
//   cutLbl[3]="c3:NotLooseMu";
//   cutLbl[4]="c4:NotLooseEl";
//   cutLbl[5]="c5:None";
//   cutLbl[6]="c6:None";
//   cutLbl[7]="c7:Jets>=NJ";
//   cutLbl[8]="BTag0";
//   cutLbl[9]="BTag1";
//   cutLbl[10]="BTag2";
//   cutLbl[11]="BTag3+";

}


void InitializePresLabels(TString ElPresLbl[NPROCESSES], TString MuPresLbl[NCUTS], TString LpPresLbl[NCUTS])
{
//   ///For Presentation Quality output tables
//   ElPresLbl[0]="HLT+Kin";
//   ElPresLbl[1]="VtxCut";
//   ElPresLbl[2]="NJets";
//   ElPresLbl[3]="One Isolated-$e$";
//   ElPresLbl[4]="$\\neg$ $e$ Conversion";
//   ElPresLbl[5]="$\\neg$ Loose-$\\mu$";
//   ElPresLbl[6]="$Z$-Veto";
//   ElPresLbl[7]="$MET_{ET}$";
//   ElPresLbl[8]="BTag0";  
//   ElPresLbl[9]="BTag1";
//   ElPresLbl[10]="BTag2";
//   ElPresLbl[11]="BTag3+";

//   MuPresLbl[0]="HLT+Kin";
//   MuPresLbl[1]="VtxCut";
//   MuPresLbl[2]="NJets";
//   MuPresLbl[3]="One Isolated-$\\mu$";
//   MuPresLbl[4]="$\\neg$ Loose-$\\mu$";
//   MuPresLbl[5]="$\\neg$ Loose-$e$";
//   MuPresLbl[6]="None";
//   MuPresLbl[7]="$MET_{ET}$";
//   MuPresLbl[8]="BTag0";  
//   MuPresLbl[9]="BTag1";
//   MuPresLbl[10]="BTag2";
//   MuPresLbl[11]="BTag3+";

//   LpPresLbl[0]="HLT+Kin";
//   LpPresLbl[1]="VtxCut";
//   LpPresLbl[2]="NJets";
//   LpPresLbl[3]="Tight $e$/$\\mu$";
//   LpPresLbl[4]="$\\neg$ $e$-conv/Loose-$\\mu$";
//   LpPresLbl[5]="$\\neg$ Loose $\\mu$/$e$";
//   LpPresLbl[6]="$Z$-Veto/None";
//   LpPresLbl[7]="$MET_{ET}$";
//   LpPresLbl[8]="BTag0";  
//   LpPresLbl[9]="BTag1";
//   LpPresLbl[10]="BTag2";
//   LpPresLbl[11]="BTag3+";

  ///Dileptons
  ElPresLbl[0]="HLT+Kin";
  ElPresLbl[1]="VtxCut";
  ElPresLbl[2]="NJets";
  ElPresLbl[3]="2Isolated-$e$";
  ElPresLbl[4]="$\\neg$ $e$ Conversion";
  ElPresLbl[5]="$\\neg$ Loose-$\\mu$";
  ElPresLbl[6]="$Z$-Veto";
  ElPresLbl[7]="$MET_{ET}$";
  ElPresLbl[8]="BTag0";  
  ElPresLbl[9]="BTag1";
  ElPresLbl[10]="BTag2";
  ElPresLbl[11]="BTag3+";

  MuPresLbl[0]="HLT+Kin";
  MuPresLbl[1]="VtxCut";
  MuPresLbl[2]="NJets";
  MuPresLbl[3]="2Isolated-$\\mu$";
  MuPresLbl[4]="$\\neg$ Loose-$\\mu$";
  MuPresLbl[5]="$\\neg$ Loose-$e$";
  MuPresLbl[6]="$Z$-Veto";
  MuPresLbl[7]="$MET_{ET}$";
  MuPresLbl[8]="BTag0";  
  MuPresLbl[9]="BTag1";
  MuPresLbl[10]="BTag2";
  MuPresLbl[11]="BTag3+";

  LpPresLbl[0]="HLT+Kin";
  LpPresLbl[1]="VtxCut";
  LpPresLbl[2]="NJets";
  LpPresLbl[3]="Tight $e$/$\\mu$";
  LpPresLbl[4]="$\\neg$ $e$-conv/Loose-$\\mu$";
  LpPresLbl[5]="$\\neg$ Loose $\\mu$/$e$";
  LpPresLbl[6]="$Z$-Veto";
  LpPresLbl[7]="$MET_{ET}$";
  LpPresLbl[8]="BTag0";  
  LpPresLbl[9]="BTag1";
  LpPresLbl[10]="BTag2";
  LpPresLbl[11]="BTag3+";

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
//   cout << "Reading the cut line (char):  " << linein << endl;
//   cout << "Reading the cut line (str):  " << str << endl;
  TString cut[NCUTS];
  //input the number of Jets
  str >> nJ >> token[0] >> token[1];
  //input the counts passing the cuts
  for (Int_t nc=0; nc<NCUTS;nc++) {
    str >> cut[nc] >> Evt[nc][nJ];
  }
  //  cout << "nJ=" << nJ << endl;
}

void writeProcessTable(ofstream& outtablefile, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS]) {
//// Writes a .txt table file for the process
   for (Int_t nj=0; nj<NJETS;nj++) {
     outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "ELECTRONS:  " << setprecision(5) << double(EvtTableEl[FINALCUT][nj])/double(EvtTableLp[FINALCUT][nj])*100 << "% of electrons (after cut " << FINALCUT << ")" << endl;
     writeCutLine(outtablefile,EvtTableEl,nj);
     outtablefile << "MUONS:  " << setprecision(5) << double(EvtTableMu[FINALCUT][nj])/double(EvtTableLp[FINALCUT][nj])*100 << "% of muons (after cut " << FINALCUT << ")" << endl;
     writeCutLine(outtablefile,EvtTableMu,nj);
     outtablefile << "LEPTONS:  " << EvtTableLp[0][nj] << " Events Total (after cut 0)" << endl;
     writeCutLine(outtablefile,EvtTableLp,nj);
   }

}

void writeIntProcessTable(ofstream& outtablefile, int EvtTableEl[NCUTS][NJETS], int EvtTableMu[NCUTS][NJETS], int EvtTableLp[NCUTS][NJETS]) {
//// Writes a .txt table file for the process
   // loop over the desired jet counts
   for (Int_t nj=0; nj<NJETS;nj++) {
     outtablefile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
     outtablefile << "ELECTRONS:  " << setprecision(5) << double(EvtTableEl[FINALCUT][nj])/double(EvtTableLp[FINALCUT][nj])*100 << "% of electrons (after cut " << FINALCUT << ")" << endl;
     writeIntCutLine(outtablefile,EvtTableEl,nj);
     outtablefile << "MUONS:  " << setprecision(5) << double(EvtTableMu[FINALCUT][nj])/double(EvtTableLp[FINALCUT][nj])*100 << "% of muons (after cut " << FINALCUT << ")" << endl;
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
  cout << "selFileName=" << selFileName << endl;

  // loop over the desired jet counts
  for (Int_t nj=0; nj<NJETS;nj++) {
    selFile.getline(linein,300);
    //read in the electrons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    //cout << "Read the electron line" << linein << endl;
    readCutLine(linein,EvtTableEl,token,nJ);
    if ( nJ!=nj ) {
      cout << "Error - Electrons: nJ=" << nJ << ",while nj=" << nj << endl;
    }
    //read in the muons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    readCutLine(linein,EvtTableMu,token,nJ);
    if ( nJ!=nj ) {
      cout << "Error - Muons: nJ=" << nJ << ",while nj=" << nj << endl;
    }	
    //read in all leptons
    selFile.getline(linein,300);
    selFile.getline(linein,300);
    readCutLine(linein,EvtTableLp,token,nJ);
    if ( nJ!=nj ) {
      cout << "Error - All Leptons: nJ=" << nJ << ",while nj=" << nj << endl;
    }
      
  }// end the loop over nj
}


void getInputInfo(const char* inXSecFile, double XSec[NPROCESSES], double NTot[NPROCESSES]) {
  //read in the crosssection info and the event counts
  char linein[5000];
  TString token[NPROCESSES];
  TString eqls="=";
  double NTotal;
  ifstream XSecFile(inXSecFile);
  XSecFile.getline(linein,5000);
  XSecFile.getline(linein,5000);
  istrstream str1(linein);
  for (Int_t i=0; i<NPROCESSES;i++) {
    str1 >> token[i] >> eqls >> XSec[i];
    if ( token[i]!=PLabel[i] ) {
      cout << "ERROR: Unable to correctly read in the XSection for the process " << PLabel[i] << endl;
    }
  }
  XSecFile.getline(linein,5000);
  XSecFile.getline(linein,5000);
  istrstream str2(linein);
  for (Int_t i=0; i<NPROCESSES;i++) {
    str2 >> token[i] >> eqls >> NTotal;
    NTot[i]=1000.0*NTotal;
    //cout << "i=" << i << " NTot[i]=" << NTot[i] << endl;
    if ( token[i]!=PLabel[i] ) {
      cout << "ERROR: Unable to correctly read in the Total Event Count for the process " << PLabel[i] << endl;
    }
  }

}

void computeProcessResults(const char* inXSecFile, int processNumber, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS], double FracPassEl[NJETS], double FracPassMu[NJETS], double FracPassLp[NJETS], double NExpectedEl[NJETS], double NExpectedMu[NJETS], double NExpectedLp[NJETS], double& Evts) {
//// Computes the Fractions which pass all of the cuts as well as the corresponding numbers of expected events (per pb^-1).
//// For Data NTotEvts->Lumi(pb), XSec=1 (which yields Evts/pb=EvtsPassingTheFinalCut/Lumi)
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