/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Common Set of variables and functions.

//
// User Defined Includes
//
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"

//Standard library includes
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <strstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


const int NPROCESSES=18;
//const int NCUTS=12;
const int NCUTS=22;
const int FINALCUT=7;
const int NJETS=5; //changed on 02/28/2012 to include more jet bins

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
  pcsLbl[2]="WpJ";
  pcsLbl[3]="ZpJ";
  pcsLbl[4]="TTbar_MG";
  pcsLbl[5]="STopT_T";
  pcsLbl[6]="STopT_Tbar";
  pcsLbl[7]="STopS_T";
  pcsLbl[8]="STopS_Tbar";
  pcsLbl[9]="STopTW_T";
  pcsLbl[10]="STopTW_Tbar";
  pcsLbl[11]="QCDMu";
  pcsLbl[12]="QCDEl_Pt30to80";
  pcsLbl[13]="QCDEl_Pt80to170";
  pcsLbl[14]="QCDEl_BCtoE30to80";
  pcsLbl[15]="QCDEl_BCtoE80to170";
  pcsLbl[16]="SingleMu";
  pcsLbl[17]="SingleEl";

  ///Matrix Element Selection Labels NORMAL
/*
  cutLbl[0]  = "NPATtupleEvts";
  cutLbl[1]  = "c0:NJets";
  cutLbl[2]  = "c1:HLT+Kin";
  cutLbl[3]  = "c2:VtxCut";
  cutLbl[4]  = "c3:PrimaryEl/Mu";
  cutLbl[5]  = "c4:NotLooseMu";
  cutLbl[6]  = "c5:NotLooseEl";
  cutLbl[7]  = "c6:METE";
  cutLbl[8]  = "BTag0";
  cutLbl[9]  = "BTag1";
  cutLbl[10] = "BTag2";
  cutLbl[11] = "BTag3+";
*/
  ///FNAL SYNC
  cutLbl[0]  = "NPATtupleEvts";
  cutLbl[1]  = "c0:NJets";
  cutLbl[2]  = "c1:HLT+Kin";
  cutLbl[3]  = "c2:VtxCut";
  cutLbl[4]  = "c3:PrimaryEl/Mu";
  cutLbl[5]  = "c4:NotLooseMu";
  cutLbl[6]  = "c5:NotLooseEl";
  cutLbl[7]  = "c6:METE";
  cutLbl[8]  = "FNAL:MET";
  cutLbl[9]  = "FNAL:Mt";
  cutLbl[10] = "FNAL:LepPt";
  cutLbl[11] = "FNAL:LepEta";
  cutLbl[12] = "FNAL:JetPt1";
  cutLbl[13] = "FNAL:JetPt2";
  cutLbl[14] = "FNAL:JetEta1";
  cutLbl[15] = "FNAL:JetEta2";
  cutLbl[16] = "FNAL:drJetLep";
  cutLbl[17] = "FNAL:Mjj";
  cutLbl[18] = "BTag0";
  cutLbl[19] = "BTag1";
  cutLbl[20] = "BTag2";
  cutLbl[21] = "BTag3+";

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

/// Creates a Table for the process (combines the electron, muon, and lepton numbers)
void getProcessTable(Table* table, int EvtTableEl[NCUTS][NJETS], int EvtTableMu[NCUTS][NJETS], int EvtTableLp[NCUTS][NJETS]) {
   TableRow* tableRow;
   TableCellVal* tableCellVal;
   stringstream out;
   //
   // loop over the desired jet counts
   //
   for (Int_t nj=0; nj<NJETS;nj++) {
      //
      // electrons
      //
      out << "ELECTRONS ( " << nj << " Jets )";
      tableRow = new TableRow(out.str());
      //
      // clears the stringstream
      //
      out.str("");

      for (Int_t nc=0; nc<NCUTS;nc++) {
         tableCellVal = new TableCellVal(string(CLabel[nc]));
         tableCellVal->val = Value(EvtTableEl[nc][nj],0.0);
         tableRow->addCellEntries(tableCellVal);
      }
      table->addRow(*tableRow);
      delete tableRow;

      //
      // muons
      //
      out << "MUONS ( " << nj << " Jets )";
      tableRow = new TableRow(out.str());
      out.str("");

      for (Int_t nc=0; nc<NCUTS;nc++) {
         tableCellVal = new TableCellVal(string(CLabel[nc]));
         tableCellVal->val = Value(EvtTableMu[nc][nj],0.0);
         tableRow->addCellEntries(tableCellVal);
      }
      table->addRow(*tableRow);
      delete tableRow;

      //
      // leptons
      //
      out << "LEPTONS ( " << nj << " Jets )";
      tableRow = new TableRow(out.str());
      out.str("");

      for (Int_t nc=0; nc<NCUTS;nc++) {
         tableCellVal = new TableCellVal(string(CLabel[nc]));
         tableCellVal->val = Value(EvtTableLp[nc][nj],0.0);
         tableRow->addCellEntries(tableCellVal);
      }
      table->addRow(*tableRow);
      delete tableRow;
   }
}

/// Creates a Table for the process
void getProcessTable(Table* table, int EvtTable[NCUTS][NJETS]) {
   TableRow* tableRow;
   TableCellVal* tableCellVal;
   stringstream out;

   //
   // loop over the desired cuts
   //
   for (Int_t nc=0; nc<NCUTS;nc++) {
      tableRow = new TableRow(string(CLabel[nc]));
      //
      // loop over the desired jet counts
      //
      for (Int_t nj=0; nj<NJETS; nj++) {
         if (nj == NJETS-1)
            out << nj << "+ Jet(s)";
         else
            out << nj << " Jet(s)";
         tableCellVal = new TableCellVal(out.str());
         out.str("");
         tableCellVal->val = Value(EvtTable[nc][nj],0.0);
         tableRow->addCellEntries(tableCellVal);
      }
      table->addRow(*tableRow);
      delete tableRow;
   }   
}

void setProcessTable(TString selFileName, TString directory, TString tablename, double EvtTableEl[NCUTS][NJETS], double EvtTableMu[NCUTS][NJETS], double EvtTableLp[NCUTS][NJETS]) {
   //
   // extracts the variables from a root file with a selection table
   //
   Table* table;
   vector<TableRow> tableRows;
   vector<TableCell*> rowCells;
   int value;
   TString token[12];
   int nJ;
   TFile selFile(selFileName);
   if (!selFile.IsOpen()) {
      cout << "ERROR: The file \"" << selFileName << "\" was not opened." << endl
           << " Return without setting the arrays." << endl;
      return;
   }
   else {
      cout << "selFileName=" << selFileName << endl;
   }

   gDirectory->cd(directory);
   table = (Table*)gDirectory->Get(tablename);
   if (!table) {
      cout << "ERROR: The table names \"" << tablename << "\" was not found in the directory given." << endl
           << " Return without setting the arrays." << endl;
      return;
   }
   tableRows = table->getRows();
   if(tableRows.size()==0) {
      cout << "ERROR: The table has 0 rows." << endl
           << " Return without setting the arrays." << endl;
      return;
         }
   
   //
   // loop over the desired jet counts
   //
   for (Int_t nj=0; nj<NJETS; nj++) {
      for (Int_t r=0; r<3; r++) {
         rowCells = tableRows[(nj*3)+r].getCellEntries();
         istrstream str(tableRows[(nj*3)+r].GetName());
         str >> token[0] >> token[1] >> token[2] >> token[3] >> token[4];
         nJ = atoi(token[2]);
         if ( nJ!=nj ) {
            if ( r==0 )
               cout << "Error - Electrons: nJ=" << nJ << ",while nj=" << nj << endl;
            else if ( r==1 )
               cout << "Error - Muons: nJ=" << nJ << ",while nj=" << nj << endl;
            else if ( r==2 )
               cout << "Error - All Leptons: nJ=" << nJ << ",while nj=" << nj << endl;
         }
         for (Int_t nc=0; nc<NCUTS;nc++) {
            value = (int)(((TableCellVal*)rowCells[nc])->val.value);
            if ( r==0 )
               EvtTableEl[nc][nJ] = value;
            else if ( r==1)
               EvtTableMu[nc][nJ] = value;
            else if ( r==2 )
               EvtTableLp[nc][nJ] = value;
         }
      }
   }// end the loop over nj
}


void setProcessTable(TString selFileName, TString directory, TString tablename, double EvtTable[NCUTS][NJETS]) {
   //
   // extracts the variables from a root file with a selection table
   //
   Table* table;
   vector<TableRow> tableRows;
   vector<TableCell*> rowCells;
   int value;
   TFile selFile(selFileName);
   if (!selFile.IsOpen()) {
      cout << "ERROR: The file \"" << selFileName << "\" was not opened." << endl
           << " Return without setting the arrays." << endl;
      return;
   }
   else {
      cout << "selFileName=" << selFileName << endl;
   }

   gDirectory->cd(directory);
   table = (Table*)gDirectory->Get(tablename);
   if (!table) {
      cout << "ERROR: The table names \"" << tablename << "\" was not found in the directory given." << endl
           << " Return without setting the arrays." << endl;
      return;
   }
   tableRows = table->getRows();
   if(tableRows.size()==0) {
      cout << "ERROR: The table has 0 rows." << endl
           << " Return without setting the arrays." << endl;
      return;
         }
   
   //
   // loop over the desired cuts
   //
   for (Int_t nc=0; nc<NCUTS;nc++) {
      rowCells = tableRows[nc].getCellEntries();
      //
      // loop over the desired jet counts
      //
      for (Int_t nj=0; nj<NJETS; nj++) {
         value = (int)(((TableCellVal*)rowCells[nj])->val.value);
         EvtTable[nc][nj] = value;
      }
   }
}
