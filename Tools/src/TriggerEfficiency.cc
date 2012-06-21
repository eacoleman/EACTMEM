// This code' libraries
#include "TAMUWW/Tools/interface/TriggerEfficiency.hh"
#include "TAMUWW/SpecialTools/interface/Defs.hh"

//C++ libraries
#include <iostream>
#include <cmath>

//Root libraries
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"

using std::cout;
using std::endl;
using std::string;

//------------------------------------------------------------------------
bool TriggerEfficiency::addEfficiency(DEFS::LeptonCat leptCat){

  string basePath = gSystem->pwd();
  basePath = basePath.substr(0,basePath.find("TAMUWW"));
  TFile* effFile = new TFile(TString(basePath)+"/TAMUWW/Tools/bin/efficiency1.root","READ");

   if (!effFile->IsOpen()) {
      cout << "ERROR TriggerEfficiency::addEfficiency(). File efficiency1.root not found" << endl;
      return false;
   }

   // Get histogram of trigger efficiencies for muons
   TString histoName = "";
   if(leptCat == DEFS::muon)
     histoName = "muEffs";
   else  if(leptCat == DEFS::electron)
     histoName = "elEffs";

   
   TH2D * aux = (TH2D*)gDirectory->Get(histoName);
   TH2D * effHist  = (TH2D*) aux->Clone(TString("trgEff_")+DEFS::getLeptonCatString(leptCat));
   if (!effHist) {
     cout << "ERROR TriggerEfficiency::addEfficiency(). Efficiency histogram "<<histoName
	  <<" was not successfully retrieved from the file " << effFile->GetName() << "." << endl;
     return false;
   } 

   mapEffs[leptCat ] = effHist;
   return true;

}//addEfficiency


//-----------------------------------------------------------------------
void TriggerEfficiency::createEfficiencies8TeV(){

  double ptbins[8]={20,25,30,35,40,45,50,7000};
  double etabins[5]={-2.5,-1.5,0,1.5,2.5};

  TH2D * h = new TH2D("trgEff_WP80toHLTEle","trgEff_WP80toHLTEle", 7, ptbins, 4, etabins);

  h->SetBinContent(1,1,0.125);                 h->SetBinError(1,1,0.168524);
  h->SetBinContent(1,2,0);                     h->SetBinError(1,2,0);
  h->SetBinContent(1,3,0);                     h->SetBinError(1,3,0);
  h->SetBinContent(1,4,0);                     h->SetBinError(1,4,0.0545844);
  h->SetBinContent(2,1,0.33952);               h->SetBinError(2,1,0.0308978);
  h->SetBinContent(2,2,0.815203);              h->SetBinError(2,2,0.0147795);
  h->SetBinContent(2,3,0.845985);              h->SetBinError(2,3,0.0125865);
  h->SetBinContent(2,4,0.310458);              h->SetBinError(2,4,0.028188);
  h->SetBinContent(3,1,0.611899);              h->SetBinError(3,1,0.00992255);
  h->SetBinContent(3,2,0.865578);              h->SetBinError(3,2,0.00347261);
  h->SetBinContent(3,3,0.861435);              h->SetBinError(3,3,0.0034465);
  h->SetBinContent(3,4,0.585977);              h->SetBinError(3,4,0.00999692);
  h->SetBinContent(4,1,0.697951);              h->SetBinError(4,1,0.00724462);
  h->SetBinContent(4,2,0.890992);              h->SetBinError(4,2,0.0024096);
  h->SetBinContent(4,3,0.881122);              h->SetBinError(4,3,0.00248905);
  h->SetBinContent(4,4,0.669297);              h->SetBinError(4,4,0.00755206);
  h->SetBinContent(5,1,0.725732);              h->SetBinError(5,1,0.00632226);
  h->SetBinContent(5,2,0.900125);              h->SetBinError(5,2,0.00203382);
  h->SetBinContent(5,3,0.895744);              h->SetBinError(5,3,0.00206729);
  h->SetBinContent(5,4,0.702364);              h->SetBinError(5,4,0.00657979);
  h->SetBinContent(6,1,0.734238);              h->SetBinError(6,1,0.00716533);
  h->SetBinContent(6,2,0.910379);              h->SetBinError(6,2,0.00243471);
  h->SetBinContent(6,3,0.901164);              h->SetBinError(6,3,0.00258043);
  h->SetBinContent(6,4,0.700102);              h->SetBinError(6,4,0.00748831);
  h->SetBinContent(7,1,0.75534);               h->SetBinError(7,1,0.00733792);
  h->SetBinContent(7,2,0.917567);              h->SetBinError(7,2,0.00254328);
  h->SetBinContent(7,3,0.909203);              h->SetBinError(7,3,0.00266635);
  h->SetBinContent(7,4,0.728246);              h->SetBinError(7,4,0.00745309);
		     
		     
  mapEffs[DEFS::electron] = h;

  double ptbins_muon[8]={20,25,30,35,40,45,50,7000};
  double etabins_muon[10]={-2.4,-2.1,-1.5,-1,-0.5,0,1,1.5,2.1,2.4};

  TH2D * h_muon = new TH2D("trgEff_muon","trgEff_muon", 7, ptbins_muon, 10, etabins_muon);

  h_muon->SetBinContent(1,1,0);                     h_muon->SetBinError(1,1,0.00111031);    
  h_muon->SetBinContent(1,2,0.246941);              h_muon->SetBinError(1,2,0.0105028);      
  h_muon->SetBinContent(1,3,0.23057);               h_muon->SetBinError(1,3,0.0111005);
  h_muon->SetBinContent(1,4,0.239272);              h_muon->SetBinError(1,4,0.0112622);    
  h_muon->SetBinContent(1,5,0.24288);               h_muon->SetBinError(1,5,0.012521);  
  h_muon->SetBinContent(1,6,0.243736);              h_muon->SetBinError(1,6,0.0122729); 
  h_muon->SetBinContent(1,7,0.232944);              h_muon->SetBinError(1,7,0.0108432); 
  h_muon->SetBinContent(1,8,0.194077);              h_muon->SetBinError(1,8,0.0102964); 
  h_muon->SetBinContent(1,9,0.236158);              h_muon->SetBinError(1,9,0.0104322); 
  h_muon->SetBinContent(1,10,0);                    h_muon->SetBinError(1,10,0.000676062);
  h_muon->SetBinContent(2,1,0);                     h_muon->SetBinError(2,1,0.000676062); 
  h_muon->SetBinContent(2,2,0.742376);              h_muon->SetBinError(2,2,0.00799608); 
  h_muon->SetBinContent(2,3,0.797698);              h_muon->SetBinError(2,3,0.00771815); 
  h_muon->SetBinContent(2,4,0.867739);              h_muon->SetBinError(2,4,0.00633557); 
  h_muon->SetBinContent(2,5,0.881944);              h_muon->SetBinError(2,5,0.00578922); 
  h_muon->SetBinContent(2,6,0.870393);              h_muon->SetBinError(2,6,0.00607255); 
  h_muon->SetBinContent(2,7,0.877853);              h_muon->SetBinError(2,7,0.00606442); 
  h_muon->SetBinContent(2,8,0.786343);              h_muon->SetBinError(2,8,0.00790827); 
  h_muon->SetBinContent(2,9,0.783943);              h_muon->SetBinError(2,9,0.00744082); 
  h_muon->SetBinContent(2,10,0);                    h_muon->SetBinError(2,10,0.000702244);
  h_muon->SetBinContent(3,1,0);                     h_muon->SetBinError(3,1,0.000478943); 
  h_muon->SetBinContent(3,2,0.78658);               h_muon->SetBinError(3,2,0.00628406); 
  h_muon->SetBinContent(3,3,0.81068);               h_muon->SetBinError(3,3,0.00610497); 
  h_muon->SetBinContent(3,4,0.895095);              h_muon->SetBinError(3,4,0.00446025); 
  h_muon->SetBinContent(3,5,0.889461);              h_muon->SetBinError(3,5,0.00414848);  
  h_muon->SetBinContent(3,6,0.902911);              h_muon->SetBinError(3,6,0.00391195); 
  h_muon->SetBinContent(3,7,0.886843);              h_muon->SetBinError(3,7,0.00450764); 
  h_muon->SetBinContent(3,8,0.78721);               h_muon->SetBinError(3,8,0.00632233); 
  h_muon->SetBinContent(3,9,0.821639);              h_muon->SetBinError(3,9,0.00580046); 
  h_muon->SetBinContent(3,10,0);                    h_muon->SetBinError(3,10,0.000465913);
  h_muon->SetBinContent(4,1,0);                     h_muon->SetBinError(4,1,0.000384865); 
  h_muon->SetBinContent(4,2,0.792867);              h_muon->SetBinError(4,2,0.00535692); 
  h_muon->SetBinContent(4,3,0.821556);              h_muon->SetBinError(4,3,0.00498267); 
  h_muon->SetBinContent(4,4,0.901116);              h_muon->SetBinError(4,4,0.00344333); 
  h_muon->SetBinContent(4,5,0.910752);              h_muon->SetBinError(4,5,0.00309193);  
  h_muon->SetBinContent(4,6,0.913198);              h_muon->SetBinError(4,6,0.0030385); 
  h_muon->SetBinContent(4,7,0.907685);              h_muon->SetBinError(4,7,0.00330015); 
  h_muon->SetBinContent(4,8,0.803082);              h_muon->SetBinError(4,8,0.00506704); 
  h_muon->SetBinContent(4,9,0.828758);              h_muon->SetBinError(4,9,0.00492407); 
  h_muon->SetBinContent(4,10,0);                    h_muon->SetBinError(4,10,0.000394385);
  h_muon->SetBinContent(5,1,0);                     h_muon->SetBinError(5,1,0.00068245); 
  h_muon->SetBinContent(5,2,0.000434972);           h_muon->SetBinError(5,2,0.00474704); 
  h_muon->SetBinContent(5,3,0.80838);               h_muon->SetBinError(5,3,0.00399422); 
  h_muon->SetBinContent(5,4,0.91272);               h_muon->SetBinError(5,4,0.00283883); 
  h_muon->SetBinContent(5,5,0.923706);              h_muon->SetBinError(5,5,0.00263639);  
  h_muon->SetBinContent(5,6,0.926825);              h_muon->SetBinError(5,6,0.00256688); 
  h_muon->SetBinContent(5,7,0.913914);              h_muon->SetBinError(5,7,0.0027955); 
  h_muon->SetBinContent(5,8,0.810017);              h_muon->SetBinError(5,8,0.00426566); 
  h_muon->SetBinContent(5,9,0.838319);              h_muon->SetBinError(5,9,0.00438509); 
  h_muon->SetBinContent(5,10,0);                    h_muon->SetBinError(5,10,0.00040652);
  h_muon->SetBinContent(6,1,0);                     h_muon->SetBinError(6,1,0.000688648); 
  h_muon->SetBinContent(6,2,0.804414);              h_muon->SetBinError(6,2,0.00597493); 
  h_muon->SetBinContent(6,3,0.82905);               h_muon->SetBinError(6,3,0.00495757); 
  h_muon->SetBinContent(6,4,0.923829);              h_muon->SetBinError(6,4,0.00332166); 
  h_muon->SetBinContent(6,5,0.92973);               h_muon->SetBinError(6,5,0.00318002);  
  h_muon->SetBinContent(6,6,0.935209);              h_muon->SetBinError(6,6,0.00304594); 
  h_muon->SetBinContent(6,7,0.91391);               h_muon->SetBinError(6,7,0.00350164); 
  h_muon->SetBinContent(6,8,0.822118);              h_muon->SetBinError(6,8,0.00515771); 
  h_muon->SetBinContent(6,9,0.843583);              h_muon->SetBinError(6,9,0.00533246); 
  h_muon->SetBinContent(6,10,0);                    h_muon->SetBinError(6,10,0.000667282);
  h_muon->SetBinContent(7,1,0);                     h_muon->SetBinError(7,1,0.000704384); 
  h_muon->SetBinContent(7,2,0.82229);               h_muon->SetBinError(7,2,0.00599885); 
  h_muon->SetBinContent(7,3,0.839963);              h_muon->SetBinError(7,3,0.00513158); 
  h_muon->SetBinContent(7,4,0.921485);              h_muon->SetBinError(7,4,0.003549); 
  h_muon->SetBinContent(7,5,0.937987);              h_muon->SetBinError(7,5,0.00310561);  
  h_muon->SetBinContent(7,6,0.937451);              h_muon->SetBinError(7,6,0.00313562); 
  h_muon->SetBinContent(7,7,0.924113);              h_muon->SetBinError(7,7,0.00343134); 
  h_muon->SetBinContent(7,8,0.812298);              h_muon->SetBinError(7,8,0.00551266); 
  h_muon->SetBinContent(7,9,0.844761);              h_muon->SetBinError(7,9,0.00559109); 
  h_muon->SetBinContent(7,10,0);                    h_muon->SetBinError(7,10,0.000712524);



  mapEffs[DEFS::muon] = h_muon;

}


//------------------------------------------------------------------------
double TriggerEfficiency::getWeight(DEFS::LeptonCat leptCat, double pt, double eta){

  double weight = 0;
  
  TH2 * trgEff = getTH2D(leptCat);
  if (trgEff == 0)
    return 0;

  //efficiencies and cuts for muons
  if(leptCat == DEFS::muon){
     
    if (pt > 7000 || fabs(eta) > 2.4)
      weight = 1.0;
    else
      weight = trgEff->GetBinContent(trgEff->FindBin(pt, eta));
    
  }  else  if(leptCat == DEFS::electron ){

    if (pt > 7000 || fabs(eta) > 2.5)
      weight = 1.0;
    else
      weight = trgEff->GetBinContent(trgEff->FindBin(pt, eta));

  } else 
    cout<<"WARNING TriggerEfficiency::getWeight() called with undefined category!. Returning zero."<<endl;
    
  return weight;

}//getWeight

//------------------------------------------------------------------------
TH2 * TriggerEfficiency::getTH2D(DEFS::LeptonCat leptonCat){

  if ( mapEffs.find(leptonCat) == mapEffs.end()){
    cout<<" ERROR TriggerEfficiency::getTH2D cannot find efficiency in map for lepton category"<<leptonCat<<endl;
    return 0;
  }
  
  // else return the map
  return mapEffs.find(leptonCat)->second;

}//getTH2D

