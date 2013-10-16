//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/Tools/interface/Plots.hh"
#include "TAMUWW/Tools/interface/PlotFiller.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"
#include "TAMUWW/Tools/interface/CSVreweight.hh"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF2.h"
#include "TH2D.h"
#include "TChain.h"
#include "TLorentzVector.h"
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
using DEFS::LeptonCat;

typedef PlotFiller::MapOfPlots MapOfPlots;

namespace UserFunctions
{
   TString outDir;
   TString cutRegion;
   DEFS::LeptonCat leptonCat;
   PUreweight* puweight;
   CSVreweight* csvweight;
   bool doJER;
   bool doMETPhiCorrection;
   bool doPUreweight;
   bool doCSVreweight;
   bool doFNAL;
   bool doMetPhiWeight;
   TH1D* metPhiWeight= 0;
   bool fillTMDF;
   bool  QCDweight;
   TH1D* QCDWeightFunc = 0;
   bool WJweight;
   TH2D* WJetsWeightFunc= 0;
   TF2 * WJetsWeightTF2 = 0;
   bool verbose; // adds or takes away cout statements when running
   
   ////////////////////////////////////////////////////////////////////////////////
   //  User Functions
   ////////////////////////////////////////////////////////////////////////////////

   // Is run once for each process before events are cut (initialize)
   void initEventFunc(EventNtuple* ntuple, const PhysicsProcess* proc);
   // this function fills all of the plots for a given process
   void fillPlots(MapOfPlots &  plots, EventNtuple * ntuple, METree * metree, 
                  MicroNtuple * mnt, vector<TString>, double weight = 1.0);
   // returns a boolean if the event passes the specified cuts
   bool eventPassCuts(EventNtuple * ntuple, const PhysicsProcess*);
   // returns a double
   double weightFunc(EventNtuple* ntuple, const PhysicsProcess* proc);
   // Is run once for each process before events (initializes PU Reweighting
   void processFunc(EventNtuple* ntuple, const PhysicsProcess* proc);
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
   // concat any two streamable objects into a string
   template <class T, class U>
   std::string concatString(const T& obj1, const U& obj2);
}

////////////////////////////////////////////////////////////////////////////////
//  Implement User Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void UserFunctions::fillPlots(MapOfPlots &  plots, EventNtuple * ntuple,  METree * metree, MicroNtuple * mnt,
                              vector<TString> MVAMethods, double weight)
{
   //Weird events with  ntuple->lLV[0].leptonCat different than electrons
   // or muons are killed in eventCuts so they never reach this stage
   DEFS::LeptonCat leptonCat = ntuple->lLV[0].leptonCat;

   //WARNING!!! Make sure that ntuple, metree, and mnt are set before using!!! 
   // Example::metree and mnt will not be set if you are running on an MEInput file.
   if (ntuple) {
      double WmT = sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Pt(), 2) -
                        pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) -
                        pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2));
      double Mjj = 0;
      if (ntuple->jLV.size()>1) {
         Mjj = (ntuple->jLV[0] + ntuple->jLV[1]).M();
         plots[leptonCat]["Mjj"]->Fill(Mjj,weight);
         plots[leptonCat]["MjjmWmT"]->Fill(Mjj - WmT, weight);
         plots[leptonCat]["j1Pt_Mjj"]->Fill(ntuple->jLV[0].Pt() / ntuple->Mjj,weight);
         plots[leptonCat]["j2Pt_Mjj"]->Fill(ntuple->jLV[1].Pt() / ntuple->Mjj,weight);
         plots[leptonCat]["Jet2Pt"]->Fill(ntuple->jLV[1].Pt(),weight);
         plots[leptonCat]["Jet2Eta"]->Fill(ntuple->jLV[1].Eta(),weight);
         plots[leptonCat]["Jet2Phi"]->Fill(ntuple->jLV[1].Phi(),weight);
         plots[leptonCat]["DeltaEtaJ1J2"]->Fill(TMath::Abs(ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta()),weight);
         plots[leptonCat]["Ptjj"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Pt(),weight);
         plots[leptonCat]["Mlvjj"]->Fill((ntuple->jLV[0] + ntuple->jLV[1] + ntuple->lLV[0] 
                                          + ntuple->METLV[0]).M(),weight);
         plots[leptonCat]["EJ1EJ2"]->Fill(ntuple->jLV[0].E() * ntuple->jLV[1].E(),weight);
         plots[leptonCat]["BetaJ1BetaJ2"]->Fill(ntuple->jLV[0].Beta() * ntuple->jLV[1].Beta(),weight);
         plots[leptonCat]["DeltaRJ1J2"]->Fill(sqrt(pow(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),2)+
                                                   pow(ntuple->jLV[0].Phi()-ntuple->jLV[1].Phi(),2)),weight);
         plots[leptonCat]["AngleJ1J2"]->Fill(ntuple->jLV[0].Angle(ntuple->jLV[1].Vect()),weight);
         plots[leptonCat]["DeltaPhi_J1J2"]->Fill(ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);
         plots[leptonCat]["DeltaPhi_LJ1_vs_J1J2"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), 
                                                        ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]),weight);
         plots[leptonCat]["DeltaEta_LJ1_vs_J1J2"]->Fill(ntuple->lLV[0].Eta()-ntuple->jLV[0].Eta(), 
                                                        ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),weight); 
         plots[leptonCat]["jjlvPhi"]->Fill((ntuple->jLV[0] + ntuple->jLV[1]).Phi() -
                                           (ntuple->lLV[0] + ntuple->METLV[0]).Phi(),weight);
         plots[leptonCat]["DeltaPhi_LJ1_vs_J1J2_Subtracted"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]),
                                                                   ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), 
                                                                   (ntuple->lLV[0].lQ)*weight);
      }

      plots[leptonCat]["LeptPt"]->Fill(ntuple->lLV[0].Pt(),weight);
      plots[leptonCat]["LeptEta"]->Fill(ntuple->lLV[0].Eta(),weight);
      plots[leptonCat]["LeptPhi"]->Fill(ntuple->lLV[0].Phi(),weight);
      plots[leptonCat]["LeptPFIso"]->Fill(ntuple->lLV[0].lpfIso ,weight);
      plots[leptonCat]["MET"]->Fill(ntuple->METLV[0].Pt(),weight);
      plots[leptonCat]["MET_vs_LeptEta"]->Fill(ntuple->lLV[0].Eta(),
					       ntuple->METLV[0].Pt(),
					       weight);      
      plots[leptonCat]["MET_vs_AbsLeptEta"]->Fill(fabs(ntuple->lLV[0].Eta()),
						  ntuple->METLV[0].Pt(),
						  weight);
      plots[leptonCat]["METPhi"]->Fill(ntuple->METLV[0].Phi(),weight);
      plots[leptonCat]["WmT"]->Fill(WmT, weight);
      plots[leptonCat]["Jet1Pt"]->Fill(ntuple->jLV[0].Pt(),weight);
      plots[leptonCat]["Jet1Eta"]->Fill(ntuple->jLV[0].Eta(),weight);
      plots[leptonCat]["Jet1Phi"]->Fill(ntuple->jLV[0].Phi(),weight);
      plots[leptonCat]["DeltaRLepMET"]->Fill(sqrt(pow(ntuple->lLV[0].Eta()-ntuple->METLV[0].Eta(),2)+
                                                  pow(ntuple->lLV[0].Phi()-ntuple->METLV[0].Phi(),2) ),weight);
      plots[leptonCat]["DeltaPhi_LJ1"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]),weight);
      plots[leptonCat]["DeltaPhi_METJ1"]->Fill(ntuple->jLV[0].DeltaPhi(ntuple->METLV[0]),weight);
      plots[leptonCat]["DeltaPhi_LMET"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->METLV[0]),weight);
      plots[leptonCat]["npv"]->Fill(ntuple->vLV[0].npv,weight);

      for (unsigned int j=0; j<ntuple->jLV.size(); j++) {
         plots[leptonCat][string(Form("JetEta_%luJets",ntuple->jLV.size()))]->Fill(ntuple->jLV[j].Eta(),weight);
         plots[leptonCat]["nJets_JetEta"]->Fill(ntuple->jLV[j].Eta(),ntuple->jLV.size(),weight);
      }

      pair<double,double> leptVsHadWMass = onVsOffShellInclusive(ntuple);
      plots[leptonCat]["MWjjVsMWlv"]->Fill(leptVsHadWMass.first,leptVsHadWMass.second,weight);

      if (ntuple->lLV[0].lQ == 1 && ntuple->jLV.size()>1){
         plots[leptonCat]["DeltaPhi_LJ1_vs_J1J2_Positive"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]), 
                                                                 ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots[leptonCat]["DeltaEta_LJ1_vs_J1J2_Positive"]->Fill(ntuple->lLV[0].Eta()-ntuple->jLV[0].Eta(), 
                                                                 ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),weight);
         plots[leptonCat]["WmT_Positive"]->Fill(WmT, weight);
      }
      if (ntuple->lLV[0].lQ == -1 && ntuple->jLV.size()>1){
         plots[leptonCat]["DeltaPhi_LJ1_vs_J1J2_Negative"]->Fill(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]),
                                                                 ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]), weight);
         plots[leptonCat]["DeltaEta_LJ1_vs_J1J2_Negative"]->Fill(ntuple->lLV[0].Eta()-ntuple->jLV[0].Eta(), 
                                                                 ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta(),weight);
         plots[leptonCat]["WmT_Negative"]->Fill(WmT, weight);
      }
      plots[leptonCat]["WmT_Subtracted"]->Fill(WmT, (ntuple->lLV[0].lQ)*weight);

      if (UserFunctions::fillTMDF){
         vector<Double_t> coord;
         //coord.assign(((TProfileMDF*)plots[leptonCat]["lpt_leta_j1pt_j1eta_j2pt_j2eta"]->templateHisto)->GetNaxis(),0);
         //coord.assign(((TProfileMDF*)plots[leptonCat]["lpt_lpt_j1pt_j1pt_j2pt_j2pt"]->templateHisto)->GetNaxis(),0);
         coord.assign(((TProfileMDF*)plots[leptonCat]["Mjj_Mjj_Mt_MET_DeltaR_DeltaR"]->templateHisto)->GetNaxis(),0);
         //coord[0] = ntuple->lLV[0].Pt();
         coord[0] = Mjj;
         //coord[1] = TMath::Abs(ntuple->lLV[0].Eta());
         //coord[1] = ntuple->lLV[0].Pt();
         coord[1] = Mjj;
         //coord[2] = ntuple->jLV[0].Pt();
         coord[2] = WmT;
         //coord[3] = TMath::Abs(ntuple->jLV[0].Eta());
         //coord[3] = ntuple->jLV[0].Pt();
         coord[3] = ntuple->METLV[0].Pt();
         //coord[4] = ntuple->jLV[1].Pt();
         coord[4] = sqrt(pow(ntuple->lLV[0].Eta()-ntuple->jLV[0].Eta(),2)
                         +pow(ntuple->lLV[0].Phi()-ntuple->jLV[0].Phi(),2));
         //coord[5] = TMath::Abs(ntuple->jLV[1].Eta());
         //coord[5] = ntuple->jLV[1].Pt();
         coord[5] = sqrt(pow(ntuple->lLV[0].Eta()-ntuple->jLV[0].Eta(),2)
                         +pow(ntuple->lLV[0].Phi()-ntuple->jLV[0].Phi(),2));
         //plots[leptonCat]["lpt_leta_j1pt_j1eta_j2pt_j2eta"]->Fill(coord,1.0,weight);
         //plots[leptonCat]["lpt_lpt_j1pt_j1pt_j2pt_j2pt"]->Fill(coord,1.0,weight);
         plots[leptonCat]["Mjj_Mjj_Mt_MET_DeltaR_DeltaR"]->Fill(coord,1.0,weight);
      }
   }

   if (metree) {
      for (unsigned int tep=0; tep<MicroNtuple::nEventProb; tep++) {
         string name = UserFunctions::concatString("tEventProb",tep);
         //cout << name << " = " << metree->getProbStat(tep)->tEventProb << " (" << (Float_t*)(&metree->getProbStat(tep)->tEventProb)<< ")"<<endl;
         plots[leptonCat][name]->Fill(TMath::Log10(metree->getProbStat(tep)->tEventProb),weight);
      }

      double tEventProb0 = metree->getProbStat(0)->tEventProb;
      double tEventProb1 = metree->getProbStat(1)->tEventProb;
      double tEventProb3 = metree->getProbStat(3)->tEventProb;
      double signal = (tEventProb0 / 0.8e-06) + (tEventProb1 / 0.1e-06);
      double back   = tEventProb3 / 0.75e-03;
      double epd    = signal /(signal+back);
      plots[leptonCat]["epdPretagWWandWZ_RE"]->Fill(epd,weight);
   }

   if (mnt) {
      plots[leptonCat]["epdPretagWWandWZ"]->Fill(-TMath::Log10(mnt->epdPretagWWandWZ),weight);
      plots[leptonCat]["epdPretagHiggs125"]->Fill(-TMath::Log10(mnt->epdPretagHiggs.At(6)),weight);

      if (!MVAMethods.empty() && ntuple) {
         //cout << "MVADiscriminator_electron = " << mnt->getMVAOutput(MVAMethods).front()["response"] << endl;
         //cout << "MVAProbability_electron = " << mnt->getMVAOutput(MVAMethods).front()["probability"] << endl;
         //cout << "weight = " << weight << endl;
         mnt->setMjjMVA(ntuple->Mjj);
         plots[leptonCat]["MVADiscriminator"]->Fill(mnt->getMVAOutput(MVAMethods).front()["response"],weight);
         plots[leptonCat]["MVAProbability"]->Fill(mnt->getMVAOutput(MVAMethods).front()["probability"],weight);
      }
   }
    
}//fillPlots

//______________________________________________________________________________
// Return true if the event pass the cuts imposed to the given lepton category
bool UserFunctions::eventPassCuts(EventNtuple * ntuple, const PhysicsProcess* proc){  

   // Remove events categorized as "both" or "none", leaving only base categories of electron and muons
   if ( ntuple->lLV[0].leptonCat == DEFS::both ||  ntuple->lLV[0].leptonCat == DEFS::none)
      return false;

   // Remove events that should not be used from this particular process.
   //if (proc->leptonCat != DEFS::both && proc->leptonCat != ntuple->lLV[0].leptonCat)
   PhysParMap ppm = proc->sigma;
   if (ppm[ntuple->lLV[0].leptonCat] == 0.0)
      return false;

   // Remove events that are not requested from the command line  
   if ( ntuple->lLV[0].leptonCat != leptonCat && (leptonCat != DEFS::both) )
      return false;

   //MET Cut
   if ( ntuple->METLV[0].Pt() <= 25.0 )
      return false;

   // PFISO cut for FULL sample
   if (proc->name.Contains("QCD") && ntuple->lLV[0].lpfIso < 0.3)
     return false;

   /*
   // Keep only specific events
   if (ntuple->event!=2663602)
   return false;
   */
 
   /*
   // Diboson analysis cuts
   TLorentzVector mt(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(),ntuple->lLV[0].Py()+ntuple->METLV[0].Py(),0,ntuple->lLV[0].Et()+ntuple->METLV[0].Et());
   double wmt = sqrt(pow(ntuple->lLV[0].Et()+ntuple->METLV[0].Et(), 2) - pow(ntuple->lLV[0].Px()+ntuple->METLV[0].Px(), 2) - pow(ntuple->lLV[0].Py()+ntuple->METLV[0].Py(), 2));

   if (ntuple->leptonCat == DEFS::muon)
   if ((ntuple->lLV[0].Pt()) <= 25.0                                  ||
   (ntuple->METLV[0].Et()) <= 25.0                                ||
   (ntuple->jLV[0].Pt()) <= 35.0                                  ||
   (ntuple->jLV[1].Pt()) <= 35.0                                  ||
   (TMath::Abs(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta())) >= 1.5 || 
   ((ntuple->jLV[0]+ntuple->jLV[1]).Pt()) <= 20.0                 ||
   (ntuple->METLV[0].DeltaPhi(ntuple->jLV[0])) <= 0.4             ||
   (mt.M()) <= 30.0                                               ||
   wmt <= 50.0                                                     )
   return false;
  
   if (ntuple->leptonCat == DEFS::electron) 
   if ((ntuple->lLV[0].Pt()) <= 35.0                                  ||
   (ntuple->METLV[0].Et()) <= 30.0                                ||
   (ntuple->jLV[0].Pt()) <= 35.0                                  ||
   (ntuple->jLV[1].Pt()) <= 35.0                                  ||
   (TMath::Abs(ntuple->jLV[0].Eta()-ntuple->jLV[1].Eta())) >= 1.5 || 
   ((ntuple->jLV[0]+ntuple->jLV[1]).Pt()) <= 20.0                 ||
   (ntuple->METLV[0].DeltaPhi(ntuple->jLV[0])) <= 0.4             ||
   (mt.M()) <= 30.0                                               ||
   wmt <= 50.0                                                     )
   return false;
   */

   //Implement FNAL cuts
   if (doFNAL){
      if (ntuple->lLV[0].leptonCat == DEFS::muon){
         if(!ntuple->FNALcutsMuon())
            return false;
      }
      else if(ntuple->lLV[0].leptonCat == DEFS::electron)
         if (!ntuple->FNALcutsElectron())
            return false;
   }

   // regardless of cut region cut on minimum lepton Pt 
   if(ntuple->lLV[0].leptonCat == DEFS::electron && ntuple->lLV[0].Pt() < 30)
     return false;
   
   if(ntuple->lLV[0].leptonCat == DEFS::muon && ntuple->lLV[0].Pt() < 25)
     return false;


   //X axis cuts
   if (cutRegion.Contains("signal")){

     // leading jet with PT > 30, and second leading with at least 25 GeV
     if(ntuple->jLV[0].Pt() < 30 || ntuple->jLV[1].Pt() < 25)
       return false;

   }
   else if (cutRegion.Contains("control1")){
     
     if (ntuple->jLV[0].Pt() > 30 || ntuple->jLV[1].Pt() > 25)
       return false;
   }
   else if (cutRegion.Contains("control2")){
      if ((ntuple->jLV[0].Pt() > 30 && ntuple->jLV[1].Pt() > 25) || 
          (ntuple->jLV[0].Pt() < 30 && ntuple->jLV[1].Pt() < 25))
         return false;
   }
   else if (cutRegion.Contains("control3")){
     if(ntuple->jLV[0].Pt() > 30 && ntuple->jLV[1].Pt() > 25)
       return false;
   }
   else if (cutRegion.Contains("control4")){
      if(ntuple->jLV.size()<4)
         return false;
   }
   else if (cutRegion.Contains("control5")){
      int nBtag = 0;
      for(unsigned int j=0; j<ntuple->jLV.size();j++) {
         if(ntuple->jLV[j].jBtagCSV==1)
            nBtag++;
      }
      if(ntuple->jLV.size()<4 || nBtag>0) {
         return false;
      }
   }
   else if (cutRegion.Contains("control6")){
      if(ntuple->jLV.size()!=1)
         return false;
   }
   else if (cutRegion.Contains("control7")){
      if(ntuple->jLV.size()!=2)
         return false;
   }
   else if (cutRegion.Contains("control8")){
      if(ntuple->jLV.size()!=3)
         return false;
   }
   else if (!cutRegion.Contains("all"))
      return false;

   return true;  

}// eventPassCuts

double UserFunctions::weightFunc(EventNtuple* ntuple, const PhysicsProcess* proc)
{
   TString auxName = proc->name;
   auxName.ToUpper();

   
   double weight = 1.0;

   if (doMetPhiWeight){
     
      if(auxName.Contains("DATA") || auxName.Contains("QCD")) {
        
         // TEST OF MET PHI WEIGHTING
         double metphi = ntuple->METLV[0].Phi();
         int bin     = metPhiWeight->FindBin(metphi);
         weight *= metPhiWeight->GetBinContent(bin);
	  	
      } //Data or QCD
	 
   } // doMetPhiWeight
  
   // Pileup reweighting
   if (doPUreweight){
      if(!auxName.Contains("DATA") && auxName.Contains("QCD"))
         weight *= puweight->getWeight(ntuple->vLV[0].npv);
      else if(!auxName.Contains("DATA"))// && !auxName.Contains("QCD"))
         weight *= puweight->getWeight(ntuple->vLV[0].tnpus[1]);
   }

   //CSV reweighting
   if (doCSVreweight)
      if (!auxName.Contains("DATA") && !auxName.Contains("QCD"))
         weight *= csvweight->getWeight(ntuple);
   
   // WJets weighting (specific to fix shape issues)
   if (WJweight && auxName.Contains("WJETS")){

      // find delta phi between lepton and leading jet and 
      // between two leading jets
      double dpLJ = ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]);
      double dpJJ = ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]);
      int bin     = WJetsWeightFunc->FindBin(dpLJ,dpJJ);
      weight     *= WJetsWeightFunc->GetBinContent(bin);
      
   } // wjets

   // QCD weighting (eta weighting)
   if (QCDweight && auxName.Contains("QCD")){
      
      // find lepton eta and apply weight
      double leptonEta = ntuple->lLV[0].Eta();
      weight *= QCDWeightFunc->GetBinContent(QCDWeightFunc->FindBin(fabs(leptonEta)));
   }
   
   return weight;

} // weightFunc

void UserFunctions::initEventFunc(EventNtuple* ntuple, const PhysicsProcess* proc)
{
   TString auxName = proc->name;
   auxName.ToUpper();

   // Correct MET Phi
   if (doMETPhiCorrection) {
      if(auxName.Contains("DATA") || auxName.Contains("QCD")) {
         if(verbose) cout << "BEFORE\tPx=" << ntuple->METLV[0].Px() << "\tPy=" << ntuple->METLV[0].Py() << endl;
         //ntuple->doMETPhiCorrection("pfMEtSysShiftCorrParameters_2012runAvsNvtx_data");
         ntuple->doMETPhiCorrection("pfMEtSysShiftCorrParameters_2012runAvsNvtx_TAMUWW_data");
         if(verbose) cout << "AFTER  \tPx=" << ntuple->METLV[0].Px() << "\tPy=" << ntuple->METLV[0].Py() << endl;
      }
      else {
         if(verbose) cout << "BEFORE\tPx=" << ntuple->METLV[0].Px() << "\tPy=" << ntuple->METLV[0].Py() << endl;
         //ntuple->doMETPhiCorrection("pfMEtSysShiftCorrParameters_2012runAvsNvtx_mc");
         ntuple->doMETPhiCorrection("pfMEtSysShiftCorrParameters_2012runAvsNvtx_TAMUWW_mc");
         if(verbose) cout << "AFTER  \tPx=" << ntuple->METLV[0].Px() << "\tPy=" << ntuple->METLV[0].Py() << endl;
      }
   }

   if(auxName.Contains("DATA") || auxName.Contains("QCD"))
      return;

   // Performs JER
   if (doJER)
      ntuple->doJER();

} // initEventFunc

//This is run once per process before looping over events
void UserFunctions::processFunc(EventNtuple* ntuple, const PhysicsProcess* proc)
{
   TString auxName = proc->name;
   auxName.ToUpper();

   if (doMetPhiWeight){
     
      // get the filename
      TString filename = "data_weight.root";

      TString hname = "metphi";

      metPhiWeight = (TH1D*) DefaultValues::getConfigTH1(filename, hname, "metPhiWeightClone");
 	 
   }// if doMetPhiWeight
  
   if (QCDweight && auxName.Contains("QCD")){

      // get the filename
      TString filename = "QCDWeight_";
      filename += DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root";
    
      // get the histogram from the inside
      TString hname = "QCDWeight_";
      hname += DEFS::getLeptonCatString(UserFunctions::leptonCat);

      QCDWeightFunc = (TH1D*) DefaultValues::getConfigTH1(filename,hname,"QCDweightClone");

   }// QCD weight

   if (WJweight){
      if(auxName.Contains("WJETS")){

         // get the filename
         TString filename = "wjweight_";
         filename += DEFS::getLeptonCatString(UserFunctions::leptonCat)+".root";

         TString hname = "DeltaPhi_LJ1_vs_J1J2_";
         hname += DEFS::getLeptonCatString(UserFunctions::leptonCat);

         WJetsWeightFunc = (TH2D*) DefaultValues::getConfigTH2(filename, hname, "WJweightClone");
       
         /* In case we wanted to use a TF2
            wJetsWeightTF2 = new TF2("f2","[1]+( [2]*(x*cos([0])-y*sin([0])) + [3]*(x*cos([0])-y*sin([0]))**2 )" 
            "+( [4]*(x*sin([0])+y*cos([0])) + [5]*(x*sin([0])+y*cos([0]))**2 )",-10,10,-10,10);
            wJetsWeightTF2->SetParameters(-5.17471e-01, 7.79407e-01, 1.84199e-02, 1.10139e-01, 1.28591e-03, 2.18312e-02);  //redchi2 = 5.11
         */
      }
   }
  
   auxName.ToUpper();
   if(auxName.Contains("DATA"))// || auxName.Contains("QCD"))
      return;
   
   // Initializes PU Reweighting
   string dataname = DefaultValues::getConfigPath()+"pileup12_noTrig.root";
   string MCname   = DefaultValues::getConfigPath()+"TPUDistributions.root";
   puweight = new PUreweight(dataname,MCname,"pileup_noTrig",
                             string(TString("TPUDist_")+proc->name));

   csvweight = new CSVreweight();

} // processFunc

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
} // charge

int UserFunctions::leptonOrQuark(int x)
{
   int absx = abs(x);
   if (absx>10 && absx < 17)
      return 0;
   else if (absx>0 && absx < 7)
      return 1;
   else
      return -1;
} // leptonOrQuark


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
         if(W.size()==2) {
            for (unsigned int j=0; j<W.size(); j++) {
               lc = 0;
               qc = 0;
               for (unsigned int k=0; k<ntuple->genParticleCollection[W[j].second].daughterPositions.size(); k++) {
                  if (W[j].first==2)
                     continue;
                  else if (ntuple->genParticleCollection[W[j].second].daughterPositions[k]<=500 && leptonOrQuark(ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId)==0) {
                     if (abs(ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId)==11 && ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].p4.Pt()<=27) {
                        if (verbose)
                           cout << "CUT" << endl;
                        return make_pair(-1.0,-1.0);
                     }
                     if (abs(ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].pdgId)==13 && ntuple->genParticleCollection[ntuple->genParticleCollection[W[j].second].daughterPositions[k]].p4.Pt()<=24) {
                        if (verbose)
                           cout << "CUT" << endl;
                        return make_pair(-1.0,-1.0);
                     }
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
         } //if there are only 2 W
         if (verbose)
            cout << endl;
      } //find the Higgs
   } //loop through the gen particles
   
   if (W.size()==0) {
      //cout << "WARNING::No W decaying from a Higgs was found." << endl;
      return make_pair(-1.0,-1.0);
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
      return make_pair(-1.0,-1.0);
   }
} // UserFunctions::onVsOffShellInclusive


double UserFunctions::round(double r) {
   return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}//round

int UserFunctions::round_int(double r) {
   return (r > 0.0) ? (r + 0.5) : (r - 0.5); 
}//round

template <class T, class U>
std::string UserFunctions::concatString(const T& obj1, const U& obj2)
{
   std::ostringstream output;
   output << obj1 << obj2;
   return output.str();
}//concatString

////////////////////////////////////////////////////////////////////////////////
//  Local Functions
////////////////////////////////////////////////////////////////////////////////

///  fills the histograms and controls the output canvas and file for the rest of the program
void doPlotter(MapOfPlots & plots, vector<PhysicsProcess*> procs, bool doJER, bool doPUrewt,
               bool doCSVrewt, bool doFNAL, int maxEvts, bool WJweight, TString MVAWeightDir, 
               vector<TString> MVAMethods, bool verbose);

// write the Canvases and plots to output files 
void writePlotsToFile(TString histoFileName, TString canvasFileName,
                      MapOfPlots & plots, vector<PhysicsProcess*> procs);

/// returns a map containing all of the plots that will be made for each process and their specific attributes
PlotFiller::MapOfPlots getPlots(DEFS::LeptonCat leptonCat, bool norm_data);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
 
   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string           lepCat           = cl.getValue<string>  ("lep",         "both");
   UserFunctions::leptonCat          = DEFS::getLeptonCat   (lepCat);
   string           ntype            = cl.getValue<string>  ("ntype","EventNtuple");
   DEFS::NtupleType ntupleType       = DEFS::getNtupleType  (ntype);
   string           jBin             = cl.getValue<string>  ("jBin",       "jets2");      
   DEFS::JetBin     jetBin           = DEFS::getJetBin      (jBin);
   UserFunctions::doJER              = cl.getValue<bool>    ("doJer",        false);
   UserFunctions::doMETPhiCorrection = cl.getValue<bool>    ("doMETPhi",     false);
   UserFunctions::doPUreweight       = cl.getValue<bool>    ("doPUrewt",      true);
   UserFunctions::doCSVreweight      = cl.getValue<bool>    ("doCSVrewt",     true);
   UserFunctions::doFNAL             = cl.getValue<bool>    ("doFNAL",       false);
   UserFunctions::doMetPhiWeight     = cl.getValue<bool>    ("doMetPhiWeight",false);
   UserFunctions::cutRegion          = cl.getValue<string>  ("cutRegion",    "all");
   UserFunctions::outDir             = cl.getValue<string>  ("outDir",         ".");
   UserFunctions::WJweight           = cl.getValue<bool>    ("WJweight",     false);
   UserFunctions::QCDweight          = cl.getValue<bool>    ("QCDweight",    false);
   UserFunctions::verbose            = cl.getValue<bool>    ("verbose",      false);
   UserFunctions::fillTMDF           = cl.getValue<bool>    ("fillTMDF",     false);
   bool             norm_data        = cl.getValue<bool>    ("norm_data",    false);
   int              maxEvts          = cl.getValue<int>     ("maxEvts",          0);
   TString          MVAWeightDir     = cl.getValue<TString> ("MVAWeightDir",    "");
   vector<TString>  MVAMethods       = cl.getVector<TString>("MVAMethods",      "");


   if (!cl.check()) return 0;
   cl.print();

   // Check that the leptcat actually exists
   if (UserFunctions::leptonCat == DEFS::none) {
      cout<<"plotter_x called with unknown lepton category "<<lepCat<<endl;
      return 1;
   }
  
   // Check that the leptcat is not both as we are not ready for it. 
   // Problems if combining:
   // - we have to figure out how to include two lums.
   // - we have to figure out how to cut on one category but not the other.
   if (UserFunctions::leptonCat == DEFS::both) {
      cout<<"plotter_x called with lepton category both. WE HAVE NOT FIXED THIS TO WORK YET."<<lepCat<<endl;
      return 1;
   }

   // Tell ROOT to not print useless INFO
   gErrorIgnoreLevel = kWarning;

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   cout << "Doing lepton category "<<lepCat << endl;

   // The vector containing all plots to be made
   MapOfPlots plots = getPlots(UserFunctions::leptonCat,norm_data);
   
   // The vector holding all processes.
   vector <PhysicsProcess*> procs = DefaultValues::getProcessesHiggs(jetBin, DEFS::pretag,
                                                                     true, true, 
                                                                     ntupleType); //DEFS::EventNtuple);// DEFS::MicroNtuple);
      
   // Report Scale Factors
   for (unsigned p = 0; p< procs.size(); p++)
     cout<<"Process "<<procs[p]->name<<"\t will be scaled by "<<procs[p]->getScaleFactor(DEFS::electron)<<endl;


   // Fill all the plots 
   doPlotter(plots, procs, UserFunctions::doJER, UserFunctions::doPUreweight, UserFunctions::doCSVreweight,
             UserFunctions::doFNAL, maxEvts, UserFunctions::WJweight, MVAWeightDir,
             MVAMethods, UserFunctions::verbose);
   
   // Write output to file. The processs are passed to obtain the canvases
   if(!UserFunctions::outDir.EndsWith("/")) UserFunctions::outDir+="/";
   if(!gSystem->OpenDirectory(UserFunctions::outDir)) gSystem->mkdir(UserFunctions::outDir);
   TString histoFileName = UserFunctions::outDir + "histos_" + lepCat + ".root";
   TString canvasFileName = UserFunctions::outDir + "canvases_" + lepCat+".root";
   writePlotsToFile(histoFileName, canvasFileName, plots, procs);

   m_benchmark->Stop("event");
   cout << "plotter_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;

}//plotter()


////////////////////////////////////////////////////////////////////////////////
//  Implement Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void writePlotsToFile(TString histoFileName, TString canvasFileName, 
                      MapOfPlots & plots,  vector<PhysicsProcess*> procs){


   //Get the canvas and write them to file and as png and eps
   cout<<"Writing canvas to rootfile "<<canvasFileName<<endl;
   TFile * canOutFile = new TFile(canvasFileName,"RECREATE");
   for ( MapOfPlots::iterator p = plots.begin(); p != plots.end() ; p++)
      for ( map<string,  Plot * >::iterator p2 = p->second.begin(); 
            p2 != p->second.end() ; p2++){
         TCanvas* can = ((FormattedPlot*) p2->second)->getCanvas(procs);
         can->Write();      
         TString canName = UserFunctions::outDir+"/"+can->GetName();
         canName += "_"+DEFS::getLeptonCatString(UserFunctions::leptonCat);
         can->SaveAs(canName+".png");
         can->SaveAs(canName+".eps");
      }
   canOutFile->Close();


   //Get the Histos and write them to file
   //NOTE: this needs to be done AFTER the writing of the canvases as the
   //      scaling of the histos is done at that stage. 
   cout<<"Writing histos to rootfile "<<histoFileName<<endl;
   bool firstH = true;
   for ( MapOfPlots::iterator p = plots.begin(); p != plots.end() ; p++){
      for ( map<string,  Plot * >::iterator p2 = p->second.begin();
            p2 != p->second.end() ; p2++){
	if ( firstH ){
	  ((FormattedPlot*) p2->second)->saveHistogramsToFile(histoFileName,"RECREATE");
	  firstH = false;
	}
	else
	  ((FormattedPlot*) p2->second)->saveHistogramsToFile(histoFileName,"UPDATE");
      }// for histos inside plot
   }// for plots  

}//writePlotsToFile

//______________________________________________________________________________
void doPlotter(MapOfPlots & plots, vector<PhysicsProcess*> procs, bool doJER, bool doPUrewt,
               bool doCSVrewt, bool doFNAL, int maxEvts, bool WJweight, TString MVAWeightDir,
               vector<TString> MVAMethods, bool verbose) {

   PlotFiller pFill(plots, procs, &UserFunctions::fillPlots);
   pFill.setCutFunction(&UserFunctions::eventPassCuts);
   pFill.setWeightFunction(&UserFunctions::weightFunc);
   pFill.setProcessFunction(&UserFunctions::processFunc);
   pFill.setInitializeEventFunction(&UserFunctions::initEventFunc);
   if (maxEvts>0)
      pFill.setMaximumEventsDEBUG(maxEvts); // TEST
   if (!MVAWeightDir.IsNull()) {
      pFill.setMVAWeightDir(MVAWeightDir);
      pFill.setMVAMethods(MVAMethods);
   }
   pFill.run();

}//plotter(.., .., etc)

//______________________________________________________________________________
MapOfPlots getPlotsForLeptonCat(DEFS::LeptonCat leptonCat, bool norm_data){

   MapOfPlots plots;

   FormattedPlot* a = new FormattedPlot;
   double pi = TMath::Pi();

   //Goes in the label and tells us whether we are looking at electrons or muons
   TString lepStr = "_"+DEFS::getLeptonCatString(leptonCat);

   // The overlay of a scaled signal. For signalName pick the groupingName 
   // of one of the processes. Or just "" if you don't want a signal overlayed.
   TString signalName = "ggH+WH+qqH(125)";
   double signalFactor = 500; //1000?

   Double_t leptonptbinslow[9] = {20,25,30,35,40,50,70,100,1000};
   Double_t leptonptbinshigh[10] = {20,50,55,60,65,70,80,100,120,1000};
   Double_t jetptbinslow[9] = {20,25,30,35,40,50,70,100,1000};   
   Double_t jetptbinshigh[10] = {20,50,80,100,110,120,130,140,160,1000};
   Double_t Mjjbinslow[11] = {20,40,50,60,70,80,90,100,200,300,1000};
   Double_t Mjjbinshigh[11] = {20,100,110,120,130,150,180,200,250,350,1000};
   Double_t Mtbinshigh[11] = {0,20,40,60,70,80,100,120,140,200,1000};
   Double_t METbinshigh[10] = {0,40,50,60,70,80,100,120,200,1000};
   Double_t DRlepjet1low[10] = {0.3,0.5,0.9,1.3,1.5,1.7,2.0,2.5,3.0,5.0};
   Double_t DRlepjet1high[11] = {0.3,2.0,2.25,2.5,2.75,3.0,3.25,3.5,4.0,4.5,5.0};
   Double_t etabins[83] = {
     -5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, 
     -3.489, -3.314, -3.139, -2.964, -2.853, -2.650, -2.500, -2.322, -2.172, 
     -2.043, -1.930, -1.830, -1.740, -1.653, -1.566, -1.479, -1.392, -1.305, 
     -1.218, -1.131, -1.044, -0.957, -0.879, -0.783, -0.696, -0.609, -0.522, 
     -0.435, -0.348, -0.261, -0.174, -0.087, 
     +0.000, 
     +0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783, 
     +0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566, 
     +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650, 
     +2.853, +2.964, +3.139, +3.314, +3.489, +3.664, +3.839, +4.013, +4.191, 
     +4.363, +4.538, +4.716, +4.889, +5.191};
   
   Double_t absetabins[42] = {0.000, 
     +0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783, 
     +0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566, 
     +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650, 
     +2.853, +2.964, +3.139, +3.314, +3.489, +3.664, +3.839, +4.013, +4.191, 
     +4.363, +4.538, +4.716, +4.889, +5.191};
   
     
   //goes in the label and tells us what cuts we are applying
   string cut = "_MET > 35, elPt > 30";
   TString cuts = TString(cut);
   TString name = "";

   a = new FormattedPlot;
   name  = "Mjj";
   a->templateHisto = new TH1D(name + lepStr, name,22,40,150);
   a->axisTitles.push_back("M_{jj} [GeV]");
   a->axisTitles.push_back("Number of Events / 5 GeV");
   a->range = make_pair(40.,150.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "MjjmWmT";
   a->templateHisto = new TH1D(name + lepStr, name,500,-250,250);
   a->axisTitles.push_back("M_{jj} - WmT [GeV]");
   a->axisTitles.push_back("Number of Events / GeV");
   a->range = make_pair(-100.,250.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
   
   a = new FormattedPlot;
   name = "LeptPt";
   a->templateHisto = new TH1D(name + lepStr, name , 1000,0,500);
   a->axisTitles.push_back("p_{T}^{lepton} [GeV]");
   a->axisTitles.push_back("Number of Events / 0.5 GeV");
   a->range = make_pair(20.,150.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
 
   a = new FormattedPlot;
   name = "LeptEta";
   a->templateHisto = new TH1D(name + lepStr, name, 82, etabins);
   a->axisTitles.push_back("#eta^{lepton}");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(-3.,3.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "LeptPhi";
   a->templateHisto = new TH1D(name + lepStr, name,70,-3.5,3.5);
   a->axisTitles.push_back("#phi^{lepton} [Radians]");
   a->axisTitles.push_back("Number of Events / 0.1 Radians");
   a->range = make_pair(-3.5,3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "LeptPFIso";
   a->templateHisto = new TH1D(name + lepStr, name,100,0,10);
   a->axisTitles.push_back("lepton PFIso ");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(0,7);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
   

   a = new FormattedPlot;
   name = "MET";
   a->templateHisto = new TH1D(name + lepStr, name,1000,0,500);
   a->axisTitles.push_back("Missing E_{T} [GeV]");
   a->axisTitles.push_back("Number of Events / 5 GeV");
   a->range = make_pair(30.,150.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "MET_vs_LeptEta";
   a->templateHisto = new TH2D(name + lepStr, name, 82, etabins, 1000,0,500);
   a->axisTitles.push_back("Lepton #eta");
   a->axisTitles.push_back("MET");
   a->axisTitles.push_back("Number of Events" );
   a->range = make_pair(-4,4);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "MET_vs_AbsLeptEta";
   a->templateHisto = new TH2D(name + lepStr, name, 41, absetabins, 1000,0,500);
   a->axisTitles.push_back("Lepton #eta");
   a->axisTitles.push_back("MET");
   a->axisTitles.push_back("Number of Events" );
   a->range = make_pair(-4,4);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "METPhi";
   a->templateHisto = new TH1D(name + lepStr, name, 62, -TMath::Pi(), TMath::Pi());
   a->axisTitles.push_back("Missing E_{T} Phi ");
   a->axisTitles.push_back("Number of Events" );
   a->range = make_pair(-3.5, 3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
   
   a = new FormattedPlot;
   name = "WmT";
   a->templateHisto = new TH1D(name + lepStr, name,1000,0,500);
   a->axisTitles.push_back("M_{T}^{W} [GeV]");
   a->axisTitles.push_back("Number of Events / 5 GeV");
   a->range = make_pair(0.,150.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet1Pt";
   a->templateHisto = new TH1D(name + lepStr, name,200,0,300);
   a->axisTitles.push_back("p_{T}^{jet_{1}} [GeV]");
   a->axisTitles.push_back("Number of Events / 5 GeV");
   a->range = make_pair(20.,200.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet1Eta";
   a->templateHisto = new TH1D(name + lepStr, name,50,-5,5);
   a->axisTitles.push_back("#eta^{jet_{1}} ");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(-3.,3.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet1Phi";
   a->templateHisto = new TH1D(name + lepStr, name,70,-3.5,3.5);
   a->axisTitles.push_back("#phi^{jet_{1}} [Radians]");
   a->axisTitles.push_back("Number of Events / 0.1 Radians");
   a->range = make_pair(-3.5,3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet2Pt";
   a->templateHisto = new TH1D(name + lepStr, name,200,0,300);
   a->axisTitles.push_back("p_{T}^{jet_{2}} [GeV]");
   a->axisTitles.push_back("Number of Events / 5 GeV");
   a->range = make_pair(20.,100.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet2Eta";
   a->templateHisto = new TH1D(name + lepStr, name,50,-5,5);
   a->axisTitles.push_back("#eta^{jet_{2}}");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(-3.,3.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Jet2Phi";
   a->templateHisto = new TH1D(name + lepStr, name,70,-3.5,3.5);
   a->axisTitles.push_back("#phi^{jet_{2}} [Radians]");
   a->axisTitles.push_back("Number of Events / 0.1 Radians");
   a->range = make_pair(-3.5,3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "DeltaEtaJ1J2";
   a->templateHisto = new TH1D(name + lepStr, name,50,0,5);
   a->axisTitles.push_back("#eta^{jet_{1}} - #eta^{jet_{2}} ");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(0.,5.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Ptjj";
   a->templateHisto = new TH1D(name + lepStr, name,100,0,300);
   a->axisTitles.push_back("p_{T}^{jj} [GeV]");
   a->axisTitles.push_back("Number of Events / 10 GeV");
   a->range = make_pair(0.,250.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "j1Pt_Mjj";
   a->templateHisto = new TH1D(name + lepStr, name,500,0,5);
   a->axisTitles.push_back("p_{T}^{jet_{1}}/M_{jj} [GeV]");
   a->axisTitles.push_back("Number of Events / 0.01 GeV");
   a->range = make_pair(0.,2.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "j2Pt_Mjj";
   a->templateHisto = new TH1D(name + lepStr, name,500,0,5);
   a->axisTitles.push_back("p_{T}^{jet_{2}}/M_{jj} [GeV]");
   a->axisTitles.push_back("Number of Events / 0.01 GeV");
   a->range = make_pair(0.,1.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "Mlvjj";
   a->templateHisto = new TH1D(name + lepStr, name,250,0,1000);
   a->axisTitles.push_back("M_{lvjj} [GeV]");
   a->axisTitles.push_back("Number of Events / 10 GeV");
   a->range = make_pair(50.,800.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "DeltaRLepMET";
   a->templateHisto = new TH1D(name + lepStr, name,50,0,10);
   a->axisTitles.push_back("#DeltaR of lep and MET ");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(0.,7.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "EJ1EJ2";
   a->templateHisto = new TH1D(name + lepStr, name,500,700,5000);
   a->axisTitles.push_back("E_{J1} * E_{J2}  [GeV]");
   a->axisTitles.push_back("Number of Events / 10 GeV");
   a->range = make_pair(0.,5000.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name  = "BetaJ1BetaJ2";
   a->templateHisto = new TH1D(name + lepStr, name,10000,0,10);
   a->axisTitles.push_back("#beta_{J1} * #beta_{J2} [GeV]");
   a->axisTitles.push_back("Number of Events / .01 GeV");
   a->range = make_pair(0.9,1.03);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "DeltaRJ1J2";
   a->templateHisto = new TH1D(name + lepStr, name,50,0,10);
   a->axisTitles.push_back("#DeltaR of J1 and J2 ");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(0.,7.);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "AngleJ1J2";
   a->templateHisto = new TH1D(name + lepStr, name,50,0,5);
   a->axisTitles.push_back("Angle between J1 and J2 [Radians]");
   a->axisTitles.push_back("Number of Events / 0.1 Radians");
   a->range = make_pair(-0.5,3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "jjlvPhi";
   a->templateHisto = new TH1D(name + lepStr, name,70,-3.5,3.5);
   a->axisTitles.push_back("#phi(jj) - #phi(lv)");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(-3.5,3.5);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "DeltaPhi_LJ1";
   a->templateHisto = new TH1D(name + lepStr, name,50,-10,10);
   a->axisTitles.push_back("#Delta #phi(LJ1)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   
   a = new FormattedPlot;
   name = "DeltaPhi_J1J2";
   a->templateHisto = new TH1D(name + lepStr, name,50,-10,10);
   a->axisTitles.push_back("#Delta #phi(J1J2)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "DeltaPhi_LMET";
   a->templateHisto = new TH1D(name + lepStr, name,50,-10,10);
   a->axisTitles.push_back("#Delta #phi(Lepton,MET)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
   
   a = new FormattedPlot;
   name = "npv";
   a->templateHisto = new TH1D(name + lepStr, name,100,0,100);
   a->axisTitles.push_back("npv");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(0,40);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaPhi_LJ1_vs_J1J2";
   a->templateHisto = new TH2D(name + lepStr, name, 15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("#Delta #phi(LJ1)");
   a->axisTitles.push_back("#Delta #phi(J1J2)");
   a->axisTitles.push_back("Number of Events" );
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "MWjjVsMWlv";
   a->templateHisto = new TH2D(name + lepStr, name ,200,0,200,200,0,200);
   a->axisTitles.push_back("M_{W_{jj}}");
   a->axisTitles.push_back("M_{W_{l#nu}}");
   a->range = make_pair(0,200);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaPhi_LJ1_vs_J1J2_Positive";
   a->templateHisto = new TH2D(name + lepStr, name,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Positive #Delta #phi(J1J2) vs. #Delta #phi(LJ1)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaPhi_LJ1_vs_J1J2_Negative";
   a->templateHisto = new TH2D(name + lepStr, name ,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Negative #Delta #phi(J1J2) vs. #Delta #phi(LJ1)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name  = "DeltaPhi_LJ1_vs_J1J2_Subtracted";
   a->templateHisto = new TH2D(name + lepStr, name ,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Subtracted #Delta #phi(J1J2) vs. #Delta #phi(LJ1)");
   a->axisTitles.push_back("Number of Events / .2 Radians");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaEta_LJ1_vs_J1J2";
   a->templateHisto = new TH2D(name + lepStr, name,40,-10,10,40,-10,10);
   a->axisTitles.push_back("#Delta #eta(J1J2) vs. #Delta #eta(LJ1)");
   a->axisTitles.push_back("Number of Events / #eta unit");
   a->range = make_pair(-10,10);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaEta_LJ1_vs_J1J2_Positive";
   a->templateHisto = new TH2D(name + lepStr, name,40,-10,10,40,-10,10);
   a->axisTitles.push_back("Positive #Delta #eta(J1J2) vs. #Delta #eta(LJ1)");
   a->axisTitles.push_back("Number of Events / #eta unit");
   a->range = make_pair(-10,10);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   a = new FormattedPlot;
   name = "DeltaEta_LJ1_vs_J1J2_Negative";
   a->templateHisto = new TH2D(name + lepStr, name,40,-10,10,40,-10,10);
   a->axisTitles.push_back("Negative #Delta #eta(J1J2) vs. #Delta #eta(LJ1)");
   a->axisTitles.push_back("Number of Events / #eta unit");
   a->range = make_pair(-10,10);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;


   //////////////test

   a = new FormattedPlot;
   name = "WmT_Negative";
   a->templateHisto = new TH2D(name + lepStr, name ,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Negative WmT");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name  = "WmT_Positive";
   a->templateHisto = new TH2D(name + lepStr, name ,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Positive WmT");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "WmT_Subtracted";
   a->templateHisto = new TH2D(name + lepStr, name ,15,-pi,pi,15,-pi,pi);
   a->axisTitles.push_back("Subtracted WmT");
   a->axisTitles.push_back("Number of Events ");
   a->range = make_pair(-pi,pi);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "nJets_JetEta";
   a->templateHisto = new TH2D(name + lepStr, name,70,-3.5,3.5,20,0,20);
   a->axisTitles.push_back("#eta^{jet_{i}}");
   a->axisTitles.push_back("Number of Jets");
   a->range = make_pair(-TMath::Pi(),TMath::Pi());
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   for (unsigned int nj=0; nj<20; nj++) {
      a = new FormattedPlot;
      name = Form("JetEta_%uJets",nj);
      a->templateHisto = new TH1D(name + lepStr, name,70,-3.5,3.5);
      a->axisTitles.push_back("#eta^{jets} ");
      a->axisTitles.push_back("Number of Events ");
      a->range = make_pair(-pi,pi);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = signalName;
      a->overlaySignalFactor = signalFactor;
      plots[leptonCat][string(name)] = a;
   }

   for (unsigned int tep=0; tep<MicroNtuple::nEventProb; tep++) {
      a = new FormattedPlot;
      name = TString(UserFunctions::concatString("tEventProb",tep));
      string xaxis = UserFunctions::concatString("log10(tEventProb[",tep) + "])";
      a->templateHisto = new TH1D(name + lepStr, name,180,-45,0);
      a->axisTitles.push_back(xaxis);
      a->axisTitles.push_back("Number of Events");
      if (tep==14)
         a->range = make_pair(-25,0);
      else
         a->range = make_pair(-25,0);
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = signalName;
      a->overlaySignalFactor = signalFactor;
      plots[leptonCat][string(name)] = a;
   }

   a = new FormattedPlot;
   name = "MVADiscriminator";
   a->templateHisto = new TH1D(name + lepStr, name,70,-0.05,0.65);
   a->axisTitles.push_back("MVADiscriminator");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(-0.05,0.65);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;
      
   a = new FormattedPlot;
   name = "MVAProbability";
   a->templateHisto = new TH1D(name + lepStr, name, 100,0,1);
   a->axisTitles.push_back("Probability");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(0,1);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "epdPretagWWandWZ";
   a->templateHisto = new TH1D(name + lepStr, name, 40,-10.0,10.0);
   a->axisTitles.push_back("-log10(epdPretagWWandWZ)");
   a->axisTitles.push_back("Number of Events");
   //a->range = make_pair(-7.0,0.0);
   a->range = make_pair(0.0,7.0);
   a->logxy = make_pair(false,false);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "epdPretagHiggs125";
   a->templateHisto = new TH1D(name + lepStr, name, 40,-10.0,10.0);
   a->axisTitles.push_back("-log10(epdPretagHiggs125)");
   a->axisTitles.push_back("Number of Events");
   //a->range = make_pair(-7.0,0.0);
   a->range = make_pair(0.0,7.0);
   a->logxy = make_pair(false,false);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "epdPretagWWandWZ_RE";
   a->templateHisto = new TH1D(name + lepStr, name, 22,0,1);
   a->axisTitles.push_back("EPD(WW,WZ)");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(0,1);
   a->logxy = make_pair(false,false);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   a = new FormattedPlot;
   name = "DeltaPhi_METJ1";
   a->templateHisto = new TH1D(name + lepStr, name, 50,-10,10);
   a->axisTitles.push_back("#Delta #phi(MET,J1)");
   a->axisTitles.push_back("Number of Events");
   a->range = make_pair(-pi,pi);
   a->logxy = make_pair(false,false);
   a->normToData = norm_data;
   a->stacked = true; a->leptonCat = DEFS::electron;
   a->overlaySignalName = signalName;
   a->overlaySignalFactor = signalFactor;
   plots[leptonCat][string(name)] = a;

   if (UserFunctions::fillTMDF){
/*
  a = new FormattedPlot;
  name = "lpt_leta_j1pt_j1eta_j2pt_j2eta";
  a->templateHisto = new TProfileMDF(name + lepStr, name);
  ((TProfileMDF*)a->templateHisto)->AddAxis("lpt",8,leptonptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("leta",10,0,2.5);
  ((TProfileMDF*)a->templateHisto)->AddAxis("j1pt",8,jetptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("j1eta",10,0,2.5);
  ((TProfileMDF*)a->templateHisto)->AddAxis("j2pt",8,jetptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("j2eta",10,0,2.5);
  ((TProfileMDF*)a->templateHisto)->Sumw2();
  a->normToData = norm_data;
  a->stacked = true; a->leptonCat = DEFS::electron;
  a->overlaySignalName = signalName;
  a->overlaySignalFactor = signalFactor;
  plots[leptonCat][string(name)] = a;
     

  name = "lpt_lpt_j1pt_j1pt_j2pt_j2pt";
  a->templateHisto = new TProfileMDF(name + lepStr, name);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{lepton} [GeV]",8,leptonptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{lepton} [GeV]",9,leptonptbinshigh);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{jet_{1}} [GeV]",8,jetptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{jet_{1}} [GeV]",9,jetptbinshigh);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{jet_{2}} [GeV]",8,jetptbinslow);
  ((TProfileMDF*)a->templateHisto)->AddAxis("p_{T}^{jet_{2}} [GeV]",9,jetptbinshigh);
  ((TProfileMDF*)a->templateHisto)->Sumw2();
  a->normToData = norm_data;
  a->stacked = true; a->leptonCat = DEFS::electron;
  a->overlaySignalName = signalName;
  a->overlaySignalFactor = signalFactor;
  plots[leptonCat][string(name)] = a;
*/
      name = "Mjj_Mjj_Mt_MET_DeltaR_DeltaR_";
      a->templateHisto = new TProfileMDF(name + lepStr, name);
      ((TProfileMDF*)a->templateHisto)->AddAxis("M_{jj} [GeV]",10,Mjjbinslow);
      ((TProfileMDF*)a->templateHisto)->AddAxis("M_{jj} [GeV]",10,Mjjbinshigh);
      ((TProfileMDF*)a->templateHisto)->AddAxis("M_{T}^{W} [GeV]",10,Mtbinshigh);
      ((TProfileMDF*)a->templateHisto)->AddAxis("Missing E_{T} [GeV]",9,METbinshigh);
      ((TProfileMDF*)a->templateHisto)->AddAxis("#DeltaR(#mu,jet1) [Radians]",9,DRlepjet1low);
      ((TProfileMDF*)a->templateHisto)->AddAxis("#DeltaR(#mu,jet1) [Radians]",10,DRlepjet1high);
      ((TProfileMDF*)a->templateHisto)->Sumw2();
      a->normToData = norm_data;
      a->stacked = true; a->leptonCat = DEFS::electron;
      a->overlaySignalName = signalName;
      a->overlaySignalFactor = signalFactor;
      plots[leptonCat][string(name)] = a;
   }

   // return all the plots to be made
   return plots;

}//getPlotsForLeptonCat


//______________________________________________________________________________
MapOfPlots getPlots(DEFS::LeptonCat leptonCat, bool norm_data){

   MapOfPlots plots;

   if (leptonCat == DEFS::electron || leptonCat == DEFS::both){
      MapOfPlots plots_ele = getPlotsForLeptonCat(DEFS::electron, norm_data);
      plots.insert(plots_ele.begin(),plots_ele.end());
   }
   if (leptonCat == DEFS::muon || leptonCat == DEFS::both){
      MapOfPlots plots_muo = getPlotsForLeptonCat(DEFS::muon, norm_data);
      plots.insert(plots_muo.begin(),plots_muo.end());
   }

   return plots;
}
