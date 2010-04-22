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
//void Discriminant_WH(TString ntags = "one", TString ttbar_choice = "withtt", TString higgsmass = "115GeV")
void Tunning_discriminant(TString ntags = "one", TString higgsmass = "115GeV")
{
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  /////////////////////////////////////
  //                                 //
  //  EVENT PROBABILITY ROOT FILES   // 
  //                                 //
  /////////////////////////////////////
 
  //trying to reproduce Peter Discriminant!
  
  TChain* WH_signal = new TChain("METree");
  char filesignal[500];
  unsigned int index;
  if(higgsmass == "100GeV"){
    index = 0;
    cout << "*** Higgs 100 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH100GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "105GeV"){
    index = 1;
    cout << "*** Higgs 105 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH105GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "110GeV"){
    index = 2;
    cout << "*** Higgs 110 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH110GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "115GeV"){
    index = 3;
    cout << "*** Higgs 115 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH115GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "120GeV"){
    index = 4;
    cout << "*** Higgs 120 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH120GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "125GeV"){
    index = 5;
    cout << "*** Higgs 125 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH125GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "130GeV"){
    index = 6;
    cout << "*** Higgs 130 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH130GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "135GeV"){
    index = 7;
    cout << "*** Higgs 135 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH135GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "140GeV"){
    index = 8;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH140GeV.root"); 
    cout << "*** Higgs 140 GeV ***" << endl;
    WH_signal->Add(filesignal);
  }else if(higgsmass == "145GeV"){
    index = 9;
    cout << "*** Higgs 145 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH145GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "150GeV"){
    index = 10;
    cout << "*** Higgs 150 GeV ***" << endl;
    sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH150GeV.root"); 
    WH_signal->Add(filesignal);
  }else{

    cout << "The higgs mass is out of range " << endl;
    return;
  }
  cout<<filesignal<<endl;
  
  TChain* bkg_tchannel = new TChain("METree"); 
  char filebkg_tchannel[500];
  sprintf(filebkg_tchannel,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tchan.root"); 
  bkg_tchannel->Add(filebkg_tchannel);
  cout<<filebkg_tchannel<<endl;


  //Wbb  new TF(Ep,NNoutput)
  TChain* bkg_Wbottom = new TChain("METree"); 
  char filebkg_Wbottom[600];
  sprintf(filebkg_Wbottom,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_bottom.root");
  bkg_Wbottom->Add(filebkg_Wbottom);
  cout<<filebkg_Wbottom<<endl;
  
  
  //schannel new TF(Ep,NNoutput)
  TChain* bkg_schannel = new TChain("METree");
  char filebkg_schannel[500];
  sprintf(filebkg_schannel,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_schan.root");    
  bkg_schannel->Add(filebkg_schannel);
  cout<<filebkg_schannel<<endl;

  //ttbar
  TChain* bkg_ttlj = new TChain("METree");
  char filebkg_ttlj[500];
  sprintf(filebkg_ttlj,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tt_nondilepton_merge.root");    
  bkg_ttlj->Add(filebkg_ttlj);
  cout<<filebkg_ttlj<<endl;
  //ttbar
  TChain* bkg_ttdil = new TChain("METree");
  char filebkg_ttdil[500];
  sprintf(filebkg_ttdil,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tt_dilepton_merge.root");    
  bkg_ttdil->Add(filebkg_ttdil);
  cout<<filebkg_ttdil<<endl;

  //diboson
  TChain* bkg_diboson = new TChain("METree");
  char filebkg_diboson[500];
  sprintf(filebkg_diboson,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_diboson.root");    
  bkg_diboson->Add(filebkg_diboson);
  cout<<filebkg_diboson<<endl;

  //Z+jets
  TChain* bkg_Zjets = new TChain("METree");
  char filebkg_Zjets[500];
  sprintf(filebkg_Zjets,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_Zjets.root");    
  bkg_Zjets->Add(filebkg_Zjets);
  cout<<filebkg_Zjets<<endl;

  //nonW
  TChain* bkg_nonW = new TChain("METree");
  char filebkg_nonW[500];
  sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_nonW_noniso.root");    
  bkg_nonW->Add(filebkg_nonW);
  cout<<filebkg_nonW<<endl;
  sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_nonW_jetele.root");    
  bkg_nonW->Add(filebkg_nonW);
  cout<<filebkg_nonW<<endl;
  sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_nonW_antiele.root");    
  bkg_nonW->Add(filebkg_nonW);
  cout<<filebkg_nonW<<endl;


  //**************W+Charm****************************

  TChain* bkg_Wcharm = new TChain("METree");
  char filebkg_Wcharm[500];
  sprintf(filebkg_Wcharm,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_charm.root");    
  bkg_Wcharm->Add(filebkg_Wcharm);
  cout<<filebkg_Wcharm<<endl;

  //************W+Light*********************** 
  TChain* bkg_Wlight = new TChain("METree");
  char filebkg_Wlight[500];
  sprintf(filebkg_Wlight,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_light.root");    
  bkg_Wlight->Add(filebkg_Wlight);
  cout<<filebkg_Wlight<<endl;

		         
  cout<<"Total Entries WH signal ("    << higgsmass << " GeV) = " <<WH_signal->GetEntries() <<endl;
  cout<<"Total Entries Wbottom bkg   " <<bkg_Wbottom->GetEntries()         <<endl;
  cout<<"Total Entries ttdil bkg     " <<bkg_ttdil->GetEntries()              <<endl;
  cout<<"Total Entries ttlj bkg      " <<bkg_ttlj->GetEntries()              <<endl;
  cout<<"Total Entries schannel bkg  " <<bkg_schannel->GetEntries()        <<endl;
  cout<<"Total Entries tchannel bkg  " <<bkg_tchannel->GetEntries()        <<endl;
  cout<<"Total Entries diboson bkg   " <<bkg_diboson->GetEntries()         <<endl;
  cout<<"Total Entries Z+jets bkg    " <<bkg_Zjets->GetEntries()           <<endl;
  cout<<"Total Entries Wcharm bkg    " <<bkg_Wcharm->GetEntries()          <<endl;
  cout<<"Total Entries W+light bkg   " <<bkg_Wlight->GetEntries()          <<endl;
  cout<<"Total Entries non-W bkg     " <<bkg_nonW->GetEntries()          <<endl;

  TCanvas *c_histo = new TCanvas("c_histo", "Discriminant", 1000, 800);
  c_histo->cd(1);
  const unsigned numberofbins = 30;      
  TH1F* WH_signal_histo    = new TH1F("WH_signal_histo", "WH_signal_histo", numberofbins, 0, 1);
  WH_signal_histo->SetNormFactor(1);
  WH_signal_histo->SetLineWidth(2);
  WH_signal_histo->SetLineColor(100);
  WH_signal_histo->SetTitle("");
  WH_signal_histo->GetXaxis()->SetTitleSize(0.045);
  WH_signal_histo->GetYaxis()->SetTitleSize(0.045);
  WH_signal_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  WH_signal_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_Wbottom_histo  = new TH1F("bkg_Wbottom_histo", "bkg_Wbottom_histo", numberofbins, 0, 1);
  bkg_Wbottom_histo->SetNormFactor(1);
  bkg_Wbottom_histo->SetLineWidth(2);
  bkg_Wbottom_histo->SetLineColor(209);
  bkg_Wbottom_histo->SetTitle("");
  bkg_Wbottom_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_Wbottom_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_Wbottom_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Wbottom_histo->GetYaxis()->SetTitle("Normalized to unit area");



  TH1F* bkg_ttlj_histo       = new TH1F("bkg_ttlj_histo", "bkg_ttlj_histo", numberofbins, 0, 1);
  bkg_ttlj_histo->SetNormFactor(1);
  bkg_ttlj_histo->SetLineWidth(2);
  bkg_ttlj_histo->SetLineColor(12);
  bkg_ttlj_histo->SetTitle("");
  bkg_ttlj_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_ttlj_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_ttlj_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_ttlj_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_ttdil_histo       = new TH1F("bkg_ttdil_histo", "bkg_ttdil_histo", numberofbins, 0, 1);
  bkg_ttdil_histo->SetNormFactor(1);
  bkg_ttdil_histo->SetLineWidth(2);
  bkg_ttdil_histo->SetLineColor(12);
  bkg_ttdil_histo->SetTitle("");
  bkg_ttdil_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_ttdil_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_ttdil_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_ttdil_histo->GetYaxis()->SetTitle("Normalized to unit area");


  TH1F* bkg_schannel_histo = new TH1F("bkg_schannel_histo", "bkg_schannel_histo", numberofbins, 0, 1);
  bkg_schannel_histo->SetNormFactor(1);
  bkg_schannel_histo->SetLineWidth(2);
  bkg_schannel_histo->SetLineColor(102);
  bkg_schannel_histo->SetTitle("");
  bkg_schannel_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_schannel_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_schannel_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_schannel_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_tchannel_histo = new TH1F("bkg_tchannel_histo", "bkg_tchannel_histo", numberofbins, 0, 1);
  bkg_tchannel_histo->SetNormFactor(1);
  bkg_tchannel_histo->SetLineWidth(2);
  bkg_tchannel_histo->SetLineColor(96);
  bkg_tchannel_histo->SetTitle("");
  bkg_tchannel_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_tchannel_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_tchannel_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_tchannel_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_diboson_histo  = new TH1F("bkg_diboson_histo", "bkg_diboson_histo", numberofbins, 0, 1);
  bkg_diboson_histo->SetNormFactor(1);
  bkg_diboson_histo->SetLineWidth(2);
  bkg_diboson_histo->SetLineColor(41);
  bkg_diboson_histo->SetTitle("");
  bkg_diboson_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_diboson_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_diboson_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_diboson_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_Zjets_histo    = new TH1F("bkg_Zjets_histo", "bkg_Zjets_histo", numberofbins, 0, 1);
  bkg_Zjets_histo->SetNormFactor(1); 
  bkg_Zjets_histo->SetLineWidth(2);
  bkg_Zjets_histo->SetLineColor(60);
  bkg_Zjets_histo->SetTitle("");
  bkg_Zjets_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_Zjets_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_Zjets_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Zjets_histo->GetYaxis()->SetTitle("Normalized to unit area");



  TH1F* bkg_Wcharm_histo     = new TH1F("bkg_Wcharm_histo", "bkg_Wcharm_histo", numberofbins, 0, 1);
  bkg_Wcharm_histo->SetNormFactor(1); 
  bkg_Wcharm_histo->SetLineWidth(2);
  bkg_Wcharm_histo->SetLineColor(8);
  bkg_Wcharm_histo->SetTitle("");
  bkg_Wcharm_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_Wcharm_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_Wcharm_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Wcharm_histo->GetYaxis()->SetTitle("Normalized to unit area");

  TH1F* bkg_Wlight_histo      = new TH1F("bkg_Wlight_histo", "bkg_Wlight_histo", numberofbins, 0, 1);
  bkg_Wlight_histo->SetNormFactor(1); 
  bkg_Wlight_histo->SetLineWidth(2);
  bkg_Wlight_histo->SetLineColor(212);
  bkg_Wlight_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Wlight_histo->GetYaxis()->SetTitle("Normalized to unit area");
  bkg_Wlight_histo->SetTitle("");

  TH1F* bkg_nonW_histo      = new TH1F("bkg_nonW_histo", "bkg_nonW_histo", numberofbins, 0, 1);
  bkg_nonW_histo->SetNormFactor(1); 
  bkg_nonW_histo->SetLineWidth(2);
  bkg_nonW_histo->SetLineColor(38);
  bkg_nonW_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_nonW_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_nonW_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_nonW_histo->GetYaxis()->SetTitle("Normalized to unit area");
  bkg_nonW_histo->SetTitle("");



  
  if( ntags == "one" ){
    cout << "Value of INDEX " << index <<  endl; 

    std::ostringstream str1tag_nonW;
    str1tag_nonW<< "(epd1tagWH[" << index << "])>>bkg_nonW_histo";
    bkg_nonW->Draw(str1tag_nonW.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))"); 
    
    std::ostringstream str1tag_Wlight;
    str1tag_Wlight << "(epd1tagWH[" << index << "])>>bkg_Wlight_histo";
    bkg_Wlight->Draw(str1tag_Wlight.str().c_str(), "(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");

    std::ostringstream str1tag_tchan;
    str1tag_tchan << "(epd1tagWH[" << index << "])>>bkg_tchannel_histo";
    bkg_tchannel->Draw(str1tag_tchan.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    
    std::ostringstream str1tag_WH;
    str1tag_WH << "(epd1tagWH[" << index << "])>>WH_signal_histo";
    WH_signal->Draw(str1tag_WH.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_ttlj;
    str1tag_ttlj << "(epd1tagWH[" << index << "])>>bkg_ttlj_histo";
    bkg_ttlj->Draw(str1tag_ttlj.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_ttdil;
    str1tag_ttdil << "(epd1tagWH[" << index << "])>>bkg_ttdil_histo";
     bkg_ttdil->Draw(str1tag_ttdil.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_schan;
    str1tag_schan << "(epd1tagWH[" << index << "])>>bkg_schannel_histo";
    bkg_schannel->Draw(str1tag_schan.str().c_str() ,"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
 

    std::ostringstream str1tag_Zjets;
    str1tag_Zjets<< "(epd1tagWH[" << index << "])>>bkg_Zjets_histo";
    bkg_Zjets->Draw(str1tag_Zjets.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_diboson;
    str1tag_diboson<< "(epd1tagWH[" << index << "])>>bkg_diboson_histo";
    bkg_diboson->Draw(str1tag_diboson.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_Wbottom;
    str1tag_Wbottom<< "(epd1tagWH[" << index << "])>>bkg_Wbottom_histo";
    bkg_Wbottom->Draw(str1tag_Wbottom.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str1tag_Wcharm;
    str1tag_Wcharm<< "(epd1tagWH[" << index << "])>>bkg_Wcharm_histo";
    bkg_Wcharm->Draw(str1tag_Wcharm.str().c_str(),"(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    
    


  }else if( ntags == "two" ){
    
    std::ostringstream str2tag_nonW;
    str2tag_nonW<< "(epd2tagWH[" << index << "])>>bkg_nonW_histo";
    bkg_nonW->Draw(str2tag_nonW.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))"); 

    std::ostringstream str2tag_Wlight;
    str2tag_Wlight << "(epd2tagWH[" << index << "])>>bkg_Wlight_histo";
    bkg_Wlight->Draw(str2tag_Wlight.str().c_str(), "(h.tagProb1*h.tagProb2*h.wgt*h.triggerTO)", "same");

    std::ostringstream str2tag_ttlj;
    str2tag_ttlj << "(epd2tagWH[" << index << "])>>bkg_ttlj_histo";
    bkg_ttlj->Draw(str2tag_ttlj.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str2tag_ttdil;
    str2tag_ttdil << "(epd2tagWH[" << index << "])>>bkg_ttdil_histo";
    bkg_ttdil->Draw(str2tag_ttdil.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    
    std::ostringstream str2tag_WH;
    str2tag_WH << "(epd2tagWH[" << index << "])>>WH_signal_histo";
    WH_signal->Draw(str2tag_WH.str().c_str(),"((h.tagProb1*h.wgt*h.tagProb2*h.triggerTO))", "same"); 

    std::ostringstream str2tag_schan;
    str2tag_schan << "(epd2tagWH[" << index << "])>>bkg_schannel_histo";
    bkg_schannel->Draw(str2tag_schan.str().c_str() ,"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 
 
    std::ostringstream str2tag_tchan;
    str2tag_tchan << "(epd2tagWH[" << index << "])>>bkg_tchannel_histo";
    bkg_tchannel->Draw(str2tag_tchan.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str2tag_Zjets;
    str2tag_Zjets<< "(epd2tagWH[" << index << "])>>bkg_Zjets_histo";
    bkg_Zjets->Draw(str2tag_Zjets.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str2tag_diboson;
    str2tag_diboson<< "(epd2tagWH[" << index << "])>>bkg_diboson_histo";
    bkg_diboson->Draw(str2tag_diboson.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str2tag_Wbottom;
    str2tag_Wbottom<< "(epd2tagWH[" << index << "])>>bkg_Wbottom_histo";
    bkg_Wbottom->Draw(str2tag_Wbottom.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    std::ostringstream str2tag_Wcharm;
    str2tag_Wcharm<< "(epd2tagWH[" << index << "])>>bkg_Wcharm_histo";
    bkg_Wcharm->Draw(str2tag_Wcharm.str().c_str(),"((h.tagProb1*h.tagProb2*h.wgt*h.triggerTO))", "same"); 
   
   
  }else{
    cout << "1 tag or 2 tags no more than that! " << endl;
    return;
  }

  TLegend *legend = new TLegend (0.32, 0.46, 0.7, 0.86);
  legend->SetFillColor(10); 
  if(higgsmass == "100GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (100 GeV)","l");
  }else  if(higgsmass == "105GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (105 GeV)","l");
  }else  if(higgsmass == "110GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (110 GeV)","l");
  }else  if(higgsmass == "115GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (115 GeV)","l");
  }else  if(higgsmass == "120GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (120 GeV)","l");
  }else  if(higgsmass == "125GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (125 GeV)","l");
  }else  if(higgsmass == "130GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (130 GeV)","l");
  }else  if(higgsmass == "135GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (135 GeV)","l");
  }else  if(higgsmass == "140GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (140 GeV)","l");
  }else  if(higgsmass == "145GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (145 GeV)","l");
  }else  if(higgsmass == "150GeV"){
    legend->AddEntry(WH_signal_histo,      "WH (150 GeV)","l");
  }else{
    cout << "Wrong Higgs Mass point " << endl;
    return;
  }

  legend->AddEntry(bkg_ttdil_histo,      "t#bar{t} l+j","l");
  legend->AddEntry(bkg_ttlj_histo,       "t#bar{t} dil","l");
  legend->AddEntry(bkg_schannel_histo,   "s-chan","l");
  legend->AddEntry(bkg_tchannel_histo,   "t-chan","l");
  legend->AddEntry(bkg_Zjets_histo,      "Z+jets","l");
  legend->AddEntry(bkg_diboson_histo,    "Diboson","l");
  legend->AddEntry(bkg_Wbottom_histo,    "W+bottom","l");
  legend->AddEntry(bkg_Wcharm_histo,     "W+charm","l");
  legend->AddEntry(bkg_Wlight_histo,     "W+light","l");
  legend->AddEntry(bkg_nonW_histo,       "non-W","l");
  legend->Draw("Same");


  
  
  ///////////////////////////
  //                       //
  //      DISCRIMINANT     // 
  //                       //
  ///////////////////////////

  TCanvas *c = new TCanvas("Discriminant", "Discriminant", 1000, 800);
  c->cd(1);


  TLegend *leg = new TLegend (0.32, 0.41, 0.58, 0.86);
  //signal newTF
  leg->SetFillColor(10);

  //****************With ttbar ME ***********************************88

  TH1F* newbkg_tchannel_histo= new TH1F("newbkg_tchannel_histo","newTF",numberofbins,0,1);
  newbkg_tchannel_histo->SetNormFactor(1);
  newbkg_tchannel_histo->SetLineColor(96);
  newbkg_tchannel_histo->SetLineWidth(2);
  newbkg_tchannel_histo->SetTitle("EPD ");
  newbkg_tchannel_histo->GetXaxis()->SetTitleSize(0.05);
  newbkg_tchannel_histo->GetYaxis()->SetTitleSize(0.05);
  newbkg_tchannel_histo->GetXaxis()->SetTitle("EPD");
  newbkg_tchannel_histo->GetYaxis()->SetTitle("# of events");
  newbkg_tchannel_histo->SetTitle("Discriminant ");
 
  TH1F* newbkg_histo = new TH1F("newbkg_histo","newbkg_histo",numberofbins,0,1);
  newbkg_histo->SetNormFactor(1);
  newbkg_histo->SetLineWidth(2);
  newbkg_histo->SetLineColor(209);
  newbkg_histo->SetTitle("Discriminant");

  TH1F* newbkg_tt_histo = new TH1F("newbkg_tt_histo","newbkg_tt_histo",numberofbins,0,1);
  newbkg_tt_histo->SetNormFactor(1);
  newbkg_tt_histo->SetLineColor(12);
  newbkg_tt_histo->SetLineWidth(2);
  newbkg_tt_histo->GetXaxis()->SetTitleSize(0.05);
  newbkg_tt_histo->GetYaxis()->SetTitleSize(0.05);
  newbkg_tt_histo->GetXaxis()->SetTitle("EPD");
  newbkg_tt_histo->SetTitle("Discriminant ");
    
  TH1F* newbkg_ttlj_histo = new TH1F("newbkg_ttlj_histo","newbkg_ttlj_histo",numberofbins,0,1);
  newbkg_ttlj_histo->SetNormFactor(1);
  newbkg_ttlj_histo->SetLineWidth(2);
  newbkg_ttlj_histo->SetLineColor(12);
  newbkg_ttlj_histo->GetXaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo->GetYaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo->GetXaxis()->SetTitle("EPD");
  newbkg_ttlj_histo->SetTitle("Discriminant");
    
  TH1F* newbkg_schannel_histo = new TH1F("newbkg_schannel_histo","newbkg_schannel_histo",numberofbins,0,1);
  newbkg_schannel_histo->SetNormFactor(1);
  newbkg_schannel_histo->SetLineColor(102);
  newbkg_schannel_histo->SetLineWidth(2);

  TH1F* newbkg_nonW_histo = new TH1F("newbkg_nonW_histo","newbkg_nonW_histo",numberofbins,0,1);
  newbkg_nonW_histo->SetNormFactor(1);
  newbkg_nonW_histo->SetLineColor(38);
  newbkg_nonW_histo->SetLineWidth(2);

  TH1F* newbkg_Zjets_histo = new TH1F("newbkg_Zjets_histo","newbkg_Zjets_histo",numberofbins,0,1);
  newbkg_Zjets_histo->SetNormFactor(1);
  newbkg_Zjets_histo->SetLineColor(60);
  newbkg_Zjets_histo->SetLineWidth(2);

  TH1F* newbkg_Wlight_histo = new TH1F("newbkg_Wlight_histo","newbkg_Wlight_histo",numberofbins,0,1);
  newbkg_Wlight_histo->SetNormFactor(1);
  newbkg_Wlight_histo->SetLineColor(212);
  newbkg_Wlight_histo->SetLineWidth(2);

  TH1F* newbkg_Wcharm_histo = new TH1F("newbkg_Wcharm_histo","newbkg_Wcharm_histo",numberofbins,0,1);
  newbkg_Wcharm_histo->SetNormFactor(1);
  newbkg_Wcharm_histo->SetLineColor(212);
  newbkg_Wcharm_histo->SetLineWidth(2);

  TH1F* newbkg_diboson_histo = new TH1F("newbkg_diboson_histo","newbkg_diboson_histo",numberofbins,0,1);
  newbkg_diboson_histo->SetNormFactor(1);
  newbkg_diboson_histo->SetLineColor(41);
  newbkg_diboson_histo->SetLineWidth(2);



  TH1F* newsignal_histo = new TH1F("newsignal_histo","newsignal_histo",numberofbins,0,1);
  newsignal_histo->SetNormFactor(1);
  newsignal_histo->SetLineColor(100);
  newsignal_histo->SetLineWidth(2);




  
  //Draw
  if( ntags == "one" ){

    // const double par115[2][12] = 
    //{{1, 0.5, 0.1, 3.63, 0.121, 0.45, 0, 0.005, 0.0001, 0, 0, 5000},
    // {0.1, 0.00165289, 0.004, 3, 0.01, 0.001, 0, 0, 1e-05, 0, 0, 1136.37}};

    bkg_nonW->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))      ))>>newbkg_nonW_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))");

    
    bkg_ttdil->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))   ))>>newbkg_tt_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    
    
    bkg_ttlj->Draw( "((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))    ))>>newbkg_ttlj_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  
    bkg_Wbottom->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))    ))>>newbkg_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
   
    bkg_schannel->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))    ))>>newbkg_schannel_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");


    bkg_Zjets->Draw( "((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))      ))>>newbkg_Zjets_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");

    bkg_Wlight->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))     ))>>newbkg_Wlight_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");
    
    bkg_Wcharm->Draw( "((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))    ))>>newbkg_Wcharm_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");
    
    bkg_diboson->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))     ))>>newbkg_diboson_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same");
    
    bkg_tchannel->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))      ))>>newbkg_tchannel_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    
    //WH

    WH_signal->Draw("((bProb[0]*mnt.eventProb[3]*6000.)/((bProb[0]*((mnt.eventProb[3]*6000.)+(mnt.eventProb[11]*1.)+(mnt.eventProb[12]*0.5)+(mnt.eventProb[13]*0.1)+(mnt.eventProb[14]*3.63) + (mnt.eventProb[18]*0.0001)))   + ((1-bProb[0])* ( (mnt.eventProb[14]*0.121) + (mnt.eventProb[15]*0.45) + (mnt.eventProb[16]*0.) + (mnt.eventProb[17]*0.005)  +  (mnt.eventProb[19]*0.0) + (mnt.eventProb[20]*0.0) ))     ))>>newsignal_histo","(h.passQCD*(h.tagProb1*h.wgt*h.triggerTO))", "same"); 



    
 }
  /*else if(ntags == "two"){

   //coefficients WH115 GeV-> {0.1, 0.00165289, 0.004, 3, 0.01, 0.001, 0, 0, 1e-05, 0, 0, 1136.37}
    newbkg_tt->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newbkg_tt_histo","((h.tagProb2*h.wgt*h.triggerTO))"); 
    
   
    newbkg_ttlj->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newbkg_ttlj_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  
    newbkg->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newbkg_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
   
    newbkg_schannel->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newbkg_schannel_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same");

    
    newbkg_tchannel->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newbkg_tchannel_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  
    WH_signal->Draw("((bProb[0]* bProb[1]*(mnt.eventProb[14]*1136.37))/((bProb[0]* bProb[1]*((mnt.eventProb[14]*1136.37)+(mnt.eventProb[0]*0.1)+(mnt.eventProb[3]*3.) + (mnt.eventProb[7]*0.00001))) +  (bProb[0]* (1-bProb[1])*  ((mnt.eventProb[1]*0.00165289)+(mnt.eventProb[2]*0.004))) + ((1-bProb[0])* (1-bProb[1])* ((mnt.eventProb[3]*0.01) + (mnt.eventProb[4]*0.001) + (mnt.eventProb[5]*0) + (mnt.eventProb[6]*0.)))      ))>>newsignal_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 

    }*/
  else{
    cout << "how many tags did you say ? " << endl;
    return;
  }

  leg->AddEntry(newsignal_histo,      "WH(100GeV)","l");
  leg->AddEntry(newbkg_histo,         "Wb#bar{b}","l");
  leg->AddEntry(newbkg_tt_histo,      "t#bar{t} (dilep)","l" );
  leg->AddEntry(newbkg_ttlj_histo,    "t#bar{t} (l+j)","l" );
  leg->AddEntry(newbkg_schannel_histo,"s-channel","l" );
  leg->AddEntry(newbkg_tchannel_histo,"t-channel","l");  
  leg->AddEntry(newbkg_diboson_histo, "Diboson","l");  
  leg->AddEntry(newbkg_Wcharm_histo,  "Wcharm","l");  
  leg->AddEntry(newbkg_Wlight_histo,  "Wlight","l");  
  leg->AddEntry(newbkg_nonW_histo,    "non-W","l");  
  leg->AddEntry(newbkg_Zjets_histo,   "Zjets","l");  

   leg->Draw("Same");



   
  //Fitter
  TH1* data = newbkg_schannel_histo ;
  
  vector<Shape> shapes;
  //Signal

  if( ntags == "one" ){
    
  //100 GeV
      shapes.push_back(Shape("WH",newsignal_histo , Value(6.3, 0.5), false));
      shapes.push_back(Shape("STopS",newbkg_schannel_histo , Value(40.6, 5.7), true));
      shapes.push_back(Shape("STopT",newbkg_tchannel_histo , Value(63.9, 9.3), true));
      shapes.push_back(Shape("Wbb",newbkg_histo , Value(542.7, 163.8), true));
      shapes.push_back(Shape("TopLJ",newbkg_ttlj_histo , Value(121.3, 17.1), true));
      shapes.push_back(Shape("TopDil",newbkg_tt_histo , Value(52.8, 7.4), true));
      shapes.push_back(Shape("Z+jets",newbkg_Zjets_histo , Value(37.2, 5.5), true));
      shapes.push_back(Shape("Wcc_Wc",newbkg_Wcharm_histo , Value(492.6, 152.1), true));
      shapes.push_back(Shape("Non-W",newbkg_nonW_histo , Value(155.6, 62.2), true));
      shapes.push_back(Shape("Mistags",newbkg_Wlight_histo , Value(460.3, 58.5), true));
 

    

  }else if( ntags == "two" ){
    
      shapes.push_back(Shape("WH",newsignal_histo , Value(1.4, 0.2), false));
      shapes.push_back(Shape("s-channel",newbkg_schannel_histo , Value(9.4, 1.5), true));
      shapes.push_back(Shape("t-channel",newbkg_tchannel_histo , Value(1.7, 0.3), true));    
      shapes.push_back(Shape("Wbb",newbkg_histo , Value(54.4, 16.9), true));
      shapes.push_back(Shape("ttlj",newbkg_ttlj_histo , Value(16.2, 2.7), true));
      shapes.push_back(Shape("ttdil",newbkg_tt_histo , Value(10.7, 1.7), true));
      
    
    
  }else{
    cout << "How many tags did you say?? " << endl;
  }
  
  
  Fitter fit(data, shapes);
  
  fit.allowNegative(true);
  fit.setMinos(true);
  fit.setRandomSeed(5);

  fit.doPseudoEx(3000); 
   
}
