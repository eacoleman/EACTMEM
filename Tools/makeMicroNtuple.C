// C++ libraries
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// ROOT libraries
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TTree.h"

// This code libraries
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::set;

//-----------------------------------------------------------------------------
// This struct is used to hold the signature of the microntuple
struct Signature
{
  int run;
  int event;

  // operator < is needed for the set ordering
  bool operator<(const Signature& rhs) const{
    if (run != rhs.run)
      return run < rhs.run;    
    return event < rhs.event;
  }//operator<

};


//-----------------------------------------------------------------------------
// This is the core of the makeMicroNtuple algorithm.
void makeMicroNtuple(TChain & chain, string output, unsigned nJets, 
		     bool doLight=false, bool doNonW= false, bool doUntag=false)
{

  // Create the base objects
  METree      * meNtuple    = new METree();
  EventNtuple * eventNtuple = new EventNtuple();
  MicroNtuple * microNtuple = new MicroNtuple(nJets);

  MicroNtuple::indexMap2 indexMap = MicroNtuple::getIndexMap();
  MicroNtuple::indexMap1 indexMapWH = indexMap[DEFS::EP::WH];
  MicroNtuple::indexMap1 indexMapHWW = indexMap[DEFS::EP::HWW];

  // and set the meNtuple to read from it
  chain.SetBranchAddress("METree.", &meNtuple);

  // Create and output file and clone the tree that will be in the output
  // and set microNtuple that fills it
  TFile outputFile(output.c_str(), "RECREATE");
  TTree* outputTree = chain.CloneTree(0);
  outputTree->Branch("mnt", "MicroNtuple", &microNtuple);

  // Get the entries and report if zero
  unsigned nentries = static_cast<unsigned>(chain.GetEntries());
  cout << "\tOriginal chain has " << nentries << " entries" << endl;
  if (nentries == 0){
      cout << "\tNo entries found!  Aborting.\n";
      return;
  }

  // Holds the list of events to avoid duplication
  set<Signature> sigSet;

  //Loop over all the entries in the original chain. The idea is 
  // to copy everything to microNtuple
  for (unsigned ientry = 0; ientry < nentries; ++ientry){

    // get the entry
    chain.GetEntry(ientry);

    // clear the microNtuple
    microNtuple->clear();

    // First copy all the event probabilities
    for (int i = 0; i < meNtuple->getNProbStat(); ++i){
      microNtuple->eventProb[i]    = meNtuple->getProbStat(i)->tEventProb;
      microNtuple->eventMaxProb[i] = meNtuple->getProbStat(i)->tEventMaxProb;  
    }    
    
    // Get the b-probabilites for each jet.
    for (unsigned int bb = 0; bb < eventNtuple->jBtag.size(); bb++)
      microNtuple->bProb[bb] = eventNtuple->jBtag[bb];
    
    // WWandWZ EPD, for 
    //      DEFS::pretag is calculated ignoring the number of tags or bProb's   
    //      DEFS::eqxTSV is calculated using the number of tags or bProb's,
    //                   and the only diff between them is the coefficients.
    microNtuple->epdPretagWWandWZ = microNtuple->calcWZEPD(DEFS::pretag);
    microNtuple->epd0tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq0TSV);
    microNtuple->epd1tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq1TSV);
    microNtuple->epd2tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq2TSV);

    // Make sure to do these last!
    microNtuple->epd1tag      = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kCombined);
    microNtuple->epd2tag      = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kCombined);
    microNtuple->epd1tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kSchan);
    microNtuple->epd2tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kSchan);
    microNtuple->epd1tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kTchan);
    microNtuple->epd2tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kTchan);

    // Loop over the WH masses
    unsigned auxI = 0;
    for (MicroNtuple::indexMap1::const_iterator it = indexMapWH.begin(); 
	 it != indexMapWH.end(); it++){
      microNtuple->epd1tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq1TSV, it->first);
      microNtuple->epd2tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq2TSV, it->first);
      auxI ++;
    }//for 

    // Loop over the H->WW masses
    auxI = 0;
    for (MicroNtuple::indexMap1::const_iterator it = indexMapHWW.begin(); 
	 it != indexMapHWW.end(); it++){
      microNtuple->epd1tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq1TSV, it->first);
      microNtuple->epd2tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq2TSV, it->first);
      auxI++;
    }//for 

    /*
    // Make sure this is not a duplicated event
    Signature temp = {eventNtuple->run, eventNtuple->event};
    if (sigSet.find(temp) != sigSet.end()) {
      cout << "\tWarning! Duplicate event found! Run " << temp.run 
	   << " Event " << temp.event << endl;
    } else {
      sigSet.insert(temp);
      outputTree->Fill();
    }
    */
    outputTree->Fill();

  }//for entries


  // Report some results
  cout << "\tWrote " << output << " with " << outputTree->GetEntries()
       << " entries" << endl;
  outputFile.Write();
  
  delete meNtuple;
  delete microNtuple;

}// makeMicroNtuple

//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
void makeMicroNtuple(vector<string> locations, string output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false){

  cout<<endl;
  cout<<"  Generating MicroNtuple "<<output<<endl;

  TChain chain("METree");
  int file_count =0;
  for (unsigned e=0;e<locations.size(); e++) {
    cout<<"\tAdding "<<(locations[e] + "*.root")<<endl;
    file_count += chain.Add((locations[e] + "*.root").c_str());
  }

  if (file_count==0){
    cout << "\tNo files found!  Aborting.\n";
    return;
  }

  //cout << "s1" << endl;
  makeMicroNtuple(chain, output, nJets, doLight, doNonW, doUntag);
  //cout << "s2" << endl;
}

//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
void makeMicroNtuple(string location, string output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false){

  vector<string> locations;
  locations.push_back(location);
  makeMicroNtuple(locations, output, nJets, doLight, doNonW, doUntag);
  
}


/*
//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
//use like this:
// first do the ttbar splitting and HF removal at Event_Probs level using mergeAll.C
.X mergeAll.C+

//Then do all the microntuple by doing
root -l 
.L ../lib/slc5_ia32_gcc434/libTAMUWWMEPATNtuple.so
.L TAMUWW/Tools/makeMicroNtuple.C+
createAllMicroNtuples()

*/

//RE auxiliary struct to make all the microntuples automatically
class  MyStr{
public:
  MyStr(string d, string n, bool m, bool nw, bool nwg){
    dirs.push_back(d);
    name = n;
    mistag = m ;
    nonw = nw;
    untag = nwg;
  }
  void   AddDir(string d){ dirs.push_back(d);}
  vector<string> GetLocationVector(string path){
    vector<string> res;
    for (unsigned d=0;d<dirs.size();d++)
      res.push_back(path + dirs[d]);
    return res;
  }
  vector<string> dirs  ; // the set of directories to run over
  string         name  ; // the file name of the output w/o the ".root" extension
  bool           mistag; // whether this is a mistag or
  bool           nonw  ; // whether this is nonW or not
  bool           untag  ; // whether this is untag or not
};

// WARNING!!! is the microNtuple as bigger than 2Gs the code crashes
void createAllMicroNtuples(){

  string basePath="/uscms/home/ilyao/nobackup/MEResults/387PF2PAT/";
  //string basePath="/uscms/home/ilyao/nobackup/MEResults/PAT356FullScan/";
  // string basePath="./";

  //Input files for 2-jet bin
  string inputPath=basePath+"";
  unsigned nJet = 2;

  //Input files for 3-jet bin
  //string inputPath=basePath+"Event_Probs_Summer09_3jets_StndTF/";
  //unsigned nJet = 3;

  //Output files
  //string outputPath=inputPath+"MicroNtuples/";
  string outputPath ="/uscms/home/eusebi/workarea/CMSSW_3_8_7/src/";

  //Create the list of MicroNtuples
  vector<MyStr> listOfMicroNtuples;

  //// Diboson
  //  listOfMicroNtuples.push_back(MyStr("WW3300Evt*","micro_WW3300Evt",false,false,false));

//   listOfMicroNtuples.push_back(MyStr("WW2200Evt*","micro_WW2200Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WZ1500Evt*","micro_WZ1500Evt",false,false,false));

//   //// W+jets and Z+jets
//   listOfMicroNtuples.push_back(MyStr("WpJ26000Evt*","micro_WpJ24000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("ZpJ680Evt*","micro_ZpJ680Evt",false,false,false));

//   //// ttbar
//   listOfMicroNtuples.push_back(MyStr("TTbar12000Evt*","micro_TTbar12000Evt",false,false,false));

  //// Single top
  listOfMicroNtuples.push_back(MyStr("STopT12000Evt*","micro_STopT24000Evt",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("STopS12000Evt*","micro_STopS24000Evt",false,false,false));

  //// QCD
//    listOfMicroNtuples.push_back(MyStr("QCDElData400Evt*","micro_QCDElData400Evt",false,false,false));
//    listOfMicroNtuples.push_back(MyStr("QCDMuData7600Evt*","micro_QCDMuData7600Evt",false,false,false));
//  listOfMicroNtuples.push_back(MyStr("QCDOnlyElData85Evt*","micro_QCDOnlyElData85Evt",false,false,false));


  //// Data
  //  listOfMicroNtuples.push_back(MyStr("DataEl1900Evt*","micro_DataEl1900Evt",false,false,false));
  listOfMicroNtuples.push_back(MyStr("DataMu2300Evt*","micro_DataMu2300Evt",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("Data*","micro_DataEl1900AndMu2300Evt",false,false,false));


//   //// Higgs 
//   listOfMicroNtuples.push_back(MyStr("HWWMH120_600Evt*","micro_HWWMH120_600Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH130_800Evt*","micro_HWWMH130_800Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH140_1100Evt*","micro_HWWMH140_1100Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH150_1600Evt*","micro_HWWMH150_1600Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH160_2700Evt*","micro_HWWMH160_2700Evt",false,false,false));

//   listOfMicroNtuples.push_back(MyStr("WHMH115_3000Evt*","micro_WHMH115_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH120_3000Evt*","micro_WHMH120_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH125_3000Evt*","micro_WHMH125_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH130_3000Evt*","micro_WHMH130_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH135_3000Evt*","micro_WHMH135_3000Evt",false,false,false));



//   //// W+jets and Z+jets
//   listOfMicroNtuples.push_back(MyStr("WpJ35700Evt*","micro_WpJ35700Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("ZpJ2000Evt*","micro_ZpJ2000Evt",false,false,false));

//   //// ttbar
//   listOfMicroNtuples.push_back(MyStr("TTbar30600Evt*","micro_TTbar30600Evt",false,false,false));

//   //// Single top
//   listOfMicroNtuples.push_back(MyStr("STopT42500Evt*","micro_STopT42500Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("STopS42500Evt*","micro_STopS42500Evt",false,false,false));

//   //// QCD
//   listOfMicroNtuples.push_back(MyStr("QCD_HT100to250NoIso92Evt*","micro_QCD_HT100to250NoIso92Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("QCD_HT250to500NoIso472Evt*","micro_QCD_HT250to500NoIso472Evt",false,false,false));


  //
  //// Non W,  has two directories 
  //MyStr nonWMyStr("nonWdata/tag/","micro_taggable_nonW",false,true);
  //nonWMyStr.AddDir("nonWdata/untagtaggable/");
  //listOfMicroNtuples.push_back(nonWMyStr);
  //listOfMicroNtuples.push_back(MyStr("nonWdata/tag/","micro_tag_nonW",false,true));
  //listOfMicroNtuples.push_back(MyStr("nonWdata/untagtaggable/","micro_untagtaggable_nonWdata",false,true));
  //
  //
  //// Data
  //listOfMicroNtuples.push_back(MyStr("data/tag/","micro_tagged_data",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("data/untagtaggable/","micro_untagtaggable_data",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("data_uptop17/tag/","micro_tagged_data_uptop17",false,false,false));
  
//   //MEs for Low and High Lumi, Data
  
//   MyStr dataMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/data/tag/","micro_tagged_data",false,false,false);
//   dataMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/data/tag/");
//   listOfMicroNtuples.push_back(dataMyStr);
  
//   //MEs for Low and High Lumi, Data
//   MyStr untagdataMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/data/untagtaggable/","micro_untagtaggable_data",false,false,true);
//   untagdataMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/data/untagtaggable/");
//   listOfMicroNtuples.push_back(untagdataMyStr);
  
  
//   //// Low and High Lumi, Non W,  has four directories 
  
//   MyStr nonWMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/tag/","micro_taggable_nonW",false,true,false);
//   nonWMyStr.AddDir("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   nonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/tag/");
//   nonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   listOfMicroNtuples.push_back(nonWMyStr);
  
//   MyStr untagnonWMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/","micro_untagtaggable_nonW",false,true,false);
//   untagnonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   listOfMicroNtuples.push_back(untagnonWMyStr);

  //Loop over all of them
  for ( vector<MyStr>::iterator it = listOfMicroNtuples.begin();
	it != listOfMicroNtuples.end(); it++){
    
    makeMicroNtuple(it->GetLocationVector(inputPath), outputPath+ it->name + ".root", nJet, it->mistag, it->nonw, it->untag);
    
  }//for


}//createAllMicroNtuples()

