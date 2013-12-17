//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"
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

/// Setup tree for UVa Type Ntuple
void setupTree(TTree* uvaNtuple,map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches);

/// Set initial Ntuple values. These will be the values assuming no other value is set in the future.
void setInitialValues(map<TString,double*> &doubleBranches,map<TString,unsigned*> &uintBranches);

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

   if (!cl.check()) return 0;
   cl.print();
   
   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

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

   for(unsigned int f=0; f<ifiles.size(); f++) {

      TFile* ifile = TFile::Open(ifilePath+ifiles[f]+ifileFormat);
      ifile->cd(ifolder);

      //Set up date and Root Chain
      TChain *itree = (TChain*)gDirectory->Get(itreeName);
      itree->SetBranchAddress(intupleName, &ntuple);
      int nEventNtuple = static_cast<int>(itree->GetEntries());

      TFile* ofile = new TFile(ofilePath+ofiles[f]+ofileFormat,"RECREATE");
      ofile->cd();
      
      std::map<TString, double *> doubleBranches;
      std::map<TString, unsigned *> uintBranches;

      TTree *uvaNtuple = new TTree(otreeName,otreeName);
      setupTree(uvaNtuple,doubleBranches,uintBranches);
      for(int i=0; i<nEventNtuple; i++)
      {
         itree->GetEntry(i);
         if(i==0) {
            assert(ntuple!=0);
         }
         if (i%10000==0) cout << "\tEntry " << i << " of " << nEventNtuple << endl;
         if (leptonCat != ntuple->lLV[0].leptonCat && leptonCat!=DEFS::both) continue;

         setInitialValues(doubleBranches,uintBranches);

         if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("QCD") && !ifiles[f].Contains("Full")) {
            (*doubleBranches["nIntxn"]) = ntuple->vLV[0].npus[1];
            (*doubleBranches["nIntxnT"]) = ntuple->vLV[0].tnpus[1];
         }
         else {
            (*doubleBranches["nIntxn"]) = 0;
            (*doubleBranches["nIntxnT"]) = 0;
         }

         if(ntuple->lLV[0].leptonCat==DEFS::muon) {
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
         if(i>=70000 && i<80000) cout << "sfsg1" << endl;
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
            TLorentzVector MWWjjTemp = ntuple->lLV[0] + ntuple->METLV[0] + m2jjTemp ;
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
            (*doubleBranches["JacobePeak"]) = ntuple->jLV[1].Pt() / ntuple->Mjj; 
         }

         (*doubleBranches["nPV"]) = ntuple->vLV[0].npv;
         
         (*doubleBranches["minDPhiMetJet"]) = minDPhiMetJet;
         (*doubleBranches["sumJetEt"])   = sumJetEt;

         (*doubleBranches["nJets"])   = min((int)ntuple->jLV.size(),10);
         (*doubleBranches["nLowJets"]) = nLowJets;
         (*doubleBranches["met"])     = ntuple->METLV[0].Et() ;
         (*doubleBranches["metPhi"])     = ntuple->METLV[0].Phi();
         TLorentzVector metLV = ntuple->METLV[0];
         metLV.SetPz(0.); //Just to be safe--MET should never have Z component.  That's why it's called ME*T*!
         metLV.SetE(ntuple->METLV[0].Pt());
         TLorentzVector leptonTrans = ntuple->lLV[0];
         leptonTrans.SetPz(0.);
         leptonTrans.SetE(ntuple->lLV[0].Pt());
         TLorentzVector WTrans = leptonTrans + metLV;
         (*doubleBranches["mT"])      = WTrans.M();
/*
         //Calculate Ht using all available jets
         double ht = 0.0;
         if (muPlusJets_ && muons.size() > 0) ht += muons[0].pt();
         if (ePlusJets_ && electrons.size() > 0) ht += electrons[0].pt();
         //	 ht += muon_->pt();
         
         
         double htLep = ht + met.et(); //htLep = only lepPt and MET
         
         for (unsigned int iJet = 0; iJet < jets.size(); ++iJet) {
            const pat::Jet *jet_ = dynamic_cast<const pat::Jet *>(jets[iJet].masterClonePtr().get());
            ht += jet_->et();
         }
         
         double htAll = ht + met.et(); //htAll = everything: lepPt + MET + sum(jetEt)
         
         (*doubleBranches["ht"])      = ht;
         (*doubleBranches["htLep"])   = htLep;
         (*doubleBranches["htAll"])   = htAll;
*/
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

         //FIX!!! FIX!!! FIX!!!
         // (*doubleBranches["dRlepjj"]) = dRljj;
         if(!ifiles[f].Contains("Data") && !ifiles[f].Contains("Full")) {
            PUreweight* puweight;
            string dataname = DefaultValues::getConfigPath()+"pileup12_noTrig.root";
            string MCname   = string(ifilePath+ifiles[f]+ifileFormat);
            if(ifiles[f].Contains("Dp6p7")) {
               cout << "ERROR EventNtupleToUVaNtuple_x::main() PU reweighting for QCD_ElEnriched has not been implemented yet." << endl;
               return 0;
            }
            else {
               puweight = new PUreweight(dataname,MCname,"pileup_noTrig","PS/TPUDist");
            }
            double weight = puweight->getWeight(ntuple->vLV[0].npus[1]);
            double weight_true = puweight->getWeight(ntuple->vLV[0].tnpus[1]);
            (*doubleBranches["weight"]) = weight;
            (*doubleBranches["weight_true"]) = weight_true;
         }
         else{
            (*doubleBranches["weight"]) = 0;
            (*doubleBranches["weight_true"]) = 0;
         }

         // (*doubleBranches["Pt1st"]) = Pt1st ;
         // (*doubleBranches["Pt2nd"]) = Pt2nd ;
         // (*doubleBranches["Pt3rd"]) = Pt3rd ;
         
         // (*doubleBranches["discrTCmax"]) = discrTCmax ;
         // (*doubleBranches["discrTC2nd"]) = discrTC2nd ;
         (*doubleBranches["nBTagsSSV"]) = nbtagsSSV;
         (*doubleBranches["nBTagsTC"]) = nbtagsTC;
         (*doubleBranches["nBTagsCSV"]) = nbtagsCSV;

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

         uvaNtuple->Fill();
      }
      
      uvaNtuple->AutoSave();
      uvaNtuple->Write();
      ofile->Close();
      ifile->Close();
   }

   m_benchmark->Stop("event");
   cout << "EventNtupleToUVaNtuple_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
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
   doubleBranches["eleSCEta"] = new double(0.0);


   doubleBranches["electronEtaAll"] = new double(0.0);
   doubleBranches["discrTCmax"] = new double(0.0);
   doubleBranches["discrTC2nd"] = new double(0.0);
   doubleBranches["nBTagsSSV"] = new double(0.0);
   doubleBranches["nBTagsTC"] = new double(0.0);
   doubleBranches["nBTagsCSV"] = new double(0.0);

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
