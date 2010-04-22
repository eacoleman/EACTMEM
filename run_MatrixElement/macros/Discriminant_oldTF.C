#include "PeterTools/Fitter.hh"
#include "TStyle.h"
#include "TChain.h"
#include "TMath.h"
#include "TH1.h"
#include "TLegend.h"
#include "TCanvas.h"
#include <vector>
#include "TGraph.h"
#include "TH2.h"
#include "TMultiGraph.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <sstream>


using namespace std;

//////////////////////////////////////////////////////////////
//#include "PeterTools/Fitter.hh"
//Before, until I fix this code for being compiled
// root [0] .L ../shlib/libPeterTools.so
// root [1] .x Discriminant_schannelsignal.C
//////////////////////////////////////////////////////////////
void Discriminant_oldTF(TString higgsmass = "WH150")

  //newTF and new selection
  //old TF selection comparison

{
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  /////////////////////////////////////
  //                                 //
  //  EVENT PROBABILITY ROOT FILES   // 
  //                                 //
  /////////////////////////////////////
 
  //trying to reproduce Peter Discriminant!
  //**********WH**********************************
  TChain* WH_newSel_oldTF = new TChain("METree");
  TChain* WH_oldSel_oldTF = new TChain("METree");
  char file_newSel_oldTF[500];
  char file_oldSel_oldTF[500];
  unsigned int index_newSel_oldTF; 
  unsigned int index_oldSel_oldTF; 
  if(higgsmass == "WH115"){
    
    index_newSel_oldTF= 3;
    index_oldSel_oldTF= 1;
    sprintf(file_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH115.root"); 
    sprintf(file_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/WH115.root"); 
  }else if(higgsmass == "WH120"){

    index_newSel_oldTF= 4;
    index_oldSel_oldTF= 2;
    sprintf(file_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH120.root"); 
    sprintf(file_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/WH120.root"); 

  }else if(higgsmass == "WH150"){
    index_newSel_oldTF= 10;
    index_oldSel_oldTF= 5;
    sprintf(file_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH150.root"); 
    sprintf(file_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/WH150.root"); 

  }else{
    cout <<"Choose another Higgs Mass point " << endl;
    return;
  }

  WH_newSel_oldTF->Add(file_newSel_oldTF);
  cout << file_newSel_oldTF << endl;

  WH_oldSel_oldTF->Add(file_oldSel_oldTF);
  cout << file_oldSel_oldTF << endl;
  //*******bottom*********************
  TChain* Wbottom_newSel_oldTF = new TChain("METree");
  TChain* Wbottom_oldSel_oldTF = new TChain("METree");
  char file_Wbottom_newSel_oldTF[500];
  char file_Wbottom_oldSel_oldTF[500];

  sprintf(file_Wbottom_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*bottom.root"); 
  Wbottom_newSel_oldTF->Add(file_Wbottom_newSel_oldTF);
  cout << file_Wbottom_newSel_oldTF << endl;

  sprintf(file_Wbottom_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/Wbottom.root"); 
  Wbottom_oldSel_oldTF->Add(file_Wbottom_oldSel_oldTF);
  cout << file_Wbottom_oldSel_oldTF << endl;


  //*******charm*********************
  TChain* Wcharm_newSel_oldTF = new TChain("METree");
  TChain* Wcharm_oldSel_oldTF = new TChain("METree");
  char file_Wcharm_newSel_oldTF[500];
  char file_Wcharm_oldSel_oldTF[500];

  sprintf(file_Wcharm_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*charm.root"); 
  Wcharm_newSel_oldTF->Add(file_Wcharm_newSel_oldTF);
  cout << file_Wcharm_newSel_oldTF << endl;

  sprintf(file_Wcharm_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/Wcharm.root"); 
  Wcharm_oldSel_oldTF->Add(file_Wcharm_oldSel_oldTF);
  cout << file_Wcharm_oldSel_oldTF << endl;

  //*******schan*********************
  TChain* schan_newSel_oldTF = new TChain("METree");
  TChain* schan_oldSel_oldTF = new TChain("METree");
  char file_schan_newSel_oldTF[500];
  char file_schan_oldSel_oldTF[500];

  sprintf(file_schan_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_schan.root"); 
  schan_newSel_oldTF->Add(file_schan_newSel_oldTF);
  cout << file_schan_newSel_oldTF << endl;

  sprintf(file_schan_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/schan.root"); 
  schan_oldSel_oldTF->Add(file_schan_oldSel_oldTF);
  cout << file_schan_oldSel_oldTF << endl;

  //*******tchan*********************
  TChain* tchan_newSel_oldTF = new TChain("METree");
  TChain* tchan_oldSel_oldTF = new TChain("METree");
  char file_tchan_newSel_oldTF[500];
  char file_tchan_oldSel_oldTF[500];

  sprintf(file_tchan_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tchan.root"); 
  tchan_newSel_oldTF->Add(file_tchan_newSel_oldTF);
  cout << file_tchan_newSel_oldTF << endl;

  sprintf(file_tchan_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/tchan.root"); 
  tchan_oldSel_oldTF->Add(file_tchan_oldSel_oldTF);
  cout << file_tchan_oldSel_oldTF << endl;

  //*******diboson*********************
  TChain* diboson_newSel_oldTF = new TChain("METree");
  TChain* diboson_oldSel_oldTF = new TChain("METree");
  char file_diboson_newSel_oldTF[500];
  char file_diboson_oldSel_oldTF[500];

  sprintf(file_diboson_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_diboson.root"); 
  diboson_newSel_oldTF->Add(file_diboson_newSel_oldTF);
  cout << file_diboson_newSel_oldTF << endl;

  sprintf(file_diboson_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/WW.root"); 
  diboson_oldSel_oldTF->Add(file_diboson_oldSel_oldTF);
  cout << file_diboson_oldSel_oldTF << endl;
  sprintf(file_diboson_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/WZ.root"); 
  diboson_oldSel_oldTF->Add(file_diboson_oldSel_oldTF);
  cout << file_diboson_oldSel_oldTF << endl;
  sprintf(file_diboson_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/ZZ.root"); 
  diboson_oldSel_oldTF->Add(file_diboson_oldSel_oldTF);
  cout << file_diboson_oldSel_oldTF << endl;

  //*******Zjets*********************
  TChain* Zjets_newSel_oldTF = new TChain("METree");
  TChain* Zjets_oldSel_oldTF = new TChain("METree");
  char file_Zjets_newSel_oldTF[500];
  char file_Zjets_oldSel_oldTF[500];

  sprintf(file_Zjets_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_Zjets.root"); 
  Zjets_newSel_oldTF->Add(file_Zjets_newSel_oldTF);
  cout << file_Zjets_newSel_oldTF << endl;

  sprintf(file_Zjets_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/Zjets.root"); 
  Zjets_oldSel_oldTF->Add(file_Zjets_oldSel_oldTF);
  cout << file_Zjets_oldSel_oldTF << endl;

  //*******ttbar*********************
  TChain* ttbar_newSel_oldTF = new TChain("METree");
  TChain* ttbar_oldSel_oldTF = new TChain("METree");
  char file_ttbar_newSel_oldTF[500];
  char file_ttbar_oldSel_oldTF[500];

  sprintf(file_ttbar_newSel_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tt_*.root"); 
  ttbar_newSel_oldTF->Add(file_ttbar_newSel_oldTF);
  cout << file_ttbar_newSel_oldTF << endl;

  sprintf(file_ttbar_oldSel_oldTF,"/cdf/disk02/g6/casal/WH/tt*.root"); 
  ttbar_oldSel_oldTF->Add(file_ttbar_oldSel_oldTF);
  cout << file_ttbar_oldSel_oldTF << endl;
  /*
  cout <<"Total number of entries before cuts: "<< endl;
  cout << " WH, old selection      = " <<  WH_oldSel_oldTF->GetEntries() << ", new selection = " << WH_newSel_oldTF->GetEntries() << endl;
  cout << " Wbottom, old selection = " <<  Wbottom_oldSel_oldTF->GetEntries() << ", new selection = " << Wbottom_newSel_oldTF->GetEntries() << endl;
  cout << " Wcharm, old selection  = " <<  Wcharm_oldSel_oldTF->GetEntries() << ", new selection = " << Wcharm_newSel_oldTF->GetEntries() << endl;
  cout << " s-chan, old selection  = " <<  schan_oldSel_oldTF->GetEntries() << ", new selection = " << schan_newSel_oldTF->GetEntries() << endl;
  cout << " t-chan, old selection  = " <<  tchan_oldSel_oldTF->GetEntries() << ", new selection = " << tchan_newSel_oldTF->GetEntries() << endl;
  cout << " ttbar, old selection   = " <<  ttbar_oldSel_oldTF->GetEntries() << ", new selection = " << ttbar_newSel_oldTF->GetEntries() << endl;
  cout << " Z+jets, old selection  = " <<  Zjets_oldSel_oldTF->GetEntries() << ", new selection = " << Zjets_newSel_oldTF->GetEntries() << endl;
  cout << " Diboson, old selection = " <<  diboson_oldSel_oldTF->GetEntries() << ", new selection = " << diboson_newSel_oldTF->GetEntries() << endl;
  */

  const unsigned numberofbins = 30;     
  TCanvas *c_histo = new TCanvas("c_histo", "Discriminant", 1000, 800);
  c_histo->Divide(1,2);
  c_histo->cd(1);
  cout <<"*** double tag ****" << endl;

  //histo Wcharm new selection and old TF
  TH1F* Wcharm_newSel_oldTF_histo    = new TH1F("Wcharm_newSel_oldTF_histo", "Wcharm_newSel_oldTF_histo", numberofbins, 0, 1);
  Wcharm_newSel_oldTF_histo->SetNormFactor(1);
  Wcharm_newSel_oldTF_histo->SetLineWidth(2);
  Wcharm_newSel_oldTF_histo->SetLineColor(8);
  Wcharm_newSel_oldTF_histo->SetLineStyle(2);
  Wcharm_newSel_oldTF_histo->GetXaxis()->SetTitle("EPD");
  Wcharm_newSel_oldTF_histo->GetYaxis()->SetTitle("Normalized to unit area");
  Wcharm_newSel_oldTF_histo->SetTitle("Double Tag Events");

  //histo Wcharm old selection and old TF
  TH1F* Wcharm_oldSel_oldTF_histo    = new TH1F("Wcharm_oldSel_oldTF_histo", "Wcharm_oldSel_oldTF_histo", numberofbins, 0, 1);
  Wcharm_oldSel_oldTF_histo->SetNormFactor(1);
  Wcharm_oldSel_oldTF_histo->SetLineWidth(2);
  Wcharm_oldSel_oldTF_histo->SetLineColor(8);

  std::ostringstream str2tag_Wcharm_newSel_oldTF;
  str2tag_Wcharm_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>Wcharm_newSel_oldTF_histo";
  Wcharm_newSel_oldTF->Draw(str2tag_Wcharm_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)",""); 
  
  std::ostringstream str2tag_Wcharm_oldSel_oldTF;
  str2tag_Wcharm_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>Wcharm_oldSel_oldTF_histo";
  Wcharm_oldSel_oldTF->Draw(str2tag_Wcharm_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo Wbottom new selection and old TF
  TH1F* Wbottom_newSel_oldTF_histo    = new TH1F("Wbottom_newSel_oldTF_histo", "Wbottom_newSel_oldTF_histo", numberofbins, 0, 1);
  Wbottom_newSel_oldTF_histo->SetNormFactor(1);
  Wbottom_newSel_oldTF_histo->SetLineWidth(2);
  Wbottom_newSel_oldTF_histo->SetLineColor(8);
  Wbottom_newSel_oldTF_histo->SetLineStyle(2);

  //histo Wbottom old selection and old TF
  TH1F* Wbottom_oldSel_oldTF_histo    = new TH1F("Wbottom_oldSel_oldTF_histo", "Wbottom_oldSel_oldTF_histo", numberofbins, 0, 1);
  Wbottom_oldSel_oldTF_histo->SetNormFactor(1);
  Wbottom_oldSel_oldTF_histo->SetLineWidth(2);
  Wbottom_oldSel_oldTF_histo->SetLineColor(8);

  std::ostringstream str2tag_Wbottom_newSel_oldTF;
  str2tag_Wbottom_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>Wbottom_newSel_oldTF_histo";
  Wbottom_newSel_oldTF->Draw(str2tag_Wbottom_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Wbottom_oldSel_oldTF;
  str2tag_Wbottom_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>Wbottom_oldSel_oldTF_histo";
  Wbottom_oldSel_oldTF->Draw(str2tag_Wbottom_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //histo schan new selection and old TF
  TH1F* schan_newSel_oldTF_histo    = new TH1F("schan_newSel_oldTF_histo", "schan_newSel_oldTF_histo", numberofbins, 0, 1);
  schan_newSel_oldTF_histo->SetNormFactor(1);
  schan_newSel_oldTF_histo->SetLineWidth(2);
  schan_newSel_oldTF_histo->SetLineColor(102);
  schan_newSel_oldTF_histo->SetLineStyle(2);

  //histo schan old selection and old TF
  TH1F* schan_oldSel_oldTF_histo    = new TH1F("schan_oldSel_oldTF_histo", "schan_oldSel_oldTF_histo", numberofbins, 0, 1);
  schan_oldSel_oldTF_histo->SetNormFactor(1);
  schan_oldSel_oldTF_histo->SetLineWidth(2);
  schan_oldSel_oldTF_histo->SetLineColor(102);

  std::ostringstream str2tag_schan_newSel_oldTF;
  str2tag_schan_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>schan_newSel_oldTF_histo";
  schan_newSel_oldTF->Draw(str2tag_schan_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_schan_oldSel_oldTF;
  str2tag_schan_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>schan_oldSel_oldTF_histo";
  schan_oldSel_oldTF->Draw(str2tag_schan_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo tchan new selection and old TF
  TH1F* tchan_newSel_oldTF_histo    = new TH1F("tchan_newSel_oldTF_histo", "tchan_newSel_oldTF_histo", numberofbins, 0, 1);
  tchan_newSel_oldTF_histo->SetNormFactor(1);
  tchan_newSel_oldTF_histo->SetLineWidth(2);
  tchan_newSel_oldTF_histo->SetLineColor(96);
  tchan_newSel_oldTF_histo->SetLineStyle(2);

  //histo tchan old selection and old TF
  TH1F* tchan_oldSel_oldTF_histo    = new TH1F("tchan_oldSel_oldTF_histo", "tchan_oldSel_oldTF_histo", numberofbins, 0, 1);
  tchan_oldSel_oldTF_histo->SetNormFactor(1);
  tchan_oldSel_oldTF_histo->SetLineWidth(2);
  tchan_oldSel_oldTF_histo->SetLineColor(96);

  std::ostringstream str2tag_tchan_newSel_oldTF;
  str2tag_tchan_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>tchan_newSel_oldTF_histo";
  tchan_newSel_oldTF->Draw(str2tag_tchan_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_tchan_oldSel_oldTF;
  str2tag_tchan_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>tchan_oldSel_oldTF_histo";
  tchan_oldSel_oldTF->Draw(str2tag_tchan_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo diboson new selection and old TF
  TH1F* diboson_newSel_oldTF_histo    = new TH1F("diboson_newSel_oldTF_histo", "diboson_newSel_oldTF_histo", numberofbins, 0, 1);
  diboson_newSel_oldTF_histo->SetNormFactor(1);
  diboson_newSel_oldTF_histo->SetLineWidth(2);
  diboson_newSel_oldTF_histo->SetLineColor(41);
  diboson_newSel_oldTF_histo->SetLineStyle(2);

  //histo diboson old selection and old TF
  TH1F* diboson_oldSel_oldTF_histo    = new TH1F("diboson_oldSel_oldTF_histo", "diboson_oldSel_oldTF_histo", numberofbins, 0, 1);
  diboson_oldSel_oldTF_histo->SetNormFactor(1);
  diboson_oldSel_oldTF_histo->SetLineWidth(2);
  diboson_oldSel_oldTF_histo->SetLineColor(41);

  std::ostringstream str2tag_diboson_newSel_oldTF;
  str2tag_diboson_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>diboson_newSel_oldTF_histo";
  diboson_newSel_oldTF->Draw(str2tag_diboson_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_diboson_oldSel_oldTF;
  str2tag_diboson_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>diboson_oldSel_oldTF_histo";
  diboson_oldSel_oldTF->Draw(str2tag_diboson_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo Zjets new selection and old TF
  TH1F* Zjets_newSel_oldTF_histo    = new TH1F("Zjets_newSel_oldTF_histo", "Zjets_newSel_oldTF_histo", numberofbins, 0, 1);
  Zjets_newSel_oldTF_histo->SetNormFactor(1);
  Zjets_newSel_oldTF_histo->SetLineWidth(2);
  Zjets_newSel_oldTF_histo->SetLineColor(4);
  Zjets_newSel_oldTF_histo->SetLineStyle(2);

  //histo Zjets old selection and old TF
  TH1F* Zjets_oldSel_oldTF_histo    = new TH1F("Zjets_oldSel_oldTF_histo", "Zjets_oldSel_oldTF_histo", numberofbins, 0, 1);
  Zjets_oldSel_oldTF_histo->SetNormFactor(1);
  Zjets_oldSel_oldTF_histo->SetLineWidth(2);
  Zjets_oldSel_oldTF_histo->SetLineColor(4);

  std::ostringstream str2tag_Zjets_newSel_oldTF;
  str2tag_Zjets_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>Zjets_newSel_oldTF_histo";
  Zjets_newSel_oldTF->Draw(str2tag_Zjets_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Zjets_oldSel_oldTF;
  str2tag_Zjets_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>Zjets_oldSel_oldTF_histo";
  Zjets_oldSel_oldTF->Draw(str2tag_Zjets_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo ttbar new selection and old TF
  TH1F* ttbar_newSel_oldTF_histo    = new TH1F("ttbar_newSel_oldTF_histo", "ttbar_newSel_oldTF_histo", numberofbins, 0, 1);
  ttbar_newSel_oldTF_histo->SetNormFactor(1);
  ttbar_newSel_oldTF_histo->SetLineWidth(2);
  ttbar_newSel_oldTF_histo->SetLineColor(37);
  ttbar_newSel_oldTF_histo->SetLineStyle(2);

  //histo ttbar old selection and old TF
  TH1F* ttbar_oldSel_oldTF_histo    = new TH1F("ttbar_oldSel_oldTF_histo", "ttbar_oldSel_oldTF_histo", numberofbins, 0, 1);
  ttbar_oldSel_oldTF_histo->SetNormFactor(1);
  ttbar_oldSel_oldTF_histo->SetLineWidth(2);
  ttbar_oldSel_oldTF_histo->SetLineColor(37);

  std::ostringstream str2tag_ttbar_newSel_oldTF;
  str2tag_ttbar_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>ttbar_newSel_oldTF_histo";
  ttbar_newSel_oldTF->Draw(str2tag_ttbar_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_ttbar_oldSel_oldTF;
  str2tag_ttbar_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>ttbar_oldSel_oldTF_histo";
  ttbar_oldSel_oldTF->Draw(str2tag_ttbar_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 


  //histo WH new selection and old TF
  TH1F* WH_newSel_oldTF_histo    = new TH1F("WH_newSel_oldTF_histo", "WH_newSel_oldTF_histo", numberofbins, 0, 1);
  WH_newSel_oldTF_histo->SetNormFactor(1);
  WH_newSel_oldTF_histo->SetLineWidth(2);
  WH_newSel_oldTF_histo->SetLineColor(2);
  WH_newSel_oldTF_histo->SetLineStyle(2);

  //histo WH old selection and old TF
  TH1F* WH_oldSel_oldTF_histo    = new TH1F("WH_oldSel_oldTF_histo", "WH_oldSel_oldTF_histo", numberofbins, 0, 1);
  WH_oldSel_oldTF_histo->SetNormFactor(1);
  WH_oldSel_oldTF_histo->SetLineWidth(2);
  WH_oldSel_oldTF_histo->SetLineColor(2);

  std::ostringstream str2tag_WH_newSel_oldTF;
  str2tag_WH_newSel_oldTF<< "(epd2tagWH[" << index_newSel_oldTF << "])>>WH_newSel_oldTF_histo";
  WH_newSel_oldTF->Draw(str2tag_WH_newSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_WH_oldSel_oldTF;
  str2tag_WH_oldSel_oldTF<< "(epd2tagWH[" << index_oldSel_oldTF << "])>>WH_oldSel_oldTF_histo";
  WH_oldSel_oldTF->Draw(str2tag_WH_oldSel_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  

  TLegend *leg1 = new TLegend(0.15, 0.35, 0.44, 0.87);
  TLegend *leg = new TLegend(0.44, 0.34, 0.72, 0.86);
  if(higgsmass == "WH115"){
    leg1->AddEntry(WH_oldSel_oldTF_histo, "WH (115GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newSel_oldTF_histo, "WH (115GeV) 2.7 1/fb TF(Ep, Ej)","l");
  }else if(higgsmass == "WH120"){
    leg1->AddEntry(WH_oldSel_oldTF_histo, "WH (120GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newSel_oldTF_histo, "WH (120GeV) 2.7 1/fb TF(Ep, Ej)","l");
  }else if(higgsmass == "WH150"){
    leg1->AddEntry(WH_oldSel_oldTF_histo, "WH (150GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newSel_oldTF_histo, "WH (150GeV) 2.7 1/fb TF(Ep, Ej)","l");
  }else {
    cout <<"Error ..." << endl;
    return;
  }
  leg1->AddEntry(Wbottom_oldSel_oldTF_histo, "Wbb 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(Wcharm_oldSel_oldTF_histo, "Wcharm 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(schan_oldSel_oldTF_histo, "s-chan 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(tchan_oldSel_oldTF_histo, "t-chan 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(diboson_oldSel_oldTF_histo, "diboson 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(Zjets_oldSel_oldTF_histo, "Z+jets 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(ttbar_oldSel_oldTF_histo, "ttbar 2.2 1/fb TF(Ep, Ej)","l");
  leg1->SetFillColor(10);
  leg1->Draw("same");

  leg->AddEntry(Wbottom_newSel_oldTF_histo, "Wbb 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(Wcharm_newSel_oldTF_histo, "Wcharm 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(schan_newSel_oldTF_histo, "s-chan 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(tchan_newSel_oldTF_histo, "t-chan 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(diboson_newSel_oldTF_histo, "diboson 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(Zjets_newSel_oldTF_histo, "Z+jets 2.7 1/fb TF(Ep, Ej)","l");
  leg->AddEntry(ttbar_newSel_oldTF_histo, "ttbar 2.7 1/fb TF(Ep, Ej)","l");
  leg->SetFillColor(10);
  leg->Draw("same");




  c_histo->cd(2);

  double WH_newsel_total      = -999.;
  double WH_oldsel_total      = -999.;
  double Wbottom_newsel_total = -999.;
  double Wbottom_oldsel_total = -999.;
  double Wcharm_newsel_total  = -999.;
  double Wcharm_oldsel_total  = -999.;
  double ttbar_newsel_total   = -999.;
  double ttbar_oldsel_total   = -999.;
  double schan_newsel_total   = -999.;
  double schan_oldsel_total   = -999.;
  double tchan_newsel_total   = -999.;
  double tchan_oldsel_total   = -999.;
  double diboson_newsel_total = -999.;
  double diboson_oldsel_total = -999.;
  double Zjets_newsel_total   = -999.;
  double Zjets_oldsel_total   = -999.;

  double WH_newsel      = -999.;
  double WH_oldsel      = -999.;
  double Wbottom_newsel = -999.;
  double Wbottom_oldsel = -999.;
  double Wcharm_newsel  = -999.;
  double Wcharm_oldsel  = -999.;
  double ttbar_newsel   = -999.;
  double ttbar_oldsel   = -999.;
  double schan_newsel   = -999.;
  double schan_oldsel   = -999.;
  double tchan_newsel   = -999.;
  double tchan_oldsel   = -999.;
  double diboson_newsel = -999.;
  double diboson_oldsel = -999.;
  double Zjets_newsel   = -999.;
  double Zjets_oldsel   = -999.;

  WH_newsel_total      = WH_newSel_oldTF_histo->Integral(1, numberofbins);
  WH_oldsel_total      = WH_oldSel_oldTF_histo->Integral(1, numberofbins);
  Wbottom_newsel_total = Wbottom_newSel_oldTF_histo->Integral(1, numberofbins);
  Wbottom_oldsel_total = Wbottom_oldSel_oldTF_histo->Integral(1, numberofbins);
  Wcharm_newsel_total  = Wcharm_newSel_oldTF_histo->Integral(1, numberofbins);
  Wcharm_oldsel_total  = Wcharm_oldSel_oldTF_histo->Integral(1, numberofbins);
  ttbar_newsel_total   = ttbar_newSel_oldTF_histo->Integral(1, numberofbins);
  ttbar_oldsel_total   = ttbar_oldSel_oldTF_histo->Integral(1, numberofbins);
  schan_newsel_total   = schan_newSel_oldTF_histo->Integral(1, numberofbins);
  schan_oldsel_total   = schan_oldSel_oldTF_histo->Integral(1, numberofbins);
  tchan_newsel_total   = tchan_newSel_oldTF_histo->Integral(1, numberofbins);
  tchan_oldsel_total   = tchan_oldSel_oldTF_histo->Integral(1, numberofbins);
  diboson_newsel_total = diboson_newSel_oldTF_histo->Integral(1, numberofbins);
  diboson_oldsel_total = diboson_oldSel_oldTF_histo->Integral(1, numberofbins); 
  Zjets_newsel_total   = Zjets_newSel_oldTF_histo->Integral(1, numberofbins);
  Zjets_oldsel_total   = Zjets_oldSel_oldTF_histo->Integral(1, numberofbins);



  double significance_newsel[numberofbins];
  double significance_oldsel[numberofbins];
  double newdiscr[numberofbins];
  double bkg_newsel = -999.;
  double bkg_oldsel = -999.;
  
  //inizialize
  for(int i=0; i<30; i++){
    significance_newsel[i] = -999.;
    significance_oldsel[i]= -999.;
    newdiscr[i]= -999.;
  }

  for(int i=0; i<30; i++){
    newdiscr[i] =(i/30.)+((1/30.)/2.);

    //signal
    WH_newsel      = WH_newSel_oldTF_histo->Integral(i+1, numberofbins)/WH_newsel_total;
    WH_oldsel      = WH_oldSel_oldTF_histo->Integral(i+1, numberofbins)/WH_oldsel_total;

    //bkg
    Wbottom_newsel = .47*Wbottom_newSel_oldTF_histo->Integral(i+1, numberofbins)/ Wbottom_newsel_total;
    Wbottom_oldsel = .47*Wbottom_oldSel_oldTF_histo->Integral(i+1, numberofbins)/Wbottom_oldsel_total;
    Wcharm_newsel  = .05*Wcharm_newSel_oldTF_histo->Integral(i+1, numberofbins)/Wcharm_newsel_total;
    Wcharm_oldsel  = .05*Wcharm_oldSel_oldTF_histo->Integral(i+1, numberofbins)/Wcharm_oldsel_total;
    ttbar_newsel   = .32*ttbar_newSel_oldTF_histo->Integral(i+1, numberofbins)/ttbar_newsel_total;
    ttbar_oldsel   = .32*ttbar_oldSel_oldTF_histo->Integral(i+1, numberofbins)/ttbar_oldsel_total;
    schan_newsel   = .09*schan_newSel_oldTF_histo->Integral(i+1, numberofbins)/schan_newsel_total;
    schan_oldsel   = .09*schan_oldSel_oldTF_histo->Integral(i+1, numberofbins)/schan_oldsel_total;
    tchan_newsel   = .01*tchan_newSel_oldTF_histo->Integral(i+1, numberofbins)/tchan_newsel_total;
    tchan_oldsel   = .01*tchan_oldSel_oldTF_histo->Integral(i+1, numberofbins)/tchan_oldsel_total;
    diboson_newsel = .05*diboson_newSel_oldTF_histo->Integral(i+1, numberofbins)/diboson_newsel_total;
    diboson_oldsel = .05*diboson_oldSel_oldTF_histo->Integral(i+1, numberofbins)/diboson_oldsel_total; 
    Zjets_newsel   = .01*Zjets_newSel_oldTF_histo->Integral(i+1, numberofbins)/Zjets_newsel_total;
    Zjets_oldsel   = .01*Zjets_oldSel_oldTF_histo->Integral(i+1, numberofbins)/Zjets_oldsel_total;
    
    bkg_newsel = Wbottom_newsel + Wcharm_newsel  + ttbar_newsel +  schan_newsel + tchan_newsel + diboson_newsel + Zjets_newsel;
    bkg_oldsel = Wbottom_oldsel + Wcharm_oldsel  + ttbar_oldsel +  schan_oldsel + tchan_oldsel + diboson_oldsel + Zjets_oldsel;

    if(bkg_newsel == 0) bkg_newsel = 0.00001;
    if(bkg_oldsel == 0) bkg_oldsel = 0.00001;

    significance_newsel[i] = WH_newsel/sqrt(bkg_newsel);
    significance_oldsel[i] = WH_oldsel/sqrt(bkg_oldsel);
    //    cout << "significance_newsel " << significance_newsel[i] << ", significance_oldsel  "<< significance_oldsel[i] <<  endl;     
  }


  TH2F *hr = new TH2F("hr","Significance",10,0,1,6,0.,2.);

  hr->GetXaxis()->SetNdivisions(510);
  hr->GetYaxis()->SetLabelFont(72);
  hr->SetTitle("N_{S}(EPD #geq EPD_{cut})/ sqrt[N_{B}(EPD #geq EPD_{cut})]");
  hr->GetYaxis()->SetTitleSize(0.05);
  hr->GetYaxis()->SetTitle("N_{S}/ #sqrt{N_{B}}");
  hr->GetXaxis()->SetTitleSize(0.05);
  hr->GetXaxis()->SetTitle("EPD_{cut}");
  hr->Draw();

  TGraph *signalandbkg_newsel = new TGraph(numberofbins, newdiscr, significance_newsel);
  TGraph *signalandbkg_oldsel = new TGraph(numberofbins, newdiscr, significance_oldsel);

  gPad->SetGrid(2);
  signalandbkg_newsel->SetLineColor(4);
  signalandbkg_newsel->SetLineStyle(2);
  signalandbkg_newsel->SetLineWidth(2);
  signalandbkg_oldsel->SetLineWidth(2);
  signalandbkg_oldsel->SetMarkerStyle(3);
  signalandbkg_newsel->SetMarkerStyle(23);
  signalandbkg_newsel->SetMarkerColor(4);
 
  TMultiGraph *mg1 = new  TMultiGraph();
  mg1->Add(signalandbkg_newsel); 
  mg1->Add(signalandbkg_oldsel); 
  mg1->Draw("PC");

  TLegend *legend = new TLegend(0.26, 0.28, 0.63, 0.52);
  legend->AddEntry(signalandbkg_newsel, "2.7 1/fb TF(Ep, Ej)", "l");
  legend->AddEntry(signalandbkg_oldsel, "2.2 1/fb TF(Ep, Ej)", "l");
  legend->SetFillColor(10);
  legend->Draw("same");

  c_histo->Print("blessing_plots/Discr_oldTF"+higgsmass+".eps");
  c_histo->Print("blessing_plots/Discr_oldTF"+higgsmass+".gif");


}
