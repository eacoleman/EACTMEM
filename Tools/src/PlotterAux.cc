// Our libraries
#include "TAMUWW/Tools/interface/PlotterAux.hh"

// ROOT libraries
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TVirtualPad.h"
#include "TH1.h"
#include "THStack.h"
#include "TChain.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TMath.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

//function to get chi2 and KS
void drawKSandChi2Tests(TH1* totalData, TH1* all, pair<double, double> range);

//function to get luminosity
void lumi(double intLum);

proc::proc(string procName, string fileName, double cross_section, double lum, 
           unsigned int in_ev, int col, string treeName):
   name(procName), chain (0), sigma(cross_section), intLum(lum),
   initial_events(in_ev), color(col) {
  
  TFile * file = TFile::Open(fileName.c_str());
  if (!file->IsOpen()){
    cout<<"ERROR proc::proc() could not open file "<<fileName<<endl;
    return;
  }
  chain = (TChain*) file->Get(treeName.c_str());
  if (chain == 0)
    cout<<"ERROR proc::proc() could not find tree named "<<treeName
	<<" in file "<<fileName<<endl;
  return;
  }


// Create a new histo here
void aPlot::prepareToFillProcess(string procName){
  string n = string(templateHisto->GetName());
  TH1 * clone = (TH1*) templateHisto->Clone((n+"_"+procName).c_str());
  clone->Sumw2();
  histos.push_back(clone);
}

// Fill the last histo here
void aPlot::Fill(double h, double w){
  if (histos.size()>0) 
    histos.back()->Fill(h,w);
  else
    cout<<"ERROR in aPlot named "<<templateHisto->GetName()<<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}


// Do the scaling to luminosity or data.
void aPlot::doScaling(vector<proc*> procs){
  
  // Make sure we can't do the scaling twice
  if (scaled) return;
  
  // Make sure that the number of histos and processes is the same
  if (procs.size() != histos.size()){
    cout<<"ERROR aPlot::doScaling() wrong sizes. Something is wrong"<<endl;
    return;
  }
  
  // First normalize to luminosity, and in case we might need to normalize to data
  // obtain also the total MC and total data
  double totalMC   = 0;
  double totalData = 0;
  bool foundData = false;
  for (unsigned int p = 0 ; p < procs.size() ; p++){
    
    histos[p]->Scale(procs[p]->getScaleFactor());
    
    if (procs[p]->name.compare("Data") == 0){
      totalData += histos[p]->Integral();
      foundData = true;
    } else{
      totalMC += histos[p]->Integral();
    }
  }// for processes
  
  // Normalize to data ?
  if (normToData){
    
    // basic sanity checks
    if (!foundData)
      cout<<"Error aPlot::doScaling requested normToData is true but \"Data\" process is not found!"<<endl;
    if (totalData == 0){
      cout<<"Error aPlot::doScaling requested normToData but Data has 0 entries!"<<endl;
      return;
    }
    
    //Do the scaling
    for (unsigned int p = 0 ; p < procs.size() ; p++)
      if (procs[p]->name.compare("Data") != 0)
         histos[p]->Scale(totalData/totalMC);
    
  }// normToData

//this will tell you the area of the process
//  for (unsigned int p = 0 ; p < procs.size() ; p++){
//     cout << histos[p]->GetName() << " " << histos[p]->Integral() << endl;
//  }
  
  //set this flag to true so we won't do the scaling again.
  scaled = true;
  
}// doScaling

// Make the Canvas here
TCanvas * aPlot::getCanvas(vector<proc*> procs){
  
    // do the scaling of the histos to lum or to data
    doScaling(procs);

    // Make the canvas
    TCanvas * c = new TCanvas(templateHisto->GetName());
    c->SetFillColor(0);
    c->Divide(1,2);

    //define the two different pads that go on the canvas
    //this is the pad with the Data/MC on it
    TVirtualPad * c1_1 = c->GetPad(2);
    c1_1->SetPad(0.01,0.01,0.99,0.32);

    //this is the pad with the actual histogram on it
    TVirtualPad *c1_2 = c->GetPad(1);
    c1_2->SetPad(0.01,0.35,0.99,0.99);
    c1_2->SetObjectStat(true);


    // Make the legend
    TLegend * l = new TLegend(0.78,0.5,0.89,0.89);
    l->SetBorderSize(0);
    l->SetFillColor(0);

    //Make the Stacks, sTop, unstacked  and data histograms
    THStack * s=0;
    TH1 * totalData=0;
    TH1 * tData=0;
    TH1 * stop=0;
    TH1 * all = 0;
    gStyle->SetOptStat(2211);

    if (stacked){
       s = new THStack(TString(templateHisto->GetName())+"_stack",TString(templateHisto->GetName())+"_stack");
       totalData = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_Data");
       tData = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_MyData");
       stop = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_STop");
       all = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_All");
       totalData->Sumw2();
       totalData->SetMarkerStyle(8);
       totalData->SetMarkerSize(0.7);
       tData->SetMarkerStyle(8);
       tData->SetMarkerSize(0.7);
       stop->SetFillColor(12);
       stop->SetLineColor(12);
    }

    // Loop over all the histos, drawing them, or add some of them up if needed
    // eg WW and WZ are usually put together...
    for (unsigned int h = 0 ; h < histos.size() ; h++){
      histos[h]->SetLineColor(procs[h]->color);
      histos[h]->SetFillColor(procs[h]->color);
      histos[h]->SetMarkerColor(procs[h]->color);
      try{
         if (stacked && s!=0 and totalData!=0){
            if (procs[h]->name.compare("Data") != 0){
               //combine the STop MC into one piece
               if (procs[h]->name.compare("STopT_T") == 0
                   || procs[h]->name.compare("STopT_Tbar") == 0
                   || procs[h]->name.compare("STopS_T") == 0
                   || procs[h]->name.compare("STopS_Tbar") == 0
                   || procs[h]->name.compare("STopTW_T") == 0
                   || procs[h]->name.compare("STopTW_Tbar") == 0){
                  stop->Add(histos[h]);
                  all->Add(histos[h]);
               }
               else {
                  s->Add(histos[h],"hist");
                  l->AddEntry(histos[h],(procs[h]->name).c_str(),"f");
                  all->Add(histos[h]);
               }
            }
            else {
               s->Add(stop,"hist");
               l->AddEntry(stop,"STop","f");
               totalData->Add(histos[h]);
               tData->Add(histos[h]);
            }
            if (h == histos.size() -1) {
               //plot the histogram on the top pad
               c1_2->cd();
               s->Draw();
               s->SetTitle(templateHisto->GetTitle());
               for(unsigned int a=0; a<axisTitles.size(); a++){
                  if (a==0){
                     s->GetXaxis()->SetTitleOffset(1.1);
                     s->GetXaxis()->SetTitle(axisTitles[a].c_str());
                     s->GetXaxis()->SetRangeUser(range.first,range.second);
                     s->GetXaxis()->SetLabelSize(0.06);
                     s->GetXaxis()->SetTitleSize(0.06);
                     s->GetXaxis()->SetLabelFont(42);
                     s->GetXaxis()->SetTitleFont(42);
                  }
                  else if (a==1){
                     s->GetYaxis()->SetTitleOffset(1.3);
                     s->GetYaxis()->SetTitle(axisTitles[a].c_str());
                     s->SetMaximum(max(s->GetMaximum(),totalData->GetMaximum())*1.1);
                     s->GetYaxis()->SetLabelSize(0.06);
                     s->GetYaxis()->SetTitleSize(0.06);
                     s->GetYaxis()->SetLabelFont(42);
                     s->GetYaxis()->SetTitleFont(42);
                     s->GetYaxis()->SetTitleOffset(0.85);
                  }
               }
               l->AddEntry(totalData,"Data","lpe");
               totalData->Draw("same");
               l->Draw("same");
               drawKSandChi2Tests(totalData, all, range);
            }
         } 
         else {
            if (h==0){
               histos[h]->Draw();
               for(unsigned int a=0; a<axisTitles.size(); a++){
                  if (a==0){
                     histos[h]->GetXaxis()->SetTitleOffset(1.1);
                     histos[h]->GetXaxis()->SetTitle(axisTitles[a].c_str());
                     histos[h]->GetXaxis()->SetRangeUser(range.first,range.second);
                     histos[h]->GetXaxis()->SetLabelSize(0.06);
                     histos[h]->GetXaxis()->SetTitleSize(0.06);
                     histos[h]->GetXaxis()->SetLabelFont(42);
                     histos[h]->GetXaxis()->SetTitleFont(42);
                  }
                  else if (a==1){
                     histos[h]->GetYaxis()->SetTitleOffset(1.3);
                     histos[h]->GetYaxis()->SetTitle(axisTitles[a].c_str());
                     histos[h]->SetMaximum(max(histos[h]->GetMaximum(),totalData->GetMaximum())*1.1);
                     histos[h]->GetYaxis()->SetLabelSize(0.06);
                     histos[h]->GetYaxis()->SetTitleSize(0.06);
                     histos[h]->GetYaxis()->SetLabelFont(42);                     
                     histos[h]->GetYaxis()->SetTitleFont(42);
                     histos[h]->GetYaxis()->SetTitleOffset(0.85);
                  }
                  else if (a==2){
                     histos[h]->GetZaxis()->SetTitle(axisTitles[a].c_str());
                  }
               }
            } 
            else
               histos[h]->Draw("same");
            if (procs[h]->name.compare("STop") == 0)
               l->AddEntry(stop,"STop","l");
            else
               l->AddEntry(histos[h],(procs[h]->name).c_str(),"l");
           
            drawKSandChi2Tests(totalData, all, range);
         }

         //plot the (data-MC)/MC part on the bottom pad
         c1_1->cd();
         c1_1->SetGridy();
         c1_1->SetObjectStat(false);
         
         tData->SetTitle("");
         tData->Add(all,-1);
         tData->Divide(all);
         tData->SetMinimum(-0.99);
         tData->SetMaximum(0.99);
         tData->SetLineWidth(2);

         tData->GetXaxis()->SetLabelFont(42);
         tData->GetXaxis()->SetLabelOffset(0.007);
         tData->GetXaxis()->SetLabelSize(0.11);
         tData->GetXaxis()->SetTitleSize(0.12);
         tData->GetXaxis()->SetTitleFont(42);
         tData->GetXaxis()->SetRangeUser(range.first,range.second);

         tData->GetYaxis()->SetTitle("#frac{Data - MC}{MC}");
         tData->GetYaxis()->SetTitleOffset(0.55);
         tData->GetYaxis()->SetNdivisions(105);
         tData->GetYaxis()->SetLabelFont(42);
         tData->GetYaxis()->SetLabelOffset(0.012);
         tData->GetYaxis()->SetLabelSize(0.11);
         tData->GetYaxis()->SetTitleSize(0.1);
         tData->GetYaxis()->SetTitleFont(42);

         tData->SetStats(0);
         tData->Draw();  

      }//try
      catch (exception& e){
         if (s==0)
            cout << "\tERROR::THStack * s is uninitiallized." << endl;
         else if (totalData==0)
            cout << "\tERROR::TH1 * totalData is uninitiallized." << endl;
         else
            cout << "\tERROR::The standard exception " << e.what() << " occured." << endl;
      }//catch
    }//for

    return c;

}//getCanvas

//________________________________________________________________________________

void drawKSandChi2Tests(TH1* totalData, TH1* all, pair<double, double> range){

   double x = (range.second - range.first)*0.45 + range.first;
   double y = totalData->GetMaximum()*1.08;

   double chi2;
   int NDF;
   int igood;

   TLatex * ks = new TLatex(x,y,Form("KolmogorovTest = %g",totalData->KolmogorovTest(all)));
   TLatex * chi2P = new TLatex(x,y*0.92,Form("Chi2Prob = %g",all->Chi2TestX(totalData,chi2,NDF,igood,"WW")));
   TLatex * chi2NDF = new TLatex(x,y*0.84,Form("Chi2/NDF = %g",chi2/NDF));

   ks->Draw();
   chi2P->Draw();
   chi2NDF->Draw();
}//drawKSandChi2Tests

//______________________________________________________________________________

void lumi(double intLum)
{               
   const float LUMINOSITY = intLum;                       
   TLatex latex; 
   latex.SetNDC();                                         
   latex.SetTextSize(0.04);                                
   latex.SetTextAlign(31); // align right                  
   latex.DrawLatex(0.90,0.96,"#sqrt{s} = 7 TeV");          
   if (LUMINOSITY > 0.) {                                  
      latex.SetTextAlign(31); // align right                
      latex.DrawLatex(0.65,0.85,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) 
                                     LUMINOSITY)); //29/07/2011    
   }             
   //latex.SetTextAlign(11); // align left
   //latex.DrawLatex(0.18,0.96,"CMS preliminary 2012");
}//lumi
