#include <stddef.h>
#include "TFile.h"
#include "TROOT.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include "mclimit_csm.h"
#include "mclimit_csm.C"
#include "TRandom3.h"
#include <math.h>

using namespace std;

int main(int argc, char **argv)
{

  char *snlist[1];
  double bguperr[1] = {0.25};
  double bgdownerr[1] = {-0.25};
  double bgupshapesigma[1] = {0};
  double bgdownshapesigma[1] = {0};
  TH1* bgdownshape[1] = {0};
  TH1* bgupshape[1] = {0};
  char *sname1;
  // = "SYSNAME1";
  char sname1_name[]="SYSNAME1";
  sname1=sname1_name;

  double bsf=1.0;
  double ssf=1.0;
  int pflag=0;
  int nnp=1;

  delete gRandom;
  gRandom = new TRandom3();

  gRandom->SetSeed(567);

  TH1 *bghist1 = (TH1*) new TH1F("bg1","bg1",1,0,1);
  TH1 *sighist1 = (TH1*) new TH1F("sg1","sg1",1,0,1);
  TH1 *dh1 = (TH1*) new TH1F("dh1","dh1",1,0,1);

  bghist1->SetBinContent(1,.5);
  dh1->SetBinContent(1,1);
  sighist1->SetBinContent(1,1);

  csm_model *testhyp_pe = new csm_model;
  csm_model *nullhyp_pe = new csm_model;
  csm_model *testhyp = new csm_model;
  csm_model *nullhyp = new csm_model;
  mclimit_csm *limitcalc = new mclimit_csm; 


  snlist[0] = sname1;
		  testhyp_pe->add_template(bghist1,
					   bsf,
					   nnp,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   0,
					   "chan1");
		  nullhyp_pe->add_template(bghist1,
					   bsf,
					   nnp,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   0,
					   "chan1");

		  testhyp->add_template(bghist1,
					   bsf,
					   0,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   0,
					   "chan1");
		  nullhyp->add_template(bghist1,
					   bsf,
					   0,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   0,
					   "chan1");



		  testhyp->add_template(sighist1,
					   ssf,
					   0,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   1,
					   "chan1");
		  testhyp_pe->add_template(sighist1,
					   ssf,
					   0,
					   snlist,
					   bgdownerr, 
					   bguperr,
					   bgdownshape,
					   bgdownshapesigma,
					   bgupshape,
					   bgupshapesigma,
					   pflag,
					   1,
					   "chan1");

		  char chan1_name[]="chan1";
	      limitcalc->set_datahist(dh1,chan1_name);

      limitcalc->set_null_hypothesis(nullhyp);
      limitcalc->set_test_hypothesis(testhyp);
      limitcalc->set_null_hypothesis_pe(nullhyp_pe);
      limitcalc->set_test_hypothesis_pe(testhyp_pe);

      limitcalc->set_npe(100000);  // raise to 100000 for a real test
      double sflimit,sfunc;//,sm2,sm1,smed,sp1,sp2;

      limitcalc->bayes_heinrich(0.95,&sflimit,&sfunc);
      cout <<  sflimit << endl;
      //limitcalc->setpxprintflag(1);

      limitcalc->set_npe(1000);  // raise to 100000 for a real test

      limitcalc->run_pseudoexperiments();

      cout << "CLs, 1-CLb: " << limitcalc->cls() << " " << limitcalc->omclb() << endl;
      cout << "cls exp bg 1: " << limitcalc->clsexpbm2() << " " << limitcalc->clsexpbm1() << " " 
	   << limitcalc->clsexpbmed() << " " << limitcalc->clsexpbp1() << " " << limitcalc->clsexpbp2() << endl;
      cout << "cls exp bg 2: " << limitcalc->gclsexpbm2() << " " << limitcalc->gclsexpbm1() << " " 
	   << limitcalc->gclsexpbmed() << " " << limitcalc->gclsexpbp1() << " " << limitcalc->gclsexpbp2() << endl;
 
      cout << endl;
      //      cout << "s95med: " << limitcalc->s95med() << endl;

      csm *mycsm = new csm();
      mycsm->setminosflag(1);
      mycsm->set_htofit(dh1,chan1_name);
      mycsm->set_modeltofit(nullhyp_pe);
      double chisq = mycsm->chisquared();
      cout << "Chisquared: " << chisq << endl;

      // comment out for now:
//       limitcalc->set_npe(1000000);  // raise to 100000 for a real test
//       double mcmclimit = limitcalc->bayeslimit_mcmc1(0.95,flat,"testout.root");
//       cout << "Markov Chain limit: " << mcmclimit << endl;
      delete mycsm;
      delete nullhyp_pe;
      delete nullhyp;
      delete testhyp_pe;
      delete testhyp;
      delete limitcalc;
    }

