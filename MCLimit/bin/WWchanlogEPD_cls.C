#include <stddef.h>
#include "TFile.h"
#include "TROOT.h"
#include "mclimit_csm.h"
#include "mclimit_csm.C"
#include <iostream>
#include <TString.h>
#include "TH1.h"
#include "TRandom3.h"

using namespace std;

int main(int argc, char **argv)
{

  TString dirPath="/uscms_data/d2/ilyao/MEResults/PAT356FullScan/MCLimit/";
  TString inputstr;
  delete gRandom;
  gRandom = (TRandom*) new TRandom3;

  char *ename[15];

  char WWVarName[]="WWVar";
  char WZVarName[]="WZVar";

  char WpJVarName[]="WpJVar";
  char ZpJVarName[]="ZpJVar";
  //  char ZZVarName[]="ZZVar";
  char TTbarVarName[]="TTbarVar";
  char STopTVarName[]="STopTVar";
  char STopSVarName[]="STopSVar";
  //  char STopTWVarName[]="STopTWVar";
  char QCDHT100to250VarName[]="QCDHT100to250Var";
  char QCDHT250to500VarName[]="QCDHT250to500Var";





  double nps_low[15];
  double nps_high[15];
  double lowsigma[15];
  double highsigma[15];
  int i;
  double sfact=1;
  //int isyst;

  TH1 *lowshape[15];
  TH1 *highshape[15];

  //-----------------------

  csm_model* nullhyp = new csm_model();
  csm_model* testhyp = new csm_model();
  csm_model* nullhyp_pe = new csm_model();
  csm_model* testhyp_pe = new csm_model();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////***** Define The Histograms *****///////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  cout << "reading histos" << endl;
  ///***Set The Luminosity in pb^-1
  double Lumi=50;
  
  ///event counts for 1pb^-1
  double scale_WW=1.43;
  scale_WW=Lumi*scale_WW;
  double scale_WZ=0.365;
  scale_WZ=Lumi*scale_WZ;

  double scale_WpJ=101;
  scale_WpJ=Lumi*scale_WpJ;
  double scale_ZpJ=6.13;
  scale_ZpJ=Lumi*scale_ZpJ;
//   double scale_ZZ=0.028;
//   scale_ZZ=Lumi*scale_ZZ;
  double scale_TTbar=4.75;
  scale_TTbar=Lumi*scale_TTbar;
  double scale_STopT=5.38;
  scale_STopT=Lumi*scale_STopT;
  double scale_STopS=0.49;
  scale_STopS=Lumi*scale_STopS;
//   double scale_STopTW=0.61;
//   scale_STopTW=Lumi*scale_STopTW;
  double scale_QCDHT100to250=8.4;
  scale_QCDHT100to250=Lumi*scale_QCDHT100to250;
  double scale_QCDHT250to500=1.67;
  scale_QCDHT250to500=Lumi*scale_QCDHT250to500;

  TH1F* histWW;
  TH1F* histWZ;

  TH1F* histWpJ;
  TH1F* histZpJ;
  //  TH1F* histZZ;
  TH1F* histTTbar;
  TH1F* histSTopT;
  TH1F* histSTopS;
  //  TH1F* histSTopTW;
  TH1F* histQCDHT100to250;
  TH1F* histQCDHT250to500;

  inputstr=dirPath+"WW_logEPDWWandWZHistFile.root";
  TFile *infileWW = (TFile*)new TFile(inputstr);
  histWW  = (TH1F*)infileWW->Get("EvtHist;1")->Clone("WWcent");
  histWW->Scale(scale_WW);
  inputstr=dirPath+"WZ_logEPDWWandWZHistFile.root";
  TFile *infileWZ = (TFile*)new TFile(inputstr);
  histWZ  = (TH1F*)infileWZ->Get("EvtHist;1")->Clone("WZcent");
  histWZ->Scale(scale_WZ);


  inputstr=dirPath+"WpJ_logEPDWWandWZHistFile.root";
  TFile *infileWpJ = (TFile*) new TFile(inputstr);
  histWpJ  = (TH1F*)infileWpJ->Get("EvtHist;1")->Clone("WpJcent");
  histWpJ->Scale(scale_WpJ);
  inputstr=dirPath+"ZpJ_logEPDWWandWZHistFile.root";
  TFile *infileZpJ = (TFile*) new TFile(inputstr);
  histZpJ  = (TH1F*)infileZpJ->Get("EvtHist;1")->Clone("ZpJcent");
  histZpJ->Scale(scale_ZpJ);
//   TFile *infileZZ = (TFile*) new TFile("ZZ_logEPDWWandWZHistFile.root");
//   histZZ  = (TH1F*)infileZZ->Get("EvtHist;1")->Clone("ZZcent");
//   histZZ->Scale(scale_ZZ);
  inputstr=dirPath+"TTbar_logEPDWWandWZHistFile.root";
  TFile *infileTTbar = (TFile*) new TFile(inputstr);
  histTTbar  = (TH1F*)infileTTbar->Get("EvtHist;1")->Clone("TTbarcent");
  histTTbar->Scale(scale_TTbar);
  inputstr=dirPath+"STopT_logEPDWWandWZHistFile.root";
  TFile *infileSTopT = (TFile*) new TFile(inputstr);
  histSTopT  = (TH1F*)infileSTopT->Get("EvtHist;1")->Clone("STopTcent");
  histSTopT->Scale(scale_STopT);
  inputstr=dirPath+"STopS_logEPDWWandWZHistFile.root";
  TFile *infileSTopS = (TFile*) new TFile(inputstr);
  histSTopS  = (TH1F*)infileSTopS->Get("EvtHist;1")->Clone("STopScent");
  histSTopS->Scale(scale_STopS);
//   TFile *infileSTopTW = (TFile*) new TFile("STopTW_logEPDWWandWZHistFile.root");
//   histSTopTW  = (TH1F*)infileSTopTW->Get("EvtHist;1")->Clone("STopTWcent");
//   histSTopTW->Scale(scale_STopTW);
  //// Use QCD without a MET Cut (in order to get a sufficient event count)
  inputstr=dirPath+"QCDHT100to250_NoIsoCut_logEPDWWandWZHistFile.root";
  TFile *infileQCDHT100to250 = (TFile*) new TFile(inputstr);
  histQCDHT100to250  = (TH1F*)infileQCDHT100to250->Get("EvtHist;1")->Clone("QCDHT100to250cent");
  histQCDHT100to250->Scale(scale_QCDHT100to250);
  inputstr=dirPath+"QCDHT250to500_NoIsoCut_logEPDWWandWZHistFile.root";
  TFile *infileQCDHT250to500 = (TFile*) new TFile(inputstr);
  histQCDHT250to500  = (TH1F*)infileQCDHT250to500->Get("EvtHist;1")->Clone("QCDHT250to500cent");
  histQCDHT250to500->Scale(scale_QCDHT250to500);


  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////***** Add The Backgrounds *****/////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------
  // "WpJ background"
  cout << "WpJ bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = WpJVarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histWpJ,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histWpJ,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histWpJ,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histWpJ,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

  //-----------------------------------------------------------
  // "ZpJ background"
  cout << "ZpJ bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = ZpJVarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histZpJ,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histZpJ,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histZpJ,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histZpJ,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

//   //-----------------------------------------------------------
//   // "ZZ background"
//   cout << "ZZ bg " << endl;
//   for(i=0;i<15;i++) {
//     nps_low[i] = 0;
//     nps_high[i] = 0;
//     lowsigma[i] = 0;
//     highsigma[i] = 0;
//     lowshape[i] = 0;
//     highshape[i] = 0;
//   }
  
//   ename[0] = ZZVarName;
//   nps_low[0] = -0.2;
//   nps_high[0] = 0.2;

//   sfact = 1;

//   nullhyp_pe->add_template(histZZ,sfact,1,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   testhyp_pe->add_template(histZZ,sfact,1,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   nullhyp->add_template(histZZ,sfact,0,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   testhyp->add_template(histZZ,sfact,0,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");


  //-----------------------------------------------------------
  // "TTbar background"
  cout << "TTbar bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = TTbarVarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histTTbar,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histTTbar,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histTTbar,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histTTbar,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

  //-----------------------------------------------------------
  // "STopT background"
  cout << "STopT bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = STopTVarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histSTopT,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histSTopT,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histSTopT,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histSTopT,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

  //-----------------------------------------------------------
  // "STopS background"
  cout << "STopS bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = STopSVarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histSTopS,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histSTopS,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histSTopS,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histSTopS,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

//   //-----------------------------------------------------------
//   // "STopTW background"
//   cout << "STopTW bg " << endl;
//   for(i=0;i<15;i++) {
//     nps_low[i] = 0;
//     nps_high[i] = 0;
//     lowsigma[i] = 0;
//     highsigma[i] = 0;
//     lowshape[i] = 0;
//     highshape[i] = 0;
//   }
  
//   ename[0] = STopTWVarName;
//   nps_low[0] = -0.2;
//   nps_high[0] = 0.2;

//   sfact = 1;

//   nullhyp_pe->add_template(histSTopTW,sfact,1,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   testhyp_pe->add_template(histSTopTW,sfact,1,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   nullhyp->add_template(histSTopTW,sfact,0,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
//   testhyp->add_template(histSTopTW,sfact,0,ename,nps_low,nps_high,
// 			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

  //-----------------------------------------------------------
  // "QCDHT100to250 background"
  cout << "QCDHT100to250 bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = QCDHT100to250VarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histQCDHT100to250,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histQCDHT100to250,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histQCDHT100to250,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histQCDHT100to250,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");

  //-----------------------------------------------------------
  // "QCDHT250to500 background"
  cout << "QCDHT250to500 bg " << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }
  
  ename[0] = QCDHT250to500VarName;
  nps_low[0] = -0.2;
  nps_high[0] = 0.2;

  sfact = 1;

  nullhyp_pe->add_template(histQCDHT250to500,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp_pe->add_template(histQCDHT250to500,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  nullhyp->add_template(histQCDHT250to500,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
  testhyp->add_template(histQCDHT250to500,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");



  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////***** Add The Signal *****//////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------
  // "WW signal"
  cout << "WW signal" << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }

  ename[0] = WWVarName;
  nps_low[0] = -0.1;
  nps_high[0] = 0.1;
  sfact = 1;

  testhyp_pe->add_template(histWW,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,1,"WWchan");
  testhyp->add_template(histWW,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,1,"WWchan");

  //-----------------------------------------------------------
  // "WZ signal"
  cout << "WZ signal" << endl;
  for(i=0;i<15;i++) {
    nps_low[i] = 0;
    nps_high[i] = 0;
    lowsigma[i] = 0;
    highsigma[i] = 0;
    lowshape[i] = 0;
    highshape[i] = 0;
  }

  ename[0] = WZVarName;
  nps_low[0] = -0.1;
  nps_high[0] = 0.1;
  sfact = 1;

  testhyp_pe->add_template(histWZ,sfact,1,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,1,"WWchan");
  testhyp->add_template(histWZ,sfact,0,ename,nps_low,nps_high,
			lowshape,lowsigma,highshape,highsigma,0,1,"WWchan");

  //-----------------------------------------------------------
  nullhyp->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);
  nullhyp_pe->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);
  testhyp->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);
  testhyp_pe->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);


  cout << "end of templates" << endl;


  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////***** Compute The Limits *****//////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  double bh_CL=0.95;//default bayes-heinrich confidence limit
  int npx=500;//default number of PE
  int nPE=200;//default number of points per PE
  if ( argc==4 ) {
    bh_CL=atof(argv[1]);
    npx=atoi(argv[2]);
    nPE=atoi(argv[3]);
  } else {
    cout << "LIMIT COMPUTATION PARAMETERS ARE NOT GIVEN PROPERLY" << endl;
    cout << "Input should be of the form: bh_CL npx nPE" << endl;
    cout << "Setting to default values:" << endl;
  }

  std::cout << "bayes-heinrich confidence limit = " << bh_CL << std::endl;
  std::cout << "# pseudoexperiments: npx = " << npx << std::endl;
  std::cout << "# of random points per PE: nPE = " << nPE << std::endl;


  // get a distribution of chisqure for the different hypotheses
  //??? Is this needed
  mclimit_csm* mymclimit = (mclimit_csm*) new mclimit_csm();
  mymclimit->set_null_hypothesis(nullhyp);
  mymclimit->set_test_hypothesis(testhyp);
  mymclimit->set_null_hypothesis_pe(nullhyp_pe);
  mymclimit->set_test_hypothesis_pe(testhyp_pe);
  mymclimit->set_npe(nPE);
  mymclimit->run_pseudoexperiments();

//   cout << "Getting results" << endl;
//   cout << "tsbm2: " << mymclimit->tsbm2() << endl;
//   cout << "tsbm1: " << mymclimit->tsbm1() << endl;
//   cout << "tsbmed: " << mymclimit->tsbmed() << endl;
//   cout << "tsbp1: " << mymclimit->tsbp1() << endl;
//   cout << "tsbp2: " << mymclimit->tsbp2() << endl;
//   cout << "tssm2: " << mymclimit->tssm2() << endl;
//   cout << "tssm1: " << mymclimit->tssm1() << endl;
//   cout << "tssmed: " << mymclimit->tssmed() << endl;
//   cout << "tssp1: " << mymclimit->tssp1() << endl;
//   cout << "tssp2: " << mymclimit->tssp2() << endl;

//   cout << "CLs -2sigma (bkg): " << mymclimit->clsexpbm2() << endl;
//   cout << "CLs -1sigma (bkg): " << mymclimit->clsexpbm1() << endl;
//   cout << "CLs median  (bkg): " << mymclimit->clsexpbmed() << endl;
//   cout << "CLs +1sigma (bkg): " << mymclimit->clsexpbp1() << endl;
//   cout << "CLs +2sigma (bkg): " << mymclimit->clsexpbp2() << endl;

//   cout << "1-CLb -2sigma (sig): " << mymclimit->omclbexpsm2() << endl;
//   cout << "1-CLb -1sigma (sig): " << mymclimit->omclbexpsm1() << endl;
//   cout << "1-CLb median  (sig): " << mymclimit->omclbexpsmed() << endl;
//   cout << "1-CLb +1sigma (sig): " << mymclimit->omclbexpsp1() << endl;
//   cout << "1-CLb +2sigma (sig): " << mymclimit->omclbexpsp2() << endl;

//   cout << "CLs -2sigma (sig): " << mymclimit->clsexpsm2() << endl;
//   cout << "CLs -1sigma (sig): " << mymclimit->clsexpsm1() << endl;
//   cout << "CLs median  (sig): " << mymclimit->clsexpsmed() << endl;
//   cout << "CLs +1sigma (sig): " << mymclimit->clsexpsp1() << endl;
//   cout << "CLs +2sigma (sig): " << mymclimit->clsexpsp2() << endl;

  mymclimit->bayes_interval_begin = 0;
  mymclimit->bayes_interval_end = 20;
  mymclimit->bayes_interval_step = 0.01;
  double sflimit;
  double unc;
  double sm2,sm1,smed,sp1,sp2;

  std::cout << "BEGIN_PE" << std::endl;
  // use the background as 'data' in order to run the code
  char WWchan_name[]="WWchan";
  mymclimit->set_datahist(histWpJ,WWchan_name);
  mymclimit->bayes_heinrich_withexpect(bh_CL,&sflimit,&unc,npx,&sm2,&sm1,&smed,&sp1,&sp2);
  //mymclimit->bayes_heinrich(bh_CL,&sflimit,&unc);
  std::cout << "END_PE" << std::endl;
  std::cout << "Expected Limit at: " << bh_CL*100 <<" % C.L "<< smed << " Full band: -2SD -1SD MPV +1SD +2SD " << sm2 << " " << sm1 << " " << smed << " " << sp1 << " " << sp2 << std::endl; 
  std::cout << "Observed Limit at: " << bh_CL*100 <<" % C.L "<< sflimit << std::endl; 
//   //from the "simple example"
//   cout << "CLs, 1-CLb: " << mymclimit->cls() << " " << mymclimit->omclb() << endl;
//   cout << "cls exp bg 1: " << mymclimit->clsexpbm2() << " " << mymclimit->clsexpbm1() << " " 
//        << mymclimit->clsexpbmed() << " " << mymclimit->clsexpbp1() << " " << mymclimit->clsexpbp2() << endl;
//   cout << "cls exp bg 2: " << mymclimit->gclsexpbm2() << " " << mymclimit->gclsexpbm1() << " " 
//        << mymclimit->gclsexpbmed() << " " << mymclimit->gclsexpbp1() << " " << mymclimit->gclsexpbp2() << endl;
//   cout << endl;




  delete mymclimit;
}
