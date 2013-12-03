// User includes
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TF1.h"
#include "TLatex.h"
#include "TPave.h"
#include "TPaveText.h"
#include "TBenchmark.h"
#include "THStack.h"
#include "TColor.h"

// C++ libraries
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <stdlib.h>

using namespace std; 

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   vector<TString> ifiles         = cl.getVector<TString> ("ifiles");
   vector<TString> ifilePaths     = cl.getVector<TString> ("ifilePaths",            "./");
   vector<TString> ifilePostFixes = cl.getVector<TString> ("ifilePostFixes",          "");
   TString         ifolder        = cl.getValue<TString>  ("ifolder",               "PS");  
   TString         itree          = cl.getValue<TString>  ("itree",             "jets2p");  
   TString         ofilepath      = cl.getValue<TString>  ("ofilepath"             ,"./");
   TString         ofilename      = cl.getValue<TString>  ("ofilename", "IP_output.root");
   vector<TString> formats        = cl.getVector<TString> ("formats",      ".eps:::.png");
   TString         plots          = cl.getValue<TString>  ("plots",                "all");

   if (!cl.check()) 
      return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   gStyle->SetOptStat(0);

   for(unsigned int i=0; i<ifilePaths.size(); i++) {
      if(!ifilePaths[i].EndsWith("/")) ifilePaths[i] += "/";
   }
   if(!ofilepath.EndsWith("/")) ofilepath += "/";
   plots.ToUpper();

   if(ifilePostFixes.size()==0 || ifilePostFixes.size()!=ifiles.size()) {
      cout << "ERROR::Number of file post fixes is either 0 or is not the same as the number of input files." << endl;
      return 0;
   }

   if(ifilePaths.size()>1 && ifiles.size()!=ifilePaths.size()) {
      cout << "ERROR::Number of file paths is greater than 1 and does not match the number of input files." << endl;
      return 0;
   }

   for(unsigned int f=0; f<ifiles.size(); f++) {
      if(ifilePaths.size()==1)
         ifiles[f] = ifilePaths[0]+ifiles[f];
      else {
         ifiles[f] = ifilePaths[f]+ifiles[f];
      }
   }

   TFile* ofile = new TFile(ofilepath+ofilename,"RECREATE");

   /*******************************************************************************************/
   //Set Containers
   /*******************************************************************************************/
   vector<TCanvas*> cpfIso;
   vector<TCanvas*> cchargedHadronIso;
   vector<TCanvas*> cneutralHadronIso;
   vector<TCanvas*> cphotonIso;
   vector<TCanvas*> cmvaTrigV0;
   vector<TCanvas*> cmvaNonTrigV0;
   vector<TCanvas*> cpfIsoMVATrigV0;
   vector<TCanvas*> cptDistributions;
   vector<TCanvas*> cptDistributionsShifted;
   vector<TCanvas*> cptDistributionsCorrected;
   vector<TCanvas*> cIsoEVsPt;
   vector<TH2D*> hpfIso;
   vector<TH2D*> hchargedHadronIso;
   vector<TH2D*> hneutralHadronIso;
   vector<TH2D*> hphotonIso;
   vector<TH2D*> hmvaTrigV0;
   vector<TH2D*> hmvaNonTrigV0;
   vector<TH2D*> hpfIsoMVATrigV0;
   vector<TH1D*> hptDistributions;
   vector<TH1D*> hptDistributionsShifted;
   vector<TH1D*> hptDistributionsCorrected;
   vector<TH2D*> hIsoEVsPt;
   vector<TProfile*> ppfIso;
   vector<TProfile*> pchargedHadronIso;
   vector<TProfile*> pneutralHadronIso;
   vector<TProfile*> pphotonIso;
   vector<TProfile*> pmvaTrigV0;
   vector<TProfile*> pmvaNonTrigV0;
   vector<TProfile*> ppfIsoMVATrigV0;
   vector<TString> cname;
   vector<TString> hname;
   vector<TString> pname;
   vector<TLegend*> legs;
   vector<double>  binsMVA;

   /*******************************************************************************************/
   //Set MVA Bins
   /*******************************************************************************************/
   int nbinsx = 1000;
   double xmin = .0001;
   double xmax = 1;
   for(int b=0; b<=nbinsx; b++) {
      double bin_boundary = TMath::Power(10,TMath::Log10(xmin)+((double(b)/double(nbinsx))*(TMath::Log10(xmax)-TMath::Log10(xmin)))); 
      binsMVA.push_back(bin_boundary);
//    cout << bin_boundary << ", ";
   }
// cout << endl;
   

   for(unsigned int f=0; f<ifiles.size(); f++) {
      cout << "Working on file " << ifiles[f] << " ..." << endl;

      TFile* ifile = new TFile(ifiles[f],"READ");
      if (!ifile->IsOpen())
      {
         cout << "\tERROR::Could not open file " << ifiles[f] << endl;
         return 0;
      }
      
      if (!ifile->cd(ifolder))
      {
         cout << "\tERROR::Could not CD into directory " << ifolder << " in file " << ifiles[f] << endl;
         return 0;
      }
      
      TTree* t = (TTree*) gDirectory->Get(itree);
      if (t == 0)
      {
         cout << "\tERROR::Could not find tree named " << itree << " in file " << ifiles[f] << endl;
         return 0;
      }

      if(plots.CompareTo("ALL")==0 || plots.CompareTo("COMPONENTS")==0) {

         /*******************************************************************************************/
         //PF Isolation
         /*******************************************************************************************/
         cout << "\tDoing PF Isolation ... " << std::flush;
         cname.push_back(Form("cpfIso_%s",ifilePostFixes[f].Data()));
         cpfIso.push_back(new TCanvas(cname.back(),Form("pfIsoVsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hpfIso_%s",ifilePostFixes[f].Data()));
         hpfIso.push_back(new TH2D(hname.back(),Form("PF Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,400,0,2));
         hpfIso.back()->GetXaxis()->SetTitle("#eta");
         hpfIso.back()->GetYaxis()->SetTitle("PF Isolation");
         pname.push_back(Form("ppfIso_%s",ifilePostFixes[f].Data()));
         ppfIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso:lLV[0].Eta()>>"+hname.back(),"lLV[0].lpfIso>0.2","colz");
            t->Draw("lLV[0].lpfIso:lLV[0].Eta()>>"+pname.back(),"lLV[0].lpfIso>0.2","sames");
            hpfIso.back()->GetYaxis()->SetRangeUser(0.3,2);
         }
         else {
            t->Draw("lLV[0].lpfIso:lLV[0].Eta()>>"+hname.back(),"","colz");
            t->Draw("lLV[0].lpfIso:lLV[0].Eta()>>"+pname.back(),"","sames");
         }
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Charged Hadron Isolation
         /*******************************************************************************************/
         cout << "\tDoing Charged Hadron Isolation ... " << std::flush;
         cname.push_back(Form("cchargedHadronIso_%s",ifilePostFixes[f].Data()));
         cchargedHadronIso.push_back(new TCanvas(cname.back(),Form("chargedHadronIsoVsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hchargedHadronIso_%s",ifilePostFixes[f].Data()));
         hchargedHadronIso.push_back(new TH2D(hname.back(),Form("Charged Hadron Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,120,0,60));
         hchargedHadronIso.back()->GetXaxis()->SetTitle("#eta");
         hchargedHadronIso.back()->GetYaxis()->SetTitle("Charged Hadron Isolation");
         pname.push_back(Form("pchargedHadronIso_%s",ifilePostFixes[f].Data()));
         pchargedHadronIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         t->Draw("lLV[0].lchargedHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
         t->Draw("lLV[0].lchargedHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Neutral Hadron Isolation
         /*******************************************************************************************/
         cout << "\tDoing Neutral Hadron Isolation ... " << std::flush;
         cname.push_back(Form("cneutralHadronIso_%s",ifilePostFixes[f].Data()));
         cneutralHadronIso.push_back(new TCanvas(cname.back(),Form("neutralHadronIsoVsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hneutralHadronIso_%s",ifilePostFixes[f].Data()));
         hneutralHadronIso.push_back(new TH2D(hname.back(),Form("Neutral Hadron Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,30,0,15));
         hneutralHadronIso.back()->GetXaxis()->SetTitle("#eta");
         hneutralHadronIso.back()->GetYaxis()->SetTitle("Neutral Hadron Isolation");
         pname.push_back(Form("pneutralHadronIso_%s",ifilePostFixes[f].Data()));
         pneutralHadronIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         t->Draw("lLV[0].lneutralHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
         t->Draw("lLV[0].lneutralHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Photon Isolation
         /*******************************************************************************************/
         cout << "\tDoing Photon Isolation ... " << std::flush;
         cname.push_back(Form("cphotonIso_%s",ifilePostFixes[f].Data()));
         cphotonIso.push_back(new TCanvas(cname.back(),Form("photonIsoVsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hphotonIso_%s",ifilePostFixes[f].Data()));
         hphotonIso.push_back(new TH2D(hname.back(),Form("Photon Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,120,0,60));
         hphotonIso.back()->GetXaxis()->SetTitle("#eta");
         hphotonIso.back()->GetYaxis()->SetTitle("Photon Isolation");
         pname.push_back(Form("pphotonIso_%s",ifilePostFixes[f].Data()));
         pphotonIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         t->Draw("lLV[0].lphotonIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
         t->Draw("lLV[0].lphotonIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //MVATrigV0
         /*******************************************************************************************/
         cout << "\tDoing MVATrigV0 ... " << std::flush;
         cname.push_back(Form("cmvaTrigV0_%s",ifilePostFixes[f].Data()));
         cmvaTrigV0.push_back(new TCanvas(cname.back(),Form("mvaTrigV0VsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hmvaTrigV0_%s",ifilePostFixes[f].Data()));
         hmvaTrigV0.push_back(new TH2D(hname.back(),Form("MVA Trig V0 Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,nbinsx,&binsMVA.at(0)));
         hmvaTrigV0.back()->GetXaxis()->SetTitle("#eta");
         hmvaTrigV0.back()->GetYaxis()->SetTitle("MVA Trig V0");
         pname.push_back(Form("pmvaTrigV0_%s",ifilePostFixes[f].Data()));
         pmvaTrigV0.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         t->Draw("lLV[0].emvaTrig:lLV[0].Eta()>>"+hname.back(),"","colz");
         t->Draw("lLV[0].emvaTrig:lLV[0].Eta()>>"+pname.back(),"","sames");
         gPad->SetLogy();
         if(ifilePostFixes[f].CompareTo("DATA")==0 || ifilePostFixes[f].CompareTo("Data")==0 || ifilePostFixes[f].CompareTo("data")==0) {
            hmvaTrigV0.back()->GetYaxis()->SetRangeUser(9.0e-1,1);
            pmvaTrigV0.back()->GetYaxis()->SetRangeUser(9.0e-1,1);
         }
         else {
            hmvaTrigV0.back()->GetYaxis()->SetRangeUser(1.0e-2,1);
            pmvaTrigV0.back()->GetYaxis()->SetRangeUser(1.0e-2,1);
         }
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //MVANonTrigV0
         /*******************************************************************************************/
         cout << "\tDoing MVANonTrigV0 Isolation ... " << std::flush;
         cname.push_back(Form("cmvaNonTrigV0_%s",ifilePostFixes[f].Data()));
         cmvaNonTrigV0.push_back(new TCanvas(cname.back(),Form("mvaNonTrigV0VsEta_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hmvaNonTrigV0_%s",ifilePostFixes[f].Data()));
         hmvaNonTrigV0.push_back(new TH2D(hname.back(),Form("MVA Non-Trig V0 Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,nbinsx,&binsMVA.at(0)));
         hmvaNonTrigV0.back()->GetXaxis()->SetTitle("#eta");
         hmvaNonTrigV0.back()->GetYaxis()->SetTitle("MVA Non-Trig V0");
         pname.push_back(Form("pmvaNonTrigV0_%s",ifilePostFixes[f].Data()));
         pmvaNonTrigV0.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
         t->Draw("lLV[0].emvaNonTrig:lLV[0].Eta()>>"+hname.back(),"","colz");
         t->Draw("lLV[0].emvaNonTrig:lLV[0].Eta()>>"+pname.back(),"","sames");
         gPad->SetLogy();
         if(ifilePostFixes[f].CompareTo("DATA")==0 || ifilePostFixes[f].CompareTo("Data")==0 || ifilePostFixes[f].CompareTo("data")==0) {
            hmvaNonTrigV0.back()->GetYaxis()->SetRangeUser(8.0e-1,1);
            pmvaNonTrigV0.back()->GetYaxis()->SetRangeUser(8.0e-1,1);
         }
         else {
            hmvaNonTrigV0.back()->GetYaxis()->SetRangeUser(1.0e-2,1);
            pmvaNonTrigV0.back()->GetYaxis()->SetRangeUser(1.0e-2,1);
         }
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //PF Isolation Vs. MVA
         /*******************************************************************************************/
         cout << "\tDoing MVANonTrigV0 Isolation ... " << std::flush;
         cname.push_back(Form("cpfIsoMVATrigV0_%s",ifilePostFixes[f].Data()));
         cpfIsoMVATrigV0.push_back(new TCanvas(cname.back(),Form("pfIsoVsMVATrigV0_%s",ifilePostFixes[f].Data()),800,800));
         hname.push_back(Form("hpfIsoMVATrigV0_%s",ifilePostFixes[f].Data()));
         hpfIsoMVATrigV0.push_back(new TH2D(hname.back(),Form("PF Isolation Vs MVA Trig V0 (%s)",ifilePostFixes[f].Data()),nbinsx,&binsMVA.at(0),400,0,2));
         hpfIsoMVATrigV0.back()->GetXaxis()->SetTitle("MVA Trig V0");
         hpfIsoMVATrigV0.back()->GetYaxis()->SetTitle("PF Isolation");
         pname.push_back(Form("ppfIsoMVATrigV0_%s",ifilePostFixes[f].Data()));
         ppfIsoMVATrigV0.push_back(new TProfile(pname.back(),pname.back(),nbinsx,&binsMVA.at(0)));
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso:lLV[0].emvaTrig>>"+hname.back(),"lLV[0].lpfIso>0.2","colz");
            t->Draw("lLV[0].lpfIso:lLV[0].emvaTrig>>"+pname.back(),"lLV[0].lpfIso>0.2","sames");
            hpfIsoMVATrigV0.back()->GetYaxis()->SetRangeUser(0.3,2);
         }
         else {
            t->Draw("lLV[0].lpfIso:lLV[0].emvaTrig>>"+hname.back(),"","colz");
            t->Draw("lLV[0].lpfIso:lLV[0].emvaTrig>>"+pname.back(),"","sames");
         }
         gPad->SetLogx();
         if(ifilePostFixes[f].CompareTo("DATA")==0 || ifilePostFixes[f].CompareTo("Data")==0 || ifilePostFixes[f].CompareTo("data")==0) {
            hpfIsoMVATrigV0.back()->GetXaxis()->SetRangeUser(9.0e-1,1);
            ppfIsoMVATrigV0.back()->GetXaxis()->SetRangeUser(9.0e-1,1);
         }
         else {
            hpfIsoMVATrigV0.back()->GetXaxis()->SetRangeUser(1.0e-1,1);
            ppfIsoMVATrigV0.back()->GetXaxis()->SetRangeUser(1.0e-1,1);
         }
         cout << "DONE" << endl;
      }

      if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0) {

         /*******************************************************************************************/
         //pT Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) 
         /*******************************************************************************************/
         cout << "\tDoing pT distribution comparison ..." << std::flush;
         cname.push_back(Form("cptDistributions_%s",ifilePostFixes[f].Data()));
         cptDistributions.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         legs.push_back(new TLegend(0.6,0.4,0.9,0.6));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         hname.push_back(Form("hptDistributions_antiMVA_signalIso_uncor_%s",ifilePostFixes[f].Data()));
         hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributions.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributions.back()->GetXaxis()->SetRangeUser(0,200);
         hptDistributions.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributions.back()->SetMarkerColor(kBlack);
         hptDistributions.back()->SetLineColor(kBlack);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso<0.2&&lLV[0].Pt()>30","goff");
         }
         hptDistributions.back()->Scale(1.0/hptDistributions.back()->Integral());
         hptDistributions.back()->Draw();
         legs.back()->AddEntry(hptDistributions.back(),"Uncorrected && 0.05<MVA EID<0.95 && pfIso<0.2 && p_{T}^{Lepton}>30 GeV","l");
         hname.push_back(Form("hptDistributions_antiMVA_qcdIso_uncor_%s",ifilePostFixes[f].Data()));
         hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributions.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributions.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributions.back()->SetMarkerColor(kBlue);
         hptDistributions.back()->SetLineColor(kBlue);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].Pt()>30","goff");
         }
         hptDistributions.back()->Scale(1.0/hptDistributions.back()->Integral());
         hptDistributions.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributions.back(),"Uncorrected && 0.05<MVA EID<0.95 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         hname.push_back(Form("hptDistributions_antiMVA_qcdIso_cor_%s",ifilePostFixes[f].Data()));
         hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributions.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributions.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributions.back()->SetMarkerColor(kRed);
         hptDistributions.back()->SetLineColor(kRed);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
         //t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2))>>"+hanme.back().Data(),
         //        "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&(lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2)))>30");
            t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2))>>"+hname.back(),
                    "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&(lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2)))>30","goff");
         }
         hptDistributions.back()->Scale(1.0/hptDistributions.back()->Integral());
         hptDistributions.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributions.back(),"Corrected && 0.05<MVA EID<0.95 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->Draw("sames");
         cout << "DONE" << endl;

         /*
         Terminal Version

         PS->cd()
         gStyle->SetPalette(1)
         TCanvas * c = new TCanvas("c","c",800,800)
         TH1D* asIs = new TH1D("asIs","asIs",1000,0,1000)
         asIs->SetMarkerColor(kBlack)
         asIs->SetLineColor(kBlack)
         jets2p->Draw("lLV[0].Pt()>>asIs","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso<0.2&&lLV[0].Pt()>30")
         TH1D* asIs2 = new TH1D("asIs2","asIs2",1000,0,1000)
         asIs2->SetMarkerColor(kBlue)
         asIs2->SetLineColor(kBlue)
         jets2p->Draw("lLV[0].Pt()>>asIs2","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].Pt()>30")
         TH1D* cor = new TH1D("cor","cor",1000,0,1000)
         cor->SetMarkerColor(kRed)
         cor->SetLineColor(kRed)
         jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2))>>cor","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&(lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2)))>30")
         //jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2))>>cor","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&(lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2)))>30")
         asIs->Scale(1.0/asIs->Integral())
         asIs2->Scale(1.0/asIs2->Integral())
         cor->Scale(1.0/cor->Integral())
         asIs->Draw()
         asIs2->Draw("sames")
         cor->Draw("sames")

         */

         /*******************************************************************************************/
         //Show Shifted pT Peaks 
         /*******************************************************************************************/
         cout << "\tDoing shifted pT peaks ... " << std::flush;
         cname.push_back(Form("cShiftedPt_%s",ifilePostFixes[f].Data()));
         cptDistributionsShifted.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         legs.push_back(new TLegend(0.6,0.4,0.9,0.6));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         hname.push_back(Form("hShiftedPt_antiMVA_0p0to0p2_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsShifted.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsShifted.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsShifted.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsShifted.back()->SetMarkerColor(kBlack);
         hptDistributionsShifted.back()->SetLineColor(kBlack);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.2","goff");
         }
         hptDistributionsShifted.back()->Scale(1.0/hptDistributionsShifted.back()->Integral());
         hptDistributionsShifted.back()->Draw("");
         legs.back()->AddEntry(hptDistributionsShifted.back(),"0.05<MVA EID<0.95 && 0.0<pfIso<0.2","l");
         hname.push_back(Form("hShiftedPt_antiMVA_0p2to0p4_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsShifted.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsShifted.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsShifted.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsShifted.back()->SetMarkerColor(kRed);
         hptDistributionsShifted.back()->SetLineColor(kRed);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.4","goff");
         }
         hptDistributionsShifted.back()->Scale(1.0/hptDistributionsShifted.back()->Integral());
         hptDistributionsShifted.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsShifted.back(),"0.05<MVA EID<0.95 && 0.2<pfIso<0.4","l");
         hname.push_back(Form("hShiftedPt_antiMVA_0p4to0p6_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsShifted.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsShifted.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsShifted.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsShifted.back()->SetMarkerColor(kBlue);
         hptDistributionsShifted.back()->SetLineColor(kBlue);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.6","goff");
         }
         hptDistributionsShifted.back()->Scale(1.0/hptDistributionsShifted.back()->Integral());
         hptDistributionsShifted.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsShifted.back(),"0.05<MVA EID<0.95 && 0.4<pfIso<0.6","l");
         hname.push_back(Form("hShiftedPt_antiMVA_0p6to0p8_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsShifted.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsShifted.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsShifted.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsShifted.back()->SetMarkerColor(kGreen);
         hptDistributionsShifted.back()->SetLineColor(kGreen);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.8","colz sames");
         }
         hptDistributionsShifted.back()->Scale(1.0/hptDistributionsShifted.back()->Integral());
         hptDistributionsShifted.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsShifted.back(),"0.05<MVA EID<0.95 && 0.6<pfIso<0.8","l");
         legs.back()->Draw("sames");
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Show Corrected pT Peaks 
         /*******************************************************************************************/
         cout << "\tDoing corrected pT peaks ... " << std::flush;
         cname.push_back(Form("cCorrectedPt_%s",ifilePostFixes[f].Data()));
         cptDistributionsCorrected.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         legs.push_back(new TLegend(0.6,0.4,0.9,0.6));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         hname.push_back(Form("hCorrectedPt_antiMVA_0p0to0p2_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsCorrected.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsCorrected.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsCorrected.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsCorrected.back()->SetMarkerColor(kBlack);
         hptDistributionsCorrected.back()->SetLineColor(kBlack);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.0871531,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>"+hname.back(),
                    "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.2","goff");
         }
         hptDistributionsCorrected.back()->Scale(1.0/hptDistributionsCorrected.back()->Integral());
         hptDistributionsCorrected.back()->Draw("");
         legs.back()->AddEntry(hptDistributionsCorrected.back(),"0.05<MVA EID<0.95 && 0.0<pfIso<0.2","l");
         hname.push_back(Form("hCorrectedPt_antiMVA_0p2to0p4_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsCorrected.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsCorrected.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsCorrected.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsCorrected.back()->SetMarkerColor(kRed);
         hptDistributionsCorrected.back()->SetLineColor(kRed);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.259677,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>"+hname.back(),
                    "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.4","goff");
         }
         hptDistributionsCorrected.back()->Scale(1.0/hptDistributionsCorrected.back()->Integral());
         hptDistributionsCorrected.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsCorrected.back(),"0.05<MVA EID<0.95 && 0.2<pfIso<0.4","l");
         hname.push_back(Form("hCorrectedPt_antiMVA_0p4to0p6_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsCorrected.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsCorrected.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsCorrected.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsCorrected.back()->SetMarkerColor(kBlue);
         hptDistributionsCorrected.back()->SetLineColor(kBlue);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.452971,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>"+hname.back(),
                    "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.6","goff");
         }
         hptDistributionsCorrected.back()->Scale(1.0/hptDistributionsCorrected.back()->Integral());
         hptDistributionsCorrected.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsCorrected.back(),"0.05<MVA EID<0.95 && 0.4<pfIso<0.6","l");
         hname.push_back(Form("hCorrectedPt_antiMVA_0p6to0p8_cor_%s",ifilePostFixes[f].Data()));
         hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
         hptDistributionsCorrected.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hptDistributionsCorrected.back()->GetXaxis()->SetRangeUser(0,150);
         hptDistributionsCorrected.back()->GetYaxis()->SetTitle("Events / 1 GeV");
         hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.06);
         hptDistributionsCorrected.back()->SetMarkerColor(kGreen);
         hptDistributionsCorrected.back()->SetLineColor(kGreen);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>"+hname.back(),
                    "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.8","goff");
         }
         hptDistributionsCorrected.back()->Scale(1.0/hptDistributionsCorrected.back()->Integral());
         hptDistributionsCorrected.back()->Draw("sames");
         legs.back()->AddEntry(hptDistributionsCorrected.back(),"0.05<MVA EID<0.95 && 0.6<pfIso<0.8","l");
         legs.back()->Draw("sames");
         cout << "DONE" << endl;

         /*
         Terminal Version

         PS->cd()
         gStyle->SetPalette(1)
         TCanvas * c = new TCanvas("After","After",800,800)
         TH1D* h01 = new TH1D("h01","h01",100,0,1000)
         h01->SetMarkerColor(kBlack)
         h01->SetLineColor(kBlack)
         jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.0871531,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>h01","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.2","colz")
         //jets2p->Draw("lLV[0].Pt()>>h01","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.2","colz")
         TH1D* h23 = new TH1D("h23","h23",100,0,1000)
         h23->SetMarkerColor(kRed)
         h23->SetLineColor(kRed)
         jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.259677,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>h23","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.4","colz sames")
         //jets2p->Draw("lLV[0].Pt()>>h23","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.4","colz sames")
         TH1D* h45 = new TH1D("h45","h45",100,0,1000)
         h45->SetMarkerColor(kBlue)
         h45->SetLineColor(kBlue)
         jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.452971,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>h45","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.6","colz sames")
         //jets2p->Draw("lLV[0].Pt()>>h45","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.6","colz sames")
         TH1D* h67 = new TH1D("h67","h67",100,0,1000)
         h67->SetMarkerColor(kGreen)
         h67->SetLineColor(kGreen)
         jets2p->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2))>>h67","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.8","colz sames")
         //jets2p->Draw("lLV[0].Pt()>>h67","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.8","colz sames")
         h01->Scale(1.0/h01->Integral())
         h23->Scale(1.0/h23->Integral())
         h45->Scale(1.0/h45->Integral())
         h67->Scale(1.0/h67->Integral())
         h01->Draw("")
         h23->Draw("sames")
         h45->Draw("sames")
         h67->Draw("sames")

         */

         /*******************************************************************************************/
         //Show Isolation Energy Vs. pT
         /*******************************************************************************************/
         //Setting a smoother palette
         const Int_t NRGBs = 5;
         const Int_t NCont = 104;
         Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
         Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
         Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
         Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
         TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
         gStyle->SetNumberContours(NCont);

         cout << "\tDoing isolation energy vs. pT ... " << std::flush;
         cname.push_back(Form("cIsoEVsPt_%s",ifilePostFixes[f].Data()));
         cIsoEVsPt.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         THStack* sIsoEVsPt = new THStack(Form("sIsoEVsPt_%s",ifilePostFixes[f].Data()),Form("sIsoEVsPt_%s",ifilePostFixes[f].Data()));
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p0to0p1_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kBlack);
         hIsoEVsPt.back()->SetLineColor(kBlack);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.1","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p1to0p2_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kRed);
         hIsoEVsPt.back()->SetLineColor(kRed);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.1&&lLV[0].lpfIso<0.2","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p2to0p3_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kBlue);
         hIsoEVsPt.back()->SetLineColor(kBlue);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.3","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p3to0p4_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kGreen);
         hIsoEVsPt.back()->SetLineColor(kGreen);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].lpfIso<0.4","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p4to0p5_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kMagenta);
         hIsoEVsPt.back()->SetLineColor(kMagenta);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.5","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p5to0p6_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kOrange);
         hIsoEVsPt.back()->SetLineColor(kOrange);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.5&&lLV[0].lpfIso<0.6","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p6to0p7_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kCyan);
         hIsoEVsPt.back()->SetLineColor(kCyan);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.7","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p7to0p8_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kRed+3);
         hIsoEVsPt.back()->SetLineColor(kRed+3);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.7&&lLV[0].lpfIso<0.8","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p8to0p9_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kBlue+3);
         hIsoEVsPt.back()->SetLineColor(kBlue+3);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.8&&lLV[0].lpfIso<0.9","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         hname.push_back(Form("hIsoEVsPt_antiMVA_0p9to1p0_uncor_%s",ifilePostFixes[f].Data()));
         hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
         hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
         hIsoEVsPt.back()->SetMarkerColor(kGreen+3);
         hIsoEVsPt.back()->SetLineColor(kGreen+3);
         if(ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0) {
            t->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>"+hname.back(),"lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.9&&lLV[0].lpfIso<1.0","goff");
         }
         hIsoEVsPt.back()->Fit("pol1");
         sIsoEVsPt->Add(hIsoEVsPt.back());
         /*
         for (unsigned int i=0; i<hIsoEVsPt.size(); i++) {
            if(i==0)
               hIsoEVsPt[i]->Draw("colz");
            else
               hIsoEVsPt[i]->Draw("colz sames");
         }
         */
         sIsoEVsPt->Draw("colz nostack");
         sIsoEVsPt->GetXaxis()->SetTitle("p_{T} [GeV]");
         sIsoEVsPt->GetYaxis()->SetTitle("Isolation Energy");
         cout << "DONE" << endl;


         /*
         Terminal Version

         PS->cd()
         gStyle->SetPalette(1)
         TCanvas * c = new TCanvas("c","c",800,800)
         TH2D* h01 = new TH2D("h01","h01",500,0,500,500,0,500)
         h01->SetMarkerColor(kBlack)
         h01->SetLineColor(kBlack)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h01","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.1","colz")
         TH2D* h12 = new TH2D("h12","h12",500,0,500,500,0,500)
         h12->SetMarkerColor(kRed)
         h12->SetLineColor(kRed)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h12","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.1&&lLV[0].lpfIso<0.2","colz sames")
         TH2D* h23 = new TH2D("h23","h23",500,0,500,500,0,500)
         h23->SetMarkerColor(kBlue)
         h23->SetLineColor(kBlue)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h23","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.3","colz sames")
         TH2D* h34 = new TH2D("h34","h34",500,0,500,500,0,500)
         h34->SetMarkerColor(kGreen)
         h34->SetLineColor(kGreen)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h34","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].lpfIso<0.4","colz sames")
         TH2D* h45 = new TH2D("h45","h45",500,0,500,500,0,500)
         h45->SetMarkerColor(kMagenta)
         h45->SetLineColor(kMagenta)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h45","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.5","colz sames")
         TH2D* h56 = new TH2D("h56","h56",500,0,500,500,0,500)
         h56->SetMarkerColor(kOrange)
         h56->SetLineColor(kOrange)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h56","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.5&&lLV[0].lpfIso<0.6","colz sames")
         TH2D* h67 = new TH2D("h67","h67",500,0,500,500,0,500)
         h67->SetMarkerColor(kCyan)
         h67->SetLineColor(kCyan)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h67","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.7","colz sames")
         h01->Fit("pol1")
         h12->Fit("pol1")
         h23->Fit("pol1")
         h34->Fit("pol1")
         h45->Fit("pol1")
         h56->Fit("pol1")
         h67->Fit("pol1")
         h01->Draw("colz")
         h12->Draw("colz sames")
         h23->Draw("colz sames")
         h34->Draw("colz sames")
         h45->Draw("colz sames")
         h56->Draw("colz sames")
         h67->Draw("colz sames")

         */

      }

      /*******************************************************************************************/
      //SaveAs & Write
      /*******************************************************************************************/

      cout << "\tSaving files ... " << std::flush;
      for(unsigned int ff=0; ff<formats.size(); ff++) {
         if(plots.CompareTo("ALL")==0 || plots.CompareTo("COMPONENTS")==0) {
            cpfIso.back()->SaveAs(ofilepath+cpfIso.back()->GetTitle()+formats[ff]);
            cchargedHadronIso.back()->SaveAs(ofilepath+cchargedHadronIso.back()->GetTitle()+formats[ff]);
            cneutralHadronIso.back()->SaveAs(ofilepath+cneutralHadronIso.back()->GetTitle()+formats[ff]);
            cphotonIso.back()->SaveAs(ofilepath+cphotonIso.back()->GetTitle()+formats[ff]);
            cmvaTrigV0.back()->SaveAs(ofilepath+cmvaTrigV0.back()->GetTitle()+formats[ff]);
            cmvaNonTrigV0.back()->SaveAs(ofilepath+cmvaNonTrigV0.back()->GetTitle()+formats[ff]);
            cpfIsoMVATrigV0.back()->SaveAs(ofilepath+cpfIsoMVATrigV0.back()->GetTitle()+formats[ff]);
         }
         if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0) {
            cptDistributions.back()->SaveAs(ofilepath+cptDistributions.back()->GetTitle()+formats[ff]);
            cptDistributionsShifted.back()->SaveAs(ofilepath+cptDistributionsShifted.back()->GetTitle()+formats[ff]);
            cptDistributionsCorrected.back()->SaveAs(ofilepath+cptDistributionsCorrected.back()->GetTitle()+formats[ff]);
            cIsoEVsPt.back()->SaveAs(ofilepath+cIsoEVsPt.back()->GetTitle()+formats[ff]);
         }
      }
      cout << "DONE" << endl;

      cout << "\tWriting canvases to " << ofilepath+ofilename << " ... ";
      ofile->cd();
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("COMPONENTS")==0) {
         cpfIso.back()->Write();
         cchargedHadronIso.back()->Write();
         cneutralHadronIso.back()->Write();
         cphotonIso.back()->Write();
         cmvaTrigV0.back()->Write();
         cmvaNonTrigV0.back()->Write();
         cpfIsoMVATrigV0.back()->Write();
      }
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0) {
         cptDistributions.back()->Write();
         cptDistributionsShifted.back()->Write();
         cptDistributionsCorrected.back()->Write();
         cIsoEVsPt.back()->Write();
      }
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //Clear vectors
      /*******************************************************************************************/
      cout << "\tClearing vectors ... " << std::flush;
      cpfIso.clear();
      cchargedHadronIso.clear();
      cneutralHadronIso.clear();
      cphotonIso.clear();
      cmvaTrigV0.clear();
      cmvaNonTrigV0.clear();
      cpfIsoMVATrigV0.clear();
      cptDistributions.clear();
      cptDistributionsShifted.clear();
      cptDistributionsCorrected.clear();
      cIsoEVsPt.clear();
      hpfIso.clear();
      hchargedHadronIso.clear();
      hneutralHadronIso.clear();
      hphotonIso.clear();
      hmvaTrigV0.clear();
      hmvaNonTrigV0.clear();
      hpfIsoMVATrigV0.clear();
      hptDistributions.clear();
      hptDistributionsShifted.clear();
      hptDistributionsCorrected.clear();
      hIsoEVsPt.clear();
      ppfIso.clear();
      pchargedHadronIso.clear();
      pneutralHadronIso.clear();
      pphotonIso.clear();
      pmvaTrigV0.clear();
      pmvaNonTrigV0.clear();
      ppfIsoMVATrigV0.clear();
      cname.clear();
      hname.clear();
      pname.clear();
      legs.clear();
      cout << "DONE" << endl;
   }

   m_benchmark->Stop("event");
   cout << "IsolationPlotter_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;
}



/*
Distributions that could be added

PS->cd()
TH1D* lessThan = new TH1D("lessThan","lessThan",500,0,500)
TH1D* greaterThan = new TH1D("greaterThan","greaterThan",500,0,500)
lessThan->SetLineColor(kBlack)
greaterThan->SetLineColor(kRed)
jets2p->Draw("lLV[0].Pt()*lLV[0].lpfIso>>lessThan","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso<0.2&&lLV[0].Pt()>30")
jets2p->Draw("lLV[0].Pt()*lLV[0].lpfIso>>greaterThan","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].Pt()>30")
lessThan->Draw("same")
TLegend* leg = new TLegend(0.6,0.4,0.9,0.6)
leg->SetBorderSize(0)
leg->SetLineStyle(0)
leg->SetFillColor(0)
leg->SetFillStyle(0)
lessThan->Scale(1.0/lessThan->Integral())
greaterThan->Scale(1.0/greaterThan->Integral())
leg->AddEntry(lessThan,"emvaTrig<0.95 && pfIso<0.2","l")
leg->AddEntry(greaterThan,"emvaTrig<0.95 && pfIso>0.3","l")
lessThan->Draw()
greaterThan->Draw("same")
leg->Draw("same")
TH1D* div = (TH1D*)lessThan->Clone("div")
div->Divide(greaterThan)
div->SetLineColor(kBlue)
TCanvas c
c.cd()
div->Draw()



PS->cd()
TH2D* lessThan = new TH2D("lessThan","lessThan",500,0,500,500,0,500)
TH2D* greaterThan = new TH2D("greaterThan","greaterThan",500,0,500,500,0,500)
jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>lessThan","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso<0.2","colz")
jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>greaterThan","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3","colz")
lessThan->GetXaxis()->SetTitle("p_{T}^{Lepton}")
greaterThan->GetXaxis()->SetTitle("p_{T}^{Lepton}")
lessThan->GetYaxis()->SetTitle("p_{T}^{Lepton}*Iso_{PF}")
greaterThan->GetYaxis()->SetTitle("p_{T}^{Lepton}*Iso_{PF}")
lessThan->ProfileX()
greaterThan->ProfileX()
greaterThan_pfx->SetLineColor(kRed)
lessThan_pfx->Fit("pol1")
greaterThan_pfx->Fit("pol1")
lessThan_pfx->Draw()
greaterThan_pfx->Draw("sames")

*/
