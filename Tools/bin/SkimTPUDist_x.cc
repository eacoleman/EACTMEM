//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TString.h"
#include "TMath.h"
#include "TError.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <stdlib.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
//  Local Functions
////////////////////////////////////////////////////////////////////////////////

///  fills the histograms and controls the output canvas and file for the rest of the program
vector<TH1D*> skimHistograms(vector<PhysicsProcess*> procs, Table& fileTable, string histogramName, bool verbose);

///  saves the resultant histograms to a user defined file
void saveHistograms(TString ofilename, vector<TH1D*> hists);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
 
   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string  histogramName = cl.getValue<string>  ("histogramName",               "PS/TPUDist");
   bool    verbose       = cl.getValue<bool>    ("verbose",                            false);
   TString ofilename     = cl.getValue<TString> ("ofilename",        "TPUDistributions.root");
   TString ofilepath     = cl.getValue<TString> ("ofilepath", "TAMUWW/ConfigFiles/Official/");

   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   // The vector holding all processes.
   vector <PhysicsProcess*> procs = DefaultValues::getProcessesHiggs(DEFS::jets2, DEFS::pretag, true, false, DEFS::EventNtuple);
      
   Table fileTable = DefaultValues::getFileLocationTable(DEFS::pretag);

   vector<TH1D*> hists = skimHistograms(procs, fileTable, histogramName, verbose);

   // The location of the table with the file locations
   string basePath = gSystem->pwd();
   basePath = basePath.substr(0,basePath.find("TAMUWW"));
   if(!TString(basePath).EndsWith("/")) basePath+="/";
   ofilepath = TString(basePath)+ofilepath;
   if(!ofilepath.EndsWith("/")) ofilepath+="/";

   saveHistograms(ofilepath+ofilename, hists);

   m_benchmark->Stop("event");
   cout << "SkimTPUDist_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
vector<TH1D*> skimHistograms(vector<PhysicsProcess*> procs, Table& fileTable, string histogramName, bool verbose) {

   vector<TH1D*> ret;

   for(unsigned int p=0; p<procs.size(); p++) {
      
      // find the file location for that process
      TableCellText * cellFile = (TableCellText *) fileTable.getCellRowColumn(string(procs[p]->name),"FilePath_"+DEFS::getNtupleTypeString(DEFS::EventNtuple));
      
      // make sure we found the cell
      if (cellFile == 0){
         cout<<"ERROR SkimTPUDist_x::main Table "<<fileTable.getTableOrigin()
             <<" does not have row "<<procs[p]->name
             <<" for column FilePath_" << DEFS::getNtupleTypeString(DEFS::EventNtuple) <<endl;
         cout<<" SKIPPING PROCESS "<<procs[p]->name<<endl;
         return ret;
      }

      TFile * file = new TFile(TString(cellFile->text),"READ");
      if (!file->IsOpen())
      {
         cout << "ERROR SkimTPUDist_x::skimHistograms() could not open file " << cellFile->text << endl;
         return ret;
      }

      /*
   if (!file->cd("PS"))
   {
      cout << "ERROR PhysicsProcess::PhysicsProcess() could not CD into directory PS in file " << fileName << endl;
      return;
   }
      */
      TH1D* temp = (TH1D*) file->Get(histogramName.c_str());

      if (temp == 0)
      {
         cout << "ERROR SkimTPUDist_x::skimHistograms() could not find histogram named " << histogramName << " in file " << cellFile->text << endl;
         if(procs[p]->name.Contains("QCD")!=0 && procs[p]->name.Contains("Data")!=0)
            assert(temp != 0);
      }
      else {
         ret.push_back((TH1D*)temp->Clone(TString("TPUDist_")+procs[p]->name));
         if(ret.back() == 0) {
            cout << "WARNING SkimTPUDist_x::skimHistograms() the clone of histogram named " << histogramName << " in file " << cellFile->text << " was a NULL pointer." << endl;
            ret.pop_back();
         }
         else {
            ret.back()->SetDirectory(0);
         }
      }

      //delete temp;
      file->Close();
      //delete file;
      
   }

   return ret;

}//skimHistograms

//______________________________________________________________________________
void saveHistograms(TString ofilename, vector<TH1D*> hists) {

   TFile * file = new TFile(ofilename,"RECREATE");
   if (!file->IsOpen())
   {
      cout << "ERROR SkimTPUDist_x::saveHistograms() could not open file " << ofilename << endl;
      return;
   }

   for(unsigned int h=0; h<hists.size(); h++) {
      hists[h]->Write();
   }

   file->Close();


}//saveHistograms


//Method to save QCD histos. To be implemented.
//QCD_ElEnriched
/*
TFile* fQCD = new TFile("/uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/QCD_Electron.root","READ")
TFile* fTPU = new TFile("TPUDistributions.root","UPDATE")
fQCD->cd("PS")
TH1D* TPUDist_QCD_ElEnriched = new TH1D("TPUDist_QCD_ElEnriched","TPUDist_QCD_ElEnriched",600,0,60)
jets2p->Draw("vLV[0].npv>>TPUDist_QCD_ElEnriched")
fTPU->cd()
TPUDist_QCD_ElEnriched->Write()
.q
*/
//QCD_ElFULL
/*
TFile* fQCD = new TFile("/uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/SingleEl_Full.root","READ")
TFile* fTPU = new TFile("TPUDistributions.root","UPDATE")
fQCD->cd("PS")
TH1D* TPUDist_QCD_ElFULL = new TH1D("TPUDist_QCD_ElFULL","TPUDist_QCD_ElFULL",600,0,60)
jets2p->Draw("vLV[0].npv>>TPUDist_QCD_ElFULL")
fTPU->cd()
TPUDist_QCD_ElFULL->Write()
.q
*/
