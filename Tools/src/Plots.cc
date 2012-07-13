// Our libraries
#include "TAMUWW/Tools/interface/Plots.hh"

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
#include <sstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

// Function to get chi2 and KS
void drawKSandChi2Tests(TH1* totalData, TH1* all, pair<double, double> range);

// Function to get luminosity
void drawLumi(float intLum);

// ##################################################
// ################### PLOT CLASS ###################
// ##################################################

// Create a new histo here
void Plot::prepareToFillProcess(PhysicsProcessNEW * process)
{
   TString n = templateHisto->GetName();
   TH1 * clone = (TH1*) templateHisto->Clone(n+"_"+process->name);
   clone->SetTitle(process->name);
   
   clone->Sumw2();
   histos.push_back(clone);
}

// Fill the last histo here
void Plot::Fill(double x, double w){
   if (histos.size()>0) 
      histos.back()->Fill(x,w);
   else
      cout<<"ERROR in Plot named "<<templateHisto->GetName()<<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}

// Fill the last histo here
void Plot::Fill(double x, double y, double w){
   if (histos.size()>0) 
      ((TH2*)histos.back())->Fill(x,y,w);
   else
      cout<<"ERROR in Plot named "<<templateHisto->GetName()<<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}

// Do the scaling to luminosity or data.
void Plot::scaleToData(vector<PhysicsProcessNEW*> procs)
{
   // Make sure we can't do the scaling twice
   if (scaled)
      return;
  
   // Make sure that the number of histos and processes is the same
   if (procs.size() != histos.size())
   {
      cout<<"ERROR Plot::scaleToData() wrong sizes. Something is wrong"<<endl;
      return;
   }

   // First normalize to luminosity, and in case we might need to normalize to data
   // obtain also the total MC and total data
   double totalData = 0;
   double totalMC = 0;
   for (unsigned int p = 0 ; p < procs.size() ; p++)
   {
      cout << "Normalizing to luminosity and cross section..." << endl;
      
      // This works for MC and data as well.
      histos[p]->Scale(procs[p]->getScaleFactor());

      cout<<" histo="<<histos[p]->GetTitle()<<", has Integral()="<<histos[p]->Integral()<<endl;

      TString hName = histos[p]->GetTitle();
      hName.ToUpper();
      if ( hName.Contains("DATA") )
         totalData += histos[p]->Integral();
      else 
         totalMC += histos[p]->Integral();
   }
   
   // If norm to data do the scaling of the all the MC to the data
   if (normToData)
   {
      cout<<"Normalizing to data..."<<endl;
      
      if (totalData == 0)
      {
         cout<<"ERROR  Plot::scaleToData() integral of data is zero, cannot normalize to data."
             <<"WILL NOT NORMALIZE"<<endl;
         return;
      }
      
      cout<<" totalData = "<<totalData<<"  totalMC = "<<totalMC<<endl;
      
      for (unsigned int p = 0 ; p < procs.size() ; p++)
      {
         // This works for MC and data as well.
         //histos[p]->Scale(procs[p]->getScaleFactor());
      
         TString hName = histos[p]->GetTitle();
         hName.ToUpper();
         if (!hName.Contains("DATA") )
            histos[p]->Scale(totalData/totalMC);
         
         cout<<"AfterNorm histo="<<histos[p]->GetTitle()<<", has Integral()="<<histos[p]->Integral()<<endl; 
      }
    
   }
   
   //set this flag to true so we won't do the scaling again.
   scaled = true;
}

// ------------------------------------------------------------
// This groups all the histograms that need grouping.
// Typically all the Single tops and Dibosons
vector<TH1*> Plot::doGrouping(vector<PhysicsProcessNEW*> procs)
{
   // The returning vector
   vector<TH1*> groups;
   if (procs.size()  != histos.size() )
   {
      cout<<"ERROR Plot::doGrouping procs.size()="<<procs.size()
          <<" is different than histos.size()="<<histos.size()<<endl;
      cout<<"\t Returning NO groups!"<<endl;
      return groups;
   }

   // The grouped ones
   TH1 * stop = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_STop");
   stop->SetTitle("STop");
   stop->Sumw2();
   TH1 * dibo = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_Diboson");
   dibo->SetTitle("WW+WZ");
   dibo->Sumw2();
   
   // Loop over histos grouping around
   for (unsigned int h=0; h < histos.size(); h ++)
   {
      TString hName = histos[h]->GetTitle();

      if (hName.Contains("STopT_Tbar")  ||
          hName.Contains("STopT_T")     ||
          hName.Contains("STopTW_Tbar") ||
          hName.Contains("STopTW_T")    ||
          hName.Contains("STopS_Tbar")  ||
          hName.Contains("STopS_T")     )
      {
         // if first time set the attributes
         if (stop->GetEntries()==0)
         {
            stop->SetLineColor(histos[h]->GetLineColor());
            stop->SetFillColor(histos[h]->GetFillColor());
            stop->SetMarkerColor(histos[h]->GetMarkerColor());
         }
         
         // add to single top
         stop->Add(histos[h]);
         
      }
      else if (hName.Contains("WW") ||
                hName.Contains("WZ")  )
      {

         // if first time set the attributes
         if (dibo->GetEntries()==0)
         {
            dibo->SetLineColor(histos[h]->GetLineColor());
            dibo->SetFillColor(histos[h]->GetFillColor());
            dibo->SetMarkerColor(histos[h]->GetMarkerColor());
         }

         // add to diboson
         dibo->Add(histos[h]);

      }
      else
      {
         // add as an independent process.
         groups.push_back(histos[h]);
      }
   }

   // Add the diboson and single top if they were present
   if (stop->GetEntries() >0)  groups.insert(groups.begin(), stop);
   if (dibo->GetEntries() >0)  groups.insert(groups.begin(), dibo);

   // return the groupedHistos
   return groups;

}

void Plot::saveHistogramsToFile(TString histoFile)
{
   TFile output(histoFile, "RECREATE");
   for (unsigned int i = 0; i < histos.size(); i++)
   {
      histos[i]->Write();
   }
   output.Close();
}

// ##################################################
// ############## FORMATTED PLOT CLASS ##############
// ##################################################

FormattedPlot::FormattedPlot()
{
   overlaySignalFactor = 1.0;
   overlaySignalName = "";
}

// ------------------------------------------------------------
// Make the Canvas here ala Ricardo Eusebi
// ------------------------------------------------------------
TCanvas* FormattedPlot::getCanvas(vector<PhysicsProcessNEW*> procs)
{
   // Format the colors
   formatColors(procs);
   
   // Do the scaling of the histos to lum or to data
   scaleToData(procs);
  
   // Do the grouping of the histos and return the list of histograms to be plotted.
   // This takes care of putting all the SingleTop Histos together, or diboson together etc.
   std::vector<TH1*> groupedHistos = doGrouping(procs);

   // Create the total Data and MC histos and Stacks
   TString tempName = templateHisto->GetName();
   TH1     * tMC = (TH1*) templateHisto->Clone(tempName+"_TotalMC");   tMC->Sumw2();
   TH1     * tDa = (TH1*) templateHisto->Clone(tempName+"_TotalData"); tDa->Sumw2();
   TH1     * signal = 0;
   THStack * sMC = new THStack(tempName+"_stackMC",tempName+"_stackMC");
   THStack * sDa = new THStack(tempName+"_stackData",tempName+"_stackData");

   // Make the legend
   TLegend * l = new TLegend(0.8,0.4,0.96,0.89);
   l->SetName(tempName+"_legend");
   l->SetBorderSize(0);
   l->SetFillColor(0);
  
   // Create the totalHistos and Stacks for the MC and Data processes
   for (unsigned int h = 0 ; h < groupedHistos.size() ; h++)
   {
      TString hname = groupedHistos[h]->GetTitle();
      hname.ToUpper();

      if (hname.Contains("DATA"))
      {
         sDa->Add(groupedHistos[h],"hist");
         tDa->Add(groupedHistos[h]);
         l->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"lpe");
      }
      else
      {
         sMC->Add(groupedHistos[h],"hist");
         tMC->Add(groupedHistos[h]);
         l->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"f");
      }
      
      if (overlaySignalName.Length() > 0)
      {
         TString tempOverlaySignalName = overlaySignalName;
         tempOverlaySignalName.ToUpper();
         if (hname.Contains(tempOverlaySignalName))
         {
            signal = (TH1*)groupedHistos[h]->Clone();
            signal->SetFillColor(0);
            signal->Scale(overlaySignalFactor);
            
            ostringstream legendTitleStream;
            legendTitleStream << signal->GetTitle() << "(x" << overlaySignalFactor << ")";
            string legendTitle = legendTitleStream.str();
            
            l->AddEntry(signal,legendTitle.c_str(),"lp0");
         }
      }
   }

   // General Style Formatting
   gStyle->SetOptTitle(0);
   //gStyle->SetOptStat(2211);

   // Create and Draw the Canvas
   TCanvas * can = new TCanvas(templateHisto->GetName());
   can->SetFillColor(0);
   can->Divide(1,2);
   
   // Define the two different pads that go on the canvas
   // canMain: this is the pad with the actual histogram on it
   TVirtualPad *canMain = can->GetPad(1);
   canMain->SetPad(0.01,0.32,0.99,0.99);
   canMain->SetObjectStat(true);
   canMain->SetBottomMargin(0.16); 
   canMain->SetLeftMargin(0.115); 
   canMain->SetRightMargin(0.03); 
   canMain->cd();
   
   //Define the graphics option
   TString gOption = stacked ? "": "nostack";
   sMC->Draw(gOption);
   sDa->Draw(gOption+"ep,SAME");
   if (signal != 0)
   {
      signal->SetLineWidth(2);
      signal->Draw("hist,SAME");
   }
   
   // Set display range
   sMC->GetXaxis()->SetRangeUser(range.first, range.second);
   sDa->GetXaxis()->SetRangeUser(range.first, range.second);
   if (signal != 0)
      signal->GetXaxis()->SetRangeUser(range.first, range.second);

   // Format the stacks, make sure to do this after the Draw command.
   // Set the maximum range for the Y-axis
   double maxi = max(sMC->GetMaximum(),sDa->GetMaximum())*1.1;
   formatStack(sMC,maxi);
   //formatStack(sDa,maxi);

   // Add the KS and Chi2 info in the active canvas
   drawKSandChi2Tests(tDa, tMC, range);

   // Add the Legend
   l->Draw("same");

   // Add the luminosity
   drawLumi(3.6);

   // canRatio: this is the pad with the Data/MC on it
   TVirtualPad * canRatio = can->GetPad(2);
   canRatio->SetPad(0.01,0.01,0.99,0.32);
   canRatio->SetBottomMargin(0.16);
   canRatio->SetLeftMargin(0.115); 
   canRatio->SetRightMargin(0.03);  
   canRatio->SetGridx();
   canRatio->SetGridy();
   canRatio->cd();
  
   // Create the Ratio plot
   TH1* hRatio = (TH1*) tDa->Clone(tempName+"_Ratio");
   hRatio->SetTitle("#frac{Data - MC}{MC}");
   hRatio->Add(tMC,-1);
   hRatio->Divide(tMC);

   // Format the ratio plot and Draw it
   formatRatio(hRatio);
   hRatio->Draw();

   // Return the Canvas
   return can;
}

void FormattedPlot::formatColors(vector<PhysicsProcessNEW*> procs)
{
   for(unsigned int i = 0; i < procs.size(); i++)
   {
      histos[i]->SetLineColor(((ColoredPhysicsProcessNEW*)procs[i])->color);
      histos[i]->SetMarkerColor(((ColoredPhysicsProcessNEW*)procs[i])->color);
      histos[i]->SetFillColor(((ColoredPhysicsProcessNEW*)procs[i])->color);
      
      TString pname = procs[i]->name;
      pname.ToUpper();
      if (pname.Contains("DATA"))
      { 
         histos[i]->SetMarkerStyle(20);
         histos[i]->SetMarkerSize(0.7);
      }
   }
}

// ------------------------------------------------------------
void FormattedPlot::formatRatio(TH1* hRatio)
{
   hRatio->SetMinimum(-0.99);
   hRatio->SetMaximum(0.99);
   hRatio->SetLineWidth(2);

   hRatio->GetXaxis()->SetLabelFont(42);
   hRatio->GetXaxis()->SetLabelOffset(0.007);
   hRatio->GetXaxis()->SetLabelSize(0.11);
   hRatio->GetXaxis()->SetTitleSize(0.12);
   hRatio->GetXaxis()->SetTitleFont(42);
   hRatio->GetXaxis()->SetRangeUser(range.first,range.second);

   hRatio->GetYaxis()->SetTitle("#frac{Data - MC}{MC}");
   hRatio->GetYaxis()->SetTitleOffset(0.55);
   hRatio->GetYaxis()->SetNdivisions(105);
   hRatio->GetYaxis()->SetLabelFont(42);
   hRatio->GetYaxis()->SetLabelOffset(0.012);
   hRatio->GetYaxis()->SetLabelSize(0.11);
   hRatio->GetYaxis()->SetTitleSize(0.1);
   hRatio->GetYaxis()->SetTitleFont(42);

   hRatio->SetStats(0);
}

// ------------------------------------------------------------
void FormattedPlot::formatStack(THStack * stack, double maxi)
{
   for(unsigned int a=0; a<axisTitles.size(); a++)
   {
      if (a==0) // X axis
      {
         stack->GetXaxis()->SetTitleOffset(1.1);
         stack->GetXaxis()->SetTitle(axisTitles[a].c_str());
         stack->GetXaxis()->SetRangeUser(range.first,range.second);
         stack->GetXaxis()->SetLabelSize(0.06);
         stack->GetXaxis()->SetTitleSize(0.06);
         stack->GetXaxis()->SetLabelFont(42);
         stack->GetXaxis()->SetTitleFont(42);
      }
      else if (a==1) // Y axis
      {
         stack->GetYaxis()->SetTitleOffset(1.3);
         stack->GetYaxis()->SetTitle(axisTitles[a].c_str());
         stack->SetMaximum(maxi);
         stack->GetYaxis()->SetLabelSize(0.06);
         stack->GetYaxis()->SetTitleSize(0.06);
         stack->GetYaxis()->SetLabelFont(42);
         stack->GetYaxis()->SetTitleFont(42);
         stack->GetYaxis()->SetTitleOffset(0.85);
      }
   }
}

//________________________________________________________________________________
void drawKSandChi2Tests(TH1* totalData, TH1* all, pair<double, double> range)
{
   double x = (range.second - range.first)*0.45 + range.first;
   double y = max(totalData->GetMaximum(),all->GetMaximum())*1.1;

   double chi2;
   int NDF;
   int igood;

   TLatex * ks      = new TLatex(x, y     , Form("KSTest   = %5.4g", totalData->KolmogorovTest(all)));
   ks->SetName("ks");
   TLatex * chi2P   = new TLatex(x, y*0.92, Form("Chi2Prob = %5.4g", all->Chi2TestX(totalData,chi2,NDF,igood,"WW")));
   chi2P->SetName("chi2P");
   TLatex * chi2NDF = new TLatex(x, y*0.84, Form("Chi2/NDF = %5.4g", chi2/NDF));
   chi2NDF->SetName("chi2NDF");

   ks->Draw();
   chi2P->Draw();
   chi2NDF->Draw();
}

//______________________________________________________________________________
void drawLumi(float intLum)
{                                
   TLatex latex; 
   latex.SetNDC();                                         
   latex.SetTextSize(0.063);                                
   latex.SetTextAlign(31); // align right                  
   latex.DrawLatex(0.97,0.936,"#sqrt{s} = 8 TeV");
   if (intLum > 0.)
   {
      latex.SetTextAlign(11); // align right
      latex.SetTextSize(0.043);
      latex.DrawLatex(0.5,0.936,"#int ");
      latex.SetTextSize(0.063);
      latex.DrawLatex(0.525,0.936,Form("L dt = %3.2f fb^{-1}",intLum));
   }
   latex.SetTextSize(0.063);
   latex.SetTextAlign(11); // align left
   latex.DrawLatex(0.113,0.936,"CMS preliminary 2012");
}

