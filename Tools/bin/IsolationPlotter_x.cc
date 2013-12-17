// User includes
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"

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
#include "TLine.h"
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
//  local functions
////////////////////////////////////////////////////////////////////////////////

/// get the index of the histogram corresponding to x
int get_index(double x,const vector<double>& binsx);

/// draw a vertical line using NDC coordinates given an x value in non-NDC coordinated
void DrawVerticalLine(Double_t x, Color_t color, Int_t width);

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   vector<TString> ifiles          = cl.getVector<TString> ("ifiles");
   vector<TString> ifilePaths      = cl.getVector<TString> ("ifilePaths",            "./");
   vector<TString> ifilePostFixes  = cl.getVector<TString> ("ifilePostFixes",          "");
   TString         ifolder         = cl.getValue<TString>  ("ifolder",               "PS");  
   TString         itree           = cl.getValue<TString>  ("itree",             "jets2p");  
   TString         ofilepath       = cl.getValue<TString>  ("ofilepath"             ,"./");
   TString         ofilename       = cl.getValue<TString>  ("ofilename", "IP_output.root");
   vector<TString> formats         = cl.getVector<TString> ("formats",      ".eps:::.png");
   TString         plots           = cl.getValue<TString>  ("plots",                "all");
   Int_t           maxEvts         = cl.getValue<Int_t>    ("maxEvts",                  0);
   double          isoBinWidth     = cl.getValue<double>   ("isoBinWidth",            0.1);
   double          isoMax          = cl.getValue<double>   ("isoMax",                10.0);
   TString         stackDrawOption = cl.getValue<TString>  ("stackDrawOption",  "nostack");
   bool            addData         = cl.getValue<bool>     ("addData",               true);
   bool            coshEta         = cl.getValue<bool>     ("coshEta",              false);

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
   vector<TCanvas*> cIsoEVsPtCoshEta;
   vector<TCanvas*> cRicardoMethod;
   vector<TCanvas*> cWeights;
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
   vector<TH2D*> hIsoEVsPtCoshEta;
   vector<TH1D*> hWeights;
   vector<TH2D*> hRicardoMethod;
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
   vector<TF1*> fits;
   vector<TF1*> fits2;
   map<double,pair<double,double> > fitParameters;
   map<double,pair<double,double> > fitParameters2;
   vector<double> binsMVA;

   /*******************************************************************************************/
   //Open data file for overlay
   /*******************************************************************************************/
   TFile* dataFile;
   TTree* dataTree;
   TH1D* dataHist;
   if(addData) {
      dataFile = new TFile("/uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/SingleEl_Data_19p148fb.root","READ");
      if(!dataFile->IsOpen()) {
         cout << "\tERROR::Could not open file /uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/SingleEl_Data_19p148fb.root" << endl;
         return 0;
      }
      if (!dataFile->cd("PS")) {
         cout << "\tERROR::Could not CD into directory PS in file /uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/SingleEl_Data_19p148fb.root" << endl;
         return 0;
      }
      dataTree = (TTree*)gDirectory->Get("jets2p");
      if (dataTree == 0) {
         cout << "\tERROR::Could not find tree named jets2p in file /uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/SingleEl_Data_19p148fb.root" << endl;
         return 0;
      }
   }

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

      if((plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION_FAST")==0 || plots.CompareTo("CORRECTION")==0) && (ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0)) {

         double constPt[10] = {0.05736893,0.14465788,0.24249983,0.34215147,0.44189681,0.54234854,0.64379960,0.74293358,0.84424901,0.94423923};
         double constPtCoshEta[10] = {0.00980507,0.02329070,0.04601302,0.08399500,0.13130544,0.18035410,0.22153813,0.26718962,0.31143341,0.34447439};
         vector<double> constVec;
         for(unsigned int cv=0; cv<10; cv++) {
            if(coshEta)
               constVec.push_back(constPtCoshEta[cv]);
            else
               constVec.push_back(constPt[cv]);
         }
         vector<double> isos;
         EventNtuple * ntuple = new EventNtuple();
         t->SetBranchAddress("EvtNtuple", &ntuple);
         Int_t nentries = t->GetEntries();
         t->SetBranchStatus("*",0);
         t->SetBranchStatus("lLV*");
         t->SetBranchStatus("METLV*");
         t->SetBranchStatus("jLV*");
         for(Int_t ientry = 0; ientry<nentries; ientry++) {
            if(maxEvts!=0 && ientry>maxEvts)
               continue;
            if(ientry==0) {
               hname.push_back(Form("hptDistributions_antiMVA_signalIso_uncor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
               hptDistributions.back()->GetXaxis()->SetRangeUser(0,150);
               hptDistributions.back()->GetYaxis()->SetTitle("Events / 1 GeV");
               hptDistributions.back()->GetYaxis()->SetRangeUser(0,0.07);
               hptDistributions.back()->SetMarkerColor(kBlack);
               hptDistributions.back()->SetLineColor(kBlack);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_MVA_signalIso_uncor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kBlack);
               hptDistributions.back()->SetLineColor(kBlack);
               hptDistributions.back()->SetMarkerStyle(20);
               hptDistributions.back()->SetMarkerSize(0.7);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_antiMVA_qcdIso_uncor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kBlue);
               hptDistributions.back()->SetLineColor(kBlue);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_antiMVA_qcdIso_cor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kRed);
               hptDistributions.back()->SetLineColor(kRed);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_antiMVA_qcdIsoLimited_cor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kYellow);
               hptDistributions.back()->SetLineColor(kYellow);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_qcdIso_cor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kOrange);
               hptDistributions.back()->SetLineColor(kOrange);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_qcdIso_uncor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kGreen);
               hptDistributions.back()->SetLineColor(kGreen);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_MVA_qcdIso_uncor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kMagenta);
               hptDistributions.back()->SetLineColor(kMagenta);
               hptDistributions.back()->Sumw2();
               hname.push_back(Form("hptDistributions_MVA_qcdIso_cor_%s",ifilePostFixes[f].Data()));
               hptDistributions.push_back(new TH1D(hname.back(),Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributions.back()->SetMarkerColor(kCyan);
               hptDistributions.back()->SetLineColor(kCyan);
               hptDistributions.back()->Sumw2();

               hname.push_back(Form("hShiftedPt_antiMVA_0p0to0p1_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
               hptDistributionsShifted.back()->GetXaxis()->SetRangeUser(0,150);
               hptDistributionsShifted.back()->GetYaxis()->SetTitle("Events / 1 GeV");
               hptDistributionsShifted.back()->GetYaxis()->SetRangeUser(0,0.07);
               hptDistributionsShifted.back()->SetMarkerColor(kBlack);
               hptDistributionsShifted.back()->SetLineColor(kBlack);
               hname.push_back(Form("hShiftedPt_antiMVA_0p1to0p2_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kRed);
               hptDistributionsShifted.back()->SetLineColor(kRed);
               hname.push_back(Form("hShiftedPt_antiMVA_0p2to0p3_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kBlue);
               hptDistributionsShifted.back()->SetLineColor(kBlue);
               hname.push_back(Form("hShiftedPt_antiMVA_0p3to0p4_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kGreen);
               hptDistributionsShifted.back()->SetLineColor(kGreen);
               hname.push_back(Form("hShiftedPt_antiMVA_0p4to0p5_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kMagenta);
               hptDistributionsShifted.back()->SetLineColor(kMagenta);
               hname.push_back(Form("hShiftedPt_antiMVA_0p5to0p6_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kCyan);
               hptDistributionsShifted.back()->SetLineColor(kCyan);
               hname.push_back(Form("hShiftedPt_antiMVA_0p6to0p7_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(kOrange);
               hptDistributionsShifted.back()->SetLineColor(kOrange);
               hname.push_back(Form("hShiftedPt_antiMVA_0p7to0p8_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsShifted.push_back(new TH1D(hname.back(),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsShifted.back()->SetMarkerColor(40);
               hptDistributionsShifted.back()->SetLineColor(40);

               hname.push_back(Form("hCorrectedPt_antiMVA_0p0to0p1_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
               hptDistributionsCorrected.back()->GetXaxis()->SetRangeUser(0,150);
               hptDistributionsCorrected.back()->GetYaxis()->SetTitle("Events / 1 GeV");
               hptDistributionsCorrected.back()->GetYaxis()->SetRangeUser(0,0.07);
               hptDistributionsCorrected.back()->SetMarkerColor(kBlack);
               hptDistributionsCorrected.back()->SetLineColor(kBlack);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p1to0p2_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kRed);
               hptDistributionsCorrected.back()->SetLineColor(kRed);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p2to0p3_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kBlue);
               hptDistributionsCorrected.back()->SetLineColor(kBlue);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p3to0p4_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kGreen);
               hptDistributionsCorrected.back()->SetLineColor(kGreen);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p4to0p5_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kMagenta);
               hptDistributionsCorrected.back()->SetLineColor(kMagenta);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p5to0p6_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kCyan);
               hptDistributionsCorrected.back()->SetLineColor(kCyan);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p6to0p7_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(kOrange);
               hptDistributionsCorrected.back()->SetLineColor(kOrange);
               hname.push_back(Form("hCorrectedPt_antiMVA_0p7to0p8_cor_%s",ifilePostFixes[f].Data()));
               hptDistributionsCorrected.push_back(new TH1D(hname.back(),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000));
               hptDistributionsCorrected.back()->SetMarkerColor(40);
               hptDistributionsCorrected.back()->SetLineColor(40);

               for(double iso=0.0; iso<isoMax; iso+=isoBinWidth) {
                  isos.push_back(iso);
                  if(iso>=isoMax-isoBinWidth)
                     isos.push_back(iso+isoBinWidth);
                  hname.push_back(Form("hIsoEVsPt_antiMVA_%.1fto%.1f_uncor_%s",iso,iso+isoBinWidth,ifilePostFixes[f].Data()));
                  hIsoEVsPt.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
                  hIsoEVsPt.back()->GetXaxis()->SetTitle("p_{T} [GeV/c]");
                  hIsoEVsPt.back()->GetYaxis()->SetTitle("Isolation Energy");
                  hIsoEVsPt.back()->SetMarkerColor((iso*10.0)+1);
                  hIsoEVsPt.back()->SetLineColor((iso*10.0)+1);

                  hname.push_back(Form("hIsoEVsPtCoshEta_antiMVA_%.1fto%.1f_uncor_%s",iso,iso+isoBinWidth,ifilePostFixes[f].Data()));
                  hIsoEVsPtCoshEta.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} x cosh(#eta) (%s)",ifilePostFixes[f].Data()),1000,0,1000,500,0,500));
                  hIsoEVsPtCoshEta.back()->GetXaxis()->SetTitle("p_{T} x cosh(#eta)[GeV]");
                  hIsoEVsPtCoshEta.back()->GetYaxis()->SetTitle("Isolation Energy");
                  hIsoEVsPtCoshEta.back()->SetMarkerColor((iso*10.0)+1);
                  hIsoEVsPtCoshEta.back()->SetLineColor((iso*10.0)+1);
               }
            }
            if (ientry%10000 == 0)
               cout << "\tIsolationPlotter_x Doing entry " << ientry << " ..." << endl;
            t->GetEntry(ientry);

            if(ntuple->lLV[0].leptonCat != DEFS::electron)
               continue;
            if(ntuple->METLV[0].Pt() <= 25.0)
               continue;
            if (ntuple->jLV[0].Pt() < 30 || ntuple->jLV[1].Pt() < 25)
               continue;

            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso<0.2&&ntuple->lLV[0].Pt()>30) {
               hptDistributions[0]->Fill(ntuple->lLV[0].Pt());
            }
            if(ntuple->lLV[0].emvaTrig>0.95&&ntuple->lLV[0].lpfIso<0.2&&ntuple->lLV[0].Pt()>30) {
               hptDistributions[1]->Fill(ntuple->lLV[0].Pt());
            }
            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].Pt()>30) {
               hptDistributions[2]->Fill(ntuple->lLV[0].Pt());
            }
            //else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2)))>30) {
            //else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.0098057,2)))>30) {
            //else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2)))>30) {
            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)))>30) {
            //if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+2.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)))>30) {
               //t->Draw("lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2))>>"+hanme.back().Data(),
               //        "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&(lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2)))>30");
               //hptDistributions[2]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.4948,2))/TMath::Sqrt(1.0+TMath::Power(0.1,2)));
               //hptDistributions[2]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.0098057,2)));
               //hptDistributions[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power(0.48,2)));

               hptDistributions[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
               //hptDistributions[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+2.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));   
            }
            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].lpfIso<1.0&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)))>30) {
               hptDistributions[4]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));   
            }
            if(ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)))>30) {
               hptDistributions[5]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));   
            }
            if(ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].Pt()>30) {
               hptDistributions[6]->Fill(ntuple->lLV[0].Pt());
            }
            if(ntuple->lLV[0].emvaTrig>0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].Pt()>30) {
               hptDistributions[7]->Fill(ntuple->lLV[0].Pt());
            }
            if(ntuple->lLV[0].emvaTrig>0.95&&ntuple->lLV[0].lpfIso>0.3&&(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)))>30) {
               hptDistributions[8]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }


            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.0&&ntuple->lLV[0].lpfIso<0.1) {
               hptDistributionsShifted[0]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.1&&ntuple->lLV[0].lpfIso<0.2) {
               hptDistributionsShifted[1]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.2&&ntuple->lLV[0].lpfIso<0.3) {
               hptDistributionsShifted[2]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].lpfIso<0.4) {
               hptDistributionsShifted[3]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.4&&ntuple->lLV[0].lpfIso<0.5) {
               hptDistributionsShifted[4]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.5&&ntuple->lLV[0].lpfIso<0.6) {
               hptDistributionsShifted[5]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.6&&ntuple->lLV[0].lpfIso<0.7) {
               hptDistributionsShifted[6]->Fill(ntuple->lLV[0].Pt());
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.7&&ntuple->lLV[0].lpfIso<0.8) {
               hptDistributionsShifted[7]->Fill(ntuple->lLV[0].Pt());
            }

            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.0&&ntuple->lLV[0].lpfIso<0.1) {
               //hptDistributionsCorrected[0]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.0871531,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[0]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.00980507,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[0]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.1&&ntuple->lLV[0].lpfIso<0.2) {
               //hptDistributionsCorrected[1]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.259677,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[1]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.02329070,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[1]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.2&&ntuple->lLV[0].lpfIso<0.3) {
               //hptDistributionsCorrected[2]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.452971,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[2]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.04601302,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[2]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].lpfIso<0.4) {
               //hptDistributionsCorrected[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.08399500,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[3]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.4&&ntuple->lLV[0].lpfIso<0.5) {
               //hptDistributionsCorrected[4]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[4]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.13130544,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[4]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.5&&ntuple->lLV[0].lpfIso<0.6) {
               //hptDistributionsCorrected[5]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[5]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.18035410,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[5]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.6&&ntuple->lLV[0].lpfIso<0.7) {
               //hptDistributionsCorrected[6]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[6]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.22153813,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[6]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }
            else if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.7&&ntuple->lLV[0].lpfIso<0.8) {
               //hptDistributionsCorrected[7]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.604066,2))/TMath::Sqrt(1.0+TMath::Power(0.0871531,2)));
               //hptDistributionsCorrected[7]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(0.26718962,2))/TMath::Sqrt(1.0+TMath::Power(0.00980507,2)));
               hptDistributionsCorrected[7]->Fill(ntuple->lLV[0].Pt()*TMath::Sqrt(1.0+TMath::Power(ntuple->lLV[0].lpfIso,2))/TMath::Sqrt(1.0+TMath::Power((constVec[0]+constVec[1])/2.0,2)));
            }

            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso<isoMax) {
               int hindex = get_index(ntuple->lLV[0].lpfIso,isos);
               hIsoEVsPt[hindex]->Fill(ntuple->lLV[0].Pt(),ntuple->lLV[0].lpfIso*ntuple->lLV[0].Pt());
               hIsoEVsPtCoshEta[hindex]->Fill(ntuple->lLV[0].Pt()*TMath::CosH(ntuple->lLV[0].Eta()),ntuple->lLV[0].lpfIso*ntuple->lLV[0].Pt());
            }

         }

         /*******************************************************************************************/
         //pT Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) 
         /*******************************************************************************************/
         cout << "\tDoing pT distribution comparison ..." << std::flush;
         cname.push_back(Form("cptDistributions_%s",ifilePostFixes[f].Data()));
         cptDistributions.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         THStack* sptDistributions = new THStack(Form("sptDistributions_%s",ifilePostFixes[f].Data()),
                                                 Form("p_{T} Distribution (in control region and signal region) Comparison (Corrected Vs. Uncorrected) (%s)",ifilePostFixes[f].Data()));
         for(unsigned int hh=0; hh<hptDistributions.size();hh++) {
            hptDistributions[hh]->Scale(1.0/hptDistributions[hh]->Integral());
            if(hh==1)
               sptDistributions->Add(hptDistributions[hh],"ep");
            else
               sptDistributions->Add(hptDistributions[hh]);
         }
         sptDistributions->Draw("nostack");
         sptDistributions->GetXaxis()->SetTitle("p_{T} [GeV]");
         sptDistributions->GetXaxis()->SetRangeUser(0,150);
         sptDistributions->GetYaxis()->SetTitle("Events / 1 GeV");
         sptDistributions->GetYaxis()->SetTitleOffset(1.4);

         TH1D* dummy = 0;
         legs.push_back(new TLegend(0.5,0.4,0.9,0.6));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         legs.back()->AddEntry(dummy,"Uncorrected","");
         legs.back()->AddEntry(hptDistributions[1],"0.95<MVA EID<1.00 && pfIso<0.2 && p_{T}^{Lepton}>30 GeV (a.k.a Data)","ep");
         legs.back()->AddEntry(hptDistributions[0],"0.05<MVA EID<0.95 && pfIso<0.2 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[6],"pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[2],"0.05<MVA EID<0.95 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[7],"0.95<MVA EID<1.00 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(dummy,"Corrected","");
         legs.back()->AddEntry(hptDistributions[3],"0.05<MVA EID<0.95 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[4],"0.05<MVA EID<0.95 && 0.3<pfIso<1.0 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[5],"pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->AddEntry(hptDistributions[8],"0.95<MVA EID<1.00 && pfIso>0.3 && p_{T}^{Lepton}>30 GeV","l");
         legs.back()->Draw("sames");

         gPad->Update();
         for(unsigned int hh=0; hh<hptDistributions.size();hh++) {
            double max = hptDistributions[hh]->GetBinCenter(hptDistributions[hh]->GetMaximumBin());
            DrawVerticalLine(max,hptDistributions[hh]->GetLineColor(),2);
         }
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
         //Make Weights
         /*******************************************************************************************/
         cout << "\tMaking weights ... " << std::flush;
         vector<double> ptBinBoundaries;
         for(double i=0; i<70; i++){
            ptBinBoundaries.push_back(i);
         }
         for(double i=70; i<100; i+=2){
            ptBinBoundaries.push_back(i);
         }
         for(double i=100; i<150; i+=5){
            ptBinBoundaries.push_back(i);
         }
         for(double i=150; i<250; i+=10){
            ptBinBoundaries.push_back(i);
         }
         for(double i=250; i<400; i+=50){
            ptBinBoundaries.push_back(i);
         }
         for(double i=400; i<1000; i+=100){
            ptBinBoundaries.push_back(i);
         }
         //for(double i=1000; i<2000; i+=500){
         //   ptBinBoundaries.push_back(i);
         //}

         //TH1D* weights = (TH1D*)hptDistributions[5]->Clone("num");
         TH1D* weights = (TH1D*)hptDistributions[3]->Clone("weights");
         TH1D* weights_rebinned = (TH1D*)weights->Rebin(ptBinBoundaries.size()-1,"QCDWeight_electron",&ptBinBoundaries.at(0));
         TH1D* num_rebinned = (TH1D*)weights_rebinned->Clone("num_rebinned");
         //TH1D* den = (TH1D*)hptDistributions[6]->Clone("den");
         TH1D* den = (TH1D*)hptDistributions[2]->Clone("den");
         TH1D* den_rebinned = (TH1D*)den->Rebin(ptBinBoundaries.size()-1,"den_rebinned",&ptBinBoundaries.at(0));

         weights_rebinned->Divide(den_rebinned);
         weights_rebinned->SetNameTitle("QCDWeight_electron","QCDWeight_electron");

         hWeights.push_back(num_rebinned);
         hWeights.push_back(den_rebinned);
         hWeights.push_back(weights_rebinned);

         cname.push_back(Form("cWeights_%s",ifilePostFixes[f].Data()));
         cWeights.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         hWeights.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hWeights.back()->GetYaxis()->SetTitle("Weight");
         hWeights.back()->Draw();
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Show Shifted pT Peaks 
         /*******************************************************************************************/
         cout << "\tDoing shifted pT peaks ... " << std::flush;
         cname.push_back(Form("cShiftedPt_%s",ifilePostFixes[f].Data()));
         cptDistributionsShifted.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         TH1D* hptDistributionsShiftedSum;
         THStack* sptDistributionsShifted = new THStack(Form("sShiftedPt_%s",ifilePostFixes[f].Data()),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()));
         for(unsigned int hh=0; hh<hptDistributionsShifted.size();hh++) {
            hptDistributionsShifted[hh]->Scale(1.0/hptDistributionsShifted[hh]->Integral());
            if(hh==0)
               hptDistributionsShiftedSum = (TH1D*)hptDistributionsShifted[0]->Clone();
            else
               hptDistributionsShiftedSum->Add(hptDistributionsShifted[hh]);
            sptDistributionsShifted->Add(hptDistributionsShifted[hh]);
         }
         sptDistributionsShifted->Draw(stackDrawOption);
         sptDistributionsShifted->GetXaxis()->SetTitle("p_{T} [GeV]");
         sptDistributionsShifted->GetXaxis()->SetRangeUser(0,150);
         sptDistributionsShifted->GetYaxis()->SetTitle("Events / 1 GeV");
         sptDistributionsShifted->GetYaxis()->SetTitleOffset(1.4);

         if(addData) {
            dataFile->cd("PS");
            dataHist = new TH1D(Form("hShiftedPt_Data_%s",ifilePostFixes[f].Data()),Form("Uncorrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000);
            dataHist->Sumw2();
            dataTree->Draw("lLV[0].Pt()>>"+TString(Form("hShiftedPt_Data_%s",ifilePostFixes[f].Data())),"","goff");
            if(stackDrawOption.CompareTo("nostack")==0)
               dataHist->Scale(1.0/dataHist->Integral());
            else
               dataHist->Scale(hptDistributionsShiftedSum->Integral()/dataHist->Integral());
            dataHist->SetMarkerColor(kBlack);
            dataHist->SetLineColor(kBlack);
            dataHist->SetMarkerStyle(20);
            dataHist->SetMarkerSize(0.7);
            dataHist->Draw("ep same");
         }

         legs.push_back(new TLegend(0.5,0.35,0.9,0.65));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         legs.back()->AddEntry(dataHist,"Data","ep");
         legs.back()->AddEntry(hptDistributionsShifted[0],"0.05<MVA EID<0.95 && 0.0<pfIso<0.1","l");
         legs.back()->AddEntry(hptDistributionsShifted[1],"0.05<MVA EID<0.95 && 0.1<pfIso<0.2","l");
         legs.back()->AddEntry(hptDistributionsShifted[2],"0.05<MVA EID<0.95 && 0.2<pfIso<0.3","l");
         legs.back()->AddEntry(hptDistributionsShifted[3],"0.05<MVA EID<0.95 && 0.3<pfIso<0.4","l");
         legs.back()->AddEntry(hptDistributionsShifted[4],"0.05<MVA EID<0.95 && 0.4<pfIso<0.5","l");
         legs.back()->AddEntry(hptDistributionsShifted[5],"0.05<MVA EID<0.95 && 0.5<pfIso<0.6","l");
         legs.back()->AddEntry(hptDistributionsShifted[6],"0.05<MVA EID<0.95 && 0.6<pfIso<0.7","l");
         legs.back()->AddEntry(hptDistributionsShifted[7],"0.05<MVA EID<0.95 && 0.7<pfIso<0.8","l");
         legs.back()->Draw("sames");
         gPad->Update();
         DrawVerticalLine(30.0,kRed,2);
         double max = hptDistributionsShiftedSum->GetBinCenter(hptDistributionsShiftedSum->GetMaximumBin());
         DrawVerticalLine(max,kBlue,2);
         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Show Corrected pT Peaks 
         /*******************************************************************************************/
         cout << "\tDoing corrected pT peaks ... " << std::flush;
         cname.push_back(Form("cCorrectedPt_%s",ifilePostFixes[f].Data()));
         cptDistributionsCorrected.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         TH1D* hptDistributionsCorrectedSum;
         THStack* sptDistributionsCorrected = new THStack(Form("sCorrectedPt_%s",ifilePostFixes[f].Data()),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()));
         for(unsigned int hh=0; hh<hptDistributionsCorrected.size();hh++) {
            hptDistributionsCorrected[hh]->Scale(1.0/hptDistributionsCorrected[hh]->Integral());
            //if(hh==0)
            if(hh==3)
               hptDistributionsCorrectedSum = (TH1D*)hptDistributionsCorrected[hh]->Clone();
            //else
            else if (hh>3)
               hptDistributionsCorrectedSum->Add(hptDistributionsCorrected[hh]);
            sptDistributionsCorrected->Add(hptDistributionsCorrected[hh]);
         }
         sptDistributionsCorrected->Draw(stackDrawOption);
         sptDistributionsCorrected->GetXaxis()->SetTitle("p_{T} [GeV]");
         sptDistributionsCorrected->GetXaxis()->SetRangeUser(0,150);
         sptDistributionsCorrected->GetYaxis()->SetTitle("Events / 1 GeV");
         sptDistributionsCorrected->GetYaxis()->SetTitleOffset(1.4);

         if(addData) {
            dataFile->cd("PS");
            dataHist = new TH1D(Form("hCorrectedPt_Data_%s",ifilePostFixes[f].Data()),Form("Corrected p_{T} Distributions (%s)",ifilePostFixes[f].Data()),1000,0,1000);
            dataHist->Sumw2();
            dataTree->Draw("lLV[0].Pt()>>"+TString(Form("hCorrectedPt_Data_%s",ifilePostFixes[f].Data())),"","goff");
            if(stackDrawOption.CompareTo("nostack")==0)
               dataHist->Scale(1.0/dataHist->Integral());
            else
               dataHist->Scale(hptDistributionsCorrectedSum->Integral()/dataHist->Integral());
            dataHist->SetMarkerColor(kBlack);
            dataHist->SetLineColor(kBlack);
            dataHist->SetMarkerStyle(20);
            dataHist->SetMarkerSize(0.7);
            dataHist->Draw("ep same");
         }

         legs.push_back(new TLegend(0.5,0.35,0.9,0.65));
         legs.back()->SetFillColor(0);
         legs.back()->SetFillStyle(0);
         legs.back()->SetLineColor(0);
         legs.back()->SetLineWidth(0);
         legs.back()->SetBorderSize(0);
         legs.back()->AddEntry(dataHist,"Data","ep");
         legs.back()->AddEntry(hptDistributionsCorrected[0],"0.05<MVA EID<0.95 && 0.0<pfIso<0.1","l");
         legs.back()->AddEntry(hptDistributionsCorrected[1],"0.05<MVA EID<0.95 && 0.1<pfIso<0.2","l");
         legs.back()->AddEntry(hptDistributionsCorrected[2],"0.05<MVA EID<0.95 && 0.2<pfIso<0.3","l");
         legs.back()->AddEntry(hptDistributionsCorrected[3],"0.05<MVA EID<0.95 && 0.3<pfIso<0.4","l");
         legs.back()->AddEntry(hptDistributionsCorrected[4],"0.05<MVA EID<0.95 && 0.4<pfIso<0.5","l");
         legs.back()->AddEntry(hptDistributionsCorrected[5],"0.05<MVA EID<0.95 && 0.5<pfIso<0.6","l");
         legs.back()->AddEntry(hptDistributionsCorrected[6],"0.05<MVA EID<0.95 && 0.6<pfIso<0.7","l");
         legs.back()->AddEntry(hptDistributionsCorrected[7],"0.05<MVA EID<0.95 && 0.7<pfIso<0.8","l");
         legs.back()->Draw("sames");
         gPad->Update();
         DrawVerticalLine(30.0,kRed,2);
         double max2 = hptDistributionsCorrectedSum->GetBinCenter(hptDistributionsCorrectedSum->GetMaximumBin());
         DrawVerticalLine(max2,kBlue,2);
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
         cout << "\tDoing isolation energy vs. pT ... " << std::flush;
         //Setting a smoother palette
         const Int_t NRGBs = 5;
         const Int_t NCont = 104;
         Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
         Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
         Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
         Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
         TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
         gStyle->SetNumberContours(NCont);
         cname.push_back(Form("cIsoEVsPt_%s",ifilePostFixes[f].Data()));
         cIsoEVsPt.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         THStack* sIsoEVsPt = new THStack(Form("sIsoEVsPt_%s",ifilePostFixes[f].Data()),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()));
         for(unsigned int h=0; h<hIsoEVsPt.size(); h++) {
            cout << endl << "****************************************" << endl << endl;
            cout << "(" << Form("%.1f",isoBinWidth*h) << " < PF Isolation < " << Form("%.1f",(isoBinWidth*h)+isoBinWidth) << ")" << endl;
            if(!hIsoEVsPt[h]->Fit("pol1","0")) {
               if(hIsoEVsPt[h]->GetFunction("pol1")->GetChisquare()/hIsoEVsPt[h]->GetFunction("pol1")->GetNDF()<900.0 &&
                  hIsoEVsPt[h]->GetFunction("pol1")->GetNDF()>=10) {
                  fitParameters[isoBinWidth*h]=make_pair(hIsoEVsPt[h]->GetFunction("pol1")->GetParameter(0),hIsoEVsPt[h]->GetFunction("pol1")->GetParameter(1));
                  fits.push_back(hIsoEVsPt[h]->GetFunction("pol1"));
               }
            }
            sIsoEVsPt->Add(hIsoEVsPt[h]);
         }
         sIsoEVsPt->Draw("colz nostack");
         sIsoEVsPt->GetXaxis()->SetTitle("p_{T} [GeV/c]");
         sIsoEVsPt->GetYaxis()->SetTitle("Isolation Energy");
         for(unsigned int ff=0; ff<fits.size(); ff++){
            fits[ff]->Draw("same");
         }

         cout << endl << "****************************************" << endl << endl;
         cout << "Parameters from all of the fits:" << endl;
         for(map<double,pair<double,double> >::iterator it=fitParameters.begin(); it!=fitParameters.end();it++) {
            cout << Form("%.1f",it->first) << " < PF Isolation < " << Form("%.1f",it->first+isoBinWidth) <<"\tSlope = " 
                 << Form("%.8f",it->second.second) << "\ty-intercept = " << Form("%.4f",it->second.first) << endl;
         }
         cout << endl << "****************************************" << endl << endl;
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
         /*
         PS->cd()
         gStyle->SetPalette(1)
         TCanvas * c = new TCanvas("c","c",800,800)
         TH2D* h = new TH2D("h","h",500,0,500,500,0,500)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()>>h","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95","colz")
         TCanvas * cc = new TCanvas("cc","cc",800,800)
         TH2D* hh = new TH2D("hh","hh",500,0,500,500,0,500)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>hh","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95","colz")
         */

         /*******************************************************************************************/
         //Show Isolation Energy Vs. pT x cosh(eta)
         /*******************************************************************************************/
         cout << "\tDoing isolation energy vs. pT x cosh(eta) ... " << std::flush;
         cname.push_back(Form("cIsoEVsPtCoshEta_%s",ifilePostFixes[f].Data()));
         cIsoEVsPtCoshEta.push_back(new TCanvas(cname.back(),cname.back(),1200,800));
         THStack* sIsoEVsPtCoshEta = new THStack(Form("sIsoEVsPtCoshEta_%s",ifilePostFixes[f].Data()),Form("Isolation Energy Vs. p_{T} x cosh(#eta) (%s)",ifilePostFixes[f].Data()));
         for(unsigned int h=0; h<hIsoEVsPt.size(); h++) {
            cout << endl << "****************************************" << endl << endl;
            cout << "(" << Form("%.1f",isoBinWidth*h) << " < PF Isolation < " << Form("%.1f",(isoBinWidth*h)+isoBinWidth) << ")" << endl;
            if(!hIsoEVsPtCoshEta[h]->Fit("pol1","0")) {
               if(hIsoEVsPtCoshEta[h]->GetFunction("pol1")->GetChisquare()/hIsoEVsPtCoshEta[h]->GetFunction("pol1")->GetNDF()<900.0 &&
                  hIsoEVsPtCoshEta[h]->GetFunction("pol1")->GetNDF()>=10) {
                  fitParameters2[isoBinWidth*h]=make_pair(hIsoEVsPtCoshEta[h]->GetFunction("pol1")->GetParameter(0),hIsoEVsPtCoshEta[h]->GetFunction("pol1")->GetParameter(1));
                  fits2.push_back(hIsoEVsPtCoshEta[h]->GetFunction("pol1"));
               }
            }
            sIsoEVsPtCoshEta->Add(hIsoEVsPtCoshEta[h]);
         }
         sIsoEVsPtCoshEta->Draw("colz nostack");
         sIsoEVsPtCoshEta->GetXaxis()->SetTitle("p_{T} x cosh(#eta) [GeV]");
         sIsoEVsPtCoshEta->GetYaxis()->SetTitle("Isolation Energy");
         for(unsigned int ff=0; ff<fits2.size(); ff++){
            fits2[ff]->Draw("same");
         }

         cout << endl << "****************************************" << endl << endl;
         cout << "Parameters from all of the fits:" << endl;
         for(map<double,pair<double,double> >::iterator it=fitParameters2.begin(); it!=fitParameters2.end();it++) {
            cout << Form("%.1f",it->first) << " < PF Isolation < " << Form("%.1f",it->first+isoBinWidth) <<"\tSlope = " 
                 << Form("%.8f",it->second.second) << "\ty-intercept = " << Form("%.4f",it->second.first) << endl;
         }
         cout << endl << "****************************************" << endl << endl;
         cout << "DONE" << endl;

         /*
         PS->cd()
         gStyle->SetPalette(1)
         TCanvas * c = new TCanvas("c","c",800,800)
         TH2D* h01 = new TH2D("h01","h01",500,0,500,500,0,500)
         h01->SetMarkerColor(kBlack)
         h01->SetLineColor(kBlack)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h01","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.0&&lLV[0].lpfIso<0.1","colz")
         TH2D* h12 = new TH2D("h12","h12",500,0,500,500,0,500)
         h12->SetMarkerColor(kRed)
         h12->SetLineColor(kRed)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h12","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.1&&lLV[0].lpfIso<0.2","colz sames")
         TH2D* h23 = new TH2D("h23","h23",500,0,500,500,0,500)
         h23->SetMarkerColor(kBlue)
         h23->SetLineColor(kBlue)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h23","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.2&&lLV[0].lpfIso<0.3","colz sames")
         TH2D* h34 = new TH2D("h34","h34",500,0,500,500,0,500)
         h34->SetMarkerColor(kGreen)
         h34->SetLineColor(kGreen)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h34","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.3&&lLV[0].lpfIso<0.4","colz sames")
         TH2D* h45 = new TH2D("h45","h45",500,0,500,500,0,500)
         h45->SetMarkerColor(kMagenta)
         h45->SetLineColor(kMagenta)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h45","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.4&&lLV[0].lpfIso<0.5","colz sames")
         TH2D* h56 = new TH2D("h56","h56",500,0,500,500,0,500)
         h56->SetMarkerColor(kOrange)
         h56->SetLineColor(kOrange)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h56","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.5&&lLV[0].lpfIso<0.6","colz sames")
         TH2D* h67 = new TH2D("h67","h67",500,0,500,500,0,500)
         h67->SetMarkerColor(kCyan)
         h67->SetLineColor(kCyan)
         jets2p->Draw("lLV[0].lpfIso*lLV[0].Pt():lLV[0].Pt()*TMath::CosH(lLV[0].Eta())>>h67","lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].lpfIso>0.6&&lLV[0].lpfIso<0.7","colz sames")
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

      if((plots.CompareTo("ALL")==0 || plots.CompareTo("RATIO")==0) && (ifilePostFixes[f].CompareTo("QCD")==0 || ifilePostFixes[f].CompareTo("qcd")==0)) {
         /*******************************************************************************************/
         //Open WJets file with Iso up to 0.2
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

         //Opening wjets file
         TFile* wjetsFile;
         TTree* wjetsTree;
         wjetsFile = new TFile("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130514_FullIsoMVA_WJets/PS.root","READ");
         if(!wjetsFile->IsOpen()) {
            cout << "\tERROR::Could not open file /eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130514_FullIsoMVA_WJets/PS.root" << endl;
            return 0;
         }
         if (!wjetsFile->cd("PS")) {
            cout << "\tERROR::Could not CD into directory PS in file /eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130514_FullIsoMVA_WJets/PS.root" << endl;
            return 0;
         }
         wjetsTree = (TTree*)gDirectory->Get("jets2p");
         if (wjetsTree == 0) {
            cout << "\tERROR::Could not find tree named jets2p in file /eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130514_FullIsoMVA_WJets/PS.root" << endl;
            return 0;
         }

         EventNtuple * ntuple = new EventNtuple();
         t->SetBranchAddress("EvtNtuple", &ntuple);
         Int_t nentries = t->GetEntries();
         t->SetBranchStatus("*",0);
         t->SetBranchStatus("lLV*");
         t->SetBranchStatus("METLV*");
         t->SetBranchStatus("jLV*");
         for(Int_t ientry = 0; ientry<nentries; ientry++) {
            if(maxEvts!=0 && ientry>maxEvts)
               continue;
            if(ientry==0) {
               hname.push_back(Form("hIsoEVsPt_antiMVA_0.0to0.2_full_%s",ifilePostFixes[f].Data()));
               hRicardoMethod.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
               hRicardoMethod.back()->GetXaxis()->SetTitle("p_{T} [GeV/c]");
               hRicardoMethod.back()->GetYaxis()->SetTitle("Isolation Energy");
               hRicardoMethod.back()->SetMarkerColor(kBlack);
               hRicardoMethod.back()->SetLineColor(kBlack);
               hRicardoMethod.back()->Sumw2();
               hname.push_back(Form("hIsoEVsPt_antiMVA_0.3toInf_full_%s",ifilePostFixes[f].Data()));
               hRicardoMethod.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
               hRicardoMethod.back()->GetXaxis()->SetTitle("p_{T} [GeV/c]");
               hRicardoMethod.back()->GetYaxis()->SetTitle("Isolation Energy");
               hRicardoMethod.back()->SetMarkerColor(kRed);
               hRicardoMethod.back()->SetLineColor(kRed);
               hRicardoMethod.back()->Sumw2();
               hname.push_back(Form("hIsoEVsPt_antiMVA_0.0to0.2_wjets_%s",ifilePostFixes[f].Data()));
               hRicardoMethod.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
               hRicardoMethod.back()->GetXaxis()->SetTitle("p_{T} [GeV/c]");
               hRicardoMethod.back()->GetYaxis()->SetTitle("Isolation Energy");
               hRicardoMethod.back()->SetMarkerColor(kBlue);
               hRicardoMethod.back()->SetLineColor(kBlue);
               hRicardoMethod.back()->Sumw2();
               hname.push_back(Form("hIsoEVsPt_antiMVA_0.0to0.2_subtracted_%s",ifilePostFixes[f].Data()));
               hRicardoMethod.push_back(new TH2D(hname.back(),Form("Isolation Energy Vs. p_{T} (%s)",ifilePostFixes[f].Data()),500,0,500,500,0,500));
               hRicardoMethod.back()->GetXaxis()->SetTitle("p_{T} [GeV/c]");
               hRicardoMethod.back()->GetYaxis()->SetTitle("Isolation Energy");
               hRicardoMethod.back()->SetMarkerColor(kGreen);
               hRicardoMethod.back()->SetLineColor(kGreen);
               hRicardoMethod.back()->Sumw2();
            }
            if (ientry%10000 == 0)
               cout << "\tIsolationPlotter_x Doing entry " << ientry << " ..." << endl;
            t->GetEntry(ientry);

            if(ntuple->lLV[0].leptonCat != DEFS::electron)
               continue;
            if(ntuple->METLV[0].Pt() <= 25.0)
               continue;
            if (ntuple->jLV[0].Pt() < 30 || ntuple->jLV[1].Pt() < 25)
               continue;

            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso<0.2&&ntuple->lLV[0].Pt()>30) {
               hRicardoMethod[0]->Fill(ntuple->lLV[0].Pt(),ntuple->lLV[0].lpfIso*ntuple->lLV[0].Pt());
               hRicardoMethod[3]->Fill(ntuple->lLV[0].Pt(),ntuple->lLV[0].lpfIso*ntuple->lLV[0].Pt());
            }
            if(ntuple->lLV[0].emvaTrig>0.05&&ntuple->lLV[0].emvaTrig<0.95&&ntuple->lLV[0].lpfIso>0.3&&ntuple->lLV[0].Pt()>30) {
               hRicardoMethod[1]->Fill(ntuple->lLV[0].Pt(),ntuple->lLV[0].lpfIso*ntuple->lLV[0].Pt());
            }
         }

         /*******************************************************************************************/
         //Show MC Subtracted Isolation Energy Vs. pT
         /*******************************************************************************************/
         cout << "\tDoing MC subtracted isolation energy vs. pT ... " << std::flush;
         cname.push_back(Form("cRicardoMethod_%s",ifilePostFixes[f].Data()));
         cRicardoMethod.push_back(new TCanvas(cname.back(),cname.back(),800,800));

         wjetsTree->Draw("lLV[0].Pt()*lLV[0].lpfIso:lLV[0].Pt()>>"+TString(Form("hIsoEVsPt_antiMVA_0.0to0.2_wjets_%s",ifilePostFixes[f].Data())),
                            "lLV[0].emvaTrig>0.05&&lLV[0].emvaTrig<0.95&&lLV[0].Pt()>30","goff");
         //cout << "Name = " << hRicardoMethod[2]->GetName() << endl;
         //cout << "Title = " << hRicardoMethod[2]->GetTitle() << endl;
         //cout << "Entries = " << hRicardoMethod[2]->GetEntries() << endl;
         //hRicardoMethod[2]->Scale(2.79399600000000000e+06/18393090.0);
         hRicardoMethod[2]->Scale((19148.0*41440.805)/18393090.0);
         hRicardoMethod[3]->Add(hRicardoMethod[2],-1.0);

         hRicardoMethod[3]->Draw("colz");
         hRicardoMethod[1]->Draw("colz same");

         TF1* func = new TF1("func","0.2*x[0]",0,500);
         func->Draw("same");
         TF1* func2 = new TF1("func2","0.3*x[0]",0,500);
         func2->Draw("same");

         cout << "DONE" << endl;

         /*******************************************************************************************/
         //Make Weights
         /*******************************************************************************************/
         cout << "\tMaking weights ... " << std::flush;
         vector<double> ptBinBoundaries;
         for(double i=0; i<70; i++){
            ptBinBoundaries.push_back(i);
         }
         for(double i=70; i<100; i+=2){
            ptBinBoundaries.push_back(i);
         }
         for(double i=100; i<150; i+=5){
            ptBinBoundaries.push_back(i);
         }
         for(double i=150; i<250; i+=10){
            ptBinBoundaries.push_back(i);
         }
         for(double i=250; i<400; i+=50){
            ptBinBoundaries.push_back(i);
         }
         for(double i=400; i<500; i+=100){
            ptBinBoundaries.push_back(i);
         }

         TH1D* ratio = hRicardoMethod[3]->ProjectionX("ratio");
         TH1D* sub_px = (TH1D*)ratio->Clone("sub_px");
         TH1D* full_px = hRicardoMethod[1]->ProjectionX();

         TH1D* ratio_rebinned = (TH1D*)ratio->Rebin(ptBinBoundaries.size()-1,"QCDWeight_electron",&ptBinBoundaries.at(0));
         TH1D* sub_px_rebinned = (TH1D*)sub_px->Rebin(ptBinBoundaries.size()-1,"sub_px_rebinned",&ptBinBoundaries.at(0));
         TH1D* full_px_rebinned = (TH1D*)full_px->Rebin(ptBinBoundaries.size()-1,"full_px_rebinned",&ptBinBoundaries.at(0));

         ratio->Divide(full_px);
         ratio_rebinned->Divide(full_px_rebinned);
         ratio_rebinned->SetNameTitle("QCDWeight_electron","QCDWeight_electron");

         hWeights.push_back(sub_px);
         hWeights.push_back(full_px);
         hWeights.push_back(sub_px_rebinned);
         hWeights.push_back(full_px_rebinned);
         hWeights.push_back(ratio);
         hWeights.push_back(ratio_rebinned);

         cname.push_back(Form("cWeights_%s",ifilePostFixes[f].Data()));
         cWeights.push_back(new TCanvas(cname.back(),cname.back(),800,800));
         hWeights.back()->GetXaxis()->SetTitle("p_{T} [GeV]");
         hWeights.back()->GetYaxis()->SetTitle("Weight");
         hWeights.back()->Draw();
         cout << "DONE" << endl;
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
         if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0 || plots.CompareTo("CORRECTION_FAST")==0) {
            cptDistributions.back()->SaveAs(ofilepath+cptDistributions.back()->GetTitle()+formats[ff]);
            cptDistributionsShifted.back()->SaveAs(ofilepath+cptDistributionsShifted.back()->GetTitle()+formats[ff]);
            cptDistributionsCorrected.back()->SaveAs(ofilepath+cptDistributionsCorrected.back()->GetTitle()+formats[ff]);
            cWeights.back()->SaveAs(ofilepath+cWeights.back()->GetTitle()+formats[ff]);
            cIsoEVsPt.back()->SaveAs(ofilepath+cIsoEVsPt.back()->GetTitle()+formats[ff]);
            cIsoEVsPtCoshEta.back()->SaveAs(ofilepath+cIsoEVsPtCoshEta.back()->GetTitle()+formats[ff]);
         }
         if(plots.CompareTo("ALL")==0 || plots.CompareTo("RATIO")==0) {
            cRicardoMethod.back()->SaveAs(ofilepath+cRicardoMethod.back()->GetTitle()+formats[ff]);
            cWeights.back()->SaveAs(ofilepath+cWeights.back()->GetTitle()+formats[ff]);
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
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0 || plots.CompareTo("CORRECTION_FAST")==0) {
         cptDistributions.back()->Write();
         cptDistributionsShifted.back()->Write();
         cptDistributionsCorrected.back()->Write();
         cIsoEVsPt.back()->Write();
         cIsoEVsPtCoshEta.back()->Write();
         cWeights.back()->Write();
      }
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("RATIO")==0) {
         cRicardoMethod.back()->Write();
         cWeights.back()->Write();
      }
      cout << "DONE" << endl;

      cout << "\tWriting weights to " << ofilepath+ofilename << ":/weights/" << " ... ";
      ofile->mkdir("weights");
      ofile->cd("weights");
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0 || plots.CompareTo("CORRECTION_FAST")==0 || plots.CompareTo("RATIO")==0) {
         for(unsigned int hh=0; hh<hWeights.size(); hh++) {
            hWeights[hh]->Write();
         }
      }
      cout << "DONE" << endl;

      ofile->cd();
      ofile->mkdir("histograms");
      ofile->cd("histograms");
      cout << "\tWriting histograms to " << ofilepath+ofilename << ":/histograms/" << " ... ";
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("COMPONENTS")==0) {
         for(unsigned int hh=0; hh<hpfIso.size(); hh++) {
           hpfIso[hh]->Write();
         }
         for(unsigned int hh=0; hh<hchargedHadronIso.size(); hh++) {
            hchargedHadronIso[hh]->Write();
         }
         for(unsigned int hh=0; hh<hneutralHadronIso.size(); hh++) {
            hneutralHadronIso[hh]->Write();
         }
         for(unsigned int hh=0; hh<hphotonIso.size(); hh++) {
            hphotonIso[hh]->Write();
         }
         for(unsigned int hh=0; hh<hmvaTrigV0.size(); hh++) {
            hmvaTrigV0[hh]->Write();
         }
         for(unsigned int hh=0; hh<hmvaNonTrigV0.size(); hh++) {
            hmvaNonTrigV0[hh]->Write();
         }
         for(unsigned int hh=0; hh<hpfIsoMVATrigV0.size(); hh++) {
            hpfIsoMVATrigV0[hh]->Write();
         }
      }
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("CORRECTION")==0 || plots.CompareTo("CORRECTION_FAST")==0) {
         for(unsigned int hh=0; hh<hptDistributions.size(); hh++) {
           hptDistributions[hh]->Write();
         }
         for(unsigned int hh=0; hh<hptDistributionsShifted.size(); hh++) {
            hptDistributionsShifted[hh]->Write();
         }
         for(unsigned int hh=0; hh<hptDistributionsCorrected.size(); hh++) {
            hptDistributionsCorrected[hh]->Write();
         }
         for(unsigned int hh=0; hh<hIsoEVsPt.size(); hh++) {
            hIsoEVsPt[hh]->Write();
         }
         for(unsigned int hh=0; hh<hIsoEVsPtCoshEta.size(); hh++) {
            hIsoEVsPtCoshEta[hh]->Write();
         }
         for(unsigned int hh=0; hh<hWeights.size(); hh++) {
            hWeights[hh]->Write();
         }
      }
      if(plots.CompareTo("ALL")==0 || plots.CompareTo("RATIO")==0) {
         for(unsigned int hh=0; hh<hRicardoMethod.size(); hh++) {
            hRicardoMethod[hh]->Write();
         }
         for(unsigned int hh=0; hh<hWeights.size(); hh++) {
            hWeights[hh]->Write();
         }
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
      cIsoEVsPtCoshEta.clear();
      cRicardoMethod.clear();
      cWeights.clear();
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
      hIsoEVsPtCoshEta.clear();
      hRicardoMethod.clear();
      hWeights.clear();
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
      fits.clear();
      fits2.clear();
      fitParameters.clear();
      fitParameters2.clear();
      cout << "DONE" << endl;
   }

   m_benchmark->Stop("event");
   cout << "IsolationPlotter_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;
}

//______________________________________________________________________________
int get_index(double x,const vector<double>& binsx)
{
  for (unsigned int ix=0;ix<binsx.size()-1;ix++)
    if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
  return -1;
}

//______________________________________________________________________________
void DrawVerticalLine(Double_t x, Color_t color, Int_t width)
{
   TLine l;
   l.SetLineColor(color);
   l.SetLineWidth(width);
   Double_t lm = gPad->GetLeftMargin();
   Double_t rm = 1.-gPad->GetRightMargin();
   Double_t tm = 1.-gPad->GetTopMargin();
   Double_t bm = gPad->GetBottomMargin();
   Double_t xndc = (rm-lm)*((x-gPad->GetUxmin())/(gPad->GetUxmax()-gPad->GetUxmin()))+lm;
   l.DrawLineNDC(xndc,bm,xndc,tm);
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
