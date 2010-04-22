//////////////////////////////////
// root -l make_Templates.C+
/////////////////////////////////

#include <TFile.h>
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
#include "MethodIITools/DataFile.hh"
#include "MethodIITools/Fitter.hh"

using namespace std;
float getNorm(int detector, int tag, int jetBin, const string& name,bool error);
void make_Templates(TString ntags = "1tag",  const string& higgsmass = "WH100")
{
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  /////////////////////////////////////
  //                                 //
  //  EVENT PROBABILITY ROOT FILES   // 
  //                                 //
  /////////////////////////////////////
 
  //trying to reproduce Peter Discriminant!
  TString address = "/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008_pass2/Event_Probs_Sep9_2008/MicroNtuples_LATEST/";
  TChain* WH_signal = new TChain("METree");
  char filesignal[500];
  unsigned int index;
  if(higgsmass == "WH100"){
    index = 0;
    cout << "*** Higgs 100 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH100GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH105"){
    index = 1;
    cout << "*** Higgs 105 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH105GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH110"){
    index = 2;
    cout << "*** Higgs 110 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH110GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH115"){
    index = 3;
    cout << "*** Higgs 115 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH115GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH120"){
    index = 4;
    cout << "*** Higgs 120 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH120GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH125"){
    index = 5;
    cout << "*** Higgs 125 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH125GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH130"){
    index = 6;
    cout << "*** Higgs 130 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH130GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH135"){
    index = 7;
    cout << "*** Higgs 135 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH135GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH140"){
    index = 8;
    sprintf(filesignal,address+"micro_WH140GeV.root"); 
    cout << "*** Higgs 140 GeV ***" << endl;
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH145"){
    index = 9;
    cout << "*** Higgs 145 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH145GeV.root"); 
    WH_signal->Add(filesignal);
  }else if(higgsmass == "WH150"){
    index = 10;
    cout << "*** Higgs 150 GeV ***" << endl;
    sprintf(filesignal,address+"micro_WH150GeV.root"); 
    WH_signal->Add(filesignal);
  }else{

    cout << "The higgs mass is out of range " << endl;
    return;
  }
  cout<<filesignal<<endl;
  
  TChain* bkg_tchannel = new TChain("METree"); 
  char filebkg_tchannel[500];
  sprintf(filebkg_tchannel,address+"micro_tchan.root"); 
  bkg_tchannel->Add(filebkg_tchannel);
  cout<<filebkg_tchannel<<endl;


  //Wbb  new TF(Ep,NNoutput)
  TChain* bkg_Wbottom = new TChain("METree"); 
  char filebkg_Wbottom[600];
  sprintf(filebkg_Wbottom,address+"*_bottom.root");
  bkg_Wbottom->Add(filebkg_Wbottom);
  cout<<filebkg_Wbottom<<endl;
  
  
  //schannel new TF(Ep,NNoutput)
  TChain* bkg_schannel = new TChain("METree");
  char filebkg_schannel[500];
  sprintf(filebkg_schannel,address+"micro_schan.root");    
  bkg_schannel->Add(filebkg_schannel);
  cout<<filebkg_schannel<<endl;

  //ttbar
  TChain* bkg_ttlj = new TChain("METree");
  char filebkg_ttlj[500];
  sprintf(filebkg_ttlj,address+"micro_tt_nondilepton.root");    
  bkg_ttlj->Add(filebkg_ttlj);
  cout<<filebkg_ttlj<<endl;
  //ttbar
  TChain* bkg_ttdil = new TChain("METree");
  char filebkg_ttdil[500];
  sprintf(filebkg_ttdil,address+"micro_tt_dilepton.root");    
  bkg_ttdil->Add(filebkg_ttdil);
  cout<<filebkg_ttdil<<endl;


  //WW
  TChain* bkg_WW = new TChain("METree");
  char filebkg_WW[500];
  sprintf(filebkg_WW,address+"micro_WW.root");    
  bkg_WW->Add(filebkg_WW);
  cout<<filebkg_WW<<endl;

  //WZ
  TChain* bkg_WZ = new TChain("METree");
  char filebkg_WZ[500];
  sprintf(filebkg_WZ,address+"micro_WZ.root");    
  bkg_WZ->Add(filebkg_WZ);
  cout<<filebkg_WZ<<endl;

  //ZZ
  TChain* bkg_ZZ = new TChain("METree");
  char filebkg_ZZ[500];
  sprintf(filebkg_ZZ,address+"micro_ZZ.root");    
  bkg_ZZ->Add(filebkg_ZZ);
  cout<<filebkg_ZZ<<endl;


  //Z+jets
  TChain* bkg_Zjets = new TChain("METree");
  char filebkg_Zjets[500];
  sprintf(filebkg_Zjets,address+"micro_Zjets.root");    
  bkg_Zjets->Add(filebkg_Zjets);
  cout<<filebkg_Zjets<<endl;

  //nonW
  TChain* bkg_nonW = new TChain("METree");
  char filebkg_nonW[500];
  sprintf(filebkg_nonW,address+"micro_taggable_nonW.root");    
  bkg_nonW->Add(filebkg_nonW);
  cout<<filebkg_nonW<<endl;


  //**************W+Charm****************************

  TChain* bkg_Wcharm = new TChain("METree");
  char filebkg_Wcharm[500];
  sprintf(filebkg_Wcharm,address+"*_charm.root");    
  bkg_Wcharm->Add(filebkg_Wcharm);
  cout<<filebkg_Wcharm<<endl;

  //************W+Light*********************** 
  TChain* bkg_Wlight = new TChain("METree");
  char filebkg_Wlight[500];
  sprintf(filebkg_Wlight,address+"*_light.root");    
  bkg_Wlight->Add(filebkg_Wlight);
  cout<<filebkg_Wlight<<endl;

  //data
  TChain* data = new TChain("METree");
  char file_data[500];
  sprintf(file_data,address+"micro_tagged_data.root");    
  data->Add(file_data);
  cout<<file_data<<endl;
  		         
  cout<<"Total Entries WH signal    ("    << higgsmass << " GeV) = " <<WH_signal->GetEntries() <<endl;
  cout<<"Total Entries Wbottom bkg   " <<bkg_Wbottom->GetEntries()   <<endl;
  cout<<"Total Entries ttdil bkg     " <<bkg_ttdil->GetEntries()     <<endl;
  cout<<"Total Entries ttlj bkg      " <<bkg_ttlj->GetEntries()      <<endl;
  cout<<"Total Entries schannel bkg  " <<bkg_schannel->GetEntries()  <<endl;
  cout<<"Total Entries tchannel bkg  " <<bkg_tchannel->GetEntries()  <<endl;
  cout<<"Total Entries ZZ bkg        " <<bkg_ZZ->GetEntries()        <<endl;
  cout<<"Total Entries Z+jets bkg    " <<bkg_Zjets->GetEntries()     <<endl;
  cout<<"Total Entries Wcharm bkg    " <<bkg_Wcharm->GetEntries()    <<endl;
  cout<<"Total Entries W+light bkg   " <<bkg_Wlight->GetEntries()    <<endl;
  cout<<"Total Entries non-W bkg     " <<bkg_nonW->GetEntries()      <<endl;
  cout<<"Total Entries data          " <<data->GetEntries()          <<endl;
  
   TString det[5][2] = {
    "(h.det == 1)", "CEM",
    "(h.det == 2)", "PHX",
    "(h.det == 3)", "CMUP",
    "(h.det == 4)", "CMX",
    "(h.det >= 5)", "LOOSE"
  };


  const unsigned numberofbins = 40;      
  TH1F* WH_signal_histo[5];
  TH1F* bkg_Wbottom_histo[5];
  TH1F* bkg_ttlj_histo[5];
  TH1F* bkg_ttdil_histo[5];
  TH1F* bkg_schan_histo[5];
  TH1F* bkg_tchan_histo[5];
  TH1F* bkg_zjets_histo[5];
  TH1F* bkg_Wcharm_histo[5];
  TH1F* bkg_Wlight_histo[5];
  TH1F* bkg_nonW_histo[5];
  TH1F* bkg_WW_histo[5];
  TH1F* bkg_WZ_histo[5];
  TH1F* bkg_ZZ_histo[5];
  TH1F* data_histo[5];

  TString nameWH[5];
  TString nameWbb[5];
  TString namettlj[5];
  TString namettdil[5];
  TString nameschan[5];
  TString nametchan[5];
  TString namezjets[5];
  TString nameWcharm[5];
  TString nameWlight[5];
  TString namenonW[5];
  TString nameWW[5];
  TString nameWZ[5];
  TString nameZZ[5];
  TString namedata[5];

  TFile *output;
  TFile *output_loose;
  //output_loose = new TFile( "test/central_2jet_"+ntags+"_2invfb_loose_"+higgsmass+"_ME.root", "recreate");
  //output       = new TFile( "test/central_2jet_"+ntags+"_2invfb_"+higgsmass+"_ME.root", "recreate");
  output_loose = new TFile( address+"templates/central_2jet_"+ntags+"_2invfb_loose_"+higgsmass+"_ME.root", "recreate");
  output       = new TFile( address+"templates/central_2jet_"+ntags+"_2invfb_"+higgsmass+"_ME.root", "recreate");

  for(int d = 0; d<5; d++){
    if(d==4) output_loose->cd();
    nameWH[d] =  higgsmass+""+det[d][1];
    WH_signal_histo[d]    = new TH1F(nameWH[d], nameWH[d], numberofbins, 0, 1); 
    WH_signal_histo[d]->SetLineWidth(2);
    WH_signal_histo[d]->SetLineColor(100);
    WH_signal_histo[d]->SetTitle("");
    WH_signal_histo[d]->GetXaxis()->SetTitleSize(0.045);
    WH_signal_histo[d]->GetYaxis()->SetTitleSize(0.045);
    WH_signal_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    WH_signal_histo[d]->Sumw2();

    nameWbb[d] =  "wbb"+det[d][1];
    bkg_Wbottom_histo[d]  = new TH1F(nameWbb[d], nameWbb[d], numberofbins, 0, 1);
    bkg_Wbottom_histo[d]->SetLineWidth(2);
    bkg_Wbottom_histo[d]->SetLineColor(209);
    bkg_Wbottom_histo[d]->SetTitle("");
    bkg_Wbottom_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_Wbottom_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_Wbottom_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_Wbottom_histo[d]->Sumw2();

    namettlj[d] =  "ttbarlj"+det[d][1];
    bkg_ttlj_histo[d]      = new TH1F(namettlj[d], namettlj[d], numberofbins, 0, 1);
    bkg_ttlj_histo[d]->SetLineWidth(2);
    bkg_ttlj_histo[d]->SetLineColor(12);
    bkg_ttlj_histo[d]->SetTitle("");
    bkg_ttlj_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_ttlj_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_ttlj_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_ttlj_histo[d]->Sumw2();

    namettdil[d] =  "ttbardil"+det[d][1];
    bkg_ttdil_histo[d]      = new TH1F(namettdil[d], namettdil[d], numberofbins, 0, 1);
    bkg_ttdil_histo[d]->SetLineWidth(2);
    bkg_ttdil_histo[d]->SetLineColor(12);
    bkg_ttdil_histo[d]->SetTitle("");
    bkg_ttdil_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_ttdil_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_ttdil_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_ttdil_histo[d]->Sumw2();

    nameschan[d] =  "schan"+det[d][1];
    bkg_schan_histo[d] = new TH1F(nameschan[d], nameschan[d], numberofbins, 0, 1);
    bkg_schan_histo[d]->SetLineWidth(2);
    bkg_schan_histo[d]->SetLineColor(102);
    bkg_schan_histo[d]->SetTitle("");
    bkg_schan_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_schan_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_schan_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_schan_histo[d]->Sumw2();

    nametchan[d] =  "tchan"+det[d][1];
    bkg_tchan_histo[d] = new TH1F(nametchan[d], nametchan[d], numberofbins, 0, 1);
    bkg_tchan_histo[d]->SetLineWidth(2);
    bkg_tchan_histo[d]->SetLineColor(96);
    bkg_tchan_histo[d]->SetTitle("");
    bkg_tchan_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_tchan_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_tchan_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_tchan_histo[d]->Sumw2();

    namezjets[d] =  "zjets"+det[d][1];
    bkg_zjets_histo[d] = new TH1F(namezjets[d], namezjets[d], numberofbins, 0, 1);
    bkg_zjets_histo[d]->SetLineWidth(2);
    bkg_zjets_histo[d]->SetLineColor(60);
    bkg_zjets_histo[d]->SetTitle("");
    bkg_zjets_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_zjets_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_zjets_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_zjets_histo[d]->Sumw2();

    nameWcharm[d] =  "wcc"+det[d][1];
    bkg_Wcharm_histo[d] = new TH1F(nameWcharm[d], nameWcharm[d], numberofbins, 0, 1);
    bkg_Wcharm_histo[d]->SetLineWidth(2);
    bkg_Wcharm_histo[d]->SetLineColor(8);
    bkg_Wcharm_histo[d]->SetTitle("");
    bkg_Wcharm_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_Wcharm_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_Wcharm_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_Wcharm_histo[d]->Sumw2();

    nameWlight[d] =  "mistag"+det[d][1];
    bkg_Wlight_histo[d] = new TH1F(nameWlight[d], nameWlight[d], numberofbins, 0, 1);
    bkg_Wlight_histo[d]->SetLineWidth(2);
    bkg_Wlight_histo[d]->SetLineColor(212);
    bkg_Wlight_histo[d]->SetTitle("");
    bkg_Wlight_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_Wlight_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_Wlight_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_Wlight_histo[d]->Sumw2();

    namenonW[d] =  "nonW"+det[d][1];
    bkg_nonW_histo[d] = new TH1F(namenonW[d], namenonW[d], numberofbins, 0, 1);
    bkg_nonW_histo[d]->SetLineWidth(2);
    bkg_nonW_histo[d]->SetLineColor(38);
    bkg_nonW_histo[d]->SetTitle("");
    bkg_nonW_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_nonW_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_nonW_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_nonW_histo[d]->Sumw2();

    nameWW[d] =  "ww"+det[d][1];
    bkg_WW_histo[d] = new TH1F(nameWW[d], nameWW[d], numberofbins, 0, 1);
    bkg_WW_histo[d]->SetLineWidth(2);
    bkg_WW_histo[d]->SetLineColor(41);
    bkg_WW_histo[d]->SetTitle("");
    bkg_WW_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_WW_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_WW_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_WW_histo[d]->Sumw2();

    nameWZ[d] =  "wz"+det[d][1];
    bkg_WZ_histo[d] = new TH1F(nameWZ[d], nameWZ[d], numberofbins, 0, 1);
    bkg_WZ_histo[d]->SetLineWidth(2);
    bkg_WZ_histo[d]->SetLineColor(41);
    bkg_WZ_histo[d]->SetTitle("");
    bkg_WZ_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_WZ_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_WZ_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_WZ_histo[d]->Sumw2();

    nameZZ[d] =  "zz"+det[d][1];
    bkg_ZZ_histo[d] = new TH1F(nameZZ[d], nameZZ[d], numberofbins, 0, 1);
    bkg_ZZ_histo[d]->SetLineWidth(2);
    bkg_ZZ_histo[d]->SetLineColor(41);
    bkg_ZZ_histo[d]->SetTitle("");
    bkg_ZZ_histo[d]->GetXaxis()->SetTitleSize(0.045);
    bkg_ZZ_histo[d]->GetYaxis()->SetTitleSize(0.045);
    bkg_ZZ_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    bkg_ZZ_histo[d]->Sumw2();

    namedata[d] =  "data"+det[d][1];
    data_histo[d] = new TH1F(namedata[d], namedata[d], numberofbins, 0, 1);
    data_histo[d]->SetLineWidth(2);
    data_histo[d]->SetLineColor(10);
    data_histo[d]->SetTitle("");
    data_histo[d]->GetXaxis()->SetTitleSize(0.045);
    data_histo[d]->GetYaxis()->SetTitleSize(0.045);
    data_histo[d]->GetXaxis()->SetTitle("Event Probability Discriminant");
    data_histo[d]->Sumw2();
  }

  //This microNtuples have 0 tags also, I am not going to include them for this study!!
  //Templates single tag channel
  if( ntags == "1tag" ){   
 
    //loop over detectors
    for(int d = 0; d <5; d++ ){
      if(d!=4){
	cout <<"Central Leptons ... " << endl;
	output->cd();
	std::ostringstream str1tag;
	TString qcdVeto = d==1 ? "&& h.met>25 && h.passQCD)" : "&& h.passQCD )";
	str1tag << "(epd1tagWH[" << index << "])>>"+nameWH[d];

	WH_signal->Draw(str1tag.str().c_str(), "(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))" );
	WH_signal_histo[d]->Scale(getNorm(d,1,2, higgsmass,false)/ WH_signal_histo[d]->Integral(0,numberofbins+1));
	WH_signal_histo[d]->Write();
	
	std::ostringstream str1tag_Wbottom;
	str1tag_Wbottom<< "(epd1tagWH[" << index << "])>>"+nameWbb[d];
	bkg_Wbottom->Draw(str1tag_Wbottom.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))" ); 
	bkg_Wbottom_histo[d]->Scale(getNorm(d,1,2,"Wbb",false)/ bkg_Wbottom_histo[d]->Integral(0,numberofbins+1));
	bkg_Wbottom_histo[d]->Write();
	
	std::ostringstream str1tag_ttlj;
	str1tag_ttlj << "(epd1tagWH[" << index << "])>>"+namettlj[d];
	bkg_ttlj->Draw(str1tag_ttlj.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_ttlj_histo[d]->Scale(getNorm(d,1,2,"TopLJ",false)/ bkg_ttlj_histo[d]->Integral(0,numberofbins+1));
	bkg_ttlj_histo[d]->Write();
	
	std::ostringstream str1tag_ttdil;
	str1tag_ttdil << "(epd1tagWH[" << index << "])>>"+namettdil[d];
	bkg_ttdil->Draw(str1tag_ttdil.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_ttdil_histo[d]->Scale(getNorm(d,1,2,"TopDil",false)/ bkg_ttdil_histo[d]->Integral(0,numberofbins+1));
	bkg_ttdil_histo[d]->Write();
	
	std::ostringstream str1tag_tchan;
	str1tag_tchan << "(epd1tagWH[" << index << "])>>"+nametchan[d];
	bkg_tchannel->Draw(str1tag_tchan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_tchan_histo[d]->Scale(getNorm(d,1,2,"STopT",false)/ bkg_tchan_histo[d]->Integral(0,numberofbins+1));
	bkg_tchan_histo[d]->Write();
	
	std::ostringstream str1tag_schan;
	str1tag_schan << "(epd1tagWH[" << index << "])>>"+nameschan[d];
	bkg_schannel->Draw(str1tag_schan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_schan_histo[d]->Scale(getNorm(d,1,2,"STopS",false)/ bkg_schan_histo[d]->Integral(0,numberofbins+1));
	bkg_schan_histo[d]->Write();
	
	std::ostringstream str1tag_zjets;
	str1tag_zjets << "(epd1tagWH[" << index << "])>>"+namezjets[d];
	bkg_Zjets->Draw(str1tag_zjets.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_zjets_histo[d]->Scale(getNorm(d,1,2,"Z+jets",false)/ bkg_zjets_histo[d]->Integral(0,numberofbins+1));
	bkg_zjets_histo[d]->Write();
	
	std::ostringstream str1tag_Wcharm;
	str1tag_Wcharm << "(epd1tagWH[" << index << "])>>"+nameWcharm[d];
	bkg_Wcharm->Draw(str1tag_Wcharm.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_Wcharm_histo[d]->Scale(getNorm(d,1,2,"Wcc_Wc",false)/ bkg_Wcharm_histo[d]->Integral(0,numberofbins+1));
	bkg_Wcharm_histo[d]->Write();
	
	std::ostringstream str1tag_Wlight;
	str1tag_Wlight << "(epd1tagWH[" << index << "])>>"+nameWlight[d];
	bkg_Wlight->Draw(str1tag_Wlight.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_Wlight_histo[d]->Scale(getNorm(d,1,2,"Mistags",false)/ bkg_Wlight_histo[d]->Integral(0,numberofbins+1));
	bkg_Wlight_histo[d]->Write();
	
	std::ostringstream str1tag_nonW;
	str1tag_nonW << "(epd1tagWH[" << index << "])>>"+namenonW[d];
	bkg_nonW->Draw(str1tag_nonW.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_nonW_histo[d]->Scale(getNorm(d,1,2,"Non-W",false)/ bkg_nonW_histo[d]->Integral(0,numberofbins+1));
	bkg_nonW_histo[d]->Write();
	
	std::ostringstream str1tag_WW;
	str1tag_WW << "(epd1tagWH[" << index << "])>>"+nameWW[d];
	bkg_WW->Draw(str1tag_WW.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_WW_histo[d]->Scale(getNorm(d,1,2,"WW",false)/ bkg_WW_histo[d]->Integral(0,numberofbins+1));
	bkg_WW_histo[d]->Write();
	
	std::ostringstream str1tag_WZ;
	str1tag_WZ << "(epd1tagWH[" << index << "])>>"+nameWZ[d];
	bkg_WZ->Draw(str1tag_WZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_WZ_histo[d]->Scale(getNorm(d,1,2,"WZ",false)/ bkg_WZ_histo[d]->Integral(0,numberofbins+1));
	bkg_WZ_histo[d]->Write();
	
	
	std::ostringstream str1tag_ZZ;
	str1tag_ZZ << "(epd1tagWH[" << index << "])>>"+nameZZ[d];
	bkg_ZZ->Draw(str1tag_ZZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*h.triggerTO))"); 
	bkg_ZZ_histo[d]->Scale(getNorm(d,1,2,"ZZ",false)/ bkg_ZZ_histo[d]->Integral(0,numberofbins+1));
	bkg_ZZ_histo[d]->Write();
	
	std::ostringstream str1tag_data;
	str1tag_data << "(epd1tagWH[" << index << "])>>"+namedata[d];
	data->Draw(str1tag_data.str().c_str(),"(("+det[d][0]+""+qcdVeto+"&& h.ntag==1)"); 
	data_histo[d]->Write();
	
      }//end central lepton loop
      else{
	cout <<"Loose Muons ... " << endl;
	output_loose->cd();
	TString qcdVeto = d==1 ? "&& h.met>25 && h.passQCD )" : "&& h.passQCD)";

	std::ostringstream str1tag;
	str1tag << "(epd1tagWH[" << index << "])>>"+nameWH[d];
	WH_signal->Draw(str1tag.str().c_str(), "(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))" );
	WH_signal_histo[d]->Scale(getNorm(d,1,2, higgsmass,false)/ WH_signal_histo[d]->Integral(0,numberofbins+1));
	WH_signal_histo[d]->Write();
	
	std::ostringstream str1tag_Wbottom;
	str1tag_Wbottom<< "(epd1tagWH[" << index << "])>>"+nameWbb[d];
	bkg_Wbottom->Draw(str1tag_Wbottom.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))" ); 
	bkg_Wbottom_histo[d]->Scale(getNorm(d,1,2,"Wbb",false)/ bkg_Wbottom_histo[d]->Integral(0,numberofbins+1));
	bkg_Wbottom_histo[d]->Write();
	
	std::ostringstream str1tag_ttlj;
	str1tag_ttlj << "(epd1tagWH[" << index << "])>>"+namettlj[d];
	bkg_ttlj->Draw(str1tag_ttlj.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ttlj_histo[d]->Scale(getNorm(d,1,2,"TopLJ",false)/ bkg_ttlj_histo[d]->Integral(0,numberofbins+1));
	bkg_ttlj_histo[d]->Write();
	
	std::ostringstream str1tag_ttdil;
	str1tag_ttdil << "(epd1tagWH[" << index << "])>>"+namettdil[d];
	bkg_ttdil->Draw(str1tag_ttdil.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ttdil_histo[d]->Scale(getNorm(d,1,2,"TopDil",false)/ bkg_ttdil_histo[d]->Integral(0,numberofbins+1));
	bkg_ttdil_histo[d]->Write();
	
	std::ostringstream str1tag_tchan;
	str1tag_tchan << "(epd1tagWH[" << index << "])>>"+nametchan[d];
	bkg_tchannel->Draw(str1tag_tchan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_tchan_histo[d]->Scale(getNorm(d,1,2,"STopT",false)/ bkg_tchan_histo[d]->Integral(0,numberofbins+1));
	bkg_tchan_histo[d]->Write();
	
	std::ostringstream str1tag_schan;
	str1tag_schan << "(epd1tagWH[" << index << "])>>"+nameschan[d];
	bkg_schannel->Draw(str1tag_schan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_schan_histo[d]->Scale(getNorm(d,1,2,"STopS",false)/ bkg_schan_histo[d]->Integral(0,numberofbins+1));
	bkg_schan_histo[d]->Write();
	
	std::ostringstream str1tag_zjets;
	str1tag_zjets << "(epd1tagWH[" << index << "])>>"+namezjets[d];
	bkg_Zjets->Draw(str1tag_zjets.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_zjets_histo[d]->Scale(getNorm(d,1,2,"Z+jets",false)/ bkg_zjets_histo[d]->Integral(0,numberofbins+1));
	bkg_zjets_histo[d]->Write();
	
	std::ostringstream str1tag_Wcharm;
	str1tag_Wcharm << "(epd1tagWH[" << index << "])>>"+nameWcharm[d];
	bkg_Wcharm->Draw(str1tag_Wcharm.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_Wcharm_histo[d]->Scale(getNorm(d,1,2,"Wcc_Wc",false)/ bkg_Wcharm_histo[d]->Integral(0,numberofbins+1));
	bkg_Wcharm_histo[d]->Write();
	
	std::ostringstream str1tag_Wlight;
	str1tag_Wlight << "(epd1tagWH[" << index << "])>>"+nameWlight[d];
	bkg_Wlight->Draw(str1tag_Wlight.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_Wlight_histo[d]->Scale(getNorm(d,1,2,"Mistags",false)/ bkg_Wlight_histo[d]->Integral(0,numberofbins+1));
	bkg_Wlight_histo[d]->Write();
	
	std::ostringstream str1tag_nonW;
	str1tag_nonW << "(epd1tagWH[" << index << "])>>"+namenonW[d];
	bkg_nonW->Draw(str1tag_nonW.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_nonW_histo[d]->Scale(getNorm(d,1,2,"Non-W",false)/ bkg_nonW_histo[d]->Integral(0,numberofbins+1));
	bkg_nonW_histo[d]->Write();
	
	std::ostringstream str1tag_WW;
	str1tag_WW << "(epd1tagWH[" << index << "])>>"+nameWW[d];
	bkg_WW->Draw(str1tag_WW.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_WW_histo[d]->Scale(getNorm(d,1,2,"WW",false)/ bkg_WW_histo[d]->Integral(0,numberofbins+1));
	bkg_WW_histo[d]->Write();
	
	std::ostringstream str1tag_WZ;
	str1tag_WZ << "(epd1tagWH[" << index << "])>>"+nameWZ[d];
	bkg_WZ->Draw(str1tag_WZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_WZ_histo[d]->Scale(getNorm(d,1,2,"WZ",false)/ bkg_WZ_histo[d]->Integral(0,numberofbins+1));
	bkg_WZ_histo[d]->Write();
	
	
	std::ostringstream str1tag_ZZ;
	str1tag_ZZ << "(epd1tagWH[" << index << "])>>"+nameZZ[d];
	bkg_ZZ->Draw(str1tag_ZZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb1*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ZZ_histo[d]->Scale(getNorm(d,1,2,"ZZ",false)/ bkg_ZZ_histo[d]->Integral(0,numberofbins+1));
	bkg_ZZ_histo[d]->Write();
	
	std::ostringstream str1tag_data;
	str1tag_data << "(epd1tagWH[" << index << "])>>"+namedata[d];
	data->Draw(str1tag_data.str().c_str(),"(("+det[d][0]+""+qcdVeto+"&& h.ntag==1)"); 
	data_histo[d]->Write();


      }//end loose muons loop
      


    }


  }else if( ntags == "2tag" ){    
    for(int d = 0; d <5; d++ ){
      if(d!=4) {
	cout <<"Central Leptons ... " << endl;

	output->cd();
	TString qcdVeto = d==1 ? "&&h.met>25 && h.passQCD )" : ")";
	//TString qcdVeto = d==1 ? "&&h.met>25 && h.passQCD && h.ntag==2)" : " && h.ntag==2  )";


	std::ostringstream str2tag;
	str2tag << "(epd2tagWH[" << index << "])>>"+nameWH[d];
	WH_signal->Draw(str2tag.str().c_str(), "(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))" );
	WH_signal_histo[d]->Scale(getNorm(d,2,2,higgsmass,false)/ WH_signal_histo[d]->Integral(0,numberofbins+1));
	WH_signal_histo[d]->Write();
	
	std::ostringstream str2tag_Wbottom;
	str2tag_Wbottom<< "(epd2tagWH[" << index << "])>>"+nameWbb[d];
	bkg_Wbottom->Draw(str2tag_Wbottom.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))" ); 
	bkg_Wbottom_histo[d]->Scale(getNorm(d,2,2,"Wbb",false)/ bkg_Wbottom_histo[d]->Integral(0,numberofbins+1));
	bkg_Wbottom_histo[d]->Write();
	
	std::ostringstream str2tag_ttlj;
	str2tag_ttlj << "(epd2tagWH[" << index << "])>>"+namettlj[d];
	bkg_ttlj->Draw(str2tag_ttlj.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_ttlj_histo[d]->Scale(getNorm(d,2,2,"TopLJ",false)/ bkg_ttlj_histo[d]->Integral(0,numberofbins+1));
	bkg_ttlj_histo[d]->Write();
	
	std::ostringstream str2tag_ttdil;
	str2tag_ttdil << "(epd2tagWH[" << index << "])>>"+namettdil[d];
	bkg_ttdil->Draw(str2tag_ttdil.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_ttdil_histo[d]->Scale(getNorm(d,2,2,"TopDil",false)/ bkg_ttdil_histo[d]->Integral(0,numberofbins+1));
	bkg_ttdil_histo[d]->Write();
	
	std::ostringstream str2tag_tchan;
	str2tag_tchan << "(epd2tagWH[" << index << "])>>"+nametchan[d];
	bkg_tchannel->Draw(str2tag_tchan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_tchan_histo[d]->Scale(getNorm(d,2,2,"STopT",false)/ bkg_tchan_histo[d]->Integral(0,numberofbins+1));
	bkg_tchan_histo[d]->Write();
	
	std::ostringstream str2tag_schan;
	str2tag_schan << "(epd2tagWH[" << index << "])>>"+nameschan[d];
	bkg_schannel->Draw(str2tag_schan.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_schan_histo[d]->Scale(getNorm(d,2,2,"STopS",false)/ bkg_schan_histo[d]->Integral(0,numberofbins+1));
	bkg_schan_histo[d]->Write();
	
	std::ostringstream str2tag_zjets;
	str2tag_zjets << "(epd2tagWH[" << index << "])>>"+namezjets[d];
	bkg_Zjets->Draw(str2tag_zjets.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_zjets_histo[d]->Scale(getNorm(d,2,2,"Z+jets",false)/ bkg_zjets_histo[d]->Integral(0,numberofbins+1));
	bkg_zjets_histo[d]->Write();
	
	std::ostringstream str2tag_Wcharm;
	str2tag_Wcharm << "(epd2tagWH[" << index << "])>>"+nameWcharm[d];
	bkg_Wcharm->Draw(str2tag_Wcharm.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_Wcharm_histo[d]->Scale(getNorm(d,2,2,"Wcc_Wc",false)/ bkg_Wcharm_histo[d]->Integral(0,numberofbins+1));
	bkg_Wcharm_histo[d]->Write();
	
	std::ostringstream str2tag_Wlight;
	str2tag_Wlight << "(epd2tagWH[" << index << "])>>"+nameWlight[d];
	bkg_Wlight->Draw(str2tag_Wlight.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_Wlight_histo[d]->Scale(getNorm(d,2,2,"Mistags",false)/ bkg_Wlight_histo[d]->Integral(0,numberofbins+1));
	bkg_Wlight_histo[d]->Write();
	
	std::ostringstream str2tag_nonW;
	str2tag_nonW << "(epd2tagWH[" << index << "])>>"+namenonW[d];
	bkg_nonW->Draw(str2tag_nonW.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_nonW_histo[d]->Scale(getNorm(d,2,2,"Non-W",false)/ bkg_nonW_histo[d]->Integral(0,numberofbins+1));
	bkg_nonW_histo[d]->Write();
	
	std::ostringstream str2tag_WW;
	str2tag_WW << "(epd2tagWH[" << index << "])>>"+nameWW[d];
	bkg_WW->Draw(str2tag_WW.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_WW_histo[d]->Scale(getNorm(d,2,2,"WW",false)/ bkg_WW_histo[d]->Integral(0,numberofbins+1));
	bkg_WW_histo[d]->Write();
	
	std::ostringstream str2tag_WZ;
	str2tag_WZ << "(epd2tagWH[" << index << "])>>"+nameWZ[d];
	bkg_WZ->Draw(str2tag_WZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_WZ_histo[d]->Scale(getNorm(d,2,2,"WZ",false)/ bkg_WZ_histo[d]->Integral(0,numberofbins+1));
	bkg_WZ_histo[d]->Write();
	
	
	std::ostringstream str2tag_ZZ;
	str2tag_ZZ << "(epd2tagWH[" << index << "])>>"+nameZZ[d];
	bkg_ZZ->Draw(str2tag_ZZ.str().c_str(),"(("+det[d][0]+"&& h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*h.triggerTO))"); 
	bkg_ZZ_histo[d]->Scale(getNorm(d,2,2,"ZZ",false)/ bkg_ZZ_histo[d]->Integral(0,numberofbins+1));
	bkg_ZZ_histo[d]->Write();
	
	std::ostringstream str2tag_data;
	str2tag_data << "(epd2tagWH[" << index << "])>>"+namedata[d];
	data->Draw(str2tag_data.str().c_str(),"(("+det[d][0]+""+qcdVeto+"&&h.ntag==2)"); 
	data_histo[d]->Write();
	
      }
      else{
	output_loose->cd();
	cout <<"Loose Muons ... " << endl;
	TString qcdVeto = d==1 ? "&& h.met>25 && h.passQCD)" : " )";
	//TString qcdVeto = d==1 ? "&& h.met>25 && h.passQCD && h.ntag==2)" : " && h.ntag==2)";

	std::ostringstream str2tag;
	str2tag << "(epd2tagWH[" << index << "])>>"+nameWH[d];
	WH_signal->Draw(str2tag.str().c_str(), "(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))" );
	WH_signal_histo[d]->Scale(getNorm(d,2,2,higgsmass,false)/ WH_signal_histo[d]->Integral(0,numberofbins+1));
	WH_signal_histo[d]->Write();
	
	std::ostringstream str2tag_Wbottom;
	str2tag_Wbottom<< "(epd2tagWH[" << index << "])>>"+nameWbb[d];
	bkg_Wbottom->Draw(str2tag_Wbottom.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))" ); 
	bkg_Wbottom_histo[d]->Scale(getNorm(d,2,2,"Wbb",false)/ bkg_Wbottom_histo[d]->Integral(0,numberofbins+1));
	bkg_Wbottom_histo[d]->Write();
	
	std::ostringstream str2tag_ttlj;
	str2tag_ttlj << "(epd2tagWH[" << index << "])>>"+namettlj[d];
	bkg_ttlj->Draw(str2tag_ttlj.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ttlj_histo[d]->Scale(getNorm(d,2,2,"TopLJ",false)/ bkg_ttlj_histo[d]->Integral(0,numberofbins+1));
	bkg_ttlj_histo[d]->Write();
	
	std::ostringstream str2tag_ttdil;
	str2tag_ttdil << "(epd2tagWH[" << index << "])>>"+namettdil[d];
	bkg_ttdil->Draw(str2tag_ttdil.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ttdil_histo[d]->Scale(getNorm(d,2,2,"TopDil",false)/ bkg_ttdil_histo[d]->Integral(0,numberofbins+1));
	bkg_ttdil_histo[d]->Write();
	
	std::ostringstream str2tag_tchan;
	str2tag_tchan << "(epd2tagWH[" << index << "])>>"+nametchan[d];
	bkg_tchannel->Draw(str2tag_tchan.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_tchan_histo[d]->Scale(getNorm(d,2,2,"STopT",false)/ bkg_tchan_histo[d]->Integral(0,numberofbins+1));
	bkg_tchan_histo[d]->Write();
	
	std::ostringstream str2tag_schan;
	str2tag_schan << "(epd2tagWH[" << index << "])>>"+nameschan[d];
	bkg_schannel->Draw(str2tag_schan.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_schan_histo[d]->Scale(getNorm(d,2,2,"STopS",false)/ bkg_schan_histo[d]->Integral(0,numberofbins+1));
	bkg_schan_histo[d]->Write();
	
	std::ostringstream str2tag_zjets;
	str2tag_zjets << "(epd2tagWH[" << index << "])>>"+namezjets[d];
	bkg_Zjets->Draw(str2tag_zjets.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_zjets_histo[d]->Scale(getNorm(d,2,2,"Z+jets",false)/ bkg_zjets_histo[d]->Integral(0,numberofbins+1));
	bkg_zjets_histo[d]->Write();
	
	std::ostringstream str2tag_Wcharm;
	str2tag_Wcharm << "(epd2tagWH[" << index << "])>>"+nameWcharm[d];
	bkg_Wcharm->Draw(str2tag_Wcharm.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_Wcharm_histo[d]->Scale(getNorm(d,2,2,"Wcc_Wc",false)/ bkg_Wcharm_histo[d]->Integral(0,numberofbins+1));
	bkg_Wcharm_histo[d]->Write();
	
	std::ostringstream str2tag_Wlight;
	str2tag_Wlight << "(epd2tagWH[" << index << "])>>"+nameWlight[d];
	bkg_Wlight->Draw(str2tag_Wlight.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_Wlight_histo[d]->Scale(getNorm(d,2,2,"Mistags",false)/ bkg_Wlight_histo[d]->Integral(0,numberofbins+1));
	bkg_Wlight_histo[d]->Write();
	
	std::ostringstream str2tag_nonW;
	str2tag_nonW << "(epd2tagWH[" << index << "])>>"+namenonW[d];
	bkg_nonW->Draw(str2tag_nonW.str().c_str(),"(("+det[d][0]+""+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_nonW_histo[d]->Scale(getNorm(d,2,2,"Non-W",false)/ bkg_nonW_histo[d]->Integral(0,numberofbins+1));
	bkg_nonW_histo[d]->Write();
	
	std::ostringstream str2tag_WW;
	str2tag_WW << "(epd2tagWH[" << index << "])>>"+nameWW[d];
	bkg_WW->Draw(str2tag_WW.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_WW_histo[d]->Scale(getNorm(d,2,2,"WW",false)/ bkg_WW_histo[d]->Integral(0,numberofbins+1));
	bkg_WW_histo[d]->Write();
	
	std::ostringstream str2tag_WZ;
	str2tag_WZ << "(epd2tagWH[" << index << "])>>"+nameWZ[d];
	bkg_WZ->Draw(str2tag_WZ.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_WZ_histo[d]->Scale(getNorm(d,2,2,"WZ",false)/ bkg_WZ_histo[d]->Integral(0,numberofbins+1));
	bkg_WZ_histo[d]->Write();
	
	
	std::ostringstream str2tag_ZZ;
	str2tag_ZZ << "(epd2tagWH[" << index << "])>>"+nameZZ[d];
	bkg_ZZ->Draw(str2tag_ZZ.str().c_str(),"(("+det[d][0]+" && h.ntag>0"+qcdVeto+"*(h.tagProb2*h.wgt*getMet2JetTurnOn_v2()))"); 
	bkg_ZZ_histo[d]->Scale(getNorm(d,2,2,"ZZ",false)/ bkg_ZZ_histo[d]->Integral(0,numberofbins+1));
	bkg_ZZ_histo[d]->Write();
	
	std::ostringstream str2tag_data;
	str2tag_data << "(epd2tagWH[" << index << "])>>"+namedata[d];
	data->Draw(str2tag_data.str().c_str(),"(("+det[d][0]+""+qcdVeto+"&&h.ntag==2)"); 
	data_histo[d]->Write();

      }//else
    }


  }
  output->Close();
  output_loose->Close();
  
 
}

float getNorm(int detector, int tag, int jetBin, const string& name,bool error)
{

  const string jetTypes[5] = {"1jet", "2jets", "3jets", "4jets",
                              "5jets"};
  
  const string det[6] = {"CEM Electrons", "PHX Electrons", "CMUP Muons", "CMX Muons","LOOSE Muons","All detectors"};
  
  string tmp_name=name;
  if(name=="Wcc_Wc") tmp_name="Wcc/Wc";

  string myname = det[detector] + " " + "Tagged " + tmp_name +" "+ jetTypes[jetBin-1];
  if (error) myname=myname+" error";
  
  cout<<"Getting " <<myname<<endl;
  
  string file_1;
  string file_gr2;
  if(name=="WH100" || name=="WH105" || name=="WH110"
                   || name=="WH115" || name=="WH120" 
                   || name=="WH125" || name=="WH130" 
                   || name=="WH135" || name=="WH140" 
                   || name=="WH145" || name=="WH150"){

    file_1  ="../Plots/method2/ICHEP_singletag/method2Table_1tag.df";
    file_gr2="../Plots/method2/ICHEP_doubletag/method2Table_gr2tag.df";

  }else{
    file_1  ="../Plots/method2/ICHEP_singletag_noWH/method2Table_1tag.df";
    file_gr2="../Plots/method2/ICHEP_doubletag_noWH/method2Table_gr2tag.df";
  }
    
  InputDataFile myData_1(file_1);
  InputDataFile myData_gr2(file_gr2);
  

  float norm;
  if(tag==1){
    norm = myData_1.readFloat(myname);
    //cout << file_1 << endl;
  }

  if(tag==2){
    norm = myData_gr2.readFloat(myname);
    // cout << file_gr2 << endl;
  }
  if (norm == 0)
    norm = std::numeric_limits<float>::min();
 
  return norm;
}
