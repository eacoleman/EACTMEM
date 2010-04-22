#include "PeterTools/Fitter.hh"
#include "TStyle.h"
#include "TChain.h"
#include "TH1.h"
#include "TLegend.h"
#include "TCanvas.h"
#include <vector>
#include "TGraph.h"
#include "TH2.h"
#include "TMultiGraph.h"
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////
//#include "PeterTools/Fitter.hh"
//Before, until I fix this code for being compiled
// root [0] .L ../shlib/libPeterTools.so
// root [1] .x Discriminant_schannelsignal.C
//////////////////////////////////////////////////////////////

//ttbar_choice = "nott" or ttbar_choice = "withtt"

void Discriminant_schannelsignal(TString ntags = "one", TString ttbar_choice = "nott")
{
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  /////////////////////////////////////
  //                                 //
  //  EVENT PROBABILITY ROOT FILES   // 
  //                                 //
  /////////////////////////////////////
 
  //trying to reproduce Peter Discriminant!
  
  TChain* signal_tchannel = new TChain("METree"); 
  char filesignal_tchannel[500];
  sprintf(filesignal_tchannel,"/cdf/disk02/g6/casal/2jet/tchan.root"); 
  signal_tchannel->Add(filesignal_tchannel);
  cout<<filesignal_tchannel<<endl;


  //Wbb  new TF(Ep,NNoutput)
  TChain* newbkg = new TChain("METree"); 
  char filenewbkg[600];
  sprintf(filenewbkg,"/cdf/disk02/g6/casal/2jet/Wbottom.root");
  newbkg->Add(filenewbkg);
  cout<<filenewbkg<<endl;
  
  
  //schannel new TF(Ep,NNoutput)
  TChain* signal_schannel = new TChain("METree");
  char filesignal_schannel[500];
  sprintf(filesignal_schannel,"/cdf/disk02/g6/casal/2jet/schan.root");    
  signal_schannel->Add(filesignal_schannel);
  cout<<filesignal_schannel<<endl;

  //ttbar
  TChain* newbkg_tt = new TChain("METree");
  char filenewbkg_tt[500];
  sprintf(filenewbkg_tt,"/cdf/disk02/g6/casal/2jet/ttdil.root");    
  newbkg_tt->Add(filenewbkg_tt);
  cout<<filenewbkg_tt<<endl;

  TChain* newbkg_ttlj = new TChain("METree");
  char filenewbkg_ttlj[500];
  sprintf(filenewbkg_ttlj,"/cdf/disk02/g6/casal/2jet/ttlj.root");    
  newbkg_ttlj->Add(filenewbkg_ttlj);
  cout<<filenewbkg_ttlj<<endl;
		         
  cout<<"Total Entries Wbbbar bg NEW TF   " <<newbkg->GetEntries()         <<endl;
  cout<<"Total Entries ttbar bg NEW TF    " <<newbkg_tt->GetEntries()      <<endl;
  cout<<"Total Entries ttbar bg NEW TF    " <<newbkg_ttlj->GetEntries()      <<endl;
  cout<<"Total Entries schannel bg NEW TF " <<signal_schannel->GetEntries()<<endl;
  cout<<"Total Entries tchannel bg NEW TF " <<signal_tchannel->GetEntries()<<endl;

  ///////////////////////////
  //                       //
  //      DISCRIMINANT     // 
  //                       //
  ///////////////////////////

  TCanvas *c = new TCanvas("Discriminant", "Discriminant", 1000, 800);
  c->Divide(1,2);
  c->cd(1);

  //blue is bg and red signal!!!
  const unsigned numberofbins = 40;      

  TLegend *leg = new TLegend (0.32, 0.41, 0.58, 0.86);
  //signal newTF
  leg->SetFillColor(10);

  //****************With ttbar ME ***********************************88
  TH1F* signal_tchannel_histo= new TH1F("signal_tchannel_histo","newTF",numberofbins,0,1);
  signal_tchannel_histo->SetNormFactor(1);
  signal_tchannel_histo->SetLineColor(96);
  signal_tchannel_histo->SetLineWidth(2);
  signal_tchannel_histo->SetTitle("EPD ");
  signal_tchannel_histo->GetXaxis()->SetTitleSize(0.05);
  signal_tchannel_histo->GetYaxis()->SetTitleSize(0.05);
  signal_tchannel_histo->GetXaxis()->SetTitle("EPD");
  signal_tchannel_histo->GetYaxis()->SetTitle("# of events");
  signal_tchannel_histo->SetTitle("Discriminant ");
 
  TH1F* newbkg_histo = new TH1F("newbkg_histo","newbkg_histo",numberofbins,0,1);
  newbkg_histo->SetNormFactor(1);
  newbkg_histo->SetLineWidth(2);
  newbkg_histo->SetLineColor(3);
  newbkg_histo->SetTitle("Discriminant");

  TH1F* newbkg_tt_histo = new TH1F("newbkg_tt_histo","newbkg_tt_histo",numberofbins,0,1);
  newbkg_tt_histo->SetNormFactor(1);
  newbkg_tt_histo->SetLineColor(13);
  newbkg_tt_histo->SetLineWidth(2);
  newbkg_tt_histo->GetXaxis()->SetTitleSize(0.05);
  newbkg_tt_histo->GetYaxis()->SetTitleSize(0.05);
  newbkg_tt_histo->GetXaxis()->SetTitle("EPD");
  newbkg_tt_histo->SetTitle("Discriminant ");
    
  TH1F* newbkg_ttlj_histo = new TH1F("newbkg_ttlj_histo","newbkg_ttlj_histo",numberofbins,0,1);
  newbkg_ttlj_histo->SetNormFactor(1);
  newbkg_ttlj_histo->SetLineWidth(2);
  newbkg_ttlj_histo->GetXaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo->GetYaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo->GetXaxis()->SetTitle("EPD");
  newbkg_ttlj_histo->SetTitle("Discriminant");
    
  TH1F* signal_schannel_histo = new TH1F("signal_schannel_histo","signal_schannel_histo",numberofbins,0,1);
  signal_schannel_histo->SetNormFactor(1);
  signal_schannel_histo->SetLineColor(2);
  signal_schannel_histo->SetLineWidth(2);

   //****************Without ttbar ME *********************************** 
  TH1F* signal_tchannel_histo_nottbar= new TH1F("signal_tchannel_histo_nottbar","newTF",numberofbins,0,1);
  signal_tchannel_histo_nottbar->SetNormFactor(1);
  signal_tchannel_histo_nottbar->SetLineColor(96);
  signal_tchannel_histo_nottbar->SetLineWidth(2);
  signal_tchannel_histo_nottbar->SetLineStyle(2);
  signal_tchannel_histo_nottbar->SetTitle("EPD ");
  signal_tchannel_histo_nottbar->GetXaxis()->SetTitleSize(0.05);
  signal_tchannel_histo_nottbar->GetYaxis()->SetTitleSize(0.05);
  signal_tchannel_histo_nottbar->GetXaxis()->SetTitle("EPD");
  signal_tchannel_histo_nottbar->GetYaxis()->SetTitle("# of events");
  signal_tchannel_histo_nottbar->SetTitle("Discriminant ");
 
  TH1F* newbkg_histo_nottbar = new TH1F("newbkg_histo_nottbar","newbkg_histo_nottbar",numberofbins,0,1);
  newbkg_histo_nottbar->SetNormFactor(1);
  newbkg_histo_nottbar->SetLineWidth(2);
  newbkg_histo_nottbar->SetLineColor(3);
  newbkg_histo_nottbar->SetLineStyle(2);
  newbkg_histo_nottbar->SetTitle("Discriminant");

  TH1F* newbkg_tt_histo_nottbar = new TH1F("newbkg_tt_histo_nottbar","newbkg_tt_histo_nottbar",numberofbins,0,1);
  newbkg_tt_histo_nottbar->SetNormFactor(1);
  newbkg_tt_histo_nottbar->SetLineColor(13);
  newbkg_tt_histo_nottbar->SetLineWidth(2);
  newbkg_tt_histo_nottbar->SetLineStyle(2);
  newbkg_tt_histo_nottbar->GetXaxis()->SetTitleSize(0.05);
  newbkg_tt_histo_nottbar->GetYaxis()->SetTitleSize(0.05);
  newbkg_tt_histo_nottbar->GetXaxis()->SetTitle("EPD");
  newbkg_tt_histo_nottbar->SetTitle("Discriminant ");
    
  TH1F* newbkg_ttlj_histo_nottbar = new TH1F("newbkg_ttlj_histo_nottbar","newbkg_ttlj_histo_nottbar",numberofbins,0,1);
  newbkg_ttlj_histo_nottbar->SetNormFactor(1);
  newbkg_ttlj_histo_nottbar->SetLineWidth(2);
  newbkg_ttlj_histo_nottbar->SetLineStyle(2);
  newbkg_ttlj_histo_nottbar->GetXaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo_nottbar->GetYaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo_nottbar->GetXaxis()->SetTitle("EPD");
  newbkg_ttlj_histo_nottbar->SetTitle("Discriminant ");
    
  TH1F* signal_schannel_histo_nottbar = new TH1F("signal_schannel_histo_nottbar","signal_schannel_histo_nottbar",numberofbins,0,1);
  signal_schannel_histo_nottbar->SetNormFactor(1);
  signal_schannel_histo_nottbar->SetLineColor(2);
  signal_schannel_histo_nottbar->SetLineWidth(2);
  signal_schannel_histo_nottbar->SetLineStyle(2);
  //Draw
  if( ntags == "one" ){

    // coefficientes-> {3, 3, 6, 1.513, 0.0454545, 0.25, 0.005, 0.005, .0001}

    newbkg_tt->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) + (mnt.eventProb[7] *0.0001))) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_tt_histo","((h.tagProb1*h.wgt*h.triggerTO))"); 

    
   
    newbkg_ttlj->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) + (mnt.eventProb[7] *0.0001))) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_ttlj_histo","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  
    newbkg->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) + (mnt.eventProb[7] *0.0001))) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_histo","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
   
    signal_schannel->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) + (mnt.eventProb[7] *0.0001))) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>signal_schannel_histo","((h.tagProb1*h.wgt*h.triggerTO))", "same");

    
    signal_tchannel->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) + (mnt.eventProb[7] *0.0001))) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>signal_tchannel_histo","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  

    newbkg_tt->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) )) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_tt_histo_nottbar","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  
   
    newbkg_ttlj->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) )) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_ttlj_histo_nottbar","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  
    newbkg->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) )) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>newbkg_histo_nottbar","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
   
    signal_schannel->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) )) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>signal_schannel_histo_nottbar","((h.tagProb1*h.wgt*h.triggerTO))", "same");
    
    signal_tchannel->Draw("(  (bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)))/((bProb[0]*((mnt.eventProb[0]*3.) + (mnt.eventProb[1] *3.) + (mnt.eventProb[2] * 6.)+ (mnt.eventProb[3] *1.513) )) + ((1-bProb[0])* ((mnt.eventProb[3] *0.045) +  (mnt.eventProb[4] *0.25) + (mnt.eventProb[5]*0.005) + (mnt.eventProb[6]*0.005)))   ))>>signal_tchannel_histo_nottbar","((h.tagProb1*h.wgt*h.triggerTO))", "same");

  }else if(ntags == "two"){

    //coefficients -> {5, 0.0227273, 0.605, 0.917, 0.0033,0.017, 0.0033, 0.0067, 0.00003}

    newbkg_tt->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.6))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.00003))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.6))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_tt_histo","((h.tagProb2*h.wgt*h.triggerTO))"); 
    
   
    newbkg_ttlj->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.00003))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_ttlj_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  
    newbkg->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.00003))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
   
    signal_schannel->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.00003))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>signal_schannel_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same");

    
    signal_tchannel->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917) + (mnt.eventProb[7]*0.00003))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>signal_tchannel_histo","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  

    newbkg_tt->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_tt_histo_nottbar","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  
   
    newbkg_ttlj->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_ttlj_histo_nottbar","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
  
    newbkg->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>newbkg_histo_nottbar","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
   
    signal_schannel->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917)+ (mnt.eventProb[7]*0.))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>signal_schannel_histo_nottbar","((h.tagProb2*h.wgt*h.triggerTO))", "same");

  
    signal_tchannel->Draw("(  ((bProb[0]*bProb[1] *(mnt.eventProb[0]*5.)) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))))/((bProb[0]*bProb[1] *((mnt.eventProb[0]*5.)+ (mnt.eventProb[3]*0.917) + (mnt.eventProb[7]*0.))) + (bProb[0]*(1 - bProb[1]) *((mnt.eventProb[1] * 0.02) + (mnt.eventProb[2] * 0.04))) + ((1-bProb[0])*(1 - bProb[1]) *((mnt.eventProb[3] * 0.0033) + (mnt.eventProb[4] * 0.017) + (mnt.eventProb[5]*0.0033) + (mnt.eventProb[6]*0.0067))))  )>>signal_tchannel_histo_nottbar","((h.tagProb2*h.wgt*h.triggerTO))", "same");
  }else{
    cout << "how many tags did you say ? " << endl;
    return;
  }

  leg->AddEntry(newbkg_histo,         "Wb#bar{b}","l");
  leg->AddEntry(newbkg_tt_histo,      "t#bar{t} (dilep)","l" );
  leg->AddEntry(newbkg_ttlj_histo,    "t#bar{t} (l+j)","l" );
  leg->AddEntry(signal_schannel_histo,"s-channel","l" );
  leg->AddEntry(signal_tchannel_histo,"t-channel","l");  

  leg->AddEntry(newbkg_histo_nottbar,         "Wb#bar{b} no t#bar{t} ME","l");
  leg->AddEntry(newbkg_tt_histo_nottbar,      "t#bar{t} (dilep) no t#bar{t} ME","l" );
  leg->AddEntry(newbkg_ttlj_histo_nottbar,    "t#bar{t} (l+j) no t#bar{t} ME","l" );
  leg->AddEntry(signal_schannel_histo_nottbar,"s-channel no t#bar{t} ME","l" );
  leg->AddEntry(signal_tchannel_histo_nottbar,"t-channel no t#bar{t} ME","l");  
  leg->Draw("Same");


  c->cd(2);
  double significance[numberofbins];
  double significance_nottbar[numberofbins];
  double newdiscr[numberofbins];

  double signal = -999.;
  double signal_nottbar= -999.;
  double bkg= -999.; 
  double bkg_Wbbbar= -999.; 
  double bkg_tt= -999.; 
  double bkg_ttlj= -999.; 
  double bkg_nottbar= -999.; 
  double bkg_Wbbbar_nottbar= -999.; 
  double bkg_tt_nottbar= -999.; 
  double bkg_ttlj_nottbar= -999.; 

  double stotal_schannel    = signal_schannel_histo->Integral(1,numberofbins) ;
  double stotal_tchannel    = signal_tchannel_histo->Integral(1,numberofbins);
  //bg 
  double bgtotal_tt         = newbkg_tt_histo->Integral(1,numberofbins);
  double bgtotal_ttlj       = newbkg_ttlj_histo->Integral(1,numberofbins);
  double bgtotal_Wbbbar     = newbkg_histo->Integral(1,numberofbins);

  double stotal_schannel_nottbar    = signal_schannel_histo_nottbar->Integral(1,numberofbins) ;
  double stotal_tchannel_nottbar    = signal_tchannel_histo_nottbar->Integral(1,numberofbins);
  //bg
  double bgtotal_tt_nottbar         = newbkg_tt_histo_nottbar->Integral(1,numberofbins);
  double bgtotal_ttlj_nottbar       = newbkg_ttlj_histo_nottbar->Integral(1,numberofbins);
  double bgtotal_Wbbbar_nottbar     = newbkg_histo_nottbar->Integral(1,numberofbins);


  for(int i=0; i<40; i++){


    //binning
    newdiscr[i] =(i/40.)+((1/40.)/2.);
    //signal
    signal = (0.6*(signal_tchannel_histo->Integral(i+1,numberofbins)/stotal_tchannel) + 0.4*(signal_schannel_histo->Integral(i+1,numberofbins)/stotal_schannel));
    signal_nottbar = (0.6*(signal_tchannel_histo_nottbar->Integral(i+1,numberofbins)/stotal_tchannel_nottbar) + 0.4*(signal_schannel_histo_nottbar->Integral(i+1,numberofbins)/stotal_schannel_nottbar));

    //bkgs
    bkg_tt       = .1*newbkg_tt_histo->Integral(i+1,numberofbins)/bgtotal_tt;
    bkg_ttlj     = .2*newbkg_ttlj_histo->Integral(i+1,numberofbins)/bgtotal_ttlj;
    bkg_Wbbbar   = .7*newbkg_histo->Integral(i+1,numberofbins)/bgtotal_Wbbbar;
    bkg          = (bkg_ttlj + bkg_tt +  bkg_Wbbbar);

    bkg_tt_nottbar       = .1*newbkg_tt_histo_nottbar->Integral(i+1,numberofbins)/bgtotal_tt_nottbar;
    bkg_ttlj_nottbar     = .2*newbkg_ttlj_histo_nottbar->Integral(i+1,numberofbins)/bgtotal_ttlj_nottbar;
    bkg_Wbbbar_nottbar   = .7*newbkg_histo_nottbar->Integral(i+1,numberofbins)/bgtotal_Wbbbar_nottbar;
    bkg_nottbar          = (bkg_ttlj_nottbar + bkg_tt_nottbar +  bkg_Wbbbar_nottbar);

    //significance
    if (bkg==0) {
      bkg = 0.000001;
      cout << "The sume of all the new bkgs is cero " << endl;
    }
    significance[i] = signal/sqrt(bkg);
    significance_nottbar[i] = signal_nottbar/sqrt(bkg_nottbar);
  }  
  
  TH2F *hr = new TH2F("hr","Significance",10,0,1,6,0.6,1.8);

  hr->GetXaxis()->SetNdivisions(510);
  hr->GetYaxis()->SetLabelFont(72);
  //hr->GetYaxis()->SetLog();
  hr->SetTitle("N_{S}(EPD #geq EPD_{cut})/ sqrt[N_{B}(EPD #geq EPD_{cut})]");
  hr->GetYaxis()->SetTitleSize(0.05);
  hr->GetYaxis()->SetTitle("N_{S}/ #sqrt{N_{B}}");
  hr->GetXaxis()->SetTitleSize(0.05);
  hr->GetXaxis()->SetTitle("EPD_{cut}");
  hr->Draw();
    
  TLegend *leg1 = new TLegend (0.42, 0.15, 0.86, 0.42);
  TGraph *signalandbkgnew = new TGraph(numberofbins, newdiscr, significance);
  TGraph *signalandbkgnew_nottbar = new TGraph(numberofbins, newdiscr, significance_nottbar);

  gPad->SetGrid(2);
  signalandbkgnew->SetLineColor(4);
  signalandbkgnew->SetLineStyle(2);
  signalandbkgnew->SetLineWidth(2);
  signalandbkgnew_nottbar->SetLineWidth(2);
  signalandbkgnew_nottbar->SetMarkerStyle(3);
  signalandbkgnew->SetMarkerStyle(23);
  signalandbkgnew->SetMarkerColor(4);


 
  leg1->SetFillColor(10);
  leg1->AddEntry(signalandbkgnew_nottbar, "TF(E_{p}, E_{j}) no t#bar{t} ME","pl" );
  leg1->AddEntry(signalandbkgnew, "TF(E_{p}, E_{j}) with t#bar{t} ME","pl" );
  leg1->Draw("Same");
 
  //mulitgraph
  TMultiGraph *mg1 = new  TMultiGraph();
  mg1->Add(signalandbkgnew); 
  mg1->Add(signalandbkgnew_nottbar); 

  mg1->Draw("PC");
  
  c->Update();
  if( ntags == "one" )c->Print("eps/Discr_schannelsignal_1tag.eps");
  else if( ntags == "two" )c->Print("eps/Discr_schannelsignal_2tags.eps");
  else {
    cout << "no eps file store " << endl;
    return;
  }


  
  
  TCanvas *c_2 = new TCanvas("c_2", "Discriminant", 1000, 800);
  c_2->cd(1);

  //blue is bg and red signal!!!
  //const double numberofbins = 40.;      

  TLegend *leg_2 = new TLegend (0.32, 0.58, 0.57, 0.86);

  TH1F* signal_tchannel_histo_2= new TH1F("signal_tchannel_histo_2","newTF",numberofbins,0,1);
  signal_tchannel_histo_2->SetNormFactor(1);
  signal_tchannel_histo_2->SetLineColor(96);
  signal_tchannel_histo_2->SetLineWidth(2);
  signal_tchannel_histo_2->SetTitle("EPD ");
  signal_tchannel_histo_2->GetXaxis()->SetTitleSize(0.05);
  signal_tchannel_histo_2->GetYaxis()->SetTitleSize(0.05);
  signal_tchannel_histo_2->GetXaxis()->SetTitle("EPD");
  signal_tchannel_histo_2->GetYaxis()->SetTitle("# of events");
  signal_tchannel_histo_2->SetTitle("Discriminant");
 
  TH1F* newbkg_histo_2 = new TH1F("newbkg_histo_2","newbkg_histo_2",numberofbins,0,1);
  newbkg_histo_2->SetNormFactor(1);
  newbkg_histo_2->SetLineWidth(2);
  newbkg_histo_2->SetLineColor(3);
  newbkg_histo_2->SetTitle("Discriminant");

  TH1F* newbkg_tt_histo_2 = new TH1F("newbkg_tt_histo_2","newbkg_tt_histo_2",numberofbins,0,1);
  newbkg_tt_histo_2->SetNormFactor(1);
  newbkg_tt_histo_2->SetLineColor(13);
  newbkg_tt_histo_2->SetLineWidth(2);
  newbkg_tt_histo_2->GetXaxis()->SetTitleSize(0.05);
  newbkg_tt_histo_2->GetYaxis()->SetTitleSize(0.05);
  newbkg_tt_histo_2->GetXaxis()->SetTitle("EPD");
  newbkg_tt_histo_2->SetTitle("Discriminant");
    
  TH1F* newbkg_ttlj_histo_2 = new TH1F("newbkg_ttlj_histo_2","newbkg_ttlj_histo_2",numberofbins,0,1);
  newbkg_ttlj_histo_2->SetNormFactor(1);
  newbkg_ttlj_histo_2->SetLineWidth(2);
  newbkg_ttlj_histo_2->GetXaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo_2->GetYaxis()->SetTitleSize(0.05);
  newbkg_ttlj_histo_2->GetXaxis()->SetTitle("EPD");
  newbkg_ttlj_histo_2->SetTitle("Discriminant");
    
  TH1F* signal_schannel_histo_2 = new TH1F("signal_schannel_histo_2","signal_schannel_histo_2",numberofbins,0,1);
  signal_schannel_histo_2->SetNormFactor(1);
  signal_schannel_histo_2->SetLineColor(2);
  signal_schannel_histo_2->SetLineWidth(2);
  if( ntags == "one" ){
    
    newbkg_tt->Draw("(epd1tag)>>newbkg_tt_histo_2","((h.tagProb1*h.wgt*h.triggerTO))"); 
    signal_schannel->Draw("(epd1tag)>>signal_schannel_histo_2","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    newbkg_ttlj->Draw("(epd1tag)>>newbkg_ttlj_histo_2","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    newbkg->Draw("(epd1tag)>>newbkg_histo_2","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
    signal_tchannel->Draw("(epd1tag)>>signal_tchannel_histo_2","((h.tagProb1*h.wgt*h.triggerTO))", "same"); 
  
  }else if( ntags == "two" ){
  
    newbkg_tt->Draw("(epd2tag)>>newbkg_tt_histo_2","((h.tagProb2*h.wgt*h.triggerTO))"); 
    signal_schannel->Draw("(epd2tag)>>signal_schannel_histo_2","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
    newbkg_ttlj->Draw("(epd2tag)>>newbkg_ttlj_histo_2","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
    newbkg->Draw("(epd2tag)>>newbkg_histo_2","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
    signal_tchannel->Draw("(epd2tag)>>signal_tchannel_histo_2","((h.tagProb2*h.wgt*h.triggerTO))", "same"); 
    
  }else{
    cout << "****************How many tags did you say ?? ****************" << endl;
  }

  leg_2->SetFillColor(10); 
  leg_2->AddEntry(newbkg_histo,         "Wb#bar{b}","l");
  leg_2->AddEntry(newbkg_tt_histo,      "t#bar{t} (dilep)","l" );
  leg_2->AddEntry(newbkg_ttlj_histo,    "t#bar{t} (l+j)","l" );
  leg_2->AddEntry(signal_schannel_histo,"schannel","l" );
  leg_2->AddEntry(signal_tchannel_histo,"tchannel","l");  
  leg_2->Draw("Same");

  c_2->Update();
  if( ntags == "one" ) c_2->Print("eps/Discr_schannelsignal_withEPDvariable_1tag.eps");
  else  if( ntags == "two" )c_2->Print("eps/Discr_schannelsignal_withEPDvariable_2tags.eps");
  else{
    cout << "Error ?? " << endl;
    return;
  }

  //Fitter
  TH1* data = signal_schannel_histo ;
  
  vector<Shape> shapes;
  //Signal
  if( ntags == "one" ){
    if( ttbar_choice =="nott"){
      shapes.push_back(Shape("s-channel",signal_schannel_histo_nottbar , Value(31.8, 4.4), false));
      shapes.push_back(Shape("t-channel",signal_tchannel_histo_nottbar , Value(60.4, 8.8), false));
      //BKGs
      shapes.push_back(Shape("Wbb", newbkg_histo_nottbar, Value(407.2, 122.7), true));
      shapes.push_back(Shape("ttljet",newbkg_ttlj_histo_nottbar, Value(80.8, 11.4), true));
      shapes.push_back(Shape("ttdil", newbkg_tt_histo_nottbar, Value(38.4, 5.4), true));
    }else if( ttbar_choice =="withtt"){
      shapes.push_back(Shape("s-channel",signal_schannel_histo , Value(31.8, 4.4), false));
      shapes.push_back(Shape("t-channel",signal_tchannel_histo , Value(60.4, 8.8), false));
      //BKGs
      shapes.push_back(Shape("Wbb", newbkg_histo, Value(407.2, 122.7), true));
      shapes.push_back(Shape("ttljet",newbkg_ttlj_histo, Value(80.8, 11.4), true));
      shapes.push_back(Shape("ttdil", newbkg_tt_histo, Value(38.4, 5.4), true));
      
    }else{
      cout << "decide with or without ttbar " << endl;
      return; 
    }
  }else  if( ntags == "two" ){
    if( ttbar_choice =="nott"){
      shapes.push_back(Shape("s-channel",signal_schannel_histo_nottbar , Value(9.4, 1.5), false));
      shapes.push_back(Shape("t-channel",signal_tchannel_histo_nottbar , Value(1.7, 0.3), false));
      //BKGs
      shapes.push_back(Shape("Wbb", newbkg_histo_nottbar, Value(54.4, 16.9), true));
      shapes.push_back(Shape("ttljet",newbkg_ttlj_histo_nottbar, Value(16.2, 2.7), true));
      shapes.push_back(Shape("ttdil", newbkg_tt_histo_nottbar, Value(10.7, 1.7), true));

    }else if( ttbar_choice =="withtt"){
      shapes.push_back(Shape("s-channel",signal_schannel_histo , Value(9.4, 1.5), false));
      shapes.push_back(Shape("t-channel",signal_tchannel_histo , Value(1.7, 0.3), false));
      //BKGs
      shapes.push_back(Shape("Wbb", newbkg_histo, Value(54.4, 16.9), true));
      shapes.push_back(Shape("ttljet",newbkg_ttlj_histo, Value(16.2, 2.7), true));
      shapes.push_back(Shape("ttdil", newbkg_tt_histo, Value(10.7, 1.7), true));
    }else{
      cout << "decide with or without ttbar " << endl;
      return; 
    }
    
  }else{
    cout << "Error ?? " << endl;
    return;
  }


  Fitter fit(data, shapes);
  
  fit.allowNegative(true);
  fit.setMinos(true);
  
  fit.doPseudoEx(3000); 
  
}
