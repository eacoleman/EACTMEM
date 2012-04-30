// Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/Tools/interface/PlotterAux.hh"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2D.h"
#include "TChain.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

enum trinary {el, mu, both};

////////////////////////////////////////////////////////////////////////////////
//  Define Local Functions
////////////////////////////////////////////////////////////////////////////////

///  fills the histograms and controls the output canvas and file for the rest of the program
void plotter(string fileName, map<string, aPlot> & plots, vector<proc*> procs, int leptCat);

/// returns the cross section for the given process
double getCrossSection(TString channelName);

/// returns the number of Monte Carlo events for the given process
double getNumMCEvts(TString channelName);

/// returns a vector containing all of the processes that the program will use
vector<proc*> getProcesses(bool el);

/// returns a vector containing all of the processes that the program will use
vector<proc*> getProcesses(vector<string>& processNames);

/// returns a map containing all of the plots that will be made for each process and their specific attributes
map<string, aPlot> getPlots(string leptonCatString);

/// this function prepares the histograms of a given process to be filled
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, int leptCat);

/// this function fills all of the plots for a given process
void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple, double weight = 1.0);

/// returns the lepton category to run over
trinary getLeptonCat(string leptonCatString);

/// returns the Color_t for a specific process
Color_t getProcessColor(TString channelName);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {

   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
  
   vector<string> processNames      = cl.getVector<string> ("processNames",            "");
   string         leptonCatString   = cl.getValue<string>  ("leptonCatString",     "both");

   if (!cl.check()) return 0;
   cl.print();
   int lepton = 0;

   // The enum telling the program to run over electrons, muons, or both
   trinary leptonCat = getLeptonCat(leptonCatString);

   // The vector containing all plots to be made
   map<string, aPlot> plots = getPlots(leptonCatString);

   if (leptonCat==el || leptonCat==both){
      cout << "Doing lepton category ELECTRONS" << endl;
      lepton = 2;

      // The vector holding all processes.
      vector <proc*> procs_el;

      if (!processNames.empty())
         procs_el = getProcesses(processNames);
      else
         procs_el = getProcesses(true);
      
      // Make all the plots and store to outputFile
      plotter("outputFile_el_diffScale.root", plots, procs_el, lepton);
   }

   else if (leptonCat==mu || leptonCat==both){
      cout << "Doing lepton category MUONS" << endl;
      lepton = 1;

      // The vector holding all processes.
      vector <proc*> procs_mu;
      if (!processNames.empty())
         procs_mu = getProcesses(processNames);
      else
         procs_mu = getProcesses(false);

      // Make all the plots and store to outputFile
      plotter("outputFile_mu_diffScale.root", plots, procs_mu, lepton);
   }
  
   return 0;

}//plotter()


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void plotter(string fileName, map<string, aPlot> & plots, vector<proc*> procs, int leptCat) {

   // Loop over each process filling the histos in each plots
   for (unsigned int p = 0 ; p < procs.size() ; p++){
    
      fillPlotsForProcess(plots, procs[p], leptCat);

      //make the table to go in the root file here
      Table numProcEvtsT;
    
   }//for 

   // Will all the info in the plots get the canvas and write it to file
   TFile * outFile = new TFile(fileName.c_str(),"RECREATE");
   for ( map<string, aPlot>::iterator p = plots.begin(); p != plots.end() ; p++){
      TH1 * histo = p->second.getHisto(procs);
      histo->Write();

      TCanvas * can = p->second.getCanvas(procs);
      can->Write();			
      can->SaveAs(TString(can->GetName())+".png");
   }
  
   outFile->Close();

}//plotter(.., .., etc)

//______________________________________________________________________________
double getCrossSection(TString channelName)
{
   Table table;
   double xsec;

   string basePath = gSystem->pwd();
   basePath = basePath.substr(0,basePath.find("CMSSW_4_2_8")+12);
   table.parseFromFile(basePath+string("src/TAMUWW/ConfigFiles/Official/CrossSections.txt"),"TableCellVal");
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

   string basePath = gSystem->pwd();
   basePath = basePath.substr(0,basePath.find("CMSSW_4_2_8")+12);
   table.parseFromFile(basePath+string("src/TAMUWW/ConfigFiles/Official/EventsFromMC.txt"),"TableCellVal");
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
vector<proc*> getProcesses(bool el){
   
   string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/";
   vector <proc*> procs;

   procs.push_back(new proc("WpJ",basePath+"WpJ_StandardCuts_outfile.root", getCrossSection("WpJ"),
                            990, getNumMCEvts("WpJ"), getProcessColor("WpJ")));
   procs.push_back(new proc("WW",basePath+"WW_StandardCuts_outfile.root", getCrossSection("WW"),
                            990, getNumMCEvts("WW"), getProcessColor("WW")));
   procs.push_back(new proc("WZ",basePath+"WZ_StandardCuts_outfile.root", getCrossSection("WZ"),
                            990, getNumMCEvts("WZ"), getProcessColor("WZ")));
   procs.push_back(new proc("ZpJ",basePath+"ZpJ_StandardCuts_outfile.root", getCrossSection("ZpJ"),
                            990, getNumMCEvts("ZpJ"), getProcessColor("ZpJ")));
   procs.push_back(new proc("TTbar",basePath+"TTbar_MG_StandardCuts_outfile.root", getCrossSection("TTbar"),
                            990, getNumMCEvts("TTbar"), getProcessColor("TTbar")));
   procs.push_back(new proc("STopT_T",basePath+"STopT_T_StandardCuts_outfile.root", getCrossSection("STopT_T"),
                            990, getNumMCEvts("STopT_T"), getProcessColor("STopT_T")));
   procs.push_back(new proc("STopT_Tbar",basePath+"STopT_Tbar_StandardCuts_outfile.root",
                            getCrossSection("STopT_Tbar"), 990, getNumMCEvts("STopT_Tbar"),
                            getProcessColor("STopT_Tbar")));
   procs.push_back(new proc("STopS_T",basePath+"STopS_T_StandardCuts_outfile.root", getCrossSection("STopS_T"),
                            990, getNumMCEvts("STopS_T"), getProcessColor("STopS_T")));
   procs.push_back(new proc("STopS_Tbar",basePath+"STopS_Tbar_StandardCuts_outfile.root",
                            getCrossSection("STopS_Tbar"), 990, getNumMCEvts("STopS_Tbar"),
                            getProcessColor("STopS_Tbar")));
   procs.push_back(new proc("STopTW_T",basePath+"STopTW_T_StandardCuts_outfile.root",
                            getCrossSection("STopTW_T"), 990, getNumMCEvts("STopTW_T"),
                            getProcessColor("STopTW_T")));
   procs.push_back(new proc("STopTW_Tbar",basePath+"STopTW_Tbar_StandardCuts_outfile.root", 
                            getCrossSection("STopTW_Tbar"), 990, getNumMCEvts("STopTW_Tbar"),
                            getProcessColor("STopTW_Tbar")));
   
   // 1's so we don't normalize data.
   if (el)
      procs.push_back(new proc("Data",basePath+"SingleEl_Data_StandardCuts_outfile.root",
                               getCrossSection("SingleEl_Data"), 1, getNumMCEvts("SingleEl_Data"),
                               getProcessColor("SingleEl_Data"))); 
   else 
      procs.push_back(new proc("Data",basePath+"SingleMu_Data_StandardCuts_outfile.root",
                               getCrossSection("SingleMu_Data"), 1, getNumMCEvts("SingleMu_Data"),
                               getProcessColor("SingleMu_Data")));
   
   return procs;
   
}//getProcesses


//______________________________________________________________________________
vector<proc*> getProcesses(vector<string>& processNames){
   
   string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/";
   vector <proc*> procs;

   for (unsigned int i=0; i<processNames.size(); i++){
      if (TString(processNames[i]).Contains("Data")==0)
         if (processNames[i].compare("TTbar")==0)
            procs.push_back(new proc(processNames[i],basePath+processNames[i]+"_MG_StandardCuts_outfile.root",
                                     getCrossSection(processNames[i]), 990, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
         else
            procs.push_back(new proc(processNames[i],basePath+processNames[i]+"_StandardCuts_outfile.root",
                                     getCrossSection(processNames[i]), 990, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
      else
         procs.push_back(new proc("Data",basePath+processNames[i]+"_StandardCuts_outfile.root",1 ,1, 1,
                                  getProcessColor(processNames[i]))); // 1's so we don't normalize data.
   }

   return procs;

}//getProcesses


//______________________________________________________________________________
map<string, aPlot> getPlots(string leptonCatString){

   map<string, aPlot> plots;

   aPlot a;

   //goes in the label and tells us whether we are looking at electrons or muons
   TString lep = TString(leptonCatString) + "_diffScale";

   //goes in the label and tells us what cuts we are applying
   string cut;
   if (lep.CompareTo("mu_diffScale") == 0)
      cut = "_MET > 30, muPt > 25";
   else
      cut = "_MET > 35, elPt > 30";
   TString cuts = TString(cut);

   a.templateHisto = new TH1D("Mjj_" + lep,"Mjj_" + lep +  cuts,500,0,5000);
   a.axisTitles.push_back("M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,400.);
   a.normToData = true;
   a.stacked = true;
   plots["Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("LeptPt_" + lep,"LeptPt_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("p_{T}^{lepton} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,150.);
   a.normToData = true;
   a.stacked = true;
   plots["LeptPt"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("LeptEta_" + lep,"LeptEta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{lepton} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = true;
   a.stacked = true;
   plots["LeptEta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("LeptPhi_" + lep,"LeptPhi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{lepton} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = true;
   a.stacked = true;
   plots["LeptPhi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("MET_" + lep,"MET_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("Missing E_{T} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(30.,150.);
   a.normToData = true;
   a.stacked = true;
   plots["MET"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("WmT_" + lep,"WmT_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("M_{T}^{W} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(0.,150.);
   a.normToData = true;
   a.stacked = true;
   plots["WmT"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Pt_" + lep,"Jet1Pt_" + lep + cuts,200,0,1000);
   a.axisTitles.push_back("p_{T}^{jet_{1}} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,200.);
   a.normToData = true;
   a.stacked = true;
   plots["Jet1Pt"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Eta_" + lep,"Jet1Eta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = true;
   a.stacked = true;
   plots["Jet1Eta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Phi_" + lep,"Jet1Phi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = true;
   a.stacked = true;
   plots["Jet1Phi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Pt_" + lep,"Jet2Pt_" + lep + cuts,200,0,1000);
   a.axisTitles.push_back("p_{T}^{jet_{2}} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,100.);
   a.normToData = true;
   a.stacked = true;
   plots["Jet2Pt"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Eta_" + lep,"Jet2Eta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = true;
   a.stacked = true;
   plots["Jet2Eta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Phi_" + lep,"Jet2Phi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = true;
   a.stacked = true;
   plots["Jet2Phi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaEtaJ1J2_" + lep,"DeltaEtaJ1J2_" + lep + cuts,50,0,5);
   a.axisTitles.push_back("#eta^{jet_{1}} - #eta^{jet_{2}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(0.,5.);
   a.normToData = true;
   a.stacked = true;
   plots["DeltaEtaJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Ptjj_" + lep,"Ptjj_" + lep + cuts,100,0,1000);
   a.axisTitles.push_back("p_{T}^{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,250.);
   a.normToData = true;
   a.stacked = true;
   plots["Ptjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("j1Pt_Mjj_" + lep,"j1Pt_Mjj_" + lep + cuts,500,0,5);
   a.axisTitles.push_back("p_{T}^{jet_{1}}/M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 0.01 GeV");
   a.range = make_pair(0.,2.);
   a.normToData = true;
   a.stacked = true;
   plots["j1Pt_Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("j2Pt_Mjj_" + lep,"j2Pt_Mjj_" + lep + cuts,500,0,5);
   a.axisTitles.push_back("p_{T}^{jet_{2}}/M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 0.01 GeV");
   a.range = make_pair(0.,1.5);
   a.normToData = true;
   a.stacked = true;
   plots["j2Pt_Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Mlvjj_" + lep,"Mlvjj_" + lep + cuts,250,0,2500);
   a.axisTitles.push_back("M_{lvjj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(50.,800.);
   a.normToData = true;
   a.stacked = true;
   plots["Mlvjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaRLepMET_" + lep,"DeltaRLepMET_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#DeltaR of lep and MET [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0.,7.);
   a.normToData = true;
   a.stacked = true;
   plots["DeltaRLepMET"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("EJ1EJ2_" + lep,"EJ1EJ2_" + lep + cuts,500,0,5000);
   a.axisTitles.push_back("E_{J1} * E_{J2}  [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,5000.);
   a.normToData = true;
   a.stacked = true;
   plots["EJ1EJ2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("BetaJ1BetaJ2_" + lep,"BetaJ1BetaJ2_" + lep + cuts,10000,0,10);
   a.axisTitles.push_back("#beta_{J1} * #beta_{J2} [GeV]");
   a.axisTitles.push_back("Number of Events / .01 GeV");
   a.range = make_pair(0.9,1.03);
   a.normToData = true;
   a.stacked = true;
   plots["BetaJ1BetaJ2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaRJ1J2_" + lep,"DeltaRJ1J2_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#DeltaR of J1 and J2 [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0.,7.);
   a.normToData = true;
   a.stacked = true;
   plots["DeltaRJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("AngleJ1J2_" + lep,"AngleJ1J2_" + lep + cuts,50,0,5);
   a.axisTitles.push_back("#theta between J1 and J2 [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-0.5,3.5);
   a.normToData = true;
   a.stacked = true;
   plots["AngleJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaPhiJ1J2_" + lep,"DeltaPhiJ1J2_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#phi_{J1} - #phi_{J2} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0,6);
   a.normToData = true;
   a.stacked = true;
   plots["DeltaPhiJ1J2"] = a;

   return plots;

}//getPlots


//______________________________________________________________________________
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, int leptCat){

   cout<<"\tDoing Process "<<proc->name<<endl;

   // Tell all plots to prepare for a filling 
   for (map<string, aPlot>::iterator p = plots.begin() ; p != plots.end() ; p++)
      p->second.prepareToFillProcess(proc->name);

   // Create the eventntuple and set the branch address
   EventNtuple * ntuple = new EventNtuple();
   TChain * c = proc->chain;
   c->SetBranchAddress("EvtNtuple",&ntuple);

   //the counter for how many events pass the cuts in each process
   int numProcEvts = 0;

   //define the weight variable and set the default as 1.0
   double weight = 1.0;

   //get the .root file that contains the trigger efficiency histograms for both electrons and muons
   TDirectory* currentDirectory = gDirectory;
   TFile* effFile = new TFile("efficiency1.root","READ");

   if (!effFile->IsOpen()) {
      cout << "\tWARNING::File efficiency1.root not found/opened!" << endl;
      return;
   }
   //define the histos for muons and electrons
   TH2D* muEffHist;
   TH2D* elEffHist;

   // Get histogram of trigger efficiencies for muons
   if(leptCat == 1){
      muEffHist = (TH2D*)gDirectory->Get("muEffs");
      if (!muEffHist) {
         cout << "\tWARNING::Muon efficiency histogram was not successfully retrieved from the file " << effFile->GetName() << "." << endl;
         return;
      } 
   }

   // Get histograms of trigger efficiencies for electrons
   if(leptCat == 2){
      elEffHist = (TH2D*)gDirectory->Get("elEffs");
      if (!elEffHist) {
         cout << "\tWARNING::Electron efficiency histogram was not successfully retrieved from the file " << effFile->GetName() << "." << endl;
         return;
      }
   }

   // Loop over events in the process
   for (unsigned ev = 0 ; ev < c->GetEntries() ; ev++){
 
      // Get the given entry
      c->GetEntry(ev);
    
      //report every now and then
      if ((ev % 10000)==0 )
         cout<<"\t\tevent "<<ev<<endl;

      // make sure the events pass the extraCuts
      if (ntuple->leptonCat != leptCat)
         continue;

      //reset the weight for each event, so the default will be 1.0
      weight = 1.0;

      //efficiencies and cuts for muons
      if(leptCat == 1){
         //ensure the efficiencies are only applied to the MC
         if (proc->name.compare("Data") != 0){
            if (ntuple->lLV[0].Pt() > 200 || abs(ntuple->lLV[0].Eta()) > 2.4){
               weight = 1.0;
            }
            else{
               weight = muEffHist->GetBinContent(muEffHist->FindBin(ntuple->lLV[0].Pt(),ntuple->lLV[0].Eta()));
            }
         }
         //cuts
         if ((ntuple->lLV[0].Pt()) <= 25.0 || (ntuple->METLV[0].Et()) <= 30.0)
            continue;
      }

      //cuts and efficiencies for electrons
      if(leptCat == 2){
         //ensure the efficiencies are only applied to the MC
         if (proc->name.compare("Data") != 0){
            if (ntuple->lLV[0].Pt() > 200 || abs(ntuple->lLV[0].Eta()) > 2.5)
               weight = 1.0;
            else
               weight = elEffHist->GetBinContent(elEffHist->FindBin(ntuple->lLV[0].Pt(),ntuple->lLV[0].Eta()));
         }
         //cuts
         if ((ntuple->lLV[0].Pt()) <= 30.0 || (ntuple->METLV[0].Et()) <= 35.0)
            continue;
      }

      //total number of events that pass the cuts for each process
      numProcEvts = ev;

      // fill plots
      fillPlots(plots, ntuple, weight);
      
   }//for events

   cout<<"\tNumber of "<<proc->name<<" Events Passing the Cuts "<<numProcEvts<<endl;
   
   effFile->Close();
   gDirectory = currentDirectory;

}//fillPlotsForProcess

//______________________________________________________________________________
void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple, double weight){

   plots["Mjj"].Fill(ntuple->Mjj,weight);
   plots["LeptPt"].Fill(ntuple->lLV[0].Pt(),weight);
   plots["LeptEta"].Fill(ntuple->lLV[0].Eta(),weight);
   plots["LeptPhi"].Fill(ntuple->lLV[0].Phi(),weight);
   plots["MET"].Fill(ntuple->METLV[0].Et(),weight);
   plots["WmT"].Fill(sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Et(), 2) - pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) - pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2)),weight);
   plots["Jet1Pt"].Fill(ntuple->jLV[0].Pt(),weight);
   plots["Jet1Eta"].Fill(ntuple->jLV[0].Eta(),weight);
   plots["Jet1Phi"].Fill(ntuple->jLV[0].Phi(),weight);
   plots["Jet2Pt"].Fill(ntuple->jLV[1].Pt(),weight);
   plots["Jet2Eta"].Fill(ntuple->jLV[1].Eta(),weight);
   plots["Jet2Phi"].Fill(ntuple->jLV[1].Phi(),weight);
   plots["DeltaEtaJ1J2"].Fill(TMath::Abs(ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta()),weight);
   plots["Ptjj"].Fill((ntuple->jLV[0] + ntuple->jLV[1]).Pt(),weight);
   plots["j1Pt_Mjj"].Fill(ntuple->jLV[0].Pt() / ntuple->Mjj,weight);
   plots["j2Pt_Mjj"].Fill(ntuple->jLV[1].Pt() / ntuple->Mjj,weight);
   plots["Mlvjj"].Fill((ntuple->jLV[0] + ntuple->jLV[1] + ntuple->lLV[0] + ntuple->METLV[0]).M(),weight);
   plots["DeltaRLepMET"].Fill(sqrt(pow(ntuple->lLV[0].Eta()-ntuple->METLV[0].Eta(),2)+pow(ntuple->lLV[0].Phi()-ntuple->METLV[0].Phi(),2)),weight);
   plots["EJ1EJ2"].Fill(ntuple->jLV[0].E() * ntuple->jLV[1].E(),weight);
   plots["BetaJ1BetaJ2"].Fill(ntuple->jLV[0].Beta() * ntuple->jLV[1].Beta(),weight);
   plots["DeltaRJ1J2"].Fill(sqrt(pow(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),2)+pow(ntuple->jLV[0].Phi()-ntuple->jLV[1].Phi(),2)),weight);
   plots["AngleJ1J2"].Fill(ntuple->jLV[0].Angle(ntuple->jLV[1].Vect()),weight);
   plots["DeltaPhiJ1J2"].Fill(abs(ntuple->jLV[0].Phi() - ntuple->jLV[1].Phi()),weight);

}//fillPlots


//______________________________________________________________________________
trinary getLeptonCat(string leptonCatString){
   if (leptonCatString.compare("el")==0)
      return el;
   else if (leptonCatString.compare("mu")==0)
      return mu;
   else if (leptonCatString.compare("both")==0)
      return both;
   else{
      cout << "WARNING::Unknown lepton category. The program will run over both electrons and muons." << endl;
      return both;
   }
}//getLeptonCat


//______________________________________________________________________________
Color_t getProcessColor(TString channelName){
   if (channelName.CompareTo("WW")==0)
      return kRed;
   else if (channelName.CompareTo("WZ")==0)
      return kBlue;
   else if (channelName.CompareTo("WpJ")==0)
      return kGreen+1;
   else if (channelName.CompareTo("ZpJ")==0)
      return kMagenta;
   else if (channelName.CompareTo("TTbar")==0)
      return kCyan;

   //right now these aren't being used because we are combining all the STop's in PlotterAux.cc
   else if (channelName.Contains("STopT_T")==0)
      return kGreen;
   else if (channelName.CompareTo("STopT_Tbar")==0)
      return kCyan+3;
   else if (channelName.CompareTo("STopS_T")==0)
      return kBlue;
   else if (channelName.CompareTo("STopS_Tbar")==0)
      return kBlue+3;
   else if (channelName.CompareTo("STopTW_T")==0)
      return kMagenta;
   else if (channelName.CompareTo("STopTW_Tbar")==0)
      return kGreen+3;

   //these are still being used
   else if (channelName.CompareTo("SingleEl_Data")==0)
      return kBlack;
   else if (channelName.CompareTo("SingleMu_Data")==0)
      return kBlack;
   else{
      cout << "WARNING::Unknown process name. Returning process color as kYellow." << endl;
      return kYellow;
   }
}//getProcessColor
