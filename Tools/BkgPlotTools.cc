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


#include "TMath.h"
#include "TF1.h"
#include "TMinuit.h"
#include "TLatex.h"

using namespace std;

void BkgPlotTools () {
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////*********Helper Functions*********//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void splitModeString(const char* stringlinein, vector<TString>& stringSetName, vector<Color_t>& stringSetColor, vector<TString>& stringSet, int& nStringSets, TString separationChar="ENDSET") {
//// Takes stringlinein, which should be of the form "name[0] color[0] string[0] separationChar name[1] color[1], string[1] separationChar ... ", strores them separately as well as records the number of strings
  nStringSets=0;
  int nSubstrings=0;
  stringSetName.clear();
  stringSetColor.clear();
  stringSet.clear();
  TString t_stringname, t_string, t_tempstr;
  Color_t t_stringcolor;
  istrstream inStrings(stringlinein);
  bool cont=true;
  bool readstring=true;


//   TString inModes_mu = "Data 1 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microMu_All_EPDv01.root 5020 ENDSET     WpJ 2 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWJets_EPDv01.root 0.0003849171 ENDSET    ZpJ 3 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microZJets_EPDv01.root 0.000084017952387 ENDSET    TTbar 5 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microTTbar_EPDv01.root 0.000044031 ENDSET    WW 4 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWW_EPDv01.root 0.0000101753087377979123";

  // input the names, file locations & event counts
  while ( cont ) {
    inStrings >> t_stringname;
    if ( t_stringname!="" ) {
      inStrings >> t_stringcolor;
      readstring=true;
      t_string="";
      nSubstrings=0;
      while ( readstring ) {
	inStrings >> t_tempstr;
	if ( (t_tempstr!="")&&(t_tempstr!=separationChar) ) {
	  t_string=t_string+t_tempstr;
	  t_string=t_string+" ";
	  nSubstrings++;
	} else {
	  readstring=false;
	  if ( t_tempstr=="" ) {
	    cont=false;
	  }
	}

	if ( nSubstrings>100 ) {
	  readstring=false;
	  cont=false;
	  cout << "Error: made 100 iterations and unable to read in the substring parameters for " << stringlinein << endl;
	}
      }
      stringSetName.push_back(t_stringname);
      stringSetColor.push_back(t_stringcolor);
      stringSet.push_back(t_string);
      nStringSets++;

      if ( nStringSets>100 ) {
	cont=false;
	cout << "Error: made 100 iterations and unable to read in the full string parameters for " << stringlinein << endl;
      }
    } else {
      cont=false;
    }
  }

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//
void readModeFilesAndScales(const char* modelinein, vector<TString>& fileName, vector<double>& scaleFactor, int& nModes) {
//// Takes modelinein, which should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color [2] modename[2] ... ", and extracts these parameters as well as the number of modes
  nModes=0;
  fileName.clear();
  scaleFactor.clear();
  TString t_fileName;
  double t_scaleFactor;
  istrstream inModes(modelinein);
  bool cont=true;

  // input the names, file locations & event counts
  while ( cont==true ) {
    inModes >> t_fileName;
    if ( t_fileName!="" ) {
      inModes >> t_scaleFactor;
      fileName.push_back(t_fileName);
      scaleFactor.push_back(t_scaleFactor);
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

void fillAHistogram(TH1F* &h, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, const char* infilename, bool dosumW2 = false)
////Fill in histogram h from the file infilename.
{
  
  TString Restrictions, histload_str;
  char Max_char[30], Min_char[30];

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
  TFile* f;
  TTree* InTree;
  f = new TFile(infilename);
  InTree = (TTree*)f->Get(TreeName);
  
  //Fill the histogram
  histload_str=">>htemp";
  histload_str=VarName+histload_str;
  cout << "hist input =" << histload_str << endl;
  
  //hname="h";
  //cout << "hname=" << hname << endl;
    
  TH1F* htemp = new TH1F("htemp","htemp",NBins,VarMin,VarMax);
  InTree->Draw(histload_str,Restrictions);
  h = (TH1F*)htemp->Clone();
  if ( dosumW2 ) {
    h->Sumw2();
  }

  cout << "hentries=" << h->GetEntries() << endl; 
  delete htemp;
  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void fillHistograms(TH1F* h[20], const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, int nModes, vector<TString>& infilenames, bool dosumW2 = false, bool useDifferingCuts=false, const char* InDiffCuts=0, bool weightTheHistograms=false, const char* weightVar=0)
////Fill in each histogram in the array h from the files in infilenames.
{
  
  //read the Differing Cuts, if any
  vector<TString> cuts;
  cuts.clear();
  TString t_cut;
  istrstream inCuts(InDiffCuts);
  if ( useDifferingCuts ) {
    for (Int_t n=0; n<nModes; n++) {
      inCuts >> t_cut;
      t_cut=t_cut+")&&";
      t_cut="("+t_cut;
      cuts.push_back(t_cut);
    }
  }


  TString Restrictions, hname, histload_str;
  char Max_char[30], Min_char[30], n_char[5];

  /// Make a string defining the restrictions (Range + Additional Ones).
  
  sprintf(Max_char,"%.2e",VarMax);
  cout << "Max_char=" << Max_char << endl;
  sprintf(Min_char,"%.2e",VarMin);
  cout << "Min_char=" << Min_char << endl;

  Restrictions = ")";
  if (AddRest!=0) {
    Restrictions = AddRest+Restrictions;
    Restrictions = ")&&("+Restrictions;
  } else {

  }
  Restrictions=Max_char+Restrictions;
  Restrictions="<"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions=")&&("+Restrictions;
  Restrictions=Min_char+Restrictions;
  Restrictions=">"+Restrictions;
  Restrictions=VarName+Restrictions;
  Restrictions="("+Restrictions;
  if ( weightTheHistograms ) {
    Restrictions=Restrictions+" )";
    Restrictions="*( "+Restrictions;
    Restrictions=weightVar+Restrictions;
  }
  cout << "Restrictions=" << Restrictions << endl;
  
  //Open the files
  TFile* f[20];
  TTree* InTree[20];
  for (Int_t n=0; n<nModes; n++) {
    f[n] = new TFile(infilenames[n]);
    InTree[n] = (TTree*)f[n]->Get(TreeName);

//     if ( useMETCorr120530 ) {
//       if ( n>101.5 ) { //i.e. correct the Data & QCD
// 	InTree[n]->SetAlias("METx","event_met_pfmet*cos(event_met_pfmetPhi)");
// 	InTree[n]->SetAlias("METy","event_met_pfmet*sin(event_met_pfmetPhi)");
// 	InTree[n]->SetAlias("Dx","0.3542+0.2635*event_nPV");
// 	InTree[n]->SetAlias("Dy","0.1889-0.1664*event_nPV");
// 	InTree[n]->SetAlias("METcorr","sqrt(METx*METx+METy*METy-2*METx*Dx-2*METy*Dy+Dx*Dx+Dy*Dy)");
// 	InTree[n]->SetAlias("DMET","sqrt(METx*METx+METy*METy)-METcorr");
// 	if ( isMuon ) {
// 	  InTree[n]->SetAlias("lphi","W_muon_phi");
// 	  InTree[n]->SetAlias("lpt","W_muon_pt");
// 	} else {
// 	  InTree[n]->SetAlias("lphi","W_electron_phi");
// 	  InTree[n]->SetAlias("lpt","W_electron_pt");
// 	}
// 	InTree[n]->SetAlias("WmTcorr","sqrt(W_mt*W_mt+2*lpt*(DMET-Dx*cos(lphi)-Dy*sin(lphi)))");
//       } else {
// 	InTree[n]->SetAlias("METcorr","event_met_pfmet");
// 	InTree[n]->SetAlias("WmTcorr","W_mt");
//       }
//     }


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
    
    TH1F* htemp = new TH1F("htemp","htemp",NBins,VarMin,VarMax);
    if ( useDifferingCuts ) {
      InTree[n]->Draw(histload_str,cuts[n]+Restrictions);
    } else {
      InTree[n]->Draw(histload_str,Restrictions);
    }
    h[n] = (TH1F*)htemp->Clone();
    if ( dosumW2 ) {
      h[n]->Sumw2();
    }
    h[n]->SetName(hname);
    h[n]->SetTitle(hname);
//     cout << "htemp Entries = " << htemp->GetEntries() << endl;
//     cout << "h[" << n << "] Entries = " << h[n]->GetEntries() << endl;  
    
    delete htemp;
  }
  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//
void fillCombinedHistograms(TH1F* h[20], const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest, vector<TString> inModeSets, vector<TString> inModeSetNames, int nModeSets, int nQCD=-1, bool firstSetIsData=true, bool dosumW2 = false)
////Fill in each histogram in the array h from the files in infilenames.
{
  
//   //read the Differing Cuts, if any
//   vector<TString> cuts;
//   cuts.clear();
//   TString t_cut;
//   istrstream inCuts(InDiffCuts);
//   if ( useDifferingCuts ) {
//     for (Int_t n=0; n<nModes; n++) {
//       inCuts >> t_cut;
//       t_cut=t_cut+")&&";
//       t_cut="("+t_cut;
//       cuts.push_back(t_cut);
//     }
//   }


  TString Restrictions, hname, histload_str;
  vector<TString> fileNames;
  vector<double> scaleFactor; //Usually CrossSection/NGenerated, so that scalefactor*nentries=number of expected events per pb. However, scaleFactor[0]=Lumi in pb if firstSetIsData=true.
  int nSubEntries=0;
  double nexpectedEvts=0;
  double nData=0.0;
  double scale=0;
  double Lumi=1.0;
  //  int startSetCount=0;
  char n_char[5];
  //  char Max_char[30], Min_char[30], 

//   /// Make a string defining the restrictions (Range + Additional Ones).
  
//   sprintf(Max_char,"%.2e",VarMax);
//   cout << "Max_char=" << Max_char << endl;
//   sprintf(Min_char,"%.2e",VarMin);
//   cout << "Min_char=" << Min_char << endl;

//   Restrictions = ")";
//   if (AddRest!=0) {
//     Restrictions = AddRest+Restrictions;
//     Restrictions = ")&&("+Restrictions;
//   } else {

//   }
//   Restrictions=Max_char+Restrictions;
//   Restrictions="<"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions=")&&("+Restrictions;
//   Restrictions=Min_char+Restrictions;
//   Restrictions=">"+Restrictions;
//   Restrictions=VarName+Restrictions;
//   Restrictions="("+Restrictions;
//   if ( weightTheHistograms ) {
//     Restrictions=Restrictions+" )";
//     Restrictions="*( "+Restrictions;
//     Restrictions=weightVar+Restrictions;
//   }
//  cout << "Restrictions=" << Restrictions << endl;
  
//   //Open the files
//   TFile* f[20];
//   TTree* InTree[20];

  for (Int_t n=0; n<nModeSets; n++) {
    sprintf(n_char,"%i",n);
    hname="]";
    hname = n_char + hname;
    hname="h["+hname;
    cout << "hname=" << hname << endl;
    //Get the filenames & scalefactors
    readModeFilesAndScales(inModeSets[n],fileNames,scaleFactor,nSubEntries);
    //Fill the histograms for this set
    TH1F* hSet[20];
    fillHistograms(hSet,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nSubEntries,fileNames,dosumW2);
    TH1F* htemp = new TH1F("htemp","htemp",NBins,VarMin,VarMax);
    if ( (n==0)&&(firstSetIsData=true) ) {
      Lumi=scaleFactor[0];
      nData=hSet[0]->GetEntries();
      nexpectedEvts=nData;
      htemp->Add(hSet[0]);
    } else {
      //Add the histograms (with the appropriate scale factors)
      nexpectedEvts=0;
      for (Int_t m=0; m<nSubEntries; m++) {
	scale=scaleFactor[m]*Lumi;
	if ( n==nQCD ) {
	  cout << "Using QCD scaling scheme" << endl;
	  scale=scaleFactor[m]*nData/(hSet[m]->GetEntries());
	}
	nexpectedEvts+=scale*(hSet[m]->GetEntries());
	hSet[m]->Scale(scale);
	htemp->Add(hSet[m]);
      }
    }

    h[n] = (TH1F*)htemp->Clone();
    h[n]->SetName(hname);
    h[n]->SetTitle(hname);
    delete htemp;
    cout << "n=" << n << " Process=" << inModeSetNames[n] << " nexpectedEvts=" << nexpectedEvts << endl;

  }


  
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////***** Primary Plot Functions*****//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Double_t RaileighFunction(Double_t *x, Double_t *par)
//input is of the form Constant,sigma1,sigma2
{
  Double_t sigmaSum = par[1]+par[2]*x[0];
  Double_t fitval = par[0]*x[0]*TMath::Exp(-0.5*x[0]*x[0]/(sigmaSum*sigmaSum));
  return fitval;
} 

void Plot_ABkg(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* inmodefile, const char* AddRest = 0, const char* SaveName = 0, int histColor = 0, char* XAxisTitle = 0, char* YAxisTitle = 0, int NDivisions = -1, bool doGaussianFit = false, bool doRaileighFit = false)
//// Plots variable VarName for a particular mode.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

  TH1D* hist = new TH1D("hist","hist",NBins,VarMin,VarMax);
  // TCanvas *cnv = new TCanvas("cnv","cnv",10,10,500,500);
  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);

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
  hist->SetLineColor(histColor);
  hist->Draw();

//   TLegend* lgnd = new TLegend(0.7,0.75,0.90,0.9);
//   lgnd->AddEntry(hist,mode_name,"l");
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.97);
  hist->SetXTitle(VarName);
  if ( XAxisTitle!=0 ) {
    hist->SetXTitle(XAxisTitle);
  }
  hist->SetYTitle("Events");
  if ( YAxisTitle!=0 ) {
    hist->SetYTitle(YAxisTitle);
  }
  if ( TitleName!=0 ) {
    hist->SetTitle(TitleName);
  } else {
    TString Title;
    Title="-Distribution";
    Title=VarName+Title;
    Title="  "+Title;
    hist->SetTitle(Title);
  }

  //lgnd->Draw();

  if ( doRaileighFit ) {
    ///Fit with a Raileigh function
    TF1 *fitRaileigh = new TF1("fitRaileigh",RaileighFunction,VarMin,VarMax,3);
    fitRaileigh->SetParameters(6000.0,1.0,1.0);
    fitRaileigh->SetParLimits(0,1,100000);
    fitRaileigh->SetParLimits(1,0.0,100.0);
    fitRaileigh->SetParLimits(2,-10.0,10.0);
    fitRaileigh->SetParNames("Amp","s0","s1");


    hist->Fit("fitRaileigh");
    gMinuit->mnmatu(1);
    gStyle->SetOptStat(0);
  
    Double_t parR[3];
    Double_t FCN;
    fitRaileigh->GetParameters(parR);
    FCN=fitRaileigh->GetChisquare();
    
//     char text0[30];
//     sprintf(text0,"FCN = %6.3f",FCN);
//     TLatex* t0 = new TLatex(0.60,0.8,text0);
//     t0->SetTextSize(1./22.);
//     t0->SetTextFont(132);
//     t0->SetNDC();
//     t0->Draw();

    char text1[30];
    sprintf(text1,"#sigma_{0} = %6.3f",parR[1]);
    TLatex* t1 = new TLatex(0.60,0.8,text1);
    t1->SetTextSize(1./22.);
    t1->SetTextFont(132);
    t1->SetNDC();
    t1->Draw();

    char text2[30];
    sprintf(text2,"#sigma_{1} = %6.3f",parR[2]);
    TLatex* t2 = new TLatex(0.60,0.75,text2);
    t2->SetTextSize(1./22.);
    t2->SetTextFont(132);
    t2->SetNDC();
    t2->Draw();

  }

  if ( doGaussianFit ) {

    ///Fit with a Raileigh function
    TF1 *fitGaus = new TF1("fitGaus","gaus(0)",VarMin,VarMax);
    fitGaus->SetParameters(1000.0,(VarMin+VarMax)/2,fabs(VarMin-VarMax)/2);
    fitGaus->SetParLimits(0,1,100000);
    fitGaus->SetParLimits(1,VarMin,VarMax);
    fitGaus->SetParLimits(2,fabs(VarMin-VarMax)/64,fabs(VarMin-VarMax));
//     fitGaus->SetParNames("Amp","s0","s1");


    hist->Fit("fitGaus");
    gMinuit->mnmatu(1);
    gStyle->SetOptStat(0);
  
    Double_t parG[3];
    Double_t* errG;
    Double_t FCN;
    fitGaus->GetParameters(parG);
    errG=fitGaus->GetParErrors();
    FCN=fitGaus->GetChisquare();
    
//     char text0[30];
//     sprintf(text0,"FCN = %6.3f",FCN);
//     TLatex* t0 = new TLatex(0.60,0.8,text0);
//     t0->SetTextSize(1./22.);
//     t0->SetTextFont(132);
//     t0->SetNDC();
//     t0->Draw();

    char text1[30];
    sprintf(text1,"#mu = %6.3f#pm%6.3f",parG[1],errG[1]);
    TLatex* t1 = new TLatex(0.65,0.8,text1);
    t1->SetTextSize(1./22.);
    t1->SetTextFont(132);
    t1->SetNDC();
    t1->Draw();

    char text2[30];
    sprintf(text2,"#sigma = %6.3f#pm%6.3f",parG[2],errG[2]);
    TLatex* t2 = new TLatex(0.65,0.75,text2);
    t2->SetTextSize(1./22.);
    t2->SetTextFont(132);
    t2->SetNDC();
    t2->Draw();

  }



  hist->GetYaxis()->SetTitleOffset(1.25);
  hist->SetNdivisions(NBins/2);
  if ( NDivisions > 0 ) {
    hist->SetNdivisions(NDivisions);
  }

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//



void Plot_CompareShapesAndCuts(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0, bool withErrors = false, bool useDifferingCuts=false, const char* InExtraCuts=0, char* XAxisTitle = 0)
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given event.  
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
//// InCuts should be of the form "cut[0] cut[1] cut[2] ..."
{
  // int NBins=25;
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
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }
  lgnd->SetFillColor(kWhite);
  double entries;

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,withErrors,useDifferingCuts,InExtraCuts);


  for (Int_t n=0; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    //    evts=evts+evtcounts[n];
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    //cout << "integral=" << h[n]->Integral() << endl;
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
  gStyle->SetTitleX(0.35);
  gStyle->SetTitleY(0.97);
  h[0]->SetMinimum(0);
  h[0]->SetMaximum(max*1.30);

  if ( withErrors ) {
    h[0]->Draw("Ep");
  } else {
    h[0]->Draw();
  }

  for (Int_t n=1; n<nModes; n++) {
    if ( withErrors ) {
      h[n]->Draw("Ep:same");
    } else {
      h[n]->Draw("same");
    }
    cout << modenames[0] << " vs. " << modenames[n] << " : Kolmogorov Test = " << h[n]->KolmogorovTest(h[0]) << endl;
  }

  //cout << " Kolmogorov Crosscheck = " << h[0]->KolmogorovTest(h[1]) << endl;

  h[0]->SetTitle(TitleName);
  h[0]->SetXTitle(VarName);
  if ( XAxisTitle!=0 ) {
    h[0]->SetXTitle(XAxisTitle);
  }

  h[0]->SetYTitle("Normalized Event Count");
  //h[0]->SetXTitleOffset(0.2);
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// // -----------------------------------------------------------------------------------------------------------------------------------------------------------//


// void Plot_CompareCuts(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* CutA, const char* CutB, const char* infilename, const char* AddRest = 0, const char* SaveName = 0, bool withErrors = false, const char* LabelA=0, const char* LabelB=0)
// //// Plots variable VarName for the given modes. Two histograms are normalized to unity
// {

//   TString AddRestA, AddRestB;
//   double entries,maxA,maxB,max;

//   AddRestA=AddRest;
//   AddRestA=")&&"+AddRestA;
//   AddRestA=CutA+AddRestA;
//   AddRestA="("+AddRestA;
//   AddRestB=AddRest;
//   AddRestB=")&&"+AddRestB;
//   AddRestB=CutB+AddRestB;
//   AddRestB="("+AddRestB;

//   //Define the histograms
//   TH1F* hA;
//   TH1F* hB;
//   fillAHistogram(hA,TreeName,VarName,NBins,VarMin,VarMax,AddRestA,infilename,true);
//   //  cout << "1" << endl;
//   fillAHistogram(hB,TreeName,VarName,NBins,VarMin,VarMax,AddRestB,infilename,true);
//   //  cout << "2" << endl;


//   TCanvas *cnv1 = new TCanvas("cnv1","cnv1",10,10,500,500);
//   TLegend *lgnd;
//   lgnd = new TLegend(0.6,0.7,0.900,0.9);
//   lgnd->SetFillColor(kWhite);

//   // process histogram A
//   entries = hA->GetEntries();
//   cout << CutA <<": entries=" << entries << endl;
//   hA->Scale(1.0/entries);
//   hA->SetLineWidth(2);
//   hA->SetLineColor(2);
//   if ( LabelA!=0 ) {
//     lgnd->AddEntry(hA,LabelA,"l");
//   } else {
//     lgnd->AddEntry(hA,CutA,"l");
//   }
//   maxA=hA->GetMaximum();
//   // process histogram B
//   entries = hB->GetEntries();
//   cout << CutB <<": entries=" << entries << endl;
//   hB->Scale(1.0/entries);
//   hB->SetLineWidth(2);
//   hB->SetLineColor(4);
//   if ( LabelB!=0 ) {
//     lgnd->AddEntry(hB,LabelB,"l");
//   } else {
//     lgnd->AddEntry(hB,CutB,"l");
//   }

//   maxB=hB->GetMaximum();

//   //Make sure the maximum is set to include all events.
//   if ( maxA > maxB ){
//     max=maxA;
//   } else {
//     max=maxB;
//   }

//   gStyle->SetOptStat(0);
//   gStyle->SetTitleX(0.13);
//   gStyle->SetTitleY(0.97);
//   hA->SetMinimum(0);
//   hA->SetMaximum(max*1.20);

//   if ( withErrors ) {
//     hA->Draw("Ep");
//   } else {
//     hA->Draw();
//   }

//   if ( withErrors ) {
//     hB->Draw("Ep:same");
//   } else {
//     hB->Draw("same");
//   }

//   hA->SetTitle(TitleName);
//   hA->SetXTitle(VarName);
//   hA->SetYTitle("Normalized Event Count");
//   hA->GetYaxis()->SetTitleOffset(1.25);
//   lgnd->Draw();

//   /// Save the output
//   if ( SaveName!=0 ) {
//     cnv1->SaveAs(SaveName);
//   }

// }


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_TwoSetsOfShapes(const char* TitleName, int NumberInSet1, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0)
//// Plots variable VarName for the given processes as two histograms: hSet1 is the sum of the histograms 0-(NumberInSet1-1), hSet2 is the sum of the histograms NumberInSet1-(nModes-1). The histogram for each process is scaled by evtCount[n] prior to being included in hSet.
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
{
  //int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
  double maxtemp=0;
  double max=0;
  //  double scale;
  double Set1cnt=0;
  double Set2cnt=0;
  cout << "1" << endl;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  cout << "1.1" << endl;
  //Define and initialize the histograms
  TH1F* h[20];
  TH1F* hSet1;
  TH1F* hSet2;
  for (Int_t n=0; n<NumberInSet1; n++) {
    h[n] = new TH1F("h[n]","h[n]",NBins,VarMin,VarMax);
    h[n]->Sumw2();
  }
  hSet1 = new TH1F("hSet1","hSet1",NBins,VarMin,VarMax);
  hSet1->Sumw2();
  hSet2 = new TH1F("hSet2","hSet2",NBins,VarMin,VarMax);
  hSet2->Sumw2();

  cout << "2" << endl;

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  lgnd = new TLegend(0.7,0.9-0.05*2,0.90,0.9);
  double entries;

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,true);

  cout << "3" << endl;

  for (Int_t n=0; n<NumberInSet1; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    Set1cnt=Set1cnt+evtcounts[n]*entries;
    h[n]->Scale(evtcounts[n]);
    cout << "3.4" << endl;
    hSet1->Add(h[n]);
    cout << "3.45" << endl;
  }

  cout << "3.5" << endl;
  //hSet1->Sumw2();
  cout << "3.6" << endl;
  hSet1->Scale(1.0/Set1cnt);
  hSet1->SetLineWidth(2);
  hSet1->SetLineColor(colors[0]);
  lgnd->AddEntry(hSet1,modenames[0],"l");
  //Make sure the maximum is set to include all events.
  maxtemp=hSet1->GetMaximum();
  if ( maxtemp > max ){
    max=maxtemp;
  }


  for (Int_t n=NumberInSet1; n<nModes; n++) {
    entries = h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    Set2cnt=Set2cnt+evtcounts[n]*entries;
    h[n]->Scale(evtcounts[n]);
    hSet2->Add(h[n]);
  }
  cout << "4" << endl;
  hSet2->Scale(1.0/Set2cnt);
  hSet2->SetLineWidth(2);
  hSet2->SetLineColor(colors[NumberInSet1]);
  lgnd->AddEntry(hSet2,modenames[NumberInSet1],"l");
  //Make sure the maximum is set to include all events.
  maxtemp=hSet2->GetMaximum();
  if ( maxtemp > max ){
    max=maxtemp;
  }


//   entries = h[0]->GetEntries();
//   h[0]->Draw();
//   cout <<"entries=" << entries << endl;
  cout << "5" << endl;

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  hSet1->SetMinimum(0);
  hSet1->SetMaximum(max*1.3);
  hSet1->Draw("Ep");
  hSet2->Draw("Ep:same");

  hSet1->SetTitle(TitleName);
  hSet1->SetXTitle(VarName);
  hSet1->SetYTitle("Normalized Events");
  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void Plot_StackedBackgrounds(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, const char* SaveName = 0 )
//// Similar to Plot_CompareShapes, but plots backgrounds stacked on top of each other.
//// Plots variable VarName for the given modes. The size (area) of each histogram is proportional to the number of given events. 
/// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
/// Use AddRest to give additional restrictions
{
  //int NBins=25;
  vector<TString> modenames, infilenames;
  vector<double> evtcounts;
  vector<int> colors;
  int nModes = 0;
//   double maxtemp=0;
//   double max=0;
  double scale;
  double entries;

  readModes(inModes,modenames,infilenames,evtcounts,colors,nModes);

  //Create the individual histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  if ( nModes<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModes,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }

  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames);

  //Create the accumulated histograms
  TH1F* ha[20];
  
  TString haname;
  //  int nentries;
  char n_char[30];

//   TString input;
//   input=")";
//   input=Max_char+input;
//   input=","+input;
//   input=Min_char+input;
//   input=">>ht(25,"+input;
//   input=VarName+input;
//   cout << "input=" << input << endl;

  for (Int_t n=0; n<nModes; n++) {
    sprintf(n_char,"%i",n);
    haname="]";
    haname = n_char + haname;
    haname="ha["+haname;
    ha[n]=new TH1F(haname,haname,NBins,VarMin,VarMax);
    entries=h[n]->GetEntries();
    cout << modenames[n] <<": entries=" << entries << endl;
    scale=evtcounts[n]/entries;
    cout << "scale=" << scale << endl;
    h[n]->Scale(scale);
    for (Int_t m=0; m<(n+1); m++) {
      ha[n]->Add(h[m]);
    }
    ha[n]->SetFillColor(colors[n]);
    
 }

  //  cout << "nModes=" << nMode << endl;
  int k;
  for (Int_t n=0; n<nModes; n++) {
    k=nModes-n-1;
    if ( n==0 ) {
      ha[k]->Draw();
    } else {
      ha[k]->Draw("same");
    }
    lgnd->AddEntry(ha[k],modenames[k],"f");
  }
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.97);
  //  ha[nModes-1]->SetTitle("Expected Yields per pb^{-1}");
  ha[nModes-1]->SetTitle(TitleName);
  ha[nModes-1]->SetXTitle(VarName);
  ha[nModes-1]->SetYTitle("Evts/pb^{-1}");

  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//

void Plot_Validation(const char* TitleName, const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* AddRest = 0, const char* inModes = 0, int nQCD=-1, const char* SaveName = 0 , bool scaleMCtoData = true, bool drawMCwithErrors=false )
//// Makes a Validation/Control plot with MC either scaled either to Data or to theory expectations.
//// inModes should be of the form "modename[0] infilename[0] evtCount[0] color[0] modename[1] infilename[1] evtCount[1] color[1] modename[2] ... " 
//// mode0=Data (evtCount[0]=Lumi), evtCount[i]=XSection/N_MCGenerated.
//// Use AddRest to give additional restrictions
{

  vector<TString> modeSetName, modeSets;
  vector<Color_t> modeSetColor;
  vector<double> inDouble;
  //vector<int> colors;
  int nModeSets = 0;
  //int nModeSets_sw2 = 0;
  double size;
  //  double entries;
  double nMCtotal=0;
  double nData=0;

  splitModeString(inModes,modeSetName,modeSetColor,modeSets,nModeSets,"ENDSET");
  //  readModes(inModes,modenames,infilenames,inDouble,colors,nModes);

  //Create the individual histograms
  TH1F* h[20];

  TCanvas *cnv = new TCanvas("cnv","cnv",10,10,900,600);
  TLegend *lgnd;
  if ( nModeSets<10 ) {
    lgnd = new TLegend(0.7,0.9-0.05*nModeSets,0.90,0.9);
  } else {
    lgnd = new TLegend(0.7,0.4,0.90,0.9);
  }

  //cout << "1" << endl;
  //  fillHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes,infilenames,false);
  fillCombinedHistograms(h,TreeName,VarName,NBins,VarMin,VarMax,AddRest,modeSets,modeSetName,nModeSets,nQCD,true,false);
  //  cout << "2" << endl;
  //cout << "h[0]->GetEntries()=" << h[0]->GetEntries() << endl;
  //cout << "h[0]->Integral()=" << h[0]->Integral() << endl;
  //cout << "3" << endl;

//   TH1F* h_sw2[20];
//   fillHistograms(h_sw2,TreeName,VarName,NBins,VarMin,VarMax,AddRest,nModes_sw2,infilenames,true);

  //Create the accumulated histograms
  TH1F* ha[20];  
  TString haname;
  char n_char[30];

  for (Int_t n=1; n<nModeSets; n++) {
    sprintf(n_char,"%i",n);
    haname="]";
    haname = n_char + haname;
    haname="ha["+haname;
    ha[n]=new TH1F(haname,haname,NBins,VarMin,VarMax);
    //    ha[n]->Sumw2();
    size=h[n]->Integral();
    //    scale=inDouble[0];//*Lumi (in pb^-1)
    nMCtotal+=size;
    //cout << modeSetName[n] <<" : entries=" << entries << " : Lumi=" << inDouble[0] <<" : expectedEvts=" <<  entries*scale << endl;
    //h[n]->Scale(scale);
  }

  //cout << "1" << endl;
  nData=h[0]->Integral();
  cout << "nMCtotalExpected=" << nMCtotal << " : nData=" << nData << endl;

  for (Int_t n=1; n<nModeSets; n++) {
    if ( scaleMCtoData ) {
      h[n]->Scale(nData/nMCtotal);
    }
    for (Int_t m=1; m<(n+1); m++) {
      ha[n]->Add(h[m]);
    }

    ha[n]->SetFillColor(modeSetColor[n]);
  }


  //  cout << "nModes=" << nMode << endl;
  int k;
  for (Int_t n=1; n<nModeSets; n++) {
    k=nModeSets-n;
    if ( n==1 ) {
      ha[k]->Draw();
    } else {
      ha[k]->Draw("same");
    }
    lgnd->AddEntry(ha[k],modeSetName[k],"f");
  }


//   if ( drawMCwithErrors ) {
//     cout << "drawMCwithErrors=true" << endl;
//     //Add the total MC with errors (we now need the histograms with the Sumw2() option)
//     ha[0]=new TH1F("ha[0]","ha[0]",NBins,VarMin,VarMax);
//     ha[0]->Sumw2();
//     for (Int_t n=1; n<nModes_sw2; n++) {
//       if ( scaleMCtoData ) {
// 	h_sw2[n]->Scale(nData/nMCtotal);
//       }
//       ha[0]->Add(h_sw2[n]);
//     }
//     ha[0]->Draw("esame");
//     ha[0]->SetLineWidth(2);
//     ha[0]->SetLineColor(4);
//     lgnd->AddEntry(ha[0],"TotalMC","l");
//   }

  //Add the data
  h[0]->Draw("esame");
  h[0]->SetLineWidth(2);
  h[0]->SetLineColor(modeSetColor[0]);
  h[0]->SetFillColor(modeSetColor[0]);
  lgnd->AddEntry(h[0],modeSetName[0],"l");

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.97);
  ha[nModeSets-1]->SetTitle(TitleName);
  ha[nModeSets-1]->SetXTitle(VarName);
  ha[nModeSets-1]->SetYTitle("Evts");

  lgnd->Draw();

  /// Save the output
  if ( SaveName!=0 ) {
    cnv->SaveAs(SaveName);
  }

}



void Plot_AllValidations(const char* TitleName, int NBins, bool scaleMCtoData = true, bool drawMCwithErrors=false )
//// Makes validation plots for all of the variables
//// the SaveName=savePrefix+VarName+.png
{
  TString saveDir = "/uscms_data/d2/aperloff/CMSSW_4_2_8/src/TAMUWW/Tools/bin/MEValidation/";
  const int NVars=15;
  TString VarName[NVars];
  TString VarTitle[NVars];
  double VarMin[NVars];
  double VarMax[NVars];

//   ///Diboson:
//   eventProbs2jet.push_back(new WWEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new WZEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new WZtobbEventProb2Jet(rootInt, bTF));


//   ///W or Z backgrounds:
//   eventProbs2jet.push_back(new WLgEventProb2Jet(rootInt, lightTF, gluonTF));
//   eventProbs2jet.push_back(new WLgSubleadingEventProb2Jet(rootInt, lightTF, gluonTF));
//   eventProbs2jet.push_back(new WLLEventProb2Jet(rootInt, lightTF));
//   eventProbs2jet.push_back(new WLbEventProb2Jet(rootInt, lightTF, bTF));
//   eventProbs2jet.push_back(new WbbEventProb2Jet(rootInt, bTF));
//   eventProbs2jet.push_back(new ZLightEventProb2Jet(rootInt, lightTF));


//   ///Top and QCD:
//   eventProbs2jet.push_back(new ttEventProb2Jet(divonneInt, bTF));
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
//   eventProbs2jet.push_back(new tChannelEventProb2Jet(rootInt, bTF, lightTF));
//   eventProbs2jet.push_back(new sChannelEventProb2Jet(rootInt, bTF));
//   eventProbs2jet.push_back(new STopTWEventProb2Jet(divonneInt_reduceComputingTime, lightTF));
//   eventProbs2jet.back()->setBounds(3, 0, MEConstants::beamEnergy);
//   eventProbs2jet.back()->setBounds(4, 0, TMath::TwoPi());
//   eventProbs2jet.back()->setBounds(5, 0, TMath::Pi());
//   eventProbs2jet.push_back(new QCDEventProb2Jet(rootInt, gluonTF));


//   ///HWW:
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 150));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 160));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 170));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 180));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 190));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 200));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 250));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 300));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 350));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 400));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 450));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 500));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 550));
//   eventProbs2jet.push_back(new HWWEventProb2Jet(rootInt, lightTF, 600));

  //Initialize
  VarName[0]="Mjj";
  VarTitle[0]="M_{jj}";
  VarMin[0]=40.0;
  VarMax[0]=300.0;
  VarName[1]="log(m_tProbStat.tEventProb[0])";
  VarTitle[1]="logWWEventProb";
  VarMin[1]=-35;
  VarMax[1]=-0.0;
  VarName[2]="log(m_tProbStat.tEventProb[1])";
  VarTitle[2]="logWZEventProb";
  VarMin[2]=-35;
  VarMax[2]=-0.0;
  VarName[3]="log(m_tProbStat.tEventProb[2])";
  VarTitle[3]="logWZbbEventProb";
  VarMin[3]=-35;
  VarMax[3]=-0.0;
  VarName[4]="log(m_tProbStat.tEventProb[3])";
  VarTitle[4]="logWLgEventProb";
  VarMin[4]=-35;
  VarMax[4]=-0.0;
  VarName[5]="log(m_tProbStat.tEventProb[4])";
  VarTitle[5]="logWLgSubleadingEventProb";
  VarMin[5]=-35;
  VarMax[5]=-0.0;
  VarName[6]="log(m_tProbStat.tEventProb[5])";
  VarTitle[6]="logWLLEventProb";
  VarMin[6]=-35;
  VarMax[6]=-0.0;
  VarName[7]="log(m_tProbStat.tEventProb[6])";
  VarTitle[7]="logWLbEventProb";
  VarMin[7]=-35;
  VarMax[7]=-0.0;
  VarName[8]="log(m_tProbStat.tEventProb[7])";
  VarTitle[8]="logWbbEventProb";
  VarMin[8]=-35;
  VarMax[8]=-0.0;
  VarName[9]="log(m_tProbStat.tEventProb[8])";
  VarTitle[9]="logZLightEventProb";
  VarMin[9]=-35;
  VarMax[9]=-0.0;
  VarName[10]="log(m_tProbStat.tEventProb[9])";
  VarTitle[10]="logttEventProb";
  VarMin[10]=-35;
  VarMax[10]=-0.0;
  VarName[11]="log(m_tProbStat.tEventProb[10])";
  VarTitle[11]="logtChannelEventProb";
  VarMin[11]=-35;
  VarMax[11]=-0.0;
  VarName[12]="log(m_tProbStat.tEventProb[11])";
  VarTitle[12]="logsChannelEventProb";
  VarMin[12]=-35;
  VarMax[12]=-0.0;
  VarName[13]="log(m_tProbStat.tEventProb[12])";
  VarTitle[13]="logtwChannelEventProb";
  VarMin[13]=-35;
  VarMax[13]=-0.0;
  VarName[14]="log(m_tProbStat.tEventProb[13])";
  VarTitle[14]="logQCDEventProb";
  VarMin[14]=-35;
  VarMax[14]=-0.0;
//   VarName[]="log(m_tProbStat.tEventProb[])";
//   VarTitle[]="log";
//   VarMin[]=-35;
//   VarMax[]=-10;
//   VarName[1]="log(epdPretagWWandWZ)";
//   VarMin[1]=-15.0;
//   VarMax[1]=5.0;


////Cross-Section/NGenerated
//18.2/4265243=4.22015814808206740e-06 for WZ
//   double STopWeight = 3.19/259971;
//   double STopBarWeight = 1.44/137980;
//   double TTopWeight = 41.92/3900171;
//   double TTopBarWeight = 22.65/1944826;
//   double TWTopWeight = 7.87/795379;
//   double TWTopBarWeight = 7.87/787629;
// QCD Fractions: double rel2jet = 0.0663, rel3jet = 0.0229, rmu2jet = 0.001625, rmu3jet = 0.;


///No QCD:
//  TString inModes_mu = "Data 1 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microMu_All_EPDv01.root 5020 ENDSET     WpJ 2 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWJets_EPDv01.root 0.0003849171 ENDSET    ZpJ 3 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microZJets_EPDv01.root 0.000084017952387 ENDSET    Top 5 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microTTbar_EPDv01.root 0.000044031 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopT_T_EPDv01.root 0.0000107482 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopS_T_EPDv01.root 0.00001227 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopTW_T_EPDv01.root 0.000009895 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopT_Tbar_EPDv01.root 0.000011646 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopS_Tbar_EPDv01.root 0.000010436 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopTW_Tbar_EPDv01.root 0.000009992 ENDSET    Diboson 4 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWW_EPDv01.root 0.0000101753087377979123 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWZ_EPDv01.root 0.00000422";


//  TString inModes_el = "Data 1 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microEl_All_EPDv01.root 5020 ENDSET     WpJ 2 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWJets_EPDv01.root 0.0003849171 ENDSET    ZpJ 3 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microZJets_EPDv01.root 0.000084017952387 ENDSET    Top 5 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microTTbar_EPDv01.root 0.000044031 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopT_T_EPDv01.root 0.0000107482 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopS_T_EPDv01.root 0.00001227 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopTW_T_EPDv01.root 0.000009895 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopT_Tbar_EPDv01.root 0.000011646 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopS_Tbar_EPDv01.root 0.000010436 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microSTopTW_Tbar_EPDv01.root 0.000009992 ENDSET    Diboson 4 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWW_EPDv01.root 0.0000101753087377979123 /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWZ_EPDv01.root 0.00000422";
//  int nQCD=-10;

// ///With QCD:
   TString inModes_mu = "Data 1 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSingleMu_Data_EPDv01.root 5020 ENDSET   QCD 6 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microQCD_Mu_EPDv01.root 0.001625 ENDSET     WpJ 2 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWJets_EPDv01.root 0.0003849171 ENDSET    ZpJ 3 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microZJets_EPDv01.root 0.000084017952387 ENDSET    Top 5 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microTTbar_EPDv01.root 0.000044031 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopT_T_EPDv01.root 0.0000107482 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopS_T_EPDv01.root 0.00001227 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopTW_T_EPDv01.root 0.000009895 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopT_Tbar_EPDv01.root 0.000011646 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopS_Tbar_EPDv01.root 0.000010436 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopTW_Tbar_EPDv01.root 0.000009992 ENDSET    Diboson 4 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWW_EPDv01.root 0.0000101753087377979123 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWZ_EPDv01.root 0.00000422";


   TString inModes_el = "Data 1 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSingleEl_Data_EPDv01.root 5020 ENDSET   QCD 6 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microQCD_El_EPDv01.root 0.0663 ENDSET     WpJ 2 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWJets_EPDv01.root 0.0003849171 ENDSET    ZpJ 3 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microZJets_EPDv01.root 0.000084017952387 ENDSET    Top 5 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microTTbar_EPDv01.root 0.000044031 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopT_T_EPDv01.root 0.0000107482 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopS_T_EPDv01.root 0.00001227 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopTW_T_EPDv01.root 0.000009895 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopT_Tbar_EPDv01.root 0.000011646 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopS_Tbar_EPDv01.root 0.000010436 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microSTopTW_Tbar_EPDv01.root 0.000009992 ENDSET    Diboson 4 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWW_EPDv01.root 0.0000101753087377979123 /uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWZ_EPDv01.root 0.00000422";
   int nQCD=1;

////Cross-Section/NGenerated
//18.2/4265243=4.22015814808206740e-06 for WZ
//   double STopWeight = 3.19/259971;
//   double STopBarWeight = 1.44/137980;
//   double TTopWeight = 41.92/3900171;
//   double TTopBarWeight = 22.65/1944826;
//   double TWTopWeight = 7.87/795379;
//   double TWTopBarWeight = 7.87/787629;
// QCD Fractions: double rel2jet = 0.0663, rel3jet = 0.0229, rmu2jet = 0.001625, rmu3jet = 0.;


//   TString inModes_mu = "Data /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microMu_All_EPDv01.root 5020 1 WpJ /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWJets_EPDv01.root 0.0003849171 2 ZpJ /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microZJets_EPDv01.root 0.000084017952387 3 TTbar /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microTTbar_EPDv01.root 0.000044031 5 WW /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWW_EPDv01.root 0.0000101753087377979123 4";


//   TString inModes_el = "Data /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microEl_All_EPDv01.root 5020 1 WpJ /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWJets_EPDv01.root 0.0003849171 2 ZpJ /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microZJets_EPDv01.root 0.000084017952387 3 TTbar /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microTTbar_EPDv01.root 0.000044031 5 WW /uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/microWW_EPDv01.root 0.0000101753087377979123 4";

//   ///// SelectionLevelCuts
//   TString cutstrName="SelectionLevelCutsNoQCD_";
//   TString AddRest_mu="( (leptonCat==1)&&(40.0<Mjj)&&(Mjj<300.0) )";
//   TString AddRest_el="( (leptonCat==2)&&(40.0<Mjj)&&(Mjj<300.0) )";

//   ///// StandarCuts
//   TString cutstrName="StandardCuts_";
//   TString AddRest_mu="( (leptonCat==1)&&(40.0<Mjj)&&(Mjj<300.0)&&(abs(lLV[0].Eta())<2.1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>30.0)&&(jLV[1].Pt()>30.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(0.3<(jLV[0].Pt()/Mjj))&&((jLV[0].Pt()/Mjj)<0.7)&&(METLV[0].Et()>30.0) )";
//   TString AddRest_el="( (leptonCat==2)&&(40.0<Mjj)&&(Mjj<300.0)&&(abs(lLV[0].Eta())<2.1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>30.0)&&(jLV[1].Pt()>30.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(0.3<(jLV[0].Pt()/Mjj))&&((jLV[0].Pt()/Mjj)<0.7)&&(METLV[0].Et()>30.0) )";

  ///// Diboson Analysis-like cuts
  TString cutstrName="DibosonAnaLikeCuts_";
  //TString AddRest_mu="( (leptonCat==1)&&(40.0<Mjj)&&(Mjj<300.0)&&(abs(lLV[0].Eta())<2.1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>35.0)&&(jLV[1].Pt()>35.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>20.0)&&( (abs(jLV[0].Phi()-METLV[0].Phi())<0.4)||((2*TMath::Pi()-abs(jLV[0].Phi()-METLV[0].Phi()))<0.4) )&&(METLV[0].Et()>25.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>50.0) )";
TString AddRest_mu="( (leptonCat==1)&&(lLV[0].Pt()>25.0)&&(jLV[0].Pt()>35.0)&&(jLV[1].Pt()>35.0)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>20.0)&&( (abs(jLV[0].Phi()-METLV[0].Phi())<0.4)||((2*TMath::Pi()-abs(jLV[0].Phi()-METLV[0].Phi()))<0.4) )&&(METLV[0].Et()>25.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>50.0) )";
//TString AddRest_el="( (leptonCat==2)&&(40.0<Mjj)&&(Mjj<300.0)&&(abs(lLV[0].Eta())<2.5)&&(lLV[0].Pt()>30.0)&&(jLV[0].Pt()>35.0)&&(jLV[1].Pt()>35.0)&&(DRlj1>0.5)&&(DRlj2>0.5)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>20.0)&&( (abs(jLV[0].Phi()-METLV[0].Phi())<0.4)||((2*TMath::Pi()-abs(jLV[0].Phi()-METLV[0].Phi()))<0.4) )&&(METLV[0].Et()>30.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>50.0) )";
TString AddRest_el="( (leptonCat==2)&&(lLV[0].Pt()>35.0)&&(jLV[0].Pt()>35.0)&&(jLV[1].Pt()>35.0)&&(abs(jLV[0].Eta()-jLV[1].Eta())<1.5)&&(sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()))>20.0)&&( (abs(jLV[0].Phi()-METLV[0].Phi())<0.4)||((2*TMath::Pi()-abs(jLV[0].Phi()-METLV[0].Phi()))<0.4) )&&(METLV[0].Et()>30.0)&&(sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()))>50.0) )";



  for (Int_t n=0; n<NVars; n++) {
    ///Muons:
    cout << "SAVING MUON VALIDATION PLOTS" << endl;
    TString SaveName="_mu.png";
    //TString SaveName="_mu.root";
    SaveName=VarTitle[n]+SaveName;
    SaveName=cutstrName+SaveName;
    SaveName=saveDir+SaveName;
    TString Title = VarTitle[n];
    Title = " Muons : " + Title;
    Title = TitleName + Title;
    Plot_Validation(Title,"METree",VarName[n],NBins,VarMin[n],VarMax[n],AddRest_mu,inModes_mu,nQCD,SaveName,scaleMCtoData,drawMCwithErrors);
    ///Electrons:
    cout << "SAVING ELECTRON VALIDATION PLOTS" << endl;
    SaveName="_el.png";
    //SaveName="_el.root";
    SaveName=VarTitle[n]+SaveName;
    SaveName=cutstrName+SaveName;
    SaveName=saveDir+SaveName;
    Title = VarTitle[n];
    Title = " Electrons : " + Title;
    Title = TitleName + Title;
    Plot_Validation(Title,"METree",VarName[n],NBins,VarMin[n],VarMax[n],AddRest_el,inModes_el,nQCD,SaveName,scaleMCtoData,drawMCwithErrors);
  }

}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Plot_StoreHist(const char* TreeName, const char* VarName, int NBins, double VarMin, double VarMax, const char* inmodefile=0, const char* mode_name=0, const char* AddRest=0, const char* StoreHistFileName=0 )
//// Plots a histogram (of VarName) to be used by MCLimit.  
{
     
  TFile* f = new TFile(inmodefile);
  TTree* InTree = (TTree*)f->Get(TreeName);

 
  TFile * outhistfile;
  if ( StoreHistFileName!=0) {
    outhistfile = new TFile(StoreHistFileName, "RECREATE");
  } 
  TH1F* EvtHist = new TH1F("EvtHist","EvtHist",NBins,VarMin,VarMax);

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

// -----------------------------------------------------------------------------------------------------------------------------------------------------------//


void Save_YieldsAndPulls(const char* TreeName, int NBins, const char* infoFileName, const char* savedirName, const char* inModes, const char* TitleSuffix="", const char* PlotNameSuffix=0)
//// inModes should be of the form "VN[0] dummystr[0] dummyEvt[0] color[0] VN[1] dummystr[1] dummyEvt[1] color[1] VN[2] ... " 
//// 1. Plot&Fit numVN-initVN for each variable (VN=VariableName). The Title is of the form "VN_{Observed}-VN_{Expected} TitleSuffix"; saved as "savedirNameVNYield_PlotNameSuffix".
//// 2. Plot&Fit (numVN-initVN)/errVN for each variable. The Title is of the form "VN Pull TitleSuffix"; saved as "savedirNameVNPull_PlotNameSuffix".
{
     

  vector<TString> VN, dummystr;
  vector<double> meanEvt;
  vector<int> color;
  int nModes = 0;

  TString Var;
  TString initVN, numVN, errVN;
  TString Title, SaveName;
  double minNum, maxNum, minErr;
  double minInit, maxInit, avgInit;
  double VarMin, VarMax;
  //char ToChar[10];

  readModes(inModes,VN,dummystr,meanEvt,color,nModes);
  TFile* f = new TFile(infoFileName);
  TTree* InTree = (TTree*)f->Get(TreeName);
 
 
  for (Int_t n=0; n<nModes; n++) {
    initVN="init"+VN[n];
    numVN="num"+VN[n];
    errVN="err"+VN[n];
    minNum=InTree->GetMinimum(numVN);
    maxNum=InTree->GetMaximum(numVN);
    minInit=InTree->GetMinimum(initVN);
    maxInit=InTree->GetMaximum(initVN);
    avgInit=(minInit+maxInit)/2;
    minErr=InTree->GetMinimum(errVN);
    ///Draw the Yield_Observed-Yield_Expected
    Title=TitleSuffix;
    Title="_{Expected} " +Title;
    Title=VN[n]+Title;
    Title="_{Observed}-" +Title;
    Title=VN[n]+Title;
    SaveName=PlotNameSuffix;
    SaveName="Yield_"+SaveName;
    SaveName=VN[n]+SaveName;
    SaveName=savedirName+SaveName;
    VarMin=floor(minNum-avgInit);
    VarMax=ceil(maxNum-avgInit);
    //sprintf(ToChar,"%i",int(meanEvt[n]));
    Var=initVN;
    Var="-"+Var;
    Var=numVN+Var;
    Plot_ABkg(Title,TreeName,Var,NBins,VarMin,VarMax,infoFileName,0,SaveName,color[n],0,"NToys",10,true,false);
    ///Draw the Pulls
    Title=TitleSuffix;
    Title=" Pull " +Title;
    Title=VN[n]+Title;
    SaveName=PlotNameSuffix;
    SaveName="Pull_"+SaveName;
    SaveName=VN[n]+SaveName;
    SaveName=savedirName+SaveName;
    VarMin=(minNum-avgInit)/minErr;
    VarMax=(maxNum-avgInit)/minErr;
    //sprintf(ToChar,"%i",int(meanEvt[n]));
    Var=errVN;
    Var=")/"+Var;
    Var=initVN+Var;
    Var="-"+Var;
    Var=numVN+Var;
    Var="("+Var;
    cout << "Var=" << Var << endl;
    Plot_ABkg(Title,TreeName,Var,NBins,VarMin,VarMax,infoFileName,0,SaveName,color[n],0,"NToys",10,true,false);
  }

}

