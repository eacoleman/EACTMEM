// This class libraries
#include "TAMUWW/Tools/interface/CutOptimization.hh"

//______________________________________________________________________________
CutOptimization::CutOptimization() {

}//C'tor

//______________________________________________________________________________
CutOptimization::~CutOptimization() {

}//D'tor

//______________________________________________________________________________
void CutOptimization::drawSaveProjections(TString ofile, vector<bool> logScales) {
   TFile* of = new TFile(ofile,"RECREATE");

   gStyle->SetOptStat(0);

   TCanvas* c = new TCanvas("FOMProjections","FOMProjections",1200,800);
   c->Divide(3,2);
   
   for(int a=0; a<fomContainer->GetNaxis(); a++) {
      c->cd(a+1)->SetLogx(logScales[a]);
      TH1F* temp = fomContainer->Get1DProjection(a,maxFOMBin);
      temp->GetYaxis()->SetTitle("FOM");
      temp->Draw();
      temp->Write();
   }
   c->Write();
   of->Close();

   fomContainer->WriteToFile(ofile,"UPDATE");
   signal->WriteToFile(ofile,"UPDATE");
   background->WriteToFile(ofile,"UPDATE");
}

//______________________________________________________________________________
Double_t CutOptimization::getFOM(Double_t FOM, vector<pair<int,int> > ranges) {
   double sIntegral = 0.0;
   double bIntegral = 0.0;

   //cout << "nSignalAxes=" << signal->GetNaxis() << endl;
   //cout << "nBackgroundAxes=" << background->GetNaxis() << endl;

   if (FOM==1) {
      sIntegral = signal->Integral(ranges);
      bIntegral = background->Integral(ranges);
      /*
      for(unsigned int i=0; i<ranges.size(); i++) {
         cout << "\t\tAxis" << i << " low = " << ranges[i].first << " high = " << ranges[i].second << endl;
      }
      cout << "\t\tsignal = " << sIntegral << " background = " << bIntegral << endl;
      */
      if(bIntegral!=0)
         return sIntegral/TMath::Sqrt(bIntegral);
      else {
         //cout << "CutOptimization::getFOM::Warning:: background integral is zero. Returning zero for FOM1." << endl;
         return 0;
      }
   }
   else if (FOM==1.5) {
/*
      double fom1 = 0.0;
      for(int i=1; i<=HistogramsFitter::backgroundHistogram->GetNbinsX(); i++)
         fom1 += TMath::Power((HistogramsFitter::signalHistogram->GetBinContent(i)/TMath::Sqrt(HistogramsFitter::backgroundHistogram->GetBinContent(i))),2);
      return TMath::Sqrt(fom1);
*/
      return 0.0;
   }
   else if (FOM==2) {
      sIntegral = signal->Integral(ranges);
      bIntegral = background->Integral(ranges);
      if((bIntegral+sIntegral)!=0)
         return sIntegral/TMath::Sqrt(sIntegral+bIntegral);
      else {
         //cout << "CutOptimization::getFOM::Warning:: signal+background integral is zero. Returning zero for FOM2." << endl;
         return 0;
      }
   }
   else if (FOM==2.5) {
/*
      double fom2 = 0.0;
      for(int i=1; i<=HistogramsFitter::backgroundHistogram->GetNbinsX(); i++)
         fom2 += TMath::Power((HistogramsFitter::signalHistogram->GetBinContent(i)/TMath::Sqrt(HistogramsFitter::signalHistogram->GetBinContent(i)+HistogramsFitter::backgroundHistogram->GetBinContent(i))),2);
      return TMath::Sqrt(fom2);
 */
      return 0.0;
   }
   else if (FOM==3) {
      //return FigureOfMerit::usingChi2(HistogramsFitter::signalHistogram,
      //                                HistogramsFitter::backgroundHistogram,0.0001);
      return 0.0;
   }
   else if (FOM==4) {
      //return FigureOfMerit::usingShapeFromTemplates(HistogramsFitter::signalHistogram,
      //HistogramsFitter::backgroundHistogram,0.0001);
      return 0.0;
   }
   else {
      cout << "CutOptimization::getFOM::WARNING The FOM type was not specified. Using defaul (FOM4)." << endl;
      return 0.0;
   }
}

//______________________________________________________________________________
void CutOptimization::getRanges(int depth, vector<Int_t>& coord) {

   if (depth==0) {
      vector<pair<int,int> > rangeND;
      for(int a=0; a<signal->GetNaxis(); a++) {
         if(cuts[a].CompareTo("lt")==0)
            rangeND.push_back(make_pair(1,coord[a]));
         else if(cuts[a].CompareTo("gt")==0)
            rangeND.push_back(make_pair(coord[a],fomContainer->GetAxis(a)->GetNbins()));
         else {
            Error("getRanges",Form("Attempt to find range for axis %d without knowing the cut (gt or lt)",a));
            return;
         }
      }
      ranges.push_back(rangeND);
   }
   else {
      for (int fRange = 1; fRange<=signal->GetAxis(depth-1)->GetNbins(); fRange++) {
         coord[depth-1] = fRange;
         getRanges(depth-1, coord);
      }
   }

}

//______________________________________________________________________________
void CutOptimization::getMaxFOMAndBin() {
   maxFOM = fomContainer->GetMaximum();
   maxFOMBin = fomContainer->GetBins(fomContainer->GetMaximumBin());

   cout << "CutOptimization::getMaxFOMAndBin::The maximum FOM is " << maxFOM << " at bin (";
   for(unsigned int i=0; i<maxFOMBin.size(); i++) {
      cout << maxFOMBin[i];
      if(i!=maxFOMBin.size()-1)
         cout << ",";
      else
         cout << ")" << endl;
   }
}

//______________________________________________________________________________
void CutOptimization::loopOverCuts(Double_t FOM, TString option) {

   option.ToLower();

   vector<int> bins(signal->GetNaxis(),0);
   getRanges(signal->GetNaxis(),bins);

   cout << "CutOptimization::loopOverCuts::There are " << ranges.size() << " cut combinations to try." << endl
        << "CutOptimization::loopOverCuts::Beginning to loop..." << endl;

   vector<double> coord(signal->GetNaxis(),0);
   for(unsigned int r=0; r<ranges.size(); r++) {
      if(option.CompareTo("debug")==0)
         cout << "\t\tRange " << r << endl;
      if(r%10000==0 && option.CompareTo("debug")!=0) 
         cout << "\t\t" << r << endl;
      for(unsigned int c=0; c<ranges[r].size(); c++) {
         if(option.CompareTo("debug")==0)
            cout << "\t\t\tAxis " << c << "\t(" << ranges[r][c].first << "," << ranges[r][c].second << ")" << endl;
         if(cuts[c].CompareTo("lt")==0)
            coord[c] = fomContainer->GetAxis(c)->GetBinCenter(ranges[r][c].second);
         else if(cuts[c].CompareTo("gt")==0)
            coord[c] = fomContainer->GetAxis(c)->GetBinCenter(ranges[r][c].first);
         else {
            Error("getRanges",Form("Attempt to find range for axis %d without knowing the cut (gt or lt)",c));
            return;
         }
      }

      fomContainer->Fill(coord,getFOM(FOM,ranges[r]));
   }

   cout << "CutOptimization::loopOverCuts::Done looping over cuts." << endl;
}

//______________________________________________________________________________
TProfileMDF* CutOptimization::readAndSumTProfileMDF(TString ifile, vector<TString> treeNames, TString profNameTitle) {
   TProfileMDF* sum = new TProfileMDF(profNameTitle,profNameTitle);

   for(unsigned int i=0; i<treeNames.size(); i++) {
      TProfileMDF* temp = readInTProfileMDF(ifile,treeNames[i]);
      if(i==0)
         sum->Add(sum,temp,1,1);
      else
         sum->Add(sum,temp,1,1,true);
      delete temp;
   }

   //cout << "nSumAxes=" << sum->GetNaxis() << endl;

   return sum;
}

//______________________________________________________________________________
TProfileMDF* CutOptimization::readInTProfileMDF(TString ifile, TString treeName) {
   TProfileMDF* temp = new TProfileMDF("temp","temp");
   temp->ReadFromFile(ifile,treeName);
   return temp;
}

//______________________________________________________________________________
void CutOptimization::setFOMContainer() {
   fomContainer = new TProfileMDF("fomContainer","fomContainer");
   
   for(int i=0; i<signal->GetNaxis(); i++) {
      fomContainer->AddAxis(signal->GetAxis(i));
   }

   fomContainer->Sumw2();
}
