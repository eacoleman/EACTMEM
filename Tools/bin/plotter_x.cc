//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/Tools/interface/Plots.hh"
#include "TAMUWW/Tools/interface/PlotFiller.hh"
#include "TAMUWW/Tools/interface/PhysicsProcessNEW.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
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
#include <utility>

using namespace std;
using DEFS::LeptonCat;

namespace UserFunctions
{
   TString outDir;
   TString cutRegion;
   DEFS::LeptonCat leptonCat;
   PUreweight* puweight;
   bool doJER;
   bool doPUreweight;
   bool doFNAL;
   bool WJweight;
   TH2D* wJetsWeight= 0;
   bool verbose; // adds or takes away cout statements when running
   
   // Used if we run both leptons
   bool doElectron;
   bool doMuon;

   // Is run once for each process before events are cut (initialize)
   void initEventFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc);
   // this function fills all of the plots for a given process
   void fillPlots(map<string, Plot*> &  plots, EventNtuple * ntuple, double weight = 1.0);
   // returns a boolean if the event passes the specified cuts
   bool eventPassCuts(EventNtuple * ntuple, const PhysicsProcessNEW*);
   // returns a double
   double weightFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc);
   // Is run once for each process before events (initializes PU Reweighting
   void processFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc);
   // returns a number based on if the particle is a lepton or quark based on its pdgId
   int leptonOrQuark(int x);
   // returns the charge of a particle based on its pdgId
   double charge(int x);
   // returns the mass of the hadronic W and the leptonic W
   pair<double,double> onVsOffShell(EventNtuple * ntuple);
   pair<double,double> onVsOffShellInclusive(EventNtuple * ntuple);
   // returns a number rounded away from zero
   double round(double r);
   int round_int(double r);
}

//______________________________________________________________________________
void UserFunctions::fillPlots(map<string, Plot*> &  plots, EventNtuple * ntuple, double weight)
{
   //-----------------ELECTRON--------------------//
   if (ntuple->leptonCat == DEFS::electron && (leptonCat == DEFS::electron || leptonCat == DEFS::both))
   {
      double Mjj = (ntuple->jLV[0] + ntuple->jLV[1]).M();
      double WmT = sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Et(), 2) - pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) - pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2));
      pair<double,double> leptVsHadWMass = onVsOffShellInclusive(ntuple);
      
      if(!doElectron)
         return;
      
      plots["Mjj_electron"]->Fill(Mjj,weight);
      plots["LeptPt_electron"]->Fill(ntuple->lLV[0].Pt(),weight);
      plots["LeptEta_electron"]->Fill(ntuple->lLV[0].Eta(),weight);
      plots["LeptPhi_electron"]->Fill(ntuple->lLV[0].Phi(),weight);
      plots["MET_electron"]->Fill(ntuple->METLV[0].Et(),weight);
      plots["WmT_electron"]->Fill(WmT, weight);
      plots["MjjmWmT_electron"]->Fill(Mjj - WmT, weight);
      plots["Jet1Pt_electron"]->Fill(ntuple->jLV[0].Pt(),weight);
      plots["Jet1Eta_electron"]->Fill(ntuple->jLV[0].Eta(),weight);
      plots["Jet1Phi_electron"]->Fill(ntuple->jLV[0].Phi(),weight);
      plots["Jet2Pt_electron"]->Fill(ntuple->jLV[1].Pt(),weight);
      plots["Jet2Eta_electron"]->Fill(ntuple->jLV[1].Eta(),weight);
      plots["Jet2Phi_electron"]->Fill(ntuple->jLV[1].Phi(),weight);
      plots["DeltaEtaJ1J2_electron"]->Fill(TMath::Abs(ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta()),weight);
      plots["Ptjj_electron"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Pt(),weight);
      plots["j1Pt_Mjj_electron"]->Fill(ntuple->jLV[0].Pt() / ntuple->Mjj,weight);
      plots["j2Pt_Mjj_electron"]->Fill(ntuple->jLV[1].Pt() / ntuple->Mjj,weight);
      plots["Mlvjj_electron"]->Fill((ntuple->jLV[0] + ntuple->jLV[1] + ntuple->lLV[0] + ntuple->METLV[0]).M(),weight);
      plots["DeltaRLepMET_electron"]->Fill(sqrt(pow(ntuple->lLV[0].Eta()-ntuple->METLV[0].Eta(),2)+pow(ntuple->lLV[0].Phi()-ntuple->METLV[0].Phi(),2)),weight);
      plots["EJ1EJ2_electron"]->Fill(ntuple->jLV[0].E() * ntuple->jLV[1].E(),weight);
      plots["BetaJ1BetaJ2_electron"]->Fill(ntuple->jLV[0].Beta() * ntuple->jLV[1].Beta(),weight);
      plots["DeltaRJ1J2_electron"]->Fill(sqrt(pow(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),2)+pow(ntuple->jLV[0].Phi()-ntuple->jLV[1].Phi(),2)),weight);
      plots["AngleJ1J2_electron"]->Fill(ntuple->jLV[0].Angle(ntuple->jLV[1].Vect()),weight);
      plots["DeltaPhi_LJ1_electron"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]),weight);
      plots["DeltaPhi_J1J2_electron"]->Fill(ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);
      plots["DeltaPhi_LJ1_vs_J1J2_electron"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);
      plots["npv_electron"]->Fill(ntuple->npv,weight);
      plots["jjlvPhi_electron"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Phi() - (ntuple->lLV[0] + ntuple->METLV[0]).Phi(),weight);
      plots["MWjjVsMWlv_electron"]->Fill(leptVsHadWMass.first,leptVsHadWMass.second,weight);
      if (ntuple->lQ == 1){
         plots["DeltaPhi_LJ1_vs_J1J2_Positive_electron"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots["WmT_Positive_electron"]->Fill(WmT, weight);
      }
      if (ntuple->lQ == -1){
         plots["DeltaPhi_LJ1_vs_J1J2_Negative_electron"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots["WmT_Negative_electron"]->Fill(WmT, weight);
      }
      plots["WmT_Subtracted_electron"]->Fill(WmT, (ntuple->lQ)*weight);
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_electron"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), (ntuple->lQ)*weight);
   }
   // -------------MUON-------------------//
   else if (ntuple->leptonCat == DEFS::muon && (leptonCat == DEFS::muon || leptonCat == DEFS::both))
   {
      double Mjj = (ntuple->jLV[0] + ntuple->jLV[1]).M();
      double WmT = sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Et(), 2) - pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) - pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2));
      pair<double,double> leptVsHadWMass = onVsOffShellInclusive(ntuple);
      
      if(!doMuon)
         return;
      
      plots["Mjj_muon"]->Fill(Mjj,weight);
      plots["LeptPt_muon"]->Fill(ntuple->lLV[0].Pt(),weight);
      plots["LeptEta_muon"]->Fill(ntuple->lLV[0].Eta(),weight);
      plots["LeptPhi_muon"]->Fill(ntuple->lLV[0].Phi(),weight);
      plots["MET_muon"]->Fill(ntuple->METLV[0].Et(),weight);
      plots["WmT_muon"]->Fill(WmT, weight);
      plots["MjjmWmT_muon"]->Fill(Mjj - WmT, weight);
      plots["Jet1Pt_muon"]->Fill(ntuple->jLV[0].Pt(),weight);
      plots["Jet1Eta_muon"]->Fill(ntuple->jLV[0].Eta(),weight);
      plots["Jet1Phi_muon"]->Fill(ntuple->jLV[0].Phi(),weight);
      plots["Jet2Pt_muon"]->Fill(ntuple->jLV[1].Pt(),weight);
      plots["Jet2Eta_muon"]->Fill(ntuple->jLV[1].Eta(),weight);
      plots["Jet2Phi_muon"]->Fill(ntuple->jLV[1].Phi(),weight);
      plots["DeltaEtaJ1J2_muon"]->Fill(TMath::Abs(ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta()),weight);
      plots["Ptjj_muon"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Pt(),weight);
      plots["j1Pt_Mjj_muon"]->Fill(ntuple->jLV[0].Pt() / ntuple->Mjj,weight);
      plots["j2Pt_Mjj_muon"]->Fill(ntuple->jLV[1].Pt() / ntuple->Mjj,weight);
      plots["Mlvjj_muon"]->Fill((ntuple->jLV[0] + ntuple->jLV[1] + ntuple->lLV[0] + ntuple->METLV[0]).M(),weight);
      plots["DeltaRLepMET_muon"]->Fill(sqrt(pow(ntuple->lLV[0].Eta()-ntuple->METLV[0].Eta(),2)+pow(ntuple->lLV[0].Phi()-ntuple->METLV[0].Phi(),2)),weight);
      plots["EJ1EJ2_muon"]->Fill(ntuple->jLV[0].E() * ntuple->jLV[1].E(),weight);
      plots["BetaJ1BetaJ2_muon"]->Fill(ntuple->jLV[0].Beta() * ntuple->jLV[1].Beta(),weight);
      plots["DeltaRJ1J2_muon"]->Fill(sqrt(pow(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),2)+pow(ntuple->jLV[0].Phi()-ntuple->jLV[1].Phi(),2)),weight);
      plots["AngleJ1J2_muon"]->Fill(ntuple->jLV[0].Angle(ntuple->jLV[1].Vect()),weight);
      plots["DeltaPhi_LJ1_muon"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]),weight);
      plots["DeltaPhi_J1J2_muon"]->Fill(ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);   
      plots["DeltaPhi_LJ1_vs_J1J2_muon"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);
      plots["npv_muon"]->Fill(ntuple->npv,weight);
      plots["jjlvPhi_muon"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Phi() - (ntuple->lLV[0] + ntuple->METLV[0]).Phi(),weight);
      plots["MWjjVsMWlv_muon"]->Fill(leptVsHadWMass.first,leptVsHadWMass.second,weight);
      if (ntuple->lQ == 1){
         plots["DeltaPhi_LJ1_vs_J1J2_Positive_muon"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots["WmT_Positive_muon"]->Fill(WmT, weight);
      }
      if (ntuple->lQ == -1){
         plots["DeltaPhi_LJ1_vs_J1J2_Negative_muon"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots["WmT_Negative_muon"]->Fill(WmT, weight);
      }
      plots["WmT_Subtracted_muon"]->Fill(WmT, (ntuple->lQ)*weight);
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_muon"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), (ntuple->lQ)*weight);
   }
}//fillPlots

//______________________________________________________________________________
// Return true if the event pass the cuts imposed to the given lepton category
bool UserFunctions::eventPassCuts(EventNtuple * ntuple, const PhysicsProcessNEW*){

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
  if ( ntuple->leptonCat != leptonCat && (leptonCat != DEFS::both && ntuple->leptonCat != DEFS::both) )
    return false;

 /* if (ntuple->leptonCat == DEFS::muon)
    if ((ntuple->lLV[0].Pt()) <= 25.0 || (ntuple->METLV[0].Et()) <= 30.0)
      return false;
  
  if (ntuple->leptonCat == DEFS::electron) 
    if ((ntuple->lLV[0].Pt()) <= 30.0 || (ntuple->METLV[0].Et()) <= 35.0)
      return false;
 */

  //Implement FNAL cuts
  if (doFNAL){
    if (ntuple->leptonCat == DEFS::muon){
      if(!ntuple->FNALcutsMuon())
	return false;
    }
    else if(ntuple->leptonCat == DEFS::electron)
      if (!ntuple->FNALcutsElectron())
	return false;
  }

  //X axis cuts
  if (cutRegion.Contains("signal")){
    if(ntuple->jLV[0].Et() < 30 || ntuple->jLV[1].Et() < 30)
      return false;
  }
  else if (cutRegion.Contains("control1")){
    if (ntuple->jLV[0].Et() > 30 || ntuple->jLV[1].Et() > 30)
      return false;
  }
  else if (cutRegion.Contains("control2")){
    if ((ntuple->jLV[0].Et() > 30 && ntuple->jLV[1].Et() > 30) || 
	(ntuple->jLV[0].Et() < 30 && ntuple->jLV[1].Et() < 30))
      return false;
  }
  else if (cutRegion.Contains("all"))
    return true;

  return true;  

}// eventPassCuts

double UserFunctions::weightFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc)
{
   TString auxName = proc->name;
   auxName.ToUpper();
   if(auxName.Contains("DATA") || auxName.Contains("QCD"))
   {
      return 1.0;
   }
   
   double weight = 1.0;

   // Pileup reweighting
   if (doPUreweight){
     weight *= puweight->getWeight(ntuple->tnpus[1]);
     
     if (WJweight){
       if (auxName.Contains("WJETS")){
	 double dpLJ = ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]);
	 double dpJJ = ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]);
	 int bin     = wJetsWeight->FindBin(dpLJ,dpJJ);
	 weight *= wJetsWeight->GetBinContent(bin);
       }
     }
   }
   return weight;
}

void UserFunctions::initEventFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc)
{
  TString auxName = proc->name;
  auxName.ToUpper();
  if(auxName.Contains("DATA") || auxName.Contains("QCD"))
    return;

   // Performs JER
  if (doJER)
    ntuple->doJER();
}

void UserFunctions::processFunc(EventNtuple* ntuple, const PhysicsProcessNEW* proc)
{
   doMuon = true;
   doElectron = true;
   if(((PlotterPhysicsProcessNEW*)proc)->leptonCat == DEFS::electron)
      doMuon = false;
   if(((PlotterPhysicsProcessNEW*)proc)->leptonCat == DEFS::muon)
      doElectron = false;
   
  TString auxName = proc->name;
  auxName.ToUpper();

  if (WJweight){
    TFile* Subs;
    if(auxName.Contains("WJETS")){
      Subs = TFile::Open(TString("/uscms/home/amejia94/CMSSW_5_2_5/src/subtracted_"+DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root"));
      if (!Subs->IsOpen()) {
	cout << "\tERROR::Weight file (subtracted_"+DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root) was not opened" << endl
	     << "\tWJets histograms will not be filled" << endl;
	return;
      }
      TH2D* auxh  = (TH2D*) Subs->Get("hdSubtract");
      if (!auxh) {
	cout << "\tERROR::Weight hist hdSubtract could not be found int subtracted_"+DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root" << endl
	     << "\tWJets histograms will not be filled" << endl;
	return;
      }
      wJetsWeight = (TH2D*) auxh->Clone();
      if (!wJetsWeight) {
	cout << "\tERROR::Weight hist wJetsWeight was not cloned properly" << endl
	     << "\tWJets histograms will not be filled" << endl;
	return;
      }
      wJetsWeight->SetDirectory(0);
      Subs->Close();
    }
  }
  
  auxName.ToUpper();
  if(auxName.Contains("DATA") || auxName.Contains("QCD"))
    return;
   
  // Initializes PU Reweighting
  string dataname = string("/uscms/home/amejia94/CMSSW_5_2_3_patch4/src/pileup12_noTrig.root");
  string MCname   = string(TString(proc->fileName));
  puweight = new PUreweight(dataname,MCname,"pileup_IsoMu24_eta2p1","PS/TPUDist");
}

double UserFunctions::charge(int x) {
   int absx = abs(x);
   if (absx==2 || absx==4 || absx==6)
      return (x > 0) ? (2./3.) : ((x < 0) ? -(2./3.) : 0);
   else if (absx==1 || absx==3 || absx==5)
      return (x > 0) ? (-1./3.) : ((x < 0) ? (1./3.) : 0);
   else if (absx==11 || absx==13 || absx==15)
      return (x > 0) ? -1 : ((x < 0) ? 1 : 0);
   else if (absx==24)
      return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
   else
      return 0;
}//charge

int UserFunctions::leptonOrQuark(int x)
{
   int absx = abs(x);
   if (absx>10 && absx < 17)
      return 0;
   else if (absx>0 && absx < 7)
      return 1;
   else
      return -1;
}//leptonOrQuark


pair<double,double> UserFunctions::onVsOffShell(EventNtuple * ntuple) {
   if (ntuple->genParticleCollection.size()==0) {
      //cout << "WARNING::No genParticleCollection present." << endl;
      return make_pair(0.0,0.0);
   }

   pair<int,int> fW = make_pair(0,0); //first = sign, second = position                                     
   pair<int,int> sW = make_pair(0,0);
   int sl  = 0;
   double cj1 = 0;
   double cj2 = 0;
   int sjj = 0;

   for (unsigned int i=0; i<ntuple->genParticleCollection.size(); i++) {
      if (abs(ntuple->genParticleCollection[i].pdgId)==24) {
         if (fW.first==0) {
            fW.first = charge(ntuple->genParticleCollection[i].pdgId);
            fW.second = i;
         }
         else {
            sW.first = charge(ntuple->genParticleCollection[i].pdgId);
            sW.second = i;
         }
      }
      if (abs(ntuple->genParticleCollection[i].pdgId)==11 || abs(ntuple->genParticleCollection[i].pdgId)==13 ||
          abs(ntuple->genParticleCollection[i].pdgId)==15) {
         sl = charge(ntuple->genParticleCollection[i].pdgId);
         if (sl==0)
            cout << "sl==0 and lept pdgId = " << (ntuple->genParticleCollection[i].pdgId) << endl;
      }
      if (abs(ntuple->genParticleCollection[i].pdgId)>0 && abs(ntuple->genParticleCollection[i].pdgId)<9) {
         if (cj1!=0.0 && cj2!=0.0)
            cout << "WARNING::There were more than two jets found. Both cj1 and cj2 are non-zero." << endl;
         /*
         cout << "pdgId" << i << " = " << ntuple->genParticleCollection[i].pdgId << "\tmother_pdgIds = ";
         for (int j=0; j<ntuple->genParticleCollection[i].numberOfMothers; j++) {
            cout << ntuple->genParticleCollection[ntuple->genParticleCollection[i].motherPositions[j]].pdgId << ", ";
         }
         cout << endl;
         */
         bool Wjet = true;
         for (int j=0; j<ntuple->genParticleCollection[i].numberOfMothers; j++) {
            if (abs(ntuple->genParticleCollection[ntuple->genParticleCollection[i].motherPositions[j]].pdgId)!=24)
               Wjet = false;
         }

         if (Wjet) {
            if (cj1==0)
               cj1 = charge(ntuple->genParticleCollection[i].pdgId);
            else {
               cj2 = charge(ntuple->genParticleCollection[i].pdgId);
            }
         }
      }
   }

   if (cj1!=0.0 && cj2!=0.0) {
      sjj = round_int(cj1 + cj2);
      //cout << "sjj = " << sjj << "\tcj1 = " << cj1 << "\tcj2 = " << cj2 << "\tcj1+cj2 = " << cj1+cj2 
      //     << "\tround_int(cj1+cj2) = " << round_int(cj1+cj2) << endl;
   }

   if (fW.first==sl && sW.first==sjj) {
      //h->Fill(ntuple->genParticleCollection[sW.second].p4.M(),
      //        ntuple->genParticleCollection[fW.second].p4.M());
      return make_pair(ntuple->genParticleCollection[sW.second].p4.M(),
                       ntuple->genParticleCollection[fW.second].p4.M());
   }
   else if (fW.first==sjj && sW.first==sl) {
      //h->Fill(ntuple->genParticleCollection[fW.second].p4.M(),
      //        ntuple->genParticleCollection[sW.second].p4.M());
      return make_pair(ntuple->genParticleCollection[fW.second].p4.M(),
                       ntuple->genParticleCollection[sW.second].p4.M());
   }
   else {
     //cout << "WARNING::Unable to determine which W is hadronic and which W is leptonic." << endl
     //      << "fW.first = " << fW.first << "\tsW.first = " << sW.first << "\tsl = " << sl << "\tcj1 = "
     //      << cj1 << "\tcj2 = " << cj2 << "\tsjj = " << sjj << endl;
      return make_pair(0.0,0.0);
   }

}//onVsOffShell

pair<double,double> UserFunctions::onVsOffShellInclusive(EventNtuple * ntuple)
{
   if (ntuple->genParticleCollection.size()==0) {
      //cout << "WARNING::No genParticleCollection present." << endl;
      return make_pair(0.0,0.0);
   }
   
   //0=leptonic, 1=hadronic, 2=both, 3=neither/not set/unknown
   vector<pair<int,int> > W;  //first = category, second = position
   int lc = 0; //lepton counter
   int qc = 0; //quark counter
   for (unsigned int i=0; i<ntuple->genParticleCollection.size(); i++) {
      if (abs(ntuple->genParticleCollection[i].pdgId)!=25)
	continue;
      else {
	if (verbose) 
	  cout << "H->";
	for (unsigned int j=0; j<ntuple->genParticleCollection[i].daughterPositions.size(); j++) {
	  if (ntuple->genParticleCollection[i].daughterPositions[j]<=500 && abs(ntuple->genParticleCollection[ntuple->genParticleCollection[i].daughterPositions[j]].pdgId)==24) {
	    if (verbose) 
	      cout << "W";
	    W.push_back(make_pair(3,ntuple->genParticleCollection[i].daughterPositions[j]));
	  }
	} //loop through the daughters of the Higgs
	if (verbose)
	  cout << "->";
	for (unsigned int j=0; j<W.size() && W.size()==2; j++) {
	  lc = 0;
	  qc = 0;
	  for (unsigned int k=0; k<ntuple->genParticleCollection[W[j].second].daughterPositions.size(); k++) {
	    if (W[j].first==2)
	      continue;
	    else if (ntuple->genParticleCollection[W[j].second].daughterPositions[k]<=500 && leptonOrQuark(ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId)==0) {
	      lc++;
	    }
	    else if (ntuple->genParticleCollection[W[j].second].daughterPositions[k]<=500 && leptonOrQuark(ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId)==1) {
	      qc++;
	    }
	    if (ntuple->genParticleCollection[W[j].second].daughterPositions[k]<=500 && ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId!=0)
	      if (verbose)
		cout << ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId
		     << ",";
	  } //loop through the daughters of the W
	  if (lc==2 && qc==0)
	    W[j].first = 0;
	  else if (lc==0 && qc==2)
	    W[j].first = 1;
	  else if (lc>0 && qc>0)
	    W[j].first = 2;
	  else {
	    cout << "Something is wonky!!!" << endl;
	    W[j].first = 3;
	  }
	} //loop through all of the W
	if (verbose)
	  cout << endl;
      } //find the Higgs
   } //loop through the gen particles
   
   if (W.size()==0) {
     //cout << "WARNING::No W decaying from a Higgs was found." << endl;
     return make_pair(0.0,0.0);
   }
   else if (W[0].first==0 && W[1].first==1)
     return make_pair(ntuple->genParticleCollection[W[1].second].p4.M(),
		      ntuple->genParticleCollection[W[0].second].p4.M());
   else if (W[0].first==1 && W[1].first==0)
     return make_pair(ntuple->genParticleCollection[W[0].second].p4.M(),
		      ntuple->genParticleCollection[W[1].second].p4.M());
   else {
     cout << "WARNING::Unable to determine which W is hadronic and which W is leptonic." << endl
	  << "W[0].first = " << W[0].first << "\tW1[].first = " << W[1].first << endl
	  << "W[0].second = " << W[0].second << "\tW1[].second = " << W[1].second << endl;
     return make_pair(0.0,0.0);
   }
}


double UserFunctions::round(double r) {
   return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}//round

int UserFunctions::round_int(double r) {
   return (r > 0.0) ? (r + 0.5) : (r - 0.5); 
}//round

///  fills the histograms and controls the output canvas and file for the rest of the program
void doPlotter(TString fileName, map<string, Plot*> & plots, vector<PhysicsProcessNEW*> procs, bool doJER, bool doPUrewt, bool doFNAL, int maxEvts, bool WJweight, bool verbose);

/// returns the cross section for the given process
double getCrossSection(TString channelName);

/// returns the branching ratio for the given process
double getBranchingRatio(TString channelName);

/// returns the number of Monte Carlo events for the given process
double getNumMCEvts(TString channelName);

/// returns a vector containing all of the processes that the program will use
vector<PhysicsProcessNEW*> getProcesses(DEFS::LeptonCat leptonCat, double intLum);

/// returns a vector containing all of the processes that the program will use
vector<PhysicsProcessNEW*> getProcesses(vector<string>& processNames, double intLum);

/// returns a map containing all of the plots that will be made for each process and their specific attributes
map<string, Plot*> getPlots(string leptonCatString);

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

   vector<string> processNames      = cl.getVector<string> ("procs",           "");
   double         intLum            = cl.getValue<double>  ("lum",           3600);  
   string         lepCat            = cl.getValue<string>  ("lep",         "both");
   UserFunctions::doJER             = cl.getValue<bool>    ("doJer",         true);
   UserFunctions::doPUreweight      = cl.getValue<bool>    ("doPUrewt",      true);
   UserFunctions::doFNAL            = cl.getValue<bool>    ("doFNAL",       false);
   UserFunctions::cutRegion         = cl.getValue<string>  ("cutRegion",    "all");
   UserFunctions::outDir            = cl.getValue<string>  ("outDir",         ".");
   UserFunctions::leptonCat         = DEFS::getLeptonCat   (lepCat);
   UserFunctions::WJweight          = cl.getValue<bool>    ("WJweight",     false);
   UserFunctions::verbose           = cl.getValue<bool>    ("verbose",      false);
   int            maxEvts           = cl.getValue<int>     ("maxEvts",          0);

   if (!cl.check()) return 0;
   cl.print();
   
   // The vector containing all plots to be made
   map<string, Plot*> plots = getPlots(lepCat);
   
   cout << "Doing lepton category "<<DEFS::getLeptonCatString(UserFunctions::leptonCat)<< endl;

   // The vector holding all processes.
   vector <PhysicsProcessNEW*> procs;
   
   if (!processNames.empty())
     procs = getProcesses(processNames,intLum);
   else
     procs = getProcesses(UserFunctions::leptonCat,intLum);
      
   // Make all the plots and store to outputFile
   TString outFileName = "outputFile_";
   outFileName += DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root";
   doPlotter(outFileName, plots, procs, UserFunctions::doJER, UserFunctions::doPUreweight, UserFunctions::doFNAL, maxEvts, UserFunctions::WJweight, UserFunctions::verbose);
   
   if (UserFunctions::leptonCat == DEFS::muon || UserFunctions::leptonCat == DEFS::both)
   {
      plots["DeltaPhi_LJ1_vs_J1J2_muon"]->saveHistogramsToFile("DeltaPhi_LJ1_vs_J1J2_mu.root");
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_muon"]->saveHistogramsToFile("DeltaPhi_LJ1_vs_J1J2_Subtracted_muon.root");
   }
   if (UserFunctions::leptonCat == DEFS::electron || UserFunctions::leptonCat == DEFS::both)
   {
      plots["DeltaPhi_LJ1_vs_J1J2_electron"]->saveHistogramsToFile("DeltaPhi_LJ1_vs_J1J2_el.root");
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_electron"]->saveHistogramsToFile("DeltaPhi_LJ1_vs_J1J2_Subtracted_electron.root");
   }
   
   return 0;

}//plotter()


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void doPlotter(TString fileName, map<string, Plot*> & plots, vector<PhysicsProcessNEW*> procs, bool doJER, bool doPUrewt, bool doFNAL, int maxEvts, bool WJweight, bool verbose) {

   PlotFiller pFill(plots, procs, &UserFunctions::fillPlots);
   pFill.setCutFunction(&UserFunctions::eventPassCuts);
   pFill.setWeightFunction(&UserFunctions::weightFunc);
   pFill.setProcessFunction(&UserFunctions::processFunc);
   pFill.setInitializeEventFunction(&UserFunctions::initEventFunc);
   if (maxEvts>0)
      pFill.setMaximumEventsDEBUG(maxEvts); // TEST
   pFill.run();

   // Will all the info in the plots get the canvas and write it to file
   TFile * outFile = new TFile(fileName,"RECREATE");
   for ( map<string, Plot*>::iterator p = plots.begin(); p != plots.end() ; p++)
   {
      TCanvas* can = ((FormattedPlot*) p->second)->getCanvas(procs);
      
      can->Write();
      
      if(!UserFunctions::outDir.EndsWith("/"))
         UserFunctions::outDir+="/";
      if(!gSystem->OpenDirectory(UserFunctions::outDir))
         gSystem->mkdir(UserFunctions::outDir);
      can->SaveAs(UserFunctions::outDir+"/"+can->GetName()+".png");
      can->SaveAs(UserFunctions::outDir+"/"+can->GetName()+".eps");
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
      cout << "WARNING::getCrossSection::channelName " << channelName << " not recognized. Returning -1 for the cross section." << endl << "The events will have the same scale as the MC sample, but on a negative scale." << endl << "Please check channel names." << endl;
      return -1.;
   }
}//getCrossSection

//______________________________________________________________________________
double getBranchingRatio(TString channelName)
{
   Table table;
   double xsec;

   string basePath = gSystem->pwd();
   basePath = basePath.substr(0,basePath.find("TAMUWW"));
   table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/BranchingRatios_8TeV.txt"),"TableCellVal");
   TableCell * cell = table.getCellRowColumn(string(channelName),"BranchingRatio");
   if(cell){
      xsec = ((TableCellVal*)cell)->val.value;
      if (xsec==0)
         cout << "WARNING::getBranchingRatio::The branching ratio for " << channelName << " is 0.0 +/- 0.0" << endl;
      return xsec;
   } else{
      cout << "WARNING::getBranchingRatio::channelName " << channelName << " not recognized. Returning -1 for the branching ratio." << endl << "The events will have the same scale as the MC sample, but on a negative scale." << endl << "Please check channel names." << endl;
      return -1.;
   }
}//getBranchingRatio

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
      cout << "WARNING::getNumMCEvts::channelName " << channelName << " not recognized. Returning -1 event from MC." << endl << "Please check channel names." << endl;
      return -1.;
   }
}//getNumMCEvts


//______________________________________________________________________________
// leptonCan = The lepton category for which the processes are needed.
// intLum    = The integrated luminosity in pb-1 for this given lepton category 
vector<PhysicsProcessNEW*> getProcesses(DEFS::LeptonCat leptonCat, double intLum){
   
  string basePath = "/uscms/home/aperloff/nobackup/PS_outfiles_20120621_NTUPLES/";
  vector <PhysicsProcessNEW*> procs;
  

  procs.push_back(new PlotterPhysicsProcessNEW("WW",basePath+"WW/PS.root", getCrossSection("WW"),
                                               intLum, getNumMCEvts("WW"), getProcessColor("WW")));
  procs.push_back(new PlotterPhysicsProcessNEW("WZ",basePath+"WZ/PS.root", getCrossSection("WZ"),
                                               intLum, getNumMCEvts("WZ"), getProcessColor("WZ")));
  procs.push_back(new PlotterPhysicsProcessNEW("DYJets",basePath+"DYJets/PS.root", getCrossSection("DYJets"),
                                               intLum, getNumMCEvts("DYJets"), getProcessColor("DYJets")));
  procs.push_back(new PlotterPhysicsProcessNEW("TTbar",basePath+"TTbar/PS.root", getCrossSection("TTbar"),
                                               intLum, getNumMCEvts("TTbar"), getProcessColor("TTbar")));
  procs.push_back(new PlotterPhysicsProcessNEW("STopT_T",basePath+"STopT_T/PS.root", getCrossSection("STopT_T"),
                                               intLum, getNumMCEvts("STopT_T"), getProcessColor("STopT_T")));
  //   procs.push_back(new PlotterPhysicsProcessNEW("STopT_Tbar",basePath+"STopT_Tbar/PS.root",
  //                            getCrossSection("STopT_Tbar"), intLum, getNumMCEvts("STopT_Tbar"),
  //                            getProcessColor("STopT_Tbar")));
  procs.push_back(new PlotterPhysicsProcessNEW("STopS_T",basePath+"STopS_T/PS.root", getCrossSection("STopS_T"),
                                               intLum, getNumMCEvts("STopS_T"), getProcessColor("STopS_T")));
  procs.push_back(new PlotterPhysicsProcessNEW("STopS_Tbar",basePath+"STopS_Tbar/PS.root",
                                               getCrossSection("STopS_Tbar"), intLum, getNumMCEvts("STopS_Tbar"),
                                               getProcessColor("STopS_Tbar")));
  procs.push_back(new PlotterPhysicsProcessNEW("STopTW_T",basePath+"STopTW_T/PS.root",
                                               getCrossSection("STopTW_T"), intLum, getNumMCEvts("STopTW_T"),
                                               getProcessColor("STopTW_T")));
  procs.push_back(new PlotterPhysicsProcessNEW("STopTW_Tbar",basePath+"STopTW_Tbar/PS.root", 
                                               getCrossSection("STopTW_Tbar"), intLum, getNumMCEvts("STopTW_Tbar"),
                                               getProcessColor("STopTW_Tbar")));

  procs.push_back(new PlotterPhysicsProcessNEW("ggH125",basePath+"ggH125/PS.root", 
                                               getCrossSection("ggH125")*getBranchingRatio("ggH125"), intLum, getNumMCEvts("ggH125"),
                                               getProcessColor("ggH125"), "PS/jets2"));
  
  // The Data
  // The normalization of the data is done in the same way as the MC. That is 
  // we multiply the histograms by getScaleFactor in class proc in PlotterAux.hh
  //  double getScaleFactor(){return sigma * intLum / initial_events;}
  // FOR DATA, to guarrantee the proper normalization, we set
  // sigma  = 1 / collected luminosity
  // initial_events = 1;
  
  if (leptonCat == DEFS::electron || leptonCat == DEFS::both){
    procs.push_back(new PlotterPhysicsProcessNEW("WJets_electron",basePath+"WJets/PS.root",1.16539*getCrossSection("WJets"),
                                                 intLum, getNumMCEvts("WJets"), getProcessColor("WJets"),
                                                 "PS/EvtTree", DEFS::electron));
    procs.push_back(new PlotterPhysicsProcessNEW("Data_electron",basePath+"SingleEl_Data/PS.root",
                                                 1./intLum, intLum, 1,
                                                 getProcessColor("SingleEl_Data"),
                                                 "PS/EvtTree", DEFS::electron)); 
    procs.push_back(new PlotterPhysicsProcessNEW("QCD_electronEnriched", basePath+"QCD_ElEnriched/PS.root",
                                                 0.000882659*getCrossSection("QCD_ElEnriched"), intLum,
                                                 getNumMCEvts("QCD_ElEnriched"),
                                                 getProcessColor("QCD_ElEnriched"),
                                                 "PS/EvtTree", DEFS::electron));
  }  
  
  if (leptonCat == DEFS::muon || leptonCat == DEFS::both){
    procs.push_back(new PlotterPhysicsProcessNEW("WJets_muon",basePath+"WJets/PS.root",1.16817*getCrossSection("WJets"),
                                                 intLum, getNumMCEvts("WJets"), getProcessColor("WJets"),
                                                 "PS/EvtTree", DEFS::muon));
    procs.push_back(new PlotterPhysicsProcessNEW("Data_muon",basePath+"SingleMu_Data/PS.root",
                                                 1./intLum, intLum, 1,
                                                 getProcessColor("SingleMu_Data"),
                                                 "PS/EvtTree", DEFS::muon));
    procs.push_back(new PlotterPhysicsProcessNEW("QCD_muonEnriched", basePath+"QCD_MuEnriched/PS.root",
                                                 0.671604*getCrossSection("QCD_MuEnriched"), intLum,
                                                 getNumMCEvts("QCD_MuEnriched"),
                                                 getProcessColor("QCD_MuEnriched"),
                                                 "PS/jets2", DEFS::muon));
  }
  return procs;
  
}//getProcesses


//______________________________________________________________________________
vector<PhysicsProcessNEW*> getProcesses(vector<string>& processNames, double intLum){
   
  //  string basePath = "/uscms_data/d3/ilyao/Summer11/428Full/Stage2SkimResults/"; 
   string basePath = "/uscms/home/aperloff/nobackup/PS_outfiles_20120621_NTUPLES/"; //602 to 613
   vector <PhysicsProcessNEW*> procs;

   for (unsigned int i=0; i<processNames.size(); i++){
      if (TString(processNames[i]).Contains("Data")==0)
         if (processNames[i].compare("TTbar")==0)
            procs.push_back(new PlotterPhysicsProcessNEW(processNames[i],basePath+processNames[i]+"/PS.root",
                                     getCrossSection(processNames[i]), intLum, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
         else
            procs.push_back(new PlotterPhysicsProcessNEW(processNames[i],basePath+processNames[i]+"/PS.root",
                                     getCrossSection(processNames[i]), intLum, getNumMCEvts(processNames[i]),
                                     getProcessColor(processNames[i])));
      else
         procs.push_back(new PlotterPhysicsProcessNEW(processNames[i],basePath+processNames[i]+"/PS.root",1 ,1, 1,
                                  getProcessColor(processNames[i]))); // 1's so we don't normalize data.
   }

   return procs;

}//getProcesses


//______________________________________________________________________________
map<string, Plot*> getPlots(string leptonCatString){

   map<string, Plot*> plots;

   FormattedPlot* a = new FormattedPlot;
   bool norm_data = false;
   double pi = TMath::Pi();

   //Goes in the label and tells us whether we are looking at electrons or muons
   TString lep = TString(leptonCatString);

   double signalFactor = 5000;
   
   //---------------------ELECTRON-------------------//
   if (lep.CompareTo("electron") == 0 || lep.CompareTo("both") == 0)
   {
      TString electron = "electron";
      
      //goes in the label and tells us what cuts we are applying
      string cut = "_MET > 35, elPt > 30";
      TString cuts = TString(cut);
      
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Mjj_" + electron,"Mjj_" + electron +  cuts,500,0,500);
      a->axisTitles.push_back("M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,400.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Mjj_electron"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH1D("MjjmWmT_" + electron,"MjjmWmT_" + electron +  cuts,500,-250,250);
      a->axisTitles.push_back("M_{jj} - WmT [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(-100.,250.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MjjmWmT_electron"] = a;
   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("LeptPt_" + electron,"LeptPt_" + electron + cuts,1000,0,500);
      a->axisTitles.push_back("p_{T}^{lepton} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptPt_electron"] = a;

   
      a = new FormattedPlot;
 
      a->templateHisto = new TH1D("LeptEta_" + electron,"LeptEta_" + electron + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{lepton} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptEta_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("LeptPhi_" + electron,"LeptPhi_" + electron + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{lepton} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptPhi_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("MET_" + electron,"MET_" + electron + cuts,1000,0,500);
      a->axisTitles.push_back("Missing E_{T} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(30.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MET_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("WmT_" + electron,"WmT_" + electron + cuts,1000,0,500);
      a->axisTitles.push_back("M_{T}^{W} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(0.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Pt_" + electron,"Jet1Pt_" + electron + cuts,200,0,300);
      a->axisTitles.push_back("p_{T}^{jet_{1}} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,200.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Pt_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Eta_" + electron,"Jet1Eta_" + electron + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Eta_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Phi_" + electron,"Jet1Phi_" + electron + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Phi_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Pt_" + electron,"Jet2Pt_" + electron + cuts,200,0,300);
      a->axisTitles.push_back("p_{T}^{jet_{2}} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,100.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Pt_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Eta_" + electron,"Jet2Eta_" + electron + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Eta_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Phi_" + electron,"Jet2Phi_" + electron + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Phi_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaEtaJ1J2_" + electron,"DeltaEtaJ1J2_" + electron + cuts,50,0,5);
      a->axisTitles.push_back("#eta^{jet_{1}} - #eta^{jet_{2}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(0.,5.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaEtaJ1J2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Ptjj_" + electron,"Ptjj_" + electron + cuts,100,0,300);
      a->axisTitles.push_back("p_{T}^{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,250.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Ptjj_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("j1Pt_Mjj_" + electron,"j1Pt_Mjj_" + electron + cuts,500,0,5);
      a->axisTitles.push_back("p_{T}^{jet_{1}}/M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 0.01 GeV");
      a->range = make_pair(0.,2.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["j1Pt_Mjj_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("j2Pt_Mjj_" + electron,"j2Pt_Mjj_" + electron + cuts,500,0,5);
      a->axisTitles.push_back("p_{T}^{jet_{2}}/M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 0.01 GeV");
      a->range = make_pair(0.,1.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["j2Pt_Mjj_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Mlvjj_" + electron,"Mlvjj_" + electron + cuts,250,0,1000);
      a->axisTitles.push_back("M_{lvjj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(50.,800.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Mlvjj_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaRLepMET_" + electron,"DeltaRLepMET_" + electron + cuts,50,0,10);
      a->axisTitles.push_back("#DeltaR of lep and MET [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(0.,7.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaRLepMET_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("EJ1EJ2_" + electron,"EJ1EJ2_" + electron + cuts,500,700,5000);
      a->axisTitles.push_back("E_{J1} * E_{J2}  [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,5000.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["EJ1EJ2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("BetaJ1BetaJ2_" + electron,"BetaJ1BetaJ2_" + electron + cuts,10000,0,10);
      a->axisTitles.push_back("#beta_{J1} * #beta_{J2} [GeV]");
      a->axisTitles.push_back("Number of Events / .01 GeV");
      a->range = make_pair(0.9,1.03);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["BetaJ1BetaJ2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaRJ1J2_" + electron,"DeltaRJ1J2_" + electron + cuts,50,0,10);
      a->axisTitles.push_back("#DeltaR of J1 and J2 [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(0.,7.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaRJ1J2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("AngleJ1J2_" + electron,"AngleJ1J2_" + electron + cuts,50,0,5);
      a->axisTitles.push_back("Angle between J1 and J2 [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-0.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["AngleJ1J2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("jjlvPhi_" + electron,"jjlvPhi__" + electron + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi(jj) - #phi(lv)");
      a->axisTitles.push_back("Number of Events");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["jjlvPhi_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaPhi_LJ1_" + electron,"DeltaPhi_LJ1__" + electron + cuts,50,-10,10);
      a->axisTitles.push_back("#delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaPhi_J1J2_" + electron,"DeltaPhi_J1J2__" + electron + cuts,50,-10,10);
      a->axisTitles.push_back("#delta #phi(J1J2)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_J1J2_electron"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("npv_" + electron,"npv__" + electron + cuts,100,0,100);
      a->axisTitles.push_back("npv");
      a->axisTitles.push_back("Number of Events");
      a->range = make_pair(0,40);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["npv_electron"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_" + electron),("DeltaPhi_LJ1_vs_J1J2__" + electron + cuts), 15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("#delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_electron"] = a;

      a = new FormattedPlot;
   
      a->templateHisto = new TH2D(("MWjjVsMWlv_" + electron),("MWjjVsMWlv_" + electron + cuts),200,0,200,200,0,200);
      a->axisTitles.push_back("M_{W_{jj}}");
      a->axisTitles.push_back("M_{W_{l#nu}}");
      a->range = make_pair(0,200);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MWjjVsMWlv_electron"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Positive_" + electron),("DeltaPhi_LJ1_vs_J1J2_Positive__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Positive #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Positive_electron"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Negative_" + electron),("DeltaPhi_LJ1_vs_J1J2_Negative__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Negative #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Negative_electron"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Subtracted_" + electron),("DeltaPhi_LJ1_vs_J1J2_Subtracted__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Subtracted #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_electron"] = a;

      //////////////test

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Negative_" + electron),("WmT_Negative__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Negative WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Negative_electron"] = a;

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Positive_" + electron),("WmT_Positive__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Positive WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Positive_electron"] = a;

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Subtracted_" + electron),("WmT_Subtracted__" + electron + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Subtracted WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Subtracted_electron"] = a;
   }
   //----------------------------MUON------------------------------//
   if (lep.CompareTo("muon") == 0 || lep.CompareTo("both") == 0)
   {
      TString muon = "muon";
      
      //goes in the label and tells us what cuts we are applying
      string cut = "_MET > 30, muPt > 25";
      TString cuts = TString(cut);
      
      a = new FormattedPlot;
      
      a->templateHisto = new TH1D("Mjj_" + muon,"Mjj_" + muon +  cuts,500,0,500);
      a->axisTitles.push_back("M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,400.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Mjj_muon"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH1D("MjjmWmT_" + muon,"MjjmWmT_" + muon +  cuts,500,-250,250);
      a->axisTitles.push_back("M_{jj} - WmT [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(-100.,250.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MjjmWmT_muon"] = a;
   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("LeptPt_" + muon,"LeptPt_" + muon + cuts,1000,0,500);
      a->axisTitles.push_back("p_{T}^{lepton} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptPt_muon"] = a;

   
      a = new FormattedPlot;
 
      a->templateHisto = new TH1D("LeptEta_" + muon,"LeptEta_" + muon + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{lepton} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptEta_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("LeptPhi_" + muon,"LeptPhi_" + muon + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{lepton} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["LeptPhi_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("MET_" + muon,"MET_" + muon + cuts,1000,0,500);
      a->axisTitles.push_back("Missing E_{T} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(30.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MET_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("WmT_" + muon,"WmT_" + muon + cuts,1000,0,500);
      a->axisTitles.push_back("M_{T}^{W} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(0.,150.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Pt_" + muon,"Jet1Pt_" + muon + cuts,200,0,300);
      a->axisTitles.push_back("p_{T}^{jet_{1}} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,200.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Pt_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Eta_" + muon,"Jet1Eta_" + muon + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Eta_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet1Phi_" + muon,"Jet1Phi_" + muon + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet1Phi_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Pt_" + muon,"Jet2Pt_" + muon + cuts,200,0,300);
      a->axisTitles.push_back("p_{T}^{jet_{2}} [GeV]");
      a->axisTitles.push_back("Number of Events / 5 GeV");
      a->range = make_pair(20.,100.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Pt_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Eta_" + muon,"Jet2Eta_" + muon + cuts,50,-5,5);
      a->axisTitles.push_back("#eta^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(-3.,3.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Eta_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Jet2Phi_" + muon,"Jet2Phi_" + muon + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi^{jet_{1}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Jet2Phi_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaEtaJ1J2_" + muon,"DeltaEtaJ1J2_" + muon + cuts,50,0,5);
      a->axisTitles.push_back("#eta^{jet_{1}} - #eta^{jet_{2}} [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(0.,5.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaEtaJ1J2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Ptjj_" + muon,"Ptjj_" + muon + cuts,100,0,300);
      a->axisTitles.push_back("p_{T}^{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,250.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Ptjj_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("j1Pt_Mjj_" + muon,"j1Pt_Mjj_" + muon + cuts,500,0,5);
      a->axisTitles.push_back("p_{T}^{jet_{1}}/M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 0.01 GeV");
      a->range = make_pair(0.,2.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["j1Pt_Mjj_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("j2Pt_Mjj_" + muon,"j2Pt_Mjj_" + muon + cuts,500,0,5);
      a->axisTitles.push_back("p_{T}^{jet_{2}}/M_{jj} [GeV]");
      a->axisTitles.push_back("Number of Events / 0.01 GeV");
      a->range = make_pair(0.,1.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["j2Pt_Mjj_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("Mlvjj_" + muon,"Mlvjj_" + muon + cuts,250,0,1000);
      a->axisTitles.push_back("M_{lvjj} [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(50.,800.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["Mlvjj_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaRLepMET_" + muon,"DeltaRLepMET_" + muon + cuts,50,0,10);
      a->axisTitles.push_back("#DeltaR of lep and MET [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(0.,7.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaRLepMET_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("EJ1EJ2_" + muon,"EJ1EJ2_" + muon + cuts,500,700,5000);
      a->axisTitles.push_back("E_{J1} * E_{J2}  [GeV]");
      a->axisTitles.push_back("Number of Events / 10 GeV");
      a->range = make_pair(0.,5000.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["EJ1EJ2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("BetaJ1BetaJ2_" + muon,"BetaJ1BetaJ2_" + muon + cuts,10000,0,10);
      a->axisTitles.push_back("#beta_{J1} * #beta_{J2} [GeV]");
      a->axisTitles.push_back("Number of Events / .01 GeV");
      a->range = make_pair(0.9,1.03);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["BetaJ1BetaJ2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaRJ1J2_" + muon,"DeltaRJ1J2_" + muon + cuts,50,0,10);
      a->axisTitles.push_back("#DeltaR of J1 and J2 [Radians]");
      a->axisTitles.push_back("Number of Events / 0.2 Radians");
      a->range = make_pair(0.,7.);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaRJ1J2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("AngleJ1J2_" + muon,"AngleJ1J2_" + muon + cuts,50,0,5);
      a->axisTitles.push_back("Angle between J1 and J2 [Radians]");
      a->axisTitles.push_back("Number of Events / 0.1 Radians");
      a->range = make_pair(-0.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["AngleJ1J2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("jjlvPhi_" + muon,"jjlvPhi__" + muon + cuts,70,-3.5,3.5);
      a->axisTitles.push_back("#phi(jj) - #phi(lv)");
      a->axisTitles.push_back("Number of Events");
      a->range = make_pair(-3.5,3.5);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["jjlvPhi_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaPhi_LJ1_" + muon,"DeltaPhi_LJ1__" + muon + cuts,50,-10,10);
      a->axisTitles.push_back("#delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("DeltaPhi_J1J2_" + muon,"DeltaPhi_J1J2__" + muon + cuts,50,-10,10);
      a->axisTitles.push_back("#delta #phi(J1J2)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_J1J2_muon"] = a;

   
      a = new FormattedPlot;

      a->templateHisto = new TH1D("npv_" + muon,"npv__" + muon + cuts,100,0,100);
      a->axisTitles.push_back("npv");
      a->axisTitles.push_back("Number of Events");
      a->range = make_pair(0,40);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["npv_muon"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_" + muon),("DeltaPhi_LJ1_vs_J1J2__" + muon + cuts), 15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("#delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_muon"] = a;

      a = new FormattedPlot;
   
      a->templateHisto = new TH2D(("MWjjVsMWlv_" + muon),("MWjjVsMWlv_" + muon + cuts),200,0,200,200,0,200);
      a->axisTitles.push_back("M_{W_{jj}}");
      a->axisTitles.push_back("M_{W_{l#nu}}");
      a->range = make_pair(0,200);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["MWjjVsMWlv_muon"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Positive_" + muon),("DeltaPhi_LJ1_vs_J1J2_Positive__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Positive #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Positive_muon"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Negative_" + muon),("DeltaPhi_LJ1_vs_J1J2_Negative__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Negative #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Negative_muon"] = a;


      a = new FormattedPlot;

      a->templateHisto = new TH2D(("DeltaPhi_LJ1_vs_J1J2_Subtracted_" + muon),("DeltaPhi_LJ1_vs_J1J2_Subtracted__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Subtracted #delta #phi(J1J2) vs. #delta #phi(LJ1)");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["DeltaPhi_LJ1_vs_J1J2_Subtracted_muon"] = a;

      //////////////test

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Negative_" + muon),("WmT_Negative__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Negative WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Negative_muon"] = a;

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Positive_" + muon),("WmT_Positive__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Positive WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Positive_muon"] = a;

      a = new FormattedPlot;

      a->templateHisto = new TH2D(("WmT_Subtracted_" + muon),("WmT_Subtracted__" + muon + cuts),15,-pi,pi,15,-pi,pi);
      a->axisTitles.push_back("Subtracted WmT");
      a->axisTitles.push_back("Number of Events / .2 Radians");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::muon;
      a->overlaySignalName = "ggH125";
      a->overlaySignalFactor = signalFactor;
      plots["WmT_Subtracted_muon"] = a;  
   }
   // return all the plots to be made
   return plots;

}//getPlots


//______________________________________________________________________________
Color_t getProcessColor(TString channelName){

  if (channelName.CompareTo("WW") == 0)
    return kPink;
  else if (channelName.CompareTo("WZ") == 0)
    return kBlue;
  else if (channelName.CompareTo("WJets") == 0)
    return kTeal+2;
  else if (channelName.CompareTo("DYJets") == 0)
    return kPink-8;
  else if (channelName.CompareTo("QCD_ElEnriched") == 0)
    return kYellow+1;
  else if (channelName.CompareTo("QCD_MuEnriched") == 0)
    return kYellow+1;
  //right now these aren't being used because we are combining all the STop's in PlotterAux.cc
  else if (channelName.CompareTo("STopT_T") == 0)
    return kOrange+1;
  else if (channelName.CompareTo("STopT_Tbar") == 0)
    return kOrange+1; //kCyan+3;
  else if (channelName.CompareTo("STopS_T") == 0)
    return kOrange+1; //kBlue;
  else if (channelName.CompareTo("STopS_Tbar") == 0)
    return kOrange+1; //kBlue+3;
  else if (channelName.CompareTo("STopTW_T") == 0)
    return kOrange+1; //kMagenta;
  else if (channelName.CompareTo("STopTW_Tbar") == 0)
    return kOrange+1; //kGreen+3;
  else if (channelName.CompareTo("STopTW_Tbar") == 0)
    return kOrange+1; //kGreen+3;
   else if (channelName.CompareTo("TTbar") == 0)
     return kAzure-2; 
   else if (channelName.CompareTo("ggH125") == 0)
     return kRed+2; 
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
