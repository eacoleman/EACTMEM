// Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/Tools/interface/PlotterAux.hh"
#include "TAMUWW/Tools/interface/TriggerEfficiency.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"
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

using DEFS::LeptonCat;

////////////////////////////////////////////////////////////////////////////////
//  Define Local Functions
////////////////////////////////////////////////////////////////////////////////

///  fills the histograms and controls the output canvas and file for the rest of the program
void plotter(TString fileName, map<string, aPlot> & plots, vector<proc*> procs, LeptonCat leptCat, bool doJER, bool doTriggEff, bool doPUrewt);

/// returns the cross section for the given process
double getCrossSection(TString channelName);

/// returns the number of Monte Carlo events for the given process
double getNumMCEvts(TString channelName);

/// returns a vector containing all of the processes that the program will use
vector<proc*> getProcesses(DEFS::LeptonCat leptonCat, double intLum);

/// returns a vector containing all of the processes that the program will use
vector<proc*> getProcesses(vector<string>& processNames, double intLum);

/// returns a map containing all of the plots that will be made for each process and their specific attributes
map<string, aPlot> getPlots(string leptonCatString);

/// this function prepares the histograms of a given process to be filled
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, LeptonCat leptCat, bool doJER, bool doTriggEff, TriggerEfficiency * trgEff, bool doPUrewt);

/// this function fills all of the plots for a given process
void fillPlots(map<string, aPlot> &  plots, EventNtuple * ntuple, double weight = 1.0);

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

   vector<string> processNames      = cl.getVector<string> ("procs",         "");
   string         leptonCatString   = cl.getValue<string>  ("lep",       "both");
   double         intLum            = cl.getValue<double>  ("lum",       1600.0);
   bool           doJER             = cl.getValue<bool>    ("doJer",       true);
   bool           doTriggEff        = cl.getValue<bool>    ("doTriggEff", false);
   bool           doPUrewt          = cl.getValue<bool>    ("doPUrewt",    true);

   if (!cl.check()) return 0;
   cl.print();

   // The enum telling the program to run over electrons, muons, or both
   DEFS::LeptonCat leptonCat = DEFS::getLeptonCat(leptonCatString);
   
    
   // The vector containing all plots to be made
   map<string, aPlot> plots = getPlots(leptonCatString);

   cout << "Doing lepton category "<<DEFS::getLeptonCatString(leptonCat)<< endl;
   //cout << "Doing lepton category "<<DEFS::getLeptonCat(leptonCatString)<< endl;

   // The vector holding all processes.
   vector <proc*> procs;
   
   if (!processNames.empty())
     procs = getProcesses(processNames,intLum);
   else
     procs = getProcesses(leptonCat,intLum);
      
   // Make all the plots and store to outputFile
   TString outFileName = "outputFile_PU_";
   outFileName += DEFS::getLeptonCatString(leptonCat)+".root";
   plotter(outFileName, plots, procs, leptonCat, doJER, doTriggEff, doPUrewt);
   

   return 0;

}//plotter()


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void plotter(TString fileName, map<string, aPlot> & plots, vector<proc*> procs, DEFS::LeptonCat leptCat, bool doJER, bool doTrigEff, bool doPUrewt) {

   // Create the trigger efficiency object
  TriggerEfficiency * trgEff = new TriggerEfficiency();
   trgEff->createEfficiencies8TeV();

   // Loop over each process filling the histos in each plots
   for (unsigned int p = 0 ; p < procs.size() ; p++){
    
     fillPlotsForProcess(plots, procs[p], leptCat, doJER, doTrigEff, trgEff, doPUrewt);

      //make the table to go in the root file here
      Table numProcEvtsT;
    
   }//for 

   // Will all the info in the plots get the canvas and write it to file
   TFile * outFile = new TFile(fileName,"RECREATE");
   for ( map<string, aPlot>::iterator p = plots.begin(); p != plots.end() ; p++){

      TCanvas * can = p->second.getCanvas(procs);
      can->Write();			
     
      can->SaveAs(TString(can->GetName())+".png");
      can->SaveAs(TString(can->GetName())+".eps");
   }
  
   outFile->Close();

}//plotter(.., .., etc)

//______________________________________________________________________________
double getCrossSection(TString channelName)
{
   Table table;
   double xsec;

   string basePath = gSystem->pwd();
   basePath = basePath.substr(0,basePath.find("TAMUWW"));
   table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/CrossSections_8TeV.txt"),"TableCellVal");
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
   basePath = basePath.substr(0,basePath.find("TAMUWW"));
   table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/EventsFromMC_commonPATTuple_525.txt"),"TableCellVal");
   TableCell * cell =table.getCellRowColumn(string(channelName),"Events_PATtuple");
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
// leptonCan = The lepton category for which the processes are needed.
// intLum    = The integrated luminosity in pb-1 for this given lepton category 
vector<proc*> getProcesses(DEFS::LeptonCat leptonCat, double intLum){
   
  //  string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/";
  string basePath = "/uscms/home/aperloff/nobackup/PS_outfiles_20120621_NTUPLES/";
  vector <proc*> procs;

  procs.push_back(new proc("WJets",basePath+"WJets/PS.root", getCrossSection("WJets"),
		   intLum, getNumMCEvts("WJets"), getProcessColor("WJets")));
 
  procs.push_back(new proc("WW",basePath+"WW/PS.root", getCrossSection("WW"),
			     intLum, getNumMCEvts("WW"), getProcessColor("WW")));
  procs.push_back(new proc("WZ",basePath+"WZ/PS.root", getCrossSection("WZ"),
		   intLum, getNumMCEvts("WZ"), getProcessColor("WZ")));
  
  procs.push_back(new proc("DYJets",basePath+"DYJets/PS.root", getCrossSection("DYJets"),
			   intLum, getNumMCEvts("DYJets"), getProcessColor("DYJets")));
  procs.push_back(new proc("TTbar",basePath+"TTbar/PS.root", getCrossSection("TTbar"),
                           intLum, getNumMCEvts("TTbar"), getProcessColor("TTbar")));
  
  
  ///////////////   QCD new code
  //  procs.push_back(new proc("QCD (Mu)", basePath+"QCD_MuEnriched/PS.root", getCrossSection("QCD"),
  //		   intLum, getNumMCEvts("QCD_Pt20_MuEnriched"), getProcessColor("QCD_MuEnriched")));
  
  procs.push_back(new proc("STopT_T",basePath+"STopT_T/PS.root", getCrossSection("STopT_T"),
			   intLum, getNumMCEvts("STopT_T"), getProcessColor("STopT_T")));
  //procs.push_back(new proc("STopT_Tbar",basePath+"STopT_Tbar/PS.root",
  //			   getCrossSection("STopT_Tbar"), intLum, getNumMCEvts("STopT_Tbar"),
  //			   getProcessColor("STopT_Tbar")));
  procs.push_back(new proc("STopS_T",basePath+"STopS_T/PS.root", getCrossSection("STopS_T"),
                              intLum, getNumMCEvts("STopS_T"), getProcessColor("STopS_T")));
  procs.push_back(new proc("STopS_Tbar",basePath+"STopS_Tbar/PS.root",
			   getCrossSection("STopS_Tbar"), intLum, getNumMCEvts("STopS_Tbar"),
			   getProcessColor("STopS_Tbar")));
  procs.push_back(new proc("STopTW_T",basePath+"STopTW_T/PS.root",
			   getCrossSection("STopTW_T"), intLum, getNumMCEvts("STopTW_T"),
			   getProcessColor("STopTW_T")));
  procs.push_back(new proc("STopTW_Tbar",basePath+"STopTW_Tbar/PS.root", 
			   getCrossSection("STopTW_Tbar"), intLum, getNumMCEvts("STopTW_Tbar"),
			   getProcessColor("STopTW_Tbar")));
   

  // The Data
  // The normalization of the data is done in the same way as the MC. That is 
  // we multiply the histograms by getScaleFactor in class proc in PlotterAux.hh
  //  double getScaleFactor(){return sigma * intLum / initial_events;}
  // FOR DATA, to guarrantee the proper normalization, we set
  // sigma  = 1 / collected luminosity
  // initial_events = 1;

  if (leptonCat == DEFS::electron || leptonCat == DEFS::both)
    procs.push_back(new proc("Data (el)",basePath+"SingleEl_Data/PS.root",
			     1./1599, intLum, 1,
			     getProcessColor("SingleEl_Data"))); 

  if (leptonCat == DEFS::muon || leptonCat == DEFS::both)
    procs.push_back(new proc("Data (mu)",basePath+"SingleMu_Data/PS.root",
			     1./1606, intLum, 1,
			     getProcessColor("SingleMu_Data")));
  
  return procs;
  
}//getProcesses


//______________________________________________________________________________
vector<proc*> getProcesses(vector<string>& processNames, double intLum){
   
  //  string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/"; 
  string basePath = "/uscms/home/aperloff/nobackup/PS_outfiles_20120621_NTUPLES/"; //602 to 613
   vector <proc*> procs;

   for (unsigned int i=0; i<processNames.size(); i++){
      if (TString(processNames[i]).Contains("Data")==0)
         if (processNames[i].compare("TTbar")==0)
            procs.push_back(new proc(processNames[i],basePath+processNames[i]+"/PS.root",
                                     getCrossSection(processNames[i]), intLum, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
         else
            procs.push_back(new proc(processNames[i],basePath+processNames[i]+"/PS.root",
                                     getCrossSection(processNames[i]), intLum, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
      else
         procs.push_back(new proc(processNames[i],basePath+processNames[i]+"/PS.root",1 ,1, 1,
                                  getProcessColor(processNames[i]))); // 1's so we don't normalize data.
   }

   return procs;

}//getProcesses


//______________________________________________________________________________
map<string, aPlot> getPlots(string leptonCatString){

   map<string, aPlot> plots;

   aPlot a;
   bool norm_data = true;

   //Goes in the label and tells us whether we are looking at electrons or muons
   TString lep = TString(leptonCatString);

   //goes in the label and tells us what cuts we are applying
   string cut;
   if (lep.CompareTo("muon") == 0)
      cut = "_MET > 30, muPt > 25";
   else
      cut = "_MET > 35, elPt > 30";
   TString cuts = TString(cut);

   TString suffix = lep;


   a.templateHisto = new TH1D("Mjj_" + suffix,"Mjj_" + lep +  cuts,500,0,5000);
   a.axisTitles.push_back("M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,400.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("LeptPt_" + suffix,"LeptPt_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("p_{T}^{lepton} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,150.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["LeptPt"] = a;

   a.axisTitles.clear();
 
   a.templateHisto = new TH1D("LeptEta_" + suffix,"LeptEta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{lepton} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["LeptEta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("LeptPhi_" + suffix,"LeptPhi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{lepton} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["LeptPhi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("MET_" + suffix,"MET_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("Missing E_{T} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(30.,150.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["MET"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("WmT_" + suffix,"WmT_" + lep + cuts,1000,0,5000);
   a.axisTitles.push_back("M_{T}^{W} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(0.,150.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["WmT"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Pt_" + suffix,"Jet1Pt_" + lep + cuts,200,0,1000);
   a.axisTitles.push_back("p_{T}^{jet_{1}} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,200.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet1Pt"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Eta_" + suffix,"Jet1Eta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet1Eta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet1Phi_" + suffix,"Jet1Phi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet1Phi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Pt_" + suffix,"Jet2Pt_" + lep + cuts,200,0,1000);
   a.axisTitles.push_back("p_{T}^{jet_{2}} [GeV]");
   a.axisTitles.push_back("Number of Events / 5 GeV");
   a.range = make_pair(20.,100.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet2Pt"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Eta_" + suffix,"Jet2Eta_" + lep + cuts,50,-5,5);
   a.axisTitles.push_back("#eta^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(-3.,3.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet2Eta"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Jet2Phi_" + suffix,"Jet2Phi_" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi^{jet_{1}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-3.5,3.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Jet2Phi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaEtaJ1J2_" + suffix,"DeltaEtaJ1J2_" + lep + cuts,50,0,5);
   a.axisTitles.push_back("#eta^{jet_{1}} - #eta^{jet_{2}} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(0.,5.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["DeltaEtaJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Ptjj_" + suffix,"Ptjj_" + lep + cuts,100,0,1000);
   a.axisTitles.push_back("p_{T}^{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,250.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Ptjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("j1Pt_Mjj_" + suffix,"j1Pt_Mjj_" + lep + cuts,500,0,5);
   a.axisTitles.push_back("p_{T}^{jet_{1}}/M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 0.01 GeV");
   a.range = make_pair(0.,2.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["j1Pt_Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("j2Pt_Mjj_" + suffix,"j2Pt_Mjj_" + lep + cuts,500,0,5);
   a.axisTitles.push_back("p_{T}^{jet_{2}}/M_{jj} [GeV]");
   a.axisTitles.push_back("Number of Events / 0.01 GeV");
   a.range = make_pair(0.,1.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["j2Pt_Mjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("Mlvjj_" + suffix,"Mlvjj_" + lep + cuts,250,0,2500);
   a.axisTitles.push_back("M_{lvjj} [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(50.,800.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["Mlvjj"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaRLepMET_" + suffix,"DeltaRLepMET_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#DeltaR of lep and MET [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0.,7.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["DeltaRLepMET"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("EJ1EJ2_" + suffix,"EJ1EJ2_" + lep + cuts,500,0,5000);
   a.axisTitles.push_back("E_{J1} * E_{J2}  [GeV]");
   a.axisTitles.push_back("Number of Events / 10 GeV");
   a.range = make_pair(0.,5000.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["EJ1EJ2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("BetaJ1BetaJ2_" + suffix,"BetaJ1BetaJ2_" + lep + cuts,10000,0,10);
   a.axisTitles.push_back("#beta_{J1} * #beta_{J2} [GeV]");
   a.axisTitles.push_back("Number of Events / .01 GeV");
   a.range = make_pair(0.9,1.03);
   a.normToData = norm_data;
   a.stacked = true;
   plots["BetaJ1BetaJ2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaRJ1J2_" + suffix,"DeltaRJ1J2_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#DeltaR of J1 and J2 [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0.,7.);
   a.normToData = norm_data;
   a.stacked = true;
   plots["DeltaRJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("AngleJ1J2_" + suffix,"AngleJ1J2_" + lep + cuts,50,0,5);
   a.axisTitles.push_back("Angle between J1 and J2 [Radians]");
   a.axisTitles.push_back("Number of Events / 0.1 Radians");
   a.range = make_pair(-0.5,3.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["AngleJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("DeltaPhiJ1J2_" + suffix,"DeltaPhiJ1J2_" + lep + cuts,50,0,10);
   a.axisTitles.push_back("#phi_{J1} - #phi_{J2} [Radians]");
   a.axisTitles.push_back("Number of Events / 0.2 Radians");
   a.range = make_pair(0,6);
   a.normToData = norm_data;
   a.stacked = true;
   plots["DeltaPhiJ1J2"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("jjlvPhi_" + suffix,"jjlvPhi__" + lep + cuts,70,-3.5,3.5);
   a.axisTitles.push_back("#phi(jj) - #phi(lv)");
   a.axisTitles.push_back("Number of Events");
   a.range = make_pair(-3.5,3.5);
   a.normToData = norm_data;
   a.stacked = true;
   plots["jjlvPhi"] = a;

   a.axisTitles.clear();

   a.templateHisto = new TH1D("npv_" + suffix,"npv__" + lep + cuts,100,0,100);
   a.axisTitles.push_back("npv");
   a.axisTitles.push_back("Number of Events");
   a.range = make_pair(0,100);
   a.normToData = norm_data;
   a.stacked = true;
   plots["npv"] = a;

   // return all the plots to be made
   return plots;

}//getPlots

//______________________________________________________________________________
// Return true if the event pass the cuts imposed to the given lepton category
bool eventPassCuts(EventNtuple * ntuple, DEFS::LeptonCat leptCat){

  /*
  ntuple->leptonCat  |    req   | result
          muon       |   muon   | continue
          electron   |   muon   | false
          both       |   muon   | continue
          muon       | electron | false
          electron   | electron | continue
          both       | electron | continue
          muon       |   both   | continue
          electron   |   both   | continue
          both       |   both   | continue
  */
  // Kill onlye electron-muon combinations, nothing else
  if ( ntuple->leptonCat != leptCat && (leptCat != DEFS::both && ntuple->leptonCat != DEFS::both) )
    return false;
  
  /*
  if (ntuple->leptonCat == DEFS::muon)
    if ((ntuple->lLV[0].Pt()) <= 25.0 || (ntuple->METLV[0].Et()) <= 30.0)
      return false;
  
  if (ntuple->leptonCat == DEFS::electron) 
    if ((ntuple->lLV[0].Pt()) <= 30.0 || (ntuple->METLV[0].Et()) <= 35.0)
      return false;
  */

  if (ntuple->leptonCat == DEFS::muon)
    return ntuple->FNALcutsMuon();
  else if(ntuple->leptonCat == DEFS::electron)
    return ntuple->FNALcutsElectron();

  // return true otherwise
  return true;  

}// eventPassCuts

//______________________________________________________________________________
void fillPlotsForProcess(map<string, aPlot> & plots, proc* proc, DEFS::LeptonCat leptCat, bool doJER, bool doTriggEff, TriggerEfficiency * trgEff, bool doPUrewt){

   cout<<"\tDoing Process "<<proc->name<<endl;

   // Determine whether this is data or not
   TString auxName = proc->name;
   auxName.ToUpper();
   //cout << auxName << endl;
   bool procIsData = auxName.Contains("DATA") ?  true : false;

   // Tell all plots to prepare for a filling 
   for (map<string, aPlot>::iterator p = plots.begin() ; p != plots.end() ; p++)
      p->second.prepareToFillProcess(proc);

   // Create the eventntuple and set the branch address
   EventNtuple * ntuple = new EventNtuple();
   TChain * c = proc->chain;
   c->SetBranchAddress("EvtNtuple",&ntuple);

   //the counter for how many events pass the cuts in each process
   unsigned int numProcEvts = 0;
   double sumW = 1.;
   
   //define the weight variable and set the default as 1.0
   double weight = 1.0;


   // Loop over events in the process
   unsigned int nev = c->GetEntries();
   //      if (nev > 10000) nev = 10000;

   PUreweight *pureweight = 0;
   if (!procIsData){
     string dataname = string("/uscms/home/ssinjini/nobackup/CMSSW_5_2_5/src/pileup12_noTrig.root");
     string MCname   = string(TString("/uscms/home/aperloff/nobackup/PS_outfiles_20120621_NTUPLES/"+proc->name+"/PS.root"));
     pureweight = new PUreweight(dataname,MCname,"pileup_IsoMu24_eta2p1","PS/TPUDist");
   }

   //for (unsigned ev = 0 ; ev < 200000; ev++){
   for (unsigned ev = 0 ; ev < nev ; ev++){
     
     // Get the given entry
     c->GetEntry(ev);
     
     // Report every now and then
      if ((ev % 10000)==0 )
         cout<<"\t\tevent "<<ev<<endl;

      // Cut events here
      if(!eventPassCuts(ntuple, leptCat))
	continue;
      
      // Obtain the weight from the TriggerEfficiency here
      //reset the weight for each event, so the default will be 1.0
      weight = 1.0;
      
      // If the process is not data then apply the trigger efficiency
      if (!procIsData && doTriggEff){
       weight *=  trgEff->getWeight(ntuple->leptonCat, ntuple->lLV[0].Pt(), ntuple->lLV[0].Eta());
     }

      // PU reweighting if needed
      if (doPUrewt && !procIsData){
	weight *= pureweight->getWeight(ntuple->tnpus[1]);
      } 

      if((!procIsData) && doJER){
	ntuple->doJER();
      }
      // fill plots
      fillPlots(plots, ntuple, weight);
      
      // keep track of the total number of events & weights passing the cuts 
      sumW += weight;
      numProcEvts ++;

   }//for events

   cout<<"\tNumber of "<<proc->name<<" Events Passing the Cuts "<<numProcEvts<<" with weights ="<<sumW<<endl;
   

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
   
   plots["npv"].Fill(ntuple->npv,weight);
   plots["jjlvPhi"].Fill((ntuple->jLV[0] + ntuple->jLV[1]).Phi() - (ntuple->lLV[0] + ntuple->METLV[0]).Phi(),weight);
 

}//fillPlots


//______________________________________________________________________________
Color_t getProcessColor(TString channelName){

  if (channelName.CompareTo("WW") == 0)
    return kRed;
   else if (channelName.CompareTo("WZ") == 0)
     return kBlue;
   else if (channelName.CompareTo("WJets") == 0)
     return kGreen+1;
   else if (channelName.CompareTo("DYJets") == 0)
     return kMagenta;
  //   else if (channelName.CompareTo("TTbar")==0)
  //      return kCyan;
     else if (channelName.CompareTo("QCD_MuEnriched") == 0)
    return kCyan;  
  //right now these aren't being used because we are combining all the STop's in PlotterAux.cc
   else if (channelName.CompareTo("STopT_T") == 0)
     return kGreen;
   else if (channelName.CompareTo("STopT_Tbar") == 0)
     return kCyan+3;
   else if (channelName.CompareTo("STopS_T") == 0)
     return kBlue;
   else if (channelName.CompareTo("STopS_Tbar") == 0)
     return kBlue+3;
   else if (channelName.CompareTo("STopTW_T") == 0)
     return kMagenta;
   else if (channelName.CompareTo("STopTW_Tbar") == 0)
     return kGreen+3;
   else if (channelName.CompareTo("STopTW_Tbar") == 0)
     return kGreen+3;
   else if (channelName.CompareTo("TTbar") == 0)
     return kCyan; 
  
  //these are still being used
  
   else if (channelName.CompareTo("SingleEl_Data") == 0)
     return kBlack;
   else if (channelName.CompareTo("SingleMu_Data") == 0)
     return kBlack;
   else{
     cout << "WARNING Plotter::GetProcessColor() Unknown process name=|"<<channelName
	  <<"|. Returning process color as kYellow." << endl;
   }

  return kYellow;

}//getProcessColor
