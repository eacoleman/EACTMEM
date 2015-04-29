//Our libraries
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//ROOT Libraries
#include <TStyle.h>
#include "TDirectory.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TMath.h"
#include "TGraph.h"

//C++ libraries
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

//A struct describing a given systematic
//Contains a boolean indicating if it is a shape systematic (0=no, 1=yes), the position in the combined vector of processes (empty if for all processes), and the values of the systematic
class systematic{
public:
  systematic() : name(""), shape(false) {}
  systematic(TString name_, bool shape_, int nvalues_) : name(name_), shape(shape_), values(vector<TString> (nvalues_,"-")) {}
  systematic(TString name_, bool shape_, int nvalues_, TString value_) : name(name_), shape(shape_), values(vector<TString> (nvalues_,value_)) {}
  systematic(TString name_, bool shape_, vector<TString> values_) : name(name_), shape(shape_), values(values_) {}

  TString         name;
  bool            shape;
  vector<TString> values;
};

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
 
  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         basepath          = cl.getValue<TString> ("basepath",          "/uscms_data/d2/aperloff/Summer12ME8TeV/2014_05_29_ValidationPlots_PU_CSV_TTbar_QCDEta_Scale/signal/QCD_ElFULL/");
  TString         histogramFileBase = cl.getValue<TString> ("histogramFileBase", "histos");
  TString         variable          = cl.getValue<TString> ("variable",           "Mlvjj");
  TString         channelName       = cl.getValue<TString> ("channelName",         "TwoJ");
  TString         lepCat            = cl.getValue<TString> ("lepton",              "both");
  DEFS::LeptonCat leptonCat         = DEFS::getLeptonCat(string(lepCat));
  string          jetBinS           = cl.getValue<string>  ("jetBin",             "jets2");
  DEFS::JetBin    jetBin            = DEFS::getJetBin(jetBinS);
  string          tagcatS           = cl.getValue<string>  ("tagcat",            "pretag");
  DEFS::TagCat    tagcat            = DEFS::getTagCat(tagcatS);
  string          anaTypeS          = cl.getValue<string>  ("anaType",    "HiggsAnalysis");
  DEFS::Ana::Type anaType           = DEFS::Ana::getAnaType(anaTypeS);
  bool            debug             = cl.getValue<bool>    ("debug",                false);

  if (!cl.check()) return 0;
  cl.print();

  TString histogramFile = histogramFileBase+"_"+lepCat+".root";
  TFile* in_file = TFile::Open(basepath+histogramFile,"READ"); // Input the root file filled with histograms

  // Get the input processes
  vector<PhysicsProcess*> processes = DefaultValues::getProcessesHiggs(jetBin, tagcat, false, false, DEFS::MicroNtuple);
  if(debug) {
     processes.erase(processes.begin(),processes.begin()+11);
  }

  // Name the bin for the processes
  vector <TString> vecSig;
  vector <TString> vecBak;
  vector <TString> vecCombined;

  for(unsigned int iprocess=0; iprocess<processes.size(); iprocess++) {

    TString processName = processes[iprocess]->name;
    //Skip Data
    if(DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::SingleEl_Data) {
       continue;
    }
    else if(DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::SingleMu_Data) {
       continue;
    }

    if(leptonCat==DEFS::electron && DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::QCD_MuFULL)
      continue;
    if(leptonCat==DEFS::muon && DEFS::PhysicsProcess::getProcessType(string(processName))==DEFS::PhysicsProcess::QCD_ElFULL)
      continue;


    if ((anaType == DEFS::Ana::WWAnalysis && (processName.EqualTo("WW") || processName.EqualTo("WZ"))) || 
        (anaType == DEFS::Ana::HiggsAnalysis && (processName.Contains("WH") || processName.Contains("ggH") || processName.Contains("qqH") || processName.Contains("ZH") || processName.Contains("TTH"))) ||
        (anaType == DEFS::Ana::UNKNOWN && (processName.Contains("WH") || processName.Contains("ggH") || processName.Contains("qqH")))){

      // Input each signal
      //vecSig.push_back(variable+"_"+processName+"_"+lepCat);
      vecSig.push_back(processName);
      
      cout<<"\tputting process "<<processName<<" into signal."<<endl;
    
    }
    else{ 
      // Input each background
      if(leptonCat==DEFS::both && processName.Contains("QCD_El")) {
         //vecBak.push_back(variable+"_QCD_"+lepCat);
        vecBak.push_back("QCD");
         cout<<"\tputting process QCD into background."<<endl;  
      }
      else if(leptonCat==DEFS::both && processName.Contains("QCD_Mu")) {
         continue;
      }
      else {
         //vecBak.push_back(variable+"_"+processName+"_"+lepCat);
        vecBak.push_back(processName);
         cout<<"\tputting process "<<processName<<" into background."<<endl;  
      }
    }
  }

  //Combine the siganl and background vectors
  vecCombined = vecSig;
  vector<TString>::iterator it = vecCombined.end();
  vecCombined.insert(it,vecBak.begin(),vecBak.end());

  // Creating the datacard
  ofstream outFile(Form("histos_%s_%s_datacard.txt",variable.Data(),lepCat.Data()));
  outFile << "#Simple datacard for ggH M125 Low mass H->WW->lnujj experiment" << endl
          << "#can list imax,jmax, kmax as specific values or * to make it auto calculate" << endl
          << "#expected limit command combine -M Asymptotic --significance *DATACARDNAME* -t -1 --expectSignal=1 -m 125" << endl;

  // process counter line
  int jmax = vecSig.size()+vecBak.size();

  // Input total number of bins
  outFile << "imax " << left << setw(2) << "1" << "  number of channels" << endl;
  
  // Input total number of processes in bin1 minus one
  //outFile << "jmax " << left << setw(2) << jmax - 1 << "  number of backgrounds minus 1" << endl;
  outFile << "jmax " << left << setw(2) << "*" << "  number of backgrounds minus 1 or *" << endl;
  
  // Input total number of systematics
  outFile << "kmax " << left << setw(2) << "*" << "  number of nuisance parameters (sources of systematic uncertainties)" << endl << endl;
  
  outFile << "------------------------" << endl
          << "#list shape uncertainties and the input files for them " << endl
          << "#\tNOTE: hists must be in root file with names exactly as listed in systematics" << endl
          << "#The channel name can be either the actual name or a * for all channels" << endl << endl;
  
  // Input the root file name and whatever edits to the names you wish to make
  outFile << "shapes " << left << setw(9) << "* "<< channelName << "\t" << basepath << "histos_" << lepCat << ".root " << variable << "_$PROCESS_" << lepCat << " " << variable << "_$PROCESS_" << lepCat << "_$SYSTEMATIC" << endl;

  // Input the root file name and the data_obs file name
  TH1D* dataHist = 0;
  if(leptonCat==DEFS::electron) {
    outFile << "shapes " << left << setw(9) << "data_obs " << channelName << "\t" << basepath << histogramFile << " " << variable << "_SingleEl_Data_" << lepCat << endl << endl;
    dataHist = (TH1D*) in_file->Get(variable+"_SingleEl_Data_"+lepCat);
  }
  else if(leptonCat==DEFS::muon) {
    outFile << "shapes " << left << setw(9) << "data_obs " << channelName << "\t" << basepath << histogramFile << " " << variable << "_SingleMu_Data_" << lepCat << endl << endl;
    dataHist = (TH1D*) in_file->Get(variable+"_SingleMu_Data_"+lepCat);
  }
  else if(leptonCat==DEFS::both) {
    outFile << "shapes " << left << setw(9) << "data_obs " << channelName << "\t" << basepath << histogramFile << " " << variable << "_Data_" << lepCat << endl << endl;
    dataHist = (TH1D*) in_file->Get(variable+"_Data_"+lepCat);
  }

  outFile << "------------------------" << endl
          << "# bin = name of channel we are observing events in, i.e. 2+jets" << endl
          << "# observation = number of events seen in that bin" << endl << endl;  
  
  // Input the name of your bin
  // Input the name of your bin
  outFile << left << setw(12) << "bin " << channelName << endl; 
  
  outFile << left << setw(12) << "observation " << std::setprecision(8) << dataHist->Integral() << endl << endl;
  
  outFile << "------------------------" << endl
          << "# bin = bin from above.  Each signal and background will have a column for every bin listed" << endl
          << "# process line 1: the name of the process.  This must match the syntax used in root files of the shape uncertainties" << endl
          << "# process line 2: assign a number to the process.  All signals get process = 0, all backgrounds get independent positive numbers" << endl
          << "# rate = expected number of events of the process in specified bin" << endl << endl;
  
  outFile << left << setw(8) << "bin "; 

  //Bin line
  for (unsigned int num_bins = 0 ; num_bins < (vecSig.size()+vecBak.size()); num_bins++){
    
    outFile << left << setw(max(vecCombined[num_bins].Length()+1,channelName.Length()+1)) << channelName;
    
  } //for bin line
  
  outFile << endl;
  
  outFile << left << setw(8) << "process ";

  // signal process name line
  for (unsigned int name_sig = 0 ; name_sig < vecSig.size(); name_sig++){
    
    //TH1D* sig_name = (TH1D*) in_file->Get(variable+"_"+vecSig[name_sig]+"_"+lepCat);
    
    //outFile << left << setw(max(vecSig[name_sig].Length()+1,channelName.Length()+1)) << sig_name->GetName();
    outFile << left << setw(max(vecSig[name_sig].Length()+1,channelName.Length()+1)) << vecSig[name_sig];

  } // for sig process name line

  // background process name line
  for (unsigned int name_bak = 0 ; name_bak < vecBak.size(); name_bak++){
    
    //TH1D* bak_name = (TH1D*) in_file->Get(variable+"_"+vecBak[name_bak]+"_"+lepCat);
    
    //outFile << left << setw(max(vecBak[name_bak].Length()+1,channelName.Length()+1)) << bak_name->GetName();
    outFile << left << setw(max(vecBak[name_bak].Length()+1,channelName.Length()+1)) << vecBak[name_bak];

  } // for bak process name line
  
  outFile << endl;
  
  outFile << left << setw(8) << "process ";

  // Making an array to output the signal process IDs
  for (int sig_num = vecSig.size()-1 ; sig_num >= 0; sig_num--){ 

    // Output a negative number (or zero) for each signal process
    if(sig_num == int(vecSig.size()-1))
      outFile << left << setw(max(vecSig[sig_num].Length()+2,channelName.Length()+2)) << Form("%i",-int(sig_num));
    else if (sig_num == 0)
      outFile << left << setw(max(vecSig[sig_num].Length(),channelName.Length())) << Form("%i",-int(sig_num));
    else
      outFile << left << setw(max(vecSig[sig_num].Length()+1,channelName.Length()+1)) << Form("%i",-int(sig_num));

  } // for sig num ID

  // Making an array to output the background process IDs
  for (unsigned int bak_num = 0; bak_num < vecBak.size(); bak_num++){

    // Output a positive number for each background process
    outFile << left << setw(max(vecBak[bak_num].Length()+1,channelName.Length()+1)) << bak_num+1;

  } // for bak num ID

  outFile << endl;
  
  outFile << left << setw(8) << "rate ";

   //Printing process rates
  for (unsigned int iproc = 0 ; iproc < vecCombined.size(); iproc++){
    
    // Get the histograms out
    TH1D* proc = (TH1D*) in_file->Get(variable+"_"+vecCombined[iproc]+"_"+lepCat);
    
    // Integrate each histogram
    double data_store = proc->Integral(); 
    
    // Print signal integral data
    outFile << left << setw(max(vecCombined[iproc].Length(),channelName.Length()+1)) << std::setprecision(8) << data_store << " "; 
    
  } // for signal integral loop

  outFile << endl << endl;
  
  outFile << "------------------------" << endl
          << "#List all systematic uncertainties" << endl
          << "#column 1 = sysName, column 2 = Dist type (options are lnN, shape), column 3+ = uncertainty value. 1.2 = 20% uncertainty. - means N/A " << endl << endl;


  // adding systematics
  // map contains systematic names, 
  vector<systematic> systematics;
  systematics.push_back(systematic("ggHXsec",false, vecCombined.size()));
  systematics.back().values[0] = "1.1082";
  systematics.back().values[1] = "1.1082";
  systematics.back().values[2] = "1.1082";
  systematics.back().values[3] = "1.1082";
  systematics.back().values[4] = "1.1082";  
  systematics.push_back(systematic("wjetsXsec",false, vecCombined.size()));
  systematics.back().values[17] = "1.033";
  systematics.push_back(systematic("ttbarXsec",false, vecCombined.size()));
  systematics.back().values[13] = "1.04";
  systematics.push_back(systematic("zjetsXsec",false, vecCombined.size()));
  systematics.back().values[16] = "1.034";
  systematics.push_back(systematic("singleTXsec",false, vecCombined.size()));
  systematics.back().values[7] = "1.05";
  systematics.back().values[8] = "1.05";
  systematics.back().values[9] = "1.05";
  systematics.back().values[10] = "1.05";
  systematics.back().values[11] = "1.05";
  systematics.back().values[12] = "1.05";
  systematics.push_back(systematic("wwXsec",false, vecCombined.size()));
  systematics.back().values[14] = "1.03";
  systematics.push_back(systematic("wzXsec",false, vecCombined.size()));
  systematics.back().values[6] = "1.03";
  systematics.push_back(systematic("qcdNorm",false, vecCombined.size()));
  systematics.back().values[15] = "1.02";
  //systematics.push_back(systematic("#qcdEtaWeight",false, vecCombined.size()));
  //systematics.push_back(systematic("#wjetsSF",false, vecCombined.size()));
  systematics.push_back(systematic("lumi",false,vector<TString> (vecCombined.size(),"1.022")));
  systematics.push_back(systematic("jetEnergy",false, vecCombined.size(), "1.02"));
  //systematics.push_back(systematic("metPhi",false, vecCombined.size()));
  //systematics.push_back(systematic("topWeight",true, vecCombined.size()));
  //systematics.back().values[6] = "1.0";

  //Find the maximum width of the systematics' names
  int sysWidth = 0;
  for (unsigned int iSys=0; iSys!=systematics.size(); iSys++) {
    if(systematics[iSys].name.Length()>sysWidth) sysWidth = systematics[iSys].name.Length();
  }

  cout << "Printing the systematics ... ";
  for (unsigned int iSys=0; iSys!=systematics.size(); iSys++) {
    outFile << left << setw(sysWidth+1) << systematics[iSys].name;
    if(systematics[iSys].shape==false) outFile << left << setw(7) << "lnN";
    else                               outFile << left << setw(7) << "shape";

    for (unsigned int iproc = 0 ; iproc < (systematics[iSys].values.size()); iproc++){
      //outFile << "1.045 ";
      //outFile << left << setw(6) << "1.022 ";
      outFile << left << setw(7) << systematics[iSys].values[iproc];
    } //for luminosity loop

    outFile << endl << flush;
  }
  cout << "DONE" << endl;

  cout << "Closing outFile ... ";
  outFile.close();
  cout << "DONE" << endl;

  return 0;
  
} //LimitsDatacard_ggH125



/*

hwwXsec       lnN    1.072     -     -     -     -     -     -   
wjetsXsec     lnN    -    1.033     -     -     -     -     -
ttbarXsec     lnN    -     -     1.04     -     -    -     -
zjetsXsec     lnN    -     -     -     1.034     -     -   -
singleTXsec   lnN    -     -     -     -     1.05    -   -
dibosonXsec   lnN    -     -     -     -     -    1.03   -
qcdNorm       lnN    -     -     -     -     -     -     1.02
#qcdEtaWeight  lnN    -     -     -     -     -     -     -
#wjetsSF       lnN    -     -     -     -     -    -   -
lumi          lnN    1.022  1.022   1.022   1.022   1.022   1.022   1.022
jetEnergy     lnN    1.02  1.02   1.02   1.02   1.02   1.02   1.02
#metPhi        lnN    -     -     -     -     -     -     -
topWeight     shape   -     -     1    -     -     -   -

*/
