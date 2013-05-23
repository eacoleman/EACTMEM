//Our libraries
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
//#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"

// ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TError.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

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
int main(int argc,char**argv) {
 
  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString ifilename  = cl.getValue<TString> ("ifile");
  TString ofilename  = cl.getValue<TString> ("ofile", "METPhiCorrections.root");
  TString foldername = cl.getValue<TString> ("foldername",                "PS");
  TString treeName   = cl.getValue<TString> ("treeName",              "jets2p");
  int     evtMax     = cl.getValue<int>     ("evtMax",                       0);

  if (!cl.check()) return 0;
  cl.print();

  TFile* ifile = new TFile(ifilename,"READ");
   if (!ifile->IsOpen())
   {
      cout << "ERROR::Could not open file " << ifilename << endl;
      return 0;
   }

   if (!ifile->cd(foldername))
   {
      cout << "ERROR::Could not CD into directory " << foldername << " in file " << ifilename << endl;
      return 0;
   }
  
   TTree* t = (TTree*) gDirectory->Get(treeName);
   if (t == 0)
   {
      cout << "ERROR::Could not find tree named " << treeName << " in file " << ifilename << endl;
      return 0;
   }

   TFile* ofile = new TFile(ofilename,"RECREATE");

   EventNtuple * ntuple = new EventNtuple();
   if (t->GetBranch("EvtTree")) {
      t->SetBranchAddress("EvtTree",&ntuple);
   }
   else if (t->GetBranch("EvtNtuple")) {
      t->SetBranchAddress("EvtNtuple",&ntuple);
   }
   else {
      cout << "ERROR::EvtTree and EvtNtuple branches not found." << endl
           << "\tProgram will now terminate." << endl;
      return 0;
   }

   map<pair<double,double>,pair<TH2D*,TF2*> > PxPy_NPVBins;
   for(int i=0; i<30; i++) {
      PxPy_NPVBins[make_pair(double(i),double(i))] = make_pair(new TH2D(Form("PxPy_NPV%.0fto%0.f",double(i),double(i)),Form("PxPy_NPV%.0fto%.0f",double(i),double(i)),400,-1000,1000,400,-1000,1000),
                                                               new TF2(Form("fit_NPV%.0fto%0.f",double(i),double(i)),"xygaus",-100,100,-100,100));
//                                                               new TF2(Form("fit_NPV%.0fto%0.f",double(i),double(i)),"[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",-60,60,-60,60));
      PxPy_NPVBins[make_pair(double(i),double(i))].second->SetParameters(46.0,0.0,43.0,0.0,43.0);
      PxPy_NPVBins[make_pair(double(i),double(i))].second->SetNpx(300);
      PxPy_NPVBins[make_pair(double(i),double(i))].second->SetNpy(300);
   }
   for(int i=30; i<100; i+=5) {
      PxPy_NPVBins[make_pair(double(i),double(i+4))] = make_pair(new TH2D(Form("PxPy_NPV%.0fto%0.f",double(i),double(i+4)),Form("PxPy_NPV%.0fto%.0f",double(i),double(i+4)),400,-1000,1000,400,-1000,1000),
                                                                 new TF2(Form("fit_NPV%.0fto%0.f",double(i),double(i+4)),"xygaus",-100,100,-100,100));
//                                                                 new TF2(Form("fit_NPV%.0fto%0.f",double(i),double(i+4)),"[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",-60,60,-60,60));
      PxPy_NPVBins[make_pair(double(i),double(i+4))].second->SetParameters(46.0,0.0,43.0,0.0,43.0);  
      PxPy_NPVBins[make_pair(double(i),double(i+4))].second->SetNpx(300);
      PxPy_NPVBins[make_pair(double(i),double(i+4))].second->SetNpy(300);
   }

   for(unsigned int j=0; j<t->GetEntries(); j++){

      if(evtMax>0 && int(j)>evtMax) continue;

      t->GetEntry(j);

      if ((j % 10000) == 0)
         cout<<"\t\tevent "<<j<<endl;

      //if(PxPy_NPVBins.find(ntuple->vLV[0].npv)==PxPy_NPVBins.end()) {
      //   PxPy_NPVBins[ntuple->vLV[0].npv] = make_pair(new TH2D(Form("PxPy_NPV%.0f",ntuple->vLV[0].npv),Form("PxPy_NPV%.0f",ntuple->vLV[0].npv),400,-1000,1000,400,-1000,1000),
      //                                                new TF2(Form("fit_NPV%.0f",ntuple->vLV[0].npv),"[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",-500,500,500,500));
      //     PxPy_NPVBins[ntuple->vLV[0].npv].second->SetParameters(1.0,0,20,0,20);
      //}

      if(ntuple->vLV[0].npv<30) {
         PxPy_NPVBins[make_pair(ntuple->vLV[0].npv,ntuple->vLV[0].npv)].first->Fill(ntuple->METLV[0].Px(),ntuple->METLV[0].Py());
      }
      else {
         double first  = ntuple->vLV[0].npv-(int(ntuple->vLV[0].npv)%5);
         double second = first+4;
         PxPy_NPVBins[make_pair(first,second)].first->Fill(ntuple->METLV[0].Px(),ntuple->METLV[0].Py());
      }
   }

   TGraphErrors* Px_NPV = new TGraphErrors();   
   Px_NPV->SetName("Px_NPV");
   Px_NPV->SetMarkerStyle(20);
   Px_NPV->SetMarkerSize(0.5);
   TGraphErrors* Py_NPV = new TGraphErrors(); 
   Py_NPV->SetName("Py_NPV");
   Py_NPV->SetMarkerStyle(20);
   Py_NPV->SetMarkerSize(0.5);

   int counter = 0;
   for(map<pair<double,double>,pair<TH2D*,TF2*> >::iterator it= PxPy_NPVBins.begin(); it!= PxPy_NPVBins.end(); ++it) {
      cout << "Doing NPV range (" << it->first.first << "," << it->first.second << ")" << endl;
      
      ofile->mkdir(Form("NPV%.0fto%0.f",it->first.first,it->first.second));
      ofile->cd(Form("NPV%.0fto%0.f",it->first.first,it->first.second));

      TFitResultPtr r = it->second.first->Fit(it->second.second,"R S");
      Int_t result = r;

      if(result!=0) {
         cout << "\tERROR::Fit " << counter << " failed. Program will now terminate." << endl;
         //return 0;
      }
      else {
         double x = (it->first.first+it->first.second)/2.0;
         if(x<0)
            cout << "\t\tERROR::NPV is negative!!!" << endl;
         Px_NPV->SetPoint(counter,x,it->second.second->GetParameter(1));
         Px_NPV->SetPointError(counter,0,it->second.second->GetParError(1));
         Py_NPV->SetPoint(counter,x,it->second.second->GetParameter(3));
         Py_NPV->SetPointError(counter,0,it->second.second->GetParError(3));
      }
      counter++;

      it->second.first->GetXaxis()->SetTitle("P_{x}");
      it->second.first->GetYaxis()->SetTitle("P_{y}");
      
      it->second.first->Write();
      it->second.second->Write();
      ofile->cd();
   }

   Px_NPV->GetXaxis()->SetTitle("NPV");
   Px_NPV->GetYaxis()->SetTitle("Mean_{x}");
   Px_NPV->Fit("pol1","R S+","",2,100);
   TF1* Px_NPV_Fit = (TF1*)Px_NPV->GetFunction("pol1");
   Px_NPV_Fit->SetName("Px_NPV_Fit");
   Px_NPV_Fit->Write();
   Px_NPV->Write();

   Py_NPV->GetXaxis()->SetTitle("NPV");
   Py_NPV->GetYaxis()->SetTitle("Mean_{y}");
   Py_NPV->Fit("pol1","R S+","",2,100);
   TF1* Py_NPV_Fit = (TF1*)Py_NPV->GetFunction("pol1");
   Py_NPV_Fit->SetName("Py_NPV_Fit");
   Py_NPV_Fit->Write();
   Py_NPV->Write();

   //ofile->Write();
   ofile->Close();

   return 0;
}//main
