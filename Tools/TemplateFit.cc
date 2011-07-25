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

#include "TObjArray.h"
#include "TFractionFitter.h"

using namespace std;

#include <TAMUWW/Tools/BkgPlotTools.cc>

void TemplateFit(const char* TitleName, const char* TreeName, const char* VarName, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0 )
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given events. 
/// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
/// Use AddRest to give additional restrictions)
//input the event counts per pb^-1, evtcounts[0]=Luminosity
{
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  //Create the canvas
  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  lgnd = new TLegend(0.7,0.7,0.90,0.9);

  ////Define the histograms
  TH1F* h[20];
  //h[0]=Data, h[1]=QCD (from Data, float), h[2]=WpJ (from MC, float), h[3]-h[nModes-1] fixed MC
  fillHistograms(h,TreeName,VarName,VarMin,VarMax,AddRest,nModes,infilenames);

  // Normalize to the data (evtcounts[0]=Lumi in pb^-1)
  double Lumi=evtcounts[0];
  double entries, entriesData;
  entriesData=h[0]->GetEntries();
  //h[0]->Scale(1.0/Lumi);//evtcounts[0]=Lumi in pb^-1
  entries=h[1]->GetEntries();
  h[1]->Scale(Lumi/evtcounts[1]);//evtcounts[1]= QCD Lumi in pb^-1
  for (Int_t n=2; n<nModes; n++) {
    entries=h[n]->GetEntries();
    h[n]->Scale(Lumi*evtcounts[n]/entries);
  }

  TH1F* h_MC_fixed = new TH1F("h_MC_fixed","h_MC_fixed",25,VarMin,VarMax);
  h_MC_fixed->Sumw2();
  double nfixedMC=0;
  cout << "s-2" << endl;
  for (Int_t n=3; n<nModes; n++) {
    nfixedMC=nfixedMC+Lumi*evtcounts[n];
    h_MC_fixed->Add(h[n]);
  }
  double fracfixedMC=nfixedMC/entriesData;
  cout << "fracfixedMC=" << fracfixedMC << endl;


  TObjArray* mc = new TObjArray(3);        // MC histograms are put in this array
  mc->Add(h[1]);
  mc->Add(h[2]);
  mc->Add(h_MC_fixed);

  //Define the fitter  
  TFractionFitter* fit = new TFractionFitter(h[0],mc); // initialise
  int n_hfit = entriesData;

  fit->Constrain(1,0.0,1.0);               // constrain fraction 1 to be between 0 and 1
  fit->Constrain(2,0.0,1.0);
  fit->Constrain(3,fracfixedMC-0.0001*fracfixedMC,fracfixedMC+0.0001*fracfixedMC);

  //cout << "s3" << endl;

  fit->SetRangeX(1,25);                    // use only some bins to fit
  int status = fit->Fit();               // perform the fit
  cout << "Fit Status = " << status << endl;
  
  TH1F * result = (TH1F*) fit->GetPlot();
  result->SetFillColor(16);
  //result->SetFillStyle(4050);
  h[0]->SetMinimum(0);
  h[0]->Draw("Ep");
  result->Draw("same");
  lgnd->AddEntry(result,"FitResult","f");

  //Draw the template histograms
  Double_t frac_value,frac_error;
  fit->GetResult(0,frac_value,frac_error);
  cout << modenames[1] << " : frac_value=" << frac_value << "  NEvts=" << frac_value*entriesData << "+/-" << frac_error*entriesData << endl;
  h[1]->Scale(n_hfit*frac_value/h[1]->Integral());

  h[1]->SetLineWidth(2);
  h[1]->SetLineColor(2);
  h[1]->Draw("same");
  lgnd->AddEntry(h[1],modenames[1],"l");
  //
  fit->GetResult(1,frac_value,frac_error);
  //cout << "frac_value1=" << frac_value << endl;
  cout << modenames[2] << " : frac_value=" << frac_value << "  NEvts=" << frac_value*entriesData << "+/-" << frac_error*entriesData << endl;
  h[2]->Scale(n_hfit*frac_value/h[2]->Integral());
  h[2]->SetLineWidth(2);
  h[2]->SetLineColor(3);
  h[2]->Draw("same");
  lgnd->AddEntry(h[2],modenames[2],"l");
  //
  fit->GetResult(2,frac_value,frac_error);
  cout << "frac_value2=" << frac_value << "+/-" << frac_error << endl;
  h_MC_fixed->Scale(n_hfit*frac_value/h_MC_fixed->Integral());
  h_MC_fixed->SetLineWidth(2);
  h_MC_fixed->SetLineColor(4);
  h_MC_fixed->Draw("same");
  lgnd->AddEntry(h_MC_fixed,"Fixed Processes","l");

  h[0]->Draw("Ep:same");
  h[0]->SetMarkerStyle(7);
  //h[0]->SetMarkerSize(10);
  lgnd->AddEntry(h[0],"Data","p");

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.25);
  gStyle->SetTitleY(0.97);
  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  h[0]->SetYTitle("Events");

  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }


}



