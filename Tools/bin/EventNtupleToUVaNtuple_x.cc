//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"
#include "TAMUWW/Tools/interface/CSVreweight.hh"
#include "TAMUWW/Tools/interface/TTbarreweight.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "SHyFT/TemplateMakers/bin/AngularVars.h"
#include "SHyFT/TemplateMakers/bin/KinFit.h"

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
#include "Riostream.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <stdlib.h>

using namespace std;
using DEFS::LeptonCat;

////////////////////////////////////////////////////////////////////////////////
//  Declare Local Functions
////////////////////////////////////////////////////////////////////////////////

///Progress bar
static inline void loadbar2(unsigned int x, unsigned int n, unsigned int w = 50);

/// Setup tree for UVa Type Ntuple
void setupTree(TTree* uvaNtuple,map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches);

/// Set initial Ntuple values. These will be the values assuming no other value is set in the future.
void setInitialValues(map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches);

/// Print a summary output of the TBenchmark
void printSummary(TBenchmark* bench, int precision, Float_t &rt, Float_t &cp, bool event = false);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
 int main(int argc,char**argv) {
   // evaluate command-line / configuration file options
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   TString         ifilePath   = cl.getValue<TString>  ("ifilePath",                                         "./");
   vector<TString> ifiles      = cl.getVector<TString> ("ifiles",                                       "WW:::WZ");
   TString         ifileFormat = cl.getValue<TString>  ("ifileFormat",                                    ".root");
   TString         intupleName = cl.getValue<TString>  ("inutpleName",                                "EvtNtuple");
   TString         itreeName   = cl.getValue<TString>  ("itreeName",                                     "jets2p");
   TString         ifolder     = cl.getValue<TString>  ("ifolder",                                           "PS");
   TString         ofilePath   = cl.getValue<TString>  ("ofilePath",                                         "./");
   vector<TString> ofiles      = cl.getVector<TString> ("ofiles",                               "WW_UVa:::WZ_UVa");
   TString         ofileFormat = cl.getValue<TString>  ("ofileFormat",                                    ".root");
   TString         otreeName   = cl.getValue<TString>  ("otreeName",                                    "anaTree");
   string          lepCat      = cl.getValue<string>   ("leptonCat",                                       "both");
   DEFS::LeptonCat leptonCat   = DEFS::getLeptonCat    (lepCat);
   bool            debug       = cl.getValue<bool>     ("debug",                                            false);

   if (!cl.check()) return 0;
   cl.print();
   
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("program");

   TBenchmark* func_benchmark = new TBenchmark();
   func_benchmark->Reset();

   TBenchmark* event_benchmark = new TBenchmark();
   event_benchmark->Reset();

   if(!ifilePath.EndsWith("/")) ifilePath += "/";
   if(!ofilePath.EndsWith("/")) ofilePath += "/";

   if(ofiles.size()>0 && ofiles.size()!=ifiles.size()) {
      Error("EventNtupleToUVANtuple","The ifiles and ofiles vectors  are not of the same size.");
      return 0;
   }
   if(ofiles.size()==0) {
      for(unsigned int f=0; f<ifiles.size(); f++) {
         ofiles.push_back(ifiles[f]+"_UVa");
      }
   }

   EventNtuple *ntuple = new EventNtuple();
   CSVreweight* csvweight = new CSVreweight();
   TTbarreweight* ttbarweight = new TTbarreweight();
   PUreweight* puweight = 0;

   for(unsigned int f=0; f<ifiles.size(); f++) {
      func_benchmark->Start("setup");
      TFile* ifile = TFile::Open(ifilePath+ifiles[f]+ifileFormat);
      ifile->cd(ifolder);

      //Set up date and Root Chain
      TChain *itree = (TChain*)gDirectory->Get(itreeName);
      itree->SetBranchAddress(intupleName, &ntuple);
      int nEventNtuple = static_cast<int>(itree->GetEntries());
      if(debug)
        nEventNtuple = 100;

      TFile* ofile = new TFile(ofilePath+ofiles[f]+ofileFormat,"RECREATE");
      ofile->cd();
      
      std::map<TString, double *> doubleBranches;
      std::map<TString, unsigned *> uintBranches;

      TTree *uvaNtuple = new TTree(otreeName,otreeName);
      setupTree(uvaNtuple,doubleBranches,uintBranches);

      if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("Full")) {
        //string dataname = DefaultValues::getConfigPath()+"pileup12_noTrig.root";
        string dataname = DefaultValues::getConfigPath()+"pileup12_noTrig_minBiasXsec69400_coarseBinning_withAdditionalNPVHist.root";
        string MCname   = string(ifilePath+ifiles[f]+ifileFormat);
        if(ifiles[f].Contains("Dp6p7")) {
           cout << "ERROR EventNtupleToUVaNtuple_x::main() PU reweighting for QCD_ElEnriched has not been implemented yet." << endl;
           return 0;
        }
        else {
           puweight = new PUreweight(dataname,MCname,"pileup_noTrig","PS/TPUDist",make_pair(1,10));
        }
      }
      func_benchmark->Stop("setup");
      for(int i=0; i<nEventNtuple; i++)
      {
         event_benchmark->Start("event");
         func_benchmark->Start("getEntry");
         itree->GetEntry(i);
         func_benchmark->Stop("getEntry");
         if(i==0) {
            assert(ntuple!=0);
         }
         //if (i%1000==0) cout << "\tEntry " << i << " of " << nEventNtuple << endl;
         func_benchmark->Start("loadbar2");
         if(!debug)
           loadbar2(i+1,nEventNtuple);
         func_benchmark->Stop("loadbar2");
         if (leptonCat != ntuple->lLV[0].leptonCat && leptonCat!=DEFS::both) continue;

         func_benchmark->Start("setInitialValues");
         setInitialValues(doubleBranches,uintBranches);
         func_benchmark->Stop("setInitialValues");

         if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("QCD") && !ifiles[f].Contains("Full")) {
            (*doubleBranches["nIntxn"]) = ntuple->vLV[0].npus[1];
            (*doubleBranches["nIntxnT"]) = ntuple->vLV[0].tnpus[1];
         }
         else {
            (*doubleBranches["nIntxn"]) = 0;
            (*doubleBranches["nIntxnT"]) = 0;
         }

         func_benchmark->Start("leptonInfo");
         if(ntuple->lLV[0].leptonCat==DEFS::muon) {
            (*doubleBranches["pfIso"]) = ntuple->lLV[0].lpfIso;
            (*doubleBranches["muPt"]) = ntuple->lLV[0].Pt();         
            (*doubleBranches["muEta"])   = ntuple->lLV[0].Eta();
            (*doubleBranches["muCharge"])   = ntuple->lLV[0].lQ;
            (*doubleBranches["muPhi"])   = ntuple->lLV[0].Phi();
         
            (*doubleBranches["relIso"])  = ntuple->lLV[0].ldetIso;
            (*doubleBranches["hcalIso"]) = ntuple->lLV[0].lhcalIso;
            (*doubleBranches["ecalIso"]) = ntuple->lLV[0].lecalIso;
            (*doubleBranches["trkIso"])  = ntuple->lLV[0].ltrkIso;
         }
         if(ntuple->lLV[0].leptonCat==DEFS::electron) {
             (*doubleBranches["electronEtaAll"]) = ntuple->lLV[0].Eta();

             (*doubleBranches["pfIso"]) = ntuple->lLV[0].lpfIso;
             //(*doubleBranches["pfIso_New"]) = pfIsoNew;
             (*doubleBranches["photonIso"]) = ntuple->lLV[0].lphotonIso;
             //(*doubleBranches["photonIso_New"]) = photonIsoNew;
             (*doubleBranches["chadronIso"]) = ntuple->lLV[0].lchargedHadronIso;
             //(*doubleBranches["chadronIso_New"]) = electron_->pfIsolationVariables().chargedHadronIso;
             (*doubleBranches["nhadronIso"]) = ntuple->lLV[0].lneutralHadronIso;
             //(*doubleBranches["nhadronIso_New"]) = electron_->pfIsolationVariables().neutralHadronIso;
             (*doubleBranches["elePt"]) = ntuple->lLV[0].Pt();
             //(*doubleBranches["elePt_New"]) = electron_->p4().Pt();
             (*doubleBranches["eleMVAID"]) = ntuple->lLV[0].emvaTrig;
             (*doubleBranches["eleMVAIDloose"]) = ntuple->lLV[0].emvaNonTrig;
             (*doubleBranches["eleSCEta"]) = ntuple->lLV[0].eSuperClusterEta;

             if(ntuple->lLV[0].eIsEE == 1 ){
                (*doubleBranches["elePt_EE"]) = ntuple->lLV[0].Pt();
                (*doubleBranches["eleEta_EE"]) = ntuple->lLV[0].Eta();
                (*doubleBranches["eleCharge_EE"]) = ntuple->lLV[0].lQ;
                (*doubleBranches["elePhi_EE"]) = ntuple->lLV[0].Phi();
                (*doubleBranches["relIso_EE"]) = ntuple->lLV[0].ldetIso;
                (*doubleBranches["hcalIso_EE"]) = ntuple->lLV[0].lhcalIso;
                (*doubleBranches["ecalIso_EE"]) = ntuple->lLV[0].lecalIso;
                (*doubleBranches["trkIso_EE"]) = ntuple->lLV[0].ltrkIso;
                (*doubleBranches["sihih_EE"]) = ntuple->lLV[0].eSigmaIetaIeta;
                (*doubleBranches["hoe_EE"]) = ntuple->lLV[0].eHadronicOverEm;
                (*doubleBranches["dphi_EE"]) = ntuple->lLV[0].eDeltaPhi;
                (*doubleBranches["deta_EE"]) = ntuple->lLV[0].eDeltaEta;
             }
             else if(ntuple->lLV[0].eIsEB == 1){
                (*doubleBranches["elePt_EB"]) = ntuple->lLV[0].Pt();
                (*doubleBranches["eleEta_EB"]) = ntuple->lLV[0].Eta();
                (*doubleBranches["eleCharge_EB"]) = ntuple->lLV[0].lQ;
                (*doubleBranches["elePhi_EB"]) = ntuple->lLV[0].Phi();
                (*doubleBranches["relIso_EB"]) = ntuple->lLV[0].ldetIso;
                (*doubleBranches["hcalIso_EB"]) = ntuple->lLV[0].lhcalIso;
                (*doubleBranches["ecalIso_EB"]) = ntuple->lLV[0].lecalIso;
                (*doubleBranches["trkIso_EB"]) = ntuple->lLV[0].ltrkIso;
                (*doubleBranches["sihih_EB"]) = ntuple->lLV[0].eSigmaIetaIeta;
                (*doubleBranches["hoe_EB"]) = ntuple->lLV[0].eHadronicOverEm;
                (*doubleBranches["dphi_EB"]) = ntuple->lLV[0].eDeltaPhi;
                (*doubleBranches["deta_EB"]) = ntuple->lLV[0].eDeltaEta;
             }
         }
         func_benchmark->Stop("leptonInfo");
         func_benchmark->Start("jetInfo");
         //if(i>=70000 && i<80000) cout << "sfsg1" << endl;
         int nLowJets = 0;
         double sumJetEt = 0.0;
         double minDPhiMetJet = 9e20;
         double dPhiMetJet = 999;
         int nbtagsSSV = 0;  
         int nbtagsTC = 0;  
         int nbtagsCSV =0;
         for(unsigned int j=0; j<ntuple->jLV.size() && j<10; j++) {

            TString histName = "jet"; histName += (j+1);

            (*doubleBranches[histName + "Pt"]) = ntuple->jLV[j].Pt();
            (*doubleBranches[histName + "Eta"]) = ntuple->jLV[j].Eta();
            (*doubleBranches[histName + "Phi"]) = ntuple->jLV[j].Phi();
            (*doubleBranches[histName + "Et"]) = ntuple->jLV[j].Et();

            sumJetEt += ntuple->jLV[j].Pt();

            if (ntuple->lLV[0].leptonCat == DEFS::muon){
               (*doubleBranches[histName + "dRMu"]) = ntuple->jLV[j].DRlj;
            }
            
            if (ntuple->lLV[0].leptonCat == DEFS::electron){
               (*doubleBranches[histName + "dREle"]) = ntuple->jLV[j].DRlj;
            }

            double tempDPhi = ntuple->METLV[0].DeltaPhi(ntuple->jLV[j]);
            if ( fabs(tempDPhi) < minDPhiMetJet ) minDPhiMetJet = tempDPhi;
            if((j+1) == 1 ){dPhiMetJet = ntuple->METLV[0].DeltaPhi(ntuple->jLV[j].Phi());}

            (*doubleBranches[histName + "discrCSV"]) = ntuple->jLV[j].jBtagDiscriminatorCSV;

            if ( ntuple->jLV[j].Pt() >= 20 && ntuple->jLV[j].Pt() <30) nLowJets++;

            if (ntuple->jLV[j].jBtagSSV) nbtagsSSV++;
            if (ntuple->jLV[j].jBtagTC) nbtagsTC++;
            if (ntuple->jLV[j].jBtagCSV) nbtagsCSV++;
         }

         if(ntuple->jLV.size() >= 2) {
            (*doubleBranches["Mjj"]) = (ntuple->jLV[0]+ntuple->jLV[1]).M();
            TLorentzVector m2jjTemp = ntuple->jLV[0] + ntuple->jLV[1] ;
            (*doubleBranches["WjjPt"]) = m2jjTemp.Pt();
            (*doubleBranches["dEtaJetJet"]) = fabs( ntuple->jLV[0].Eta() - ntuple->jLV[1].Eta() );
            (*doubleBranches["EtaJetJet"]) = m2jjTemp.Eta();
            (*doubleBranches["dPhiJetJet"]) = fabs(ntuple->jLV[0].DeltaPhi(ntuple->jLV[1]));
            TLorentzVector MWWjjTemp = ntuple->lLV[0] + ntuple->METLV[0] + m2jjTemp;
            (*doubleBranches["MWWTop2jets"]) = MWWjjTemp.M();
            double dPhilj_f = 0;
            if ( fabs(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0])) < fabs(ntuple->lLV[0].DeltaPhi(ntuple->jLV[1])) ) {
               dPhilj_f = fabs(ntuple->lLV[0].DeltaPhi(ntuple->jLV[0]));
            }
            else {  
               dPhilj_f = fabs(ntuple->lLV[0].DeltaPhi(ntuple->jLV[1]));
            }
            (*doubleBranches["minDPhiLepJet"]) = dPhilj_f ;
            (*doubleBranches["dPhiMetJet"]) = dPhiMetJet;
            (*doubleBranches["Ptlnujj"]) = MWWjjTemp.Pt();
            (*doubleBranches["JacobePeak"]) = ntuple->jLV[1].Pt() / ((ntuple->jLV[0]+ntuple->jLV[1]).M()); 

            double minDeta = 9999;
            for (int iJet = 0; iJet < (int)ntuple->jLV.size()-1; ++iJet) {
               for (int jJet = iJet+1; jJet < (int)ntuple->jLV.size(); ++jJet) {
                  double deltaEta = fabs( ntuple->jLV[iJet].Eta() - ntuple->jLV[jJet].Eta() );
                  if ( deltaEta < minDeta ) {
                     minDeta = deltaEta ;
                     TLorentzVector m2jj_an = ntuple->jLV[iJet] + ntuple->jLV[jJet];
                     double dRljj = ntuple->lLV[0].DeltaR(m2jj_an);
                     (*doubleBranches["dRlepjj"]) = dRljj;
                  }
               }
            }
         }
         func_benchmark->Stop("jetInfo");
         func_benchmark->Start("misc.");
         (*doubleBranches["nPV"]) = ntuple->vLV[0].npv;
         
         (*doubleBranches["minDPhiMetJet"]) = minDPhiMetJet;
         (*doubleBranches["sumJetEt"])   = sumJetEt;

         (*doubleBranches["nJets"])   = min((int)ntuple->jLV.size(),10);
         (*doubleBranches["nLowJets"]) = nLowJets;
         (*doubleBranches["met"])     = ntuple->METLV[0].Pt();
         (*doubleBranches["metPhi"])     = ntuple->METLV[0].Phi();
         TLorentzVector metLV = ntuple->METLV[0];
         metLV.SetPz(0.); //Just to be safe--MET should never have Z component.  That's why it's called ME*T*!
         metLV.SetE(ntuple->METLV[0].Pt());
         TLorentzVector leptonTrans = ntuple->lLV[0];
         leptonTrans.SetPz(0.);
         leptonTrans.SetE(ntuple->lLV[0].Pt());
         TLorentzVector WTrans = leptonTrans + metLV;
         (*doubleBranches["mT"])      = WTrans.M();

         //Calculate Ht using all available jets
         double ht = 0.0;
         if (ntuple->lLV.size() > 0) ht += ntuple->lLV[0].Pt();
         //	 ht += muon_->pt();
         
         
         double htLep = ht + ntuple->METLV[0].Pt(); //htLep = only lepPt and MET
         
         for (unsigned int iJet = 0; iJet < ntuple->jLV.size(); ++iJet) {
            ht += ntuple->jLV[iJet].Et();
         }
         
         double htAll = ht + ntuple->METLV[0].Pt(); //htAll = everything: lepPt + MET + sum(jetEt)
         
         (*doubleBranches["ht"])      = ht;
         (*doubleBranches["htLep"])   = htLep;
         (*doubleBranches["htAll"])   = htAll;

         (*uintBranches["run"])       = ntuple->run;
         //(*uintBranches["lumi"])      = ev.id().luminosityBlock();
         (*uintBranches["event"])     = ntuple->event;
         
         //Robin
         (*doubleBranches["dPhiMetLep"]) = fabs(ntuple->METLV[0].DeltaPhi(ntuple->lLV[0]));
         // (*doubleBranches["Mjj_an"]) = m2jj;
         // (*doubleBranches["WjjPt_an"]) = m2jj_pt;
         // (*doubleBranches["dEtaJetJet_an"]) = dEtajj ;
         // (*doubleBranches["dPhiJetJet_an"]) = dPhijj ;
         // (*doubleBranches["MWWTop2jets_an"]) = mWWjj ;
         // (*doubleBranches["minDPhiLepJet_an"]) = dPhilj ;
         // (*doubleBranches["Ptlnujj_an"]) = Ptlnujj;
         // (*doubleBranches["JacobePeak_an"]) = Jacobe
         func_benchmark->Stop("misc.");
         func_benchmark->Start("puweight");
         if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("Full")) {
            double weight = puweight->getWeight(ntuple->vLV[0].npus[1]);
            double weight_true = puweight->getWeight(ntuple->vLV[0].tnpus[1]);
            (*doubleBranches["weight"]) = weight;
            (*doubleBranches["weight_true"]) = weight_true;
         }
         else{
            (*doubleBranches["weight"]) = 0;
            (*doubleBranches["weight_true"]) = 0;
         }
         func_benchmark->Stop("puweight");
         // (*doubleBranches["Pt1st"]) = Pt1st ;
         // (*doubleBranches["Pt2nd"]) = Pt2nd ;
         // (*doubleBranches["Pt3rd"]) = Pt3rd ;
         
         // (*doubleBranches["discrTCmax"]) = discrTCmax ;
         // (*doubleBranches["discrTC2nd"]) = discrTC2nd ;
         func_benchmark->Start("btag");
         (*doubleBranches["nBTagsSSV"]) = nbtagsSSV;
         (*doubleBranches["nBTagsTC"]) = nbtagsTC;
         (*doubleBranches["nBTagsCSV"]) = nbtagsCSV;
         if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("QCD") && !ifiles[f].Contains("Full"))
          (*doubleBranches["csvWeight"]) = csvweight->getWeight(ntuple);
         else
          (*doubleBranches["csvWeight"]) = 1.0;
         func_benchmark->Stop("btag");
         func_benchmark->Start("ttbarweight");
         if(ifiles[f].Contains("TTbar") || ifiles[f].Contains("TTBar") || ifiles[f].Contains("TTJets"))
          (*doubleBranches["top_weight"]) = ttbarweight->getWeight(ntuple,0);
         else
          (*doubleBranches["top_weight"]) = 1.0;
         func_benchmark->Stop("ttbarweight");

         func_benchmark->Start("jetInfoSize2");
         if(ntuple->jLV.size() >= 2) {

            //------solve neutrino Pz                                                                                           
            double real = 1.0 ;
            double massW = 80.4 ;
            double massMuon = 0.1;
            double nvPz1 = 0;
            double nvPz2 = 0;
            double coeffA = (massW*massW - massMuon*massMuon)/2 + ntuple->lLV[0].Px()*ntuple->METLV[0].Px() + ntuple->lLV[0].Py()*ntuple->METLV[0].Py();
            double coeffa= ntuple->lLV[0].E()*ntuple->lLV[0].E() - ntuple->lLV[0].Pz()*ntuple->lLV[0].Pz();
            double coeffb = (-2)*coeffA*ntuple->lLV[0].Pz() ;
            double coeffc = ntuple->lLV[0].E()*ntuple->lLV[0].E()*(ntuple->METLV[0].Px()*ntuple->METLV[0].Px()+ntuple->METLV[0].Py()*ntuple->METLV[0].Py()) - coeffA*coeffA;
            double rootDelta = coeffb*coeffb - 4*coeffa*coeffc ;

            // imaginary roots                                                                                                  
            if (rootDelta<0){
               real = 0;
               nvPz1 = (-1)*coeffb/(2*coeffa);
               nvPz2 = (-1)*coeffb/(2*coeffa);
            }
            //real roots                                                                                                        
            else {
               double root1 = (-1)*coeffb/(2*coeffa) + sqrt(rootDelta)/(2*coeffa);
               double root2 = (-1)*coeffb/(2*coeffa) - sqrt(rootDelta)/(2*coeffa);
               if (fabs(root1)>fabs(root2)) {
                  nvPz1= root2; nvPz2= root1 ;
               }
               else {
                  nvPz1= root1; nvPz2= root2 ;
               }
            }

            (*doubleBranches["real"]) = real;

            double nvEt = TMath::Sqrt(ntuple->METLV[0].E()*ntuple->METLV[0].E() + nvPz1*nvPz1);
            TLorentzVector pu(ntuple->lLV[0].Px(),ntuple->lLV[0].Py(),ntuple->lLV[0].Pz(),ntuple->lLV[0].E());
            TLorentzVector pv(ntuple->METLV[0].Px(),ntuple->METLV[0].Py(),nvPz1,nvEt);
            TLorentzVector pj1(ntuple->jLV[0].Px(),ntuple->jLV[0].Py(),ntuple->jLV[0].Pz(),ntuple->jLV[0].E());
            TLorentzVector pj2(ntuple->jLV[1].Px(),ntuple->jLV[1].Py(),ntuple->jLV[1].Pz(),ntuple->jLV[1].E());

            //KinFit vars
            float cosdPhiWW=0.; float costhetal=0.; float costhetaj=0.;
            float cosdPhiWH=0.; float costhetaWH=0.; float jacksonAngle = 0.;

            double mWWjj_KF = 0 ;
            double mJJ_KF   = 0 ;
       
            TLorentzVector      fit_mup(0,0,0,0); 
            TLorentzVector      fit_nvp(0,0,0,0);
            TLorentzVector      fit_ajp(0,0,0,0); 
            TLorentzVector      fit_bjp(0,0,0,0); 
       
            Float_t             fit_chi2;
            Int_t               fit_NDF;
            Int_t               fit_status;
            double chi2 = 0 ;
            double chi2KF = 0 ;
       
            bool isMuon = false ;
            if ( ntuple->lLV[0].leptonCat == DEFS::muon ) isMuon = true ;
       
            doKinematicFit(isMuon, 1,  pu,  pv,  pj1,  pj2, 
                           fit_mup,  fit_nvp, fit_ajp,  fit_bjp, 
                           fit_chi2,  fit_NDF,  fit_status);

            (*doubleBranches["fitStatus"]) = double(fit_status);
            chi2KF = double(fit_chi2/fit_NDF) ;
            mWWjj_KF = (fit_mup + fit_nvp + fit_ajp + fit_bjp).M();
            mJJ_KF   = (fit_ajp + fit_bjp).M();
            chi2 = fit_chi2 ;

            if (fit_status==-10){
               //         nNaN++;
               chi2 = 999. ;
               mWWjj_KF = -999. ;
               chi2KF = 999. ;
            }

            (*doubleBranches["chi2KF"]) = chi2KF;
            (*doubleBranches["chi2"]) = chi2;
            (*doubleBranches["mWWKF"]) = mWWjj_KF;
            (*doubleBranches["mJJKF"]) = mJJ_KF;

            // 
            dg_kin_Wuv_Wjj( pu, pv, pj1, pj2, cosdPhiWW, cosdPhiWH, costhetal, costhetaj, costhetaWH);
            jacksonAngle = JacksonAngle( pj1,pj2);



            (*doubleBranches["fitStatus"]) = double(fit_status);
            
            (*doubleBranches["chi2KF"]) = chi2KF;
            (*doubleBranches["chi2"]) = chi2;
            (*doubleBranches["mWWKF"]) = mWWjj_KF;
            (*doubleBranches["mJJKF"]) = mJJ_KF;
            
            (*doubleBranches["Cos_dPhiWW"]) = cosdPhiWW;
            (*doubleBranches["CosTheta_l"]) = costhetal;
            (*doubleBranches["CosTheta_j"]) = costhetaj;
            (*doubleBranches["Cos_dPhiWH"]) = cosdPhiWH;
            (*doubleBranches["CosTheta_WH"]) = costhetaWH;
            (*doubleBranches["jacksonAngle"]) = jacksonAngle;

         }
         func_benchmark->Stop("jetInfoSize2");
         func_benchmark->Start("fill");
         uvaNtuple->Fill();
         func_benchmark->Stop("fill");
         event_benchmark->Stop("event");
         if(debug) {
           float rt = 0, ct = 0;
           cout << endl << "EventNtupleToUVaNtuple_x::func_benchmark" << endl;
           //func_benchmark->Summary(rt,ct);
           printSummary(func_benchmark, 8, rt, ct, false);
           cout << endl << "EventNtupleToUVaNtuple_x::event_benchmark" << endl;
           //event_benchmark->Summary(rt,ct);
           printSummary(event_benchmark, 8, rt, ct, true);
         }
         func_benchmark->Reset();
         event_benchmark->Reset();
      }
      
      uvaNtuple->AutoSave();
      uvaNtuple->Write();
      ofile->Close();
      ifile->Close();

      delete func_benchmark;
      delete event_benchmark;
   }

   m_benchmark->Stop("program");
   cout << endl << "EventNtupleToUVaNtuple_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("program") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("program") << " s" << endl;
   delete m_benchmark;

   return 0;

}//main

////////////////////////////////////////////////////////////////////////////////
//  Define Local Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void setupTree(TTree* uvaNtuple,map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches){
   doubleBranches["muPt"] = new double(0.0);
   doubleBranches["muEta"] = new double(0.0);
   doubleBranches["muCharge"] = new double(0.0);
   doubleBranches["muPhi"] = new double(0.0);
   doubleBranches["relIso"] = new double(0.0);

   doubleBranches["elePt_EE"] = new double(0.0);
   doubleBranches["eleEta_EE"] = new double(0.0);
   doubleBranches["eleCharge_EE"] = new double(0.0);
   doubleBranches["elePhi_EE"] = new double(0.0);
   doubleBranches["relIso_EE"] = new double(0.0);
   doubleBranches["hcalIso_EE"] = new double(0.0);
   doubleBranches["ecalIso_EE"] = new double(0.0);
   doubleBranches["trkIso_EE"] = new double(0.0);
   doubleBranches["sihih_EE"] = new double(0.0); 
   doubleBranches["dphi_EE"] = new double(0.0);
   doubleBranches["deta_EE"] = new double(0.0);
   doubleBranches["hoe_EE"] = new double(0.0);
   
   doubleBranches["elePt_EB"] = new double(0.0);
   doubleBranches["eleEta_EB"] = new double(0.0);
   doubleBranches["eleCharge_EB"] = new double(0.0);
   doubleBranches["elePhi_EB"] = new double(0.0);
   doubleBranches["relIso_EB"] = new double(0.0);
   doubleBranches["hcalIso_EB"] = new double(0.0);
   doubleBranches["ecalIso_EB"] = new double(0.0);
   doubleBranches["trkIso_EB"] = new double(0.0);
   doubleBranches["sihih_EB"] = new double(0.0);
   doubleBranches["dphi_EB"] = new double(0.0);
   doubleBranches["deta_EB"] = new double(0.0);
   doubleBranches["hoe_EB"] = new double(0.0);
   doubleBranches["eleMVAID"] = new double(0.0);
   doubleBranches["eleMVAIDloose"] = new double(0.0);
   doubleBranches["eleSCEta"] = new double(0.0);


   doubleBranches["electronEtaAll"] = new double(0.0);
   doubleBranches["discrTCmax"] = new double(0.0);
   doubleBranches["discrTC2nd"] = new double(0.0);
   doubleBranches["nBTagsSSV"] = new double(0.0);
   doubleBranches["nBTagsTC"] = new double(0.0);
   doubleBranches["nBTagsCSV"] = new double(0.0);
   doubleBranches["csvWeight"] = new double(0.0);

   doubleBranches["top_weight"] = new double(0.0);

   doubleBranches["jet1discrCSV"] = new double(0.0);
   doubleBranches["jet2discrCSV"] = new double(0.0);
   doubleBranches["jet3discrCSV"] = new double(0.0);
   doubleBranches["jet4discrCSV"] = new double(0.0);
   doubleBranches["jet5discrCSV"] = new double(0.0);
   doubleBranches["jet6discrCSV"] = new double(0.0);
   doubleBranches["jet7discrCSV"] = new double(0.0);
   doubleBranches["jet8discrCSV"] = new double(0.0);
   doubleBranches["jet9discrCSV"] = new double(0.0);
   doubleBranches["jet10discrCSV"] = new double(0.0);

   doubleBranches["jet1Pt"] = new double(0.0);
   doubleBranches["jet2Pt"] = new double(0.0);
   doubleBranches["jet3Pt"] = new double(0.0);
   doubleBranches["jet4Pt"] = new double(0.0);
   doubleBranches["jet5Pt"] = new double(0.0);
   doubleBranches["jet6Pt"] = new double(0.0);
   doubleBranches["jet7Pt"] = new double(0.0);
   doubleBranches["jet8Pt"] = new double(0.0);
   doubleBranches["jet9Pt"] = new double(0.0);
   doubleBranches["jet10Pt"] = new double(0.0);

   doubleBranches["jet1Eta"] = new double(0.0);
   doubleBranches["jet2Eta"] = new double(0.0);
   doubleBranches["jet3Eta"] = new double(0.0);
   doubleBranches["jet4Eta"] = new double(0.0);
   doubleBranches["jet5Eta"] = new double(0.0);
   doubleBranches["jet6Eta"] = new double(0.0);
   doubleBranches["jet7Eta"] = new double(0.0);
   doubleBranches["jet8Eta"] = new double(0.0);
   doubleBranches["jet9Eta"] = new double(0.0);
   doubleBranches["jet10Eta"] = new double(0.0);

   doubleBranches["jet1Phi"] = new double(0.0);
   doubleBranches["jet2Phi"] = new double(0.0);
   doubleBranches["jet3Phi"] = new double(0.0);
   doubleBranches["jet4Phi"] = new double(0.0);
   doubleBranches["jet5Phi"] = new double(0.0);
   doubleBranches["jet6Phi"] = new double(0.0);
   doubleBranches["jet7Phi"] = new double(0.0);
   doubleBranches["jet8Phi"] = new double(0.0);
   doubleBranches["jet9Phi"] = new double(0.0);
   doubleBranches["jet10Phi"] = new double(0.0);

   doubleBranches["jet1Et"] = new double(0.0);
   doubleBranches["jet2Et"] = new double(0.0);
   doubleBranches["jet3Et"] = new double(0.0);
   doubleBranches["jet4Et"] = new double(0.0);
   doubleBranches["jet5Et"] = new double(0.0);
   doubleBranches["jet6Et"] = new double(0.0);
   doubleBranches["jet7Et"] = new double(0.0);
   doubleBranches["jet8Et"] = new double(0.0);
   doubleBranches["jet9Et"] = new double(0.0);
   doubleBranches["jet10Et"] = new double(0.0);

   doubleBranches["sumJetEt"] = new double(0.0);

   doubleBranches["hcalIso"] = new double(0.0);
   doubleBranches["ecalIso"] = new double(0.0);
   doubleBranches["trkIso"] = new double(0.0);
   doubleBranches["pfIso"] = new double(0.0);
   doubleBranches["pfIso_New"] = new double(0.0);
   doubleBranches["photonIso"] = new double(0.0);
   doubleBranches["photonIso_New"] = new double(0.0);
   doubleBranches["chadronIso"] = new double(0.0);
   doubleBranches["chadronIso_New"] = new double(0.0);
   doubleBranches["nhadronIso"] = new double(0.0);
   doubleBranches["nhadronIso_New"] = new double(0.0);
   doubleBranches["elePt"] = new double(0.0);
   doubleBranches["elePt_New"] = new double(0.0);

   doubleBranches["nJets"] = new double(0.0);
   doubleBranches["nLowJets"] = new double(0.0);
   doubleBranches["met"] = new double(0.0);
   doubleBranches["metPhi"] = new double (0.0);
   doubleBranches["mT"] = new double(0.0);
   doubleBranches["ht"] = new double(0.0);
   doubleBranches["htLep"] = new double(0.0);
   doubleBranches["htAll"] = new double(0.0);

   doubleBranches["minDPhiMetJet"] = new double(0.0);
   doubleBranches["dPhiMetJet"] = new double(0.0);

   doubleBranches["jet1dRMu"] = new double(0.0);
   doubleBranches["jet2dRMu"] = new double(0.0);
   doubleBranches["jet3dRMu"] = new double(0.0);
   doubleBranches["jet4dRMu"] = new double(0.0);
   doubleBranches["jet5dRMu"] = new double(0.0);
   doubleBranches["jet6dRMu"] = new double(0.0);
   doubleBranches["jet7dRMu"] = new double(0.0);
   doubleBranches["jet8dRMu"] = new double(0.0);
   doubleBranches["jet9dRMu"] = new double(0.0);
   doubleBranches["jet10dRMu"] = new double(0.0);



   doubleBranches["jet1dREle"] = new double(0.0);
   doubleBranches["jet2dREle"] = new double(0.0);
   doubleBranches["jet3dREle"] = new double(0.0);
   doubleBranches["jet4dREle"] = new double(0.0);
   doubleBranches["jet5dREle"] = new double(0.0);
   doubleBranches["jet6dREle"] = new double(0.0);
   doubleBranches["jet7dREle"] = new double(0.0);
   doubleBranches["jet8dREle"] = new double(0.0);
   doubleBranches["jet9dREle"] = new double(0.0);
   doubleBranches["jet10dREle"] = new double(0.0);

   uintBranches["run"] = new unsigned(0);
   uintBranches["lumi"] = new unsigned(0);
   uintBranches["event"] = new unsigned(0);

   //Robin new variables
   doubleBranches["weight"] = new double(0.0);
   doubleBranches["weight_true"] = new double(0.0);
   doubleBranches["dPhiMetLep"] = new double(0.0);
   //
   doubleBranches["Mjj_an"] = new double(0.0);
   doubleBranches["WjjPt_an"] = new double(0.0);
   doubleBranches["dEtaJetJet_an"] = new double(0.0);
   doubleBranches["dPhiJetJet_an"] = new double(0.0);

   doubleBranches["Ptlnujj_an"] = new double(0.0);
   doubleBranches["MWWTop2jets_an"] = new double(0.0);
   doubleBranches["minDPhiLepJet_an"] = new double(0.0);
   
   doubleBranches["JacobePeak_an"] = new double(0.0);

   doubleBranches["Pt1st"] = new double(0.0);
   doubleBranches["Pt2nd"] = new double(0.0);
   doubleBranches["Pt3rd"] = new double(0.0);

   doubleBranches["dRlepjj"] = new double(0.0);

   doubleBranches["Mjj"] = new double(0.0);
   doubleBranches["WjjPt"] = new double(0.0);
   doubleBranches["dEtaJetJet"] = new double(0.0);
   doubleBranches["EtaJetJet"] = new double(0.0);
   doubleBranches["dPhiJetJet"] = new double(0.0);
   doubleBranches["MWWTop2jets"] = new double(0.0);
   doubleBranches["minDPhiLepJet"] = new double(0.0);
   doubleBranches["Ptlnujj"] = new double(0.0);

   doubleBranches["JacobePeak"] = new double(0.0);

   doubleBranches["Cos_dPhiWW"] = new double(0.0);
   doubleBranches["CosTheta_l"] = new double(0.0);
   doubleBranches["CosTheta_j"] = new double(0.0);
   doubleBranches["Cos_dPhiWH"] = new double(0.0);
   doubleBranches["CosTheta_WH"] = new double(0.0);
   doubleBranches["jacksonAngle"] = new double(0.0);

   doubleBranches["chi2KF"] = new double(0.0);
   doubleBranches["chi2"] = new double(0.0);
   doubleBranches["mWWKF"] = new double(0.0);
   doubleBranches["mJJKF"] = new double(0.0);
   doubleBranches["fitStatus"] = new double(0.0);
   doubleBranches["nPV"] = new double(0.0);
   doubleBranches["nIntxn"] = new double(0.0);
   doubleBranches["nIntxnT"] = new double(0.0);

   //---neutrino Pz                                                                                                        
   doubleBranches["real"] = new double(0.0);

   for (std::map<TString, double*>::iterator iDMap = doubleBranches.begin();
        iDMap != doubleBranches.end();
        iDMap ++) {
     
      std::cout << "Creating branch   " << (iDMap->first).Data() << endl;
      TString ttString = iDMap->first;
      ttString += "/D";
      uvaNtuple->Branch(iDMap->first, iDMap->second, ttString);       
   }
     
   for (std::map<TString, unsigned*>::iterator iIMap = uintBranches.begin();
        iIMap != uintBranches.end();
        iIMap ++) {
     
      std::cout << "Creating branch   " << (iIMap->first).Data() << endl;
      TString ttString = iIMap->first;
      ttString += "/i";
      uvaNtuple->Branch(iIMap->first, iIMap->second, ttString);       
   }

}

//______________________________________________________________________________
void setInitialValues(map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches) {
   //cout << "\tEventNtupleToUVaNtuple_x::setInitialValues() Setting the initial values for all variables...";
   for (std::map<TString, double*>::iterator iDMap = doubleBranches.begin();
        iDMap != doubleBranches.end();
        iDMap ++) {
      
      (* (iDMap->second) ) = -9.99e3;
   }
   
   for (std::map<TString, unsigned*>::iterator iIMap = uintBranches.begin();
        iIMap != uintBranches.end();
        iIMap ++) {
      
      (* (iIMap->second)) = 9999; // not a lot of good choices here
   }
   //cout << "DONE" << endl;
}

//______________________________________________________________________________
static inline void loadbar2(unsigned int x, unsigned int n, unsigned int w) {
   if ( (x != n) && (x % (n/100) != 0) ) return;
 
   float ratio  =  x/(float)n;
   int   c      =  ratio * w;
 
   cout << setw(3) << (int)(ratio*100) << "% [";
   for (int x=0; x<c; x++) cout << "=";
   for (unsigned int x=c; x<w; x++) cout << " ";
   cout << "] (" << x << "/" << n << ")\r" << flush;
}

//______________________________________________________________________________
void printSummary(TBenchmark* bench, int precision, Float_t &rt, Float_t &ct, bool event) {
// Prints a summary of all benchmarks.

   vector<TString> bench_names;
   //bench_names.push_back("program");
   if(!event) {
    bench_names.push_back("setup");
    bench_names.push_back("getEntry");
    bench_names.push_back("loadbar2");
    bench_names.push_back("setInitialValues");
    bench_names.push_back("leptonInfo");
    bench_names.push_back("jetInfo");
    bench_names.push_back("misc.");
    bench_names.push_back("puweight");
    bench_names.push_back("btag");
    bench_names.push_back("ttbarweight");
    bench_names.push_back("jetInfoSize2");
    bench_names.push_back("fill");
   }
   else {
     bench_names.push_back("event");
   }

   rt = 0;
   ct = 0;

   for (unsigned int i=0;i<bench_names.size();i++) {
      cout << Form("%-10s: Real Time = %6.*f seconds Cpu Time = %6.*f seconds",(const char*)bench_names[i],
                   precision,bench->GetRealTime(bench_names[i]),precision,bench->GetCpuTime(bench_names[i])) << endl;
      rt += bench->GetRealTime(bench_names[i]);
      ct += bench->GetCpuTime(bench_names[i]);
   }
   cout << Form("%-10s: Real Time = %6.*f seconds Cpu Time = %6.*f seconds","TOTAL",precision,rt,precision,ct) << endl;

}
