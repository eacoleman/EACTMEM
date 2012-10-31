#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "THStack.h"
#include "TList.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TH1D.h"
#include "TH1.h"
#include "TLatex.h"

#include <map>
#include <vector>
#include <iostream>

using namespace std;

void makeBoth() {
   cout << "Open Files...";
   TFile* inf1 = TFile::Open("2012_10_28_WithCuts_NewEPD/outputFile_muon.root");
   TFile* inf2 = TFile::Open("test/outputFile_electron.root");
   TFile* outf = TFile::Open("test/outputFile_both.root","RECREATE");
   cout << "DONE" << endl;
   TString prefixEl =  "MVADiscriminator_electron";
   TString prefixMu =  "MVADiscriminator_muon";
   map<TString,TH1D*> mymap;

   inf1->cd();
   cout << "Get muon canvas...";
   TCanvas* testCanCopyMu = (TCanvas*)gDirectory->Get(prefixMu);
   TCanvas* canvasMu = (TCanvas*)testCanCopyMu->Clone();
   cout << "DONE" << endl;
   cout << "Get muon pad...";
   TPad* padMu = (TPad *) canvasMu->GetPrimitive(prefixMu + "_1");
   cout << "DONE" << endl;
   cout << "Get muon stacks...";
   THStack* mcStackMu = (THStack*) padMu->GetPrimitive(prefixMu + "_stackMC");
   THStack* dataStackMu = (THStack*) padMu->GetPrimitive(prefixMu + "_stackData");
   cout << "DONE" << endl;
   cout << "Get muon lists...";
   TList* mcListMu = mcStackMu->GetHists();
   //mcListMu->Print();
   TList* dataListMu = dataStackMu->GetHists();
   //dataListMu->Print();
   cout << "DONE" << endl;
   cout << "Get muon histograms...";
   mymap["DibosonMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_Diboson");
   mymap["STopMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_STop");
   mymap["WJetsMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_WJets_muon");
   mymap["DYJetsMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_DYJets");
   mymap["TTbarMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_TTbar");
   mymap["QCDMu"] = (TH1D*) mcListMu->FindObject(prefixMu + "_QCD_muonEnriched");
   mymap["DataMu"] = (TH1D*) dataListMu->FindObject(prefixMu + "_Data_muon");
   cout << "DONE" << endl;

   inf2->cd();
   cout << "Get electron canvas...";
   TCanvas* testCanCopyEl = (TCanvas*)gDirectory->Get(prefixEl);
   TCanvas* canvasEl = (TCanvas*)testCanCopyEl->Clone();
   cout << "DONE" << endl;
   cout << "Get electron pad...";
   TPad* padEl = (TPad *) canvasEl->GetPrimitive(prefixEl + "_1");
   cout << "DONE" << endl;
   cout << "Get electron stacks...";
   THStack* mcStackEl = (THStack*) padEl->GetPrimitive(prefixEl + "_stackMC");
   THStack* dataStackEl = (THStack*) padEl->GetPrimitive(prefixEl + "_stackData");
   cout << "DONE" << endl;
   cout << "Get electron lists...";
   TList* mcListEl = mcStackEl->GetHists();
   TList* dataListEl = dataStackEl->GetHists();
   //dataListEl->Print();
   cout << "DONE" << endl;
   cout << "Get electron histograms...";
   mymap["DibosonEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_Diboson");
   mymap["STopEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_STop");
   mymap["WJetsEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_WJets_electron");
   mymap["DYJetsEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_DYJets");
   mymap["TTbarEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_TTbar");
   mymap["QCDEl"] = (TH1D*) mcListEl->FindObject(prefixEl + "_QCD_electronEnriched");
   mymap["DataEl"] = (TH1D*) dataListEl->FindObject(prefixEl + "_Data_electron");
   cout << "DONE" << endl;

/*
   outf->cd();
   cout << "Make new histograms...";
   mymap["DibosonBoth"] = new TH1D("Diboson","Diboson",mymap["DibosonEl"]->GetNbinsX(),mymap["DibosonEl"]->GetXaxis()->GetXmin(),mymap["DibosonEl"]->GetXaxis()->GetXmax());
   mymap["STopBoth"] = new TH1D("STop","STop",mymap["STopEl"]->GetNbinsX(),mymap["STopEl"]->GetXaxis()->GetXmin(),mymap["STopEl"]->GetXaxis()->GetXmax());
   mymap["WJetsBoth"] = new TH1D("WJets","WJets",mymap["WJetsEl"]->GetNbinsX(),mymap["WJetsEl"]->GetXaxis()->GetXmin(),mymap["WJetsEl"]->GetXaxis()->GetXmax());
   mymap["DYJetsBoth"] = new TH1D("DYJets","DYJets",mymap["DYJetsEl"]->GetNbinsX(),mymap["DYJetsEl"]->GetXaxis()->GetXmin(),mymap["DYJetsEl"]->GetXaxis()->GetXmax());
   mymap["TTbarBoth"] = new TH1D("TTbar","TTbar",mymap["TTbarEl"]->GetNbinsX(),mymap["TTbarEl"]->GetXaxis()->GetXmin(),mymap["TTbarEl"]->GetXaxis()->GetXmax());
   mymap["QCDBoth"] = new TH1D("QCD","QCD",mymap["QCDEl"]->GetNbinsX(),mymap["QCDEl"]->GetXaxis()->GetXmin(),mymap["QCDEl"]->GetXaxis()->GetXmax());
   mymap["DataBoth"] = new TH1D("Data","Data",mymap["DataEl"]->GetNbinsX(),mymap["DataEl"]->GetXaxis()->GetXmin(),mymap["DataEl"]->GetXaxis()->GetXmax());
   cout << "DONE" << endl;

   mymap["DibosonBoth"]->Add(mymap["DibosonEl"]);
   mymap["DibosonBoth"]->Add(mymap["DibosonMu"]);
   mymap["STopBoth"]->Add(mymap["STopEl"]);
   mymap["STopBoth"]->Add(mymap["STopMu"]);
   mymap["WJetsBoth"]->Add(mymap["WJetsEl"]);
   mymap["WJetsBoth"]->Add(mymap["WJetsMu"]);
   mymap["DYJetsBoth"]->Add(mymap["DYJetsEl"]);
   mymap["DYJetsBoth"]->Add(mymap["DYJetsMu"]);
   mymap["TTbarBoth"]->Add(mymap["TTbarEl"]);
   mymap["TTbarBoth"]->Add(mymap["TTbarMu"]);
   mymap["QCDBoth"]->Add(mymap["QCDEl"]);
   mymap["QCDBoth"]->Add(mymap["QCDMu"]);
   mymap["DataBoth"]->Add(mymap["DataEl"]);
   mymap["DataBoth"]->Add(mymap["DataMu"]);


   mymap["DibosonBoth"]->Write();
   mymap["STopBoth"]->Write();
   mymap["WJetsBoth"]->Write();
   mymap["DYJetsBoth"]->Write();
   mymap["TTbarBoth"]->Write();
   mymap["QCDBoth"]->Write();
   mymap["DataBoth"]->Write();
*/
   cout << "Add histograms...";
   cout << "Diboson...";
   mymap["DibosonEl"]->Add(mymap["DibosonMu"]);
   cout << "STop...";
   mymap["STopEl"]->Add(mymap["STopMu"]);
   cout << "WJets...";
   mymap["WJetsEl"]->Add(mymap["WJetsMu"]);
   cout << "DYJets...";
   mymap["DYJetsEl"]->Add(mymap["DYJetsMu"]);
   cout << "TTbar...";
   mymap["TTbarEl"]->Add(mymap["TTbarMu"]);
   cout << "QCD...";
   mymap["QCDEl"]->Add(mymap["QCDMu"]);
   cout << "Data...";
   mymap["DataEl"]->Add(mymap["DataMu"]);
   cout << "DONE" << endl;

   mcStackEl->Modified();
   dataStackEl->Modified();

   outf->cd();
   canvasEl->Write();
}

void saveAfterFit(){
   cout << "Opening file...";
   TFile* inf = TFile::Open("test/Fitter_both_MVADiscriminator_modified.root");
   cout << "DONE" << endl;


   TString prefix =  "MVADiscriminator_electron";
   cout << "Getting canvas...";
   TCanvas* testCanCopy = (TCanvas*)gDirectory->Get(prefix);
   TCanvas* canvas = (TCanvas*)testCanCopy->Clone();
   cout << "DONE" << endl;
   cout << "Getting pads...";
   TPad* pad = (TPad *) canvas->GetPrimitive(prefix + "_1");
   TPad* pad2 = (TPad *) canvas->GetPrimitive(prefix + "_2");
   cout << "DONE" << endl;
   //pad->GetListOfPrimitives()->Print();
   cout << "Getting stacks...";
   THStack* mcStack = (THStack*) pad->GetPrimitive(prefix + "_stackMC");
   mcStack->SetTitle("");
   THStack* dataStack = (THStack*) pad->GetPrimitive(prefix + "_stackData");
   dataStack->SetTitle("");
   cout << "DONE" << endl;
   cout << "Getting lists...";
   TList* mcList = mcStack->GetHists();
   TList* dataList = dataStack->GetHists();
   cout << "DONE" << endl;

   cout << "Getting legend...";
   TLegend* leg = (TLegend*)pad->GetPrimitive("MVADiscriminator_electron_legend");
   cout << "DONE" << endl;
/*
   cout << "Getting legend entries...";
   TLegendEntry* WJets = (TLegendEntry*)pad->GetPrimitive("MVADiscriminator_electron_WJets_electron");
   TLegendEntry* QCD = (TLegendEntry*)pad->GetPrimitive("MVADiscriminator_electron_QCD_electronEnriched");
   TLegendEntry* Data = (TLegendEntry*)pad->GetPrimitive("MVADiscriminator_electron_Data_electron");
   cout << "DONE" << endl;

   cout << "Setting legend entries..." << WJets << "\t" << QCD << "\t" << Data;
   WJets->SetLabel("WJets");
   QCD->SetLabel("QCD");
   Data->SetLabel("Data");
   cout << "DONE" << endl;
*/
   cout << "Setting maximums and minimums...";
   dataStack->SetMaximum(4500);
   dataStack->SetMinimum(0);
   mcStack->SetMaximum(4500);
   mcStack->SetMinimum(0);
   cout << "DONE" << endl;

   map<TString,TH1D*> mymap;
   mymap["Diboson"] = (TH1D*) mcList->FindObject(prefix + "_Diboson");
   mymap["STop"] = (TH1D*) mcList->FindObject(prefix + "_STop");
   mymap["WJets"] = (TH1D*) mcList->FindObject(prefix + "_WJets_electron");
   mymap["DYJets"] = (TH1D*) mcList->FindObject(prefix + "_DYJets");
   mymap["TTbar"] = (TH1D*) mcList->FindObject(prefix + "_TTbar");
   mymap["QCD"] = (TH1D*) mcList->FindObject(prefix + "_QCD_electronEnriched");
   mymap["Data"] = (TH1D*) dataList->FindObject(prefix + "_Data_electron");
   TH1D* mcSum = new TH1D("sum","sum",mymap["Diboson"]->GetNbinsX(),mymap["Diboson"]->GetXaxis()->GetXmin(),mymap["Diboson"]->GetXaxis()->GetXmax());
   mcSum->Add(mymap["Diboson"]);
   mcSum->Add(mymap["STop"]);
   mcSum->Add(mymap["WJets"]);
   mcSum->Add(mymap["DYJets"]);
   mcSum->Add(mymap["TTbar"]);
   mcSum->Add(mymap["QCD"]);
   double chi2;
   int NDF;
   int igood;

   cout << "Reseting TLatex...";
   pad->cd();
   TH1D* data = (TH1D*) dataList->FindObject(prefix + "_Data_electron");
   double y = data->GetMaximum()*1.05;

   TLatex* ks = (TLatex*)pad->GetPrimitive("ks");
   double x = ks->GetX();
   //ks->SetY(y);
   ks->Delete();
   TLatex* newKS = new TLatex(x, y     , Form("KSTest   = %5.4g", mymap["Data"]->KolmogorovTest(mcSum)));
   newKS->SetName("ks");
   newKS->Draw();

   TLatex* chi2P = (TLatex*)pad->GetPrimitive("chi2P");
   //chi2P->SetY(y*0.92);
   chi2P->Delete();
   TLatex* newChi2P = new TLatex(x, y*0.92, Form("Chi2Prob = %5.4g", mcSum->Chi2TestX(mymap["Data"],chi2,NDF,igood,"WW")));
   newChi2P->SetName("chi2P");
   newChi2P->Draw();

   TLatex* chi2NDF = (TLatex*)pad->GetPrimitive("chi2NDF");
   //chi2NDF->SetY(y*0.84);
   chi2NDF->Delete();
   TLatex* newChi2NDF = new TLatex(x, y*0.84, Form("Chi2/NDF = %5.4g", chi2/NDF));
   newChi2NDF->SetName("chi2NDF");
   newChi2NDF->Draw();
   cout << "DONE" << endl;

   cout << "Getting ratio hist...";
   pad2->cd();
   TH1D* ratio = (TH1D*)pad2->GetPrimitive("MVADiscriminator_electron_Ratio");
   ratio->SetTitle("");
   cout << "DONE" << endl;
   cout << "Reseting ratio...";
   TH1* hRatio = (TH1*)mymap["Data"]->Clone("blah");
   hRatio->SetTitle("");
   hRatio->Add(mcSum,-1);
   hRatio->Divide(mcSum);
   hRatio->SetMinimum(-0.99);
   hRatio->SetMaximum(0.99);
   hRatio->SetLineWidth(2);

   hRatio->GetXaxis()->SetLabelFont(42);
   hRatio->GetXaxis()->SetLabelOffset(0.007);
   hRatio->GetXaxis()->SetLabelSize(0.11);
   hRatio->GetXaxis()->SetTitleSize(0.12);
   hRatio->GetXaxis()->SetTitleFont(42);
   hRatio->GetXaxis()->SetRangeUser(-0.05,0.65);

   hRatio->GetYaxis()->SetTitle("#frac{Data - MC}{MC}");
   hRatio->GetYaxis()->SetTitleOffset(0.55);
   hRatio->GetYaxis()->SetNdivisions(105);
   hRatio->GetYaxis()->SetLabelFont(42);
   hRatio->GetYaxis()->SetLabelOffset(0.012);
   hRatio->GetYaxis()->SetLabelSize(0.11);
   hRatio->GetYaxis()->SetTitleSize(0.1);
   hRatio->GetYaxis()->SetTitleFont(42);

   hRatio->SetStats(0);
   ratio->Delete();
   hRatio->Draw();
   cout << "DONE" << endl;

   cout << "Drawing canvas...";
   canvas->Draw();
   cout << "DONE" << endl;

   cout << "Saving canvas...";
   canvas->SaveAs("test/Fitter_both_MVADiscriminator.eps");
   canvas->SaveAs("test/Fitter_both_MVADiscriminator.png");
   cout << "DONE" << endl;
}
