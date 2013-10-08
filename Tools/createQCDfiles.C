int createQCDfiles(){

  double vabseta[] = { +0.000, 
		       ,+0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783
		       ,+0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566 
		       ,+1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650 
		       ,+2.853, +2.964, +3.139, +3.314, +3.489, +3.664, +3.839, +4.013, +4.191 
		       ,+4.363, +4.538, +4.716, +4.889, +5.191};
  
  TString metCut = "METLV.fP.Pt() > 25";


  // SIGNAL
  //TFile *_file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20130505_FullIsoMVA/Full.root");
  TFile * _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20121102_FullIsoMVA/SingleEl_Full.root");

  PS->cd();
  TTree * jets2p = (TTree *) gDirectory->Get("jets2p");
  TFile * fout = new TFile("output_signal.root","RECREATE");
  TH2D * h2 = new TH2D("h2","h2",41,vabseta,500,0,500);
  jets2p->Draw("METLV.fP.Pt():abs(lLV[0].fP.Eta()) >> h2",metCut + " && lLV[0].lpfIso < 0.2 && lLV[0].emvaTrig>0.95");
  h2->Write();
  fout->Close();

  // QCD - MVA cut
  //  _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20130505_FullIsoMVA/Full.root");
  _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20121102_FullIsoMVA/SingleEl_Full.root");
  PS->cd();
  jets2p = (TTree *) gDirectory->Get("jets2p");
  fout = new TFile("output_mvacutnew.root","RECREATE");
  h2 = new TH2D("h2","h2",41,vabseta,500,0,500);
  jets2p->Draw("METLV.fP.Pt():abs(lLV[0].fP.Eta()) >> h2","METLV.fE > 25 && lLV[0].lpfIso > 0.3 && lLV[0].emvaTrig>0.95");
  h2->Write();
  fout->Close();
  
  // QCD no MVA
  //  _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20130505_FullIsoMVA/Full.root");
  _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20121102_FullIsoMVA/SingleEl_Full.root");
  PS->cd();
  jets2p = (TTree *) gDirectory->Get("jets2p");
  fout = new TFile("output_nomvacutnew.root","RECREATE");
  h2 = new TH2D("h2","h2",41,vabseta,500,0,500);
  jets2p->Draw("METLV.fP.Pt():abs(lLV[0].fP.Eta()) >> h2",metCut + " && lLV[0].lpfIso > 0.3");
  h2->Write();
  fout->Close();

  /*  
  //WJets
  _file0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_20130514_FullIsoMVA_WJets/PS.root");
  PS->cd();
  jets2p = (TTree *) gDirectory->Get("jets2p");
  fout = new TFile("output_wjets.root","RECREATE");
  h2 = new TH2D("h2","h2",41,vabseta,500,0,500);
  jets2p->Draw("METLV.fP.Pt():abs(lLV[0].fP.Eta()) >> h2",metCut + " && lLV[0].lpfIso < 0.2 && lLV[0].emvaTrig>0.95 ");
  h2->Write();
  fout->Close();
  */

}



