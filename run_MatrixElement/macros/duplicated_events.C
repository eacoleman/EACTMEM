#include <TChain.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1F.h>
#include <iostream>
using namespace std;

void duplicated_events(){
  //compare tnt with mnt
  //singletopntuple
  TChain* snt_tchannel = new TChain("TopTree");
  char filesingle[500];
  sprintf(filesingle,"/cdf/disk01/g6/caf_ICHEP2008/*_47_*.root"); 
  snt_tchannel->Add(filesingle);
  cout<< filesingle << endl;
  cout <<"Number of total entries SingletopNtuple " << snt_tchannel->GetEntries() <<  endl;

  snt_tchannel->Draw("evt.eventNumber","m_nJets==2&&evt.runNumber==198423 &&evt.eventNumber ==273");
  
 
 //tinyntuples
  TChain* tnt_tchannel = new TChain("jet2");
  char file[500];
  sprintf(file,"/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008/tnt_tchan/*.root"); 
  tnt_tchannel->Add(file);
  cout<< file << endl;
  cout <<"Number of total entries " << tnt_tchannel->GetEntries() <<  endl;


  //microntuples
  TChain* mnt_tchannel = new TChain("METree");
  char filemicro[500];
  sprintf(filemicro,"/cdf/disk01/g6/caf_ICHEP2008/Event_Probs/tchan/*.root"); 
  mnt_tchannel->Add(filemicro);
  cout<< filemicro << endl;
  cout <<"Number of total entries in the microNtuple " << mnt_tchannel->GetEntries() <<  endl;

  /*
  TCanvas  *c_eventnumber = new TCanvas ("c_eventnumber", "c_eventnumber", 1000, 800);
  c_eventnumber->cd(1);

  TH1F *tinyntuple_histo = new TH1F("tinyntuple_histo", "tinyntuple_histo", 100, 0, 4000);
  tinyntuple_histo->SetLineColor(2);
  TH1F *microntuple_histo = new TH1F("microntuple_histo", "microntuple_histo", 100, 0, 4000);
  microntuple_histo->SetLineStyle(2);

  tnt_tchannel->Draw("h.event >> tinyntuple_histo","h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)");
  mnt_tchannel->Draw("h.event>>microntuple_histo","h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)", "same");
  
  TLegend *legend = new TLegend(0.5, 0.5, 0.8, 0.8);
  legend->SetFillColor(10); 
  legend->AddEntry(tinyntuple_histo,  "tinyNtuple","l");
  legend->AddEntry(microntuple_histo, "microNtuple", "l");
  legend->Draw("same");


  //same variables for the duplicated events
  TCanvas *c_variables = new TCanvas ("c_variables", "c_variables", 1000, 800);
  c_variables->Divide(2,2);
  c_variables->cd(1);
  TH1F *tinyntuple_histo_eta = new TH1F("tinyntuple_histo_eta", "tinyntuple_histo_eta", 100, -2.5, 2.5);
  tinyntuple_histo_eta->SetLineColor(2);
  TH1F *microntuple_histo_eta = new TH1F("microntuple_histo_eta", "microntuple_histo_eta", 100, -2.5, 2.5);
  microntuple_histo_eta->SetLineStyle(2);

  tnt_tchannel->Draw("jets[0].lv.l5.Eta() >> tinyntuple_histo_eta","h.run==198423 &&h.event ==273 && h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)");
  //mnt_tchannel->Draw("jets[0].lv.l5.Eta()>>microntuple_histo_eta","h.run==198423 &&h.event ==273 &&h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)", "same");

  c_variables->cd(2);
  TH1F *tinyntuple_histo_E = new TH1F("tinyntuple_histo_E", "tinyntuple_histo_E", 100, 0, 200);
  tinyntuple_histo_E->SetLineColor(2);
  TH1F *microntuple_histo_E = new TH1F("microntuple_histo_E", "microntuple_histo_E", 100, 0, 200);
  microntuple_histo_E->SetLineStyle(2);

  tnt_tchannel->Draw("jets[0].lv.l5.E() >> tinyntuple_histo_E","h.run==198423 &&h.event ==273 && h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)");
  //mnt_tchannel->Draw("jets[0].lv.l5.E()>>microntuple_histo_E","h.run==198423 &&h.event ==273 &&h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)", "same");


  c_variables->cd(3);
  TH1F *tinyntuple_histo_Ht = new TH1F("tinyntuple_histo_Ht", "tinyntuple_histo_Ht", 100, 0, 400);
  tinyntuple_histo_Ht->SetLineColor(2);
  TH1F *microntuple_histo_Ht = new TH1F("microntuple_histo_Ht", "microntuple_histo_Ht", 100, 0, 400);
  microntuple_histo_Ht->SetLineStyle(2);

  tnt_tchannel->Draw("h.ht >> tinyntuple_histo_Ht","h.run==198423 &&h.event ==273 && h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)");
  //mnt_tchannel->Draw("h.ht >>microntuple_histo_Ht","h.run==198423 &&h.event ==273 &&h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)", "same");

  c_variables->cd(4);
  TH1F *tinyntuple_histo_met = new TH1F("tinyntuple_histo_met", "tinyntuple_histo_met", 100, 0, 400);
  tinyntuple_histo_met->SetLineColor(2);
  TH1F *microntuple_histo_met = new TH1F("microntuple_histo_met", "microntuple_histo_met", 100, 0, 400);
  microntuple_histo_met->SetLineStyle(2);

  tnt_tchannel->Draw("h.met >> tinyntuple_histo_met","h.run==198423 &&h.event ==273 && h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)");
  // mnt_tchannel->Draw("h.met >>microntuple_histo_met","h.run==198423 &&h.event ==273 &&h.met>20&&h.passMetJetL2&&h.uniqueJet&&(h.ntag>1||h.ntag==1)", "same");

  */





/*
Warning!  Duplicate event found: Run 200536 Event 516
Warning!  Duplicate event found: Run 200536 Event 524
Warning!  Duplicate event found: Run 198423 Event 273
Warning!  Duplicate event found: Run 198423 Event 300
Warning!  Duplicate event found: Run 198423 Event 306
Warning!  Duplicate event found: Run 198423 Event 332
Warning!  Duplicate event found: Run 198423 Event 348
Warning!  Duplicate event found: Run 198423 Event 373
*/

}
