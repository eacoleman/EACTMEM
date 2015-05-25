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
#include <utility>

using namespace std;

//A struct describing a given systematic
//Contains a boolean indicating if it is a shape systematic (0=no, 1=yes), the position in the combined vector of processes (empty if for all processes), and the values of the systematic
class systematic{
public:
  systematic() : name(""), shape(false) {}
  systematic(TString name_, bool shape_, int nvalues_) : name(name_), shape(shape_), values(vector<pair<TString,TString> > (nvalues_,make_pair("-","-"))) {}
  systematic(TString name_, bool shape_, int nvalues_, TString value_) : name(name_), shape(shape_), values(vector<pair<TString,TString> > (nvalues_,make_pair(value_,value_))) {}
  systematic(TString name_, bool shape_, vector<pair<TString,TString> > values_) : name(name_), shape(shape_), values(values_) {}

  string getValueString(int index);

  TString         name;
  bool            shape;
  vector<pair<TString,TString> > values;
};

string systematic::getValueString(int index) {
  std::stringstream ss;
  if(values[index].first == values[index].second) {
    ss << values[index].first;
  }
  else {
    ss << values[index].first << "/" << values[index].second;
  }
  return ss.str();
}

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
  TString         channelName       = cl.getValue<TString> ("channelName",       "TwoJ0B");
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
  ofstream outFile(Form("DataCard_%s_%s_%s.txt",lepCat.Data(),channelName.Data(),variable.Data()));
  outFile << "#Simple datacard for ggH M125 Low mass H->WW->lnujj experiment" << endl
          << "#can list imax,jmax, kmax as specific values or * to make it auto calculate" << endl
          << "#expected limit command (with systematics) \"combine -M Asymptotic --significance *DATACARDNAME* -t -1 --expectSignal=1 -m 125 --run expected\"" << endl
          << "#expected limit command (without systematics) \"combine -M Asymptotic --significance *DATACARDNAME* -t -1 --expectSignal=1 -S 0 -m 125 --run expected\"" << endl
          << "#observed limit command (with systematics) \"combine -M Asymptotic --significance *DATACARDNAME* -t -1 --expectSignal=1 -m 125\"" << endl
          << "#observed p-value/significance command \"combine -M ProfileLikelihood --significance *DATACARDNAME* --expectSignal=1 -S 0 -m 125\"" << endl
          << "#expected p-value/significance command \"combine -M ProfileLikelihood --significance *DATACARDNAME* -t -1 --expectSignal=1 -S 0 -m 125\"" << endl
          << "#To combine the datacards in each channel use the command \"combineCards.py Name1=card1.txt Name2=card2.txt .... > card.txt\"" << endl
          << "#MaxLikelidooh fit of datacard to get mlfit.root file \"combine -M MaxLikelihoodFit -t -1 --expectSignal=1 --minos=all DataCardName.txt\"" << endl
          << "#To get the post-fit nuisance parameters \"python diffNuisances.py -a --format=latex mlfit.root -g plots.root\"" << endl;

  // process counter line
  //int jmax = vecSig.size()+vecBak.size();

  // Input total number of bins
  outFile << "imax " << left << setw(2) << "1" << "  number of channels" << endl;
  
  // Input total number of processes in bin1 minus one
  //outFile << "jmax " << left << setw(2) << jmax - 1 << "  number of backgrounds minus 1" << endl;
  outFile << "jmax " << left << setw(2) << "*" << "  number of backgrounds minus 1 or *" << endl;
  
  // Input total number of systematics
  outFile << "kmax " << left << setw(2) << "*" << "  number of nuisance parameters (sources of systematic uncertainties)" << endl << endl;
  
  outFile << "#list shape uncertainties and the input files for them " << endl
          << "#NOTE: hists must be in root file with names exactly as listed in systematics" << endl
          << "#The channel name can be either the actual name or a * for all channels" << endl
          << "----------------------------------------------------------------------------------------------"  << endl << endl;

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

  outFile << "# bin = name of channel we are observing events in, i.e. 2+jets" << endl
          << "# observation = number of events seen in that bin" << endl << endl;  
  
  // Input the name of your bin
  outFile << left << setw(12) << "bin " << channelName << endl; 
  
  outFile << left << setw(12) << "observation " << std::setprecision(8) << dataHist->Integral() << endl << endl;
  
  outFile << "# bin = bin from above.  Each signal and background will have a column for every bin listed" << endl
          << "# process line 1: the name of the process.  This must match the syntax used in root files of the shape uncertainties" << endl
          << "# process line 2: assign a number to the process.  All signals get process = 0, all backgrounds get independent positive numbers" << endl
          << "# rate = expected number of events of the process in specified bin" << endl
          << "----------------------------------------------------------------------------------------------" << endl << endl;

  outFile << left << setw(8) << "bin "; 

  //Bin line
  for (unsigned int num_bins = 0 ; num_bins < (vecSig.size()+vecBak.size()); num_bins++){
    outFile << left << setw(max(11,max(vecCombined[num_bins].Length()+1,channelName.Length()+1))) << channelName;
  } //for bin line
  
  outFile << endl;
  
  outFile << left << setw(8) << "process ";

  // signal process name line
  for (unsigned int name_sig = 0 ; name_sig < vecSig.size(); name_sig++){
    outFile << left << setw(max(11,max(vecSig[name_sig].Length()+1,channelName.Length()+1))) << vecSig[name_sig];
  } // for sig process name line

  // background process name line
  for (unsigned int name_bak = 0 ; name_bak < vecBak.size(); name_bak++){
    outFile << left << setw(max(11,max(vecBak[name_bak].Length()+1,channelName.Length()+1))) << vecBak[name_bak];
  } // for bak process name line
  
  outFile << endl;
  
  outFile << left << setw(8) << "process ";

  // Making an array to output the signal process IDs
  for (int sig_num = vecSig.size()-1 ; sig_num >= 0; sig_num--){ 

    // Output a negative number (or zero) for each signal process
    //if(sig_num == int(vecSig.size()-1))
    //  outFile << left << setw(max(10,max(vecSig[sig_num].Length()+2,channelName.Length()+2))) << Form("%i",-int(sig_num));
    //if (sig_num == 0)
    //  outFile << left << setw(max(11,max(vecSig[sig_num].Length(),channelName.Length()))) << Form("%i",-int(sig_num));
    //else
      outFile << left << setw(max(11,max(vecSig[vecSig.size()-1-sig_num].Length()+1,channelName.Length()+1))) << Form("%i",-int(sig_num));

  } // for sig num ID

  // Making an array to output the background process IDs
  for (unsigned int bak_num = 0; bak_num < vecBak.size(); bak_num++){
    // Output a positive number for each background process
    outFile << left << setw(max(11,max(vecBak[bak_num].Length()+1,channelName.Length()+1))) << bak_num+1;
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
    outFile << left << setw(max(11,max(vecCombined[iproc].Length()+1,channelName.Length()+1))) << std::setprecision(8) << data_store; 
    
  } // for signal integral loop

  outFile << endl << endl;
  
  outFile << "------------------------" << endl
          << "#List all systematic uncertainties" << endl
          << "#column 1 = sysName, column 2 = Dist type (options are lnN, shape), column 3+ = uncertainty value. 1.2 = 20% uncertainty. - means N/A " << endl << endl;


  // adding systematics
  //WH_HToZZ_M125 ZH_HToZZ_M125 TTH_HToZZ_M125 WH125_HToBB TTH_HToBB_M125 ggH125 qqH125 WH_HToWW_M125 ZH_HToWW_M125 TTH_HToWW_M125 WZ STopS_T STopS_Tbar STopT_T STopT_Tbar STopTW_T STopTW_Tbar TTbar WW QCD_ElFULL ZJets WJets     
  //0             1             2              3           4              5      6      7             8             9              10 11      12         13      14         15       16          17    18 19         20    21        
  vector<systematic>systematics;
  //Flat ratee uncernties across the boar
  systematics.push_back(systematic("lumi_8TeV",false,vecCombined.size(),"1.026"));
  systematics.back().values[19] = make_pair("-","-"); //QCD needs rate sys removed
  systematics.back().values[21] = make_pair("-","-"); //WJets needs rate sys removed
  if(leptonCat==DEFS::electron) {
    systematics.push_back(systematic("CMS_eff_e",false,vecCombined.size(),"1.02"));
    systematics.back().values[19] = make_pair("-","-"); //QCD needs rate sys removed
    systematics.back().values[21] = make_pair("-","-"); //WJets needs rate sys removed
    systematics.push_back(systematic("CMS_eff_m",false,vecCombined.size()));
  }
  else if(leptonCat==DEFS::muon) {
    systematics.push_back(systematic("CMS_eff_m",false,vecCombined.size(),"1.02"));
    systematics.back().values[19] = make_pair("-","-"); //QCD needs rate sys removed
    systematics.back().values[21] = make_pair("-","-"); //WJets needs rate sys removed
    systematics.push_back(systematic("CMS_eff_e",false,vecCombined.size()));
  }
  else if(leptonCat==DEFS::both) {
    systematics.push_back(systematic("CMS_eff_l",false,vecCombined.size(),"1.02"));
    systematics.back().values[19] = make_pair("-","-"); //QCD needs rate sys removed
    systematics.back().values[21] = make_pair("-","-"); //WJets needs rate sys removed
  }
  systematics.push_back(systematic("CMS_scale_met",false,vecCombined.size(),"1.002"));
  systematics.back().values[19] = make_pair("-","-"); //QCD needs rate sys removed
  systematics.back().values[21] = make_pair("-","-"); //WJets needs rate sys removed

  //rate uncertainties specific to a given process
  systematics.push_back(systematic("QCDscale_ggH",false,vecCombined.size())); //ggH scale uncertainty
  systematics.back().values[5] = make_pair("0.922","1.072"); //ggH125
  systematics.push_back(systematic("QCDscale_qqH",false,vecCombined.size())); //qqH scale uncertainty
  systematics.back().values[6] = make_pair("0.998","1.002"); //qqH125
  systematics.push_back(systematic("QCDscale_WH",false,vecCombined.size())); //WH scale uncertainty
  systematics.back().values[3] = make_pair("0.99","1.01"); //WH125_HToBB
  systematics.back().values[7] = make_pair("0.99","1.01"); //WH_HToWW_M125
  systematics.back().values[0] = make_pair("0.99","1.01"); //WH_HToZZ_M125
  systematics.push_back(systematic("QCDscale_ZH",false,vecCombined.size())); //ZH scale uncertainty
  systematics.back().values[8] = make_pair("0.969","1.031"); //ZH_HToWW_M125
  systematics.back().values[1] = make_pair("0.969","1.031"); //ZH_HToZZ_M125
  systematics.push_back(systematic("QCDscale_TTH",false,vecCombined.size())); //TTH scale uncertainty
  systematics.back().values[9] = make_pair("0.907","1.038"); //TTH_HToWW_M125
  systematics.back().values[2] = make_pair("0.907","1.038"); //TTH_HToZZ_M125
  systematics.back().values[4] = make_pair("0.907","1.038"); //TTH_HToBB_M125
  systematics.push_back(systematic("pdf_gg",false,vecCombined.size())); //gg->X PDF+alpha_s uncertainty
  systematics.back().values[5] = make_pair("0.931","1.075"); //ggH125
  systematics.back().values[9] = make_pair("0.919","1.081"); //TTH_HToWW_M125
  systematics.back().values[2] = make_pair("0.919","1.081"); //TTH_HToZZ_M125
  systematics.back().values[4] = make_pair("0.919","1.081"); //TTH_HToBB_M125
  systematics.push_back(systematic("pdf_qq",false,vecCombined.size())); //qq->X PDF+alpha_s uncertainty
  systematics.back().values[6] = make_pair("0.972","1.026"); //qqH125
  systematics.back().values[3] = make_pair("0.977","1.023"); //WH125_HToBB
  systematics.back().values[7] = make_pair("0.977","1.023"); //WH_HToWW_M125
  systematics.back().values[8] = make_pair("0.975","1.025"); //ZH_HToWW_M125
  systematics.back().values[0] = make_pair("0.977","1.023"); //WH_HToWZZ_M125
  systematics.back().values[1] = make_pair("0.975","1.025"); //ZH_HToWZZ_M125
  //systematics.push_back(systematic("CMS_QCDscale_pdf_wjets",false, vecCombined.size())); //Do not include when using QCD Eta Weight Uncertainties
  //systematics.back().values[21] = "1.033";
  systematics.push_back(systematic("CMS_QCDscale_pdf_ttbar",false,vecCombined.size()));
  systematics.back().values[17] = make_pair("1.04","1.04"); //TTbar
  systematics.push_back(systematic("CMS_QCDscale_pdf_zjets",false,vecCombined.size()));
  systematics.back().values[20] = make_pair("1.034","1.034"); //ZJets
  systematics.push_back(systematic("CMS_QCDscale_pdf_singlet",false,vecCombined.size()));
  systematics.back().values[11] = make_pair("1.05","1.05");
  systematics.back().values[12] = make_pair("1.05","1.05");
  systematics.back().values[13] = make_pair("1.05","1.05");
  systematics.back().values[14] = make_pair("1.05","1.05");
  systematics.back().values[15] = make_pair("1.05","1.05");
  systematics.back().values[16] = make_pair("1.05","1.05");
  systematics.push_back(systematic("CMS_QCDscale_pdf_diboson",false,vecCombined.size()));
  systematics.back().values[18] = make_pair("1.03","1.03"); //WZ
  systematics.back().values[10] = make_pair("1.03","1.03"); //WW
  //systematics.push_back(systematic("CMS_hww_lnujj_qcdNorm",false,vecCombined.size())); //Do not include when using QCD Eta Weight Uncertainties
  //systematics.back().values[21] = make_pair("1.02","1.02");
  //systematics.push_back(systematic("CMS_hww_lnujj_PUWeight",false,vecCombined.size())); //Vary min bias xsec up and down by 7% (nominal 69400 up 74258 down 64542)
  //systematics.push_back(systematic("CMS_hww_lnujj_CSVWeight",false,vecCombined.size())); //up as CSVWeight^2 and down as no CSVWeight
  //systematics.push_back(systematic("CMS_hww_lnujj_topPtWeight",false,vecCombined.size())); //up as TopPtWeight^2 and down as no topPTWeight
  //systematics.push_back(systematic("CMS_hww_lnujj_QCDEtaWeight",false,vecCombined.size()));
  //systematics.push_back(systematic("CMS_scale_j",false,vecCombined.size()));

  //shape uncertainties
  //systematics.push_back(systematic("CMS_hww_lnujj_matching_shape",true,vecCombined.size()));
  //systematics.back().values[10] = make_pair("1","1");
  //systematics.push_back(systematic("CMS_hww_lnujj_scale_shape",true,vecCombined.size()));
  //systematics.back().values[10] = make_pair("1","1");
  //systematics.push_back(systematic("CMS_hww_lnujj_topPtWeight_shape",true,vecCombined.size()));
  //systematics.push_back(systematic("CMS_hww_lnujj_QCDEtaWeight_shape",true,vecCombined.size()));
  //systematics.push_back(systematic("CMS_scale_j_shape",true,vector<TString> (vecCombined.size(),"1")));
  //systematics.back().values[21] = make_pair("-","-"); //QCD does not have a jes uncertainty


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
      outFile << left << setw(13) << systematics[iSys].getValueString(iproc);
    } //for luminosity loop

    outFile << endl << flush;
  }
  cout << "DONE" << endl;

  cout << "Closing outFile ... ";
  outFile.close();
  cout << "DONE" << endl;

  cout << "Closing the in_file ... ";
  in_file->Close();
  cout << "DONE" << endl;

  return 0;
  
} //LimitsDatacard
