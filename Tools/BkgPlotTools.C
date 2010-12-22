#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TString.h>
#include <TH1.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
using namespace std;

void BkgPlotTools () {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////*********Helper Functions*********//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void readModes(const char* modelinein, vector<TString>& modename, vector<TString>& infilename, vector<double>& evtcount, vector<int>& color, int& nModes) {
//// Takes modelinein, which should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color [2] modename[2] ... ", and extracts these parameters as well as the number of modes
  nModes=0;
  modename.clear();
  infilename.clear();
  evtcount.clear();
  color.clear();
  TString t_modename, t_infilename;
  double t_evtcount;
  int t_color;
  istrstream inModes(modelinein);
  bool cont=true;

  // input the names, file locations & event counts
  while ( cont==true ) {
    inModes >> t_modename;
    if ( t_modename!="" ) {
      inModes >> t_infilename >> t_evtcount >> t_color;
      modename.push_back(t_modename);
      infilename.push_back(t_infilename);
      evtcount.push_back(t_evtcount);
      color.push_back(t_color);
      nModes++;
    } else {
      cont=false;
    }
    if ( nModes>100 ) {
      cont=false;
      cout << "Error: made 100 iterations and unable to read in the parameters for " << modelinein << endl;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void fillHistograms(TH1F* h[20], const char* TreeName, const char* VarName, double VarMin, double VarMax, const char* AddRest, int nModes, vector<TString>& infilenames)
////Fill in each histogram in the array h from the files in infilenames.
{
  
  TString Restrictions, hname, histload_str;
  char Max_char[30], Min_char[30], n_char[5];

  /// Make a string defining the restrictions (Range + Additional Ones).
  
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }
  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;
  cout << "Restrictions=" << Restrictions << endl;
  
  //Open the files
  TFile* f[20];
  TTree* InTree[20];
  for (Int_t n=0; n<nModes; n++) {
    f[n] = new TFile(infilenames[n]);
    InTree[n] = (TTree*)f[n]->Get(TreeName);
  }
  
  //Fill the histograms
  histload_str=">>htemp";
  histload_str=VarName+histload_str;
  cout << "hist input =" << histload_str << endl;
  for (Int_t n=0; n<nModes; n++) {
    sprintf(n_char,"%i",n);
    hname="]";
    hname = n_char + hname;
    hname="h["+hname;
    cout << "hname=" << hname << endl;
    
    TH1F* htemp = new TH1F("htemp","htemp",25,VarMin,VarMax);
    InTree[n]->Draw(histload_str,Restrictions);
    h[n] = (TH1F*)htemp->Clone();
    h[n]->SetName(hname);
    h[n]->SetTitle(hname);
//     cout << "htemp Entries = " << htemp->GetEntries() << endl;
//     cout << "h[" << n << "] Entries = " << h[n]->GetEntries() << endl;  
    
    delete htemp;
  }
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*********Plot Functions*********///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////





// void Plot_AllBackgrounds(const char* VarName, double VarMin, double VarMax, const char* AddRest = 0, const char* inAllModes = 0) 
// //// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
// /// inAllModes should be of the form "modename[0] infilename[0] evtCount[0] modename[1] infilename[1] evtCount[1] modename[2] ... "
// /// Use AddRest to give additional restrictions
// {
//   vector<TString> modename;
//   vector<TString> infilename;
//   vector<double> evtcount;
//   vector<int> color;
//   int nMode = 0;
//   readModes(inAllModes,modename,infilename,evtcount,color,nMode);

//   //add the additional restrictions if any.
//   char Max_char[30];
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   char Min_char[30];
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;

//   TString Restrictions;
//   if (AddRest!="") {
//     Restrictions = AddRest;
//     Restrictions = ")&&"+Restrictions;
//   } else {
//     Restrictions = ")";
//   }
//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;

//   cout << "Restrictions=" << Restrictions << endl;

//   //create the histograms
//   TH1F* h[20];
//   TH1F* ha[20];
  
//   TString hname;
//   TString haname;
//   int nentries;
//   double scale;
//   double entries;
//   char n_char[30];

//   TString input;
//   input=")";
//   input=Max_char+input;
//   input=","+input;
//   input=Min_char+input;
//   input=">>ht(25,"+input;
//   input=VarName+input;
//   cout << "input=" << input << endl;

//   for (Int_t n=0; n<nMode; n++) {
//     sprintf(n_char,"%i",n);
//     hname="]";
//     hname = n_char + hname;
//     haname="ha["+hname;
//     hname="h["+hname;
//     h[n]=new TH1F(hname,hname,25,VarMin,VarMax);
//     ha[n]=new TH1F(haname,haname,25,VarMin,VarMax);
//     TFile* f = new TFile(infilename[n]);
//     TTree* EvtTree = (TTree*)f->Get("EvtTree");
//     EvtTree->Draw(input,Restrictions);
//     cout << "testing1" << endl;
//     h[n]=ht;
//     cout << "testing2" << endl;
//     entries=h[n]->GetEntries();
//     cout << modename[n] <<": entries=" << entries << endl;
//     scale=evtcount[n]/entries;
//     cout << "scale=" << scale << endl;
//     h[n]->Scale(scale);
//     for (Int_t m=0; m<(n+1); m++) {
//       ha[n]->Add(h[m]);
//     }
//     ha[n]->SetFillColor(color[n]);
    
//  }

//   cout << "nModes=" << nMode << endl;
//   int k;
//   TLegend * legend;
//   legend = new TLegend(0.70,0.5,0.9,0.9);
//   for (Int_t n=0; n<nMode; n++) {
//     k=nMode-n-1;
//     if ( n==0 ) {
//       ha[k]->Draw();
//     } else {
//       ha[k]->Draw("same");
//     }
//     legend->AddEntry(ha[k],modename[k],"f");
//   }
//   gStyle->SetOptStat(0);
//   ha[nMode-1]->SetTitle("Expected Yields per pb^{-1}");
//   ha[nMode-1]->SetXTitle(VarName);
//   ha[nMode-1]->SetYTitle("Evts/pb^{-1}");

//   legend->Draw();

// }

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_ABkg(const char* TreeName, const char* VarName, double VarMin, double VarMax, const char* inmodefile, const char* mode_name=0, const char* AddRest=0)
//// Plots variable VarName for a particular mode.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

  TH1D* hist = new TH1D("hist","hist",25,VarMin,VarMax);


  TString input=">>hist";
  input=VarName+input;

  // input the ranges & additonal restrictions
  char Max_char[30];
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  char Min_char[30];
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  TString Restrictions;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }


  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;

  cout << "Restrictions=" << Restrictions << endl;
  InTree->Draw(input,Restrictions);

  hist->SetLineWidth(2);
  //hist->SetLineColor(2);
  hist->Draw();

  TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
  lgnd->AddEntry(hist,mode_name,"l");
  gStyle->SetOptStat(0);
  hist->SetXTitle(VarName);
  hist->SetYTitle("Events");
  TString Title;
  Title="-Distribution";
  Title=VarName+Title;
  Title="  "+Title;

  hist->SetTitle(Title);
  lgnd->Draw();


}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_CompareShapes(const char* TitleName, const char* TreeName, const char* VarName, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0)
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
{

  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  double maxtemp=0;
  double max=0;
  double scale;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  //Define the histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  //  TLegend *lgnd = new TLegend(0.7,0.75,0.90,0.9);
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }
  double entries;

  fillHistograms(h,TreeName,VarName,VarMin,VarMax,AddRest,nModes,infilenames);


  for (Int_t n=0; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    //    evts=evts+evtcounts[n];
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    h[n]->SetLineWidth(2);
    h[n]->SetLineColor(colors[n]);
    lgnd->AddEntry(h[n],modenames[n],"l");
    //Make sure the maximum is set to include all events.
    maxtemp=h[n]->GetMaximum();
    if ( maxtemp > max ){
      max=maxtemp;
    }

  }

//   entries = h[0]->GetEntries();
//   h[0]->Draw();
//   cout <<"entries=" << entries << endl;

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  h[0]->SetMinimum(0);
  h[0]->SetMaximum(max*1.02);

  h[0]->Draw();
  for (Int_t n=1; n<nModes; n++) {
    h[n]->Draw("same");
  }
  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  h[0]->SetYTitle("Event Count");
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


// void Plot_TwoBkgTypes(const char* XTitle, const char* VarName, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes1, const char* inModes2, int scalingType, const char* SaveName = 0) 
// //// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
// /// inAllModes should be of the form "modename[0] infilename[0] evtCount[0] modename[1] infilename[1] evtCount[1] modename[2] ... "
// /// Use AddRest to give additional restrictions
// /// scalingType=0: preserve the given scaling. scalingType=1: same scale for both histograms.
// {
//   vector<TString> modenames1, infilenames1, modenames2, infilenames2;
//   vector<double> evtcounts1, evtcounts2;
//   vector<int> colors1, colors2;
//   int nModes1 = 0;
//   int nModes2 = 0;
//   double evts1=0;
//   double evts2=0;
//   double max1, max2;
//   readModes(inModes1,modenames1,infilenames1,evtcounts1,colors1,nModes1);
//   readModes(inModes2,modenames2,infilenames2,evtcounts2,colors2,nModes2);

//   //Define the range for VarName and add the additional restrictions if any.
//   char Max_char[30];
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   char Min_char[30];
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;


//   TString Restrictions;
//   if (AddRest!="") {
//     Restrictions = AddRest;
//     Restrictions = ")&&"+Restrictions;
//   } else {
//     Restrictions = ")";
//   }
//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;

//   cout << "Restrictions=" << Restrictions << endl;


//   //create the histograms
//   TH1F* h1[20];
//   TH1F* h2[20];
//   //  TH1F* htemp;

//   TH1F* hTotal1;
//   TH1F* hTotal2;
//   hTotal1=new TH1F("hTotal1","hTotal1",25,VarMin,VarMax);
//   hTotal2=new TH1F("hTotal2","hTotal2",25,VarMin,VarMax);

//   TString hname;
//   TString labelstr1, labelstr2;
//   double scale;
//   double entries;
//   char n_char[30];

//   TString input;
//   input=")";
//   input=Max_char+input;
//   input=","+input;
//   input=Min_char+input;
//   input=">>ht(25,"+input;
//   input=VarName+input;

//   cout << "input=" << input << endl;

//   /// Combine the first set of modes.
//   for (Int_t n=0; n<nModes1; n++) {
//     sprintf(n_char,"%i",n);
//     hname="]";
//     hname = n_char + hname;
//     hname="h["+hname;
//      cout << "testing0" << endl;
//     //ht=new TH1F("ht","ht",25,VarMin,VarMax);
//     h1[n]=new TH1F(hname,hname,25,VarMin,VarMax);
//     TFile* f = new TFile(infilenames1[n]);
//     TTree* EvtTree = (TTree*)f->Get("EvtTree");
//     EvtTree->Draw(input,Restrictions);

//      cout << "testing1" << endl;

// //     htemp=ht;

//      cout << "testing2" << endl;

//     h1[n]=ht;

// //     cout << "testing3" << endl;

//     entries=h1[n]->GetEntries();
//     cout << modenames1[n] <<": entries=" << entries << endl;
//     scale=evtcounts1[n]/entries;
//     evts1=evts1+evtcounts1[n];
//     cout << "scale=" << scale << endl;
//     h1[n]->Scale(scale);
//     //hTotal1->Add(h1[m])
//   }
//   for (Int_t m=0; m<nModes1; m++) {
//     hTotal1->Add(h1[m]);
//   }

//   cout << "nModes1=" << nModes1 << endl;
//   // create the label
//   TString labelstr1;
//   labelstr1="";
//   for (Int_t n=0; n<nModes1; n++) {
//     labelstr1 =  modenames1[nModes1-n-1] + labelstr1;
//     if ( n!=(nModes1-1) ) {
//       labelstr1 = "&" + labelstr1;
//     }
//   }

//   /// Combine the second set of modes.
//   for (Int_t n=0; n<nModes2; n++) {
//     sprintf(n_char,"%i",n);
//     hname="]";
//     hname = n_char + hname;
//     hname="h["+hname;
//     h2[n]=new TH1F(hname,hname,25,VarMin,VarMax);
//     TFile* f = new TFile(infilenames2[n]);
//     TTree* EvtTree = (TTree*)f->Get("EvtTree");
//     EvtTree->Draw(input,Restrictions);
//     h2[n]=ht;
//     entries=h2[n]->GetEntries();
//     cout << modenames2[n] <<": entries=" << entries << endl;
//     scale=evtcounts2[n]/entries;
//     evts2=evts2+evtcounts2[n];
//     cout << "scale=" << scale << endl;
//     h2[n]->Scale(scale);
//   }
//   for (Int_t m=0; m<nModes2; m++) {
//     hTotal2->Add(h2[m]);
//   }

//   cout << "nModes2=" << nModes2 << endl;
//   // create the label
//   TString labelstr2;
//   labelstr2="";
//   for (Int_t n=0; n<nModes2; n++) {
//     labelstr2 =  modenames2[nModes2-n-1] + labelstr2;
//     if ( n!=(nModes2-1) ) {
//       labelstr2 = "&" + labelstr2;
//     }
//   }
  


//   /// Draw the output
//   if ( scalingType==1 ) {
//     scale=1/evts1;
//     hTotal1->Scale(scale);
//     scale=1/evts2;
//     hTotal2->Scale(scale);
//   }
//   TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
//   gStyle->SetOptStat(0);
//   gStyle->SetTitleX(0.13);
//   gStyle->SetTitleY(0.97);
//   hTotal1->SetFillStyle(3254);
//   hTotal1->SetFillColor(kRed);
//   hTotal1->SetMinimum(0);
//   //Make sure the maximum is set to include all of the signal.
//   max1=hTotal1->GetMaximum();
//   max2=hTotal2->GetMaximum();
//   if ( max1<max2 ) {
//     hTotal1->SetMaximum(max2*1.02);
//   }
//   hTotal1->Draw();
//   hTotal2->SetFillStyle(3245);
//   hTotal2->SetFillColor(kBlue+2);
//   hTotal2->SetMinimum(0);
//   hTotal2->Draw("same");

//   legend = new TLegend(0.45,0.9,0.9,1.0);
//   legend->AddEntry(hTotal1,labelstr1,"f");
//   legend->AddEntry(hTotal2,labelstr2,"f");

//   hTotal1->SetXTitle(VarName);
//   if ( scalingType==0 ) {
//     hTotal1->SetTitle("Expected Yields per pb^{-1}");
//     hTotal1->SetXTitle(XTitle);
//     hTotal1->SetYTitle("Evts/pb^{-1}");
//   }
//   if ( scalingType==1 ) {
//     hTotal1->SetTitle("Normalized Yields");
//     hTotal1->SetXTitle(XTitle);
//     hTotal1->SetYTitle("Normalized Evts");
//   }

//   legend->Draw();
//   /// Save the output
//   if ( SaveName!=0 ) {
//     cnv->SaveAs(SaveName);
//   }

// }


// void Plot_SigLineVsBkg(const char* XTitle, const char* VarName, double VarMin, double VarMax, const char* AddRest = 0, const char* inModesSig, const char* inModesBkg, const char* SaveName = 0) 
// //// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
// /// inAllModes should be of the form "modename[0] infilename[0] evtCount[0] modename[1] infilename[1] evtCount[1] modename[2] ... "
// /// Use AddRest to give additional restrictions
// /// scalingType=0: preserve the given scaling. scalingType=1: same scale for both histograms.
// {
//   vector<TString> modenamesSig, infilenamesSig, modenamesBkg, infilenamesBkg;
//   vector<double> evtcountsSig, evtcountsBkg;
//   vector<int> colorsSig, colorsBkg;
//   int nModesSig = 0;
//   int nModesBkg = 0;
//   double evtsSig = 0;
//   double evtsBkg = 0;
//   double maxSig, maxBkg;
//   readModes(inModesSig,modenamesSig,infilenamesSig,evtcountsSig,colorsSig,nModesSig);
//   readModes(inModesBkg,modenamesBkg,infilenamesBkg,evtcountsBkg,colorsBkg,nModesBkg);

//   //Define the range for VarName and add the additional restrictions if any.
//   char Max_char[30];
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   char Min_char[30];
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;


//   TString Restrictions;
//   if (AddRest!="") {
//     Restrictions = AddRest;
//     Restrictions = ")&&"+Restrictions;
//   } else {
//     Restrictions = ")";
//   }
//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;

//   cout << "Restrictions=" << Restrictions << endl;


//   //create the histograms
//   TH1F* hSig[20];
//   TH1F* hBkg[20];
//   TH1F* haBkg[20];
//   //  TH1F* htemp;

//   TH1F* hTotalSig;
//   //  TH1F* hTotalBkg;
//   hTotalSig=new TH1F("hTotalSig","hTotalSig",25,VarMin,VarMax);
//   //  hTotalBkg=new TH1F("hTotalBkg","hTotalBkg",25,VarMin,VarMax);

//   TString hname;
//   TString labelstr1, labelstr2;
//   double scale;
//   double entries;
//   char n_char[30];

//   TString input;
//   input=")";
//   input=Max_char+input;
//   input=","+input;
//   input=Min_char+input;
//   input=">>ht(25,"+input;
//   input=VarName+input;

//   cout << "input=" << input << endl;

//   /// Combine the Signal modes.
//   for (Int_t n=0; n<nModesSig; n++) {
//     sprintf(n_char,"%i",n);
//     hname="]";
//     hname = n_char + hname;
//     hname="hSig["+hname;
//     //  cout << "testing0" << endl;
//     //ht=new TH1F("ht","ht",25,VarMin,VarMax);
//     hSig[n]=new TH1F(hname,hname,25,VarMin,VarMax);
//     TFile* f = new TFile(infilenamesSig[n]);
//     TTree* EvtTree = (TTree*)f->Get("EvtTree");
//     EvtTree->Draw(input,Restrictions);

//     cout << "testing1" << endl;

// //     htemp=ht;

//     cout << "testing2" << endl;

//     hSig[n]=ht;

//     cout << "testing3" << endl;

//     entries=hSig[n]->GetEntries();
//     cout << modenamesSig[n] <<": entries=" << entries << endl;
//     scale=evtcountsSig[n]/entries;
//     evtsSig=evtsSig+evtcountsSig[n];
//     cout << "scale=" << scale << endl;
//     hSig[n]->Scale(scale);
//     //hTotalSig->Add(hSig[m])
//   }
//   for (Int_t m=0; m<nModesSig; m++) {
//     hTotalSig->Add(hSig[m]);
//   }
//   scale=1/evtsSig;
//   hTotalSig->Scale(scale);
//   maxSig=hTotalSig->GetMaximum();
//   cout << "nModesSig=" << nModesSig << endl;
//   /// create the Signal label
//   TLegend * legend;
//   legend = new TLegend(0.70,0.5,0.9,0.9);
//   TString labelstr1;
//   labelstr1="";
//   for (Int_t n=0; n<nModesSig; n++) {
//     labelstr1 =  modenamesSig[nModesSig-n-1] + labelstr1;
//     if ( n!=(nModesSig-1) ) {
//       labelstr1 = "&" + labelstr1;
//     }
//   }
//   legend->AddEntry(hTotalSig,labelstr1,"l");




//   /// Combine the Background modes.
  
//   for (Int_t n=0; n<nModesBkg; n++) {
//     sprintf(n_char,"%i",n);
//     hname="]";
//     hname = n_char + hname;
//     haname="haBkg["+hname;
//     hname="hBkg["+hname;
//     hBkg[n]=new TH1F(hname,hname,25,VarMin,VarMax);
//     haBkg[n]=new TH1F(haname,haname,25,VarMin,VarMax);
//     TFile* f = new TFile(infilenamesBkg[n]);
//     TTree* EvtTree = (TTree*)f->Get("EvtTree");
//     EvtTree->Draw(input,Restrictions);
//     hBkg[n]=ht;
//     entries=hBkg[n]->GetEntries();
//     cout << modenamesBkg[n] <<": entries=" << entries << endl;
//     scale=evtcountsBkg[n]/entries;
//     evtsBkg=evtsBkg+evtcountsBkg[n];
//     cout << "scale=" << scale << endl;
//     hBkg[n]->Scale(scale);
//     for (Int_t m=0; m<(n+1); m++) {
//       haBkg[n]->Add(hBkg[m]);
//     }
//     haBkg[n]->SetFillColor(colorsBkg[n]);
    
//   }

//   scale=1/evtsBkg;
//   TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
//   cout << "nModesBkg=" << nModesBkg << endl;
//   int k;
//   for (Int_t n=0; n<nModesBkg; n++) {
//     k=nModesBkg-n-1;
//     haBkg[k]->Scale(scale);
//     if ( n==0 ) {
//       haBkg[k]->SetMinimum(0);
//       //Make sure the maximum is set to include all of the signal.
//       maxBkg=haBkg[k]->GetMaximum();
//       if ( maxBkg<maxSig ) {
// 	haBkg[k]->SetMaximum(maxSig*1.02);
//       }

//       haBkg[k]->Draw();
//     } else {
//       haBkg[k]->Draw("same");
//     }
//     legend->AddEntry(haBkg[k],modenamesBkg[k],"f");
//   }
//   gStyle->SetOptStat(0);
//   haBkg[nModesBkg-1]->SetTitle("Normalized Yields");
//   haBkg[nModesBkg-1]->SetXTitle(XTitle);
//   haBkg[nModesBkg-1]->SetYTitle("Normalized Evts");


//   hTotalSig->SetLineWidth(3);
//   hTotalSig->Draw("lsame");

//   gStyle->SetTitleX(0.32);
//   gStyle->SetTitleY(0.96);

//   legend->Draw();

//   /// Save the output
//   if ( SaveName!=0 ) {
//     cnv->SaveAs(SaveName);
//   }

// }



// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_StoreHist(const char* TreeName, const char* VarName, double VarMin, double VarMax, const char* inmodefile=0, const char* mode_name=0, const char* AddRest=0, const char* StoreHistFileName=0 )
//// Plots a histogram (of VarName) to be used by MCLimit.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

 
  TFile * outhistfile;
  if ( StoreHistFileName!=0) {
    outhistfile = new TFile(StoreHistFileName, "RECREATE");
  } 
  TH1F* EvtHist = new TH1F("EvtHist","EvtHist",20,VarMin,VarMax);

  TString input=">>EvtHist";
  input=VarName+input;

  // input the ranges & additonal restrictions
  char Max_char[30];
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  char Min_char[30];
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  TString Restrictions;
  if (AddRest!=0) {
    Restrictions = AddRest;
    Restrictions = ")&&"+Restrictions;
  } else {
    Restrictions = ")";
  }


  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;

  cout << "Restrictions=" << Restrictions << endl;
  InTree->Draw(input,Restrictions);

  double ent1 = EvtHist->GetEntries();
  cout << "ent1=" << ent1 << endl;

  double scale;

  EvtHist->SetLineWidth(2);
  EvtHist->SetLineColor(2);
  scale=1/ent1;
  EvtHist->Scale(scale);
  EvtHist->Draw();

  TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
  lgnd->AddEntry(EvtHist,mode_name,"l");
  //lgnd->SetHeader("The Legend Title");
  gStyle->SetOptStat(0);
  EvtHist->SetXTitle(VarName);
  EvtHist->SetYTitle("Normalized Events");
  TString Title;
  Title="-Distribution";
  Title=VarName+Title;
  Title="  "+Title;
//   Title=mode2_name+Title;
//   Title=" vs. "+Title;
  Title=mode_name+Title;

  EvtHist->SetTitle(Title);
  lgnd->Draw();
  if (StoreHistFileName!=0) {
   outhistfile->Write();
  }
  EvtHist->Draw();
  lgnd->Draw();
  //outhistfile->Close();


}



// void Plot_StoreHist1Bin(const char* VarName, double VarMin, double VarMax, const char* inmodefile1, const char* mode1_name=0, const char* AddRest, const char* StoreHistFileName )
// //// Plot A histogram where all bins except 9 are 0. 
// {
     
//   TFile* f1 = new TFile(inmodefile1);
//   TTree* EvtTree1 = (TTree*)f1->Get("EvtTree");

 
//   TFile * outhistfile;
//   if ( StoreHistFileName!="") {
//     outhistfile = new TFile(StoreHistFileName, "RECREATE");
//   } 
//   TH1F* EvtHist = new TH1F("EvtHist","EvtHist",25,VarMin,VarMax);
//   //TTree *EvtTree = new TTree("EvtTree", "EvtTree"); //Temporary for Synchronization.

//   TString input1=">>EvtHist";
//   input1=VarName+input1;

//   // input the ranges & additonal restrictions
//   char Max_char[30];
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   char Min_char[30];
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;

//   TString Restrictions;
//   if (AddRest!="") {
//     Restrictions = AddRest;
//     Restrictions = ")&&"+Restrictions;
//   } else {
//     Restrictions = ")";
//   }

//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;

//   cout << "Restrictions=" << Restrictions << endl;
//   EvtTree1->Draw(input1,Restrictions);
//   double entbin9;

//   double nbinsx=EvtHist->GetNbinsX();
//   for (Int_t n=1; n<(nbinsx+1);n++) {
//     if ( n==9 ) {
//       entbin9 = EvtHist->GetBinContent(n);
//       cout << "Ninth bin has " << entbin9 << " events" << endl;
//     } else {
//       EvtHist->SetBinContent(n,0);
//     }
//   }

//   //cout << "entbin9=" << entbin9 << endl;
//   double scale;

//   EvtHist->SetLineWidth(2);
//   EvtHist->SetLineColor(2);
//   scale=1/entbin9;
//   EvtHist->Scale(scale);
//   EvtHist->Draw();

//   lgnd = new TLegend(0.7,0.75,0.90,0.9);
//   lgnd->AddEntry(EvtHist,mode1_name,"l");
//   //lgnd->SetHeader("The Legend Title");
//   gStyle->SetOptStat(0);
//   EvtHist->SetXTitle(VarName);
//   EvtHist->SetYTitle("Normalized Events");
//   TString Title;
//   Title="-Distribution 9th Bin only";
//   Title=VarName+Title;
//   Title="  "+Title;
// //   Title=mode2_name+Title;
// //   Title=" vs. "+Title;
//   Title=mode1_name+Title;

//   EvtHist->SetTitle(Title);
//   lgnd->Draw();
//   if (StoreHistFileName!="") {
//    outhistfile->Write();
//   }
//   EvtHist->Draw();
//   lgnd->Draw();
//   //outhistfile->Close();


// }





// ////****OBSOLETE****////
// void Plot_CompareShapes_Obsolete(const char* VarName, double VarMin, double VarMax, const char* inmodefile1, const char* mode1_name=0, const char* inmodefile2, const char* mode2_name=0, const char* AddRest)
// //// Plots variable VarName for two modes, to the same scale (of mode1).  
// {
     
//   TFile* f1 = new TFile(inmodefile1);
//   TTree* EvtTree1 = (TTree*)f1->Get("EvtTree");
//   TFile* f2 = new TFile(inmodefile2);
//   TTree* EvtTree2 = (TTree*)f2->Get("EvtTree");

//   TH1D* h1 = new TH1D("h1","h1",25,VarMin,VarMax);
//   TH1D* h2 = new TH1D("h2","h2",25,VarMin,VarMax);

//   TString input1=">>h1";
//   input1=VarName+input1;
//   TString input2=">>h2";
//   input2=VarName+input2;

//   // input the ranges & additonal restrictions
//   char Max_char[30];
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   char Min_char[30];
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;

//   TString Restrictions;
//   if (AddRest!="") {
//     Restrictions = AddRest;
//     Restrictions = ")&&"+Restrictions;
//   } else {
//     Restrictions = ")";
//   }


//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;

//   cout << "Restrictions=" << Restrictions << endl;
//   EvtTree1->Draw(input1,Restrictions);
//   EvtTree2->Draw(input2,Restrictions);

//   double ent1 = h1->GetEntries();
//   cout << "ent1=" << ent1 << endl;
//   double ent2 = h2->GetEntries();
//   cout << "ent2=" << ent2 << endl;

//   double scale;
// //   scale = ent1/ent2;
// //   cout << "scale=" << scale << endl;

//   h1->SetLineWidth(2);
//   h1->SetLineColor(2);
//   scale=1/ent1;
//   h1->Scale(scale);
//   h1->Draw();

//   h2->SetLineWidth(2);
//   h2->SetLineColor(4);
//   scale=1/ent2;
//   h2->Scale(scale);
//   h2->Draw("same");

//   //lgnd = new TLegend(0.55,0.6,0.85,0.8);
//   lgnd = new TLegend(0.7,0.75,0.90,0.9);
//   lgnd->AddEntry(h1,mode1_name,"l");
//   lgnd->AddEntry(h2,mode2_name,"l");
//   //lgnd->SetHeader("The Legend Title");
//   gStyle->SetOptStat(0);
//   h1->SetXTitle(VarName);
//   h1->SetYTitle("Normalized Events");
//   TString Title;
//   Title="-Distributions";
//   Title=VarName+Title;
//   Title="  "+Title;
//   Title=mode2_name+Title;
//   Title=" vs. "+Title;
//   Title=mode1_name+Title;

//   h1->SetTitle(Title);
//   lgnd->Draw();


// }
