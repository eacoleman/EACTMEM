#include "TH1.h"
#include <TLegend.h>
#include <TStyle.h>
using namespace std;

void DrawLimitBands(double TwoSigLow, double OneSigLow, double Mpv, double OneSigHigh, double TwoSigHigh, double LumiMin, double LumiMax, double LumiEval)
//// Draw the limit bands by extrapolating the values at LumiEval between LumiMax and LumiMin
{
  double Nsteps=500;
  double LumiStep=(LumiMax-LumiMin)/Nsteps;
//   double TwoSigLowMin=TwoSigLow*LumiMin/LumiEval;
//   double OneSigLowMin=OneSigLow*LumiMin/LumiEval;
//   double MpvMin=Mpv*LumiMin/LumiEval;
//   double OneSigHighMin=OneSigHigh*LumiMin/LumiEval;
//   double TwoSigHighMin=TwoSigHigh*LumiMin/LumiEval;

  TH1F* hist_TwoSigLow=new TH1F("hist_TwoSigLow","hist_TwoSigLow",500,LumiMin,LumiMax);
  TH1F* hist_OneSigLow=new TH1F("hist_OneSigLow","hist_OneSigLow",500,LumiMin,LumiMax);
  TH1F* hist_Mpv=new TH1F("hist_Mpv","hist_Mpv",500,LumiMin,LumiMax);
  TH1F* hist_OneSigHigh=new TH1F("hist_OneSigHigh","hist_OneSigHigh",500,LumiMin,LumiMax);
  TH1F* hist_TwoSigHigh=new TH1F("hist_TwoSigHigh","hist_TwoSigHigh",500,LumiMin,LumiMax);
  TH1F* hist_Line=new TH1F("hist_Line","hist_Line",500,LumiMin,LumiMax);

  double currentLumi;
  double fcnEval;

  for (Int_t n=0; n<Nsteps;n++) {
    currentLumi=LumiMin+n*LumiStep;
    fcnEval=TMath::Sqrt(currentLumi/LumiEval);

    hist_TwoSigLow->SetBinContent(n+1,TwoSigLow/fcnEval);
    hist_OneSigLow->SetBinContent(n+1,OneSigLow/fcnEval);
    hist_Mpv->SetBinContent(n+1,Mpv/fcnEval);
    hist_Line->SetBinContent(n+1,1);
    hist_OneSigHigh->SetBinContent(n+1,OneSigHigh/fcnEval);
    hist_TwoSigHigh->SetBinContent(n+1,TwoSigHigh/fcnEval);
  }

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  
  hist_TwoSigHigh->SetFillColor(5);
  hist_OneSigHigh->SetFillColor(3);
  hist_OneSigLow->SetFillColor(5);
  hist_TwoSigLow->SetFillColor(kGray);


  hist_Mpv->SetLineWidth(2);
  hist_Line->SetLineWidth(2);
  hist_Line->SetLineStyle(2);
  hist_TwoSigHigh->SetLineColor(5);
  hist_OneSigHigh->SetLineColor(3);
  hist_OneSigLow->SetLineColor(3);
  hist_TwoSigLow->SetLineColor(5);

  gStyle->SetOptStat(0);


  //hist_TwoSigHigh->SetMinimum(TwoSigLow*TMath::Sqrt(LumiEval/LumiMax));
  hist_TwoSigHigh->SetMaximum(Mpv*TMath::Sqrt(LumiEval/LumiMin));
  hist_TwoSigHigh->SetMinimum(0);
  hist_TwoSigHigh->SetTitle("Exclusion Limits");
  gStyle->SetTitleX(0.33);
  gStyle->SetTitleY(0.96);
  hist_TwoSigHigh->SetXTitle("Luminosity pb^{-1}");
  hist_TwoSigHigh->SetYTitle("99% CL Signal/SM");
  hist_TwoSigHigh->Draw("l");
  hist_OneSigHigh->Draw("lsame");
  hist_Mpv->Draw("lsame");
  hist_OneSigLow->Draw("lsame");
  hist_TwoSigLow->Draw("lsame");
  hist_Line->Draw("lsame");

  TLegend * legend;
  legend = new TLegend(0.70,0.7,0.9,0.9);
  legend->AddEntry(hist_Mpv,"Expected","l");
  legend->AddEntry(hist_TwoSigHigh,"Expected #pm 2#sigma","f");
  legend->AddEntry(hist_OneSigHigh,"Expected #pm 1#sigma","f");
  legend->Draw();

}




