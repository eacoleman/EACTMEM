PlotMassScan(){

  TFile * file = TFile::Open("test.root");

  //define the number of points per graph
  static const int NPOINTS = 10;

  double me[NPOINTS], me_err[NPOINTS], me_err2[NPOINTS];
  
  // Initialize
  for (int po=0;po< NPOINTS; po++)
    me[po]= me_err2[po] = me_err[po] = 0;

  double mass[NPOINTS];
  for (int m=0; m<NPOINTS; m++)
    mass[m] = 100 + 5* m;


  //Loop over events
  for (int ev =0; ev<1000;ev++){

    char graph_name[500];
    sprintf(graph_name,"MassProfile0_%i",ev);
    TGraphErrors * ge = (TGraphErrors*) (file->Get(graph_name));

    
    for (int po = 0; po < ge->GetN() || po < NPOINTS; po++){
      double x,y, xerr,yerr;
      ge->GetPoint(po,x,y);
      me[po] += y;     
    }//for

  }//loop over events

  TGraph * gr = new TGraph(NPOINTS,mass,me);
  gr->Draw("AL");

}
