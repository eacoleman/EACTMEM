// Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/Tools/interface/PlotterAux.hh"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TChain.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  Define Local Functions
////////////////////////////////////////////////////////////////////////////////

///  fills the histograms and controls the output canvas and file for the rest of the program
void plotter(string fileName, map<string, aPlot> & plots, vector<proc*> procs, string extraCuts);

/// returns the cross section for the given process
double getCrossSection(TString channelName);

/// returns the number of Monte Carlo events for the given process
double getNumMCEvts(TString channelName);

/// returns a vector containing all of the processes that the program will use
vector<proc*> getProcesses(bool ele);

/// returns a map containing all of the plots that will be made for each process and their specific attributes
map<string, aPlot> getPlots();

/// this function prepares the histograms of a given process to be filled
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, string extraCuts);

/// this function fills all of the plots for a given process
void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {

   // The vector containing all plots to be made
   map<string, aPlot> plots = getPlots();

   // The vector holding all processes.
   vector <proc*> procs_el = getProcesses(true);
  
   // Make all the plots and store to outputFile
   plotter("outputFile_el.root", plots, procs_el, "idet==0");
  
   // The vector holding all processes.
   vector <proc*> procs_mu = getProcesses(false);

   // Make all the plots and store to outputFile
   //plotter("outputFile_mu.root", plots, procs_mu, "idet==1");
  
   return 0;

}//plotter()


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void plotter(string fileName, map<string, aPlot> & plots, vector<proc*> procs, string extraCuts) {

   TFile * outFile = new TFile(fileName.c_str(),"RECREATE");

   // Loop over each process filling the histos in each plots
   for (unsigned int p = 0 ; p < procs.size() ; p++){
    
      fillPlotsForProcess(plots, procs[p], extraCuts);
    
   }//for 

   // Will all the info in the plots get the canvas and write it to file
   for ( map<string, aPlot>::iterator p = plots.begin(); p != plots.end() ; p++){
      TCanvas * can = p->second.getCanvas(procs);
      can->Write();
   }
  
   outFile->Close();

}//plotter(.., .., etc)

//______________________________________________________________________________
double getCrossSection(TString channelName)
{
   Table table;
   double xsec;

   table.parseFromFile(gSystem->pwd()+string("/../../ConfigFiles/Official/CrossSections.txt","TableCellVal"));
   TableCell * cell = table.getCellRowColumn(string(channelName),"CrossSection");
   if(cell){
      xsec = ((TableCellVal*)cell)->val.value;
      if (xsec==0)
         cout << "WARNING::getCrossSection::The cross section for " << channelName << " is 0.0 +/- 0.0" << endl;
      return xsec;
   } else{
      cout << "WARNING::getCrossSection::channelName not recognized. Returning -1 for the cross section." << endl << "The events will have the same scale as the MC sample, but on a negative scale." << endl << "Please check channel names." << endl;
      return -1.;
   }
}//getCrossSection


//______________________________________________________________________________
double getNumMCEvts(TString channelName)
{
   Table table;
   double value;

   table.parseFromFile(gSystem->pwd()+string("/../../ConfigFiles/Official/EventsFromMC.txt","TableCellVal"));
   TableCell * cell = table.getCellRowColumn(string(channelName),"Events");
   if(cell){
      value = ((TableCellVal*)cell)->val.value;
      if (value==0)
         cout << "WARNING::getNumMCEvts::There are 0 events in the " << channelName << " MC." << endl;
      return value;
   } else{
      cout << "WARNING::getNumMCEvts::channelName not recognized. Returning -1 event from MC." << endl << "Please check channel names." << endl;
      return -1.;
   }
}//getNumMCEvts


//______________________________________________________________________________
vector<proc*> getProcesses(bool ele){

   string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/";
   vector <proc*> procs;
   procs.push_back(new proc("WW",basePath+"WW_StandardCuts_outfile.root", getCrossSection("WW"), 990, getNumMCEvts("WW"),kRed));
   procs.push_back(new proc("WZ",basePath+"WZ_StandardCuts_outfile.root", getCrossSection("WZ"), 990, getNumMCEvts("WZ"),kRed+3));
   procs.push_back(new proc("WpJ",basePath+"WpJ_StandardCuts_outfile.root", getCrossSection("WpJ"), 990, getNumMCEvts("WpJ"),kGreen));
   procs.push_back(new proc("ZpJ",basePath+"ZpJ_StandardCuts_outfile.root", getCrossSection("ZpJ"), 990, getNumMCEvts("ZpJ"),kGreen+3));
   procs.push_back(new proc("TTbar",basePath+"TTbar_MG_StandardCuts_outfile.root", getCrossSection("TTbar"), 990, getNumMCEvts("TTbar"),kYellow));
   procs.push_back(new proc("STopTT",basePath+"STopT_T_StandardCuts_outfile.root", getCrossSection("STopTT"), 990, getNumMCEvts("STopTT"),kCyan));
   procs.push_back(new proc("STopTTbar",basePath+"STopT_Tbar_StandardCuts_outfile.root", getCrossSection("STopTTbar"), 990, getNumMCEvts("STopTTbar"),kCyan+3));
   procs.push_back(new proc("STopST",basePath+"STopS_T_StandardCuts_outfile.root", getCrossSection("STopST"), 990, getNumMCEvts("STopST"),kBlue));
   procs.push_back(new proc("STopSTbar",basePath+"STopS_Tbar_StandardCuts_outfile.root", getCrossSection("STopSTbar"), 990, getNumMCEvts("STopSTbar"),kBlue+3));
   procs.push_back(new proc("STopTWT",basePath+"STopTW_T_StandardCuts_outfile.root", getCrossSection("STopTWT"), 990, getNumMCEvts("STopTWT"),kMagenta));
   procs.push_back(new proc("STopTWTbar",basePath+"STopTW_Tbar_StandardCuts_outfile.root", getCrossSection("STopTWTbar"), 990, getNumMCEvts("STopTWTbar"),kMagenta+3));
   
   if (ele)
      procs.push_back(new proc("Data",basePath+"SingleEl_Data_StandardCuts_outfile.root",1 ,1, 1, kBlack)); // 1's so we don't normalize data.
   else 
      procs.push_back(new proc("Data",basePath+"SingleMu_Data_StandardCuts_outfile.root",1 ,1, 1, kBlack)); // 1's so we don't normalize data.
   
   return procs;
   
}// getProcesses


//______________________________________________________________________________
map<string, aPlot> getPlots(){

   map<string, aPlot> plots;

   aPlot a;

   a.templateHisto = new TH1D("Mjj","Mjj",200,0,2000);
   a.axisTitles.push_back("M_{jj} (per 10 GeV)");
   a.axisTitles.push_back("Events");
   a.normToData = true;
   a.stacked = true;
   plots["Mjj"] = a;
/*
  a.templateHisto = new TH1D("LeptPt","LeptPt",500,0,5000);
  a.normToData = true;
  a.stacked = true;
  plots["LeptPt"] = a;

  a.templateHisto = new TH1D("LeptEta","LeptEta",50,-5,5);
  a.normToData = true;
  a.stacked = true;
  plots["LeptEta"] = a;

  a.templateHisto = new TH1D("LeptPhi","LeptPhi",70,-3.5,3.5);
  a.normToData = true;
  a.stacked = true;
  plots["LeptPhi"] = a;

  a.templateHisto = new TH1D("MET","MET",500,0,5000);
  a.normToData = true;
  a.stacked = true;
  plots["MET"] = a;

  a.templateHisto = new TH1D("WmT","WmT",500,0,5000);
  a.normToData = true;
  a.stacked = true;
  plots["WmT"] = a;

  a.templateHisto = new TH1D("Jet1Pt","Jet1Pt",200,0,2000);
  a.normToData = true;
  a.stacked = true;
  plots["Jet1Pt"] = a;

  a.templateHisto = new TH1D("Jet1Eta","Jet1Eta",50,-5,5);
  a.normToData = true;
  a.stacked = true;
  plots["Jet1Eta"] = a;

  a.templateHisto = new TH1D("Jet1Phi","Jet1Phi",70,-3.5,3.5);
  a.normToData = true;
  a.stacked = true;
  plots["Jet1Phi"] = a;

  a.templateHisto = new TH1D("Jet2Pt","Jet2Pt",200,0,2000);
  a.normToData = true;
  a.stacked = true;
  plots["Jet2Pt"] = a;

  a.templateHisto = new TH1D("Jet2Eta","Jet2Eta",50,-5,5);
  a.normToData = true;
  a.stacked = true;
  plots["Jet2Eta"] = a;

  a.templateHisto = new TH1D("Jet2Phi","Jet2Phi",70,-3.5,3.5);
  a.normToData = true;
  a.stacked = true;
  plots["Jet2Phi"] = a;

  a.templateHisto = new TH1D("EtaJ1-EtaJ2","EtaJ1-EtaJ2",50,0,5);
  a.normToData = true;
  a.stacked = true;
  plots["EtaJ1-EtaJ2"] = a;

  a.templateHisto = new TH1D("Ptjj","Ptjj",500,0,5000);
  a.normToData = true;
  a.stacked = true;
  plots["Ptjj"] = a;

  a.templateHisto = new TH1D("j1Pt/Mjj","j1Pt/Mjj",500,0,5);
  a.normToData = true;
  a.stacked = true;
  plots["j1Pt/Mjj"] = a;

  a.templateHisto = new TH1D("j2Pt/Mjj","j2Pt/Mjj",500,0,5);
  a.normToData = true;
  a.stacked = true;
  plots["j2Pt/Mjj"] = a;

  a.templateHisto = new TH1D("Mlvjj","Mlvjj",500,0,5000);
  a.normToData = true;
  a.stacked = true;
  plots["Mlvjj"] = a;
*/

   return plots;

}//getPlots


//______________________________________________________________________________
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, string extraCuts){

   cout<<"\tDoing Process "<<proc->name<<endl;

   // Tell all plots to prepare for a filling 
   for (map<string, aPlot>::iterator p = plots.begin() ; p != plots.end() ; p++)
      p->second.prepareToFillProcess(proc->name);

   // Create the eventntuple and set the branch address
   EventNtuple * ntuple = new EventNtuple();
   TChain * c = proc->chain;
   c->SetBranchAddress("EvtNtuple",&ntuple);
  
   // Loop over events in the process
   for (unsigned ev = 0 ; ev < c->GetEntries() ; ev++){
 
      // Get the given entry
      c->GetEntry(ev);
    
      //report every now and then
      if ((ev % 10000)==0 )
         cout<<"\t\tevent "<<ev<<endl;
      
      // make sure the event pass the extraCuts
      //if (!passCuts(ntuple, extraCuts))
      //ontinue;

      // fill plots
      fillPlots(plots, ntuple);

   }//for events

}//fillPlotsForProcess


//______________________________________________________________________________
void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple){

   plots["Mjj"].Fill(ntuple->Mjj);
/*
  plots["LeptPt"].Fill(ntuple->lLV[0].Pt()); 
  plots["LeptEta"].Fill(ntuple->lLV[0].Eta());
  plots["LeptPhi"].Fill(ntuple->lLV[0].Phi());
  plots["MET"].Fill(ntuple->METLV[0].Et());
  plots["WmT"].Fill(sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Et(), 2) - pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) - pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2)));
  plots["Jet1Pt"].Fill(ntuple->jLV[0].Pt());
  plots["Jet1Eta"].Fill(ntuple->jLV[0].Eta());
  plots["Jet1Phi"].Fill(ntuple->jLV[0].Phi());
  plots["Jet2Pt"].Fill(ntuple->jLV[1].Pt());
  plots["Jet2Eta"].Fill(ntuple->jLV[1].Eta());
  plots["Jet2Phi"].Fill(ntuple->jLV[1].Phi());
  plots["EtaJ1-EtaJ2"].Fill(TMath::Abs(ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta()));
  plots["Ptjj"].Fill((ntuple->jLV[0] + ntuple->jLV[1]).Pt());
  plots["j1Pt/Mjj"].Fill(ntuple->jLV[0].Pt() / ntuple->Mjj);
  plots["j2Pt/Mjj"].Fill(ntuple->jLV[1].Pt() / ntuple->Mjj);
  plots["Mlvjj"].Fill((ntuple->jLV[0] + ntuple->jLV[1] + ntuple->lLV[0] + ntuple->METLV[0]).M());
*/

}//fillPlots
