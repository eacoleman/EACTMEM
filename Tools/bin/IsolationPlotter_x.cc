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
   bool            debug          = cl.getValue<bool>     ("debug",                false);

   if (!cl.check()) 
      return 0;
   cl.print();

   gStyle->SetOptStat(0);

   for(unsigned int i=0; i<ifilePaths.size(); i++) {
      if(!ifilePaths[i].EndsWith("/")) ifilePaths[i] += "/";
   }
   if(!ofilepath.EndsWith("/")) ofilepath += "/";

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
   vector<TH2D*> hpfIso;
   vector<TH2D*> hchargedHadronIso;
   vector<TH2D*> hneutralHadronIso;
   vector<TH2D*> hphotonIso;
   vector<TH2D*> hmvaTrigV0;
   vector<TH2D*> hmvaNonTrigV0;
   vector<TH2D*> hpfIsoMVATrigV0;
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
   vector<TString> sname;
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

      /*******************************************************************************************/
      //PF Isolation
      /*******************************************************************************************/
      cout << "\tDoing PF Isolation ... ";
      cname.push_back(Form("cpfIso_%s",ifilePostFixes[f].Data()));
      cpfIso.push_back(new TCanvas(cname.back(),cname.back(),800,800));
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
      sname.push_back(Form("pfIsoVsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //Charged Hadron Isolation
      /*******************************************************************************************/
      cout << "\tDoing Charged Hadron Isolation ... ";
      cname.push_back(Form("cchargedHadronIso_%s",ifilePostFixes[f].Data()));
      cchargedHadronIso.push_back(new TCanvas(cname.back(),cname.back(),800,800));
      hname.push_back(Form("hchargedHadronIso_%s",ifilePostFixes[f].Data()));
      hchargedHadronIso.push_back(new TH2D(hname.back(),Form("Charged Hadron Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,120,0,60));
      hchargedHadronIso.back()->GetXaxis()->SetTitle("#eta");
      hchargedHadronIso.back()->GetYaxis()->SetTitle("Charged Hadron Isolation");
      pname.push_back(Form("pchargedHadronIso_%s",ifilePostFixes[f].Data()));
      pchargedHadronIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
      t->Draw("lLV[0].lchargedHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
      t->Draw("lLV[0].lchargedHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
      sname.push_back(Form("chargedHadronIsoVsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //Neutral Hadron Isolation
      /*******************************************************************************************/
      cout << "\tDoing Neutral Hadron Isolation ... ";
      cname.push_back(Form("cneutralHadronIso_%s",ifilePostFixes[f].Data()));
      cneutralHadronIso.push_back(new TCanvas(cname.back(),cname.back(),800,800));
      hname.push_back(Form("hneutralHadronIso_%s",ifilePostFixes[f].Data()));
      hneutralHadronIso.push_back(new TH2D(hname.back(),Form("Neutral Hadron Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,30,0,15));
      hneutralHadronIso.back()->GetXaxis()->SetTitle("#eta");
      hneutralHadronIso.back()->GetYaxis()->SetTitle("Neutral Hadron Isolation");
      pname.push_back(Form("pneutralHadronIso_%s",ifilePostFixes[f].Data()));
      pneutralHadronIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
      t->Draw("lLV[0].lneutralHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
      t->Draw("lLV[0].lneutralHadronIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
      sname.push_back(Form("neutralHadronIsoVsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //Photon Isolation
      /*******************************************************************************************/
      cout << "\tDoing Photon Isolation ... ";
      cname.push_back(Form("cphotonIso_%s",ifilePostFixes[f].Data()));
      cphotonIso.push_back(new TCanvas(cname.back(),cname.back(),800,800));
      hname.push_back(Form("hphotonIso_%s",ifilePostFixes[f].Data()));
      hphotonIso.push_back(new TH2D(hname.back(),Form("Photon Isolation Vs Eta (%s)",ifilePostFixes[f].Data()),120,-3,3,120,0,60));
      hphotonIso.back()->GetXaxis()->SetTitle("#eta");
      hphotonIso.back()->GetYaxis()->SetTitle("Photon Isolation");
      pname.push_back(Form("pphotonIso_%s",ifilePostFixes[f].Data()));
      pphotonIso.push_back(new TProfile(pname.back(),pname.back(),120,-3,3));
      t->Draw("lLV[0].lphotonIso/lLV[0].Pt():lLV[0].Eta()>>"+hname.back(),"","colz");
      t->Draw("lLV[0].lphotonIso/lLV[0].Pt():lLV[0].Eta()>>"+pname.back(),"","sames");
      sname.push_back(Form("photonIsoVsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //MVATrigV0
      /*******************************************************************************************/
      cout << "\tDoing MVATrigV0 ... ";
      cname.push_back(Form("cmvaTrigV0_%s",ifilePostFixes[f].Data()));
      cmvaTrigV0.push_back(new TCanvas(cname.back(),cname.back(),800,800));
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
      sname.push_back(Form("mvaTrigV0VsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //MVANonTrigV0
      /*******************************************************************************************/
      cout << "\tDoing MVANonTrigV0 Isolation ... ";
      cname.push_back(Form("cmvaNonTrigV0_%s",ifilePostFixes[f].Data()));
      cmvaNonTrigV0.push_back(new TCanvas(cname.back(),cname.back(),800,800));
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
      sname.push_back(Form("mvaNonTrigV0VsEta_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //PF Isolation Vs. MVA
      /*******************************************************************************************/
      cout << "\tDoing MVANonTrigV0 Isolation ... ";
      cname.push_back(Form("cpfIsoMVATrigV0_%s",ifilePostFixes[f].Data()));
      cpfIsoMVATrigV0.push_back(new TCanvas(cname.back(),cname.back(),800,800));
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
      sname.push_back(Form("pfIsoVsMVATrigV0_%s",ifilePostFixes[f].Data()));
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //SaveAs & Write
      /*******************************************************************************************/

      cout << "\tSaving files ... ";
      for(unsigned int ff=0; ff<formats.size(); ff++) {
         if (debug) cout << "\t\tSaving canvas " << ofilepath+sname[0]+formats[ff] << " ... ";
         cpfIso.back()->SaveAs(ofilepath+sname[0]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[1]+formats[ff] << " ... ";
         cchargedHadronIso.back()->SaveAs(ofilepath+sname[1]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[2]+formats[ff] << " ... ";
         cneutralHadronIso.back()->SaveAs(ofilepath+sname[2]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[3]+formats[ff] << " ... ";
         cphotonIso.back()->SaveAs(ofilepath+sname[3]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[4]+formats[ff] << " ... ";
         cmvaTrigV0.back()->SaveAs(ofilepath+sname[4]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[5]+formats[ff] << " ... ";
         cmvaNonTrigV0.back()->SaveAs(ofilepath+sname[5]+formats[ff]);
         if (debug) cout << "DONE" << endl << "\t\tSaving canvas " << ofilepath+sname[6]+formats[ff] << " ... ";
         cpfIsoMVATrigV0.back()->SaveAs(ofilepath+sname[6]+formats[ff]);
         if (debug) cout << "DONE" << endl;
      }
      cout << "DONE" << endl;

      cout << "\tWriting canvases to " << ofilepath+ofilename << " ... ";
      ofile->cd();
      for(unsigned int s=0; s<sname.size(); s++) {
         cpfIso.back()->Write();
         cchargedHadronIso.back()->Write();
         cneutralHadronIso.back()->Write();
         cphotonIso.back()->Write();
         cmvaTrigV0.back()->Write();
         cmvaNonTrigV0.back()->Write();
         cpfIsoMVATrigV0.back()->Write();
      }
      cout << "DONE" << endl;

      /*******************************************************************************************/
      //Clear vectors
      /*******************************************************************************************/
      cpfIso.clear();
      cchargedHadronIso.clear();
      cneutralHadronIso.clear();
      cphotonIso.clear();
      cmvaTrigV0.clear();
      cmvaNonTrigV0.clear();
      cpfIsoMVATrigV0.clear();
      hpfIso.clear();
      hchargedHadronIso.clear();
      hneutralHadronIso.clear();
      hphotonIso.clear();
      hmvaTrigV0.clear();
      hmvaNonTrigV0.clear();
      hpfIsoMVATrigV0.clear();
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
      sname.clear();
   }

   return 0;
}
