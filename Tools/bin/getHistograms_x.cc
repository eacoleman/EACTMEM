///////////////////////////////////////////////////////////////////////////////
//
// getHistograms_x
// ---------------
//
//               07/16/2012 Alexx Perloff            <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TLine.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLatex.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element,const string& lepton);

/// format and analyze the on vs off shell percentages in a higgs sample
void analyzeOnVsOffShell(string inFileLoc,string inFile,string lepton);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string lepton           = cl.getValue<string> ("lepton",        "");
   string inFileLoc        = cl.getValue<string> ("inFileLoc",     "");
   string inFile           = cl.getValue<string> ("inFile",        "");
   string outFileLoc       = cl.getValue<string> ("outFileLoc",    "");
   string outFile          = cl.getValue<string> ("outFile",       "");
   vector<string> objects  = cl.getVector<string>("objects",       "");
   bool anaH               = cl.getValue<bool>   ("anaH",       false);
   
   if (!cl.check()) return 0;
   cl.print();

   if(!TString(inFileLoc).EndsWith("/")) inFileLoc+="/";
   if(!TString(outFileLoc).EndsWith("/")) outFileLoc+="/";
   if(!gSystem->OpenDirectory(TString(outFileLoc))) gSystem->mkdir(TString(outFileLoc));

  // Check that lepton is set to either muon or electron
   if(lepton != "muon" && lepton != "electron")
   {
      cout << "ERROR: lepton was not properly set. Options are electron and muon." << endl;
      return 1;
   }

   if(inFile == "")
      inFile = "outputFile_" + lepton + ".root";
   
   if(outFile == "")
      outFile = "outputFile_" + lepton + "_Histograms.root";
   
   TFile* ifile = new TFile((inFileLoc+inFile).c_str(),"READ");
   if (!ifile->IsOpen()) { cout<<"Can't open "<<inFileLoc<<outFile<<endl; return 0; }
   
   TFile* ofile = new TFile((outFileLoc+outFile).c_str(),"RECREATE");
   if (!ofile->IsOpen()) { cout<<"Can't create "<<outFileLoc<<outFile<<endl; return 0; }
   
   TIter nextCan(ifile->GetListOfKeys());
   TKey* canKey(0);
   while ((canKey=(TKey*)nextCan())) {
      if (strcmp(canKey->GetClassName(),"TCanvas")!=0) continue;

      TCanvas* ican = (TCanvas*)canKey->ReadObj();
      string can(ican->GetName());
      
      if (objects.size()>0&&!contains(objects,can,lepton)) continue;

      TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(ican->GetName());
      odir->cd();
      
      cout<<can<<" ... "<<endl;

      TPad* pad = (TPad *) ican->GetPrimitive((can+"_1").c_str());
      THStack* mcStack = (THStack*) pad->GetPrimitive((can+"_stackMC").c_str());
      THStack* dataStack = (THStack*) pad->GetPrimitive((can+"_stackData").c_str());
      TList* mcList = mcStack->GetHists();
      TList* dataList = dataStack->GetHists();
      
      TIter nextHistMC(mcList);
      TKey* histKey(0);
      while ((histKey=(TKey*)nextHistMC())) {
         TH1* hist = (TH1*)mcList->FindObject(histKey->GetName());
         if (!TString(hist->ClassName()).Contains("TH1") && !TString(hist->ClassName()).Contains("TH2"))
            continue;

         if (!hist) {
            cout << "\tWARNING::Histogram " << histKey->GetName() << " could not be found." << endl
                 << "\tSkipping histogram" << endl;
            continue;
         }

         string histname(hist->GetName());
         cout << "\t" << histname << " ... ";
         hist->Write();
         cout << "DONE" << endl;
      }
      TIter nextHistDATA(dataList);
      histKey = 0;
      while ((histKey=(TKey*)nextHistDATA())) {
         TH1* hist = (TH1*)dataList->FindObject(histKey->GetName());
         if (!TString(hist->ClassName()).Contains("TH1") && !TString(hist->ClassName()).Contains("TH2"))
            continue;

         if (!hist) {
            cout << "\tWARNING::Histogram " << histKey->GetName() << " could not be found." << endl
                 << "\tSkipping histogram" << endl;
            continue;
         }

         string histname(hist->GetName());
         cout << "\t" << histname << " ... ";
         hist->Write();
         cout << "DONE" << endl;
      }
   }

   ofile->Close();

   if (anaH)
      analyzeOnVsOffShell(outFileLoc,outFile,lepton);      


   return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element,const string& lepton) {
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)+"_"+lepton==element) return true;
  return false;
}

//______________________________________________________________________________
void analyzeOnVsOffShell(string inFileLoc,string inFile,string lepton) {

   Int_t sty = gStyle->GetOptStat();
   gStyle->SetOptStat(0);

   TFile* ifile = new TFile((inFileLoc+inFile).c_str(),"UPDATE");
   if (!ifile->IsOpen()) { cout<<"Can't create "<<inFileLoc<<inFile<<endl; return; }

   ifile->cd(TString("/MWjjVsMWlv_"+lepton));

   TH2D* h = (TH2D*)gDirectory->Get(TString("MWjjVsMWlv_"+lepton+"_ggH125"));
   h->SetName(TString(h->GetName())+"_formated");

   TLine* lineXlow = new TLine(65,0,65,200);
   TLine* lineXhigh = new TLine(95,0,95,200);
   TLine* lineYlow = new TLine(0,65,200,65);
   TLine* lineYhigh = new TLine(0,95,200,95);
   lineXlow->SetLineColor(1);
   lineXlow->SetLineWidth(1);
   lineXlow->SetLineStyle(2);
   lineXhigh->SetLineColor(1);
   lineXhigh->SetLineWidth(1);
   lineXhigh->SetLineStyle(2);
   lineYlow->SetLineColor(1);
   lineYlow->SetLineWidth(1);
   lineYlow->SetLineStyle(2);
   lineYhigh->SetLineColor(1);
   lineYhigh->SetLineWidth(1);
   lineYhigh->SetLineStyle(2);
 
   TCanvas can("c","c",800,800);
   can.cd();

   h->SetTitle("Mass of Leptonic W Vs. Mass of Hadronic W");
   h->GetXaxis()->SetTitle("M_{W_{jj}}");
   h->GetXaxis()->SetTitleOffset(1.2);
   h->GetYaxis()->SetTitle("M_{W_{l#nu}}");
   h->GetYaxis()->SetTitleOffset(1.4);

   h->Draw("colz");
   lineXlow->Draw("same");
   lineXhigh->Draw("same");
   lineYlow->Draw("same");
   lineYhigh->Draw("same");
   
   char buffer[1024];
   TLatex latex;
   latex.SetTextSize(0.03);
   latex.SetTextAlign(12);

   sprintf(buffer,"%.2f%%",100*(h->Integral(1,65,1,65)/h->Integral()));
   latex.DrawLatex(28,32.5,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(66,95,1,65)/h->Integral()));
   latex.DrawLatex(70,32.5,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(96,200,1,65)/h->Integral()));
   latex.DrawLatex(140,32.5,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(1,65,66,95)/h->Integral()));
   latex.DrawLatex(28,80,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(66,95,66,95)/h->Integral()));
   latex.DrawLatex(70,80,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(96,200,66,95)/h->Integral()));
   latex.DrawLatex(140,80,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(1,65,96,200)/h->Integral()));
   latex.DrawLatex(28,150,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(66,95,96,200)/h->Integral()));
   latex.DrawLatex(70,150,TString(buffer));

   sprintf(buffer,"%.2f%%",100*(h->Integral(96,200,96,200)/h->Integral()));
   latex.DrawLatex(140,150,TString(buffer));

   /*
   pave[i]->SetFillColor(0);
   pave[i]->SetBorderSize(0);
   pave[i]->SetTextFont(42);
   pave[i]->SetTextSize(0.03);
   pave[i]->Draw("EP");
   */

   can.Write();
   can.SaveAs((inFileLoc+"ggH_onVsOffShell_"+lepton+".png").c_str());
   can.SaveAs((inFileLoc+"ggH_onVsOffShell_"+lepton+".eps").c_str());

   gStyle->SetOptStat(sty);
}
