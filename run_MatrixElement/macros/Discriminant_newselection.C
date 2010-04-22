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
void Discriminant_newselection(TString higgsmass = "WH150")

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
  TChain* WH_newTF = new TChain("METree");
  TChain* WH_oldTF = new TChain("METree");
  char file_newTF[500];
  char file_oldTF[500];
  unsigned int index; 
  if(higgsmass == "WH115"){
    
    index = 3;
    sprintf(file_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH115GeV.root"); 
    sprintf(file_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH115.root"); 
  }else if(higgsmass == "WH120"){

    index = 4;
    sprintf(file_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH120GeV.root"); 
    sprintf(file_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH120.root"); 

  }else if(higgsmass == "WH150"){
    index = 10;
    sprintf(file_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH150GeV.root"); 
    sprintf(file_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH150.root"); 

  }else{
    cout <<"Choose another Higgs Mass point " << endl;
    return;
  }

  WH_newTF->Add(file_newTF);
  cout << file_newTF << endl;

  WH_oldTF->Add(file_oldTF);
  cout << file_oldTF << endl;
  //*******bottom*********************
  TChain* Wbottom_newTF = new TChain("METree");
  TChain* Wbottom_oldTF = new TChain("METree");
  char file_Wbottom_newTF[500];
  char file_Wbottom_oldTF[500];

  sprintf(file_Wbottom_newTF,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*bottom.root"); 
  Wbottom_newTF->Add(file_Wbottom_newTF);
  cout << file_Wbottom_newTF << endl;

  sprintf(file_Wbottom_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*bottom.root"); 
  Wbottom_oldTF->Add(file_Wbottom_oldTF);
  cout << file_Wbottom_oldTF << endl;


  //*******charm*********************
  TChain* Wcharm_newTF = new TChain("METree");
  TChain* Wcharm_oldTF = new TChain("METree");
  char file_Wcharm_newTF[500];
  char file_Wcharm_oldTF[500];

  sprintf(file_Wcharm_newTF,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*charm.root"); 
  Wcharm_newTF->Add(file_Wcharm_newTF);
  cout << file_Wcharm_newTF << endl;

  sprintf(file_Wcharm_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*charm.root"); 
  Wcharm_oldTF->Add(file_Wcharm_oldTF);
  cout << file_Wcharm_oldTF << endl;

  //*******schan*********************
  TChain* schan_newTF = new TChain("METree");
  TChain* schan_oldTF = new TChain("METree");
  char file_schan_newTF[500];
  char file_schan_oldTF[500];

  sprintf(file_schan_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_schan.root"); 
  schan_newTF->Add(file_schan_newTF);
  cout << file_schan_newTF << endl;

  sprintf(file_schan_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_schan.root"); 
  schan_oldTF->Add(file_schan_oldTF);
  cout << file_schan_oldTF << endl;

  //*******tchan*********************
  TChain* tchan_newTF = new TChain("METree");
  TChain* tchan_oldTF = new TChain("METree");
  char file_tchan_newTF[500];
  char file_tchan_oldTF[500];

  sprintf(file_tchan_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tchan.root"); 
  tchan_newTF->Add(file_tchan_newTF);
  cout << file_tchan_newTF << endl;

  sprintf(file_tchan_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tchan.root"); 
  tchan_oldTF->Add(file_tchan_oldTF);
  cout << file_tchan_oldTF << endl;

  //*******diboson*********************
  TChain* diboson_newTF = new TChain("METree");
  TChain* diboson_oldTF = new TChain("METree");
  char file_diboson_newTF[500];
  char file_diboson_oldTF[500];

  sprintf(file_diboson_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_diboson.root"); 
  diboson_newTF->Add(file_diboson_newTF);
  cout << file_diboson_newTF << endl;

  sprintf(file_diboson_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_diboson.root"); 
  diboson_oldTF->Add(file_diboson_oldTF);
  cout << file_diboson_oldTF << endl;

  //*******Zjets*********************
  TChain* Zjets_newTF = new TChain("METree");
  TChain* Zjets_oldTF = new TChain("METree");
  char file_Zjets_newTF[500];
  char file_Zjets_oldTF[500];

  sprintf(file_Zjets_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_Zjets.root"); 
  Zjets_newTF->Add(file_Zjets_newTF);
  cout << file_Zjets_newTF << endl;

  sprintf(file_Zjets_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_Zjets.root"); 
  Zjets_oldTF->Add(file_Zjets_oldTF);
  cout << file_Zjets_oldTF << endl;

  //*******ttbar*********************
  TChain* ttbar_newTF = new TChain("METree");
  TChain* ttbar_oldTF = new TChain("METree");
  char file_ttbar_newTF[500];
  char file_ttbar_oldTF[500];

  sprintf(file_ttbar_newTF,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tt_*.root"); 
  ttbar_newTF->Add(file_ttbar_newTF);
  cout << file_ttbar_newTF << endl;

  sprintf(file_ttbar_oldTF,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tt_*.root"); 
  ttbar_oldTF->Add(file_ttbar_oldTF);
  cout << file_ttbar_oldTF << endl;
  
  cout <<"Total number of entries before cuts: "<< endl;
  cout << " WH, old TF      = " <<  WH_oldTF->GetEntries() << ", new TF = " << WH_newTF->GetEntries() << endl;
  cout << " Wbottom, old TF = " <<  Wbottom_oldTF->GetEntries() << ", new TF = " << Wbottom_newTF->GetEntries() << endl;
  cout << " Wcharm, old TF  = " <<  Wcharm_oldTF->GetEntries() << ", new TF = " << Wcharm_newTF->GetEntries() << endl;
  cout << " s-chan, old TF  = " <<  schan_oldTF->GetEntries() << ", new TF = " << schan_newTF->GetEntries() << endl;
  cout << " t-chan, old TF  = " <<  tchan_oldTF->GetEntries() << ", new TF = " << tchan_newTF->GetEntries() << endl;
  cout << " ttbar, old TF   = " <<  ttbar_oldTF->GetEntries() << ", new TF = " << ttbar_newTF->GetEntries() << endl;
  cout << " Z+jets, old TF  = " <<  Zjets_oldTF->GetEntries() << ", new TF = " << Zjets_newTF->GetEntries() << endl;
  cout << " Diboson, old TF = " <<  diboson_oldTF->GetEntries() << ", new TF = " << diboson_newTF->GetEntries() << endl;
  

  const unsigned numberofbins = 30;     
  TCanvas *c_histo = new TCanvas("c_histo", "Discriminant", 1000, 800);
  c_histo->Divide(1,2);
  c_histo->cd(1);
  cout <<"*** double tag ****" << endl;
  //histo Wcharm new selection and old TF
  TH1F* Wcharm_newTF_histo    = new TH1F("Wcharm_newTF_histo", "Wcharm_newTF_histo", numberofbins, 0, 1);
  Wcharm_newTF_histo->SetNormFactor(1);
  Wcharm_newTF_histo->SetLineWidth(2);
  Wcharm_newTF_histo->SetLineColor(8);
  Wcharm_newTF_histo->SetLineStyle(2);
  Wcharm_newTF_histo->GetXaxis()->SetTitle("EPD");
  Wcharm_newTF_histo->GetYaxis()->SetTitle("Normalized to unit area");
  Wcharm_newTF_histo->SetTitle("Double Tag Events");

  //histo Wcharm old selection and old TF
  TH1F* Wcharm_oldTF_histo    = new TH1F("Wcharm_oldTF_histo", "Wcharm_oldTF_histo", numberofbins, 0, 1);
  Wcharm_oldTF_histo->SetNormFactor(1);
  Wcharm_oldTF_histo->SetLineWidth(2);
  Wcharm_oldTF_histo->SetLineColor(8);

  std::ostringstream str2tag_Wcharm_newTF;
  str2tag_Wcharm_newTF<< "(epd2tagWH[" << index << "])>>Wcharm_newTF_histo";
  Wcharm_newTF->Draw(str2tag_Wcharm_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)",""); 
  
  std::ostringstream str2tag_Wcharm_oldTF;
  str2tag_Wcharm_oldTF<< "(epd2tagWH[" << index << "])>>Wcharm_oldTF_histo";
  Wcharm_oldTF->Draw(str2tag_Wcharm_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo ttbar new selection and old TF
  TH1F* ttbar_newTF_histo    = new TH1F("ttbar_newTF_histo", "ttbar_newTF_histo", numberofbins, 0, 1);
  ttbar_newTF_histo->SetNormFactor(1);
  ttbar_newTF_histo->SetLineWidth(2);
  ttbar_newTF_histo->SetLineColor(37);
  ttbar_newTF_histo->SetLineStyle(2);
  ttbar_newTF_histo->SetTitle("Double Tag Events");
  ttbar_newTF_histo->GetXaxis()->SetTitle("EPD");

  //histo ttbar old selection and old TF
  TH1F* ttbar_oldTF_histo    = new TH1F("ttbar_oldTF_histo", "ttbar_oldTF_histo", numberofbins, 0, 1);
  ttbar_oldTF_histo->SetNormFactor(1);
  ttbar_oldTF_histo->SetLineWidth(2);
  ttbar_oldTF_histo->SetLineColor(37);

  std::ostringstream str2tag_ttbar_newTF;
  str2tag_ttbar_newTF<< "(epd2tagWH[" << index << "])>>ttbar_newTF_histo";
  ttbar_newTF->Draw(str2tag_ttbar_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_ttbar_oldTF;
  str2tag_ttbar_oldTF<< "(epd2tagWH[" << index << "])>>ttbar_oldTF_histo";
  ttbar_oldTF->Draw(str2tag_ttbar_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 




  //histo Zjets new selection and old TF
  TH1F* Zjets_newTF_histo    = new TH1F("Zjets_newTF_histo", "Zjets_newTF_histo", numberofbins, 0, 1);
  Zjets_newTF_histo->SetNormFactor(1);
  Zjets_newTF_histo->SetLineWidth(2);
  Zjets_newTF_histo->SetLineColor(4);
  Zjets_newTF_histo->SetLineStyle(2);

  //histo Zjets old selection and old TF
  TH1F* Zjets_oldTF_histo    = new TH1F("Zjets_oldTF_histo", "Zjets_oldTF_histo", numberofbins, 0, 1);
  Zjets_oldTF_histo->SetNormFactor(1);
  Zjets_oldTF_histo->SetLineWidth(2);
  Zjets_oldTF_histo->SetLineColor(4);

  std::ostringstream str2tag_Zjets_newTF;
  str2tag_Zjets_newTF<< "(epd2tagWH[" << index << "])>>Zjets_newTF_histo";
  Zjets_newTF->Draw(str2tag_Zjets_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Zjets_oldTF;
  str2tag_Zjets_oldTF<< "(epd2tagWH[" << index << "])>>Zjets_oldTF_histo";
  Zjets_oldTF->Draw(str2tag_Zjets_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 



  //histo Wbottom new selection and old TF
  TH1F* Wbottom_newTF_histo    = new TH1F("Wbottom_newTF_histo", "Wbottom_newTF_histo", numberofbins, 0, 1);
  Wbottom_newTF_histo->SetNormFactor(1);
  Wbottom_newTF_histo->SetLineWidth(2);
  Wbottom_newTF_histo->SetLineColor(8);
  Wbottom_newTF_histo->SetLineStyle(2);

  //histo Wbottom old selection and old TF
  TH1F* Wbottom_oldTF_histo    = new TH1F("Wbottom_oldTF_histo", "Wbottom_oldTF_histo", numberofbins, 0, 1);
  Wbottom_oldTF_histo->SetNormFactor(1);
  Wbottom_oldTF_histo->SetLineWidth(2);
  Wbottom_oldTF_histo->SetLineColor(8);

  std::ostringstream str2tag_Wbottom_newTF;
  str2tag_Wbottom_newTF<< "(epd2tagWH[" << index << "])>>Wbottom_newTF_histo";
  Wbottom_newTF->Draw(str2tag_Wbottom_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Wbottom_oldTF;
  str2tag_Wbottom_oldTF<< "(epd2tagWH[" << index << "])>>Wbottom_oldTF_histo";
  Wbottom_oldTF->Draw(str2tag_Wbottom_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //histo schan new selection and old TF
  TH1F* schan_newTF_histo    = new TH1F("schan_newTF_histo", "schan_newTF_histo", numberofbins, 0, 1);
  schan_newTF_histo->SetNormFactor(1);
  schan_newTF_histo->SetLineWidth(2);
  schan_newTF_histo->SetLineColor(102);
  schan_newTF_histo->SetLineStyle(2);

  //histo schan old selection and old TF
  TH1F* schan_oldTF_histo    = new TH1F("schan_oldTF_histo", "schan_oldTF_histo", numberofbins, 0, 1);
  schan_oldTF_histo->SetNormFactor(1);
  schan_oldTF_histo->SetLineWidth(2);
  schan_oldTF_histo->SetLineColor(102);

  std::ostringstream str2tag_schan_newTF;
  str2tag_schan_newTF<< "(epd2tagWH[" << index << "])>>schan_newTF_histo";
  schan_newTF->Draw(str2tag_schan_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_schan_oldTF;
  str2tag_schan_oldTF<< "(epd2tagWH[" << index << "])>>schan_oldTF_histo";
  schan_oldTF->Draw(str2tag_schan_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo tchan new selection and old TF
  TH1F* tchan_newTF_histo    = new TH1F("tchan_newTF_histo", "tchan_newTF_histo", numberofbins, 0, 1);
  tchan_newTF_histo->SetNormFactor(1);
  tchan_newTF_histo->SetLineWidth(2);
  tchan_newTF_histo->SetLineColor(96);
  tchan_newTF_histo->SetLineStyle(2);

  //histo tchan old selection and old TF
  TH1F* tchan_oldTF_histo    = new TH1F("tchan_oldTF_histo", "tchan_oldTF_histo", numberofbins, 0, 1);
  tchan_oldTF_histo->SetNormFactor(1);
  tchan_oldTF_histo->SetLineWidth(2);
  tchan_oldTF_histo->SetLineColor(96);

  std::ostringstream str2tag_tchan_newTF;
  str2tag_tchan_newTF<< "(epd2tagWH[" << index << "])>>tchan_newTF_histo";
  tchan_newTF->Draw(str2tag_tchan_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_tchan_oldTF;
  str2tag_tchan_oldTF<< "(epd2tagWH[" << index << "])>>tchan_oldTF_histo";
  tchan_oldTF->Draw(str2tag_tchan_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 

  //histo diboson new selection and old TF
  TH1F* diboson_newTF_histo    = new TH1F("diboson_newTF_histo", "diboson_newTF_histo", numberofbins, 0, 1);
  diboson_newTF_histo->SetNormFactor(1);
  diboson_newTF_histo->SetLineWidth(2);
  diboson_newTF_histo->SetLineColor(41);
  diboson_newTF_histo->SetLineStyle(2);

  //histo diboson old selection and old TF
  TH1F* diboson_oldTF_histo    = new TH1F("diboson_oldTF_histo", "diboson_oldTF_histo", numberofbins, 0, 1);
  diboson_oldTF_histo->SetNormFactor(1);
  diboson_oldTF_histo->SetLineWidth(2);
  diboson_oldTF_histo->SetLineColor(41);

  std::ostringstream str2tag_diboson_newTF;
  str2tag_diboson_newTF<< "(epd2tagWH[" << index << "])>>diboson_newTF_histo";
  diboson_newTF->Draw(str2tag_diboson_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_diboson_oldTF;
  str2tag_diboson_oldTF<< "(epd2tagWH[" << index << "])>>diboson_oldTF_histo";
  diboson_oldTF->Draw(str2tag_diboson_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 


  //histo WH new selection and old TF
  TH1F* WH_newTF_histo    = new TH1F("WH_newTF_histo", "WH_newTF_histo", numberofbins, 0, 1);
  WH_newTF_histo->SetNormFactor(1);
  WH_newTF_histo->SetLineWidth(2);
  WH_newTF_histo->SetLineColor(2);
  WH_newTF_histo->SetLineStyle(2);

  //histo WH old selection and old TF
  TH1F* WH_oldTF_histo    = new TH1F("WH_oldTF_histo", "WH_oldTF_histo", numberofbins, 0, 1);
  WH_oldTF_histo->SetNormFactor(1);
  WH_oldTF_histo->SetLineWidth(2);
  WH_oldTF_histo->SetLineColor(2);

  std::ostringstream str2tag_WH_newTF;
  str2tag_WH_newTF<< "(epd2tagWH[" << index << "])>>WH_newTF_histo";
  WH_newTF->Draw(str2tag_WH_newTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_WH_oldTF;
  str2tag_WH_oldTF<< "(epd2tagWH[" << index << "])>>WH_oldTF_histo";
  WH_oldTF->Draw(str2tag_WH_oldTF.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  

  TLegend *leg1 = new TLegend(0.15, 0.35, 0.44, 0.87);
  TLegend *leg = new TLegend(0.44, 0.34, 0.76, 0.86);
  if(higgsmass == "WH115"){
    leg1->AddEntry(WH_oldTF_histo, "WH (115GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newTF_histo, "WH (115GeV) 2.7 1/fb TF(Ep, NNoutput)","l");
  }else if(higgsmass == "WH120"){
    leg1->AddEntry(WH_oldTF_histo, "WH (120GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newTF_histo, "WH (120GeV) 2.7 1/fb TF(Ep, NNoutput)","l");
  }else if(higgsmass == "WH150"){
    leg1->AddEntry(WH_oldTF_histo, "WH (150GeV) 2.2 1/fb TF(Ep, Ej)","l");
    leg->AddEntry(WH_newTF_histo, "WH (150GeV) 2.7 1/fb TF(Ep, NNoutput)","l");
  }else {
    cout <<"Error ..." << endl;
    return;
  }
  leg1->AddEntry(Wbottom_oldTF_histo, "Wbb 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(Wcharm_oldTF_histo, "Wcharm 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(schan_oldTF_histo, "s-chan 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(tchan_oldTF_histo, "t-chan 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(diboson_oldTF_histo, "diboson 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(Zjets_oldTF_histo, "Z+jets 2.2 1/fb TF(Ep, Ej)","l");
  leg1->AddEntry(ttbar_oldTF_histo, "ttbar 2.2 1/fb TF(Ep, Ej)","l");
  leg1->SetFillColor(10);
  leg1->Draw("same");

  leg->AddEntry(Wbottom_newTF_histo, "Wbb 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(Wcharm_newTF_histo, "Wcharm 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(schan_newTF_histo, "s-chan 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(tchan_newTF_histo, "t-chan 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(diboson_newTF_histo, "diboson 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(Zjets_newTF_histo, "Z+jets 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->AddEntry(ttbar_newTF_histo, "ttbar 2.7 1/fb TF(Ep, NNoutput)","l");
  leg->SetFillColor(10);
  leg->Draw("same");




  c_histo->cd(2);

  double WH_newTF_total      = -999.;
  double WH_oldTF_total      = -999.;
  double Wbottom_newTF_total = -999.;
  double Wbottom_oldTF_total = -999.;
  double Wcharm_newTF_total  = -999.;
  double Wcharm_oldTF_total  = -999.;
  double ttbar_newTF_total   = -999.;
  double ttbar_oldTF_total   = -999.;
  double schan_newTF_total   = -999.;
  double schan_oldTF_total   = -999.;
  double tchan_newTF_total   = -999.;
  double tchan_oldTF_total   = -999.;
  double diboson_newTF_total = -999.;
  double diboson_oldTF_total = -999.;
  double Zjets_newTF_total   = -999.;
  double Zjets_oldTF_total   = -999.;

  double WH_newTF_s      = -999.;
  double WH_oldTF_s      = -999.;
  double Wbottom_newTF_s = -999.;
  double Wbottom_oldTF_s = -999.;
  double Wcharm_newTF_s  = -999.;
  double Wcharm_oldTF_s  = -999.;
  double ttbar_newTF_s   = -999.;
  double ttbar_oldTF_s   = -999.;
  double schan_newTF_s   = -999.;
  double schan_oldTF_s   = -999.;
  double tchan_newTF_s   = -999.;
  double tchan_oldTF_s   = -999.;
  double diboson_newTF_s = -999.;
  double diboson_oldTF_s = -999.;
  double Zjets_newTF_s   = -999.;
  double Zjets_oldTF_s   = -999.;

  WH_newTF_total      = WH_newTF_histo->Integral(1, numberofbins);
  WH_oldTF_total      = WH_oldTF_histo->Integral(1, numberofbins);
  Wbottom_newTF_total = Wbottom_newTF_histo->Integral(1, numberofbins);
  Wbottom_oldTF_total = Wbottom_oldTF_histo->Integral(1, numberofbins);
  Wcharm_newTF_total  = Wcharm_newTF_histo->Integral(1, numberofbins);
  Wcharm_oldTF_total  = Wcharm_oldTF_histo->Integral(1, numberofbins);
  ttbar_newTF_total   = ttbar_newTF_histo->Integral(1, numberofbins);
  ttbar_oldTF_total   = ttbar_oldTF_histo->Integral(1, numberofbins);
  schan_newTF_total   = schan_newTF_histo->Integral(1, numberofbins);
  schan_oldTF_total   = schan_oldTF_histo->Integral(1, numberofbins);
  tchan_newTF_total   = tchan_newTF_histo->Integral(1, numberofbins);
  tchan_oldTF_total   = tchan_oldTF_histo->Integral(1, numberofbins);
  diboson_newTF_total = diboson_newTF_histo->Integral(1, numberofbins);
  diboson_oldTF_total = diboson_oldTF_histo->Integral(1, numberofbins); 
  Zjets_newTF_total   = Zjets_newTF_histo->Integral(1, numberofbins);
  Zjets_oldTF_total   = Zjets_oldTF_histo->Integral(1, numberofbins);



  double significance_newTF[numberofbins];
  double significance_oldTF[numberofbins];
  double newdiscr[numberofbins];
  double bkg_newTF = -999.;
  double bkg_oldTF = -999.;
  
  //inizialize
  for(int i=0; i<30; i++){
    significance_newTF[i] = -999.;
    significance_oldTF[i]= -999.;
    newdiscr[i]= -999.;
  }

  for(int i=0; i<30; i++){
    newdiscr[i] =(i/30.)+((1/30.)/2.);

    //signal
    WH_newTF_s      = WH_newTF_histo->Integral(i+1, numberofbins)/WH_newTF_total;
    WH_oldTF_s      = WH_oldTF_histo->Integral(i+1, numberofbins)/WH_oldTF_total;

    //bkg
    Wbottom_newTF_s = .47*Wbottom_newTF_histo->Integral(i+1, numberofbins)/ Wbottom_newTF_total;
    Wbottom_oldTF_s = .47*Wbottom_oldTF_histo->Integral(i+1, numberofbins)/Wbottom_oldTF_total;
    Wcharm_newTF_s  = .05*Wcharm_newTF_histo->Integral(i+1, numberofbins)/Wcharm_newTF_total;
    Wcharm_oldTF_s  = .05*Wcharm_oldTF_histo->Integral(i+1, numberofbins)/Wcharm_oldTF_total;
    ttbar_newTF_s   = .32*ttbar_newTF_histo->Integral(i+1, numberofbins)/ttbar_newTF_total;
    ttbar_oldTF_s   = .32*ttbar_oldTF_histo->Integral(i+1, numberofbins)/ttbar_oldTF_total;
    schan_newTF_s   = .09*schan_newTF_histo->Integral(i+1, numberofbins)/schan_newTF_total;
    schan_oldTF_s   = .09*schan_oldTF_histo->Integral(i+1, numberofbins)/schan_oldTF_total;
    tchan_newTF_s   = .01*tchan_newTF_histo->Integral(i+1, numberofbins)/tchan_newTF_total;
    tchan_oldTF_s   = .01*tchan_oldTF_histo->Integral(i+1, numberofbins)/tchan_oldTF_total;
    diboson_newTF_s = .05*diboson_newTF_histo->Integral(i+1, numberofbins)/diboson_newTF_total;
    diboson_oldTF_s = .05*diboson_oldTF_histo->Integral(i+1, numberofbins)/diboson_oldTF_total; 
    Zjets_newTF_s   = .01*Zjets_newTF_histo->Integral(i+1, numberofbins)/Zjets_newTF_total;
    Zjets_oldTF_s   = .01*Zjets_oldTF_histo->Integral(i+1, numberofbins)/Zjets_oldTF_total;
    
    bkg_newTF = Wbottom_newTF_s + Wcharm_newTF_s  + ttbar_newTF_s +  schan_newTF_s + tchan_newTF_s + diboson_newTF_s + Zjets_newTF_s;
    bkg_oldTF = Wbottom_oldTF_s + Wcharm_oldTF_s  + ttbar_oldTF_s +  schan_oldTF_s + tchan_oldTF_s + diboson_oldTF_s + Zjets_oldTF_s;

    if(bkg_newTF == 0) bkg_newTF = 0.00001;
    if(bkg_oldTF == 0) bkg_oldTF = 0.00001;

    significance_newTF[i] = WH_newTF_s/sqrt(bkg_newTF);
    significance_oldTF[i] = WH_oldTF_s/sqrt(bkg_oldTF);
    //    cout << "significance_newTF " << significance_newTF[i] << ", significance_oldTF  "<< significance_oldTF[i] <<  endl;     
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

  TGraph *signalandbkg_newTF = new TGraph(numberofbins, newdiscr, significance_newTF);
  TGraph *signalandbkg_oldTF = new TGraph(numberofbins, newdiscr, significance_oldTF);

  gPad->SetGrid(2);
  signalandbkg_newTF->SetLineColor(4);
  signalandbkg_newTF->SetLineStyle(2);
  signalandbkg_newTF->SetLineWidth(2);
  signalandbkg_oldTF->SetLineWidth(2);
  signalandbkg_oldTF->SetMarkerStyle(3);
  signalandbkg_newTF->SetMarkerStyle(23);
  signalandbkg_newTF->SetMarkerColor(4);
 
  TMultiGraph *mg1 = new  TMultiGraph();
  mg1->Add(signalandbkg_newTF); 
  mg1->Add(signalandbkg_oldTF); 
  mg1->Draw("PC");

  TLegend *legend = new TLegend(0.26, 0.28, 0.63, 0.52);
  legend->AddEntry(signalandbkg_newTF, "2.7 1/fb, TF(Ep, NNoutput)", "l");
  legend->AddEntry(signalandbkg_oldTF, "2.7 1/fb, TF(Ep, Ej)", "l");
  legend->SetFillColor(10);
  legend->Draw("same");

  c_histo->Print("blessing_plots/Discr_newselection_"+higgsmass+".eps");
  c_histo->Print("blessing_plots/Discr_newselection_"+higgsmass+".gif");


}
