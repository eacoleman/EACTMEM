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

// ##################################################
// ################### PLOT CLASS ###################
// ##################################################

// Create a new histo here
void Plot::prepareToFillProcess(TString suffix, TString groupName)
{
   TString n = templateHisto->GetTitle();
   TH1 * clone = (TH1*) templateHisto->Clone(n+suffix);
   clone->SetTitle(groupName);
   clone->Sumw2();
   histos.push_back(clone);
}

// Fill the last histo here
void Plot::Fill(double x, double w){
   if (histos.size()>0) 
      histos.back()->Fill(x,w);
   else
      cout<<"ERROR in Plot named "<<templateHisto->GetName()
	  <<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}

// Fill the last histo here
void Plot::Fill(double x, double y, double w){
   if (histos.size()>0) 
      ((TH2*)histos.back())->Fill(x,y,w);
   else
      cout<<"ERROR in Plot named "<<templateHisto->GetName()
	  <<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}

// Fill the last histo here
void Plot::Fill(vector<Double_t> coord, double v, double w){
   if (histos.size()>0) 
      ((TProfileMDF*)histos.back())->Fill(coord,v,w);
   else
      cout<<"ERROR in Plot named "<<templateHisto->GetName()
	  <<" Fill(..) called without calling prepareToFillProcess first"<<endl; 
}

// Do the scaling to luminosity or data.
void Plot::scaleToData(vector<PhysicsProcess*> procs, DEFS::LeptonCat lepCat)
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
      //cout << "Normalizing to luminosity and cross section..." << endl;
      
      // This works for MC and data as well.
      histos[p]->Scale(procs[p]->getScaleFactor(lepCat));
      //cout << " histo="<<histos[p]->GetTitle()<<"\tscaleFactor="<<procs[p]->getScaleFactor(lepCat) << "\tleptonCat=" << DEFS::getLeptonCatString(lepCat) << endl;
      //cout<<" histo="<<histos[p]->GetTitle()<<", has Integral()="<<histos[p]->Integral()<<endl;

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
      //cout<<"Normalizing to data..."<<endl;
      
      if (totalData == 0)
      {
         cout<<"ERROR  Plot::scaleToData() integral of data is zero, cannot normalize to data."
             <<"WILL NOT NORMALIZE"<<endl;
         return;
      }
      
      //cout<<" totalData = "<<totalData<<"  totalMC = "<<totalMC<<endl;
      
      for (unsigned int p = 0 ; p < procs.size() ; p++)
      {
         // This works for MC and data as well.
         //histos[p]->Scale(procs[p]->getScaleFactor());
      
         TString hName = histos[p]->GetTitle();
         hName.ToUpper();
         if (!hName.Contains("DATA") )
            histos[p]->Scale(totalData/totalMC);
         
         //cout<<"AfterNorm histo="<<histos[p]->GetTitle()<<", has Integral()="<<histos[p]->Integral()<<endl; 
      }
    
   }
   
   //set this flag to true so we won't do the scaling again.
   scaled = true;
}

/* 
// ------------------------------------------------------------
// This groups all the histograms that need grouping.
// Typically all the Single tops and Dibosons
vector<TH1*> Plot::doGrouping(vector<PhysicsProcess*> procs)
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
   dibo->SetTitle("WW+WZ+ZZ");
   dibo->Sumw2();
   TH1 * higgs = (TH1*) templateHisto->Clone(TString(templateHisto->GetName())+"_Higgs");
   higgs->SetTitle("ggH+WH+VBF");
   higgs->Sumw2();
   
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
               hName.Contains("WZ") ||
               hName.Contains("ZZ") )
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
      else if (hName.Contains("ggH") || 
               hName.Contains("qqH") ||
               hName.Contains("WH")  )
      {
         // if first time set the attributes
         if (higgs->GetEntries()==0)
         {
            higgs->SetLineColor(histos[h]->GetLineColor());
            higgs->SetFillColor(histos[h]->GetFillColor());
            higgs->SetMarkerColor(histos[h]->GetMarkerColor());
         }

         // add to higgs
         higgs->Add(histos[h]);
      }
      else
      {
         // add as an independent process.
         groups.push_back(histos[h]);
      }
   }

   // Add the higgs, diboson, and single top if they were present
   if (stop->GetEntries() >0)  groups.insert(groups.begin(), stop);
   if (dibo->GetEntries() >0)  groups.insert(groups.begin(), dibo);
   if (higgs->GetEntries() >0)  groups.insert(groups.begin(), higgs);

   // return the groupedHistos
   return groups;

}
*/
 // This is wrong, someone needs to fix it;
 // ** the output->close before the ProfileMDF::WriteToFile call will 
 // not allow the regular histo to be save after a TProfileMDF has been saved
void Plot::saveHistogramsToFile(TString histoFile, TString option)
{
   TFile output(histoFile, option);
   for (unsigned int i = 0; i < histos.size(); i++){
     if(TString(histos[i]->ClassName()).Contains("TProfileMDF")==1) {
       output.Close();
       ((TProfileMDF*)histos[i])->WriteToFile(histoFile,"UPDATE");
     }
     else {
       histos[i]->Write();
     }
   }
   if(output.IsOpen())
     output.Close();
}

// ##################################################
// ############## FORMATTED PLOT CLASS ##############
// ##################################################

FormattedPlot::FormattedPlot()
{
   overlaySignalFactor = 1.0;
   overlaySignalName = "";
   logxy = make_pair(false,false);
}

// ------------------------------------------------------------
// Make the Canvas here ala Ricardo Eusebi
// ------------------------------------------------------------
TCanvas* FormattedPlot::getCanvas(vector<PhysicsProcess*> procs)
{
   // These flags are to determine if this FormattedPlot contains data or Monte Carlo histograms
   // With these flags, it is possible for the user to create canvases with only data or only Monte Carlo
   bool areMCHists = false;
   bool areDataHists = false;
   
   // Format the colors
   formatColors(procs);
   
   // Do the scaling of the histos to lum or to data
   scaleToData(procs,leptonCat);
  
   // Do the grouping of the histos and return the list of histograms to be plotted.
   // This takes care of putting all the processes with the same groupName together
   // typically SingleTop Histos together, or diboson together etc.
   std::vector<TH1*> groupedHistos = doGrouping(procs);
   //for(unsigned int i = 0; i < procs.size(); i++) {cout << "procs[i]->groupName = " << procs[i]->groupName << endl;}
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
   l->SetFillStyle(0); 
  
   // Create the totalHistos and Stacks for the MC and Data processes
   for (unsigned int h = 0 ; h < groupedHistos.size() ; h++)
   {
      TString hname = groupedHistos[h]->GetTitle();
      hname.ToUpper();
      //cout << "groupedHistos[h]->GetTitle() = " << hname << endl; 
      if (hname.Contains("DATA"))
      {
         sDa->Add(groupedHistos[h],"hist");
         tDa->Add(groupedHistos[h]);
         l->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"lpe");
         
         areDataHists = true;
      }
      else
      {
         sMC->Add(groupedHistos[h],"hist");
         tMC->Add(groupedHistos[h]);
         l->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"f");
         
         areMCHists = true;
      }
      
      // Check if this is the signal we want to overlay
      if (overlaySignalName.Contains(groupedHistos[h]->GetTitle())){
	
	//TString className = tMC->ClassName(); 
	//if (!className.Contains("TH2")){
	  
	  ostringstream signalNameStream;
	  signalNameStream <<  groupedHistos[h]->GetName() << "_x" << overlaySignalFactor;
	  signal = (TH1*)groupedHistos[h]->Clone(TString(signalNameStream.str()));
	  signal->SetFillColor(0);
     signal->SetLineColor(kRed-4);
	  signal->Scale(overlaySignalFactor);
          
	  ostringstream legendTitleStream;
	  legendTitleStream << signal->GetTitle() << "(x" << overlaySignalFactor << ")";
	  string legendTitle = legendTitleStream.str();
	  l->AddEntry(signal,legendTitle.c_str(),"lp0");

	  //}// skip TH2 for some reasons.

      } // if this is the signal histo
      
   }// for grouped histos
   
   // General Style Formatting
   gStyle->SetOptTitle(0);
   //gStyle->SetOptStat(2211);

   // Create and Draw the Canvas
   TCanvas * can = new TCanvas(templateHisto->GetTitle());
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
   TPad* padMain = (TPad*)canMain->GetPad(0);
   padMain->SetLogx(logxy.first);
   padMain->SetLogy(logxy.second);
   
   //Define the graphics option
   TString gOption = stacked ? "": "nostack";
   if (areDataHists)
      sMC->Draw(gOption);
   else
      sMC->Draw(gOption+"colz");
   
   if (areMCHists)
      sDa->Draw(gOption+"ep SAME");
   else
      sDa->Draw(gOption+"ep");
   //Draw the MC error bars
   if(tMC != 0)
   {
      tMC->SetFillColor(kGray+2);
      tMC->SetFillStyle(3005);
      tMC->Draw("E2 SAME");
   }
   
   //Draw the signal overlay
   if (signal != 0)
   {
      signal->SetLineWidth(2);
      signal->Draw("hist,SAME");
   }
   
   // Set display range
   if (areMCHists)
      sMC->GetXaxis()->SetRangeUser(range.first, range.second);
   if (areDataHists)
      sDa->GetXaxis()->SetRangeUser(range.first, range.second);
   if (signal != 0)
      signal->GetXaxis()->SetRangeUser(range.first, range.second);

   // Format the stacks, make sure to do this after the Draw command.
   // Set the maximum range for the Y-axis
   double maxi = max(sMC->GetMaximum(),sDa->GetMaximum())*1.1;
   if (areMCHists)
      formatStack(sMC,maxi);
   //formatStack(sDa,maxi);
   
   // Add the KS and Chi2 info in the active canvas
   if (areMCHists && areDataHists)
      drawKSandChi2Tests(tDa, tMC, range);

   // Add the Legend
   l->Draw("same");

   // Add the luminosity
   //drawLumi(3.6);
   drawLumi(procs[0]->intLum[leptonCat]/1000);

   // canRatio: this is the pad with the Data/MC on it
   TVirtualPad * canRatio = can->GetPad(2);
   canRatio->SetPad(0.01,0.01,0.99,0.32);
   canRatio->SetBottomMargin(0.16);
   canRatio->SetLeftMargin(0.115); 
   canRatio->SetRightMargin(0.03);  
   canRatio->SetGridx();
   canRatio->SetGridy();
   canRatio->cd();
   TPad* padRatio = (TPad*)canRatio->GetPad(0);
   padRatio->SetLogx(logxy.first);
   //padRatio->SetLogy(logxy.second);
  
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

// ------------------------------------------------------------
// Just like the getCanvas member function, but in the TDR style
TCanvas* FormattedPlot::getCanvasTDR(vector<PhysicsProcess*> procs) {
  Style* st = new Style();
  st->setTDRStyle();

  // These flags are to determine if this FormattedPlot contains data or Monte Carlo histograms
  // With these flags, it is possible for the user to create canvases with only data or only Monte Carlo
  bool areMCHists = false;
  bool areDataHists = false;

  // Format the colors
  formatColors(procs);

  // Do the scaling of the histos to lum or to data
  scaleToData(procs,leptonCat);

  // Do the grouping of the histos and return the list of histograms to be plotted.
  // This takes care of putting all the processes with the same groupName together
  // typically SingleTop Histos together, or diboson together etc.
  std::vector<TH1*> groupedHistos = doGrouping(procs);
  // Create the total Data and MC histos and Stacks
  TString tempName = templateHisto->GetName();
  TH1     * tMC = (TH1*) templateHisto->Clone(tempName+"_TotalMC");   tMC->Sumw2();
  TH1     * tDa = (TH1*) templateHisto->Clone(tempName+"_TotalData"); tDa->Sumw2();
  TH1     * signal = 0;
  THStack * sMC = new THStack(tempName+"_stackMC",tempName+"_stackMC");
  THStack * sDa = new THStack(tempName+"_stackData",tempName+"_stackData");

  // Make the legend
  TLegend * l = st->tdrLeg(0.35,0.64,0.55,0.89);
  l->SetName(tempName+"_legend");
  TLegend * lsig = st->tdrLeg(0.65,0.64,0.85,0.89);
  lsig->SetName(tempName+"_signal_legend");

  // Create the totalHistos and Stacks for the MC and Data processes
  for (unsigned int h = 0 ; h < groupedHistos.size() ; h++) {
    TString hname = groupedHistos[h]->GetTitle();
    hname.ToUpper();

    if (hname.Contains("DATA")) {
      sDa->Add(groupedHistos[h],"hist");
      tDa->Add(groupedHistos[h]);
      lsig->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"lpe");

      areDataHists = true;
    }
    else {
      sMC->Add(groupedHistos[h],"hist");
      tMC->Add(groupedHistos[h]);
      if(hname.Contains("H(125)"))
        lsig->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"f");
      else
        l->AddEntry(groupedHistos[h],groupedHistos[h]->GetTitle(),"f");

      areMCHists = true;
    }

    // Check if this is the signal we want to overlay
    if (overlaySignalName.Contains(groupedHistos[h]->GetTitle())){

      ostringstream signalNameStream;
      signalNameStream <<  groupedHistos[h]->GetName() << "_x" << overlaySignalFactor;
      signal = (TH1*)groupedHistos[h]->Clone(TString(signalNameStream.str()));
      signal->Scale(overlaySignalFactor);

      //ostringstream legendTitleStream;
      //legendTitleStream << signal->GetTitle() << "(x" << overlaySignalFactor << ")";
      //string legendTitle = legendTitleStream.str();
      //lsig->AddEntry(signal,legendTitle.c_str(),"lp0");
      //Form("%s(x%.0d)",signal->GetTitle(),overlaySignalFactor);
      lsig->AddEntry(signal,signal->GetTitle(),"lp0");
      lsig->AddEntry((TObject*)0,Form("(x%.0f)",overlaySignalFactor),"");

    } // if this is the signal histo
      
  }// for grouped histos
   
   // Create and Draw the Canvas
  TH1D* frame_up = new TH1D();
  frame_up->GetXaxis()->SetLimits(range.first, range.second);
  frame_up->GetXaxis()->SetMoreLogLabels();
  frame_up->GetXaxis()->SetNoExponent();
  frame_up->GetYaxis()->SetRangeUser(0.0001,max(tMC->GetBinContent(tMC->GetMaximumBin()),
                                     tDa->GetBinContent(tDa->GetMaximumBin()))*1.25);
  frame_up->GetXaxis()->SetTitle(axisTitles[0].c_str());
  frame_up->GetYaxis()->SetTitle(axisTitles[1].c_str());
  TH1D* frame_down = new TH1D();
  frame_down->GetXaxis()->SetLimits(range.first, range.second);
  frame_down->GetXaxis()->SetMoreLogLabels();
  frame_down->GetXaxis()->SetNoExponent();
  frame_down->GetYaxis()->SetRangeUser(-0.50,0.50);
  frame_down->GetXaxis()->SetTitle(axisTitles[0].c_str());
  frame_down->GetYaxis()->SetTitle("#frac{Data - MC}{MC}");
  if (signal != 0) {
    signal->GetXaxis()->SetRangeUser(range.first, range.second);
  }
  TCanvas * can = st->tdrDiCanvas(templateHisto->GetTitle(),frame_up,frame_down,2,11,procs[0]->intLum[leptonCat]/1000);
  can->cd(1);

  TPad* padMain = (TPad*)can->GetPad(0);
  padMain->SetLogx(logxy.first);
  padMain->SetLogy(logxy.second);

   //Define the graphics option
  string gOption = stacked ? "": "nostack";
  if (areDataHists)
    st->tdrDraw(sMC,gOption);
  else
    st->tdrDraw(sMC,gOption+"colz");

  if (areMCHists)
    st->tdrDraw(sDa,gOption+"ep");
  else
    st->tdrDraw(sDa,gOption+"ep");
  //Draw the MC error bars
  if(tMC != 0)
    st->tdrDraw(tMC,"E2",kNone,kGray+2,kNone,kGray+2,3005,kGray+2);

  //Draw the signal overlay
  if (signal != 0) {
    signal->SetLineWidth(2);
    st->tdrDraw(signal,"hist",kNone,kRed-4,kSolid,kRed-4,kNone,0);
  }

  // Add the KS and Chi2 info in the active canvas
  //if (areMCHists && areDataHists)
  //  drawKSandChi2Tests(tDa, tMC, range);

  // Add the Legend
  l->Draw("same");
  lsig->Draw("same");

  // canRatio: this is the pad with the Data/MC on it
  TVirtualPad * canRatio = can->GetPad(2);
  canRatio->SetGridx();
  canRatio->SetGridy();
  canRatio->cd();
  TPad* padRatio = (TPad*)canRatio->GetPad(0);
  padRatio->SetLogx(logxy.first);
  st->tdrGrid(true);
  
  // Create the Ratio plot
  TH1* hRatio = (TH1*) tDa->Clone(tempName+"_Ratio");
  hRatio->SetTitle("#frac{Data - MC}{MC}");
  hRatio->Add(tMC,-1);
  hRatio->Divide(tMC);

  // Format the ratio plot and Draw it
  st->tdrDraw(hRatio,"");

  delete st;

  // Return the Canvas
  return can;
}

// ------------------------------------------------------------
// Find the first histo in groupedHistos that matches the title,
// return a null pointer if it can't find one
TH1 * FormattedPlot::findTitleInTH1Vector(TString title, vector<TH1*> groupedHistos) {
  
   // Loop over original histos
  for (unsigned int h=0; h < groupedHistos.size(); h ++)
    if (title.EqualTo(groupedHistos[h]->GetTitle())) 
      return groupedHistos[h];

    return 0;

}//findTitleInTH1Vector
 

// ------------------------------------------------------------
// This groups all the histograms with the same title together
// Since the titles are given by process->groupName it groups all processes
// with the same groupNames together
vector<TH1*> FormattedPlot::doGrouping(vector<PhysicsProcess*> procs){

  // The returning vector
  vector<TH1*> groupedHistos;
  
  // Loop over original histos
  for (unsigned int h=0; h < histos.size(); h ++){
    //cout << "\thistos[h]->GetName() = " << histos[h]->GetName() << endl;
    //cout << "\thistos[h]->GetTitle() = " << histos[h]->GetTitle() << endl;
    // Find the histo in groupedHistos that matches the title, return null pointer if it can't.
    TH1 * hgroup = findTitleInTH1Vector(histos[h]->GetTitle(),groupedHistos);
    
    // if it does not exist, add one to the groupedHistos
    if (hgroup==0){
      TString cloneName = histos[h]->GetName();
      cloneName += "_clone";
      groupedHistos.push_back((TH1*) histos[h]->Clone(cloneName));
    }
    // if it does exist just add this to the already existing one
    else 
      hgroup->Add(histos[h]);
  }

  return groupedHistos;

}//doGrouping

// ------------------------------------------------------------
void FormattedPlot::formatColors(vector<PhysicsProcess*> procs)
{
   for(unsigned int i = 0; i < procs.size(); i++)
   {
      //histos[i]->SetLineColor(((PlotterPhysicsProcess*)procs[i])->color);
      histos[i]->SetLineColor(kBlack);
      histos[i]->SetMarkerColor(((PlotterPhysicsProcess*)procs[i])->color);
      histos[i]->SetFillColor(((PlotterPhysicsProcess*)procs[i])->color);
      
      TString pname = procs[i]->name;
      pname.ToUpper();

      if(!pname.Contains("DATA"))
      {
         histos[i]->SetFillStyle(1001);   
      }
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
   hRatio->SetMinimum(-0.50); //formerly -0.99
   hRatio->SetMaximum(0.50); //formerly -0.99
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
void FormattedPlot::drawKSandChi2Tests(TH1* totalData, TH1* all, pair<double, double> range){

    // Skip all this if either histo has no integral
    if (totalData->Integral() == 0){
      cout<<"\tWARNING in Plots::drawKSandChi2Tests() not drawing KS or chi2 because  "
	  <<totalData->GetName()<<" has zero integral"<<endl;
      return ;
    }
    // Skip all this if either histo has no integral
    if (all->Integral() == 0){
      cout<<"\tWARNING in Plots::drawKSandChi2Tests() not drawing KS or chi2 because  "
	  <<all->GetName()<<" has zero integral"<<endl;
      return ;
    }
  
   double x = (range.second - range.first)*0.45 + range.first;
   double y = max(totalData->GetMaximum(),all->GetMaximum())*1.1;

   double chi2;
   int NDF;
   int igood;

/*   
   TLatex * ks      = new TLatex(x, y     , Form("KSTest   = %5.4g", totalData->KolmogorovTest(all)));
   ks->SetName("ks");
   TLatex * chi2P   = new TLatex(x, y*0.92, Form("Chi2Prob = %5.4g", all->Chi2TestX(totalData,chi2,NDF,igood,"WW")));
   chi2P->SetName("chi2P");
   TLatex * chi2NDF = new TLatex(x, y*0.84, Form("Chi2/NDF = %5.4g", chi2/NDF));
   chi2NDF->SetName("chi2NDF");
   
   ks->Draw();
   chi2P->Draw();
   chi2NDF->Draw();
*/
   TLatex Tl;
   Tl.DrawLatex(x, y*1.00, Form("KSTest   = %5.4g", totalData->KolmogorovTest(all)));
   Tl.DrawLatex(x, y*0.92, Form("Chi2Prob = %5.4g", all->Chi2TestX(totalData,chi2,NDF,igood,"WW")));
   Tl.DrawLatex(x, y*0.84, Form("Chi2/NDF = %5.4g", chi2/NDF));

}

//______________________________________________________________________________
void FormattedPlot::drawLumi(float intLum)
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

