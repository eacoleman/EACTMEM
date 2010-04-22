#include "TH1F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include <iostream>

using std::cout;
using std::endl;

TH1F * background, * siggnal;

TH1F* MEWH[20];
TH1F* MEWBB[20];
TH1F* MEWH_old[20];
TH1F* MEWBB_old[20];


Double_t fitFunction(Double_t *x, Double_t *par) {

   Double_t xx = x[0];
   Int_t bin = background->GetXaxis()->FindBin(xx);
   Double_t br = par[1]*siggnal->GetBinContent(bin);
   br += (1.0-par[1])*background->GetBinContent(bin);
   return par[0]* br;
}



TH1F * GetME(TChain * c, int ME_i, char *prefix){

  char histoName[100];
  sprintf(histoName,"ME%i_%s",ME_i,prefix);
  TH1F* MEWH = new TH1F(histoName,histoName,100,-100,0);

  char command[200];
  sprintf(command,"log (m_tProbStat[%i].tEventProb) >> %s",ME_i,histoName);
  c->Draw(command);

  //Normalize
  double inte = MEWH->Integral();
  MEWH->Scale(1.0/inte);

  return MEWH;

}//GetME


TH1F * MakeOnePSE(TH1F * sig, int nsig, TH1F * back, int nback){

  //threw one data PSE  and fill it
  TH1F * data  = (TH1F*) sig ->Clone("data");
  data->Reset();
  data->FillRandom(sig,nsig);
  data->FillRandom(back,nback);

  return data;

}//MakeOnePSE


void DrawME(int me_i){

  MEWH[me_i]->Draw();
  MEWBB[me_i]->Draw("same");

  MEWH_old[me_i]->Draw("same");
  MEWBB_old[me_i]->Draw("same");

}//DrawME

void ThrowPSEME(int me_i, bool use_old=false){

  // ====================================================
  //                       Create a fit
  // ====================================================
  TF1 *ftot = new TF1("fitFunction",fitFunction,-100,0,2);

  //threw one data PSE  and fill it
  if (use_old){
    cout<<"OLD"<<endl;
    siggnal = MEWH_old[me_i];
    background = MEWBB_old[me_i];
  }else{
    cout<<"NEW"<<endl;
    siggnal = MEWH[me_i];
    background = MEWBB[me_i];
  }

  TH1F * Err = new TH1F("Err","Err",1000,0,1);

  for (int pse=0;pse<100;pse++){

    //Get one PSE
    TH1F * data  =  MakeOnePSE(siggnal,1000,background,2000);
    
    //fit it
    data->Fit("fitFunction","b");   
    Err->Fill(ftot->GetParError(1));
    delete data;

  }
  Err->Draw();

}//ThrowPSEME


void PlotME(){

  // ====================================================
  //                       MY NEW ME
  // ====================================================
  TChain *c_WH = new TChain("METree","METree");
  c_WH->Add("results/WH115/WH115_*.root");

  TChain *c_WBB = new TChain("METree","METree");
  c_WBB->Add("results/WBB0p/WBB0p_*.root");

  for (int me_i=3;me_i<21;me_i++){

    MEWH[me_i] = GetME(c_WH,me_i,"WH");
    MEWH[me_i]->SetLineColor(2);
    
    MEWBB[me_i] = GetME(c_WBB,me_i,"WBB");

  }


  // ====================================================
  //                       OLD  ME
  // ====================================================
  TChain *c_WH_old = new TChain("METree","METree");
  c_WH_old->Add("/cdf/disk01/g6/caf_ICHEP2008/MicroNtuples/micro_WH115GeV.root");

  TChain *c_WBB_old = new TChain("METree","METree");
  c_WBB_old->Add("/cdf/disk01/g6/caf_ICHEP2008/preliminary_micro/micro_WBB.root");
  for (int me_i=3;me_i<21;me_i++){

    MEWH_old[me_i] = GetME(c_WH_old,me_i,"WHo");
    MEWH_old[me_i]->SetLineColor(2);
    MEWH_old[me_i]->SetLineStyle(2);
    
    MEWBB_old[me_i] = GetME(c_WBB_old,me_i,"WBBo");
    MEWBB_old[me_i]->SetLineStyle(2);

  }

  // ====================================================
  //                       Draw Everything
  // ====================================================
  DrawME(3);


}//PlotME



