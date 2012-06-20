#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TString.h>
#include <TH1.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TStyle.h>

#include "TObjArray.h"
#include "TFractionFitter.h"

using namespace std;

#include </uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/src/TAMUWW/Tools/BkgPlotTools.cc>

void TemplateFit(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0, const char* XAxisName=0 , bool weightHists=false, const char* wtVar=0)
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given events. 
/// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
/// Use AddRest to give additional restrictions)
//input the event counts per pb^-1, evtcounts[0]=Luminosity
{
  //int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);
  cout << "nModes=" << nModes << endl;
  TString label;
  double BinWidth=(VarMax-VarMin)/NBins;
  char BinWidth_char[5];
  sprintf(BinWidth_char,"%4.1f",BinWidth);


  //Create the canvas
  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,500,500);
  //TFrame *frm = new TFrame(10,10,300,300);
  TLegend *lgnd;
  lgnd = new TLegend(0.6,0.6,0.90,0.9);
  lgnd->SetFillColor(kWhite);

  ////Define the histograms
  TH1F* h[20];
  //h[0]=Data, h[1]=QCD (from Data, float), h[2]=WpJ (from MC, float), h[3]-h[nModes-1] fixed MC
  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,true,false,0,weightHists,wtVar);


  // Normalize to the data (evtcounts[0]=Lumi in pb^-1)
  double Lumi=evtcounts[0];
  double entries, entriesData;
  entriesData=h[0]->GetEntries();
  cout << "entriesData=" << entriesData << endl;

  //h[0]->Scale(1.0/Lumi);//evtcounts[0]=Lumi in pb^-1
  entries=h[1]->GetEntries();
  cout << "entriesQCD=" << entries << endl;
  h[1]->Scale(Lumi/evtcounts[1]);//evtcounts[1]= QCD Lumi in pb^-1

  for (Int_t n=2; n<nModes; n++) {
    entries=h[n]->GetEntries();
    cout << "Mode" << n << ": entries=" << entries << endl;
    h[n]->Scale(Lumi*evtcounts[n]/entries);
  }


  TH1F* h_MC_fixed = new TH1F("h_MC_fixed","h_MC_fixed",NBins,VarMin,VarMax);
  h_MC_fixed->Sumw2();
  double nfixedMC=0;
  cout << "s-2" << endl;
  double fracfixedMC;
  if ( nModes>=4 ) {
    for (Int_t n=3; n<nModes; n++) {
      nfixedMC=nfixedMC+Lumi*evtcounts[n];
      h_MC_fixed->Add(h[n]);
    }
    fracfixedMC=nfixedMC/entriesData;
    cout << "fracfixedMC=" << fracfixedMC << endl;
  }

  TObjArray* mc;
  if ( nModes>=4 ) {
    mc = new TObjArray(3);        // MC histograms are put in this array
  } else {
    mc = new TObjArray(2);
  }

  h[1]->Scale(entriesData/(h[1]->Integral()));
  mc->Add(h[1]);
  h[2]->Scale(entriesData/(h[2]->Integral()));
  mc->Add(h[2]);

  /// Add the fixed MC modes if they exist
  if ( nModes>=4 ) {
    mc->Add(h_MC_fixed);
  }

  cout << "s1" << endl;
  //Define the fitter  
  TFractionFitter* fit = new TFractionFitter(h[0],mc); // initialise
  //int n_hfit = entriesData;
  cout << "s2" << endl;

  fit->Constrain(1,0.0,1.0);               // constrain fraction 1 to be between 0 and 1
  fit->Constrain(2,0.0,1.0);
  if ( nModes>=4 ) {
    fit->Constrain(3,fracfixedMC-0.0001*fracfixedMC,fracfixedMC+0.0001*fracfixedMC);
  }

  cout << "s3" << endl;

  fit->SetRangeX(1,NBins);                    // use only some bins to fit
  int status = fit->Fit();               // perform the fit
  cout << "Fit Status = " << status << endl;
  
//   //********** Make and save Canvas for the plots ********** //
//   gROOT->ProcessLine(".L ~kalanand/tdrstyle.C");
//   setTDRStyle();
//   tdrStyle->SetErrorX(0.5);
//   tdrStyle->SetPadLeftMargin(0.19);
//   tdrStyle->SetPadRightMargin(0.10);
//   tdrStyle->SetPadBottomMargin(0.15);
//   tdrStyle->SetLegendBorderSize(0);
//   tdrStyle->SetTitleYOffset(1.5);


  TH1F * result = (TH1F*) fit->GetPlot();
  //result->SetFillColor(16);
  result->SetFillColor(kYellow);
  //result->SetFillStyle(4050);
  h[0]->SetMinimum(0);
  h[0]->Draw("Ep");
  result->Draw("same");
  lgnd->AddEntry(result,"TemplatePrediction","f");

  //Draw the template histograms
  Double_t frac_value,frac_error;
  fit->GetResult(0,frac_value,frac_error);
  cout << modenames[1] << " : frac_value=" << frac_value << "+/-" << frac_error << "  NEvts=" << frac_value*entriesData << "+/-" << frac_error*entriesData << endl;
  //h[1]->Scale(n_hfit*frac_value/h[1]->Integral());
  h[1]->Scale(frac_value);

  h[1]->SetLineWidth(2);
  h[1]->SetLineColor(colors[1]);
  h[1]->Draw("same");
  lgnd->AddEntry(h[1],modenames[1],"l");
  //
  fit->GetResult(1,frac_value,frac_error);
  //cout << "frac_value1=" << frac_value << endl;
  cout << modenames[2] << " : frac_value=" << frac_value << "+/-" << frac_error << "  NEvts=" << frac_value*entriesData << "+/-" << frac_error*entriesData << endl;
  h[2]->Scale(frac_value);
  h[2]->SetLineWidth(2);
  h[2]->SetLineColor(colors[2]);
  h[2]->Draw("same");
  lgnd->AddEntry(h[2],modenames[2],"l");
  /// Add the fixed MC modes if they exist
  if ( nModes>=4 ) {
    fit->GetResult(2,frac_value,frac_error);
    cout << "frac_value2=" << frac_value << "+/-" << frac_error << endl;
    h_MC_fixed->Scale(entriesData*frac_value/h_MC_fixed->Integral());
    h_MC_fixed->SetLineWidth(2);
    h_MC_fixed->SetLineColor(colors[3]);
    h_MC_fixed->Draw("same");
    lgnd->AddEntry(h_MC_fixed,"Fixed Processes","l");
  }

  h[0]->Draw("Ep:same");
  h[0]->SetLineColor(colors[0]);
  //h[0]->SetMarkerStyle(20);
  //h[0]->SetMarkerSize(1);
  lgnd->AddEntry(h[0],"Data","p");

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.97);
  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  if ( XAxisName!=0 ) {
    h[0]->SetXTitle(XAxisName);
  }
  //h[0]->GetXaxis()->Draw("same");
  h[0]->GetXaxis()->SetTicks("+-");
  h[0]->GetXaxis()->SetTitleOffset(1.2);

  label="GeV";
  label=BinWidth_char+label;
  label="Events/"+label;
  h[0]->SetYTitle(label);
  cnv->SetLeftMargin(0.17);
  double max=h[0]->GetMaximum();
  h[0]->SetMaximum(max*1.15);

  h[0]->GetYaxis()->SetTitleOffset(-2.10);
  h[0]->GetYaxis()->Draw("same");
  h[0]->GetYaxis()->SetTicks("+-");

  ///Draw chi2/ndof:
  char text[30];
  double chi2dof=(fit->GetChisquare())/(NBins-2.0);
  //sprintf(text,"#chi^{2}/ndof = %5.2f",chi2dof);
  //TLatex* tl = new TLatex(0.62,0.45,text);
//   tl->SetTextSize(1./25.);
//   tl->SetTextFont(132);
//   tl->SetNDC();
//   tl->Draw();

  lgnd->Draw();
  cnv->Draw();

  cout << "FitChiSquare/ndof=" << chi2dof << endl;
  cout << "FitProbatility=" << fit->GetProb() << endl;


  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }


}



