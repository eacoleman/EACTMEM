// Our libraries
#include "TAMUWW/Tools/interface/PlotterAux.hh"

// ROOT libraries
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "THStack.h"
#include "TChain.h"
#include "TLegend.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

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
  
  //flag to keep track of whether we did the scaling already or not.
  bool scaled = false;
  
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
    } else
      totalMC += histos[p]->Integral();
    
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
    gPad->SetFillColor(0);
    gPad->SetFrameFillColor(0);

    // Make the legend
    TLegend * l = new TLegend(0.7,0.5,0.89,0.89);
    l->SetBorderSize(0);
    l->SetFillColor(0);
    
    THStack * s=0;
    TH1 * totalData=0;
    if (stacked){
       s = new THStack(TString(templateHisto->GetName())+"_stack",TString(templateHisto->GetName())+"_stack");
       totalData = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_Data");
       totalData->SetMarkerStyle(8);
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
               s->Add(histos[h],"hist");
               l->AddEntry(histos[h],(procs[h]->name).c_str(),"f");
            } else {
               totalData->Add(histos[h]);
            }
            
            if (h == histos.size() -1) {
               s->Draw();
               s->SetTitle(templateHisto->GetName());
               for(unsigned int a=0; a<axisTitles.size(); a++){
                  if (a==0){
                     s->GetXaxis()->SetTitleOffset(1.1);
                     s->GetXaxis()->SetTitle(axisTitles[a].c_str());
                  }
                  else if (a==1){
                     s->GetYaxis()->SetTitleOffset(1.3);
                     s->GetYaxis()->SetTitle(axisTitles[a].c_str());
                  }
               }
               l->AddEntry(totalData,"Data","p");
               totalData->Draw("same");
            }
         } else {
            if (h==0){
               histos[h]->Draw();
               for(unsigned int a=0; a<axisTitles.size(); a++){
                  if (a==0){
                     histos[h]->GetXaxis()->SetTitleOffset(1.1);
                     histos[h]->GetXaxis()->SetTitle(axisTitles[a].c_str());
                  }
                  else if (a==1){
                     histos[h]->GetYaxis()->SetTitleOffset(1.3);
                     histos[h]->GetYaxis()->SetTitle(axisTitles[a].c_str());
                  }
                  else if (a==2){
                     histos[h]->GetZaxis()->SetTitle(axisTitles[a].c_str());
                  }
               }
            } else
               histos[h]->Draw("same");
            l->AddEntry(histos[h],(procs[h]->name).c_str(),"l");
         }
      }
      catch (exception& e){
         if (s==0)
            cout << "\tERROR::THStack * s is uninitiallized." << endl;
         else if (totalData==0)
            cout << "\tERROR::TH1 * totalData is uninitiallized." << endl;
         else
            cout << "\tERROR::The standard exception " << e.what() << " occured." << endl;
      }
    }
    l->Draw("same");

    return c;

}//getCanvas



