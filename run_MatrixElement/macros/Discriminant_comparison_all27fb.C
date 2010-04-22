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
void Discriminant_comparison_all27fb( TString higgsmass = "WH115", TString result = "oldTF")

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
  
  TChain* WH_signal = new TChain("METree");
  TChain* WH_signal_Peter = new TChain("METree");
  char filesignal[500];
  char filesignal_Peter[500];
  unsigned int index;
  unsigned int index_Peter;

  if(higgsmass == "WH115"){
    
    index_Peter = 3;
    cout << "*** Higgs 115 GeV ***" << endl;
    if( result == "newTF"){
      index = 3;
      cout << "New TF " << endl;
      sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH115GeV.root"); 
      
    }else if( result == "oldTF"){
      index = 1;
      cout <<"old TF " << endl;
      //2.2 fb-1
      sprintf(filesignal,"/cdf/disk02/g6/casal/WH/WH115.root"); 
    }else{
      cout <<"Error WH115GeV !!!" << endl;
      return;
    }
    //2.7fb standard TF
    sprintf(filesignal_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH115.root"); 

  }else if(higgsmass == "WH120"){
    cout << "*** Higgs 120 GeV ***" << endl; 
    
    index_Peter = 4;
    
    if( result == "newTF"){
      index = 4;
      cout << "New vs old TF study " << endl;
      sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH120GeV.root"); 
    }else if( result == "oldTF"){
      //2.2 fb-1 
      index = 2;
      cout <<"old TF " << endl;
      sprintf(filesignal,"/cdf/disk02/g6/casal/WH/WH120.root"); 
    }else{
      cout <<"Error WH120GeV !!!" << endl; 
      return;
    }
    sprintf(filesignal_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH120.root"); 

  }else if(higgsmass == "WH150"){
    cout << "*** Higgs 150 GeV ***" << endl; 
    index_Peter = 10;
    sprintf(filesignal_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_WH150.root"); 

    if( result == "newTF"){
      index = 10;
      cout << "New vs old TF study " << endl;
      sprintf(filesignal,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH150GeV.root"); 
      
    }else if( result == "oldTF"){
      //2.2 fb-1 
      index = 5;
      cout <<"old TF " << endl;
      sprintf(filesignal,"/cdf/disk02/g6/casal/WH/WH150.root"); 
    }else{
      cout <<"Error WH150GeV !!!" << endl; 
      return;
    }
  }else{
    cout << "Choose another Higss mass " << endl;
    return;
  }
  
  WH_signal->Add(filesignal);
  cout<<filesignal<<endl;
  
  WH_signal_Peter->Add(filesignal_Peter);
  cout<<filesignal_Peter<<endl;
  
  TChain* bkg_tchannel = new TChain("METree"); 
  char filebkg_tchannel[500];

  if( result == "newTF"){
    sprintf(filebkg_tchannel,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tchan.root"); 
  }else if( result == "oldTF"){
    //2.2fb -1
    sprintf(filebkg_tchannel,"/cdf/disk02/g6/casal/WH/tchan.root"); 
  }else{
    cout <<"Error tchannel !!!" << endl; 
    return;
  }
  bkg_tchannel->Add(filebkg_tchannel);
  cout<<filebkg_tchannel<<endl;

  TChain* bkg_tchannel_Peter = new TChain("METree"); 
  char filebkg_tchannel_Peter[500];
  sprintf(filebkg_tchannel_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tchan.root"); 
  bkg_tchannel_Peter->Add(filebkg_tchannel_Peter);
  cout<<filebkg_tchannel_Peter<<endl;


  //Wbb  new TF(Ep,NNoutput)
  TChain* bkg_Wbottom = new TChain("METree"); 
  char filebkg_Wbottom[600];
  if( result == "newTF"){
    sprintf(filebkg_Wbottom,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_bottom.root");
  }else if( result == "oldTF"){
    //2.2 fb-1 
    sprintf(filebkg_Wbottom,"/cdf/disk02/g6/casal/WH/Wbottom.root");
  }else{
      cout <<"Error bottom !!!" << endl; return;
  }
  bkg_Wbottom->Add(filebkg_Wbottom);
  cout<<filebkg_Wbottom<<endl;
  
  //Wbb  
  TChain* bkg_Wbottom_Peter = new TChain("METree"); 
  char filebkg_Wbottom_Peter[600];
  sprintf(filebkg_Wbottom_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*bottom.root");
  bkg_Wbottom_Peter->Add(filebkg_Wbottom_Peter);
  cout<<filebkg_Wbottom_Peter<<endl;
  
  
  //schannel new TF(Ep,NNoutput)
  TChain* bkg_schannel = new TChain("METree");
  char filebkg_schannel[500];
  if( result == "newTF"){
    sprintf(filebkg_schannel,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_schan.root");    
  }else if( result == "oldTF"){
    //2.2 fb-1
    sprintf(filebkg_schannel,"/cdf/disk02/g6/casal/WH/schan.root");    
  }else{
      cout <<"Error schan !!!" << endl; return;
  }
  bkg_schannel->Add(filebkg_schannel);
  cout<<filebkg_schannel<<endl;

  //schannel new TF(Ep,NNoutput)
  TChain* bkg_schannel_Peter = new TChain("METree");
  char filebkg_schannel_Peter[500];
  sprintf(filebkg_schannel_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_schan.root");    
  bkg_schannel_Peter->Add(filebkg_schannel_Peter);
  cout<<filebkg_schannel_Peter<<endl;

  //ttbar
  TChain* bkg_tt = new TChain("METree");
  char filebkg_tt[500];
  if( result == "newTF"){
    sprintf(filebkg_tt,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_tt_*.root");    
  }else if( result == "oldTF"){
    //2.2fb-1 
    sprintf(filebkg_tt,"/cdf/disk02/g6/casal/WH/tt*.root");    
  }else{
      cout <<"Error tt !!!" << endl; return;
  }
  bkg_tt->Add(filebkg_tt);
  cout<<filebkg_tt<<endl;

  //ttbar
  TChain* bkg_tt_Peter = new TChain("METree");
  char filebkg_tt_Peter[500];
  sprintf(filebkg_tt_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_tt_*.root");    
  bkg_tt_Peter->Add(filebkg_tt_Peter);
  cout<<filebkg_tt_Peter<<endl;

  //diboson
  TChain* bkg_diboson = new TChain("METree");
  char filebkg_diboson[500];
  if( result == "newTF"){
    sprintf(filebkg_diboson,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*WW*.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
    sprintf(filebkg_diboson,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*WZ*.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
    sprintf(filebkg_diboson,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*ZZ*.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
  }else if( result == "oldTF"){
    sprintf(filebkg_diboson,"/cdf/disk02/g6/casal/WH/WW.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
    sprintf(filebkg_diboson,"/cdf/disk02/g6/casal/WH/WZ.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
    sprintf(filebkg_diboson,"/cdf/disk02/g6/casal/WH/ZZ.root");    
    bkg_diboson->Add(filebkg_diboson);
    cout<<filebkg_diboson<<endl;
  }else{
    cout <<"Error tt !!!" << endl; 
    return;
  }
  //diboson
  TChain* bkg_diboson_Peter = new TChain("METree");
  char filebkg_diboson_Peter[500];
  sprintf(filebkg_diboson_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_diboson.root");    
  bkg_diboson_Peter->Add(filebkg_diboson_Peter);
  cout<<filebkg_diboson_Peter<<endl;

  //Z+jets
  TChain* bkg_Zjets = new TChain("METree");
  char filebkg_Zjets[500];
  if( result == "newTF"){
    sprintf(filebkg_Zjets,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_Zjets.root");    
  }else if( result == "oldTF"){ 
    //2.2 fb-1
    sprintf(filebkg_Zjets,"/cdf/disk02/g6/casal/WH/Zjets.root");    
  }else{
    cout <<"Error Zjets !!!" << endl; return;
  }
    
  bkg_Zjets->Add(filebkg_Zjets);
  cout<<filebkg_Zjets<<endl;

  //Z+jets
  TChain* bkg_Zjets_Peter = new TChain("METree");
  char filebkg_Zjets_Peter[500];
  sprintf(filebkg_Zjets_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_Zjets.root");    
  bkg_Zjets_Peter->Add(filebkg_Zjets_Peter);
  cout<<filebkg_Zjets_Peter<<endl;
  
  TChain* bkg_nonW = new TChain("METree");
  char filebkg_nonW[500];
  if( result == "newTF"){
    sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*antiele*.root");    
    bkg_nonW->Add(filebkg_nonW);
    cout<<filebkg_nonW<<endl;
    sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*jetele*.root");    
    bkg_nonW->Add(filebkg_nonW);
    cout<<filebkg_nonW<<endl;
    sprintf(filebkg_nonW,"/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/*noniso*.root");    
    bkg_nonW->Add(filebkg_nonW);
    cout<<filebkg_nonW<<endl;
  }else if( result == "oldTF"){ 
    sprintf(filebkg_nonW,"/cdf/disk02/g6/casal/WH/nonW.root");    
    bkg_nonW->Add(filebkg_nonW);
    cout<<filebkg_nonW<<endl;
  }else{
    cout <<"Error nonW !!!" << endl; return;
  }


  //nonW
  TChain* bkg_nonW_Peter = new TChain("METree");
  char filebkg_nonW_Peter[500];
  sprintf(filebkg_nonW_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_nonWdata.root");    
  bkg_nonW_Peter->Add(filebkg_nonW_Peter);
  cout<<filebkg_nonW_Peter<<endl;
  


  //**************W+Charm****************************

  TChain* bkg_Wcharm = new TChain("METree");
  char filebkg_Wcharm[500];
  if( result == "newTF"){
    sprintf(filebkg_Wcharm,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_charm.root");    
  }else if( result == "oldTF"){ 
    sprintf(filebkg_Wcharm,"/cdf/disk02/g6/casal/WH/Wcharm.root");    
  }else{
    cout <<"Error nonW !!!" << endl; return;
  }

  bkg_Wcharm->Add(filebkg_Wcharm);
  cout<<filebkg_Wcharm<<endl;

  TChain* bkg_Wcharm_Peter = new TChain("METree");
  char filebkg_Wcharm_Peter[500];
  sprintf(filebkg_Wcharm_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*charm.root");    
  bkg_Wcharm_Peter->Add(filebkg_Wcharm_Peter);
  cout<<filebkg_Wcharm_Peter<<endl;

  //************W+Light*********************** 
  TChain* bkg_Wlight = new TChain("METree");
  char filebkg_Wlight[500];
  if( result == "newTF"){
    sprintf(filebkg_Wlight,"/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/*_light.root");    
    bkg_Wlight->Add(filebkg_Wlight);
    cout<<filebkg_Wlight<<endl;
  }else if( result == "oldTF"){ 
    sprintf(filebkg_Wlight,"/cdf/disk02/g6/casal/WH/Wlight1.root");    
    bkg_Wlight->Add(filebkg_Wlight);
    cout<<filebkg_Wlight<<endl;
    sprintf(filebkg_Wlight,"/cdf/disk02/g6/casal/WH/Wlight2.root");    
    bkg_Wlight->Add(filebkg_Wlight);
    cout<<filebkg_Wlight<<endl;
    sprintf(filebkg_Wlight,"/cdf/disk02/g6/casal/WH/Wlight3.root");    
    bkg_Wlight->Add(filebkg_Wlight);
    cout<<filebkg_Wlight<<endl; 
  }else{
    cout <<"Error Wlight !!!" << endl; 
    return;
  }

  
  TChain* bkg_Wlight_Peter = new TChain("METree");
  char filebkg_Wlight_Peter[500];
  sprintf(filebkg_Wlight_Peter,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/standar_TF/MicroNtuples/micro_*light.root");    
  bkg_Wlight_Peter->Add(filebkg_Wlight_Peter);
  cout<<filebkg_Wlight_Peter<<endl;



		         
  cout<<"Total Entries WH signal  =  " <<WH_signal->GetEntries() <<endl;
  cout<<"Total Entries Wbottom bkg   " <<bkg_Wbottom->GetEntries()         <<endl;
  cout<<"Total Entries ttbar bkg     " <<bkg_tt->GetEntries()              <<endl;
  cout<<"Total Entries schannel bkg  " <<bkg_schannel->GetEntries()        <<endl;
  cout<<"Total Entries tchannel bkg  " <<bkg_tchannel->GetEntries()        <<endl;
  cout<<"Total Entries diboson bkg   " <<bkg_diboson->GetEntries()         <<endl;
  cout<<"Total Entries Z+jets bkg    " <<bkg_Zjets->GetEntries()           <<endl;
  cout<<"Total Entries Wcharm bkg    " <<bkg_Wcharm->GetEntries()          <<endl;
  cout<<"Total Entries W+light bkg   " <<bkg_Wlight->GetEntries()          <<endl;
  cout<<"Total Entries nonW bkg      " <<bkg_nonW->GetEntries()          <<endl;

  cout <<"Peter" << endl;
  cout<<"Total Entries WH signal  = " <<WH_signal_Peter->GetEntries() <<endl;
  cout<<"Total Entries Wbottom bkg   " <<bkg_Wbottom_Peter->GetEntries()         <<endl;
  cout<<"Total Entries ttbar bkg     " <<bkg_tt_Peter->GetEntries()              <<endl;
  cout<<"Total Entries schannel bkg  " <<bkg_schannel_Peter->GetEntries()        <<endl;
  cout<<"Total Entries tchannel bkg  " <<bkg_tchannel_Peter->GetEntries()        <<endl;
  cout<<"Total Entries diboson bkg   " <<bkg_diboson_Peter->GetEntries()         <<endl;
  cout<<"Total Entries Z+jets bkg    " <<bkg_Zjets_Peter->GetEntries()           <<endl;
  cout<<"Total Entries Wcharm bkg    " <<bkg_Wcharm_Peter->GetEntries()          <<endl;
  cout<<"Total Entries nonW bkg      " <<bkg_nonW_Peter->GetEntries()          <<endl;
  cout<<"Total Entries W+light bkg   " <<bkg_Wlight_Peter->GetEntries()          <<endl;

  TCanvas *c_histo = new TCanvas("c_histo", "Discriminant", 1000, 800);
  c_histo->Divide(1,2);
  c_histo->cd(1);

  const unsigned numberofbins = 40;      
  TH1F* WH_signal_histo    = new TH1F("WH_signal_histo", "WH_signal_histo", numberofbins, 0, 1);
  WH_signal_histo->SetNormFactor(1);
  WH_signal_histo->SetLineWidth(2);
  WH_signal_histo->SetLineColor(2);

  TH1F* WH_signal_histo_Peter    = new TH1F("WH_signal_histo_Peter", "WH_signal_histo_Peter", numberofbins, 0, 1);
  WH_signal_histo_Peter->SetNormFactor(1);
  WH_signal_histo_Peter->SetLineWidth(2);
  WH_signal_histo_Peter->SetLineColor(2);
  WH_signal_histo_Peter->SetLineStyle(2);




  TH1F* bkg_Wbottom_histo  = new TH1F("bkg_Wbottom_histo", "bkg_Wbottom_histo", numberofbins, 0, 1);
  bkg_Wbottom_histo->SetNormFactor(1);
  bkg_Wbottom_histo->SetLineWidth(2);
  bkg_Wbottom_histo->SetLineColor(8);

  TH1F* bkg_Wbottom_histo_Peter  = new TH1F("bkg_Wbottom_histo_Peter", "bkg_Wbottom_histo_Peter", numberofbins, 0, 1);
  bkg_Wbottom_histo_Peter->SetNormFactor(1);
  bkg_Wbottom_histo_Peter->SetLineWidth(2);
  bkg_Wbottom_histo_Peter->SetLineColor(8);
  bkg_Wbottom_histo_Peter->SetLineStyle(2);



  TH1F* bkg_tt_histo_Peter       = new TH1F("bkg_tt_histo_Peter", "bkg_tt_histo_Peter", numberofbins, 0, 1);
  bkg_tt_histo_Peter->SetNormFactor(1);
  bkg_tt_histo_Peter->SetLineWidth(2);
  bkg_tt_histo_Peter->SetLineColor(37);
  bkg_tt_histo_Peter->SetLineStyle(2);

  TH1F* bkg_tt_histo       = new TH1F("bkg_tt_histo", "bkg_tt_histo", numberofbins, 0, 1);
  bkg_tt_histo->SetNormFactor(1);
  bkg_tt_histo->SetLineWidth(2);
  bkg_tt_histo->SetLineColor(37);
  
  TH1F* bkg_nonW_histo_Peter       = new TH1F("bkg_nonW_histo_Peter", "bkg_nonW_histo_Peter", numberofbins, 0, 1);
  bkg_nonW_histo_Peter->SetNormFactor(1);
  bkg_nonW_histo_Peter->SetLineWidth(2);
  bkg_nonW_histo_Peter->SetLineStyle(2);

  TH1F* bkg_nonW_histo       = new TH1F("bkg_nonW_histo", "bkg_nonW_histo", numberofbins, 0, 1);
  bkg_nonW_histo->SetNormFactor(1);
  bkg_nonW_histo->SetLineWidth(2);
  

  TH1F* bkg_schannel_histo = new TH1F("bkg_schannel_histo", "bkg_schannel_histo", numberofbins, 0, 1);
  bkg_schannel_histo->SetNormFactor(1);
  bkg_schannel_histo->SetLineWidth(2);
  bkg_schannel_histo->SetLineColor(102);

  TH1F* bkg_schannel_histo_Peter = new TH1F("bkg_schannel_histo_Peter", "bkg_schannel_histo_Peter", numberofbins, 0, 1);
  bkg_schannel_histo_Peter->SetNormFactor(1);
  bkg_schannel_histo_Peter->SetLineWidth(2);
  bkg_schannel_histo_Peter->SetLineColor(102);
  bkg_schannel_histo_Peter->SetLineStyle(2);


  TH1F* bkg_tchannel_histo = new TH1F("bkg_tchannel_histo", "bkg_tchannel_histo", numberofbins, 0, 1);
  bkg_tchannel_histo->SetNormFactor(1);
  bkg_tchannel_histo->SetLineWidth(2);
  bkg_tchannel_histo->SetLineColor(96);

  TH1F* bkg_tchannel_histo_Peter = new TH1F("bkg_tchannel_histo_Peter", "bkg_tchannel_histo_Peter", numberofbins, 0, 1);
  bkg_tchannel_histo_Peter->SetNormFactor(1);
  bkg_tchannel_histo_Peter->SetLineWidth(2);
  bkg_tchannel_histo_Peter->SetLineColor(96);
  bkg_tchannel_histo_Peter->SetLineStyle(2);

  TH1F* bkg_diboson_histo  = new TH1F("bkg_diboson_histo", "bkg_diboson_histo", numberofbins, 0, 1);
  bkg_diboson_histo->SetNormFactor(1);
  bkg_diboson_histo->SetLineWidth(2);
  bkg_diboson_histo->SetLineColor(41);

  TH1F* bkg_diboson_histo_Peter  = new TH1F("bkg_diboson_histo_Peter", "bkg_diboson_histo_Peter", numberofbins, 0, 1);
  bkg_diboson_histo_Peter->SetNormFactor(1);
  bkg_diboson_histo_Peter->SetLineWidth(2);
  bkg_diboson_histo_Peter->SetLineColor(41);
  bkg_diboson_histo_Peter->SetLineStyle(2);

  TH1F* bkg_Zjets_histo    = new TH1F("bkg_Zjets_histo", "bkg_Zjets_histo", numberofbins, 0, 1);
  bkg_Zjets_histo->SetNormFactor(1); 
  bkg_Zjets_histo->SetLineWidth(2);
  bkg_Zjets_histo->SetLineColor(4);

  TH1F* bkg_Zjets_histo_Peter    = new TH1F("bkg_Zjets_histo_Peter", "bkg_Zjets_histo_Peter", numberofbins, 0, 1);
  bkg_Zjets_histo_Peter->SetNormFactor(1); 
  bkg_Zjets_histo_Peter->SetLineWidth(2);
  bkg_Zjets_histo_Peter->SetLineColor(4);
  bkg_Zjets_histo_Peter->SetLineStyle(2);



  TH1F* bkg_Wcharm_histo     = new TH1F("bkg_Wcharm_histo", "bkg_Wcharm_histo", numberofbins, 0, 1);
  bkg_Wcharm_histo->SetNormFactor(1); 
  bkg_Wcharm_histo->SetLineWidth(2);
  bkg_Wcharm_histo->SetLineColor(8);

  TH1F* bkg_Wcharm_histo_Peter     = new TH1F("bkg_Wcharm_histo_Peter", "bkg_Wcharm_histo_Peter", numberofbins, 0, 1);
  bkg_Wcharm_histo_Peter->SetNormFactor(1); 
  bkg_Wcharm_histo_Peter->SetLineWidth(2);
  bkg_Wcharm_histo_Peter->SetLineColor(8);
  bkg_Wcharm_histo_Peter->SetLineStyle(2);

  TH1F* bkg_Wlight_histo      = new TH1F("bkg_Wlight_histo", "bkg_Wlight_histo", numberofbins, 0, 1);
  bkg_Wlight_histo->SetNormFactor(1); 
  bkg_Wlight_histo->SetLineWidth(2);
  bkg_Wlight_histo->SetLineColor(3);
  bkg_Wlight_histo->GetXaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo->GetYaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Wlight_histo->GetYaxis()->SetTitle("Normalized to unit area");
  bkg_Wlight_histo->SetTitle("");

  TH1F* bkg_Wlight_histo_Peter      = new TH1F("bkg_Wlight_histo_Peter", "bkg_Wlight_histo_Peter", numberofbins, 0, 1);
  bkg_Wlight_histo_Peter->SetNormFactor(1); 
  bkg_Wlight_histo_Peter->SetLineWidth(2);
  bkg_Wlight_histo_Peter->SetLineColor(3);
  bkg_Wlight_histo_Peter->GetXaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo_Peter->GetYaxis()->SetTitleSize(0.045);
  bkg_Wlight_histo_Peter->GetXaxis()->SetTitle("Event Probability Discriminant");
  bkg_Wlight_histo_Peter->GetYaxis()->SetTitle("Normalized to unit area");
  bkg_Wlight_histo_Peter->SetTitle("");
  bkg_Wlight_histo_Peter->SetLineStyle(2);
 
 
  cout <<"*** double tag ****" << endl;
  cout << "Index " << index << ", peter's index " << index_Peter <<  endl;
  //nonW
  std::ostringstream str2tag_nonW;
  str2tag_nonW<< "(epd2tagWH[" << index << "])>>bkg_nonW_histo";
  // bkg_nonW->Draw(str2tag_nonW.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)"); 
  
  std::ostringstream str2tag_nonW_Peter;
  str2tag_nonW_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_nonW_histo_Peter";
  //bkg_nonW_Peter->Draw(str2tag_nonW_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)" ,"same"); 
  
  //light
  std::ostringstream str2tag_Wlight;
  str2tag_Wlight<< "(epd2tagWH[" << index << "])>>bkg_Wlight_histo";
  //bkg_Wlight->Draw(str2tag_Wlight.str().c_str(),"((h.ntag==2))*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Wlight_Peter;
  str2tag_Wlight_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_Wlight_histo_Peter";
  //bkg_Wlight_Peter->Draw(str2tag_Wlight_Peter.str().c_str(),"((h.ntag==2))*(h.triggerTO*h.wgt)","same"); 

  //Wcharm 
  std::ostringstream str2tag_Wcharm_Peter;
  str2tag_Wcharm_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_Wcharm_histo_Peter";
  //bkg_Wcharm_Peter->Draw(str2tag_Wcharm_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)",""); 

  std::ostringstream str2tag_Wcharm;
  str2tag_Wcharm<< "(epd2tagWH[" << index << "])>>bkg_Wcharm_histo";
  // bkg_Wcharm->Draw(str2tag_Wcharm.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //Wbottom 
  std::ostringstream str2tag_Wbottom_Peter;
  str2tag_Wbottom_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_Wbottom_histo_Peter";
  //bkg_Wbottom_Peter->Draw(str2tag_Wbottom_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Wbottom;
  str2tag_Wbottom<< "(epd2tagWH[" << index << "])>>bkg_Wbottom_histo";
  //bkg_Wbottom->Draw(str2tag_Wbottom.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //WH
  std::ostringstream str2tag_WH;
  str2tag_WH<< "(epd2tagWH[" << index << "])>>WH_signal_histo";
  //WH_signal->Draw(str2tag_WH.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_WH_Peter;
  str2tag_WH_Peter<< "(epd2tagWH[" << index_Peter << "])>>WH_signal_histo_Peter";
  WH_signal_Peter->Draw(str2tag_WH_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)",""); 
  
  //tt
  std::ostringstream str2tag_tt;
  str2tag_tt<< "(epd2tagWH[" << index << "])>>bkg_tt_histo";
  //bkg_tt->Draw(str2tag_tt.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_tt_Peter;
  str2tag_tt_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_tt_histo_Peter";
  //bkg_tt_Peter->Draw(str2tag_tt_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //schannel
  std::ostringstream str2tag_schannel;
  str2tag_schannel<< "(epd2tagWH[" << index << "])>>bkg_schannel_histo";
  //bkg_schannel->Draw(str2tag_schannel.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_schannel_Peter;
  str2tag_schannel_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_schannel_histo_Peter";
  //bkg_schannel_Peter->Draw(str2tag_schannel_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //tchannel
  std::ostringstream str2tag_tchannel;
  str2tag_tchannel<< "(epd2tagWH[" << index << "])>>bkg_tchannel_histo";
  //bkg_tchannel->Draw(str2tag_tchannel.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_tchannel_Peter;
  str2tag_tchannel_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_tchannel_histo_Peter";
  //bkg_tchannel_Peter->Draw(str2tag_tchannel_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //Zjets
  std::ostringstream str2tag_Zjets;
  str2tag_Zjets<< "(epd2tagWH[" << index << "])>>bkg_Zjets_histo";
  //bkg_Zjets->Draw(str2tag_Zjets.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_Zjets_Peter;
  str2tag_Zjets_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_Zjets_histo_Peter";
  //bkg_Zjets_Peter->Draw(str2tag_Zjets_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  //diboson
  std::ostringstream str2tag_diboson;
  str2tag_diboson<< "(epd2tagWH[" << index << "])>>bkg_diboson_histo";
  //bkg_diboson->Draw(str2tag_diboson.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  std::ostringstream str2tag_diboson_Peter;
  str2tag_diboson_Peter<< "(epd2tagWH[" << index_Peter << "])>>bkg_diboson_histo_Peter";
  //bkg_diboson_Peter->Draw(str2tag_diboson_Peter.str().c_str(),"(h.ntag==2)*(h.triggerTO*h.wgt)","same"); 
  
  
  
  
 
  TLegend *legend = new TLegend (0.17, 0.34, 0.56, 0.86);
  legend->SetFillColor(10); 

  TLegend *legend_P = new TLegend (0.44, 0.32, 0.82, 0.85);
  legend_P->SetFillColor(10); 
  if(higgsmass == "WH115"){

    legend->AddEntry(WH_signal_histo,        "WH (115 GeV) ","l");
    legend_P->AddEntry(WH_signal_histo_Peter,"WH (115 GeV) Peter","l");
  }else  if(higgsmass == "WH120"){

    legend->AddEntry(WH_signal_histo,        "WH (120 GeV) ","l");
    legend_P->AddEntry(WH_signal_histo_Peter,"WH (120 GeV) Peter","l");
  }else  if(higgsmass == "WH150"){

    legend->AddEntry(WH_signal_histo,        "WH (150 GeV) ","l");
    legend_P->AddEntry(WH_signal_histo_Peter,"WH (150 GeV) Peter","l");
  }else{
    cout << "Writting the legend " << endl;
    return;
  }


  legend->AddEntry(bkg_tt_histo,         "t#bar{t}","l");
  legend->AddEntry(bkg_schannel_histo,   "s-chan","l");
  legend->AddEntry(bkg_tchannel_histo,   "t-chan","l");
  legend->AddEntry(bkg_Zjets_histo,      "Z+jets","l");
  legend->AddEntry(bkg_diboson_histo,    "Diboson","l");
  legend->AddEntry(bkg_Wbottom_histo,    "W+bottom","l");
  legend->AddEntry(bkg_Wcharm_histo,     "W+charm","l");
  //  legend->AddEntry(bkg_Wlight_histo,     "W+light","l");
  //  legend->AddEntry(bkg_nonW_histo,       "nonW","l");
  legend->Draw("Same");

  legend_P->AddEntry(bkg_tt_histo_Peter,         "t#bar{t} Peter","l");
  legend_P->AddEntry(bkg_schannel_histo_Peter,   "s-chan Peter","l");
  legend_P->AddEntry(bkg_tchannel_histo_Peter,   "t-chan Peter","l");
  legend_P->AddEntry(bkg_Zjets_histo_Peter,      "Z+jets Peter","l");
  legend_P->AddEntry(bkg_diboson_histo_Peter,    "Diboson Peter","l");
  legend_P->AddEntry(bkg_Wbottom_histo_Peter,    "W+bottom Peter","l");
  legend_P->AddEntry(bkg_Wcharm_histo_Peter,     "W+charm Peter","l");
  //  legend_P->AddEntry(bkg_Wlight_histo_Peter,     "W+light Peter","l");
  //  legend_P->AddEntry(bkg_nonW_histo_Peter,       "nonW Peter","l");
  legend_P->Draw("Same");



  c_histo->cd(2);
  
  double significance[numberofbins];
  double significance_Peter[numberofbins];
  double newdiscr[numberofbins];

  for(int i=0; i<40; i++){
    significance[i] = -999.;
    significance_Peter[i]= -999.;
    newdiscr[i]= -999.;
  }

  double signal        = -999.;
  double bckg          = -999.; 
  double bckg_Wbbbar   = -999.; 
  double bckg_Wcharm   = -999.; 
  double bckg_Wlight   = -999.; 
  double bckg_tt       = -999.; 
  double bckg_schannel = -999.;
  double bckg_tchannel = -999.;
  double bckg_Zjets    = -999.;
  double bckg_diboson  = -999.;
  double bckg_nonW     = -999.;
 
  double signal_Peter       = -999.;
  double bckg_Peter         = -999.; 
  double bckg_Wbbbar_Peter  = -999.; 
  double bckg_Wcharm_Peter  = -999.; 
  double bckg_Wlight_Peter  = -999.; 
  double bckg_tt_Peter      = -999.; 
  double bckg_schannel_Peter= -999.;
  double bckg_tchannel_Peter= -999.;
  double bckg_Zjets_Peter   = -999.;
  double bckg_diboson_Peter = -999.;
  double bckg_nonW_Peter    = -999.;

  //WH
  double WH_total       = -999.;
  WH_total       = WH_signal_histo->Integral(1,numberofbins);
  double WH_total_Peter = -999.;
  WH_total_Peter = WH_signal_histo_Peter->Integral(1,numberofbins);
  
  //schannel  
  double bgtotal_schannel          = -999.;
  bgtotal_schannel          = bkg_schannel_histo->Integral(1,numberofbins) ;
  double bgtotal_schannel_Peter    = -999.;
  cout << "bgtotal_schannel_Peter  " <<bgtotal_schannel_Peter << endl;
  bgtotal_schannel_Peter    =   bkg_schannel_histo_Peter->Integral(1,numberofbins) ;
  
  cout << "bgtotal_schannel_Peter  " <<bgtotal_schannel_Peter <<", numberofbins  " << numberofbins <<  endl;

  //tchannel
  double bgtotal_tchannel          = -999.;
  bgtotal_tchannel          =bkg_tchannel_histo->Integral(1,numberofbins);
  double bgtotal_tchannel_Peter    = -999.;
  bgtotal_tchannel_Peter    =bkg_tchannel_histo_Peter->Integral(1,numberofbins); 
  
  //ttbar
  double bgtotal_tt           = -999.;
  bgtotal_tt           =bkg_tt_histo->Integral(1,numberofbins);
  double bgtotal_tt_Peter     = -999.;
  bgtotal_tt_Peter     = bkg_tt_histo_Peter->Integral(1,numberofbins);
  
  //Wbbbar
  double bgtotal_Wbbbar       =-999.;
  bgtotal_Wbbbar       =bkg_Wbottom_histo->Integral(1,numberofbins);
  double bgtotal_Wbbbar_Peter = -999.;
  bgtotal_Wbbbar_Peter =bkg_Wbottom_histo_Peter->Integral(1,numberofbins);
  
  //Wcharm
  double bgtotal_Wcharm       = -999.;
  bgtotal_Wcharm       =bkg_Wcharm_histo->Integral(1,numberofbins);
  double bgtotal_Wcharm_Peter = -999.;
  bgtotal_Wcharm_Peter =bkg_Wcharm_histo_Peter->Integral(1,numberofbins);
  
  //Wlight
  double bgtotal_Wlight_Peter = -999.;
  bgtotal_Wlight_Peter = bkg_Wlight_histo_Peter->Integral(1,numberofbins);
  double bgtotal_Wlight       = -999.;
  bgtotal_Wlight       = bkg_Wlight_histo->Integral(1,numberofbins);
  
  //diboson
  double bgtotal_diboson_Peter = -999.;
  bgtotal_diboson_Peter =bkg_diboson_histo_Peter->Integral(1,numberofbins);
  double bgtotal_diboson       = -999.;
  bgtotal_diboson       = bkg_diboson_histo->Integral(1,numberofbins);
  
  //Zjets
  double bgtotal_Zjets_Peter = -999.;
  bgtotal_Zjets_Peter = bkg_Zjets_histo_Peter->Integral(1,numberofbins);
  double bgtotal_Zjets       =-999.;
  bgtotal_Zjets       = bkg_Zjets_histo->Integral(1,numberofbins);
  
  //nonW
  double bgtotal_nonW       = -999.;
  bgtotal_nonW       = bkg_nonW_histo->Integral(1,numberofbins);
  double bgtotal_nonW_Peter = -999.;
  bgtotal_nonW_Peter = bkg_nonW_histo_Peter->Integral(1,numberofbins);

  for(int i=0; i<40; i++){


    //binning
    newdiscr[i] =(i/40.)+((1/40.)/2.);
    //signal
    signal = WH_signal_histo->Integral(i+1,numberofbins)/WH_total;
    signal_Peter = WH_signal_histo_Peter->Integral(i+1,numberofbins)/WH_total_Peter;
     
    //bkgs
    
    bckg_tt       = 0.085*(bkg_tt_histo->Integral(i+1,numberofbins)/bgtotal_tt);
    bckg_Wbbbar   = 0.265*(bkg_Wbottom_histo->Integral(i+1,numberofbins)/bgtotal_Wbbbar);
    bckg_tchannel = 0.03*(bkg_tchannel_histo->Integral(i+1,numberofbins)/bgtotal_tchannel);
    bckg_schannel = 0.02*(bkg_schannel_histo->Integral(i+1,numberofbins)/bgtotal_schannel);
    bckg_Wcharm   = 0.231*(bkg_Wcharm_histo->Integral(i+1,numberofbins)/bgtotal_Wcharm); 
    bckg_Wlight   = 0.225*(bkg_Wlight_histo->Integral(i+1,numberofbins)/bgtotal_Wlight); 
    bckg_Zjets    = 0.022*(bkg_Zjets_histo->Integral(i+1,numberofbins)/bgtotal_Zjets);
    bckg_diboson  = 0.039*(bkg_diboson_histo->Integral(i+1,numberofbins)/bgtotal_diboson);
    bckg_nonW     = 0.076*(bkg_nonW_histo->Integral(i+1,numberofbins)/bgtotal_nonW);
    
    //bckg          = ( bckg_tt +  bckg_Wbbbar + bckg_tchannel + bckg_schannel + bckg_Wcharm + bckg_Wlight + bckg_Zjets + bckg_diboson + bckg_nonW);
    
    bckg          = ( bckg_tt +  bckg_Wbbbar + bckg_tchannel + bckg_schannel + bckg_Wcharm + bckg_Zjets + bckg_diboson );
       
    //bckgs
    
    bckg_tt_Peter       = 0.085*(bkg_tt_histo_Peter->Integral(i+1,numberofbins)/bgtotal_tt_Peter);
    bckg_Wbbbar_Peter   = 0.265*(bkg_Wbottom_histo_Peter->Integral(i+1,numberofbins)/bgtotal_Wbbbar_Peter);
    bckg_tchannel_Peter = 0.03*(bkg_tchannel_histo_Peter->Integral(i+1,numberofbins)/bgtotal_tchannel_Peter);
    bckg_schannel_Peter = 0.02*(bkg_schannel_histo_Peter->Integral(i+1,numberofbins)/bgtotal_schannel_Peter);
    bckg_Wcharm_Peter   = 0.231*(bkg_Wcharm_histo_Peter->Integral(i+1,numberofbins)/bgtotal_Wcharm_Peter); 
    bckg_Wlight_Peter   = 0.225*(bkg_Wlight_histo_Peter->Integral(i+1,numberofbins)/bgtotal_Wlight_Peter); 
    bckg_Zjets_Peter    = 0.022*(bkg_Zjets_histo_Peter->Integral(i+1,numberofbins)/bgtotal_Zjets_Peter);
    bckg_diboson_Peter  = 0.039*(bkg_diboson_histo_Peter->Integral(i+1,numberofbins)/bgtotal_diboson_Peter);
    bckg_nonW_Peter     = 0.076*(bkg_nonW_histo_Peter->Integral(i+1,numberofbins)/bgtotal_nonW_Peter);


    //bckg_Peter          = ( bckg_tt_Peter +  bckg_Wbbbar_Peter + bckg_tchannel_Peter + bckg_schannel_Peter + bckg_Wcharm_Peter + bckg_Wlight_Peter + bckg_Zjets_Peter +  bckg_diboson_Peter + bckg_nonW_Peter);
    
    bckg_Peter          = ( bckg_tt_Peter +  bckg_Wbbbar_Peter + bckg_tchannel_Peter + bckg_schannel_Peter + bckg_Wcharm_Peter +  bckg_Zjets_Peter +  bckg_diboson_Peter);

    // cout << "Peter bkg " << bckg_Peter << endl;
    //significance
    if (bckg==0) {
      bckg = 0.000001;
      cout << "The sume of all the new bckgs is cero " << endl;
    }
    significance[i] = signal/sqrt(bckg);
    significance_Peter[i] = signal_Peter/sqrt(bckg_Peter);
    // cout << " significance[i] " << significance[i] << ", significance_Peter[i]  " << significance_Peter[i]  <<  endl;

  }  
  
  TH2F *hr = new TH2F("hr","Significance",10,0,1,6,0.,3.2);

  hr->GetXaxis()->SetNdivisions(510);
  hr->GetYaxis()->SetLabelFont(72);
  //hr->GetYaxis()->SetLog();
  hr->SetTitle("N_{S}(EPD #geq EPD_{cut})/ sqrt[N_{B}(EPD #geq EPD_{cut})]");
  hr->GetYaxis()->SetTitleSize(0.05);
  hr->GetYaxis()->SetTitle("N_{S}/ #sqrt{N_{B}}");
  hr->GetXaxis()->SetTitleSize(0.05);
  hr->GetXaxis()->SetTitle("EPD_{cut}");
  hr->Draw();
    
  TLegend *leg1 = new TLegend (0.26, 0.28, 0.63, 0.52);
  TGraph *signalandbkgnew = new TGraph(numberofbins, newdiscr, significance);
  TGraph *signalandbkgnew_Peter = new TGraph(numberofbins, newdiscr, significance_Peter);

  gPad->SetGrid(2);
  signalandbkgnew->SetLineColor(4);
  signalandbkgnew->SetLineStyle(2);
  signalandbkgnew->SetLineWidth(2);
  signalandbkgnew_Peter->SetLineWidth(2);
  signalandbkgnew_Peter->SetMarkerStyle(3);
  signalandbkgnew->SetMarkerStyle(23);
  signalandbkgnew->SetMarkerColor(4);


 
  leg1->SetFillColor(10);
  leg1->AddEntry(signalandbkgnew_Peter, "TF(E_{p}, E_{j}) (2.7 fb^{-1} result) ","pl" );
  if( result == "newTF"){
    leg1->AddEntry(signalandbkgnew, "TF(E_{p}, NNoutput) (2.7 fb^{-1} result)","pl" );
  }else if( result == "oldTF"){ 
    leg1->AddEntry(signalandbkgnew, "TF(E_{p}, E_{j}) (2.2 fb^{-1} result)","pl" );
  }else{
    cout << "Error in the legend " << endl;
    return;
  }
  leg1->Draw("Same");
 
  //mulitgraph
  TMultiGraph *mg1 = new  TMultiGraph();
  mg1->Add(signalandbkgnew); 
  mg1->Add(signalandbkgnew_Peter); 

  mg1->Draw("PC");
  c_histo->Update();
  // c_histo->Print("preblessing_plots/Discr_improvement_"+higgsmass+"_"+ntags+".eps");
  //c_histo->Print("preblessing_plots/Discr_improvement_"+higgsmass+"_"+ntags+".gif");
  
}
