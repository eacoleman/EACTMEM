TH1* processFile(TString fname, double scaleFactor){

  static int hcounter=0;

  cout<<"Doing fname="<<fname<<endl;
  TFile *_file2 = TFile::Open(fname);
  PS->cd();
  TString plot =  "lLV[0].lpfIso:lLV[0].fP.Eta()";
  TString hname = Form("h%i",hcounter);
  jets2p->Draw(plot +" >> " + hname+"(24,-3,3,100,0,2)","","qn");
  TH2D *aux = (TH2D*)gDirectory->Get(hname);
  //aux->Scale(scaleFactor);
  TH1D * ss = aux->ProjectionX(hname+"ss",1,10); // signal region 
  TH1D * ni = aux->ProjectionX(hname+"ni",16,100);// nonIso region (use 11
  //cout<<"h="<<hcounter<<" ss="<<ss->GetEntries()<<" ni="<<ni->GetEntries()<<endl;

  TString statis = Form("( %i / %i )",(int)ss->GetEntries(),(int)ni->GetEntries());
  ss->Divide(ni);
  ss->GetYaxis()->SetTitle(TString("N_{signal}/N_{nonIso}")+statis);
  ss->GetXaxis()->SetTitle("#eta");
  
  hcounter++;

  return ss;

}

void QCD_MC_test(){
  
  TString basepath = "/uscms/home/aperloff/nobackup/Summer12ME8TeV/MEInput/";

  vector<TString> fnames;
  vector<double>  scaleFactors;

  fnames.push_back("QCD_EM_20to30.root"  ); scaleFactors.push_back(2.886e+08/3504052.0);
  fnames.push_back("QCD_EM_30to80.root"  ); scaleFactors.push_back(7.433e+07/3308873.0);
  fnames.push_back("QCD_EM_80to170.root" ); scaleFactors.push_back(1.191e+06/3447258.0);
  fnames.push_back("QCD_EM_170to250.root"); scaleFactors.push_back(  30990.0/3169702.0);
  fnames.push_back("QCD_EM_250to350.root"); scaleFactors.push_back(   4250.0/3461112.0);
  fnames.push_back("QCD_EM_350.root"     ); scaleFactors.push_back(    810.0/3408046.0);

  TCanvas * c = new TCanvas("sf","sf", 1200,800);
  c->Divide(fnames.size()/2,2);
  for (int h=0;h<fnames.size();h++){
    c->cd(h+1);
    TH1* aux = processFile(basepath+fnames[h], scaleFactors[h]);
    aux->SetTitle(fnames[h]);
    aux->Draw("E");
  }

}//QCD_MC_test
