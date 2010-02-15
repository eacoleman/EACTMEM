#include <iostream>
#include <strstream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

#include <string.h>
/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Preliminary Sensitivity Analysis
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <TString.h>

void MakeSelectionTable(const char* XSecFile_in, const char* outTableFileName, const char* TTbar_insel, const char* WpJ_insel, const char* ZpJ_insel, const char* VVpJ_insel, const char* QCD_insel, const char* STopS_insel, const char* STopT_insel)
//// Produce the selection table given the XSections and selection results for the relevant processes.
//// NJets=number of final state jets (which pass the cuts).

{
  int NProcesses=7;
  double XSec[7];
  double FEff[7];
  TString token[7];
  //  istrstream str;
  int errCode=0;
  double fPass;
  double FracPassEl[7];
  double FracPassMu[7];
  double FracPassLp[7];
  int nJ=-1;
  char linein[200];
  TString PLabel[7];
  PLabel[0]="TTbar";
  PLabel[1]="WpJ";
  PLabel[2]="ZpJ";
  PLabel[3]="VVpJ";
  PLabel[4]="QCD";
  PLabel[5]="STopS";
  PLabel[6]="STopT";

  ofstream outTableFile;
  outTableFile.open(outTableFileName,ios::out);
  //outTableFile << "testing 1 2 3 " << endl;

  //read in the crosssection info
  ifstream XSecFile(XSecFile_in);
  XSecFile.getline(linein,200);
  XSecFile.getline(linein,200);
  istrstream str1(linein);
  str1 >> token[0] >> "=" >> XSec[0] >> token[1] >> "=" >> XSec[1] >> token[2] >> "=" >> XSec[2] >> token[3] >> "=" >> XSec[3] >> token[4] >> "=" >> XSec[4] >> token[5] >> "=" >> XSec[5] >> token[6] >> "=" >> XSec[6];
  for (Int_t i=0; i<NProcesses;i++) {
    if ( token[i]!=PLabel[i] ) {
      errCode=1;
    }
  }

  XSecFile.getline(linein,200);
  XSecFile.getline(linein,200);
  istrstream str2(linein);
  str2 >> token[0] >> "=" >> FEff[0] >> token[1] >> "=" >> FEff[1] >> token[2] >> "=" >> FEff[2] >> token[3] >> "=" >> FEff[3] >> token[4] >> "=" >> FEff[4] >> token[5] >> "=" >> FEff[5] >> token[6] >> "=" >> FEff[6];
  for (Int_t i=0; i<NProcesses;i++) {
    if ( token[i]!=PLabel[i] ) {
      errCode=2;
    }
  }

  // Read in the selection results
  ifstream sel[7];
  sel[0].open(TTbar_insel);
  sel[1].open(WpJ_insel);
  sel[2].open(ZpJ_insel);
  sel[3].open(VVpJ_insel);
  sel[4].open(QCD_insel);
  sel[5].open(STopS_insel);
  sel[6].open(STopT_insel);

  // loop over the desired jet counts: 1-5
  for (Int_t nj=1; nj<6;nj++) {
    // loop over the processes and extract the relevant fraction of passing events.
    for (Int_t i=0; i<NProcesses;i++) {
      sel[i].getline(linein,200);
      //read in the electrons
      sel[i].getline(linein,200);
      sel[i].getline(linein,200);
      istrstream strEl(linein);
      strEl >> nJ >> token[0] >> fPass ;
      if ( nJ!=nj ) {
	errCode=3;
	cout << "Error, Code=" << errCode << " i=" << i << " nJ=" << nJ << endl;
      } else {
	FracPassEl[i]=fPass;
	cout<< "FracPassEl " << FracPassEl[i] << endl;
      }
      //read in the muons
      sel[i].getline(linein,200);
      sel[i].getline(linein,200);
      istrstream strMu(linein);
      strMu >> nJ >> token[0] >> fPass ;
      if ( nJ!=nj ) {
	errCode=4;
	cout << "Error, Code=" << errCode << " i=" << i << " nJ=" << nJ << endl;
      } else {
	FracPassMu[i]=fPass;
	cout<< "FracPassMu " << FracPassMu[i] << endl;
      }
      //read in all leptons
      sel[i].getline(linein,200);
      sel[i].getline(linein,200);
      istrstream strLp(linein);
      strLp >> nJ >> token[0] >> fPass ;
      if ( nJ!=nj ) {
	errCode=5;
	cout << "Error, Code=" << errCode << " i=" << i << " nJ=" << nJ << endl;
      } else {
	FracPassLp[i]=fPass;
	cout<< "FracPassLp " << FracPassLp[i] << endl;
      }


    }
    // Make the table(s)

      outTableFile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
      outTableFile << "--------------------------------------------------------------------------------------------------------------------------" << endl;
      outTableFile << "__________________________________________________________________________________________________________________________" << endl;
      //outTableFile << "errCode=" << errCode << "  nJ=" << nJ << endl;
      if ( errCode==0 ) {
	outTableFile << nJ << " Jets : ELECTRONS" << endl;
	for (Int_t i=0; i<NProcesses;i++) {
	  outTableFile << PLabel[i] << " : " << XSec[i]*FracPassEl[i] << " Evts/pb^-1" << "   |   (FracPassEl=" << FracPassEl[i] << " , sigma_eff=" << XSec[i]*FEff[i] << ")" << endl;
	  outTableFile << " " << endl;
	}
      } else {
	outTableFile << "Error, Code=" << errCode << endl;
      }
      outTableFile << "__________________________________________________________________________________________________________________________" << endl;
      //outTableFile << "errCode=" << errCode << "  nJ=" << nJ << endl;
      if ( errCode==0 ) {
	outTableFile << nJ << " Jets : MUONS" << endl;
	for (Int_t i=0; i<NProcesses;i++) {
	  outTableFile << PLabel[i] << " : " << XSec[i]*FracPassMu[i] << " Evts/pb^-1" << "   |   (FracPassMu=" << FracPassMu[i] << " , sigma_eff=" << XSec[i]*FEff[i] << ")" << endl;
	  outTableFile << " " << endl;
	}
      } else {
	outTableFile << "Error, Code=" << errCode << endl;
      }
      outTableFile << "__________________________________________________________________________________________________________________________" << endl;
      //outTableFile << "errCode=" << errCode << "  nJ=" << nJ << endl;
      if ( errCode==0 ) {
	outTableFile << nJ << " Jets : LEPTONS" << endl;
	for (Int_t i=0; i<NProcesses;i++) {
	  outTableFile << PLabel[i] << " : " << XSec[i]*FracPassLp[i] << " Evts/pb^-1" << "   |   (FracPassLp=" << FracPassLp[i] << " , sigma_eff=" << XSec[i]*FEff[i] << ")" << endl;
	  outTableFile << " " << endl;
	}
      } else {
	outTableFile << "Error, Code=" << errCode << endl;
      }




  }

}
