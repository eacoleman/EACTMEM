// Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/Tools/interface/PlotterAux.hh"

// ROOT libraries
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


//getProcesses()
vector <proc*> getProcesses(bool ele) {

  string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/";
  vector <proc*> procs;
  procs.push_back(new proc("WW",basePath+"WW_StandardCuts_outfile.root",43, 990, 122200));
  procs.push_back(new proc("WZ",basePath+"WZ_StandardCuts_outfile.root",45, 990, 123023));
  if (ele)
    procs.push_back(new proc("Data",basePath+"SingleEl_Data_StandardCuts_outfile.root",1 ,1, 1)); // 1's so we don't normalize data.
  else 
    procs.push_back(new proc("Data",basePath+"SingleMu_Data_StandardCuts_outfile.root",1 ,1, 1)); // 1's so we don't normalize data.

  return procs;

}// getProcesses




// getPlots(), the vector containing all plots to be made
map<string, aPlot> getPlots(){

  map<string, aPlot> plots;

  aPlot a;
  a.templateHisto = new TH1D("j0Px","j0Px",100,0,200);
  a.normToData = false;
  a.stacked = true;
  plots["j0Px"] = a;

  a.templateHisto = new TH1D("j1Px","j1Px",100,0,200);
  a.normToData = true;
  plots["j1Py"] = a;

  a.templateHisto = new TH1D("asd","asd",100,0,200);
  a.normToData = true;
  plots["asd"] = a;

  return plots;

}//getPlots


// fillPlots()

void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple){

  plots["j0Px"].Fill(ntuple->jLV[0].Px());
  plots["j1Py"].Fill(ntuple->jLV[1].Py());
  plots["asd"].Fill((ntuple->jLV[1]+ntuple->jLV[0]).M());

}

// This method fills all the plots for a given process
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

//plotter( ..., ... , etc)
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

} //plotter

// plotter()
void plotter() {

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
  
}
