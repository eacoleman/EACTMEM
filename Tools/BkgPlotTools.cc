#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TString.h>
#include <TH1.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
using namespace std;

void BkgPlotTools () {
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////*********Helper Functions*********//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void readModes(const char* modelinein, vector<TString>& modename, vector<TString>& infilename, vector<double>& evtcount, vector<int>& color, int& nModes) {
//// Takes modelinein, which should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color [2] modename[2] ... ", and extracts these parameters as well as the number of modes
  nModes=0;
  modename.clear();
  infilename.clear();
  evtcount.clear();
  color.clear();
  TString t_modename, t_infilename;
  double t_evtcount;
  int t_color;
  istrstream inModes(modelinein);
  bool cont=true;

  // input the names, file locations & event counts
  while ( cont==true ) {
    inModes >> t_modename;
    if ( t_modename!="" ) {
      inModes >> t_infilename >> t_evtcount >> t_color;
      modename.push_back(t_modename);
      infilename.push_back(t_infilename);
      evtcount.push_back(t_evtcount);
      color.push_back(t_color);
      nModes++;
    } else {
      cont=false;
    }
    if ( nModes>100 ) {
      cont=false;
      cout << "Error: made 100 iterations and unable to read in the parameters for " << modelinein << endl;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void fillHistograms(TH1F* h[20], const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, int nModes, vector<TString>& infilenames)
////Fill in each histogram in the array h from the files in infilenames.
{
  
  TString Restrictions, hname, histload_str;
  char Max_char[30], Min_char[30], n_char[5];

  /// Make a string defining the restrictions (Range + Additional Ones).
  
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }
  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;
  cout << "Restrictions=" << Restrictions << endl;
  
  //Open the files
  TFile* f[20];
  TTree* InTree[20];
  for (Int_t n=0; n<nModes; n++) {
    f[n] = new TFile(infilenames[n]);
    InTree[n] = (TTree*)f[n]->Get(TreeName);
  }
  
  //Fill the histograms
  histload_str=">>htemp";
  histload_str=VarName+histload_str;
  cout << "hist input =" << histload_str << endl;
  for (Int_t n=0; n<nModes; n++) {
    sprintf(n_char,"%i",n);
    hname="]";
    hname = n_char + hname;
    hname="h["+hname;
    cout << "hname=" << hname << endl;
    
    TH1F* htemp = new TH1F("htemp","htemp",NBins,VarMin,VarMax);
    InTree[n]->Draw(histload_str,Restrictions);
    h[n] = (TH1F*)htemp->Clone();
    //h[n]->Sumw2();
    h[n]->SetName(hname);
    h[n]->SetTitle(hname);
//     cout << "htemp Entries = " << htemp->GetEntries() << endl;
//     cout << "h[" << n << "] Entries = " << h[n]->GetEntries() << endl;  
    
    delete htemp;
  }
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////***** Primary Plot Functions*****//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void Plot_ABkg(const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* inmodefile, const char* AddRest=0, const char* TitleName = 0, const char* SaveName = 0)
//// Plots variable VarName for a particular mode.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

  TH1D* hist = new TH1D("hist","hist",NBins,VarMin,VarMax);
  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);

  TString input=">>hist";
  input=VarName+input;

  // input the ranges & additonal restrictions
  char Max_char[30];
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  char Min_char[30];
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  TString Restrictions;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }


  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;

  cout << "Restrictions=" << Restrictions << endl;
  InTree->Draw(input,Restrictions);

  hist->SetLineWidth(2);
  //hist->SetLineColor(2);
  hist->Draw();

//   TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
//   lgnd->AddEntry(hist,mode_name,"l");
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleY(0.96);
  hist->SetXTitle(VarName);
  hist->SetYTitle("Events");
  if ( TitleName!=0 ) {
    hist->SetTitle(TitleName);
  } else {
    TString Title;
    Title="-Distribution";
    Title=VarName+Title;
    Title="  "+Title;
    hist->SetTitle(Title);
  }

  //lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_CompareShapes(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0)
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
{
  // int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  double maxtemp=0;
  double max=0;
  double scale;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  //Define the histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }
  double entries;

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames);


  for (Int_t n=0; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    //    evts=evts+evtcounts[n];
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    h[n]->SetLineWidth(2);
    h[n]->SetLineColor(colors[n]);
    lgnd->AddEntry(h[n],modenames[n],"l");
    //Make sure the maximum is set to include all events.
    maxtemp=h[n]->GetMaximum();
    if ( maxtemp > max ){
      max=maxtemp;
    }

  }

//   entries = h[0]->GetEntries();
//   h[0]->Draw();
//   cout <<"entries=" << entries << endl;

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  h[0]->SetMinimum(0);
  h[0]->SetMaximum(max*1.02);

  h[0]->Draw();
  for (Int_t n=1; n<nModes; n++) {
    h[n]->Draw("same");
  }
  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  h[0]->SetYTitle("Event Count");
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_TwoSetsOfShapes(const char* TitleName, int NumberInSet1, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0)
//// Plots variable VarName for the given processes as two histograms: hSet1 is the sum of the histograms 0-(NumberInSet1-1), hSet2 is the sum of the histograms NumberInSet1-(nModes-1). The histogram for each process is scaled by evtCount[n] prior to being included in hSet.
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
{
  //int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  double maxtemp=0;
  double max=0;
  //  double scale;
  double Set1cnt=0;
  double Set2cnt=0;
  cout << "1" << endl;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  cout << "1.1" << endl;
  //Define and initialize the histograms
  TH1F* h[20];
  TH1F* hSet1;
  TH1F* hSet2;
  for (Int_t n=0; n<NumberInSet1; n++) {
    h[n] = new TH1F("h[n]","h[n]",NBins,VarMin,VarMax);
    h[n]->Sumw2();
  }
  hSet1 = new TH1F("hSet1","hSet1",NBins,VarMin,VarMax);
  hSet1->Sumw2();
  hSet2 = new TH1F("hSet2","hSet2",NBins,VarMin,VarMax);
  hSet2->Sumw2();

  cout << "2" << endl;

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  lgnd = new TLegend(0.7,0.9-0.05*2,0.90,0.9);
  double entries;

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames);

  cout << "3" << endl;

  for (Int_t n=0; n<NumberInSet1; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    Set1cnt=Set1cnt+evtcounts[n]*entries;
    h[n]->Scale(evtcounts[n]);
    cout << "3.4" << endl;
    hSet1->Add(h[n]);
    cout << "3.45" << endl;
  }

  cout << "3.5" << endl;
  //hSet1->Sumw2();
  cout << "3.6" << endl;
  hSet1->Scale(1.0/Set1cnt);
  hSet1->SetLineWidth(2);
  hSet1->SetLineColor(colors[0]);
  lgnd->AddEntry(hSet1,modenames[0],"l");
  //Make sure the maximum is set to include all events.
  maxtemp=hSet1->GetMaximum();
  if ( maxtemp > max ){
    max=maxtemp;
  }


  for (Int_t n=NumberInSet1; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    Set2cnt=Set2cnt+evtcounts[n]*entries;
    h[n]->Scale(evtcounts[n]);
    hSet2->Add(h[n]);
  }
  cout << "4" << endl;
  hSet2->Scale(1.0/Set2cnt);
  hSet2->SetLineWidth(2);
  hSet2->SetLineColor(colors[NumberInSet1]);
  lgnd->AddEntry(hSet2,modenames[NumberInSet1],"l");
  //Make sure the maximum is set to include all events.
  maxtemp=hSet2->GetMaximum();
  if ( maxtemp > max ){
    max=maxtemp;
  }


//   entries = h[0]->GetEntries();
//   h[0]->Draw();
//   cout <<"entries=" << entries << endl;
  cout << "5" << endl;

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  hSet1->SetMinimum(0);
  hSet1->SetMaximum(max*1.3);
  hSet1->Draw("Ep");
  hSet2->Draw("Ep:same");

  hSet1->SetTitle(TitleName);
  hSet1->SetXTitle(VarName);
  hSet1->SetYTitle("Normalized Events");
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_StackedBackgrounds(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0 )
//// Similar to Plot_CompareShapes, but plots backgrounds stacked on top of each other.
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given events. 
/// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
/// Use AddRest to give additional restrictions
{
  //int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  double maxtemp=0;
  double max=0;
  double scale;
  double entries;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  //Create the individual histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames);

  //Create the accumulated histograms
  TH1F* ha[20];
  
  TString haname;
  //  int nentries;
  char n_char[30];

//   TString input;
//   input=")";
//   input=Max_char+input;
//   input=","+input;
//   input=Min_char+input;
//   input=">>ht(25,"+input;
//   input=VarName+input;
//   cout << "input=" << input << endl;

  for (Int_t n=0; n<nModes; n++) {
    sprintf(n_char,"%i",n);
    haname="]";
    haname = n_char + haname;
    haname="ha["+haname;
    ha[n]=new TH1F(haname,haname,NBins,VarMin,VarMax);
    entries=h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    for (Int_t m=0; m<(n+1); m++) {
      ha[n]->Add(h[m]);
    }
    ha[n]->SetFillColor(colors[n]);
    
 }

  //  cout << "nModes=" << nMode << endl;
  int k;
  for (Int_t n=0; n<nModes; n++) {
    k=nModes-n-1;
    if ( n==0 ) {
      ha[k]->Draw();
    } else {
      ha[k]->Draw("same");
    }
    lgnd->AddEntry(ha[k],modenames[k],"f");
  }
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  //  ha[nModes-1]->SetTitle("Expected Yields per pb^{-1}");
  ha[nModes-1]->SetTitle(TitleName);
  ha[nModes-1]->SetXTitle(VarName);
  ha[nModes-1]->SetYTitle("Evts/pb^{-1}");

  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_StoreHist(const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* inmodefile=0, const char* mode_name=0, const char* AddRest=0, const char* StoreHistFileName=0 )
//// Plots a histogram (of VarName) to be used by MCLimit.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

 
  TFile * outhistfile;
  if ( StoreHistFileName!=0) {
    outhistfile = new TFile(StoreHistFileName, "RECREATE");
  } 
  TH1F* EvtHist = new TH1F("EvtHist","EvtHist",NBins,VarMin,VarMax);

  TString input=">>EvtHist";
  input=VarName+input;

  // input the ranges & additonal restrictions
  char Max_char[30];
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  char Min_char[30];
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  TString Restrictions;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }


  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;

  cout << "Restrictions=" << Restrictions << endl;
  InTree->Draw(input,Restrictions);

  double ent1 = EvtHist->GetEntries();
  cout << "ent1=" << ent1 << endl;

  double scale;

  EvtHist->SetLineWidth(2);
  EvtHist->SetLineColor(2);
  scale=1/ent1;
  EvtHist->Scale(scale);
  EvtHist->Draw();

  TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
  lgnd->AddEntry(EvtHist,mode_name,"l");
  //lgnd->SetHeader("The Legend Title");
  gStyle->SetOptStat(0);
  EvtHist->SetXTitle(VarName);
  EvtHist->SetYTitle("Normalized Events");
  TString Title;
  Title="-Distribution";
  Title=VarName+Title;
  Title="  "+Title;
//   Title=mode2_name+Title;
//   Title=" vs. "+Title;
  Title=mode_name+Title;

  EvtHist->SetTitle(Title);
  lgnd->Draw();
  if (StoreHistFileName!=0) {
   outhistfile->Write();
  }
  EvtHist->Draw();
  lgnd->Draw();
  //outhistfile->Close();

}
