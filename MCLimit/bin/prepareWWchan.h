#include "mclimit_csm.h"
#include "mclimit_csm.C"

// common source for preparing t-channel likelihood function
// limit inputs -- using SM with SM single top
// as the null hypothesis and SM with SM single top and extra anomalous
// single top as the test hypothesis

char *ename[15];
//char ename[]="testname";


// char jesname[]="JES";
// char isrname[]="ISR";
// char fsrname[]="FSR";
// char pdfname[]="PDF";
// char luminame[]="LUMI";
// char btagname[]="BTAG";
// char wbbname[]="WBB";
// char wccname[]="WBB";  // Take these to be all correlated for now -- 38% Method 2
// char mistagname[]="MISTAG";
// char nonwname[]="NONW";
// char nonwflavname[]="NONWFLAV";
// char nonwantiname[]="NONWANTI";
// char bsvname[]="BSV";
// char ttbarname[]="TTBAR";
// char qsquaredname[]="QSQUARED";
// char drjjname[]="DRJJRW";
// char phxename[]="PHXE";
// char etaj2name[]="ETAJ2";

double nps_low[15];
double nps_high[15];
double lowsigma[15];
double highsigma[15];
int i;
double sfact;
//int isyst;

TH1 *lowshape[15];
TH1 *highshape[15];

//-----------------------

csm_model* nullhyp = new csm_model();
csm_model* testhyp = new csm_model();
// csm_model* nullhyp_pe = new csm_model();
// csm_model* testhyp_pe = new csm_model();

/* first index:  
   0:  ttbar 
   1:  wbb
   2:  tchan
   3:  schan
   4:  data
   5:  wc(c)
   6:  mistags
   7:  ww
   8:  wz
   9:  zz
   10: nonw
   11: Z+jets

   Second index:

   0: central value
   1: isr less
   2: isr more
   3: fsr less
   4: fsr more
   5: jes minus
   6: jes plus
   7: bsv opt
   8: bsv pes   (symmetrized)
   9: qsquared +variation
   10: qsquared -variation 
   11: nonW flav+
   12: nonW flav-
   13: nonW jet-electrons
   14: nonW anti-electrons
   15: DRJJ reweight
   16: DRJJ symmetrized
   17: PHXE reweight
   18: PHXE symmetrized
   19: ETAJ2 reweight
   20: ETAJ2 symmetrized
*/


TH1F* histWpJ;
TH1F* histWW;

cout << "reading histos" << endl;

TFile *infileWpJ = (TFile*) new TFile("WpJ_MjjHistFile.root");
histWpJ  = (TH1F*)  infileWpJ->Get("EvtHist;1")->Clone("WpJcent");

TFile *infileWW = (TFile*) new TFile("WW_MjjHistFile.root");
histWW  = (TH1F*)  infileWW->Get("EvtHist;1")->Clone("WWcent");

// TFile *inputWpJ = new TFile("WpJ_MjjHistFile.root");
// histWpJ  = inputWpJ->Get("EvtHist;1")->Clone("WpJcent");

// TFile *inputWW = new TFile("WW_MjjHistFile.root");
// histWW  = inputWW->Get("EvtHist;1")->Clone("WWcent");


// TFile *wglfroot = (TFile*) new TFile("../hbooktmp/ltchan_central.root");
// isyst = 0;
// histo[0][isyst]  = (TH1F*)  wglfroot->Get("h10;1")->Clone("ttcentd");


// normalize syst. variations to the central value and 
// symmetrize errors 

// step 1 -- normalize all syst. variations to unit area
// (isyst=0 = central value)

double histsum;
int nbins;
int ibin;
nbins = histWpJ->GetNbinsX();
histsum = 0;
for (ibin=1;ibin<=nbins;ibin++)
  {
    histsum +=histWpJ->GetBinContent(ibin);
  }

if (histsum>0)
  {
    for (ibin=1;ibin<=nbins;ibin++)
      {
	histWpJ->SetBinContent(ibin,histWpJ->GetBinContent(ibin)/histsum);
      }
  }


nbins = histWW->GetNbinsX();
histsum = 0;
for (ibin=1;ibin<=nbins;ibin++)
  {
    histsum +=histWW->GetBinContent(ibin);
  }

if (histsum>0)
  {
    for (ibin=1;ibin<=nbins;ibin++)
      {
	histWW->SetBinContent(ibin,histWW->GetBinContent(ibin)/histsum);
      }
  }

//-----------------------------------------------------------
// "WpJ background"


cout << "WpJ bg " << endl;

for(i=0;i<15;i++)
{
  nps_low[i] = 0;
  nps_high[i] = 0;
  lowsigma[i] = 0;
  highsigma[i] = 0;
  lowshape[i] = 0;
  highshape[i] = 0;
}

// ename[0] = luminame;
// nps_low[0] = -0.06;
// nps_high[0] = 0.06;

// ename[1] = btagname;
// nps_low[1] = -0.042;
// nps_high[1] = 0.042;

// ename[2] = jesname;
// nps_low[2] = 0.098;
// nps_high[2] = -0.091;
// lowsigma[2] = -1;
// highsigma[2] = 1;
// highshape[2] = histo[0][6];
// lowshape[2] = histo[0][5];

// ename[3] = pdfname;
// nps_low[3] = -0.024;
// nps_high[3] = 0.024;

// ename[4] = ttbarname;
// nps_low[4] = -0.18;
// nps_high[4] = 0.18;

// ename[5] = drjjname;
// lowsigma[5] = -1;
// highsigma[5] = 1;
// highshape[5] = histo[0][15];
// lowshape[5] = histo[0][16];

// ename[6] = etaj2name;
// lowsigma[6] = -1;
// highsigma[6] = 1;
// highshape[6] = histo[0][19];
// lowshape[6] = histo[0][20];

sfact = 1;

// cout << "nullhyp_pe ttbar" <<endl;
// nullhyp_pe->add_template(histo[0][0],sfact,7,ename,nps_low,nps_high,
// 			 lowshape,lowsigma,highshape,highsigma,0,0,"tchan");
// cout << "testhyp_pe ttbar" <<endl;
// testhyp_pe->add_template(histo[0][0],sfact,7,ename,nps_low,nps_high,
// 			 lowshape,lowsigma,highshape,highsigma,0,0,"tchan");
cout << "nullhyp WW" <<endl;
nullhyp->add_template(histWpJ,sfact,0,ename,nps_low,nps_high,
		      lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");
cout << "testhyp WW" <<endl;
testhyp->add_template(histWpJ,sfact,0,ename,nps_low,nps_high,
		      lowshape,lowsigma,highshape,highsigma,0,0,"WWchan");


//-----------------------------------------------------------
// "WW signal"

for(i=0;i<15;i++)
{
  nps_low[i] = 0;
  nps_high[i] = 0;
  lowsigma[i] = 0;
  highsigma[i] = 0;
  lowshape[i] = 0;
  highshape[i] = 0;
}

// skip ISR and FSR shapes -- they are indistinguishable from no shape difference

// ename[0] = isrname;
// nps_low[0] = -0.01;
// nps_high[0] = 0.03;
// //lowsigma[0] = -1;
// //highsigma[0] = 1;
// //lowshape[0] = histo[2][1];
// //highshape[0] = histo[2][2];

// ename[1] = fsrname;
// nps_low[1] = -0.015;
// nps_high[1] = 0.0532;
// //lowsigma[1] = -1;
// //highsigma[1] = 1;
// //lowshape[1] = histo[2][3];
// //highshape[1] = histo[2][4];

// ename[2] = btagname;
// nps_low[2] = -0.042;
// nps_high[2] = 0.042;

// ename[3] = jesname;
// nps_low[3] = -0.008;
// nps_high[3] = 0.003;
// lowsigma[3] = -1;
// highsigma[3] = 1;
// lowshape[3] = histo[2][5];
// highshape[3] = histo[2][6];

// ename[4] = luminame;
// nps_low[4] = -0.06;
// nps_high[4] = 0.06;

// ename[5] = pdfname;
// nps_low[5] = -0.017;
// nps_high[5] = 0.011;

// ename[6] = drjjname;
// lowsigma[6] = -1;
// highsigma[6] = 1;
// highshape[6] = histo[2][15];
// lowshape[6] = histo[2][16];

// ename[7] = etaj2name;
// lowsigma[7] = -1;
// highsigma[7] = 1;
// highshape[7] = histo[2][19];
// lowshape[7] = histo[2][20];

sfact = 1;

// testhyp_pe->add_template(histo[2][0],sfact,8,ename,nps_low,nps_high,
// 			 lowshape,lowsigma,highshape,highsigma,0,1,"tchan");
testhyp->add_template(histWW,sfact,0,ename,nps_low,nps_high,
		      lowshape,lowsigma,highshape,highsigma,0,1,"WWchan");


nullhyp->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);
// nullhyp_pe->set_interpolation_style("tchan",CSM_INTERP_VERTICAL);
testhyp->set_interpolation_style("WWchan",CSM_INTERP_VERTICAL);
// testhyp_pe->set_interpolation_style("tchan",CSM_INTERP_VERTICAL);

/*
  nullhyp->set_interpolation_style("tchan",CSM_INTERP_HORIZONTAL);
  nullhyp_pe->set_interpolation_style("tchan",CSM_INTERP_HORIZONTAL);
  testhyp->set_interpolation_style("tchan",CSM_INTERP_HORIZONTAL);
  testhyp_pe->set_interpolation_style("tchan",CSM_INTERP_HORIZONTAL);
*/

cout << "end of templates" << endl;

