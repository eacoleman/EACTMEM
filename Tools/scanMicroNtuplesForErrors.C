#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TMath.h"

#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

#include <vector>
#include <iostream>
#include <utility>
#include <map>

using namespace std;

void scanMicroNtuplesForErrors(TString filename) {
   TFile* f = new TFile(filename,"READ");
   TTree* t = (TTree*)f->Get("METree");
   MicroNtuple* mnt = new MicroNtuple(2);
   t->SetBranchAddress("mnt",&mnt);
   EventNtuple* ntuple = new EventNtuple();
   t->SetBranchAddress("EvtTree",&ntuple);
   t->GetEntry(0);
   vector<TString> mem;
   mem.push_back("BDT");
   map<TString,double> evMap;
   mnt->setMVAReader(mem,"/uscms_data/d2/aperloff/Summer12ME8TeV/2014_09_16_TMVA_output_both_ggH_WJets_all/TMVAClassification_BDT.weights.xml");

   for(unsigned int i=0; i<t->GetEntries(); i++) {
      if(i%100000 == 0) cout << "Doing entry " << i << " ... " << endl;
      t->GetEntry(i);
      evMap["run"] = ntuple->run;
      evMap["lumi"] = ntuple->lumi;
      evMap["event"] = ntuple->event;
      mnt->setExtraVarMVA(evMap);
      double mvao = mnt->getMVAOutput("BDT","response"); 
      int evt = mnt->event; 
      if(mvao<=-0.195 && mvao>=-0.196) {
      //if(mvao>-0.195 || mvao<-0.196) {
         //cout << "\tEvent = " << mnt->event << "\tdisc = " << mvao << endl;
         for(int i=0; i<13; i++) {
            //cout << "\t\ttEventProb" << i << " = " << mnt->eventProbMVA[i] << endl;
            assert(TMath::Abs(mnt->eventProbMVA[i])!=TMath::Infinity());
         }
         //cout << "\t\ttEventProb19" << " = " << mnt->eventProbMVA[19] << endl;
         assert(TMath::Abs(mnt->eventProbMVA[19])!=TMath::Infinity());
         //cout << "\t\ttEventProb54" << " = " << mnt->eventProbMVA[54] << endl;
         assert(TMath::Abs(mnt->eventProbMVA[54])!=TMath::Infinity());
         //cout << "event = " << evt << "\tdisc = " << mvao << endl;
      }
   }
}
