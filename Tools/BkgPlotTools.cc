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


#include "TMath.h"
#include "TF1.h"
#include "TMinuit.h"
#include "TLatex.h"

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

void fillAHistogram(TH1F* &h, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, const char* infilename, bool dosumW2 = false)
////Fill in histogram h from the file infilename.
{
  
  TString Restrictions, histload_str;
  char Max_char[30], Min_char[30];

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
  TFile* f;
  TTree* InTree;
  f = new TFile(infilename);
  InTree = (TTree*)f->Get(TreeName);
  
  //Fill the histogram
  histload_str=">>htemp";
  histload_str=VarName+histload_str;
  cout << "hist input =" << histload_str << endl;
  
  //hname="h";
  //cout << "hname=" << hname << endl;
    
  TH1F* htemp = new TH1F("htemp","htemp",NBins,VarMin,VarMax);
  InTree->Draw(histload_str,Restrictions);
  h = (TH1F*)htemp->Clone();
  if ( dosumW2 ) {
    h->Sumw2();
  }

  cout << "hentries=" << h->GetEntries() << endl; 
  delete htemp;
  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void fillHistograms(TH1F* h[20], const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, int nModes, vector<TString>& infilenames, bool dosumW2 = false, bool useDifferingCuts=false, const char* InDiffCuts=0)
////Fill in each histogram in the array h from the files in infilenames.
{
  
  //read the Differing Cuts, if any
  vector<TString> cuts;
  cuts.clear();
  TString t_cut;
  istrstream inCuts(InDiffCuts);
  if ( useDifferingCuts ) {
    for (Int_t n=0; n<nModes; n++) {
      inCuts >> t_cut;
      t_cut=t_cut+")&&";
      t_cut="("+t_cut;
      cuts.push_back(t_cut);
    }
  }


  TString Restrictions, hname, histload_str;
  char Max_char[30], Min_char[30], n_char[5];

  /// Make a string defining the restrictions (Range + Additional Ones).
  
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  Restrictions = ")";
  if (AddRest!=0) {
    Restrictions = AddRest+Restrictions;
    Restrictions = ")&&("+Restrictions;
  } else {

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
    if ( useDifferingCuts ) {
      InTree[n]->Draw(histload_str,cuts[n]+Restrictions);
    } else {
      InTree[n]->Draw(histload_str,Restrictions);
    }
    h[n] = (TH1F*)htemp->Clone();
    if ( dosumW2 ) {
      h[n]->Sumw2();
    }
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
Double_t RaileighFunction(Double_t *x, Double_t *par)
//input is of the form Constant,sigma1,sigma2
{
  Double_t sigmaSum = par[1]+par[2]*x[0];
  Double_t fitval = par[0]*x[0]*TMath::Exp(-0.5*x[0]*x[0]/(sigmaSum*sigmaSum));
  return fitval;
} 

void Plot_ABkg(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* inmodefile, const char* AddRest = 0, const char* SaveName = 0, int histColor = 0, char* XAxisTitle = 0, char* YAxisTitle = 0, int NDivisions = -1, bool doGaussianFit = false, bool doRaileighFit = false)
//// Plots variable VarName for a particular mode.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

  TH1D* hist = new TH1D("hist","hist",NBins,VarMin,VarMax);
  // TCanvas *cnv = new TCanvas("cnv","cnv",10,10,500,500);
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
  hist->SetLineColor(histColor);
  hist->Draw();

//   TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
//   lgnd->AddEntry(hist,mode_name,"l");
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleY(0.97);
  hist->SetXTitle(VarName);
  if ( XAxisTitle!=0 ) {
    hist->SetXTitle(XAxisTitle);
  }
  hist->SetYTitle("Events");
  if ( YAxisTitle!=0 ) {
    hist->SetYTitle(YAxisTitle);
  }
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

  if ( doRaileighFit ) {
    ///Fit with a Raileigh function
    TF1 *fitRaileigh = new TF1("fitRaileigh",RaileighFunction,VarMin,VarMax,3);
    fitRaileigh->SetParameters(6000.0,1.0,1.0);
    fitRaileigh->SetParLimits(0,1,100000);
    fitRaileigh->SetParLimits(1,0.0,100.0);
    fitRaileigh->SetParLimits(2,-10.0,10.0);
    fitRaileigh->SetParNames("Amp","s0","s1");


    hist->Fit("fitRaileigh");
    gMinuit->mnmatu(1);
    gStyle->SetOptStat(0);
  
    Double_t parR[3];
    Double_t FCN;
    fitRaileigh->GetParameters(parR);
    FCN=fitRaileigh->GetChisquare();
    
//     char text0[30];
//     sprintf(text0,"FCN = %6.3f",FCN);
//     TLatex* t0 = new TLatex(0.60,0.8,text0);
//     t0->SetTextSize(1./22.);
//     t0->SetTextFont(132);
//     t0->SetNDC();
//     t0->Draw();

    char text1[30];
    sprintf(text1,"#sigma_{0} = %6.3f",parR[1]);
    TLatex* t1 = new TLatex(0.60,0.8,text1);
    t1->SetTextSize(1./22.);
    t1->SetTextFont(132);
    t1->SetNDC();
    t1->Draw();

    char text2[30];
    sprintf(text2,"#sigma_{1} = %6.3f",parR[2]);
    TLatex* t2 = new TLatex(0.60,0.75,text2);
    t2->SetTextSize(1./22.);
    t2->SetTextFont(132);
    t2->SetNDC();
    t2->Draw();

  }

  if ( doGaussianFit ) {

    ///Fit with a Raileigh function
    TF1 *fitGaus = new TF1("fitGaus","gaus(0)",VarMin,VarMax);
    fitGaus->SetParameters(1000.0,(VarMin+VarMax)/2,fabs(VarMin-VarMax)/2);
    fitGaus->SetParLimits(0,1,100000);
    fitGaus->SetParLimits(1,VarMin,VarMax);
    fitGaus->SetParLimits(2,fabs(VarMin-VarMax)/64,fabs(VarMin-VarMax));
//     fitGaus->SetParNames("Amp","s0","s1");


    hist->Fit("fitGaus");
    gMinuit->mnmatu(1);
    gStyle->SetOptStat(0);
  
    Double_t parG[3];
    Double_t* errG;
    Double_t FCN;
    fitGaus->GetParameters(parG);
    errG=fitGaus->GetParErrors();
    FCN=fitGaus->GetChisquare();
    
//     char text0[30];
//     sprintf(text0,"FCN = %6.3f",FCN);
//     TLatex* t0 = new TLatex(0.60,0.8,text0);
//     t0->SetTextSize(1./22.);
//     t0->SetTextFont(132);
//     t0->SetNDC();
//     t0->Draw();

    char text1[30];
    sprintf(text1,"#mu = %6.3f#pm%6.3f",parG[1],errG[1]);
    TLatex* t1 = new TLatex(0.65,0.8,text1);
    t1->SetTextSize(1./22.);
    t1->SetTextFont(132);
    t1->SetNDC();
    t1->Draw();

    char text2[30];
    sprintf(text2,"#sigma = %6.3f#pm%6.3f",parG[2],errG[2]);
    TLatex* t2 = new TLatex(0.65,0.75,text2);
    t2->SetTextSize(1./22.);
    t2->SetTextFont(132);
    t2->SetNDC();
    t2->Draw();

  }



  hist->GetYaxis()->SetTitleOffset(1.25);
  hist->SetNdivisions(NBins/2);
  if ( NDivisions > 0 ) {
    hist->SetNdivisions(NDivisions);
  }

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//



void Plot_CompareShapesAndCuts(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0, bool withErrors = false, bool useDifferingCuts=false, const char* InExtraCuts=0, char* XAxisTitle = 0)
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
//// InCuts should be of the form "cut[0] cut[1] cut[2] ..."
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
  lgnd->SetFillColor(kWhite);
  double entries;

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,withErrors,useDifferingCuts,InExtraCuts);


  for (Int_t n=0; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    //    evts=evts+evtcounts[n];
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    //cout << "integral=" << h[n]->Integral() << endl;
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
  gStyle->SetTitleX(0.18);
  gStyle->SetTitleY(0.97);
  h[0]->SetMinimum(0);
  h[0]->SetMaximum(max*1.30);

  if ( withErrors ) {
    h[0]->Draw("Ep");
  } else {
    h[0]->Draw();
  }

  for (Int_t n=1; n<nModes; n++) {
    if ( withErrors ) {
      h[n]->Draw("Ep:same");
    } else {
      h[n]->Draw("same");
    }
  }
  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  if ( XAxisTitle!=0 ) {
    h[0]->SetXTitle(XAxisTitle);
  }

  h[0]->SetYTitle("Normalized Event Count");
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// // -----------------------------------------------------------------------------------------------------------------------------------------------------------//


// void Plot_CompareCuts(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* CutA, const char* CutB, const char* infilename, const char* AddRest = 0, const char* SaveName = 0, bool withErrors = false, const char* LabelA=0, const char* LabelB=0)
// //// Plots variable VarName for the given modes. Two histograms are normalized to unity
// {

//   TString AddRestA, AddRestB;
//   double entries,maxA,maxB,max;

//   AddRestA=AddRest;
//   AddRestA=")&&"+AddRestA;
//   AddRestA=CutA+AddRestA;
//   AddRestA="("+AddRestA;
//   AddRestB=AddRest;
//   AddRestB=")&&"+AddRestB;
//   AddRestB=CutB+AddRestB;
//   AddRestB="("+AddRestB;

//   //Define the histograms
//   TH1F* hA;
//   TH1F* hB;
//   fillAHistogram(hA,TreeName,VarName,NBins,VarMin,VarMax,AddRestA,infilename,true);
//   //  cout << "1" << endl;
//   fillAHistogram(hB,TreeName,VarName,NBins,VarMin,VarMax,AddRestB,infilename,true);
//   //  cout << "2" << endl;


//   TCanvas *cnv1 = new TCanvas("cnv1","cnv1",10,10,500,500);
//   TLegend *lgnd;
//   lgnd = new TLegend(0.6,0.7,0.900,0.9);
//   lgnd->SetFillColor(kWhite);

//   // process histogram A
//   entries = hA->GetEntries();
//   cout << CutA <<": entries=" << entries << endl;
//   hA->Scale(1.0/entries);
//   hA->SetLineWidth(2);
//   hA->SetLineColor(2);
//   if ( LabelA!=0 ) {
//     lgnd->AddEntry(hA,LabelA,"l");
//   } else {
//     lgnd->AddEntry(hA,CutA,"l");
//   }
//   maxA=hA->GetMaximum();
//   // process histogram B
//   entries = hB->GetEntries();
//   cout << CutB <<": entries=" << entries << endl;
//   hB->Scale(1.0/entries);
//   hB->SetLineWidth(2);
//   hB->SetLineColor(4);
//   if ( LabelB!=0 ) {
//     lgnd->AddEntry(hB,LabelB,"l");
//   } else {
//     lgnd->AddEntry(hB,CutB,"l");
//   }

//   maxB=hB->GetMaximum();

//   //Make sure the maximum is set to include all events.
//   if ( maxA > maxB ){
//     max=maxA;
//   } else {
//     max=maxB;
//   }

//   gStyle->SetOptStat(0);
//   gStyle->SetTitleX(0.13);
//   gStyle->SetTitleY(0.97);
//   hA->SetMinimum(0);
//   hA->SetMaximum(max*1.20);

//   if ( withErrors ) {
//     hA->Draw("Ep");
//   } else {
//     hA->Draw();
//   }

//   if ( withErrors ) {
//     hB->Draw("Ep:same");
//   } else {
//     hB->Draw("same");
//   }

//   hA->SetTitle(TitleName);
//   hA->SetXTitle(VarName);
//   hA->SetYTitle("Normalized Event Count");
//   hA->GetYaxis()->SetTitleOffset(1.25);
//   lgnd->Draw();

//   /// Save the output
//   if ( SaveName!=0 ) {
//     cnv1->SaveAs(SaveName);
//   }

// }


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

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,true);

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
//   double maxtemp=0;
//   double max=0;
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

void Plot_Validation(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0 , bool scaleMCtoData = true, bool drawMCwithErrors=false )
//// Makes a Validation/Control plot with MC either scaled either to Data or to theory expectations.
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
//// mode0=Data (evtCount[0]=Lumi), evtCount[i]=XSection/N_MCGenerated.
//// Use AddRest to give additional restrictions
{

  vector<TString> modenames, infilenames;
  vector<double> inDouble;
  vector<int> colors;
  int nModes = 0;
  int nModes_sw2 = 0;
  double scale;
  double entries;
  double nMCtotal=0;
  double nData=0;

  readModes(inModes,modenames,infilenames,inDouble,colors,nModes);

  //Create the individual histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,false);

  TH1F* h_sw2[20];
  fillHistograms(h_sw2,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes_sw2,infilenames,true);

  //Create the accumulated histograms
  TH1F* ha[20];  
  TString haname;
  char n_char[30];

  for (Int_t n=1; n<nModes; n++) {
    sprintf(n_char,"%i",n);
    haname="]";
    haname = n_char + haname;
    haname="ha["+haname;
    ha[n]=new TH1F(haname,haname,NBins,VarMin,VarMax);
    //    ha[n]->Sumw2();
    entries=h[n]->GetEntries();
    scale=inDouble[n]*inDouble[0];//(CrossSection/NEvtsGen)*Lumi
    nMCtotal+=entries*scale;
    cout << modenames[n] <<" : entries=" << entries << " : Lumi=" << inDouble[0] <<" : expectedEvts=" <<  entries*scale << endl;
    h[n]->Scale(scale);
  }

  //cout << "1" << endl;
  nData=h[0]->GetEntries();
  cout << "nMCtotal=" << nMCtotal << " : nData=" << nData << endl;

  for (Int_t n=1; n<nModes; n++) {
    if ( scaleMCtoData ) {
      h[n]->Scale(nData/nMCtotal);
    }
    for (Int_t m=1; m<(n+1); m++) {
      ha[n]->Add(h[m]);
    }
    ha[n]->SetFillColor(colors[n]);
  }


  //  cout << "nModes=" << nMode << endl;
  int k;
  for (Int_t n=1; n<nModes; n++) {
    k=nModes-n;
    if ( n==1 ) {
      ha[k]->Draw();
    } else {
      ha[k]->Draw("same");
    }
    lgnd->AddEntry(ha[k],modenames[k],"f");
  }


//   if ( drawMCwithErrors ) {
//     cout << "drawMCwithErrors=true" << endl;
//     //Add the total MC with errors (we now need the histograms with the Sumw2() option)
//     ha[0]=new TH1F("ha[0]","ha[0]",NBins,VarMin,VarMax);
//     ha[0]->Sumw2();
//     for (Int_t n=1; n<nModes_sw2; n++) {
//       if ( scaleMCtoData ) {
// 	h_sw2[n]->Scale(nData/nMCtotal);
//       }
//       ha[0]->Add(h_sw2[n]);
//     }
//     ha[0]->Draw("esame");
//     ha[0]->SetLineWidth(2);
//     ha[0]->SetLineColor(4);
//     lgnd->AddEntry(ha[0],"TotalMC","l");
//   }

  //Add the data
  h[0]->Draw("esame");
  h[0]->SetLineWidth(2);
  h[0]->SetLineColor(colors[0]);
  h[0]->SetFillColor(colors[0]);
  lgnd->AddEntry(h[0],modenames[0],"l");

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.97);
  ha[nModes-1]->SetTitle(TitleName);
  ha[nModes-1]->SetXTitle(VarName);
  ha[nModes-1]->SetYTitle("Evts");

  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



void Plot_AllValidations(const char* TitleName, int NBins, bool scaleMCtoData = true, bool drawMCwithErrors=false )
//// Makes validation plots for all of the variables
//// the SaveName=savePrefix+VarName+.png
{
  const int NVars=5;
  TString VarName[NVars];
  double VarMin[NVars];
  double VarMax[NVars];


//   eventProbs2jet.push_back(new WWEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new WZEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new WLightEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new ZLightEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new ttEventProb2Jet(divonneInt, bTF));
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
//   eventProbs2jet.push_back(new tChannelEventProb2Jet(rootInt, bTF, lightTF));
//   eventProbs2jet.push_back(new sChannelEventProb2Jet(rootInt, bTF));
//   eventProbs2jet.push_back(new QCDEventProb2Jet(rootInt, gluonTF));

  //Initialize
  VarName[0]="Mjj";
  VarMin[0]=40.0;
  VarMax[0]=300.0;
  VarName[1]="log(m_tProbStat.tEventProb[0])";
  VarMin[1]=-35;
  VarMax[1]=-10;
  VarName[2]="log(m_tProbStat.tEventProb[2])";
  VarMin[2]=-30;
  VarMax[2]=-8;
  VarName[3]="log(m_tProbStat.tEventProb[3])";
  VarMin[3]=-30;
  VarMax[3]=-10;
  VarName[4]="log(m_tProbStat.tEventProb[5])";
  VarMin[4]=-35;
  VarMax[4]=-10;
//   VarName[1]="log(epdPretagWWandWZ)";
//   VarMin[1]=-15.0;
//   VarMax[1]=5.0;

//  TString AddRest="( (leptonCat==1)&&(40.0<Mjj)&&(Mjj<300.0)&&(WmT>40.0)&&(abs(lEta)<2.1)&&(lPt>25.0)&&(j1pT>30.0)&&(j2pT>30.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(DEtajj<1.5)&&(Ptjj>40.0)&&(0.3<(j1pT/Mjj))&&((j1pT/Mjj)<0.7)&&(MET>30.0) )";
//   TString inModes_mu = "Data /uscms/home/ilyao/nobackup/Summer11/428Reduced/MEResults/microNtuples/flatmicro_SingleMu_77100Evt.root 990.0 1 WpJ /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/flatmicroWpJ_FullCutsV01DEF.root 0.0003849171 2 WW /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/flatmicroWW_FullCutsV01DEF.root 0.0000101753087377979123 3";
  TString inModes_mu = "Data /uscms/home/ilyao/nobackup/Summer11/428Full/MEResults/microNtuples/micro_SingleMu_77100Evt.root 990.0 1 WpJ /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microWpJ_FullCutsV01DEF.root 0.0003849171 2 ZpJ /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microZpJ_FullCutsV01DEF.root 0.000084017952387 3 TTbar /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microTTbar_MG_FullCutsV01DEF.root 0.00004254518 5 WW /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microWW_FullCutsV01DEF.root 0.0000101753087377979123 4";
  TString AddRest_mu="( (leptonCat==1)&&(40.0<Mjj)&&(Mjj<300.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>40.0)&&(abs(lLV[0].Eta())<2.1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>30.0)&&(jLV[1].Pt()>30.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>40.0)&&(0.3<(jLV[0].Pt()/Mjj))&&((jLV[0].Pt()/Mjj)<0.7)&&(METLV[0].Et()>30.0) )";
  TString inModes_el = "Data /uscms/home/ilyao/nobackup/Summer11/428Full/MEResults/microNtuples/micro_SingleEl_51900Evt.root 990.0 1 WpJ /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microWpJ_FullCutsV01DEF.root 0.0003849171 2 ZpJ /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microZpJ_FullCutsV01DEF.root 0.000084017952387 3 TTbar /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microTTbar_MG_FullCutsV01DEF.root 0.00004254518 5 WW /uscms_data/d3/ilyao/Summer11/428Full/MEResults/microNtuples/microWW_FullCutsV01DEF.root 0.0000101753087377979123 4";
  TString AddRest_el="( (leptonCat==2)&&(40.0<Mjj)&&(Mjj<300.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>40.0)&&(abs(lLV[0].Eta())<2.1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>30.0)&&(jLV[1].Pt()>30.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>40.0)&&(0.3<(jLV[0].Pt()/Mjj))&&((jLV[0].Pt()/Mjj)<0.7)&&(METLV[0].Et()>30.0) )";

  for (Int_t n=0; n<NVars; n++) {
    TString SaveName="_mu.png";
    SaveName=VarName[n]+SaveName;
    SaveName="~ilyao/public_html/MEValidation/Scaled_"+SaveName;
    TString Title = VarName[n];
    Title = " Muons : " + Title;
    Title = TitleName + Title;
    Plot_Validation(Title,"METree",VarName[n],NBins,VarMin[n],VarMax[n],AddRest_mu,inModes_mu,SaveName,scaleMCtoData,drawMCwithErrors);
    SaveName="_el.png";
    SaveName=VarName[n]+SaveName;
    SaveName="~ilyao/public_html/MEValidation/Scaled_"+SaveName;
    Title = VarName[n];
    Title = " Electrons : " + Title;
    Title = TitleName + Title;
    Plot_Validation(Title,"METree",VarName[n],NBins,VarMin[n],VarMax[n],AddRest_el,inModes_el,SaveName,scaleMCtoData,drawMCwithErrors);
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

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Save_YieldsAndPulls(const char* TreeName, int NBins, const char* infoFileName, const char* savedirName, const char* inModes, const char* TitleSuffix="", const char* PlotNameSuffix=0)
//// inModes should be of the form "VN[0] dummystr[0] meanEvt[0] color[0] VN[1] dummystr[1] meanEvt[1] color[1] VN[2] ... " 
//// 1. Plot&Fit numVN-meanEvt for each variable (VN=VariableName). The Title is of the form "VN_{Observed}-VN_{Expected} TitleSuffix"; saved as "savedirNameVNYield_PlotNameSuffix".
//// 2. Plot&Fit (numVN-meanEvt)/errVN for each variable. The Title is of the form "VN Pull TitleSuffix"; saved as "savedirNameVNPull_PlotNameSuffix".
{
     

  vector<TString> VN, dummystr;
  vector<double> meanEvt;
  vector<int> color;
  int nModes = 0;

  TString Var;
  TString numVN, errVN;
  TString Title, SaveName;
  double minNum, maxNum, minErr;
  double VarMin, VarMax;
  char ToChar[10];

  readModes(inModes,VN,dummystr,meanEvt,color,nModes);
  TFile* f = new TFile(infoFileName);
  TTree* InTree = (TTree*)f->Get(TreeName);
 
 
  for (Int_t n=0; n<nModes; n++) {
    numVN="num"+VN[n];
    errVN="err"+VN[n];
    minNum=InTree->GetMinimum(numVN);
    maxNum=InTree->GetMaximum(numVN);
    minErr=InTree->GetMinimum(errVN);
    ///Draw the Yield_Observed-Yield_Expected
    Title=TitleSuffix;
    Title="_{Expected} " +Title;
    Title=VN[n]+Title;
    Title="_{Observed}-" +Title;
    Title=VN[n]+Title;
    SaveName=PlotNameSuffix;
    SaveName="Yield_"+SaveName;
    SaveName=VN[n]+SaveName;
    SaveName=savedirName+SaveName;
    VarMin=floor(minNum-meanEvt[n]);
    VarMax=ceil(maxNum-meanEvt[n]);
    sprintf(ToChar,"%i",int(meanEvt[n]));
    Var=ToChar;
    Var="-"+Var;
    Var=numVN+Var;
    Plot_ABkg(Title,TreeName,Var,NBins,VarMin,VarMax,infoFileName,0,SaveName,color[n],0,"NToys",5,true,false);
    ///Draw the Pulls
    Title=TitleSuffix;
    Title=" Pull " +Title;
    Title=VN[n]+Title;
    SaveName=PlotNameSuffix;
    SaveName="Pull_"+SaveName;
    SaveName=VN[n]+SaveName;
    SaveName=savedirName+SaveName;
    VarMin=(minNum-meanEvt[n])/minErr;
    VarMax=(maxNum-meanEvt[n])/minErr;
    sprintf(ToChar,"%i",int(meanEvt[n]));
    Var=errVN;
    Var=")/"+Var;
    Var=ToChar+Var;
    Var="-"+Var;
    Var=numVN+Var;
    Var="("+Var;
    cout << "Var=" << Var << endl;
    Plot_ABkg(Title,TreeName,Var,NBins,VarMin,VarMax,infoFileName,0,SaveName,color[n],0,"NToys",5,true,false);
  }

}

